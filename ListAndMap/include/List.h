#pragma once


#include <memory>
#include <type_traits>

template <typename T, typename Allocator = std::allocator<T>>
class List {
 private:
  struct BaseNode;
  struct Node;

 public:
  template <bool IsConst>
  class CommonIterator {
   public:
    using difference_type = int;
    using value_type = std::conditional_t<IsConst, const T, T>;
    using pointer = std::conditional_t<IsConst, const T*, T*>;
    using reference = std::conditional_t<IsConst, const T&, T&>;
    using iterator_category = std::bidirectional_iterator_tag;

    operator CommonIterator<true>() const {
      return CommonIterator<true>(cur_node_);
    }

    reference operator*() { return static_cast<Node*>(cur_node_)->value_; }
    pointer operator->() { return &static_cast<Node*>(cur_node_)->value_; }

    CommonIterator<IsConst>& operator++() {
      cur_node_ = cur_node_->next_;
      return *this;
    }

    CommonIterator<IsConst> operator++(int) {
      auto to_return = *this;
      ++(*this);
      return to_return;
    }

    CommonIterator<IsConst>& operator--() {
      cur_node_ = cur_node_->prev_;
      return *this;
    }

    CommonIterator<IsConst> operator--(int) {
      auto to_return = *this;
      --(*this);
      return to_return;
    }

    bool operator==(const CommonIterator<true>& another) const {
      return cur_node_ == another.cur_node_;
    }

    bool operator!=(const CommonIterator<true>& another) const {
      return !(cur_node_ == another.cur_node_);
    }

   private:
    explicit CommonIterator(BaseNode* node) : cur_node_(node) {}
    friend class List;

    BaseNode* cur_node_;
  };

  using iterator = CommonIterator<false>;
  using const_iterator = CommonIterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  List();
  explicit List(size_t size, const Allocator& alloc = Allocator());
  explicit List(const Allocator& alloc);
  List(size_t size, const T& value, const Allocator& alloc = Allocator());
  List(const List& another);
  List(List&& another) noexcept;

  size_t size() const { return size_; }
  Allocator get_allocator() const { return node_alloc_; }

  iterator begin() { return iterator(base_node_->next_); };
  iterator end() { return iterator(base_node_); }
  const_iterator begin() const { return cbegin(); }
  const_iterator end() const { return cend(); }

  const_iterator cbegin() const {
    return const_iterator(const_cast<BaseNode*>(base_node_->next_));
  }

  const_iterator cend() const {
    return const_iterator(const_cast<BaseNode*>(base_node_));
  }

  reverse_iterator rbegin() { return std::reverse_iterator<iterator>(end()); }
  reverse_iterator rend() { return reverse_iterator(begin()); }

  const_reverse_iterator rbegin() const { return rcbegin(); }

  const_reverse_iterator rend() const { return rcend(); }

  const_reverse_iterator rcbegin() const {
    return const_reverse_iterator(cend());
  };

  const_reverse_iterator rcend() const {
    return const_reverse_iterator(cbegin());
  }

  List<T, Allocator>& operator=(const List<T, Allocator>& another);

  iterator insert(const_iterator it, const T& value);
  iterator insert(const_iterator it, T&& value) {
    return emplace(it, std::move(value));
  }
  void push_back(const T& value) { insert(end(), value); }
  void push_back(T&& value) { insert(end(), std::move(value)); }
  void push_front(const T& value) { insert(begin(), value); }
  void push_front(T&& value) { insert(begin(), std::move(value)); }

  void erase(const_iterator it);
  void pop_back() { erase(--end()); }
  void pop_front() { erase(begin()); }

  void swap(List<T, Allocator>& another) { swap(*this, another); }
  iterator splice(const_iterator pos, List& other, const_iterator first,
                  const_iterator last);

  template <class... Args>
  iterator emplace(const_iterator pos, Args&&... args);

  ~List();

 private:
  struct BaseNode {
    BaseNode* next_;
    BaseNode* prev_;

    BaseNode() : next_(this), prev_(this) {}
  };

  struct Node : public BaseNode {
    T value_;
    Node() = default;
    explicit Node(const T& value) : value_(value) {}
    explicit Node(T&& value) : value_(std::move(value)) {}

    template <typename... Args>
    Node(Args&&... args) : value_(std::forward<Args>(args)...) {}
  };

  void create_objects(size_t size, const T* value);
  void clear();
  void swap(List<T, Allocator>& first, List<T, Allocator>& second);
  void create_base_node();

  using NodeAlloc =
      typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
  using allocator_traits = std::allocator_traits<NodeAlloc>;

  using BaseNodeAlloc = typename std::allocator_traits<
      Allocator>::template rebind_alloc<BaseNode>;
  using allocator_traits_base = std::allocator_traits<BaseNodeAlloc>;

  BaseNode* base_node_;
  size_t size_ = 0;
  [[no_unique_address]] NodeAlloc node_alloc_;
};

