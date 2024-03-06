#pragma once

#include <string>
#include <vector>

class BigInteger {
  public:
    BigInteger();
    BigInteger(int);
    BigInteger(const std::string&);
    explicit BigInteger(const char*);

    BigInteger operator-() const;

    BigInteger& operator+=(const BigInteger&);
    BigInteger& operator-=(const BigInteger&);
    BigInteger& operator*=(const BigInteger&);
    BigInteger& operator/=(const BigInteger&);
    BigInteger& operator%=(const BigInteger&);

    BigInteger& operator++();
    BigInteger operator++(int);
    BigInteger& operator--();
    BigInteger operator--(int);

    std::string toString() const;

    explicit operator bool() const;

    friend bool operator<(const BigInteger&, const BigInteger&);
    friend std::ostream& operator<<(std::ostream&, const BigInteger&);
    friend std::istream& operator>>(std::istream&, BigInteger&);

  private:
    static const long long kBIBase_ = 1'000'000'000;
    static const size_t kBIBaseSize_ = 9;

    std::vector<long long> digits_;
    bool is_negative_;

    void normalizeNum();
    void reverseNum();
    size_t len() const;
    BigInteger getSubNum(size_t, size_t) const;

    bool isZero() const;
    void absoluteSum(const BigInteger&);
    void absoluteSubtraction(const BigInteger&);
    int absoluteComparison(const BigInteger&) const;

    static void addLeadingZeroes(std::string&, long long, bool);
    static long long moduloBase(long long);
    static size_t getNumeralLen(long long);
};

BigInteger operator+(BigInteger, const BigInteger&);
BigInteger operator-(BigInteger, const BigInteger&);
BigInteger operator*(BigInteger, const BigInteger&);
BigInteger operator/(BigInteger, const BigInteger&);
BigInteger operator%(BigInteger, const BigInteger&);

BigInteger operator"" _bi(unsigned long long);
BigInteger operator"" _bi(const char*);

bool operator<(const BigInteger&, const BigInteger&);
bool operator>(const BigInteger&, const BigInteger&);
bool operator==(const BigInteger&, const BigInteger&);
bool operator!=(const BigInteger&, const BigInteger&);
bool operator>=(const BigInteger&, const BigInteger&);
bool operator<=(const BigInteger&, const BigInteger&);

std::ostream& operator<<(std::ostream&, const BigInteger&);
std::istream& operator>>(std::istream&, const BigInteger&);

void setSign(BigInteger&, bool);