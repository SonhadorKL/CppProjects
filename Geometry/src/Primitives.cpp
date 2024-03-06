#include "Primitives.h"
#include "General.h"

#include <cmath>

Point::Point(double x, double y) : x{x}, y{y} {}
Point::Point() : x{0}, y{0} {}

void Point::rotate(const Point& center, double angle) {
  Point vec = *this - center;
  x = center.x + std::cos(angle) * vec.x - std::sin(angle) * vec.y;
  y = center.y + std::sin(angle) * vec.x + std::cos(angle) * vec.y;
}

void Point::reflect(const Point& center) {
  x += (center.x - x) * 2;
  y += (center.y - y) * 2;
}

void Point::reflect(const Line& axis) {
  Line perpendicular = getPerpendicular(axis);
  reflect(perpendicular.getIntersectionPoint(axis));
}

void Point::scale(const Point& center, double coefficient) {
  x = center.x + (x - center.x) * coefficient;
  y = center.y + (y - center.y) * coefficient;
}

Line Point::getPerpendicular(const Line& ax) const {
  Point normal = ax.getVector();
  double a_coeff = normal.x;
  double b_coeff = normal.y;
  return Line(a_coeff, b_coeff, -x * a_coeff - y * b_coeff);
}

bool Point::operator==(const Point& point) const {
  return geometry::isEqual(x, point.x) && geometry::isEqual(y, point.y);
}

bool Point::operator!=(const Point& point) const { return !(*this == point); }

Point& Point::operator+=(const Point& point) {
  x += point.x;
  y += point.y;
  return *this;
}

Point& Point::operator-=(const Point& point) {
  x -= point.x;
  y -= point.y;
  return *this;
}

Point Point::operator+(const Point& point) const {
  Point result(*this);
  return result += point;
}

Point Point::operator-(const Point& point) const {
  Point result(*this);
  return result -= point;
}

double Point::crossProduct(const Point& point) const {
  return x * point.y - point.x * y;
}

void Point::normalize() {
  double length = std::sqrt(x * x + y * y);
  x /= length;
  y /= length;
}

//////////////////////////// LINE ////////////////////////////

Line::Line(const Point& point1, const Point& point2) {
  double x1, x2, y1, y2;
  x1 = point1.x;
  x2 = point2.x;
  y1 = point1.y;
  y2 = point2.y;

  a_coeff = y1 - y2;
  b_coeff = x2 - x1;
  c_coeff = x1 * y2 - x2 * y1;
}

Line::Line(double slope, double shift) : Line(Point(shift, 0), slope) {}

Line::Line(const Point& point, double slope)
    : Line(point, Point(point.x + 1, (point.y + slope))) {}

Line::Line(double a, double b, double c) : a_coeff(a), b_coeff(b), c_coeff(c) {}

bool Line::isContainingPoint(const Point& point) const {
  return geometry::isEqual(point.x * a_coeff + point.y * b_coeff + c_coeff, 0);
}

bool Line::isIntersect(const Line& line) const {
  if (geometry::isEqual(line.a_coeff, 0)) {
    return !geometry::isEqual(line.c_coeff / line.b_coeff, c_coeff / b_coeff);
  }
  if (geometry::isEqual(line.b_coeff, 0)) {
    return !geometry::isEqual(line.c_coeff / line.a_coeff, c_coeff / a_coeff);
  }
  return !geometry::isEqual(line.getVector().crossProduct(getVector()), 0) &&
         line != *this;
}

Point Line::getIntersectionPoint(const Line& another_line) const {
  double a1, a2, b1, b2, c1, c2;
  a1 = a_coeff;
  a2 = another_line.a_coeff;
  b1 = b_coeff;
  b2 = another_line.b_coeff;
  c1 = c_coeff;
  c2 = another_line.c_coeff;

  Point intersection;
  intersection.x = -(b2 * c1 - b1 * c2) / (a1 * b2 - a2 * b1);
  intersection.y = -(a1 * c2 - a2 * c1) / (a1 * b2 - a2 * b1);

  return intersection;
}

Point Line::getVector() const {
  if (a_coeff == 0) {
    return Point(1, 0);
  }
  if (b_coeff == 0) {
    return Point(0, 1);
  }
  return Point(1, -(a_coeff + c_coeff) / b_coeff + c_coeff / b_coeff);
}

bool Line::operator==(const Line& line) const {
  Point p1, p2;
  if (a_coeff != 0 && b_coeff != 0) {
    p1 = Point(0, -c_coeff / b_coeff);
    p2 = Point(-c_coeff / a_coeff, 0);
  } else if (b_coeff != 0) {
    p1 = Point(0, -c_coeff / b_coeff);
    p2 = Point(1, -c_coeff / b_coeff);
  } else {
    p1 = Point(-c_coeff / a_coeff, 0);
    p2 = Point(-c_coeff / a_coeff, 1);
  }
  return line.isContainingPoint(p1) && line.isContainingPoint(p2);
}

bool Line::operator!=(const Line& line) const { return !(*this == line); }
