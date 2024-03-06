#include "Polygons.h"
#include "General.h"
#include "Ellipse.h"
#include "Primitives.h"

#include <cmath>


//////////////////////////////// POLYGON ////////////////////////////////
Polygon::Polygon(const std::vector<Point>& points) : vertices_(points) {}

double Polygon::perimeter() const {
  double perimeter = 0;
  for (size_t i = 0; i < vertices_.size() - 1; ++i) {
    perimeter += geometry::getDistance(vertices_.at(i), vertices_.at(i + 1));
  }
  perimeter += geometry::getDistance(vertices_.at(vertices_.size() - 1),
                                     vertices_.at(0));
  return perimeter;
}

double Polygon::area() const {
  double area = 0;
  for (size_t i = 0; i < verticesCount(); ++i) {
    area += vertices_.at(i).x * vertices_.at((i + 1) % verticesCount()).y;
    area -= vertices_.at((i + 1) % verticesCount()).x * vertices_.at(i).y;
  }
  area = 0.5 * std::abs(area);
  return area;
}

bool Polygon::operator==(const Shape& shape) const {
  if (dynamic_cast<const Polygon*>(&shape) != nullptr) {
    const Polygon& polygon = dynamic_cast<const Polygon&>(shape);
    size_t vert_count = polygon.verticesCount();
    if (vert_count != verticesCount()) {
      return false;
    }

    int ind_of_second_polygon = -1;
    for (size_t i = 0; i < verticesCount(); ++i) {
      if (vertices_.at(0) == polygon.vertices_.at(i)) {
        ind_of_second_polygon = i;
        break;
      }
    }
    if (ind_of_second_polygon == -1) {
      return false;
    }
    bool is_equal_clockwised = true;
    bool is_equal_counterclockwised = true;
    for (size_t i = 1; i < verticesCount(); ++i) {
      is_equal_clockwised =
          is_equal_clockwised &&
          (vertices_.at(i) ==
           polygon.vertices_.at((ind_of_second_polygon + i) % verticesCount()));
      is_equal_counterclockwised =
          is_equal_counterclockwised &&
          (vertices_.at(i) ==
           polygon.vertices_.at((vert_count + ind_of_second_polygon - i) %
                                vert_count));
    }
    return is_equal_clockwised || is_equal_counterclockwised;
  }
  return false;
}
bool Polygon::operator==(const Polygon& polygon) const {
  return (*this == dynamic_cast<const Shape&>(polygon));
}

bool Polygon::operator!=(const Shape& another) const {
  return !(*this == another);
}

bool Polygon::isCongruentTo(const Shape& another) const {
  if (dynamic_cast<const Ellipse*>(&another) != nullptr) {
    return false;
  }
  const Polygon& other = dynamic_cast<const Polygon&>(another);
  if (other.verticesCount() != verticesCount()) {
    return false;
  }
  size_t vcount = verticesCount();
  std::vector<double> self_lengths;
  std::vector<double> other_lengths;
  for (size_t i = 0; i < vcount; ++i) {
    self_lengths.push_back(
        geometry::getDistance(vertices_.at(i), vertices_.at((i + 1) % vcount)));
    other_lengths.push_back(geometry::getDistance(
        other.vertices_.at(i), other.vertices_.at((i + 1) % vcount)));
  }
  bool equal_clockwised = true;
  bool equal_counterclockwised = true;
  bool congruent = false;
  for (size_t i = 0; i < vcount; ++i) {
    equal_clockwised = true;
    equal_counterclockwised = true;
    for (size_t j = 0; j < vcount; ++j) {
      equal_clockwised = equal_clockwised &&
                         geometry::isEqual(self_lengths.at(j),
                                           other_lengths.at((i + j) % vcount));
      equal_counterclockwised =
          equal_counterclockwised &&
          geometry::isEqual(self_lengths.at(j),
                            other_lengths.at((vcount + i - j) % vcount));
    }
    if ((equal_clockwised || equal_counterclockwised) &&
        (isConvex() == other.isConvex()) &&
        (geometry::isEqual(area(), other.area()))) {
      congruent = true;
      break;
    }
  }
  return congruent;
}

