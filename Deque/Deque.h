#pragma once


#include <stdexcept>
#include <vector>

template <typename T>
class Deque {
 public:
  template <bool IsConst>
  class CommonIterator {
   public:
    friend class Deque;

    using difference_type = int;
    using value_type = std::conditional_t<IsConst, const T, T>;
    using pointer = std::conditional_t<IsConst, const T*, T*>;
    using reference = std::conditional_t<IsConst, const T&, T&>;
    using iterator_category = std::random_access_iterator_tag;

    operator CommonIterator<true>() {
      return CommonIterator<true>(cur_array_, index_);
    }

    reference operator*() { return *((*cur_array_) + index_); }
    pointer operator->() { return (*cur_array_) + index_; }

    CommonIterator<IsConst>& operator++();
    CommonIterator<IsConst> operator++(int);

    CommonIterator<IsConst>& operator--();
    CommonIterator<IsConst> operator--(int);

    Deque<T>::CommonIterator<IsConst>& operator+=(difference_type index);
    Deque<T>::CommonIterator<IsConst>& operator-=(difference_type index);

    Deque<T>::CommonIterator<IsConst> operator+(difference_type index) const;

    Deque<T>::CommonIterator<IsConst> operator-(difference_type index) const;
    int operator-(const Deque::CommonIterator<IsConst>& another) const;

    auto operator<=>(const Deque<T>::CommonIterator<IsConst>& another) const {
      return (*this) - another;
    }

    bool operator==(const Deque<T>::CommonIterator<IsConst>& another) const;

   private:
    CommonIterator(pointer* array, int ind) : cur_array_(array), index_(ind) {}
    pointer* cur_array_;
    int index_;
  };

  using iterator = CommonIterator<false>;
  using const_iterator = CommonIterator<true>;

  Deque();
  Deque(const Deque<T>&);
  explicit Deque(size_t);
  Deque(size_t, const T&);

  size_t size() const;

  T& operator[](size_t);
  const T& operator[](size_t) const;

  T& at(size_t);
  const T& at(size_t) const;

  Deque<T>& operator=(const Deque<T>&);

  void push_front(const T&);
  void push_back(const T&);

  void pop_front();
  void pop_back();

  void erase(Deque<T>::iterator);
  void insert(Deque<T>::iterator, const T&);

  Deque<T>::iterator begin();
  Deque<T>::iterator end();
  Deque<T>::const_iterator begin() const;
  Deque<T>::const_iterator end() const;

  Deque<T>::const_iterator cbegin() const;
  Deque<T>::const_iterator cend() const;

  std::reverse_iterator<Deque<T>::iterator> rbegin();
  std::reverse_iterator<Deque<T>::iterator> rend();

  std::reverse_iterator<Deque<T>::const_iterator> rbegin() const;
  std::reverse_iterator<Deque<T>::const_iterator> rend() const;

  std::reverse_iterator<Deque<T>::const_iterator> rcbegin() const;
  std::reverse_iterator<Deque<T>::const_iterator> rcend() const;

  ~Deque();

 private:
  static const int kSubArraySize = 32;

  std::vector<T*> main_array_;
  size_t start_index_;
  size_t end_index_;
  size_t capacity_;

  // Basic constructor to use in other constructors
  explicit Deque(size_t start_index, size_t sz, size_t end_index);
  // Return a new T[kSubArraySize]
  T* get_sub_array();
  // Fill main array with sub arrays T[kSubArraySize]
  void fill();
  // Return an element in position from the start of main_array
  T* get_element_by_abs_index(size_t ind);
  // Delete all elements and set all deque variables to zero
  void clear();
  // Delete all elements
  void free_memory();
  // Make an empty deque with given sz and set start index to start_ind
  void init(size_t start_ind, size_t sz);
  // Just triple size: <old_mem> -> <new_mem><old_mem><new_mem>
  void triple_size();
};

template <typename T>
Deque<T>::Deque(size_t start_index, size_t sz, size_t end_index)
    : main_array_(sz / kSubArraySize + 1),
      start_index_(start_index),
      end_index_(end_index),
      capacity_((sz / kSubArraySize + 1) * kSubArraySize) {
  fill();
}


