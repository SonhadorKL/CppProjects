#pragma once


#include <cmath>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "List.h"

template <typename Key, typename Value, typename Hash = std::hash<Key>,
          typename Equal = std::equal_to<Key>,
          typename Alloc = std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {
 public:
  
  using ValueType = std::pair<const Key, Value>;

  using iterator = typename List<ValueType>::iterator;
  using const_iterator = typename List<ValueType>::const_iterator;

  iterator begin() { return elements_.begin(); }
  const_iterator cbegin() const { return elements_.cbegin(); }
  iterator end() { return elements_.end(); }
  const_iterator cend() const { return elements_.cend(); }

  UnorderedMap();
  explicit UnorderedMap(size_t bucket_count);
  UnorderedMap(UnorderedMap&& another) noexcept;
  UnorderedMap(const UnorderedMap& another);

  UnorderedMap& operator=(const UnorderedMap& other);
  UnorderedMap& operator=(UnorderedMap&& other) noexcept;

  size_t size() const { return elements_.size(); }

  Value& at(const Key& key);

  const Value& at(const Key& key) const {
    // Const_cast is used to remove copypaste of method at
    return const_cast<UnorderedMap*>(this)->at(key);
  };

  Value& operator[](const Key& key);
  Value& operator[](Key&& key);

  iterator find(const Key& key);
  const_iterator find(const Key& key) const { return find(key); }

  std::pair<iterator, bool> insert(ValueType&& value);
  std::pair<iterator, bool> insert(const ValueType& value);

  template <class P>
  std::pair<iterator, bool> insert(P&& value);

  template <class InputIt>
  void insert(InputIt first, InputIt last);

  void erase(const_iterator first, const_iterator last);
  void erase(const_iterator it);

  template <typename... Args>
  std::pair<iterator, bool> emplace(Args&&... args);

  void swap(UnorderedMap& another);

  void rehash(size_t bucket_count);

  void reserve(size_t count);

  float load_factor() const {
    return static_cast<float>(elements_.size()) / bucket_count_;
  }
  size_t bucket_count() const { return bucket_count_; }

  void max_load_factor(float load_factor) { max_load_factor_ = load_factor; }
  float max_load_factor() const { return max_load_factor_; }

 private:
  constexpr static const size_t kDefaultBucket = 16;
  constexpr static const double kDefaultLoadFactor = 0.8;
  constexpr static const size_t kDefaultExpandFactor = 2;

  using ValueTypeAlloc =
      typename std::allocator_traits<Alloc>::template rebind_alloc<ValueType>;

  double max_load_factor_ = kDefaultLoadFactor;
  size_t bucket_count_ = 0;
  List<ValueType, ValueTypeAlloc> elements_;
  std::vector<iterator> buckets_;
};

template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap()
    : UnorderedMap(kDefaultBucket) {}

template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap(size_t bucket_count)
    : max_load_factor_(kDefaultLoadFactor),
      bucket_count_(0),
      buckets_(bucket_count, elements_.end()) {}

template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap(
    UnorderedMap&& another) noexcept
    : elements_(std::move(another.elements_)),
      buckets_(std::move(another.buckets_)) {
  bucket_count_ = another.bucket_count_;
  another.max_load_factor_ = another.kDefaultLoadFactor;

  another.bucket_count_ = 0;
  another.buckets_.resize(kDefaultBucket, another.elements_.end());
  another.buckets_.clear();
}

template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap(
    const UnorderedMap& another)
    : buckets_(another.buckets_.size(), elements_.end()),
      bucket_count_(0),
      max_load_factor_(another.max_load_factor_) {
  for (auto el = another.cbegin(); el != another.cend(); ++el) {
    insert(*el);
  }
}

template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
auto UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator=(
    const UnorderedMap& other) -> UnorderedMap& {
  UnorderedMap temp(other);
  swap(temp);
  return *this;
}

template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
auto UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator=(
    UnorderedMap&& other) noexcept -> UnorderedMap& {
  UnorderedMap temp(std::move(other));
  swap(temp);
  return *this;
}

template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
Value& UnorderedMap<Key, Value, Hash, Equal, Alloc>::at(const Key& key) {
  auto it_to_value = find(key);
  if (it_to_value == end()) {
    throw std::out_of_range("Ahahahaha you've been cocknut");
  }
  return it_to_value->second;
}

template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
Value& UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator[](
    const Key& key) {
  auto it_to_value = find(key);
  if (it_to_value == end()) {
    it_to_value = emplace(key, Value()).first;
  }
  return it_to_value->second;
}

template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
Value& UnorderedMap<Key, Value, Hash, Equal, Alloc>::operator[](Key&& key) {
  auto it_to_value = find(key);
  if (it_to_value == end()) {
    it_to_value = emplace(std::move(key), Value()).first;
  }
  return it_to_value->second;
}

