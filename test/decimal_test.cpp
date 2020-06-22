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
            if (test.magnitude != -1)
                CHECK(d.magnitude() == test.magnitude);
        }
    }
}
