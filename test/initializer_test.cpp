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
#include "checks.hpp"


TEST_CASE("Initialize from ascii literal") {

    struct test_data {
        std::string literal;
        fpdec_sign_t sign;
        fpdec_dec_prec_t dec_prec;
        fpdec_exp_t exp;
        fpdec_n_digits_t n_digits;
        std::vector<fpdec_digit_t> digits;
    };

    SECTION("Length of coeff <= MAX_N_DEC_DIGITS_IN_SHINT and "
            "(dec_prec <= MAX_DEC_PREC_FOR_SHINT or result == 0)") {
        struct test_data tests[] = {
                {
                        .literal = "  000000000000001926.83 \n",
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
                        .literal = "-12345678901234567890e-7",
                        .sign = -1,
                        .dec_prec = 7,
                        .digits = {12345678901234567890UL, 0UL}
                },
                {
                        .literal = "82345678901234567890e-9",
                        .sign = 1,
                        .dec_prec = 9,
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
                        .literal = "-000000e5",
                        .sign = 0,
                        .dec_prec = 0,
                        .digits = {0UL, 0UL}
                },
                {
                        .literal = "-000000e-10",
                        .sign = 0,
                        .dec_prec = 10,
                        .digits = {0UL, 0UL}
                },
                {
                    .literal = "1235931852938539958205",
                    .sign = 1,
                    .dec_prec = 0,
                    .digits = {18446744073709551549UL, 66UL}
                },
                {
                    .literal = "313594649253062377472",
                    .sign = 1,
                    .dec_prec = 0,
                    .digits = {0UL, 17UL}
                },
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

    SECTION("Dec_prec > MAX_DEC_PREC_FOR_SHINT") {
        struct test_data tests[] = {
                {
                        .literal = "+5.387E-17",
                        .sign = 1,
                        .dec_prec = 20,
                        .exp = -2,
                        .n_digits = 2,
                        .digits = {7000000000000000000UL, 538UL}
                },
                {
                        .literal = "82345678901234567890e-12",
                        .sign = 1,
                        .dec_prec = 12,
                        .exp = -1,
                        .n_digits = 2,
                        .digits = {9012345678900000000UL, 82345678UL}
                },
                {
                        .literal = "1e459",
                        .sign = 1,
                        .dec_prec = 0,
                        .exp = 24,
                        .n_digits = 1,
                        .digits = {1000UL}
                },
                {
                        .literal = "-3.0e-9",
                        .sign = -1,
                        .dec_prec = 10,
                        .exp = -1,
                        .n_digits = 1,
                        .digits = {30000000000UL}
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
                CHECK(FPDEC_DYN_EXP(&fpdec) == test.exp);
                REQUIRE(FPDEC_DYN_N_DIGITS(&fpdec) == test.n_digits);
                for (int i = 0; i < test.n_digits; ++i) {
                    CHECK(fpdec.digit_array->digits[i] == test.digits[i]);
                }
            }
            fpdec_reset_to_zero(&fpdec, 0);
        }
    }

    SECTION("Length of coeff > MAX_N_DEC_DIGITS_IN_SHINT") {
        struct test_data tests[] = {
                {
                        .literal = "  0001926.837209e26",
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
                        .literal = "0.0000000000000000000000000000000005",
                        .sign = 1,
                        .dec_prec = 34,
                        .exp = -2,
                        .n_digits = 1,
                        .digits = {50000UL}
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
                },
                {
                        .literal =
                        "-53095100000000000000000000000000000000000000"
                        ".00",
                        .sign = -1,
                        .dec_prec = 2,
                        .exp = 2,
                        .n_digits = 1,
                        .digits = {530951UL}
                },
                {
                        .literal = "792281625142643375935439504.11",
                        .sign = 1,
                        .dec_prec = 2,
                        .exp = -1,
                        .n_digits = 3,
                        .digits = {1100000000000000000UL,
                                   5142643375935439504UL,
                                   79228162UL}
                },
                {
                    .literal = "1e20401094656",
                    .sign = 1,
                    .dec_prec = 0,
                    .exp = 1073741824,
                    .n_digits = 1,
                    .digits = {1UL}
                },
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
                CHECK(FPDEC_DYN_EXP(&fpdec) == test.exp);
                REQUIRE(FPDEC_DYN_N_DIGITS(&fpdec) == test.n_digits);
                for (int i = 0; i < test.n_digits; ++i) {
                    CHECK(fpdec.digit_array->digits[i] == test.digits[i]);
                }
            }
            fpdec_reset_to_zero(&fpdec, 0);
        }
    }

    SECTION("Invalid ascii input") {
        std::string literals[] = {
                " 1.23.5", "1.24e", "--4.92", "", "   ", "3,49E-3",
                "\t+   \r\n"
        };
        for (const auto &literal : literals) {
            fpdec_t fpdec = FPDEC_ZERO;
            std::string name;
            if (literal.empty())
                name = "empty";
            else if (literal == "   ")
                name = "blank";
            else
                name = literal;

            SECTION(name) {
                CHECK(fpdec_from_ascii_literal(&fpdec, literal.c_str()) ==
                              FPDEC_INVALID_DECIMAL_LITERAL);
            }
        }
    }

    SECTION("Exponent limit exceeded") {
        std::string literals[] = {
            " 1.e40802189294", "4.9200835e40802189301",
        };
        for (const auto &literal : literals) {
            fpdec_t fpdec = FPDEC_ZERO;

            SECTION(literal) {
                CHECK(fpdec_from_ascii_literal(&fpdec, literal.c_str()) ==
                          FPDEC_EXP_LIMIT_EXCEEDED);
            }
        }
    }

    SECTION("Precision limit exceeded") {
        std::string literals[] = {
            "1e-65536", "4.9200835880e-65526",
        };
        for (const auto &literal : literals) {
            fpdec_t fpdec = FPDEC_ZERO;

            SECTION(literal) {
                CHECK(fpdec_from_ascii_literal(&fpdec, literal.c_str()) ==
                          FPDEC_PREC_LIMIT_EXCEEDED);
            }
        }
    }
}

TEST_CASE("Initialize from unicode literal") {

    int offsets[] = {0x000006F0, 0x0000A8D0, 0x00011066, 0x0001E950};
    const fpdec_digit_t abs_val = 9876543210;
    char buf[30];
    fpdec_sign_t sign;
    wchar_t *literal;
    fpdec_t fpdec = FPDEC_ZERO;
    int r;
    error_t rc;

    for (const auto offset : offsets) {
        snprintf((char *) (&buf), 30, "0x%08X", offset);
        r = offset % 7;
        sign = r < 4 ? FPDEC_SIGN_NEG : FPDEC_SIGN_POS;
        literal = (wchar_t *) fpdec_mem_alloc(r + 12, sizeof(wchar_t));
        for (int i = 0; i < r; ++i)
            literal[i] = ' ';
        literal[r] = sign < 0 ? '-' : '+';
        r++;
        for (int i = 0; i < 10; ++i)
            literal[i + r] = offset + 9 - i;
        rc = fpdec_from_unicode_literal(&fpdec, literal);

        SECTION(buf) {
            REQUIRE(rc == FPDEC_OK);
            CHECK(is_shint(&fpdec));
            CHECK(FPDEC_SIGN(&fpdec) == sign);
            CHECK(FPDEC_DEC_PREC(&fpdec) == 0);
            CHECK(fpdec.lo == abs_val);
            CHECK(fpdec.hi == 0);
        }
        fpdec_mem_free(literal);
        fpdec_reset_to_zero(&fpdec, 0);
    }
}

TEST_CASE("Initialize from long long.") {

    long long test_vals[] = {INT64_MIN, -290382, 0, INT64_MAX};
    char buf[30];

    for (const auto test_val : test_vals) {
        snprintf((char *) (&buf), 30, "%lld", test_val);
        fpdec_t *fpdec = fpdec_new();
        assert(fpdec != NULL);
        error_t rc = fpdec_from_long_long(fpdec, test_val);
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
            CHECK(is_shint(fpdec));
            CHECK(FPDEC_SIGN(fpdec) == sign);
            CHECK(FPDEC_DEC_PREC(fpdec) == 0);
            CHECK(fpdec->lo == abs_val);
            CHECK(fpdec->hi == 0);
        }
        fpdec_mem_free(fpdec);
    }
}

