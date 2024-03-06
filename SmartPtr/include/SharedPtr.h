#pragma once

#include <cstddef>
#include <memory>
#include <type_traits>

template <class T>
class WeakPtr;

template <class T>
class EnableSharedFromThis;

template <typename T>
class SharedPtr {
  template <class Y>
  friend class SharedPtr;

  template <class Y, class Alloc, class... Args>
  friend SharedPtr<Y> allocateShared(const Alloc& allocator, Args&&... args);

  template <class Y>
  friend class WeakPtr;

 private:
  class BaseControlBlock;

 public:
  SharedPtr();

  template <class Y, class Deleter = std::default_delete<Y>,
            class Alloc = std::allocator<Y>,
            std::enable_if_t<std::is_base_of<T, Y>::value ||
                                 std::is_same<T, Y>::value,
                             bool> = true>
  explicit SharedPtr(Y* ptr, const Deleter& d = Deleter(),
                     const Alloc& allocator = Alloc());

  SharedPtr(const SharedPtr& other);

  template <class Y, std::enable_if_t<std::is_base_of<T, Y>::value ||
                                          std::is_same<T, Y>::value,
                                      bool> = true>
  SharedPtr(const SharedPtr<Y>& other);

  SharedPtr(SharedPtr&& other) noexcept;

  template <class Y, std::enable_if_t<std::is_base_of<T, Y>::value ||
                                          std::is_same<T, Y>::value,
                                      bool> = true>
  SharedPtr(SharedPtr<Y>&& other);

  SharedPtr& operator=(const SharedPtr<T>& other);

  SharedPtr& operator=(SharedPtr<T>&& other) noexcept;

  size_t use_count() const;

  T* get() const noexcept;
  T& operator*() const noexcept { return *get(); }
  T* operator->() const noexcept { return get(); }

  template <class Y>
  void reset(Y* ptr);

  void reset();

  void swap(SharedPtr& other);

  ~SharedPtr();

 private:
  /* Base Control Block */
  struct BaseControlBlock {

    BaseControlBlock() : shared_count(1), weak_count(0) {}

    virtual void* get_pointer() = 0;
    virtual void use_deleter() = 0;
    virtual void delete_block() = 0;
    virtual ~BaseControlBlock() = default;
    
    size_t shared_count;
    size_t weak_count;
  };

  /* Control Block Regular */
  template <typename Y, typename Deleter, typename Alloc>
  struct ControlBlockRegular : public BaseControlBlock {
    ControlBlockRegular(Y* object, const Deleter& deleter,
                        const Alloc& allocator);

    void* get_pointer() override;
    void use_deleter() override;
    void delete_block() override;

    ~ControlBlockRegular() = default;

    using block_alloc = typename std::allocator_traits<
        Alloc>::template rebind_alloc<ControlBlockRegular>;
    using allocator_traits = std::allocator_traits<block_alloc>;

    Y* object_;
    Deleter deleter_;
    Alloc allocator_;
  };

  /* Control Block from Make Shared */
  template <typename Alloc>
  struct ControlBlockMakeShared : public BaseControlBlock {
    template <typename... Args>
    explicit ControlBlockMakeShared(const Alloc& alloc, Args&&... args);

    void* get_pointer() override;

    void use_deleter() override;

    void delete_block() override;

    ~ControlBlockMakeShared() = default;

    using block_alloc = typename std::allocator_traits<
        Alloc>::template rebind_alloc<ControlBlockMakeShared>;
    using allocator_traits = std::allocator_traits<block_alloc>;

    using object_alloc =
        typename std::allocator_traits<Alloc>::template rebind_alloc<T>;
    using object_allocator_traits = std::allocator_traits<object_alloc>;

    char object_[sizeof(T)];
    Alloc allocator_;
  };
  /*=====================================*/

  template <typename Alloc>
  explicit SharedPtr(ControlBlockMakeShared<Alloc>* block);

  explicit SharedPtr(BaseControlBlock* block);

  using base_block_ptr = typename SharedPtr<T>::BaseControlBlock*;

  BaseControlBlock* control_block_;
};

template <typename T>
SharedPtr<T>::SharedPtr() : control_block_(nullptr) {}

template <typename T>
template <class Y, class Deleter, class Alloc,
          std::enable_if_t<
              std::is_base_of<T, Y>::value || std::is_same<T, Y>::value, bool>>
SharedPtr<T>::SharedPtr(Y* ptr, const Deleter& d, const Alloc& allocator) {
  using regular_block = ControlBlockRegular<Y, Deleter, Alloc>;
  using regular_block_alloc = typename regular_block::block_alloc;
  using regular_alloc_traits = typename regular_block::allocator_traits;

  regular_block_alloc block_allocator(allocator);
  regular_block* regular_cb =
      regular_alloc_traits::allocate(block_allocator, 1);
  new (regular_cb) regular_block(ptr, d, block_allocator);
  control_block_ = regular_cb;

  if constexpr (std::is_base_of<EnableSharedFromThis<Y>, Y>::value) {
    ptr->wptr_ = *this;
  }
}

template <typename T>
SharedPtr<T>::SharedPtr(const SharedPtr& other)
    : control_block_(other.control_block_) {
  if (control_block_) {
    ++(control_block_->shared_count);
  }
}

template <typename T>
template <class Y,
          std::enable_if_t<
              std::is_base_of<T, Y>::value || std::is_same<T, Y>::value, bool>>
SharedPtr<T>::SharedPtr(const SharedPtr<Y>& other)
    : control_block_(reinterpret_cast<base_block_ptr>(other.control_block_)) {
  if (control_block_) {
    ++(control_block_->shared_count);
  }
}

