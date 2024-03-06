#include <cstring>
#include <algorithm>
#include <iostream>

#include "BigInteger.h"

BigInteger::BigInteger() : digits_ {0}, is_negative_(false) {}

BigInteger::BigInteger(int num) {
    is_negative_ = (num < 0);
    num = std::abs(num);
    do {
        digits_.push_back(num % kBIBase_);
        num /= kBIBase_;
    } while (num > 0);
    normalizeNum();
}

BigInteger::BigInteger(const char* num) : is_negative_(false) {
    if (num[0] == '-') {
        is_negative_ = true;
    }
    int cur_pos = strlen(num) - kBIBaseSize_;
    int prev_pos = strlen(num) - 1;
    int min_index = is_negative_ ? 1 : 0;
    std::string subStr;
    subStr.resize(kBIBaseSize_);
    for (; cur_pos >= min_index; cur_pos -= kBIBaseSize_) {
        strncpy(&subStr[0], num + cur_pos, prev_pos - cur_pos + 1);
        digits_.push_back(std::stoll(subStr));
        prev_pos = cur_pos - 1;
    }
    if (prev_pos != min_index - 1) {
        subStr.resize(prev_pos - min_index + 1);
        strncpy(&subStr[0], num + min_index, subStr.size());
        digits_.push_back(std::stoll(subStr));
    }
    normalizeNum();
}

BigInteger::BigInteger(const std::string& num) : BigInteger(num.data()) {}

BigInteger BigInteger::operator-() const {
    BigInteger opposite_num = *this;
    opposite_num.is_negative_ = !opposite_num.is_negative_;
    opposite_num.normalizeNum();
    return opposite_num;
}

BigInteger& BigInteger::operator+=(const BigInteger& num) {
    if (num.is_negative_ == is_negative_) {
        absoluteSum(num);
    } else {
        absoluteSubtraction(num);
    }
    return *this;
}

BigInteger& BigInteger::operator-=(const BigInteger& num) {
    if (num.is_negative_ == is_negative_) {
        absoluteSubtraction(num);
    } else {
        absoluteSum(num);
    }
    return *this;
}

BigInteger& BigInteger::operator*=(const BigInteger& num) {
    if (num == BigInteger(-1) && *this != BigInteger(0)) {
        is_negative_ = !is_negative_;
        return *this;
    }
    BigInteger result;
    const BigInteger* bigger_num = &num;
    const BigInteger* smaller_num = this;
    if (num.len() < len()) {
        std::swap(bigger_num, smaller_num);
    }
    size_t minimum_digit = smaller_num->len();
    size_t maximum_digit = bigger_num->len();
    for (size_t i = 0; i < minimum_digit; ++i) {
        BigInteger presum;
        presum.digits_.resize(maximum_digit + i, 0);
        long long next_digit = 0;
        for (size_t j = 0; j < maximum_digit; ++j) {
            long long cur_digit =
                smaller_num->digits_[i] * bigger_num->digits_[j] + next_digit;
            presum.digits_[j + i] = cur_digit % kBIBase_;
            next_digit = cur_digit / kBIBase_;
        }
        presum.digits_.push_back(next_digit);
        result += presum;
    }
    result.is_negative_ = smaller_num->is_negative_ != bigger_num->is_negative_;
    result.normalizeNum();
    *this = result;
    return *this;
}

BigInteger& BigInteger::operator/=(const BigInteger& num) {
    BigInteger divider = num;
    BigInteger result;
    result.is_negative_ = is_negative_ != num.is_negative_;
    result.digits_.resize(0);

    is_negative_ = false;
    divider.is_negative_ = false;

    if (*this < divider) {
        return *this = 0;
    }

    BigInteger subNum = getSubNum(len() - divider.len(), len() - 1);
    long long last_digit = divider.len() + 1;

    for (size_t i = last_digit; i <= len() + 1; ++i) {
        long long left_edge = 0;
        long long right_edge = kBIBase_;
        while (right_edge - left_edge > 1) {
            long long medium = (left_edge + right_edge) / 2;
            if (divider * medium > subNum) {
                right_edge = medium;
            } else {
                left_edge = medium;
            }
        }
        result.digits_.push_back(left_edge);
        if (i <= len()) {
            subNum -= (divider * left_edge);
            subNum *= kBIBase_;
            subNum += digits_.at(len() - i);
        }
    }
    *this = result;
    reverseNum();
    normalizeNum();
    return *this;
}

