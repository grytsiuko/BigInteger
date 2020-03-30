//*******************************************//
//       Developed by Oleksandr Hrytsiuk     //
//                  Project                  //
//*******************************************//


#include <iostream>
#include <stack>
#include "BigInteger.h"

using std::stack;
using std::cout;
using std::endl;

const BigInteger BigInteger::ZERO(0);
const BigInteger BigInteger::ONE(1);

const Byte BigInteger::BITS_IN_BYTE = 8;
const Byte BigInteger::BYTES_IN_LONG = sizeof(long);
const Byte BigInteger::MIN_BYTE = 0;
const Byte BigInteger::MAX_BYTE = 255;
const int BigInteger::BYTE = 256;

BigInteger::BigInteger(string number) :
        _neg(false),
        _size(0),
        _capacity(0),
        _bytes(nullptr) {

    if (number.empty())
        throw BigIntegerException("Empty string parameter");

    bool negative = false;
    if (number[0] == '-') {
        number[0] = '0';
        negative = true;
    }

    for (string::iterator it = number.begin(); it < number.end(); ++it)
        if (*it < '0' || *it > '9')
            throw BigIntegerException("Illegal string parameter");

    stack<bool> bits = stack<bool>();
    do
        bits.push(is_odd(number.back()));
    while (divide_string_number_by_two(number));

    _capacity = static_cast<int>(bits.size() - 1) / BITS_IN_BYTE + 1;
    _size = _capacity;
    _bytes = new Byte[_capacity];

    int curr_byte_length = static_cast<int>(bits.size() - 1) % BITS_IN_BYTE + 1;
    for (int i = 0; i < _size; i++) {
        Byte curr_byte = 0;
        for (int k = 0; k < curr_byte_length; k++) {
            curr_byte <<= static_cast<Byte>(1);
            curr_byte += bits.top();
            bits.pop();
        }
        _bytes[i] = curr_byte;
        curr_byte_length = BITS_IN_BYTE;
    }

    if (negative)
        negate();
}

BigInteger::BigInteger(const long number) noexcept :
        _neg(static_cast<unsigned long>(number) &
             (static_cast<unsigned long>(1) << static_cast<Byte>(BYTES_IN_LONG * BITS_IN_BYTE - 1))),
        _size(BYTES_IN_LONG),
        _capacity(BYTES_IN_LONG),
        _bytes(new Byte[BYTES_IN_LONG]) {

    unsigned long n = number;
    for (Byte *it = end() - 1; it >= begin(); it--) {
        *it = n & MAX_BYTE;
        n >>= BITS_IN_BYTE;
    }

    normalize();
}

BigInteger::BigInteger(const BigInteger &a) noexcept :
        _neg(a.is_neg()),
        _size(a.size()),
        _capacity(a.capacity()),
        _bytes(new Byte[a.capacity()]) {

    const Byte *a_it = a._bytes;
    for (Byte *it = _bytes; it < end(); it++, a_it++)
        *it = *a_it;
}

BigInteger::BigInteger(BigInteger &&old) noexcept :
        _neg(old.is_neg()),
        _size(old.size()),
        _capacity(old.capacity()),
        _bytes(old._bytes) {

    old._bytes = nullptr;
}

BigInteger &BigInteger::operator=(const BigInteger &a) & noexcept {

    if (this == &a)
        return *this;

    delete[] _bytes;
    _neg = a.is_neg();
    _size = a.size();
    _capacity = a.capacity();
    _bytes = new Byte[_capacity];

    const Byte *a_it = a._bytes;
    for (Byte *it = _bytes; it < end(); it++, a_it++)
        *it = *a_it;

    return *this;
}

BigInteger &BigInteger::operator=(BigInteger &&old) & noexcept {

    if (&old == this)
        return *this;

    _neg = old.is_neg();
    _size = old.size();
    _capacity = old.capacity();
    _bytes = old._bytes;
    old._bytes = nullptr;

    return *this;
}

BigInteger::~BigInteger() {
    delete[] _bytes;
}

