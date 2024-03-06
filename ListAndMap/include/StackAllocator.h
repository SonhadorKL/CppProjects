#pragma once


#include <memory>
#include <type_traits>

template <size_t N>
class StackStorage {
 public:
  StackStorage() = default;

  StackStorage<N> operator=(const StackStorage<N>&) = delete;

  void* GetAccess() { return reinterpret_cast<void*>(storage_ + taken_); }
  void Update(size_t to_add) { taken_ += to_add; }
  size_t GetLeft() { return N - taken_; }

 private:
  char storage_[N];
  size_t taken_ = 0;
};

template <typename T, size_t N>
class StackAllocator {
 public:
  StackAllocator() = default;
  explicit StackAllocator(StackStorage<N>& storage);

  template <typename U>
  explicit StackAllocator(const StackAllocator<U, N>& another);

  ~StackAllocator() = default;

  StackAllocator<T, N>& operator=(const StackAllocator<T, N>& another);

  bool operator==(const StackAllocator<T, N>& another) const;
  bool operator!=(const StackAllocator<T, N>& another) const;

  T* allocate(size_t size);
  void deallocate(T* ptr, size_t size = 0);

  StackStorage<N>* GetStorage() const { return memory_; }

  using value_type = T;
  using propagate_on_container_copy_assignment = std::true_type;

  template <class U>
  struct rebind {
    using other = StackAllocator<U, N>;
  };

  StackAllocator<T, N> select_on_container_copy_construction() { return *this; }

 private:
  StackStorage<N>* memory_ = nullptr;
};

template <typename T, size_t N>
StackAllocator<T, N>::StackAllocator(StackStorage<N>& storage)
    : memory_(&storage) {}

template <typename T, size_t N>
template <typename U>
StackAllocator<T, N>::StackAllocator(const StackAllocator<U, N>& another) {
  memory_ = another.GetStorage();
}

template <typename T, size_t N>
T* StackAllocator<T, N>::allocate(size_t size) {
  size_t type_size = sizeof(T);
  void* ptr_to_memory = memory_->GetAccess();
  size_t size_of_storage = memory_->GetLeft();
  if (std::align(alignof(T), type_size * size, ptr_to_memory,
                 size_of_storage)) {
    T* result = reinterpret_cast<T*>(ptr_to_memory);
    memory_->Update(type_size * size);
    return result;
  }
  return nullptr;
}

template <typename T, size_t N>
void StackAllocator<T, N>::deallocate(T*, size_t) {}

template <typename T, size_t N>
StackAllocator<T, N>& StackAllocator<T, N>::operator=(
    const StackAllocator<T, N>& another) {
  if (this == &another) {
    return *this;
  }
  memory_ = another.GetStorage();
  return *this;
}

template <typename T, size_t N>
bool StackAllocator<T, N>::operator==(
    const StackAllocator<T, N>& another) const {
  return memory_ == another.memory_;
}

template <typename T, size_t N>
bool StackAllocator<T, N>::operator!=(
    const StackAllocator<T, N>& another) const {
  return !(*this == another);
}
