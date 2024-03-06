#include "Ellipse.h"
#include "Polygons.h"
#include "General.h"

#include <cmath>

/////////////////////////////////// ELLIPSE ///////////////////////////////////
Ellipse::Ellipse(Point f1, Point f2, double sum)
    : first_focus_(f1), second_focus_(f2) {
  semimajor_axis_ = 0.5 * (sum - geometry::getDistance(f1, f2)) +
                    0.5 * geometry::getDistance(f1, f2);
}

std::pair<Point, Point> Ellipse::focuses() const {
  return std::make_pair(first_focus_, second_focus_);
}

std::pair<Line, Line> Ellipse::directrices() const {
  Point middle = center();
  Line axis = Line(first_focus_, second_focus_);
  Point direction = axis.getVector();
  direction.normalize();
  direction.x *= semimajor_axis_ / eccentricity();
  direction.y *= semimajor_axis_ / eccentricity();
  Line dir1 = (middle + direction).getPerpendicular(axis);
  Line dir2 = (middle - direction).getPerpendicular(axis);

  return std::make_pair(dir1, dir2);
}

double Ellipse::eccentricity() const {
  return 0.5 * geometry::getDistance(first_focus_, second_focus_) /
         semimajor_axis_;
}

Point Ellipse::center() const {
  return geometry::getSegmentCenter(first_focus_, second_focus_);
}

double Ellipse::getSemiMinorAxis() const {
  return semimajor_axis_ * std::sqrt(1 - std::pow(eccentricity(), 2));
}

double Ellipse::perimeter() const {
  double a = semimajor_axis_;
  double b = getSemiMinorAxis();
  return M_PI * (3 * (a + b) - std::sqrt((3 * a + b) * (a + 3 * b)));
}

double Ellipse::area() const {
  return M_PI * semimajor_axis_ * getSemiMinorAxis();
}

bool Ellipse::operator==(const Shape& another) const {
  if (dynamic_cast<const Polygon*>(&another) != nullptr) {
    return false;
  }
  const Ellipse& other = dynamic_cast<const Ellipse&>(another);
  bool testAxis = geometry::isEqual(other.semimajor_axis_, semimajor_axis_);
  bool testFocuses = (other.first_focus_ == first_focus_ &&
                      other.second_focus_ == second_focus_) ||
                     (other.second_focus_ == first_focus_ &&
                      other.first_focus_ == second_focus_);
  return testAxis && testFocuses;
}

bool Ellipse::operator==(const Ellipse& ellipse) const {
  return (*this == dynamic_cast<const Shape&>(ellipse));
}

bool Ellipse::isCongruentTo(const Shape& another) const {
  if (dynamic_cast<const Polygon*>(&another) != nullptr) {
    return false;
  }
  const Ellipse& other = dynamic_cast<const Ellipse&>(another);
  return geometry::isEqual(semimajor_axis_, other.semimajor_axis_) &&
         geometry::isEqual(getSemiMinorAxis(), other.getSemiMinorAxis());
}

bool Ellipse::isSimilarTo(const Shape& another) const {
  if (dynamic_cast<const Polygon*>(&another) != nullptr) {
    return false;
  }
  const Ellipse& other = dynamic_cast<const Ellipse&>(another);
  return geometry::isEqual(other.semimajor_axis_ / semimajor_axis_,
                           other.getSemiMinorAxis() / getSemiMinorAxis());
}

bool Ellipse::containsPoint(const Point& point) const {
  return geometry::isSmaller(
      (std::pow(point.x - center().x, 2) / std::pow(semimajor_axis_, 2) +
       std::pow(point.y - center().y, 2) / std::pow(getSemiMinorAxis(), 2)), 1);
}

void Ellipse::rotate(const Point& center, double angle) {
  first_focus_.rotate(center, angle);
  second_focus_.rotate(center, angle);
}

void Ellipse::reflect(const Point& center) {
  first_focus_.reflect(center);
  second_focus_.reflect(center);
}

void Ellipse::reflect(const Line& axis) {
  first_focus_.reflect(axis);
  second_focus_.reflect(axis);
}

void Ellipse::scale(const Point& center, double coefficient) {
  first_focus_.scale(center, coefficient);
  second_focus_.scale(center, coefficient);
  semimajor_axis_ *= coefficient;
}

/////////////////////////////////// CIRCLE ///////////////////////////////////

Circle::Circle(const Point& center, double radius)
    : Ellipse(center, center, 2 * radius) {}
double Circle::perimeter() const { return 2 * M_PI * radius(); }

double Circle::area() const { return M_PI * radius() * radius(); }

double Circle::radius() const { return semimajor_axis_; }