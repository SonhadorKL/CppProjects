#include "String.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>

String::String(const char *old_str)
    : size_{strlen(old_str) + 1},
      capacity_{strlen(old_str) + 1},
      str{new char[strlen(old_str) + 1]} {
  memcpy(str, old_str, size_);
}

String::String(size_t symbol_count, char symbol)
    : size_{symbol_count + 1},
      capacity_{symbol_count + 1},
      str{new char[symbol_count + 1]} {
  memset(str, symbol, symbol_count);
  str[symbol_count] = '\0';
}

String::String() : size_{1}, capacity_{1}, str{new char[capacity_]} {
  str[0] = '\0';
}

String::String(const String &new_str)
    : size_{new_str.size_},
      capacity_{new_str.capacity_},
      str{new char[new_str.capacity_]} {
  memcpy(str, new_str.str, size_);
}

String::String(char symbol) : size_{2}, capacity_{2}, str{new char[capacity_]} {
  str[0] = symbol;
  str[1] = '\0';
}

String &String::operator=(const String &str1) {
  size_t str1_size = str1.length() + 1;
  if (str1_size <= capacity_) {
    memcpy(str, str1.data(), str1_size);
    size_ = str1_size;
  } else {
    update(str1_size, str1_size, new char[str1_size]);
    memcpy(str, str1.data(), size_);
  }
  return *this;
}

String &String::operator+=(const String &str1) {
  size_t str1_size = strlen(str1.data());
  if (str1_size + size_ <= capacity_) {
    memcpy(str + size_ - 1, str1.data(), str1_size + 1);
    size_ += str1_size;
  } else {
    char *new_str = new char[str1_size + size_];
    memcpy(new_str, str, size_);
    memcpy(new_str + size_ - 1, str1.data(), str1_size + 1);
    update(size_ + str1_size, size_ + str1_size, new_str);
  }
  return *this;
}

void String::pop_back() {
  --size_;
  str[size_ - 1] = '\0';
}

void String::push_back(char symbol) {
  if (size_ < capacity_) {
    str[size_ - 1] = symbol;
    str[size_] = '\0';
    ++size_;
  } else {
    char *new_str = new char[capacity_ * 2];
    memcpy(new_str, str, capacity_);
    new_str[size_ - 1] = symbol;
    new_str[size_] = '\0';
    update(size_ + 1, capacity_ * 2, new_str);
  }
}

void String::clear() {
  size_ = 1;
  str[0] = '\0';
}

void String::shrink_to_fit() {
  if (size_ == capacity_) {
    return;
  }
  char *new_str = new char[size_];
  memcpy(new_str, str, size_);
  update(size_, size_, new_str);
}

String String::substr(size_t start, size_t count) const {
  count = std::min(count, size_ - start - 1);
  String subs{count, '\0'};
  memcpy(subs.data(), str + start, count);
  return subs;
}

size_t String::find(const char *substr) const {
  size_t cur_ind = 0;
  size_t substr_ind = 0;
  size_t substr_size = strlen(substr);
  while (cur_ind < size_) {
    if (str[cur_ind] == substr[substr_ind]) {
      ++substr_ind;
    } else {
      cur_ind -= substr_ind;
      substr_ind = 0;
    }
    if (substr_ind == substr_size) {
      return cur_ind - substr_size + 1;
    }
    ++cur_ind;
  }
  return size_ - 1;
}

size_t String::rfind(const char *substr) const {
  size_t cur_ind = size_ - 1;
  size_t substr_ind = 0;
  size_t substr_size = strlen(substr);
  while (cur_ind >= 1) {
    if (substr[substr_ind] == str[cur_ind - 1] && cur_ind < size_) {
      ++substr_ind;
      ++cur_ind;
    } else {
      cur_ind -= substr_ind + 1;
      substr_ind = 0;
    }

    if (substr_ind == substr_size) {
      return cur_ind - substr_ind - 1;
    }
  }
  return size_ - 1;
}

void String::update(size_t new_size, size_t new_capacity, char *new_str) {
  delete[] str;
  size_ = new_size;
  capacity_ = new_capacity;
  str = new_str;
}

bool operator==(const String &str1, const String &str2) {
  return strcmp(str1.data(), str2.data()) == 0;
}

bool operator!=(const String &str1, const String &str2) {
  return !(str1 == str2);
}

bool operator<(const String &str1, const String &str2) {
  return strcmp(str1.data(), str2.data()) == -1;
}

bool operator>(const String &str1, const String &str2) { return str2 < str1; }

bool operator>=(const String &str1, const String &str2) {
  return !(str1 < str2);
}

bool operator<=(const String &str1, const String &str2) {
  return !(str1 > str2);
}

String operator+(String str1, const String &str2) {
  str1 += str2;
  return str1;
}

std::ostream &operator<<(std::ostream &out, const String &str) {
  out << str.data();
  return out;
}

std::istream &operator>>(std::istream &in, String &str) {
  str.clear();
  char cur_symbol;
  in.get(cur_symbol);
  while (!iscntrl(cur_symbol) && !isspace(cur_symbol) && !in.eof()) {
    str.push_back(cur_symbol);
    in.get(cur_symbol);
  }
  return in;
}