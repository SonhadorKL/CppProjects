#pragma once

#include "Shape.h"
#include <utility>

class Ellipse : public Shape {
 public:
  Ellipse(Point, Point, double);
  std::pair<Point, Point> focuses() const;
  std::pair<Line, Line> directrices() const;
  double eccentricity() const;
  Point center() const;

  double perimeter() const override;
  double area() const override;
  bool operator==(const Shape&) const override;
  bool operator==(const Ellipse&) const;
  bool isCongruentTo(const Shape&) const override;
  bool isSimilarTo(const Shape&) const override;
  bool containsPoint(const Point&) const override;

  void rotate(const Point& center, double angle) override;
  void reflect(const Point& center) override;
  void reflect(const Line& axis) override;
  void scale(const Point& center, double coefficient) override;

 protected:
  double getSemiMinorAxis() const;

  Point first_focus_;
  Point second_focus_;
  double semimajor_axis_;
};

class Circle : public Ellipse {
 public:
  Circle(const Point&, double);
  double perimeter() const override;
  double area() const override;
  double radius() const;
};