BigInteger& BigInteger::operator%=(const BigInteger& num) {
    *this -= num * (*this / num);
    normalizeNum();
    return *this;
}

BigInteger& BigInteger::operator++() {
    is_negative_ ? absoluteSubtraction(1) : absoluteSum(1);
    return *this;
}

BigInteger& BigInteger::operator--() {
    is_negative_ ? absoluteSum(1) : absoluteSubtraction(1);
    return *this;
}

BigInteger BigInteger::operator++(int) {
    BigInteger num = *this;
    ++(*this);
    return num;
}

BigInteger BigInteger::operator--(int) {
    BigInteger num = *this;
    --(*this);
    return num;
}

void BigInteger::normalizeNum() {
    if (digits_.size() == 1 && digits_.at(0) == 0) {
        is_negative_ = false;
    }
    long long cur_ind = digits_.size() - 1;
    while (digits_.at(cur_ind) == 0 && cur_ind > 0) {
        digits_.pop_back();
        --cur_ind;
    }
}

void BigInteger::reverseNum() { std::reverse(digits_.begin(), digits_.end()); }
size_t BigInteger::len() const { return digits_.size(); }

BigInteger BigInteger::getSubNum(size_t start, size_t end) const {
    BigInteger subNum;
    subNum.digits_.resize(end - start + 1);
    for (size_t i = start; i <= end; ++i) {
        subNum.digits_.at(i - start) = digits_.at(i);
    }
    subNum.normalizeNum();
    return subNum;
}

bool BigInteger::isZero() const {
    return (digits_.size() == 1) && (digits_.at(0) == 0);
}

void BigInteger::absoluteSum(const BigInteger& num) {
    if (num.len() >= len()) {
        digits_.resize(num.len() + 1);
    }
    long long next_digit = 0;
    for (size_t i = 0; i < len(); ++i) {
        long long cur_digit = digits_[i];
        long long digit_of_num = i < num.len() ? num.digits_.at(i) : 0;
        digits_[i] = (cur_digit + digit_of_num + next_digit) % kBIBase_;
        next_digit = (cur_digit + digit_of_num + next_digit) / kBIBase_;

        if (next_digit == 0 && i > num.len()) {
            break;
        }
    }
    digits_.at(len() - 1) += next_digit;
    normalizeNum();
}

void BigInteger::absoluteSubtraction(const BigInteger& num) {
    const BigInteger* bigger_num = this;
    const BigInteger* smaller_num = &num;

    if (absoluteComparison(num) == -1) {
        is_negative_ = !is_negative_;
        std::swap(bigger_num, smaller_num);
        digits_.resize(num.len());
    }
    int loan = 0;
    for (size_t i = 0; i < bigger_num->len(); ++i) {
        long long cur_digit = bigger_num->digits_.at(i) - loan;
        long long num_digit =
            i >= smaller_num->len() ? 0 : smaller_num->digits_.at(i);
        long long delta = cur_digit - num_digit;
        loan = 0;
        if (delta < 0) {
            loan = 1;
        }
        digits_.at(i) = moduloBase(delta);
    }
    normalizeNum();
}

int BigInteger::absoluteComparison(const BigInteger& num) const {
    if (len() < num.len()) {
        return -1;
    }
    if (len() > num.len()) {
        return 1;
    }
    for (size_t i = num.len(); i > 0; --i) {
        long long ind = i - 1;
        if (digits_.at(ind) < num.digits_.at(ind)) {
            return -1;
        } else if (digits_.at(ind) > num.digits_.at(ind)) {
            return 1;
        }
    }
    return 0;
}

