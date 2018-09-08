#ifndef BIGINT_BIG_INTEGER_H
#define BIGINT_BIG_INTEGER_H

#include <algorithm>
#include <string>
#include <climits>
#include <cmath>
#include "vector_opt.h"

struct big_integer {
    typedef unsigned int ui;
    typedef unsigned long long ull;

    big_integer();

    big_integer(big_integer const &);

    big_integer(int);

    explicit big_integer(std::string);

    ~big_integer();

    big_integer &operator=(big_integer const &);

    big_integer &operator+=(big_integer const &);

    big_integer &operator-=(big_integer const &);

    big_integer &operator*=(big_integer const &);

    big_integer &operator/=(big_integer const &);

    big_integer &operator%=(big_integer const &);

    big_integer operator+() const;

    big_integer operator-() const;

    big_integer operator~() const;

    big_integer &operator&=(big_integer const &rhs);

    big_integer &operator|=(big_integer const &rhs);

    big_integer &operator^=(big_integer const &rhs);

    big_integer &operator<<=(int rhs);

    big_integer &operator>>=(int rhs);

    ui operator[](size_t i) const;

    big_integer abs() const;

    friend big_integer operator+(big_integer a, big_integer const &b);

    friend big_integer operator-(big_integer a, big_integer const &b);

    friend big_integer operator*(big_integer a, big_integer const &b);

    friend big_integer operator/(big_integer a, big_integer const &b);

    friend big_integer operator%(big_integer a, big_integer const &b);

    friend std::string to_string(big_integer const &);

    friend bool operator==(big_integer const &a, big_integer const &b);

    friend bool operator!=(big_integer const &a, big_integer const &b);

    friend bool operator<(big_integer const &a, big_integer const &b);

    friend bool operator>(big_integer const &a, big_integer const &b);

    friend bool operator<=(big_integer const &a, big_integer const &b);

    friend bool operator>=(big_integer const &a, big_integer const &b);

    friend big_integer operator&(big_integer a, big_integer const &b);

    friend big_integer operator|(big_integer a, big_integer const &b);

    friend big_integer operator^(big_integer a, big_integer const &b);

    friend big_integer operator<<(big_integer a, int b);

    friend big_integer operator>>(big_integer a, int b);

    size_t length() const;

private:

    bool sign;
    vector_opt number;

    std::pair<big_integer, big_integer> div_mod(big_integer const &a);

    big_integer mul_long_short(ui x, size_t offset) const;

    big_integer quotient(ui x) const;

    ui remainder(ui x) const;

    template<class FunctorT>
    big_integer & bitwise_operation(big_integer const &rhs, FunctorT functor);

    void reduce();
};

const big_integer::ui BASE32 = 32;
const big_integer::ull SHIFT_BASE = ((big_integer::ull) 1 << BASE32);
const big_integer::ui BIL = 1000000000;

#endif //BIGINT_BIG_INTEGER_H