void BigInteger::allocate_more() {

    Byte *new_alloc = new Byte[capacity() * 2];

    for (int i = 0; i < capacity(); i++)
        new_alloc[i] = filler();
    for (int i = capacity(); i < capacity() * 2; i++)
        new_alloc[i] = _bytes[i - capacity()];

    delete[] _bytes;
    _bytes = new_alloc;
    _capacity *= 2;
}

void BigInteger::push_front(const Byte byte) {
    _size++;
    if (_size > _capacity)
        allocate_more();
    *begin() = byte;
}

int BigInteger::first_significant_bit() const {
    bool significant_bit_val = !is_neg();
    for (const Byte *it = begin(); it < end(); it++) {
        for (Byte k = 0; k < BITS_IN_BYTE; k++) {
            Byte mask = get_one_bit_mask(BITS_IN_BYTE - 1 - k);
            if (static_cast<bool>((*it) & mask) == significant_bit_val)
                return static_cast<int>(it - begin()) * BITS_IN_BYTE
                       + k - static_cast<int>(is_neg());
        }
    }
    return size() * BITS_IN_BYTE - 1;
}

void BigInteger::normalize() {
    int first_byte = first_significant_bit() / BITS_IN_BYTE;
    _size -= first_byte;
    if (_size == 1 && *begin() == 0)
        _neg = false;
}

bool BigInteger::divide_string_number_by_two(string &number) {

    int remainder = 0;
    bool is_zero = true;

    for (string::iterator it = number.begin(); it < number.end(); ++it) {
        char curr_digit = *it;
        *it = '0' + ((curr_digit - '0') / 2 + remainder);
        if (*it != '0')
            is_zero = false;
        remainder = static_cast<int>(is_odd(curr_digit)) * 5;
    }

    return !is_zero;
}

BigInteger &BigInteger::negate() &{

    if (size() == 1 && *begin() == 0)
        return *this;

    if (is_neg()) {
        int remainder = -1;
        for (Byte *it = end() - 1; it >= begin(); it--) {
            *it += remainder;
            if (remainder == -1 && (*it) != MAX_BYTE)
                remainder = 0;
            *it = ~(*it);
        }

    } else {
        int remainder = 1;
        for (Byte *it = end() - 1; it >= begin(); it--) {
            *it = ~(*it);
            *it += remainder;
            if (remainder == 1 && (*it) != MIN_BYTE)
                remainder = 0;
        }
    }

    _neg = !_neg;
    return *this;
}

string BigInteger::to_string() const &{

    string ans;
    BigInteger curr(*this);

    if (_neg)
        curr.negate();

    bool is_zero;
    do {
        is_zero = true;
        unsigned int remainder = 0;
        for (Byte *it = curr.begin(); it < curr.end(); it++) {
            remainder = remainder * BYTE + (*it);
            *it = remainder / 10;
            if ((*it) > 0)
                is_zero = false;
            remainder %= 10;
        }
        ans = char('0' + remainder) + ans;
    } while (!is_zero);

    if (_neg && ans != "0")
        ans = '-' + ans;

    return ans;
}

string BigInteger::to_bits_string() const &{

    string ans = is_neg() ? "negative " : "positive ";

    ans += "(";
    for (const Byte *it = _bytes; it < begin(); it++) {
        ans += "_";
        for (int k = 0; k < BITS_IN_BYTE; k++) {
            Byte mask = get_one_bit_mask(BITS_IN_BYTE - 1 - k);
            ans += (*it) & mask ? "1" : "0";
        }
    }
    ans += ")";

    for (const Byte *it = begin(); it < end(); it++) {
        for (int k = 0; k < BITS_IN_BYTE; k++) {
            Byte mask = get_one_bit_mask(BITS_IN_BYTE - 1 - k);
            ans += (*it) & mask ? "1" : "0";
        }
        ans += "_";
    }

    return ans;
}

bool BigInteger::is_odd(const char ch) {
    return (ch - '0') % 2 == 1;
}

Byte BigInteger::get_one_bit_mask(const Byte byte) {
    return static_cast<Byte>(1) << byte;
}