std::string BigInteger::toString() const {
    std::string ans;
    ans.reserve(kBIBaseSize_ * len() + 1);
    if (is_negative_) {
        ans += '-';
    }
    addLeadingZeroes(ans, digits_.at(len() - 1), true);
    for (size_t i = len() - 1; i > 0; --i) {
        addLeadingZeroes(ans, digits_.at(i - 1), false);
    }
    return ans;
}

void BigInteger::addLeadingZeroes(std::string& str, long long digit,
                                  bool is_first_digit) {
    size_t zero_count = kBIBaseSize_;
    if (is_first_digit) {
        zero_count = BigInteger::getNumeralLen(digit);
    }
    str += std::string(zero_count, '0');
    for (size_t i = 0; i < zero_count; ++i) {
        str.at(str.size() - i - 1) = digit % 10 + '0';
        digit /= 10;
    }
}

long long BigInteger::moduloBase(long long num) {
    return num % kBIBase_ < 0 ? num % kBIBase_ + kBIBase_ : num % kBIBase_;
}

size_t BigInteger::getNumeralLen(long long num) {
    if (num == 0) {
        return 1;
    }
    size_t numeralCount = 0;
    while (num != 0) {
        num /= 10;
        numeralCount += 1;
    }
    return numeralCount;
}

BigInteger::operator bool() const {
    return digits_.at(0) != 0 || digits_.size() > 1;
}

BigInteger operator+(BigInteger num1, const BigInteger& num2) {
    num1 += num2;
    return num1;
}
BigInteger operator-(BigInteger num1, const BigInteger& num2) {
    num1 -= num2;
    return num1;
}
BigInteger operator*(BigInteger num1, const BigInteger& num2) {
    num1 *= num2;
    return num1;
}
BigInteger operator/(BigInteger num1, const BigInteger& num2) {
    num1 /= num2;
    return num1;
}

BigInteger operator%(BigInteger num1, const BigInteger& num2) {
    num1 %= num2;
    return num1;
}

BigInteger operator"" _bi(unsigned long long num) { return BigInteger(num); }

BigInteger operator"" _bi(const char* num) { return BigInteger(num); }

bool operator<(const BigInteger& num1, const BigInteger& num2) {
    if (num2.isZero()) {
        return num1.is_negative_;
    }
    if (num1.is_negative_ && !num2.is_negative_) {
        return true;
    }
    if (!num1.is_negative_ && num2.is_negative_) {
        return false;
    }

    int compare = num1.absoluteComparison(num2);
    if (compare == -1) {
        return num1.is_negative_ == false;
    }
    if (compare == 1) {
        return num1.is_negative_ == true;
    }
    return false;
}

bool operator>(const BigInteger& num1, const BigInteger& num2) {
    return num2 < num1;
}

bool operator==(const BigInteger& num1, const BigInteger& num2) {
    return !(num1 < num2) && !(num1 > num2);
}

bool operator!=(const BigInteger& num1, const BigInteger& num2) {
    return !(num1 == num2);
}

bool operator<=(const BigInteger& num1, const BigInteger& num2) {
    return num1 < num2 || num1 == num2;
}

bool operator>=(const BigInteger& num1, const BigInteger& num2) {
    return num1 > num2 || num1 == num2;
}

std::ostream& operator<<(std::ostream& out, const BigInteger& num) {
    out << num.toString();
    return out;
}

std::istream& operator>>(std::istream& in, BigInteger& num) {
    std::string new_num;
    char numeral;
    bool is_negative_ = false;
    in.get(numeral);
    while (!isdigit(numeral) && numeral != '-') {
        in.get(numeral);
    }
    if (numeral == '-') {
        is_negative_ = true;
        in.get(numeral);
    }
    while (isdigit(numeral) && !iscntrl(numeral) && !isspace(numeral) &&
           !in.eof()) {
        new_num.push_back(numeral);
        in.get(numeral);
    }
    num = BigInteger(new_num);
    num.is_negative_ = is_negative_;
    num.normalizeNum();
    return in;
}

void setSign(BigInteger& num, bool is_positive) {
    if ((num > 0) == is_positive) {
        return;
    }
    num *= BigInteger(-1);
}
