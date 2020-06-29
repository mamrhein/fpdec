/* ---------------------------------------------------------------------------
Name:        decimal_test.cpp

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application. For license
             details please read the file LICENSE.TXT provided together
             with the application.
------------------------------------------------------------------------------
$Source$
$Revision$
*/

#include "catch.hpp"
#include "fpdecimal.hpp"

using namespace fpdec;

TEST_CASE("Default constructor / destructor") {
    for (int i = 0; i <= 10; i++) {
        Decimal d = {};
        Decimal *ptr2d = &d;
        CHECK(ptr2d != NULL);
        CHECK(d.sign() == 0);
        CHECK(d.precision() == 0);
    }
}

TEST_CASE("Decimal from string") {

    struct test_data {
        std::string literal;
        fpdec_sign_t sign;
        fpdec_dec_prec_t dec_prec;
        int magnitude;
    };

    struct test_data tests[] = {
        {
            .literal = "  000000001926.837 \n",
            .sign = 1,
            .dec_prec = 3,
            .magnitude = 3,
        },
        {
            .literal = "+5.387E+1",
            .sign = 1,
            .dec_prec = 2,
            .magnitude = 1,
        },
        {
            .literal = "   +0.00e-0",
            .sign = 0,
            .dec_prec = 2,
            .magnitude = -1,
        },
        {
            .literal = "-3",
            .sign = -1,
            .dec_prec = 0,
            .magnitude = 0,
        },
        {
            .literal = "-0.700",
            .sign = -1,
            .dec_prec = 3,
            .magnitude = -1,
        },
        {
            .literal = "-0.007",
            .sign = -1,
            .dec_prec = 3,
            .magnitude = -3,
        },
        {
            .literal = "-1111111111111111111"
                       "2222222222222222222"
                       "3333333333333333333"
                       "4444444444444444444"
                       "5555555555555555555"
                       "."
                       "6666666666666666666"
                       "7777777777777777777"
                       "88888",
            .sign = -1,
            .dec_prec = 43,
            .magnitude = 5 * 19 - 1,
        },
    };

    for (const auto &test : tests) {

        SECTION(test.literal) {
            Decimal d = Decimal(test.literal);
            CHECK(d.sign() == test.sign);
            CHECK(d.precision() == test.dec_prec);
            if (test.sign != 0)
                CHECK(d.magnitude() == test.magnitude);
        }
    }
}

template<typename T>
int sign(T num) {
    static_assert(std::is_arithmetic<T>(), "T must be a number type.");
    return (num > 0 ? 1 : (num < 0 ? -1 : 0));
}

template<typename T>
int magn(T num) {
    static_assert(std::is_arithmetic<T>(), "T must be a number type.");
    if (num == INT64_MIN)   // special case where std::abs gives wrong value
        return 18;
    return (int)std::log10(std::abs((int64_t)num));
}

TEST_CASE("Decimal from integer") {

    for (const auto &val : {INT16_MIN, -14, 0, 328, INT16_MAX}) {

        SECTION(std::to_string(val)) {
            Decimal d = Decimal{val};
            CHECK(d.sign() == sign(val));
            CHECK(d.precision() == 0);
            if (val != 0)
                CHECK(d.magnitude() == magn(val));
        }
    }

    for (const auto &val : {INT32_MIN, -143, 1, 320937, INT32_MAX}) {

        SECTION(std::to_string(val)) {
            Decimal d = Decimal{val};
            CHECK(d.sign() == sign(val));
            CHECK(d.precision() == 0);
            CHECK(d.magnitude() == magn(val));
        }
    }

    for (const auto &val : {INT64_MIN, -143L, 1L, 640937L, INT64_MAX}) {

        SECTION(std::to_string(val)) {
            Decimal d = Decimal{val};
            CHECK(d.sign() == sign(val));
            CHECK(d.precision() == 0);
            CHECK(d.magnitude() == magn(val));
        }
    }
}

TEST_CASE("Decimal from Decimal") {

    SECTION("Simple copy") {
        Decimal a = Decimal{"123456789012345678901234567890.1234"};
        Decimal b = Decimal(a);
        CHECK(a == b);
    }

    SECTION("With adjusting precision") {
        Decimal a = Decimal{"123456789012345678901234567890.12345678"};
        Decimal b = Decimal(a, 8);
        Decimal c = Decimal(a, 12);
        Decimal d = Decimal(a, 6);
        Decimal e = Decimal(a, 4, Rounding::round_half_down);
        CHECK(a == b);
        CHECK(b.precision() == a.precision());
        CHECK(a == c);
        CHECK(c.precision() == 12);
        CHECK(a != d);
        CHECK(d.precision() == 6);
        CHECK(a != e);
        CHECK(e.precision() == 4);
    }
}