template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
auto UnorderedMap<Key, Value, Hash, Equal, Alloc>::find(const Key& key)
    -> iterator {
  size_t start_hash = Hash()(key);
  size_t cur_index = start_hash % buckets_.size();
  for (auto it = buckets_.at(cur_index);
       it != elements_.end() &&
       Hash()(it->first) % buckets_.size() == start_hash % buckets_.size();
       ++it) {
    if (Equal()(it->first, key)) {
      return it;
    }
  }
  return elements_.end();
}

template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
auto UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert(
    const ValueType& value) -> std::pair<iterator, bool> {
  return emplace(value);
}

template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
template <typename P>
auto UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert(P&& value)
    -> std::pair<iterator, bool> {
  return emplace(std::forward<P>(value));
}

template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
auto UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert(ValueType&& value)
    -> std::pair<iterator, bool> {
  return emplace(std::move(value));
}

template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
template <class InputIt>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert(InputIt first,
                                                          InputIt last) {
  for (auto it = first; it != last; ++it) {
    emplace(*it);
  }
}

template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::erase(const_iterator first,
                                                         const_iterator last) {
  auto next_it = first;
  ++next_it;
  for (auto it = first; it != last;) {
    erase(it);
    it = next_it;
    ++next_it;
  }
}

template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::erase(const_iterator it) {
  size_t cur_hash = Hash()(it->first) % buckets_.size();
  auto next_it = buckets_[cur_hash];
  ++next_it;
  if (buckets_[cur_hash] != it) {
    // case when node is not at beginning
    elements_.erase(it);
  } else if (Hash()(next_it->first) % buckets_.size() == cur_hash) {
    // case when node at beginning and there are another elements at the bucket
    auto it_to_delete = buckets_[cur_hash];
    ++buckets_[cur_hash];
    elements_.erase(it_to_delete);
  } else {
    // case when node at beginning and there is no more elements at the bucket
    elements_.erase(buckets_[cur_hash]);
    buckets_[cur_hash] = elements_.end();
  }
}

template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
template <typename... Args>
auto UnorderedMap<Key, Value, Hash, Equal, Alloc>::emplace(Args&&... args)
    -> std::pair<iterator, bool> {
  List<ValueType, Alloc> temp;
  temp.emplace(temp.begin(), std::forward<Args>(args)...);
  const auto& node = *temp.begin();
  size_t node_hash = Hash()(node.first);
  iterator it_to_value = find(node.first);
  iterator it = it_to_value;

  if (it_to_value == elements_.end()) {
    if (buckets_[node_hash % buckets_.size()] == elements_.end()) {
      ++bucket_count_;
      elements_.splice(elements_.begin(), temp, temp.begin(), temp.end());
      buckets_[node_hash % buckets_.size()] = elements_.begin();
      it = elements_.begin();
      if (max_load_factor() * bucket_count() > size()) {
        rehash(buckets_.size() * kDefaultExpandFactor);
      }
    } else {
      it = elements_.splice(buckets_[node_hash % buckets_.size()], temp,
                            temp.begin(), temp.end());
      buckets_[node_hash % buckets_.size()] = it;
    }
    return {it, true};
  }
  return {it_to_value, false};
}

template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::swap(UnorderedMap& another) {
  buckets_.swap(another.buckets_);
  elements_.swap(another.elements_);
  std::swap(bucket_count_, another.bucket_count_);
  std::swap(max_load_factor_, another.max_load_factor_);
}

template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::rehash(size_t bucket_count) {
  bucket_count = std::max(
      size_t(std::ceil(static_cast<float>(size()) / max_load_factor())),
      bucket_count);
  UnorderedMap<Key, Value, Hash, Equal, Alloc> temp(bucket_count);
  for (auto it = elements_.begin(); it != elements_.end();) {
    iterator it_to_value = temp.buckets_[Hash()(it->first) % bucket_count];
    auto cur_it = it;
    ++cur_it;
    if (it_to_value == temp.elements_.end()) {
      ++temp.bucket_count_;
      temp.elements_.splice(temp.elements_.begin(), elements_, it, cur_it);
      temp.buckets_[Hash()(it->first) % bucket_count] = temp.elements_.begin();
    } else {
      temp.elements_.splice(it_to_value, elements_, it, cur_it);
      buckets_[Hash()(it->first) % buckets_.size()] = --it_to_value;
    }
    it = cur_it;
  }
  swap(temp);
}

template <typename Key, typename Value, typename Hash, typename Equal,
          typename Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::reserve(size_t count) {
  rehash(std::ceil(count / max_load_factor()));
}