template <typename T>
SharedPtr<T>::SharedPtr(SharedPtr&& other) noexcept
    : control_block_(other.control_block_) {
  other.control_block_ = nullptr;
}

template <typename T>
template <class Y,
          std::enable_if_t<
              std::is_base_of<T, Y>::value || std::is_same<T, Y>::value, bool>>
SharedPtr<T>::SharedPtr(SharedPtr<Y>&& other)
    : control_block_(reinterpret_cast<base_block_ptr>(other.control_block_)) {
  other.control_block_ = nullptr;
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr<T>& other) {
  SharedPtr<T>(other).swap(*this);
  return *this;
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<T>&& other) noexcept {
  SharedPtr<T>(std::move(other)).swap(*this);
  return *this;
}

template <typename T>
size_t SharedPtr<T>::use_count() const {
  if (control_block_) {
    return control_block_->shared_count;
  }
  return 0;
}

template <typename T>
T* SharedPtr<T>::get() const noexcept {
  if (control_block_) {
    return reinterpret_cast<T*>(control_block_->get_pointer());
  }
  return nullptr;
}

template <typename T>
template <class Y>
void SharedPtr<T>::reset(Y* ptr) {
  SharedPtr<T>(ptr).swap(*this);
}

template <typename T>
void SharedPtr<T>::reset() {
  SharedPtr<T>().swap(*this);
}

template <typename T>
void SharedPtr<T>::swap(SharedPtr& other) {
  std::swap(control_block_, other.control_block_);
}

template <typename T>
SharedPtr<T>::~SharedPtr() {
  if (control_block_ == nullptr) {
    return;
  }

  --(control_block_->shared_count);
  if (control_block_->shared_count == 0) {
    control_block_->use_deleter();
    if (control_block_->weak_count == 0) {
      control_block_->delete_block();
      control_block_ = nullptr;
    }
  }
}

template <typename T>
template <typename Y, typename Deleter, typename Alloc>
SharedPtr<T>::ControlBlockRegular<Y, Deleter, Alloc>::ControlBlockRegular(
    Y* object, const Deleter& deleter, const Alloc& allocator)
    : object_(object), deleter_(deleter), allocator_(allocator) {}

template <typename T>
template <typename Y, typename Deleter, typename Alloc>
void* SharedPtr<T>::ControlBlockRegular<Y, Deleter, Alloc>::get_pointer() {
  return reinterpret_cast<void*>(object_);
}

template <typename T>
template <typename Y, typename Deleter, typename Alloc>
void SharedPtr<T>::ControlBlockRegular<Y, Deleter, Alloc>::use_deleter() {
  deleter_(object_);
  object_ = nullptr;
}

template <typename T>
template <typename Y, typename Deleter, typename Alloc>
void SharedPtr<T>::ControlBlockRegular<Y, Deleter, Alloc>::delete_block() {
  block_alloc temp_alloc = allocator_;
  this->~ControlBlockRegular();
  allocator_traits::deallocate(temp_alloc, this, 1);
}

template <typename T>
template <typename Alloc>
template <typename... Args>
SharedPtr<T>::ControlBlockMakeShared<Alloc>::ControlBlockMakeShared(
    const Alloc& alloc, Args&&... args)
    : allocator_(alloc) {
  new (reinterpret_cast<T*>(object_)) T(std::forward<Args>(args)...);
  if constexpr (std::is_base_of<EnableSharedFromThis<T>, T>::value) {
    reinterpret_cast<T*>(object_)->wptr_ = SharedPtr<T>(this);
  }
}

template <typename T>
template <typename Alloc>
void* SharedPtr<T>::ControlBlockMakeShared<Alloc>::get_pointer() {
  return reinterpret_cast<void*>(object_);
}

template <typename T>
template <typename Alloc>
void SharedPtr<T>::ControlBlockMakeShared<Alloc>::use_deleter() {
  object_alloc alloc(allocator_);
  object_allocator_traits::destroy(alloc, reinterpret_cast<T*>(object_));
}

template <typename T>
template <typename Alloc>
void SharedPtr<T>::ControlBlockMakeShared<Alloc>::delete_block() {
  block_alloc temp_alloc = allocator_;
  this->~ControlBlockMakeShared();
  allocator_traits::deallocate(temp_alloc, this, 1);
}

template <typename T>
template <typename Alloc>
SharedPtr<T>::SharedPtr(ControlBlockMakeShared<Alloc>* block)
    : control_block_(block) {}

template <typename T>
SharedPtr<T>::SharedPtr(BaseControlBlock* block) : control_block_(block) {
  if (control_block_) {
    control_block_->shared_count++;
  }
}

/////////////////////////////////////////////////////////////////////////////

template <class T, class Alloc, class... Args>
SharedPtr<T> allocateShared(const Alloc& allocator, Args&&... args) {
  using make_shared_block =
      typename SharedPtr<T>::template ControlBlockMakeShared<Alloc>;
  using make_shared_block_alloc = typename make_shared_block::block_alloc;
  using make_shared_block_traits = typename make_shared_block::allocator_traits;

  make_shared_block_alloc block_alloc(allocator);
  make_shared_block* block = make_shared_block_traits::allocate(block_alloc, 1);
  make_shared_block_traits::construct(block_alloc, block, allocator,
                                      std::forward<Args>(args)...);

  return SharedPtr<T>(block);
}

template <class T, class... Args>
SharedPtr<T> makeShared(Args&&... args) {
  return allocateShared<T>(std::allocator<T>(), std::forward<Args>(args)...);
}