TEST_CASE("Initialize from sign, digits, exp.") {

    struct test_data {
        fpdec_sign_t sign;
        size_t n_digits;
        const fpdec_digit_t digits[7];
        fpdec_exp_t exp;
        bool res_is_dyn_alloc;
        size_t res_n_digits;
        fpdec_digit_t res_digits[7];
        fpdec_exp_t res_exp;
    };

    struct test_data test_vals[] = {
            {FPDEC_SIGN_POS, 1, {823,},                       0,  false, 0, {823,},                    0},
            {FPDEC_SIGN_POS, 5, {27, 5,     0,   0, 0,},      0,  true,  2, {27,    5,},               0},
            {FPDEC_SIGN_NEG, 5, {0,  76629, 0,   0, 0,},      -1, false, 0, {76629,},                  0},
            {FPDEC_SIGN_NEG, 5, {0,  76629, 333, 0, 0,},      -4,
                                                                  true,  2, {76629, 333,},             -3},
            {FPDEC_SIGN_NEG, 7, {1,  2,     3,   4, 5, 6, 7}, -3,
                                                                  true,  7, {1,     2, 3, 4, 5, 6, 7}, -3},
    };
    char buf[30];

    for (struct test_data test_val : test_vals) {
        snprintf((char *) (&buf), 30, "%c %zu: %lu ...",
                 test_val.sign < 0 ? '-' : '+',
                 test_val.res_n_digits, test_val.res_digits[0]);
        fpdec_t *fpdec = fpdec_new();
        assert(fpdec != NULL);
        error_t rc = fpdec_from_sign_digits_exp(fpdec, test_val.sign,
                                                test_val.n_digits,
                                                test_val.digits,
                                                test_val.exp);

        SECTION(buf) {
            REQUIRE(rc == FPDEC_OK);
            CHECK(FPDEC_SIGN(fpdec) == test_val.sign);
            if (test_val.res_is_dyn_alloc) {
                CHECK(is_digit_array(fpdec));
                CHECK(FPDEC_DYN_EXP(fpdec) == test_val.res_exp);
                if (test_val.res_exp >= 0)
                    CHECK(FPDEC_DEC_PREC(fpdec) == 0);
                else
                    CHECK(FPDEC_DEC_PREC(fpdec) ==
                                  -test_val.res_exp * DEC_DIGITS_PER_DIGIT);
                CHECK(FPDEC_DYN_N_DIGITS(fpdec) == test_val.res_n_digits);
                for (int idx = 0; idx > test_val.res_n_digits; ++idx)
                    CHECK(FPDEC_DYN_DIGITS(fpdec)[idx] ==
                                  test_val.res_digits[idx]);
            }
            else {
                CHECK(is_shint(fpdec));
                CHECK(FPDEC_DEC_PREC(fpdec) == 0);
                CHECK(fpdec->lo == test_val.res_digits[0]);
                CHECK(fpdec->hi == 0);
            }
        }
        fpdec_reset_to_zero(fpdec, 0);
        fpdec_mem_free(fpdec);
        fpdec = NULL;
    }
}