bool Polygon::isSimilarTo(const Shape& another) const {
  if (dynamic_cast<const Ellipse*>(&another) != nullptr) {
    return false;
  }
  const Polygon& other = dynamic_cast<const Polygon&>(another);
  if (other.verticesCount() != verticesCount()) {
    return false;
  }
  size_t vcount = verticesCount();
  std::vector<double> self_lengths;
  std::vector<double> other_lengths;
  for (size_t i = 0; i < vcount; ++i) {
    self_lengths.push_back(
        geometry::getDistance(vertices_.at(i), vertices_.at((i + 1) % vcount)));
    other_lengths.push_back(geometry::getDistance(
        other.vertices_.at(i), other.vertices_.at((i + 1) % vcount)));
  }
  bool equal_clockwised = true;
  bool equal_counterclockwised = true;
  double ratio;
  bool is_similar = false;
  for (size_t i = 0; i < vcount; ++i) {
    equal_counterclockwised = true;
    equal_clockwised = true;
    ratio = self_lengths.at(0) / other_lengths.at(i);
    for (size_t j = 0; j < vcount - 1; ++j) {
      equal_clockwised =
          equal_clockwised &&
          geometry::isEqual(ratio, self_lengths.at(j + 1) /
                                       other_lengths.at((i + j + 1) % vcount));
      equal_counterclockwised =
          equal_counterclockwised &&
          geometry::isEqual(
              ratio, self_lengths.at(j + 1) /
                         other_lengths.at((vcount + i - j - 1) % vcount));
      if (equal_clockwised ||
          (equal_counterclockwised &&
           geometry::isEqual(ratio * ratio, area() / other.area()))) {
        is_similar = true;
        break;
      }
    }
  }
  return is_similar;
}

bool Polygon::containsPoint(const Point& point) const {
  size_t intersection_count = 0;
  for (size_t i = 0; i < verticesCount(); ++i) {
    const Point& p1 = vertices_.at(i);
    const Point& p2 = vertices_.at((i + 1) % verticesCount());
    Line cur_side = Line(p1, p2);
    if (cur_side.isIntersect(Line(point, 1e-7))) {
      Point intersection = cur_side.getIntersectionPoint(Line(point, 1e-7));
      if (intersection.x >= point.x &&
          geometry::inSegment(intersection, p1, p2)) {
        intersection_count += 1;
      }
    }
  }
  return intersection_count % 2 == 1;
}

void Polygon::rotate(const Point& center, double angle) {
  for (Point& point : vertices_) {
    point.rotate(center, angle);
  }
}
void Polygon::reflect(const Point& center) {
  for (Point& point : vertices_) {
    point.reflect(center);
  }
}
void Polygon::reflect(const Line& axis) {
  for (Point& point : vertices_) {
    point.reflect(axis);
  }
}
void Polygon::scale(const Point& center, double coefficient) {
  for (Point& point : vertices_) {
    point.scale(center, coefficient);
  }
}

size_t Polygon::verticesCount() const { return vertices_.size(); }
std::vector<Point> Polygon::getVertices() const { return vertices_; }

bool Polygon::isConvex() const {
  double prev_product = 0;
  double cur_product = 0;
  size_t vert_size = vertices_.size();
  for (size_t i = 0; i < vertices_.size(); ++i) {
    cur_product = (vertices_.at((i + 1) % vert_size) - vertices_.at(i))
                      .crossProduct((vertices_.at((i + 2) % vert_size) -
                                     vertices_.at(i)));
    if (cur_product * prev_product < 0) {
      return false;
    }
    prev_product = cur_product;
  }
  return true;
}

//////////////////////////////// RECTANGLE ////////////////////////////////

Rectangle::Rectangle(const Point& point1, const Point& point2, double ratio)
    : Polygon(construct(point1, point2, ratio)) {}

Point Rectangle::center() const {
  return Line(vertices_.at(0), vertices_.at(2))
      .getIntersectionPoint(Line(vertices_.at(1), vertices_.at(3)));
}

std::pair<Line, Line> Rectangle::diagonals() const {
  Line diagonal1 = Line(vertices_.at(0), vertices_.at(2));
  Line diagonal2 = Line(vertices_.at(1), vertices_.at(3));
  return {diagonal1, diagonal2};
}

double Rectangle::area() const {
  return geometry::getDistance(vertices_.at(0), vertices_.at(1)) *
         geometry::getDistance(vertices_.at(1), vertices_.at(2));
}
double Rectangle::perimeter() const {
  return 2 * (geometry::getDistance(vertices_.at(0), vertices_.at(1)) +
              geometry::getDistance(vertices_.at(1), vertices_.at(2)));
}

