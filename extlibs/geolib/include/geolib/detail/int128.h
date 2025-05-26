#pragma once

#include "util.h"

namespace GEOLIB_NAMESPACE::detail {

class Int128
{
public:
    Int128() noexcept = default;
    Int128(const Int128 &) noexcept = default;
    Int128(Int128 &&) noexcept = default;
    Int128& operator=(const Int128&) noexcept = default;
    Int128& operator=(Int128&&) noexcept = default;

    Int128(int val): Int128(-(val < 0), val) {}
    Int128(long val): Int128(-(val < 0), val) {}
    Int128(long long val): Int128(-(val < 0), val) {}

    Int128(unsigned int val): Int128(0, val) {}
    Int128(unsigned long val): Int128(0, val) {}
    Int128(unsigned long long val): Int128(0, val) {}

    explicit operator bool() const { return high_ || low_; }
    explicit operator char() const { return char(low_); }
    explicit operator signed char() const { return static_cast<signed char>(low_); }
    explicit operator unsigned char() const { return static_cast<unsigned char>(low_); }
    explicit operator short() const { return short(low_); }
    explicit operator unsigned short() const { return static_cast<unsigned short>(low_); }
    explicit operator int() const { return int(low_); }
    explicit operator unsigned int() const { return unsigned(low_); }
    explicit operator long() const { return long(low_); }
    explicit operator unsigned long() const { return static_cast<unsigned long>(low_); }
    explicit operator long long() const { return static_cast<long long>(low_); }
    explicit operator unsigned long long() const { return static_cast<unsigned long long>(low_); }

    Int128 operator+() const { return *this; }
    Int128 operator-() const { return Int128(-high_ - (low_ != 0), -low_); }
    Int128 operator~() const { return Int128(~high_, ~low_); }
    bool operator!() const { return !high_ && !low_; }

    friend Int128 operator+(Int128 lhs, Int128 rhs) {
        return Int128(lhs.high_ + rhs.high_ + (lhs.low_ + rhs.low_ < lhs.low_), lhs.low_ + rhs.low_);
    }

    friend Int128 operator-(Int128 lhs, Int128 rhs) {
        return Int128(lhs.high_ - rhs.high_ - (lhs.low_ < rhs.low_), lhs.low_ - rhs.low_);
    }

    friend Int128 operator*(Int128 lhs, Int128 rhs) {
        return mul(lhs, rhs);
    }

    friend Int128 operator&(Int128 lhs, Int128 rhs) {
        return Int128(lhs.high_ & rhs.high_, lhs.low_ & rhs.low_);
    }

    friend Int128 operator|(Int128 lhs, Int128 rhs) {
        return Int128(lhs.high_ | rhs.high_, lhs.low_ | rhs.low_);
    }

    friend Int128 operator^(Int128 lhs, Int128 rhs) {
        return Int128(lhs.high_ ^ rhs.high_, lhs.low_ ^ rhs.low_);
    }

    friend Int128 operator>>(Int128 lhs, unsigned int rhs) {
        return shr(lhs, rhs);
    }

    friend Int128 operator<<(Int128 lhs, unsigned int rhs) {
        return shl(lhs, rhs);
    }

    friend bool operator==(Int128 lhs, Int128 rhs) {
        return lhs.high_ == rhs.high_ && lhs.low_ == rhs.low_;
    }

    friend bool operator!=(Int128 lhs, Int128 rhs) {
        return lhs.high_ != rhs.high_ || lhs.low_ != rhs.low_;
    }

    friend bool operator<(Int128 lhs, Int128 rhs) {
        return lhs.high_ < rhs.high_ || (lhs.high_ == rhs.high_ && lhs.low_ < rhs.low_);
    }

    friend bool operator<=(Int128 lhs, Int128 rhs) {
        return lhs.high_ < rhs.high_ || (lhs.high_ == rhs.high_ && lhs.low_ <= rhs.low_);
    }

    friend bool operator>(Int128 lhs, Int128 rhs) {
        return rhs < lhs;
    }

    friend bool operator>=(Int128 lhs, Int128 rhs) {
        return rhs <= lhs;
    }

    friend Int128& operator+=(Int128& lhs, Int128 rhs) {
        return lhs = (lhs + rhs);
    }

    friend Int128& operator-=(Int128& lhs, Int128 rhs) {
        return lhs = (lhs - rhs);
    }

    friend Int128& operator*=(Int128& lhs, Int128 rhs) {
        return lhs = (lhs * rhs);
    }

    friend Int128& operator<<=(Int128& lhs, unsigned int rhs) {
        return lhs = (lhs << rhs);
    }

    friend Int128& operator>>=(Int128& lhs, unsigned int rhs) {
        return lhs = (lhs >> rhs);
    }