BigInteger &BigInteger::operator>>=(const unsigned int shift) &{

    const unsigned int full_bytes = shift / BigInteger::BITS_IN_BYTE;
    const unsigned int left_bits = shift % BigInteger::BITS_IN_BYTE;

    for (Byte *it = end() - 1; it >= begin(); it--) {
        if (it - full_bytes < begin())
            *it = filler();
        else
            *it = *(it - full_bytes);
    }

    Byte remainder_mask = 0;
    for (unsigned int i = 0; i < left_bits; i++)
        remainder_mask = (remainder_mask << static_cast<Byte>(1)) + 1;
    Byte remainder = filler() & remainder_mask;

    for (Byte *it = begin(); it < end(); it++) {
        Byte next_remainder = (*it) & remainder_mask;
        (*it) >>= left_bits;
        (*it) |= static_cast<Byte>(remainder << (BigInteger::BITS_IN_BYTE - left_bits));
        remainder = next_remainder;
    }

    normalize();

    return (*this);
}

BigInteger &BigInteger::operator<<=(const unsigned int shift) &{

    const int new_greatest_bit = first_significant_bit() - static_cast<int>(shift);
    if (new_greatest_bit < 0) {
        _size += (-new_greatest_bit - 1) / BITS_IN_BYTE + 1;
        while (size() > capacity())
            allocate_more();
    }

    const unsigned int full_bytes = shift / BigInteger::BITS_IN_BYTE;
    const unsigned int left_bits = shift % BigInteger::BITS_IN_BYTE;

    for (Byte *it = begin(); it < end(); it++) {
        if (it + full_bytes >= end())
            *it = BigInteger::MIN_BYTE;
        else
            *it = *(it + full_bytes);
    }

    Byte remainder_mask = 0;
    for (unsigned int i = 0; i < left_bits; i++)
        remainder_mask = (remainder_mask << static_cast<Byte>(1)) + 1;
    remainder_mask <<= (BigInteger::BITS_IN_BYTE - left_bits);
    Byte remainder = BigInteger::MIN_BYTE & remainder_mask;

    for (Byte *it = end() - 1; it >= begin(); it--) {
        Byte next_remainder = (*it) & remainder_mask;
        (*it) <<= left_bits;
        (*it) |= static_cast<Byte>(remainder >> (BigInteger::BITS_IN_BYTE - left_bits));
        remainder = next_remainder;
    }

    return (*this);
}

BigInteger &BigInteger::operator+=(const BigInteger &b) &{

    if (size() < b.size()) {
        _size = b.size();
        while (size() > capacity())
            allocate_more();
    }

    int remainder = 0;
    const Byte *b_it = b.end() - 1;
    for (Byte *it = end() - 1; it >= begin(); it--, b_it--) {
        remainder += *it;
        if (b_it >= b.begin())
            remainder += *b_it;
        else
            remainder += b.filler();
        *it = remainder % BigInteger::BYTE;
        remainder /= BigInteger::BYTE;
    }

    if (is_neg() == b.is_neg()) {
        if (is_neg()) {
            if (remainder > 0)
                push_front(MAX_BYTE);
            else
                push_front(MAX_BYTE - 1);
        } else {
            if (remainder > 0)
                push_front(MIN_BYTE + 1);
        }
    } else {
        _neg = !static_cast<bool>(remainder);
    }

    normalize();

    return *this;
}

BigInteger &BigInteger::operator-=(BigInteger b) &{
    b.negate();
    (*this) += b;
    return *this;
}

BigInteger &BigInteger::operator*=(const BigInteger &b) &{

    if (this == &b)
        return (*this) *= BigInteger(b);

    if (b.is_neg()) {
        (*this) *= -b;
        return (*this).negate();
    }

    bool negative = false;

    if (is_neg()) {
        negative = true;
        negate();
    }

    BigInteger shiftedX(*this);
    (*this) -= *this;

    for (const Byte *b_it = b.end() - 1; b_it >= b.begin(); b_it--) {
        for (Byte k = 0; k < BITS_IN_BYTE; k++) {
            Byte mask = get_one_bit_mask(k);
            if ((*b_it & mask) != 0)
                (*this) += shiftedX;
            shiftedX <<= 1;
        }
    }

    if (negative)
        negate();

    return *this;
}