//////////////////////////// Implementation ////////////////////////////
template <typename T>
template <bool IsConst>
Deque<T>::CommonIterator<IsConst>&
Deque<T>::CommonIterator<IsConst>::operator++() {
  cur_array_ += (index_ + 1) / kSubArraySize;
  index_ = (index_ + 1) % kSubArraySize;
  return *this;
}

template <typename T>
template <bool IsConst>
Deque<T>::CommonIterator<IsConst> Deque<T>::CommonIterator<IsConst>::operator++(
    int) {
  auto to_return = *this;
  ++(*this);
  return to_return;
}

template <typename T>
template <bool IsConst>
Deque<T>::CommonIterator<IsConst>&
Deque<T>::CommonIterator<IsConst>::operator--() {
  cur_array_ = index_ == 0 ? cur_array_ - 1 : cur_array_;
  index_ = index_ == 0 ? kSubArraySize - 1 : index_ - 1;
  return *this;
}

template <typename T>
template <bool IsConst>
Deque<T>::CommonIterator<IsConst> Deque<T>::CommonIterator<IsConst>::operator--(
    int) {
  auto to_return = *this;
  --(*this);
  return to_return;
}

template <typename T>
template <bool IsConst>
Deque<T>::CommonIterator<IsConst>&
Deque<T>::CommonIterator<IsConst>::operator+=(difference_type index) {
  if (index_ + index >= 0) {
    cur_array_ += (index_ + index) / kSubArraySize;
    index_ = (index_ + index) % kSubArraySize;
  } else {
    cur_array_ += ((index_ + index) / kSubArraySize) - 1;
    index_ = kSubArraySize + ((index_ + index)) % kSubArraySize;
  }
  return *this;
}

template <typename T>
template <bool IsConst>
Deque<T>::CommonIterator<IsConst>&
Deque<T>::CommonIterator<IsConst>::operator-=(difference_type index) {
  return (*this) += (-index);
}

template <typename T>
template <bool IsConst>
Deque<T>::CommonIterator<IsConst> Deque<T>::CommonIterator<IsConst>::operator+(
    difference_type index) const {
  Deque<T>::CommonIterator<IsConst> res = *this;
  res += index;
  return res;
}

template <typename T>
template <bool IsConst>
Deque<T>::CommonIterator<IsConst> Deque<T>::CommonIterator<IsConst>::operator-(
    difference_type index) const {
  return (*this) + (-index);
}

template <typename T>
template <bool IsConst>
int Deque<T>::CommonIterator<IsConst>::operator-(
    const Deque::CommonIterator<IsConst>& another) const {
  return (cur_array_ - another.cur_array_) * kSubArraySize + index_ -
         another.index_;
}

template <typename T>
template <bool IsConst>
bool Deque<T>::CommonIterator<IsConst>::operator==(
    const Deque<T>::CommonIterator<IsConst>& another) const {
  return (*this) - another == 0;
}

template <typename T>
Deque<T>::Deque() : Deque(0, 0, 0) {}

template <typename T>
Deque<T>::Deque(size_t sz, const T& element) : Deque(0, sz, sz) {
  size_t i = 0;
  try {
    for (; i < sz; ++i) {
      new (main_array_[i / kSubArraySize] + (i % kSubArraySize)) T(element);
    }
  } catch (...) {
    for (size_t j = 0; j < i; ++j) {
      (main_array_[j / kSubArraySize] + (j % kSubArraySize))->~T();
    }
    clear();
    throw;
  }
}

template <typename T>
Deque<T>::Deque(size_t sz) : Deque(0, sz, sz) {
  size_t i = 0;
  try {
    for (; i < sz; ++i) {
      new (main_array_[i / kSubArraySize] + (i % kSubArraySize)) T();
    }
  } catch (...) {
    for (size_t j = 0; j < i; ++j) {
      get_element_by_abs_index(j)->~T();
    }
    clear();
    throw;
  }
}

