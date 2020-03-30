//*******************************************//
//       Developed by Oleksandr Hrytsiuk     //
//                  Project                  //
//*******************************************//

#pragma once

#include "BigInteger.h"


class BigIntegerTester {

private:

    using binary_operator = BigInteger (*const)(BigInteger, const BigInteger &);
    using shift_operator = BigInteger (*const)(BigInteger, unsigned int);
    using cmp_operator = bool (*const)(const BigInteger &, const BigInteger &);

    static void assert_expression(const string &a, binary_operator, const string &b, const string &ans);

    static void assert_expression(const string &a, shift_operator, unsigned int, const string &ans);

    static void assert_expression(const string &a, cmp_operator, const string &b);

    static void assert_equal_expression(const BigInteger &a, const string &ans);

    static void assert_power_expression(const string &a, unsigned int pow, const string &ans);

public:

    static void test_all();

    static void test_converting();

    static void test_negating();

    static void test_shifts();

    static void test_add_sub();

    static void test_mul_div();

    static void test_compare();

    static void test_inc_dec();

    static void test_self_operator();

    static void test_power();
};