    friend std::ostream& operator<<(std::ostream& dest, Int128 value) {
        static const char* digits = "0123456789";
        if (std::ostream::sentry(dest)) {
            int sign = 1;
            if (value < 0) {
                sign = -sign, value = -value;
            }

            char buffer[128] = {0};
            char* d = std::end(buffer);
            do {
                *(--d) = digits[mod10(value)];
                value = div10(value);
            } while (value > 0);

            if (sign < 0) {
                *(--d) = '-';
            }
            int len = std::end(buffer) - d;
            if (dest.rdbuf()->sputn(d, len) != len) {
                dest.setstate(std::ios_base::badbit);
            }
        }
        return dest;
    }

private:
    Int128(int64_t hi, uint64_t lo): high_(hi), low_(lo) {}

    inline static Int128 mul(Int128 lhs, Int128 rhs) {
        const int64_t s1 =
            static_cast<uint64_t>(lhs.high_) * rhs.low_ +
            static_cast<uint64_t>(rhs.high_) * lhs.low_ +
            (lhs.low_ >> 32) * (rhs.low_ >> 32);
        const uint64_t s2 = (lhs.low_ & 0xFFFFFFFF) * (rhs.low_ & 0xFFFFFFFF);
        const uint64_t s3 = (lhs.low_ >> 32) * (rhs.low_ & 0xFFFFFFFF);
        const uint64_t s4 = (rhs.low_ >> 32) * (lhs.low_ & 0xFFFFFFFF);
        return Int128(s1, s2) +
               Int128(s3 >> 32, (s3 & 0xFFFFFFFF) << 32) +
               Int128(s4 >> 32, (s4 & 0xFFFFFFFF) << 32);
    }

    inline static Int128 shl(Int128 lhs, unsigned int rhs) {
        assert(lhs.high_ >= 0 && "Bitwise-shift for negative numbers is undefined");
        return rhs & 64 ? Int128(lhs.low_ << (rhs & 63), 0) :
               rhs & 63 ? Int128(
                    (static_cast<uint64_t>(lhs.high_) << (rhs & 63)) | (lhs.low_ >> (64 - (rhs & 63))),
                    lhs.low_ << (rhs & 63)) : lhs;
    }

    inline static Int128 shr(Int128 lhs, unsigned int rhs) {
        assert(lhs.high_ >= 0 && "Bitwise-shift for negative numbers is undefined");
        return rhs & 64 ? Int128(0, lhs.high_ >> (rhs & 63)) :
               rhs & 63 ? Int128(lhs.high_ >> (rhs & 63), lhs.high_ << (64 - (rhs & 63)) | (lhs.low_ >> (rhs & 63))) : lhs;
    }

    inline static int mod10(Int128 value) {
        assert(value >= 0 && "mod10 for negative numbers is undefined");
        return (6 * static_cast<int>(value.high_ % 10) + static_cast<int>(value.low_ % 10)) % 10;
    }

    inline static Int128 div10(Int128 value) {
        assert(value >= 0 && "div10 for negative numbers is undefined");
        static const uint64_t N = 0x1999999999999999ULL; // 2^64 / 10
        const int64_t h1 = value.high_ / 10;
        const uint64_t h2 = value.high_ % 10;
        const uint64_t l1 = value.low_ / 10;
        const uint64_t l2 = value.low_ % 10;
        return Int128(h1, (h2 * 6 + l2) / 10) + Int128(0, l1 + h2 * N);
    }

    int64_t high_;
    uint64_t low_;
};

using int128_t = Int128;

inline std::pair<int64_t, int128_t> divmod128(int128_t a, int64_t b, int128_t c)
{
    // ab/c
    assert(0 <= a);
    assert(0 <= b);
    assert(a < c);

    int64_t mask = 1;
    int64_t q = 0;      // quotient
    int128_t r = 0;     // remainder

    for(auto index = b; index > 1; index >>= 1) {
        mask <<= 1;
    }

    while (mask) {
        q <<= 1;
        r <<= 1;
        if (r >= c) {
            r -= c;
            q += 1;
        }
        if (b & mask) {
            r += a;
            if (r >= c) {
                r -= c;
                q += 1;
            }
        }
        mask >>= 1;
    }

    return std::make_pair(q, r);
}

inline int64_t rdiv128(int128_t a, int64_t b, int128_t c)
{
    assert(c != 0);

    if (a == 0 || b == 0) {
        return 0;
    }

    int sign = 1;

    if (a < 0) {
        sign = -sign, a = -a;
    }

    if (b < 0) {
        sign = -sign, b = -b;
    }

    if (c < 0) {
        sign = -sign, c = -c;
    }

    auto [q, r] = divmod128(a, b, c);

    if ((r <<= 1) >= c) {
        q += 1;
    }

    return sign > 0 ? q : -q;
}

inline int128_t det128(int64_t a11, int64_t a12, int64_t a21, int64_t a22)
{
    int128_t result, tmp;
    tmp = a11; tmp *= a22; result = tmp;
    tmp = a12; tmp *= a21; result -= tmp;
    return result;
}

} // namespace GEOLIB_NAMESPACE::detail
