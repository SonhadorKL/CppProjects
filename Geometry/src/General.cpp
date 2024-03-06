#include "General.h"

#include <cmath>
#include <cstdarg>
#include <algorithm>

double geometry::getDistance(const Point& point1, const Point& point2) {
  return std::sqrt(std::pow((point1.x - point2.x), 2) +
                   std::pow((point1.y - point2.y), 2));
}

bool geometry::isSmaller(double num1, double num2) {
  return num1 - num2 < geometry::kPrecision;
}
bool geometry::isBigger(double num1, double num2) {
  return num1 - num2 > -geometry::kPrecision;
}
bool geometry::isEqual(double num1, double num2) {
  return std::abs(num1 - num2) < geometry::kPrecision;
}
bool geometry::inSegment(const Point& test_point, const Point& segm_p1,
                         const Point& segm_p2) {
  return geometry::isBigger(test_point.x, std::min(segm_p1.x, segm_p2.x)) &&
         geometry::isSmaller(test_point.x, std::max(segm_p1.x, segm_p2.x)) &&
         geometry::isBigger(test_point.y, std::min(segm_p1.y, segm_p2.y)) &&
         geometry::isSmaller(test_point.y, std::max(segm_p1.y, segm_p2.y));
}

Point geometry::getSegmentCenter(const Point& point1, const Point& point2) {
  return Point((point1.x + point2.x) / 2, (point1.y + point2.y) / 2);
}

Point geometry::getSegmentPoint(const Point& p1, const Point& p2,
                                double ratio) {
  Point ans;
  ans.x = (p2.x - p1.x) * ratio + p1.x;
  ans.y = (p2.y - p1.y) * ratio + p1.y;
  return ans;
}
