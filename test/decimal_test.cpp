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
#include "fpdec.hpp"

using namespace fpdec;

TEST_CASE("Default constructor / destructor") {
    Decimal d;
    Decimal *ptr2d;
    for (int i = 0; i <= 10; i++) {
        d = Decimal();
        ptr2d = &d;
        CHECK(ptr2d != NULL);
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
    return (int)std::log10(std::abs((int64_t) num));
}

TEST_CASE("Decimal from integer") {

    for (const auto &val : {INT16_MIN, -14, 0, 328, INT16_MAX}) {

        SECTION(std::to_string(val)) {
            Decimal d = {val};
            CHECK(d.sign() == sign(val));
            CHECK(d.precision() == 0);
            if (val != 0)
                CHECK(d.magnitude() == magn(val));
        }
    }

    for (const auto &val : {INT32_MIN, -143, 1, 320937, INT32_MAX}) {

        SECTION(std::to_string(val)) {
            Decimal d = {val};
            CHECK(d.sign() == sign(val));
            CHECK(d.precision() == 0);
            CHECK(d.magnitude() == magn(val));
        }
    }

    for (const auto &val : {INT64_MIN, -143L, 1L, 640937L, INT64_MAX}) {

        SECTION(std::to_string(val)) {
            Decimal d = {val};
            CHECK(d.sign() == sign(val));
            CHECK(d.precision() == 0);
            CHECK(d.magnitude() == magn(val));
        }
    }
}
