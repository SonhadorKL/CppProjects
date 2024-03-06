#pragma once

#include "Shape.h"

#include <vector>
#include <stddef.h>

class Circle;

class Polygon : public Shape {
 public:
  Polygon(const std::vector<Point>&);
  
  template <class... Args>
  Polygon(const Point&, Args...);

  Polygon() = default;

  double perimeter() const override;
  double area() const override;
  bool operator==(const Shape&) const override;
  bool operator==(const Polygon&) const;
  bool operator!=(const Shape&) const;
  bool isCongruentTo(const Shape&) const override;
  bool isSimilarTo(const Shape&) const override;
  bool containsPoint(const Point&) const override;

  void rotate(const Point& center, double angle) override;
  void reflect(const Point& center) override;
  void reflect(const Line& axis) override;
  void scale(const Point& center, double coefficient) override;

  size_t verticesCount() const;
  std::vector<Point> getVertices() const;
  bool isConvex() const;

 protected:
  std::vector<Point> vertices_;
};

class Rectangle : public Polygon {
 public:
  Rectangle(const Point&, const Point&, double);
  double area() const override;
  double perimeter() const override;
  Point center() const;
  std::pair<Line, Line> diagonals() const;

 private:
  std::vector<Point> construct(const Point&, const Point&, double);
};

class Square : public Rectangle {
 public:
  Square(const Point&, const Point&);
  double area() const override;
  double perimeter() const override;
  Circle circumscribedCircle() const;
  Circle inscribedCircle() const;
};

class Triangle : public Polygon {
 public:
  Triangle(const Point&, const Point&, const Point&);

  Circle circumscribedCircle() const;
  Circle inscribedCircle() const;
  Point centroid() const;
  Point orthocenter() const;
  Line EulerLine() const;
  Circle ninePointsCircle() const;
};


template <class... Args>
Polygon::Polygon(const Point& p, Args... args) : Polygon(args...) {
  vertices_.push_back(p);
}