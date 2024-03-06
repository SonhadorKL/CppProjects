#pragma once

#include <cstring>

class String {
 public:
  String(const char*);
  String(size_t, char);
  String();
  String(const String&);
  String(char);

  ~String() { delete[] str; }

  size_t length() const { return size_ - 1; }
  size_t size() const { return size_ - 1; }
  size_t capacity() const { return capacity_ - 1; }

  char* data() { return str; }
  const char* data() const { return str; }

  void pop_back();
  void push_back(char);

  char& front() { return str[0]; }
  const char& front() const { return str[0]; }

  char& back() { return str[size_ - 2]; }
  const char& back() const { return str[size_ - 2]; }

  String& operator+=(const String&);
  String& operator=(const String&);

  char& operator[](size_t ind) { return str[ind]; }
  const char& operator[](size_t ind) const { return str[ind]; }

  bool empty() { return size_ <= 1; }

  void clear();

  void shrink_to_fit();

  String substr(size_t, size_t) const;

  size_t find(const char*) const;
  size_t find(const String& substr) const { return find(substr.data()); }

  size_t rfind(const char*) const;
  size_t rfind(const String& str1) const { return rfind(str1.data()); }

 private:
  void update(size_t, size_t, char*);

  size_t size_;
  size_t capacity_;
  char* str;
};