TEST_CASE("Comparison") {

    SECTION("Equality") {
        Decimal a = Decimal{"123456789012345678901234567890.1234"};
        Decimal b = {a};
        const long long l = {-2083L * INT32_MAX};
        const long long m = l + 1;
        Decimal c = Decimal{l};
        Decimal d = Decimal{m};

        CHECK(a == b);
        CHECK(!(a != b));
        CHECK(a != c);
        CHECK(!(a == c));
        CHECK(c != d);
        CHECK(!(c == d));
        CHECK(c == Decimal{l});
        CHECK(!(c != Decimal{l}));
        CHECK(m == d);
        CHECK(!(m != d));
    }

    SECTION("Ordering") {
        Decimal a = Decimal{"1234567890123456789012345678901234.56789"};
        Decimal b = {a};
        long l = {12083L * INT32_MAX};
        long m = l + 1;
        Decimal c = Decimal{l};
        Decimal d = Decimal{m};

        CHECK(a <= b);
        CHECK(a >= b);
        CHECK(!(a < b));
        CHECK(!(a > b));
        CHECK(a >= c);
        CHECK(a > c);
        CHECK(!(a < c));
        CHECK(c <= d);
        CHECK(!(c > d));
        CHECK(c <= Decimal{l});
        CHECK(!(c < Decimal{l}));
        CHECK(c >= Decimal{l});
        CHECK(!(c > Decimal{l}));
        CHECK(m <= d);
        CHECK(!(m < d));
        CHECK(m >= d);
        CHECK(!(m > d));
    }
}

TEST_CASE("Arithmetic ops") {

    SECTION("Add / sub") {

        struct test_data {
            std::string lit_x;
            std::string lit_y;
            std::string lit_sum;
            std::string lit_diff;
        };

        struct test_data tests[] = {
            {
                .lit_x = "-0.282763e-7",
                .lit_y = "9.30001e32",
                .lit_sum = "930000999999999999999999999999999.9999999717237",
                .lit_diff = "-930001000000000000000000000000000.0000000282763"
            },
            {
                .lit_x = "1001000000780000030000000000.25",
                .lit_y = "1000000000700000030000000000.20",
                .lit_sum = "2001000001480000060000000000.45",
                .lit_diff = "1000000080000000000000000.05"
            },
            {
                .lit_x =
                    "999999999999999999999999999999999999999999999999999"
                    "999999999999999999999999999999999999999999999999999"
                    "999999999999999999999999999999999999999999999999999"
                    "999999999999999999999999999999999999999999999999999"
                    "999999999999999999999999999999999999999999999999999"
                    "999999999999999999999999999999999999999"
                    ".18318318318318318318318318318318318318318318318318"
                    "318318318318318318318318318318318318318318318318318"
                    "318318318318318318318318318318318318318318318318318"
                    "318318318318318318318318318318318318318318318318318"
                    "3183183183183183183183183183183183183183",
                .lit_y =
                    "999999999999999999999999999999999999999999999999999"
                    "999999999999999999999999999999999999999999999999999"
                    "999999999999999999999999999999999999999999999999999"
                    "999999999999999999999999999999999999999999999999999"
                    "999999999999999999999999999999999999999999999999999"
                    "999999999999999999999999999999999999999"
                    ".18318318318318318318318318318318318318318318318318"
                    "318318318318318318318318318318318318318318318318318"
                    "318318318318318318318318318318318318318318318318318"
                    "318318318318318318318318318318318318318318318318318"
                    "3183183183183183183183183183183183183183",
                .lit_sum =
                    "199999999999999999999999999999999999999999999999999"
                    "999999999999999999999999999999999999999999999999999"
                    "999999999999999999999999999999999999999999999999999"
                    "999999999999999999999999999999999999999999999999999"
                    "999999999999999999999999999999999999999999999999999"
                    "9999999999999999999999999999999999999998"
                    ".36636636636636636636636636636636636636636636636636"
                    "636636636636636636636636636636636636636636636636636"
                    "636636636636636636636636636636636636636636636636636"
                    "636636636636636636636636636636636636636636636636636"
                    "6366366366366366366366366366366366366366",
                .lit_diff = "0"
            },
        };

        for (const auto &test : tests) {
            auto x = Decimal(test.lit_x);
            auto y = Decimal(test.lit_y);
            auto sum = Decimal(test.lit_sum);
            auto diff = Decimal(test.lit_diff);
            CHECK(Decimal() + x == x);
            CHECK(x - x == Decimal());
            CHECK(x + y == sum);
            CHECK(x - y == diff);
        }
    }

    SECTION("Mul / div") {

        struct test_data {
            std::string lit_x;
            std::string lit_y;
            std::string lit_prod;
        };

        struct test_data tests[] = {
            {
                .lit_x = "-0.2827637",
                .lit_y = "9.30001e-32",
                .lit_prod = "-0.00000000000000000000000000000002629705237637",
            },
            {
                .lit_x = "17.4",
                .lit_y = "-17.4",
                .lit_prod = "-302.76",
            },
        };
        auto zero = Decimal();

        for (const auto &test : tests) {
            auto x = Decimal(test.lit_x);
            auto y = Decimal(test.lit_y);
            auto prod = Decimal(test.lit_prod);
            CHECK(Decimal(1) * x == x);
            CHECK(x / x == Decimal(1));
            CHECK(x * zero == zero);
            CHECK(zero * x == zero);
            CHECK(zero / x == zero);
            CHECK(x * y == prod);
            CHECK(y * x == prod);
            CHECK(prod / x == y);
            CHECK(prod / y == x);
        }
    }

    SECTION("Div by zero") {
        auto zero = Decimal();
        auto x = Decimal(5);
        CHECK_THROWS_AS(x / zero, DivisionByZero);
    }
}
