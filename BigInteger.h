//*******************************************//
//       Developed by Oleksandr Hrytsiuk     //
//                  Project                  //
//*******************************************//

#pragma once

#include <iostream>
#include <string>
#include <utility>

using std::string;
using std::ostream;
using std::exception;

using Byte = unsigned char;


class BigInteger {

public:

    static const BigInteger ZERO;
    static const BigInteger ONE;

    static const Byte BITS_IN_BYTE;
    static const Byte BYTES_IN_LONG;
    static const Byte MIN_BYTE;
    static const Byte MAX_BYTE;
    static const int BYTE;

    class BigIntegerException;

    BigInteger(string);

    BigInteger(long = 0) noexcept;

    BigInteger(const BigInteger &) noexcept;

    BigInteger(BigInteger &&) noexcept;

    BigInteger &operator=(const BigInteger &) & noexcept;

    BigInteger &operator=(BigInteger &&) & noexcept;

    ~BigInteger();

    bool is_neg() const &{ return _neg; }

    int size() const &{ return _size; }

    int capacity() const &{ return _capacity; }

    BigInteger &negate() &;

    string to_string() const &;

    string to_bits_string() const &;

    BigInteger &operator<<=(unsigned int) &;

    BigInteger &operator>>=(unsigned int) &;

    BigInteger &operator+=(const BigInteger &) &;

    BigInteger &operator-=(BigInteger) &;

    BigInteger &operator*=(const BigInteger &) &;

    BigInteger &operator/=(const BigInteger &) &;

    static int compare(const BigInteger &, const BigInteger &);

private:

    bool _neg;
    int _size;
    int _capacity;
    Byte *_bytes;

    Byte *begin() { return _bytes + (_capacity - _size); }

    Byte *end() { return _bytes + _capacity; }

    const Byte *begin() const { return _bytes + (_capacity - _size); }

    const Byte *end() const { return _bytes + _capacity; }

    Byte filler() const { return _neg ? BigInteger::MAX_BYTE : BigInteger::MIN_BYTE; };

    void allocate_more();

    void push_front(Byte);

    int first_significant_bit() const;

    void normalize();

    static bool is_odd(char);

    static Byte get_one_bit_mask(Byte);

    static bool divide_string_number_by_two(string &);

    static BigInteger divide_positive(const BigInteger &, const BigInteger &);
};

class BigInteger::BigIntegerException : exception {

private:

    const string _error_message;

public:

    explicit BigIntegerException(string error) : _error_message(std::move(error)) {};

    string get_error_message() const &{ return _error_message; };

};


ostream &operator<<(ostream &, const BigInteger &);

BigInteger operator<<(BigInteger, unsigned int);

BigInteger operator>>(BigInteger, unsigned int);

BigInteger operator+(BigInteger, const BigInteger &);

BigInteger operator-(BigInteger, const BigInteger &);

BigInteger operator*(BigInteger, const BigInteger &);

BigInteger operator/(BigInteger, const BigInteger &);

bool operator==(const BigInteger &, const BigInteger &);

bool operator!=(const BigInteger &, const BigInteger &);

bool operator<=(const BigInteger &, const BigInteger &);

bool operator>=(const BigInteger &, const BigInteger &);

bool operator<(const BigInteger &, const BigInteger &);

bool operator>(const BigInteger &, const BigInteger &);

BigInteger &operator++(BigInteger &);

BigInteger operator++(BigInteger &, int);

BigInteger &operator--(BigInteger &);

BigInteger operator--(BigInteger &, int);

BigInteger operator-(BigInteger);

BigInteger power(const BigInteger &, unsigned int);
