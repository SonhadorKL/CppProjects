#pragma once

#include "SharedPtr.h"

template <typename T>
class WeakPtr {
  template <class Y>
  friend class WeakPtr;

 private:
  typename SharedPtr<T>::BaseControlBlock* control_block_;

 public:
  WeakPtr();

  template <class Y, std::enable_if_t<std::is_base_of<T, Y>::value ||
                                          std::is_same<T, Y>::value,
                                      bool> = true>
  WeakPtr(const SharedPtr<Y>& ptr);

  template <class Y, std::enable_if_t<std::is_base_of<T, Y>::value ||
                                          std::is_same<T, Y>::value,
                                      bool> = true>
  WeakPtr(const WeakPtr<Y>& other);

  template <class Y, std::enable_if_t<std::is_base_of<T, Y>::value ||
                                          std::is_same<T, Y>::value,
                                      bool> = true>
  WeakPtr(WeakPtr<Y>&& other);

  template <class Y, std::enable_if_t<std::is_base_of<T, Y>::value ||
                                          std::is_same<T, Y>::value,
                                      bool> = true>
  WeakPtr& operator=(const WeakPtr<Y>& other);

  template <class Y, std::enable_if_t<std::is_base_of<T, Y>::value ||
                                          std::is_same<T, Y>::value,
                                      bool> = true>
  WeakPtr& operator=(const SharedPtr<Y>& other);

  template <class Y, std::enable_if_t<std::is_base_of<T, Y>::value ||
                                          std::is_same<T, Y>::value,
                                      bool> = true>
  WeakPtr& operator=(WeakPtr<Y>&& other);

  bool expired() const noexcept;

  int use_count() const noexcept;

  SharedPtr<T> lock() const noexcept;

  void swap(WeakPtr& other);

  ~WeakPtr();
};

template <typename T>
WeakPtr<T>::WeakPtr() : control_block_(nullptr) {}

template <typename T>
template <class Y,
          std::enable_if_t<
              std::is_base_of<T, Y>::value || std::is_same<T, Y>::value, bool>>
WeakPtr<T>::WeakPtr(const SharedPtr<Y>& ptr)
    : control_block_(reinterpret_cast<typename SharedPtr<T>::BaseControlBlock*>(
          ptr.control_block_)) {
  if (control_block_) {
    ++(control_block_->weak_count);
  }
}

template <typename T>
template <class Y,
          std::enable_if_t<
              std::is_base_of<T, Y>::value || std::is_same<T, Y>::value, bool>>
WeakPtr<T>::WeakPtr(const WeakPtr<Y>& other)
    : control_block_(reinterpret_cast<typename SharedPtr<T>::BaseControlBlock*>(
          other.control_block_)) {
  if (control_block_) {
    ++(control_block_->weak_count);
  }
}

template <typename T>
template <class Y,
          std::enable_if_t<
              std::is_base_of<T, Y>::value || std::is_same<T, Y>::value, bool>>
WeakPtr<T>::WeakPtr(WeakPtr<Y>&& other) : control_block_(other.control_block_) {
  if (control_block_) {
    ++control_block_->weak_count;
  }
  other.control_block_ = nullptr;
}

template <typename T>
template <class Y,
          std::enable_if_t<
              std::is_base_of<T, Y>::value || std::is_same<T, Y>::value, bool>>
WeakPtr<T>& WeakPtr<T>::operator=(const SharedPtr<Y>& other) {
  WeakPtr(other).swap(*this);
  return *this;
}

template <typename T>
template <class Y,
          std::enable_if_t<
              std::is_base_of<T, Y>::value || std::is_same<T, Y>::value, bool>>
WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr<Y>& other) {
  WeakPtr(other).swap(*this);
  return *this;
}

template <typename T>
template <class Y,
          std::enable_if_t<
              std::is_base_of<T, Y>::value || std::is_same<T, Y>::value, bool>>
WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr<Y>&& other) {
  WeakPtr(std::move(other)).swap(*this);
  return *this;
}

template <typename T>
bool WeakPtr<T>::expired() const noexcept {
  return control_block_->shared_count == 0;
}

template <typename T>
SharedPtr<T> WeakPtr<T>::lock() const noexcept {
  return expired() ? SharedPtr<T>() : SharedPtr<T>(control_block_);
}

template <typename T>
int WeakPtr<T>::use_count() const noexcept {
  if (control_block_) {
    return control_block_->shared_count;
  }
  return 0;
}

template <typename T>
void WeakPtr<T>::swap(WeakPtr& other) {
  std::swap(control_block_, other.control_block_);
}

template <typename T>
WeakPtr<T>::~WeakPtr() {
  if (control_block_ == nullptr) {
    return;
  }
  --(control_block_->weak_count);
  if (control_block_->shared_count == 0 && control_block_->weak_count == 0) {
    control_block_->delete_block();
    control_block_ = nullptr;
  }
}