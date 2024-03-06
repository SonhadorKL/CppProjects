#pragma once

#include "SharedPtr.h"
#include "WeakPtr.h"

template <typename T>
class EnableSharedFromThis {
 public:
  SharedPtr<T> shared_from_this() const noexcept { return wptr_.lock(); }

 private:
  WeakPtr<T> wptr_;
};