std::vector<Point> Rectangle::construct(const Point& point1,
                                        const Point& point2, double ratio) {
  double angle_small_diag = geometry::isBigger(ratio, 1)
                                ? std::atan(ratio)
                                : M_PI_2 - std::atan(ratio);
  Point point1_copy = point1;
  Point point2_copy = point2;
  point2_copy.rotate(point1, angle_small_diag);
  point1_copy.rotate(point2, -(M_PI_2 - angle_small_diag));

  Point p2 =
      Line(point1, point2_copy).getIntersectionPoint(Line(point2, point1_copy));
  Point p4 = p2;
  p4.reflect(geometry::getSegmentCenter(point1, point2));
  return {point1, p2, point2, p4};
}

//////////////////////////////// SQUARE ////////////////////////////////

Square::Square(const Point& p1, const Point& p2) : Rectangle(p1, p2, 1) {}

Circle Square::circumscribedCircle() const {
  return Circle(center(), geometry::getDistance(center(), vertices_.at(0)));
}

Circle Square::inscribedCircle() const {
  return Circle(center(),
                geometry::getDistance(vertices_.at(0), vertices_.at(1)) / 2);
}

double Square::area() const {
  return std::pow(geometry::getDistance(vertices_.at(0), vertices_.at(1)), 2);
}

double Square::perimeter() const {
  return 4.0 * geometry::getDistance(vertices_.at(0), vertices_.at(1));
}

//////////////////////////////// TRIANGLE ////////////////////////////////

Triangle::Triangle(const Point& p1, const Point& p2, const Point& p3)
    : Polygon(p1, p2, p3) {}

Circle Triangle::circumscribedCircle() const {
  Point midd1 = geometry::getSegmentCenter(vertices_.at(0), vertices_.at(1));
  Point midd2 = geometry::getSegmentCenter(vertices_.at(1), vertices_.at(2));
  Line perp1 = midd1.getPerpendicular(Line(vertices_.at(0), vertices_.at(1)));
  Line perp2 = midd2.getPerpendicular(Line(vertices_.at(1), vertices_.at(2)));
  Point center = perp1.getIntersectionPoint(perp2);
  return Circle(center, geometry::getDistance(center, vertices_.at(0)));
}

Circle Triangle::inscribedCircle() const {
  Point A, B, C;
  A = vertices_.at(0);
  B = vertices_.at(1);
  C = vertices_.at(2);

  double ratio1 = geometry::getDistance(A, C) / geometry::getDistance(A, B);
  double ratio2 = geometry::getDistance(B, C) / geometry::getDistance(B, A);
  Point bis1 = geometry::getSegmentPoint(B, C, 1.0 / (ratio1 + 1));
  Point bis2 = geometry::getSegmentPoint(A, C, 1.0 / (ratio2 + 1));
  Point circ_center = Line(bis1, A).getIntersectionPoint(Line(bis2, B));
  Point radius_point =
      circ_center.getPerpendicular(Line(A, B)).getIntersectionPoint(Line(A, B));

  return Circle(circ_center, geometry::getDistance(radius_point, circ_center));
}

Point Triangle::centroid() const {
  Point midd1 = geometry::getSegmentCenter(vertices_.at(0), vertices_.at(1));
  Point midd2 = geometry::getSegmentCenter(vertices_.at(1), vertices_.at(2));
  return Line(midd1, vertices_.at(2))
      .getIntersectionPoint(Line(midd2, vertices_.at(0)));
}

Point Triangle::orthocenter() const {
  Line perp1 =
      vertices_.at(0).getPerpendicular(Line(vertices_.at(1), vertices_.at(2)));
  Line perp2 =
      vertices_.at(2).getPerpendicular(Line(vertices_.at(0), vertices_.at(1)));
  return perp1.getIntersectionPoint(perp2);
}

Line Triangle::EulerLine() const { return Line(centroid(), orthocenter()); }

Circle Triangle::ninePointsCircle() const {
  Point mid1 = geometry::getSegmentCenter(vertices_.at(1), vertices_.at(2));
  Point mid2 = geometry::getSegmentCenter(vertices_.at(2), vertices_.at(0));
  Point mid3 = geometry::getSegmentCenter(vertices_.at(0), vertices_.at(1));
  return Triangle(mid1, mid2, mid3).circumscribedCircle();
}