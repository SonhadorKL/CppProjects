#include "Rational.h"

Rational::Rational() : numerator_(0), denominator_(1) {};
Rational::Rational(int num) : numerator_(num), denominator_(1) {};
Rational::Rational(const BigInteger& num) : numerator_(num), denominator_(1) {};
Rational::Rational(const BigInteger& numerator, const BigInteger& denominator) {
    numerator_ = numerator;
    denominator_ = denominator;
    setSign(numerator_, (numerator_ < 0) == (denominator_ < 0));
    setSign(denominator_, true);
    toCoPrime();
}

std::string Rational::toString() const {
    std::string str_num;
    str_num += numerator_.toString();
    if (denominator_ != 1) {
        str_num += '/';
        str_num += denominator_.toString();
    }
    return str_num;
}

std::string Rational::asDecimal(size_t precision) const {
    std::string decimal;
    if (numerator_ < 0) {
        decimal += "-";
    }
    decimal += (numerator_ / denominator_).toString();
    if (precision > 0) {
        BigInteger numerator_copy(numerator_ % denominator_);
        setSign(numerator_copy, true);
        decimal += ".";
        for (size_t i = 0; i < precision; ++i) {
            numerator_copy *= 10;
            decimal += (numerator_copy / denominator_).toString();
            numerator_copy %= denominator_;
        }
    }
    return decimal;
}

Rational::operator double() const { return std::stod(asDecimal(15)); }

Rational Rational::operator-() const {
    Rational num = *this;
    num.numerator_ = -num.numerator_;
    return num;
}

Rational& Rational::operator+=(const Rational& num) {
    numerator_ = numerator_ * num.denominator_ + num.numerator_ * denominator_;
    denominator_ *= num.denominator_;
    toCoPrime();
    return *this;
}

Rational& Rational::operator-=(const Rational& num) { return *this += -num; }

Rational& Rational::operator*=(const Rational& num) {
    numerator_ *= num.numerator_;
    denominator_ *= num.denominator_;
    toCoPrime();
    return *this;
}

Rational& Rational::operator/=(const Rational& num) {
    numerator_ *= num.denominator_;
    denominator_ *= num.numerator_;
    if (num.numerator_ < 0) {
        denominator_ *= -1;
        numerator_ *= -1;
    }
    toCoPrime();
    return *this;
}

Rational operator+(Rational num1, const Rational& num2) { return num1 += num2; }

Rational operator-(Rational num1, const Rational& num2) { return num1 -= num2; }

Rational operator*(Rational num1, const Rational& num2) { return num1 *= num2; }

Rational operator/(Rational num1, const Rational& num2) { return num1 /= num2; }

bool operator<(Rational num1, Rational num2) {
    BigInteger lms = Rational::lms(num1.denominator_, num2.denominator_);
    num1.numerator_ *= lms / num1.denominator_;
    num2.numerator_ *= lms / num2.denominator_;
    return num1.numerator_ < num2.numerator_;
}

bool operator>(const Rational& num1, const Rational& num2) {
    return num2 < num1;
}

bool operator==(const Rational& num1, const Rational& num2) {
    return !(num1 < num2) && !(num1 > num2);
}

bool operator!=(const Rational& num1, const Rational& num2) {
    return !(num1 == num2);
}

bool operator<=(const Rational& num1, const Rational& num2) {
    return !(num1 > num2);
}

bool operator>=(const Rational& num1, const Rational& num2) {
    return !(num1 < num2);
}

BigInteger Rational::gcd(BigInteger num1, BigInteger num2) {
    setSign(num1, true);
    setSign(num2, true);
    while (num2 != 0) {
        num1 %= num2;
        std::swap(num1, num2);
    }
    return num1;
}

BigInteger Rational::lms(BigInteger num1, BigInteger num2) {
    setSign(num1, true);
    setSign(num2, true);
    return num1 * num2 / Rational::gcd(num1, num2);
}

void Rational::toCoPrime() {
    BigInteger gcd = Rational::gcd(numerator_, denominator_);
    numerator_ /= gcd;
    denominator_ /= gcd;
}