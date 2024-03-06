#pragma once

class Line;

struct Point {
 public:
  Point(double, double);
  Point();

  void rotate(const Point&, double);
  void reflect(const Point&);
  void reflect(const Line&);
  void scale(const Point&, double);

  Line getPerpendicular(const Line&) const;

  bool operator==(const Point&) const;
  bool operator!=(const Point&) const;

  Point& operator+=(const Point&);
  Point& operator-=(const Point&);
  Point operator+(const Point&) const;
  Point operator-(const Point&) const;

  double crossProduct(const Point&) const;
  void normalize();

  double x;
  double y;
};

class Line {
 public:
  Line(const Point&, const Point&);
  Line(double, double);
  Line(const Point&, double);
  Line(double, double, double);

  bool isContainingPoint(const Point&) const;
  bool isIntersect(const Line&) const;
  Point getIntersectionPoint(const Line&) const;

  Point getVector() const;

  bool operator==(const Line&) const;
  bool operator!=(const Line&) const;

 private:
  double a_coeff;
  double b_coeff;
  double c_coeff;
};
