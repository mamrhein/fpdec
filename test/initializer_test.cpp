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

#include <vector>

#include "catch.hpp"
#include "fpdec.h"


bool
is_shint(fpdec_t *fpdec) {
    return (fpdec->dyn_alloc == 0 && fpdec->normalized == 0);
}

bool
is_digit_array(fpdec_t *fpdec) {
    return (fpdec->dyn_alloc == 1 && fpdec->normalized == 1);
}

bool check_normalized(fpdec_t *fpdec) {
    fpdec_digit_array_t *digit_array = fpdec->digit_array;
    fpdec_n_digits_t n_signif = digit_array->n_signif;
    return n_signif == 0 ||
           (digit_array->digits[0] != 0 &&
            digit_array->digits[n_signif] != 0);
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
            fpdec_t fpdec;
            const char *literal = test.literal.c_str();
            int rc = fpdec_from_ascii_literal(&fpdec, literal);

            SECTION(literal) {
                REQUIRE(rc == 0);
                REQUIRE(is_shint(&fpdec));
                REQUIRE(FPDEC_SIGN(&fpdec) == test.sign);
                REQUIRE(FPDEC_DEC_PREC(&fpdec) == test.dec_prec);
                REQUIRE(fpdec.lo == test.digits[0]);
                REQUIRE(fpdec.hi == test.digits[1]);
            }
        }
    }

    SECTION("Coeff > MAX_N_DEC_DIGITS_IN_SHINT") {

    }

    SECTION("Invalid input") {
        std::string literals[7] = {
                " 1.23.5", "1.24e", "--4.92", "", "   ", "3,49E-3",
                "\t+   \r\n"
        };
        for (const auto &literal : literals) {
            fpdec_t fpdec;

            SECTION(literal) {
                REQUIRE(fpdec_from_ascii_literal(&fpdec, literal.c_str()) ==
                        FPDEC_INVALID_DECIMAL_LITERAL);
            }
        }
    }
}