template <typename T>
Deque<T>::Deque(const Deque<T>& another)
    : main_array_(another.capacity_ / kSubArraySize),
      start_index_(another.start_index_),
      end_index_(another.end_index_),
      capacity_(another.capacity_) {
  fill();
  size_t i = start_index_;
  try {
    for (; i < end_index_; ++i) {
      size_t ind_in_main = i / kSubArraySize;
      size_t ind_in_sub = i % kSubArraySize;
      new (main_array_[ind_in_main] + ind_in_sub)
          T(another.main_array_[ind_in_main][ind_in_sub]);
    }
  } catch (...) {
    for (size_t j = 0; j < i; ++j) {
      get_element_by_abs_index(j)->~T();
    }
    clear();
    throw;
  }
}

template <typename T>
size_t Deque<T>::size() const {
  return end_index_ - start_index_;
}

template <typename T>
T& Deque<T>::operator[](size_t index) {
  return main_array_[(start_index_ + index) / kSubArraySize]
                    [(start_index_ + index) % kSubArraySize];
}

template <typename T>
const T& Deque<T>::operator[](size_t index) const {
  return main_array_[(start_index_ + index) / kSubArraySize]
                    [(start_index_ + index) % kSubArraySize];
}

template <typename T>
T& Deque<T>::at(size_t index) {
  if (index + start_index_ >= end_index_) {
    throw std::out_of_range("out_of_range");
  }
  return (*this)[index];
}

template <typename T>
const T& Deque<T>::at(size_t index) const {
  return (*this).at(index);
}

template <typename T>
void Deque<T>::pop_front() {
  get_element_by_abs_index(start_index_++)->~T();
}

template <typename T>
void Deque<T>::pop_back() {
  get_element_by_abs_index(--end_index_)->~T();
}

template <typename T>
void Deque<T>::erase(Deque<T>::iterator to_delete) {
  for (auto it = to_delete; it != end() - 1; ++it) {
    std::swap(*it, *(it + 1));
  }
  (end() - 1)->~T();
  --end_index_;
}

template <typename T>
void Deque<T>::insert(Deque<T>::iterator to_insert, const T& element) {
  size_t index = to_insert - begin();

  push_back(element);

  auto it = end() - 1;
  while (it != begin() + index) {
    std::swap(*it, *(it - 1));
    --it;
  }
}

template <typename T>
Deque<T>& Deque<T>::operator=(const Deque<T>& another) {
  if (&another == this) {
    return *this;
  }
  Deque<T> tmp(another);
  std::swap(main_array_, tmp.main_array_);
  std::swap(start_index_, tmp.start_index_);
  std::swap(end_index_, tmp.end_index_);
  std::swap(capacity_, tmp.capacity_);
  return *this;
}

template <typename T>
void Deque<T>::push_front(const T& el) {
  if (start_index_ > 0) {
    T* to_add = get_element_by_abs_index(start_index_ - 1);
    new (to_add) T(el);
    --start_index_;
    return;
  }

  T* next_array = nullptr;
  try {
    next_array = get_sub_array();
    new (next_array + kSubArraySize - 1) T(el);
  } catch (...) {
    delete[] reinterpret_cast<char*>(next_array);
    throw;
  }
  triple_size();
  std::swap(main_array_[--start_index_ / kSubArraySize], next_array);
  delete[] reinterpret_cast<char*>(next_array);
}

template <typename T>
void Deque<T>::push_back(const T& el) {
  if (end_index_ < capacity_) {
    T* to_add = get_element_by_abs_index(end_index_);
    new (to_add) T(el);
    ++end_index_;
    return;
  }

  T* next_array = nullptr;
  try {
    next_array = get_sub_array();
    new (next_array) T(el);
  } catch (...) {
    delete[] reinterpret_cast<char*>(next_array);
    throw;
  }
  triple_size();
  std::swap(main_array_[end_index_ / kSubArraySize], next_array);

  delete[] reinterpret_cast<char*>(next_array);

  ++end_index_;
}

template <typename T>
typename Deque<T>::iterator Deque<T>::begin() {
  return Deque<T>::iterator(&main_array_[start_index_ / kSubArraySize],
                            start_index_ % kSubArraySize);
}

