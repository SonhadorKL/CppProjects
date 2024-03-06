#pragma once

#include "Primitives.h"

class Shape {
 public:
  virtual ~Shape() = default;

  virtual double perimeter() const = 0;
  virtual double area() const = 0;
  virtual bool operator==(const Shape&) const = 0;
  virtual bool isCongruentTo(const Shape&) const = 0;
  virtual bool isSimilarTo(const Shape&) const = 0;
  virtual bool containsPoint(const Point&) const = 0;

  virtual void rotate(const Point& center, double angle) = 0;
  virtual void reflect(const Point& center) = 0;
  virtual void reflect(const Line& axis) = 0;
  virtual void scale(const Point& center, double coefficient) = 0;
};