BigInteger &BigInteger::operator/=(const BigInteger &b) &{

    if (this == &b)
        return (*this) /= BigInteger(b);

    if (b.size() == 1 && *b.begin() == 0)
        throw BigIntegerException("Division by zero");

    if (b.is_neg()) {
        (*this) /= -b;
        return (*this).negate();
    }

    if (is_neg()) {
        negate();
        BigInteger ans = divide_positive(*this, b);
        if (ans * b < *this)
            ++ans;
        (*this) = ans;
        (*this).negate();
    } else {
        (*this) = divide_positive(*this, b);
    }

    return *this;
}

int BigInteger::compare(const BigInteger &a, const BigInteger &b) {
    if (a.is_neg() == b.is_neg()) {
        if (a.size() != b.size()) {
            if (a.is_neg())
                return b.size() - a.size();
            else
                return a.size() - b.size();
        }
        for (const Byte *a_it = a.begin(), *b_it = b.begin(); a_it < a.end(); a_it++, b_it++) {
            for (Byte k = 0; k < BITS_IN_BYTE; k++) {
                Byte mask = get_one_bit_mask(BITS_IN_BYTE - 1 - k);
                int a_bit = (*a_it & mask);
                int b_bit = (*b_it & mask);
                if (a_bit != b_bit) {
                    if (a.is_neg())
                        return b_bit - a_bit;
                    else
                        return a_bit - b_bit;
                }
            }
        }
        return 0;
    } else {
        return a.is_neg() ? -1 : 1;
    }
}

BigInteger BigInteger::divide_positive(const BigInteger &a, const BigInteger &b) {
    if (compare(a, b) < 0)
        return ZERO;
    BigInteger q = divide_positive(a, b << 1);
    q <<= 1;
    if (compare(a - q * b, b) < 0)
        return q;
    else
        return ++q;
}

ostream &operator<<(ostream &os, const BigInteger &number) {
    os << number.to_string();
    return os;
}

BigInteger operator>>(BigInteger a, unsigned int shift) {
    return a >>= shift;
}

BigInteger operator<<(BigInteger a, unsigned int shift) {
    return a <<= shift;
}

BigInteger operator+(BigInteger a, const BigInteger &b) {
    return a += b;
}

BigInteger operator-(BigInteger a, const BigInteger &b) {
    return a -= b;
}

BigInteger operator*(BigInteger a, const BigInteger &b) {
    return a *= b;
}

BigInteger operator/(BigInteger a, const BigInteger &b) {
    return a /= b;
}

BigInteger &operator++(BigInteger &a) {
    return a += BigInteger::ONE;
}

BigInteger operator++(BigInteger &a, int) {
    BigInteger copy(a);
    ++a;
    return copy;
}

BigInteger &operator--(BigInteger &a) {
    return a -= BigInteger::ONE;
}

BigInteger operator--(BigInteger &a, int) {
    BigInteger copy(a);
    --a;
    return copy;
}

BigInteger operator-(BigInteger a) {
    return a.negate();
}

bool operator==(const BigInteger &a, const BigInteger &b) {
    return BigInteger::compare(a, b) == 0;
}

bool operator!=(const BigInteger &a, const BigInteger &b) {
    return BigInteger::compare(a, b) != 0;
}

bool operator<=(const BigInteger &a, const BigInteger &b) {
    return BigInteger::compare(a, b) <= 0;
}

bool operator>=(const BigInteger &a, const BigInteger &b) {
    return BigInteger::compare(a, b) >= 0;
}

bool operator<(const BigInteger &a, const BigInteger &b) {
    return BigInteger::compare(a, b) < 0;
}

bool operator>(const BigInteger &a, const BigInteger &b) {
    return BigInteger::compare(a, b) > 0;
}

BigInteger power(const BigInteger &a, const unsigned int m) {

    if (a == BigInteger("2"))
        return a << m - 1;

    unsigned int n = m;
    BigInteger multiplier(a);
    BigInteger ans(BigInteger::ONE);

    while (n > 0) {
        if (n % 2 == 1) {
            ans *= multiplier;
            n--;
        } else {
            multiplier *= multiplier;
            n /= 2;
        }
    }

    return ans;
}
