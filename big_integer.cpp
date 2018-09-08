//
// Created by Yaroslav239 on 24.06.2018.
//

#include "big_integer.h"

//UINT32_MAX

size_t big_integer::length() const {
    return number.size();
}

big_integer::big_integer()
        : sign(false), number({0}) {}

big_integer::big_integer(int a)
        : sign(a < 0), number({static_cast<ui>(a)}) {}



big_integer::big_integer(big_integer const & a)
        : sign(a.sign), number(a.number){
    reduce();
}

big_integer big_integer::abs() const {
    if (sign) {
        return -(*this);
    }
    return (*this);
}

big_integer::big_integer(std::string str)
        : sign(false), number({0}) {
    bool sign_tmp = (str[0] == '-');
    size_t i = sign_tmp ? 1 : 0;
    while (i+8 < str.length()) {
        *this = *this * BIL + stoi(str.substr(i,9));
        i+=9;
    }
    if (i < str.length()) {
        *this = *this * (static_cast<ui>((pow(10, str.length() - i) + 0.1))) + stoi(str.substr(i));
    }
    if (sign_tmp) {
        *this = -(*this);
    }
    reduce();
}

big_integer::~big_integer() {
    number.clear();
}

big_integer &big_integer::operator=(big_integer const &other) {
    sign = other.sign;
    number = other.number;
    return *this;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer:: operator-() const {
    if (*this == 0) {
        return *this;
    }
    big_integer ans;
    auto _size = number.size();
    ans.number.resize(_size + 1);
    auto data = number.get_data();
    auto ans_data = ans.number.copy_and_get();
    for (size_t i = 0; i < _size; ++i)
        ans_data[i] = ~data[i];
    ans.reduce();
    ans += 1;
    ans.sign = !sign;
    return ans;
}



big_integer &big_integer::operator+=(big_integer const &rhs) {
    ull cf = 0;
    ull sum = 0;
    big_integer ans;
    ans.number.clear();
    ans.sign = sign;
    auto new_length = std::max(length(), rhs.length()) + 2;
    for (size_t it = 0; it < new_length; it++) {
        sum = static_cast<ull>((*this)[it]) + rhs[it] + cf;
        ans.number.push_back(static_cast<ui>(sum & UINT32_MAX));
        cf = sum >> BASE32;
    }
    if (sign != rhs.sign && abs() <= rhs.abs()) {
        if (abs() == rhs.abs())
            ans.sign = false;
        else
            ans.sign = !ans.sign;
    }
    ans.reduce();
    return *this = ans;
}

big_integer operator+(big_integer a, big_integer const &b) {
    return a += b;
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
    return *this += - rhs;
}

big_integer operator-(big_integer a, big_integer const &b) {
    return a -= b;
}

big_integer big_integer::mul_long_short(big_integer::ui x, size_t offset) const {
    big_integer ans;
    ans.sign = sign;
    size_t new_length = length() + offset + 2;
    ans.number.resize(new_length, sign ? UINT32_MAX : 0);
    ull mul = 0;
    ull cf = 0;
    for (size_t i = offset; i < new_length; i++) {
        mul = static_cast<ull>((*this)[i - offset]) * x + cf;
        ans.number[i] = static_cast<ui>(mul & UINT32_MAX);
        cf = mul >> BASE32;
    }
    ans.reduce();
    return ans;
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    big_integer tmp = *this;
    big_integer rhs_copy = rhs;
    bool s1 = sign;
    bool s2 = rhs_copy.sign;
    if (s1) tmp = -tmp;
    if (s2) rhs_copy = -rhs_copy;
    *this = 0;
    for (size_t i = 0; i < rhs_copy.length(); i++)
        *this += tmp.mul_long_short(rhs_copy[i], i);
    reduce();
    if (s1 != s2) *this = -(*this);
    return *this;
}

big_integer big_integer::quotient(big_integer::ui x) const {
    big_integer ans;
    ans.sign = sign;
    size_t n = length();
    ans.number.resize(n, sign ? UINT32_MAX : 0);
    ull div = 0;
    ull cf = 0;
    for (int i = static_cast<int>(n - 1); i >= 0; i--) {
        div = static_cast<ull>((*this)[i]) + (cf << BASE32);
        ans.number[i] = static_cast<ui>((div / x) & UINT32_MAX);
        cf = div % x;
    }
    ans.reduce();
    return ans;
}

big_integer::ui big_integer::remainder(big_integer::ui x) const {
    size_t new_length = length();
    ull cf = 0;
    for (int i = static_cast<int>(new_length - 1); i >= 0; i--)
        cf = ((cf << BASE32) + (*this)[i]) % x;
    return static_cast<ui>(cf & UINT32_MAX);
}



std::pair<big_integer, big_integer> big_integer::div_mod(big_integer const &rhs) {
    if (rhs == 0) {
        throw "division by zero";
    }
    if (rhs.length() > length()) {
        return std::make_pair(0, *this);
    }
    if (*this == rhs) {
        return std::make_pair(1,0);
    }
    if (rhs.length() == 1) {
        return std::make_pair(quotient(rhs[0]), remainder(rhs[0]));
    }
    big_integer b(rhs);

    ui k = static_cast<ui>(ceil(log2((static_cast<double>(static_cast<ui>((SHIFT_BASE >> 1) & UINT32_MAX)) / b.number.back()))));
    b <<= k;
    (*this) <<= k;
    size_t m = length() - b.length();
    size_t n = b.length();
    big_integer q;
    q.number.resize(m + 1);
    b <<= (BASE32 * m);
    if (*this > b) {
        q.number[m] = 1;
        *this -= b;
    }

    for (int j = static_cast<int>(m - 1); j >= 0; j--) {
        q.number[j] = static_cast<ui>((((static_cast<ull>((*this)[n + j]) << BASE32) + (*this)[n + j - 1]) / b.number.back()) & UINT32_MAX);
        b >>= BASE32;
        *this -= b.mul_long_short(q[j], 0);

        while (sign) {
            q.number[j]--;
            *this += b;
        }
    }
    q.reduce();
    reduce();
    return std::make_pair(q, *this >> k);
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    bool s1 = sign;
    bool s2 = rhs.sign;
    if (s1) *this = -(*this);
    *this = !s2 ? div_mod(rhs).first : div_mod(-rhs).first;
    if (s1 != s2) *this = -(*this);
    return *this;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    bool s1 = sign;
    bool s2 = rhs.sign;
    if (s1) *this = -(*this);
    *this = !s2 ? div_mod(rhs).second : div_mod(-rhs).second;
    if (s1) *this = -(*this);
    return *this;
}

big_integer operator*(big_integer a, big_integer const &b) {
    return a*=b;
}

big_integer operator/(big_integer a, big_integer const &b) {
    return a/=b;
}

big_integer operator%(big_integer a, big_integer const &b) {
    return a %= b;
}

bool operator==(big_integer const &a, big_integer const &b) {
    if (a.sign != b.sign || a.length() != b.length()) {
        return false;
    }
    for (int i = static_cast<int>(a.length()) - 1; i >= 0; i--) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a == b);
}