template <typename T>
typename Deque<T>::iterator Deque<T>::end() {
  auto it = Deque<T>::iterator(&main_array_[end_index_ / kSubArraySize],
                               end_index_ % kSubArraySize);
  return Deque<T>::iterator(&main_array_[end_index_ / kSubArraySize],
                            end_index_ % kSubArraySize);
}

template <typename T>
typename Deque<T>::const_iterator Deque<T>::begin() const {
  return cbegin();
}

template <typename T>
typename Deque<T>::const_iterator Deque<T>::end() const {
  return cend();
}

template <typename T>
typename Deque<T>::const_iterator Deque<T>::cbegin() const {
  return Deque<T>::const_iterator(
      const_cast<const T**>(&main_array_[start_index_ / kSubArraySize]),
      start_index_ % kSubArraySize);
}

template <typename T>
typename Deque<T>::const_iterator Deque<T>::cend() const {
  return Deque<T>::const_iterator(
      const_cast<const T**>(&main_array_[end_index_ / kSubArraySize]),
      end_index_ % kSubArraySize);
}

template <typename T>
std::reverse_iterator<typename Deque<T>::iterator> Deque<T>::rbegin() {
  return std::reverse_iterator<Deque<T>::iterator>(end());
}

template <typename T>
std::reverse_iterator<typename Deque<T>::iterator> Deque<T>::rend() {
  return std::reverse_iterator<Deque<T>::iterator>(begin());
}

template <typename T>
std::reverse_iterator<typename Deque<T>::const_iterator> Deque<T>::rbegin()
    const {
  return rcbegin();
}

template <typename T>
std::reverse_iterator<typename Deque<T>::const_iterator> Deque<T>::rend()
    const {
  return rcend();
}

template <typename T>
typename std::reverse_iterator<typename Deque<T>::const_iterator>
Deque<T>::rcbegin() const {
  return std::reverse_iterator<Deque<T>::const_iterator>(cend());
}

template <typename T>
typename std::reverse_iterator<typename Deque<T>::const_iterator>
Deque<T>::rcend() const {
  return std::reverse_iterator<Deque<T>::const_iterator>(cbegin());
}

template <typename T>
Deque<T>::~Deque() {
  clear();
}

template <typename T>
T* Deque<T>::get_element_by_abs_index(size_t ind) {
  return main_array_[ind / kSubArraySize] + ind % kSubArraySize;
}

template <typename T>
void Deque<T>::clear() {
  free_memory();
  start_index_ = 0;
  end_index_ = 0;
  capacity_ = 0;
  main_array_.clear();
}

template <typename T>
void Deque<T>::free_memory() {
  for (size_t i = 0; i < capacity_ / kSubArraySize; ++i) {
    delete[] reinterpret_cast<char*>(main_array_[i]);
  }
}

template <typename T>
void Deque<T>::triple_size() {
  std::vector<T*> new_array(capacity_ * 3 / kSubArraySize);
  for (size_t i = 0; i < main_array_.size(); ++i) {
    new_array[i + capacity_ / kSubArraySize] = main_array_[i];
  }

  for (size_t i = 0; i < new_array.size(); ++i) {
    if (new_array[i] == nullptr) {
      new_array[i] = get_sub_array();
    }
  }

  main_array_ = new_array;
  end_index_ = end_index_ + capacity_;
  start_index_ = start_index_ + capacity_;
  capacity_ *= 3;
}

template <typename T>
T* Deque<T>::get_sub_array() {
  return reinterpret_cast<T*>(new char[kSubArraySize * sizeof(T)]);
}

template <typename T>
void Deque<T>::init(size_t start_ind, size_t sz) {
  main_array_.resize(sz / kSubArraySize + 1);
  start_index_ = start_ind;
  end_index_ = sz;
  capacity_ = (sz / kSubArraySize + 1) * kSubArraySize;
  fill();
}

template <typename T>
void Deque<T>::fill() {
  for (size_t i = 0; i < main_array_.size(); ++i) {
    main_array_[i] = get_sub_array();
  }
}