template <typename T, typename Allocator>
void List<T, Allocator>::create_objects(size_t size, const T* value) {
  try {
    BaseNode* cur_node = base_node_;
    for (size_t ind = 0; ind < size; ++ind) {
      Node* vert = allocator_traits::allocate(node_alloc_, 1);
      try {
        if (value == nullptr) {
          allocator_traits::construct(node_alloc_, vert);
        } else {
          allocator_traits::construct(node_alloc_, vert, *value);
        }
      } catch (...) {
        allocator_traits::deallocate(node_alloc_, static_cast<Node*>(vert), 0);
        throw;
      }
      cur_node->next_ = vert;
      vert->prev_ = cur_node;
      vert->next_ = base_node_;
      base_node_->prev_ = vert;

      cur_node = vert;
    }
    size_ = size;
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Allocator>
void List<T, Allocator>::clear() {
  BaseNode* cur_node = base_node_->next_;
  while (cur_node != base_node_) {
    BaseNode* temp = cur_node->next_;
    allocator_traits::destroy(node_alloc_, static_cast<Node*>(cur_node));
    allocator_traits::deallocate(node_alloc_, static_cast<Node*>(cur_node), 0);
    cur_node = temp;
  }
  auto base_allocator = BaseNodeAlloc(node_alloc_);
  allocator_traits_base::deallocate(base_allocator, base_node_, 0);
}

template <typename T, typename Allocator>
void List<T, Allocator>::swap(List<T, Allocator>& first,
                              List<T, Allocator>& second) {
  std::swap(first.node_alloc_, second.node_alloc_);
  std::swap(first.base_node_, second.base_node_);
  std::swap(first.size_, second.size_);
}

template <typename T, typename Allocator>
List<T, Allocator>::List() {
  create_base_node();
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t size, const Allocator& alloc)
    : node_alloc_(
          allocator_traits::select_on_container_copy_construction(alloc)) {
  create_base_node();
  create_objects(size, nullptr);
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t size, const T& value, const Allocator& alloc)
    : node_alloc_(
          allocator_traits::select_on_container_copy_construction(alloc)) {
  create_base_node();
  create_objects(size, &value);
}

template <typename T, typename Allocator>
List<T, Allocator>::List(const Allocator& alloc) : node_alloc_(alloc) {
  create_base_node();
}

template <typename T, typename Allocator>
List<T, Allocator>::List(const List& another) {
  create_base_node();
  node_alloc_ = allocator_traits::select_on_container_copy_construction(
      another.node_alloc_);
  try {
    for (const auto& el : another) {
      this->push_back(el);
    }
    size_ = another.size();
  } catch (...) {
    this->clear();
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(List&& another) noexcept
    : node_alloc_(another.get_allocator()) {
  base_node_ = another.base_node_;
  size_ = another.size_;
  another.size_ = 0;
  another.create_base_node();
}

template <typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(
    const List<T, Allocator>& another) {
  if (&another == this) {
    return *this;
  }

  List<T, Allocator> temp(another);
  swap(*this, temp);
  if (allocator_traits::propagate_on_container_copy_assignment::value) {
    node_alloc_ = another.node_alloc_;
  }
  return *this;
}

template <typename T, typename Allocator>
auto List<T, Allocator>::insert(const_iterator it, const T& value) -> iterator {
  return emplace(it, value);
}

template <typename T, typename Allocator>
void List<T, Allocator>::erase(const_iterator it) {
  Node* cur_node = static_cast<Node*>(it.cur_node_);
  it.cur_node_->prev_->next_ = it.cur_node_->next_;
  it.cur_node_->next_->prev_ = it.cur_node_->prev_;
  allocator_traits::destroy(node_alloc_, cur_node);
  allocator_traits::deallocate(node_alloc_, cur_node, 0);
  size_--;
}

template <typename T, typename Allocator>
List<T, Allocator>::~List() {
  clear();
}

template <typename T, typename Allocator>
template <class... Args>
auto List<T, Allocator>::emplace(const_iterator pos, Args&&... args)
    -> iterator {
  Node* node = allocator_traits::allocate(node_alloc_, 1);
  try {
    allocator_traits::construct(node_alloc_, node, std::forward<Args>(args)...);
  } catch (...) {
    allocator_traits::deallocate(node_alloc_, node, 0);
    throw;
  }
  node->prev_ = pos.cur_node_->prev_;
  node->next_ = pos.cur_node_;
  node->prev_->next_ = node;
  node->next_->prev_ = node;
  size_++;
  return iterator(node);
}

template <typename T, typename Allocator>
void List<T, Allocator>::create_base_node() {
  auto base_allocator = BaseNodeAlloc(node_alloc_);
  base_node_ = allocator_traits_base::allocate(base_allocator, 1);
  allocator_traits_base::construct(base_allocator, base_node_);
}

template <typename T, typename Allocator>
auto List<T, Allocator>::splice(const_iterator pos, List& other,
                                const_iterator first, const_iterator last)
    -> iterator {
  auto fcopy = first;
  auto ecopy = last;
  size_t changed = std::distance(fcopy, ecopy);
  auto end_node = pos.cur_node_;
  auto node = (--pos).cur_node_;

  auto last_node = (--last).cur_node_;
  auto prev_node = (--first).cur_node_;
  ++last;
  ++first;

  node->next_ = first.cur_node_;
  first.cur_node_->prev_ = node;
  end_node->prev_ = last_node;
  last_node->next_ = end_node;

  prev_node->next_ = last.cur_node_;
  last.cur_node_->prev_ = prev_node;
  size_ += changed;
  other.size_ -= changed;
  return iterator(first.cur_node_);
}