bool operator<(big_integer const &a, big_integer const &b) {
    if (a.sign != b.sign) {
        return a.sign > b.sign;
    }
    if (a.length() != b.length()) {
        return a.sign ^ (a.length() < b.length());
    }
    for (int i = static_cast<int>(a.length()) - 1; i >= 0; i--) {
        if (a[i] != b[i]) {
            return a.sign ^ (a[i] < b[i]);
        }
    }
    return false;
}

bool operator>(big_integer const &a, big_integer const &b) {
    return b < a;
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return !(b < a);
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return !(a < b);
}

big_integer big_integer::operator~() const {
    return -(*this) - 1;
}

template<class FunctorT>
big_integer &big_integer::bitwise_operation(big_integer const &rhs, FunctorT functor) {
    for (size_t i = 0; i < length(); i++) {
        number[i] = functor(number[i], rhs[i]);
    }
    sign = functor(sign, rhs.sign);
    return *this;
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    return bitwise_operation(rhs, std::bit_and<ui>());
}

big_integer operator&(big_integer a, big_integer const &b) {
    return a &= b;
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    return bitwise_operation(rhs, std::bit_or<ui>());
}

big_integer operator|(big_integer a, big_integer const &b) {
    return a |= b;
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    return bitwise_operation(rhs, std::bit_xor<ui>());
}

