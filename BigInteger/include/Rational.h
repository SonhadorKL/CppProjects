#pragma once

#include "BigInteger.h"

class Rational {
  public:
    Rational();
    Rational(const BigInteger&);
    Rational(const BigInteger&, const BigInteger&);
    Rational(int);

    std::string toString() const;
    std::string asDecimal(size_t) const;
    explicit operator double() const;

    Rational operator-() const;
    Rational& operator+=(const Rational&);
    Rational& operator-=(const Rational&);
    Rational& operator*=(const Rational&);
    Rational& operator/=(const Rational&);

    friend bool operator<(Rational, Rational);

  private:
    void toCoPrime();

    static BigInteger gcd(BigInteger, BigInteger);
    static BigInteger lms(BigInteger, BigInteger);

    BigInteger numerator_;
    BigInteger denominator_;
};

Rational operator+(Rational, const Rational&);
Rational operator-(Rational, const Rational&);
Rational operator*(Rational, const Rational&);
Rational operator/(Rational, const Rational&);

bool operator<(Rational, Rational);
bool operator>(const Rational&, const Rational&);
bool operator==(const Rational&, const Rational&);
bool operator!=(const Rational&, const Rational&);
bool operator<=(const Rational&, const Rational&);
bool operator>=(const Rational&, const Rational&);
