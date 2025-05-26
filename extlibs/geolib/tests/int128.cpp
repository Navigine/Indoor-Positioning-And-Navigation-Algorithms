#include "point.h"

namespace {

using namespace geolib::detail;

void test_divmod(const std::string& name, int128_t a, int64_t b, int128_t c, int64_t expected_q, int128_t expected_r)
{
    std::stringstream ss;
    ss << "Test " << name << ":" << std::endl;
    ss << "  a = " << a << std::endl;
    ss << "  b = " << b << std::endl;
    ss << "  c = " << c << std::endl;

    auto p = divmod128(a, b, c);

    ss << "Result divmod(ab/c):" << std::endl;
    ss << "  quotient  = " << p.first << std::endl;
    ss << "  remainder = " << p.second << " = " << (p.second>>64) << " * 2^64 + " << (p.second & (~0ULL)) << std::endl;

    auto ok = true;

    if (p.first != expected_q) {
        ss << "Test " << name << " failed:" << std::endl;
        ss << "  quotient = " << p.first << std::endl;
        ss << "  expected = " << expected_q << std::endl;
        ok = false;
    }

    if (p.second != expected_r) {
        ss << "Test " << name << " failed:" << std::endl;
        ss << "  remainder = " << p.second << std::endl;
        ss << "  expected  = " << expected_r << std::endl;
        ok = false;
    }

    if (!ok) {
        throw std::runtime_error(ss.str());
    }

    std::cout << ss.str() << std::endl;
}

void test_rdiv(const std::string& name, int128_t a, int64_t b, int128_t c, int64_t expected)
{
    std::stringstream ss;
    ss << "Test " << name << ":" << std::endl;
    ss << "  a = " << a << std::endl;
    ss << "  b = " << b << std::endl;
    ss << "  c = " << c << std::endl;

    auto result = rdiv128(a, b, c);

    ss << "Result round(ab/c):" << std::endl;
    ss << "  result = " << result << std::endl;

    auto ok = true;

    if (result != expected) {
        ss << "Test " << name << " failed:" << std::endl;
        ss << "  result   = " << result << std::endl;
        ss << "  expected = " << expected << std::endl;
        ok = false;
    }

    if (!ok) {
        throw std::runtime_error(ss.str());
    }

    std::cout << ss.str() << std::endl;
}

} // namespace

void test_divmod_1()
{
    test_divmod(__FUNCTION__,
        13, 4, 14,      // a b c
        3, 10);         // q r
}

void test_divmod_2()
{
    test_divmod(__FUNCTION__,
        13, 3, 14,      // a b c
        2, 11);         // q r
}

void test_divmod_3()
{
    test_divmod(__FUNCTION__,
        13, 5, 14,      // a b c
        4, 9);          // q r
}

void test_divmod_4()
{
    // Testing some random big numbers
    test_divmod(__FUNCTION__,
        (static_cast<int128_t>(123456789012345678)<<64) + 123456789012345678,     // a (128-bit)
        374637673643767364,                                                       // b (64-bit)
        (static_cast<int128_t>(872376520126524378)<<64) + 872376520126524378,     // c (128-bit)
        53017892118882966,                                                        // q (64-bit)
        (static_cast<int128_t>(718338145254907644)<<64) + 718338145254907644);    // r (128-bit)
}

void test_rdiv_1()
{
    test_rdiv(__FUNCTION__, 13, 3, 14, 3);
}

void test_rdiv_2()
{
    test_rdiv(__FUNCTION__, -13, 3, 14, -3);
}

void test_rdiv_3()
{
    test_rdiv(__FUNCTION__, -13, -3, 14, 3);
}

void test_rdiv_4()
{
    test_rdiv(__FUNCTION__, -7, -8, -14, -4);
}

void test_rdiv_5()
{
    // Testing some random big numbers
    test_rdiv(__FUNCTION__,
        (static_cast<int128_t>(123456789012345678)<<64) + 123456789012345678,       // a (128-bit)
        -374637673643767364,                                                        // b (64-bit)
        (static_cast<int128_t>(872376520126524378)<<64) + 872376520126524378,       // c (128-bit)
        -53017892118882967);                                                        // q (64-bit)
}