big_integer operator^(big_integer a, big_integer const &b) {
    return a ^= b;
}

big_integer &big_integer::operator<<=(int rhs) {
    if (rhs < 0)
        throw "error: right value is negative";
    bool negative_flag = sign;
//    if (rhs % BASE32 == 0) {
//        std::vector<int> zeros(rhs / BASE32, negative_flag ? UINT32_MAX : 0);
//        number.insert(number.begin(), zeros.begin(), zeros.end());
//        return *this;
//    }
    if (negative_flag) {
        *this = -(*this);
    }
    big_integer ans;
    size_t begin = rhs/BASE32;
    ans.number.resize(begin + number.size(),negative_flag ? UINT32_MAX : 0);
    auto ans_data = ans.number.copy_and_get();
    auto data = number.copy_and_get();
    rhs %= BASE32;
    ui cf = 0;
    if (rhs == 0) {
        for (size_t i = 0; i < number.size(); i++)
            ans_data[begin + i] = data[i];
    } else {
        for (size_t i = 0; i < number.size(); i++) {
            ans_data[begin + i] = (data[i] << rhs) + cf;
            cf = static_cast<ui>((data[i] >> (BASE32 - rhs)) & UINT32_MAX);
        }
    }
    if (cf != 0)
        ans.number.push_back(cf);
    if (negative_flag)
        ans = -ans;
    ans.reduce();
    return *this = ans;
}

big_integer operator<<(big_integer a, int b) {
    return a <<= b;
}

big_integer &big_integer::operator>>=(int rhs) {
    if (rhs < 0)
        throw "error! right value is negative";
//    if (rhs % BASE32 == 0) {
//        number.erase(number.begin(), number.begin() + (rhs / BASE32));
//        return *this;
//    }
    bool negative_flag = sign;
    if (negative_flag)
        *this = -(*this);
    big_integer ans;
    int diff = rhs / BASE32;
    rhs %= BASE32;
    ans.number.resize(number.size() - diff + 1);
//    ui cf = 0;
//    for (size_t i = length() - 1; static_cast<int>(i) >= gap; i--) {
//        ans.number.push_back(((*this)[i] >> rhs) + cf);
//        cf = static_cast<ui>(((*this)[i] << (BASE32 - rhs)) & UINT32_MAX);
//    }
//    std::reverse(ans.number.begin(), ans.number.end());
    auto res_data = ans.number.copy_and_get();
    auto data = number.copy_and_get();
    if (rhs == 0) {
        for (int i = static_cast<int>(number.size()) - 1; i >= diff; i--)
            res_data[i - diff] = data[i];
    } else {
        ui cf = 0;
        for (int i = static_cast<int>(number.size()) - 1; i >= diff; i--) {
            res_data[i - diff] = (data[i] >> rhs) + cf;
            cf = static_cast<ui>((data[i] << (BASE32 - rhs)) & UINT32_MAX);
        }
    }
    ans.reduce();
    ans.sign = sign;
    if (negative_flag)
        ans = -ans - 1;
    return *this = ans;
}

big_integer operator>>(big_integer a, int b) {
    return a >>= b;
}

big_integer::ui big_integer::operator[](size_t pos) const {
    if (pos < length()) {
        return number[pos];
    }
    return sign ? UINT32_MAX : 0;
}

void big_integer::reduce() {
    while (length() > 1 && ((!sign && number.back() == 0) || (sign && number.back() == UINT32_MAX))) {
        number.pop_back();
    }
}

std::string to_string(big_integer const &a) {
    if (a == 0) {
        return "0";
    }
    std::string ans = "";
    auto a_copy = (a.sign ? -a : a);
    while (a_copy > 0) {
        auto piece = (a_copy % BIL)[0];
        for (int i = 0; i < 9; ++i) {
            ans.push_back('0' + char (piece % 10));
            piece /= 10;
        }
        a_copy /= BIL;
    }
    while (!ans.empty() && ans.back() == '0') {
        ans.pop_back();
    }
    if (a.sign) {
        ans.push_back('-');
    }
    std::reverse(ans.begin(), ans.end());
    return ans;
}


