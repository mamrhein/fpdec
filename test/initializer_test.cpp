/* ---------------------------------------------------------------------------
Name:        initializer_test.cpp

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application. For license
             details please read the file LICENSE.TXT provided together
             with the application.
------------------------------------------------------------------------------
$Source$
$Revision$
*/

#include <cstdio>
#include <vector>

#include "catch.hpp"
#include "fpdec.h"


bool
is_shint(fpdec_t *fpdec) {
    return !(fpdec->dyn_alloc && fpdec->normalized);
}

bool
is_digit_array(fpdec_t *fpdec) {
    return (fpdec->dyn_alloc && fpdec->normalized);
}

bool check_normalized(fpdec_t *fpdec) {
    fpdec_digit_array_t *digit_array = fpdec->digit_array;
    fpdec_n_digits_t n_signif = digit_array->n_signif;
    return n_signif == 0 ||
           (digit_array->digits[0] != 0 &&
            digit_array->digits[n_signif - 1] != 0);
}

TEST_CASE("Initialize from string") {

    struct test_data {
        std::string literal;
        fpdec_sign_t sign;
        fpdec_dec_prec_t dec_prec;
        fpdec_exp_t exp;
        fpdec_n_digits_t n_digits;
        std::vector<fpdec_digit_t> digits;
    };

    SECTION("Coeff <= MAX_N_DEC_DIGITS_IN_SHINT") {
        struct test_data tests[8] = {
            {
                .literal = "  1926.83 \n",
                .sign = 1,
                .dec_prec = 2,
                .digits = {192683UL, 0UL}
            },
            {
                .literal = "+5.387E+1",
                .sign = 1,
                .dec_prec = 2,
                .digits = {5387UL, 0UL}
            },
            {
                .literal = "+5.387E-17",
                .sign = 1,
                .dec_prec = 20,
                .digits = {5387UL, 0UL}
            },
            {
                .literal = "-12345678901234567890e-7",
                .sign = -1,
                .dec_prec = 7,
                .digits = {12345678901234567890UL, 0UL}
            },
            {
                .literal = "82345678901234567890e-12",
                .sign = 1,
                .dec_prec = 12,
                .digits = {8558702606396361426UL, 4UL}
            },
            {
                .literal = "+007e28",
                .sign = 1,
                .dec_prec = 0,
                .digits = {12899172069043863552UL, 3794707603UL}
            },
            {
                .literal = "   +0.00e-0",
                .sign = 0,
                .dec_prec = 2,
                .digits = {0UL, 0UL}
            },
            {
                .literal = "-000000",
                .sign = 0,
                .dec_prec = 0,
                .digits = {0UL, 0UL}
            }
        };

        for (const auto &test : tests) {
            fpdec_t fpdec = FPDEC_ZERO;
            const char *literal = test.literal.c_str();
            error_t rc = fpdec_from_ascii_literal(&fpdec, literal);

            SECTION(test.literal) {
                REQUIRE(rc == FPDEC_OK);
                CHECK(is_shint(&fpdec));
                CHECK(FPDEC_SIGN(&fpdec) == test.sign);
                CHECK(FPDEC_DEC_PREC(&fpdec) == test.dec_prec);
                CHECK(fpdec.lo == test.digits[0]);
                CHECK(fpdec.hi == test.digits[1]);
            }
        }
    }

    SECTION("Coeff > MAX_N_DEC_DIGITS_IN_SHINT") {
        struct test_data tests[3] = {
            {
                .literal = "  1926.837209e26",
                .sign = 1,
                .dec_prec = 0,
                .exp = 1,
                .n_digits = 1,
                .digits = {19268372090UL}
            },
            {
                .literal = "-11702439610000.0000002938162540",
                .sign = -1,
                .dec_prec = 16,
                .exp = -1,
                .n_digits = 2,
                .digits = {2938162540000UL, 11702439610000UL}
            },
            {
                .literal = "1111111111111111111"
                           "2222222222222222222"
                           "3333333333333333333"
                           "4444444444444444444"
                           "5555555555555555555"
                           "."
                           "6666666666666666666"
                           "7777777777777777777"
                           "888",
                .sign = 1,
                .dec_prec = 41,
                .exp = -3,
                .n_digits = 8,
                .digits = {8880000000000000000UL,
                           7777777777777777777UL,
                           6666666666666666666UL,
                           5555555555555555555UL,
                           4444444444444444444UL,
                           3333333333333333333UL,
                           2222222222222222222UL,
                           1111111111111111111UL}
            }
        };

        for (const auto &test : tests) {
            fpdec_t fpdec = FPDEC_ZERO;
            const char *literal = test.literal.c_str();
            error_t rc = fpdec_from_ascii_literal(&fpdec, literal);

            SECTION(test.literal) {
                REQUIRE(rc == FPDEC_OK);
                REQUIRE(is_digit_array(&fpdec));
                CHECK(check_normalized(&fpdec));
                CHECK(FPDEC_SIGN(&fpdec) == test.sign);
                CHECK(FPDEC_DEC_PREC(&fpdec) == test.dec_prec);
                CHECK(FPDEC_EXP(&fpdec) == test.exp);
                REQUIRE(FPDEC_N_DIGITS(&fpdec) == test.n_digits);
                for (int i = 0; i < test.n_digits; ++i) {
                    CHECK(fpdec.digit_array->digits[i] == test.digits[i]);
                }
            }
            fpdec_dealloc(&fpdec);
        }
    }

    SECTION("Invalid input") {
        std::string literals[7] = {
            " 1.23.5", "1.24e", "--4.92", "", "   ", "3,49E-3",
            "\t+   \r\n"
        };
        for (const auto &literal : literals) {
            fpdec_t fpdec = FPDEC_ZERO;

            SECTION(literal) {
                CHECK(fpdec_from_ascii_literal(&fpdec, literal.c_str()) ==
                        FPDEC_INVALID_DECIMAL_LITERAL);
            }
        }
    }
}

TEST_CASE("Initialize from long long.") {

    long long test_vals[4] = {INT64_MIN, -290382, 0, INT64_MAX};
    char buf[30];

    for (long long test_val : test_vals) {
        snprintf((char *) (&buf), 30, "%lld", test_val);
        fpdec_t fpdec = FPDEC_ZERO;
        error_t rc = fpdec_from_long_long(&fpdec, test_val);
        long long abs_val = std::abs(test_val);
        fpdec_sign_t sign;

        if (test_val == 0)
            sign = FPDEC_SIGN_ZERO;
        else if (test_val > 0)
            sign = FPDEC_SIGN_POS;
        else
            sign = FPDEC_SIGN_NEG;

        SECTION(buf) {
            REQUIRE(rc == FPDEC_OK);
            CHECK(is_shint(&fpdec));
            CHECK(FPDEC_SIGN(&fpdec) == sign);
            CHECK(FPDEC_DEC_PREC(&fpdec) == 0);
            CHECK(fpdec.lo == abs_val);
            CHECK(fpdec.hi == 0);
        }
    }
}
