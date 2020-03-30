cmake_minimum_required(VERSION 3.14)
project(BigInteger)

set(CMAKE_CXX_STANDARD 14)

add_executable(BigInteger main.cpp BigInteger.cpp BigInteger.h BigIntegerTester.cpp BigIntegerTester.h)