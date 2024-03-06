#pragma once

#include "Primitives.h"

namespace geometry {
  const double kPrecision = 1e-8;

  bool isEqual(double, double);
  bool isSmaller(double, double);
  bool isBigger(double, double);
  
  double getDistance(const Point&, const Point&);
  bool inSegment(const Point&, const Point&, const Point&);
  Point getSegmentCenter(const Point&, const Point&);
  Point getSegmentPoint(const Point&, const Point&, double);
} 