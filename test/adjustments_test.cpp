/* ---------------------------------------------------------------------------
Name:        adjustments_test.cpp

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
#include "checks.hpp"

void
check_adjusted_shint(FPDEC_ROUNDING_MODE rnd, const char *literal,
                     const int32_t dec_prec,
                     const std::vector<fpdec_digit_t> &digits);

void
check_adjusted_digit_array(enum FPDEC_ROUNDING_MODE rnd, const char *literal,
                           const int32_t dec_prec, const fpdec_exp_t exp,
                           const std::vector<fpdec_digit_t> &digits);

TEST_CASE("Adjust presision") {

    struct test_data {
        std::string literal;
        int32_t dec_prec;
        fpdec_exp_t exp;
        std::vector<fpdec_digit_t> digits;
    };

    SECTION("Shifted int variant") {

        SECTION("Default rounding") {
            struct test_data tests[] = {
                {
                    .literal = "-1926.83",
                    .dec_prec = 4,
                    .digits = {19268300UL, 0UL}
                },
                {
                    .literal = "1926.83",
                    .dec_prec = 2,
                    .digits = {192683UL, 0UL}
                },
                {
                    .literal = "1926.8396",
                    .dec_prec = 1,
                    .digits = {19268UL, 0UL}
                },
                {
                    .literal = "1926.837",
                    .dec_prec = 2,
                    .digits = {192684UL, 0UL}
                },
                {
                    .literal = "0.00015",
                    .dec_prec = 4,
                    .digits = {2UL, 0UL}
                },
                {
                    .literal = "-0.00025",
                    .dec_prec = 4,
                    .digits = {2UL, 0UL}
                },
                {
                    .literal = "0.0001594",
                    .dec_prec = 3,
                    .digits = {0UL, 0UL}
                },
                {
                    .literal = "999999999999999999.9999999",
                    .dec_prec = 5,
                    .digits = {200376420520689664UL, 5421UL}
                },
                {
                    .literal = "-0.00000",
                    .dec_prec = 15,
                    .digits = {0UL, 0UL}
                },
                {
                    .literal = "-155.90",
                    .dec_prec = -1,
                    .digits = {160UL, 0UL}
                },
                {
                    .literal = "1234567890123456789012345.6789",
                    .dec_prec = -20,
                    .digits = {14759378840838995968UL, 66927UL}
                },
                {
                    .literal = "-9e27",
                    .dec_prec = -28,
                    .digits = {4477988020393345024UL, 542101086UL}
                },
                {
                    .literal = "-6e28",
                    .dec_prec = -30,
                    .digits = {0UL, 0UL}
                },
            };

            for (const auto &test : tests) {
                const char *literal = test.literal.c_str();
                SECTION(test.literal) {
                    check_adjusted_shint(FPDEC_ROUND_DEFAULT, literal,
                                         test.dec_prec, test.digits);
                }
            }
        }

        SECTION("Round 05 up") {
            struct test_data tests[8] = {
                {
                    .literal = "-1926.83",
                    .dec_prec = 4,
                    .digits = {19268300UL, 0UL}
                },
                {
                    .literal = "1926.83",
                    .dec_prec = 2,
                    .digits = {192683UL, 0UL}
                },
                {
                    .literal = "1926.8396",
                    .dec_prec = 1,
                    .digits = {19268UL, 0UL}
                },
                {
                    .literal = "1926.837",
                    .dec_prec = 2,
                    .digits = {192683UL, 0UL}
                },
                {
                    .literal = "0.00015",
                    .dec_prec = 4,
                    .digits = {1UL, 0UL}
                },
                {
                    .literal = "-0.00025",
                    .dec_prec = 4,
                    .digits = {2UL, 0UL}
                },
                {
                    .literal = "0.0001594",
                    .dec_prec = 3,
                    .digits = {1UL, 0UL}
                },
                {
                    .literal = "999999999999999999.9999999",
                    .dec_prec = 5,
                    .digits = {200376420520689663UL, 5421UL}
                },
            };

            for (const auto &test : tests) {
                const char *literal = test.literal.c_str();
                SECTION(test.literal) {
                    check_adjusted_shint(FPDEC_ROUND_05UP, literal,
                                         test.dec_prec, test.digits);
                }
            }
        }

        SECTION("Round ceiling") {
            struct test_data tests[8] = {
                {
                    .literal = "-1926.83",
                    .dec_prec = 4,
                    .digits = {19268300UL, 0UL}
                },
                {
                    .literal = "1926.83",
                    .dec_prec = 2,
                    .digits = {192683UL, 0UL}
                },
                {
                    .literal = "1926.8396",
                    .dec_prec = 1,
                    .digits = {19269UL, 0UL}
                },
                {
                    .literal = "1926.837",
                    .dec_prec = 2,
                    .digits = {192684UL, 0UL}
                },
                {
                    .literal = "0.00015",
                    .dec_prec = 4,
                    .digits = {2UL, 0UL}
                },
                {
                    .literal = "-0.00025",
                    .dec_prec = 4,
                    .digits = {2UL, 0UL}
                },
                {
                    .literal = "0.0001594",
                    .dec_prec = 3,
                    .digits = {1UL, 0UL}
                },
                {
                    .literal = "999999999999999999.9999999",
                    .dec_prec = 5,
                    .digits = {200376420520689664UL, 5421UL}
                },
            };

            for (const auto &test : tests) {
                const char *literal = test.literal.c_str();
                SECTION(test.literal) {
                    check_adjusted_shint(FPDEC_ROUND_CEILING, literal,
                                         test.dec_prec, test.digits);
                }
            }
        }

        SECTION("Round down") {
            struct test_data tests[8] = {
                {
                    .literal = "-1926.83",
                    .dec_prec = 4,
                    .digits = {19268300UL, 0UL}
                },
                {
                    .literal = "1926.83",
                    .dec_prec = 2,
                    .digits = {192683UL, 0UL}
                },
                {
                    .literal = "1926.8396",
                    .dec_prec = 1,
                    .digits = {19268UL, 0UL}
                },
                {
                    .literal = "1926.837",
                    .dec_prec = 2,
                    .digits = {192683UL, 0UL}
                },
                {
                    .literal = "0.00015",
                    .dec_prec = 4,
                    .digits = {1UL, 0UL}
                },
                {
                    .literal = "-0.00025",
                    .dec_prec = 4,
                    .digits = {2UL, 0UL}
                },
                {
                    .literal = "0.0001594",
                    .dec_prec = 3,
                    .digits = {0UL, 0UL}
                },
                {
                    .literal = "999999999999999999.9999999",
                    .dec_prec = 5,
                    .digits = {200376420520689663UL, 5421UL}
                },
            };

            for (const auto &test : tests) {
                const char *literal = test.literal.c_str();
                SECTION(test.literal) {
                    check_adjusted_shint(FPDEC_ROUND_DOWN, literal,
                                         test.dec_prec, test.digits);
                }
            }
        }

        SECTION("Round floor") {
            struct test_data tests[8] = {
                {
                    .literal = "-1926.83",
                    .dec_prec = 4,
                    .digits = {19268300UL, 0UL}
                },
                {
                    .literal = "1926.83",
                    .dec_prec = 2,
                    .digits = {192683UL, 0UL}
                },
                {
                    .literal = "1926.8396",
                    .dec_prec = 1,
                    .digits = {19268UL, 0UL}
                },
                {
                    .literal = "1926.837",
                    .dec_prec = 2,
                    .digits = {192683UL, 0UL}
                },
                {
                    .literal = "0.00015",
                    .dec_prec = 4,
                    .digits = {1UL, 0UL}
                },
                {
                    .literal = "-0.00025",
                    .dec_prec = 4,
                    .digits = {3UL, 0UL}
                },
                {
                    .literal = "0.0001594",
                    .dec_prec = 3,
                    .digits = {0UL, 0UL}
                },
                {
                    .literal = "999999999999999999.9999999",
                    .dec_prec = 5,
                    .digits = {200376420520689663UL, 5421UL}
                },
            };

            for (const auto &test : tests) {
                const char *literal = test.literal.c_str();
                SECTION(test.literal) {
                    check_adjusted_shint(FPDEC_ROUND_FLOOR, literal,
                                         test.dec_prec, test.digits);
                }
            }
        }

        SECTION("Round half down") {
            struct test_data tests[8] = {
                {
                    .literal = "-1926.83",
                    .dec_prec = 4,
                    .digits = {19268300UL, 0UL}
                },
                {
                    .literal = "1926.83",
                    .dec_prec = 2,
                    .digits = {192683UL, 0UL}
                },
                {
                    .literal = "1926.8396",
                    .dec_prec = 1,
                    .digits = {19268UL, 0UL}
                },
                {
                    .literal = "1926.837",
                    .dec_prec = 2,
                    .digits = {192684UL, 0UL}
                },
                {
                    .literal = "0.00015",
                    .dec_prec = 4,
                    .digits = {1UL, 0UL}
                },
                {
                    .literal = "-0.00025",
                    .dec_prec = 4,
                    .digits = {2UL, 0UL}
                },
                {
                    .literal = "0.0001594",
                    .dec_prec = 3,
                    .digits = {0UL, 0UL}
                },
                {
                    .literal = "999999999999999999.9999999",
                    .dec_prec = 5,
                    .digits = {200376420520689664UL, 5421UL}
                },
            };

            for (const auto &test : tests) {
                const char *literal = test.literal.c_str();
                SECTION(test.literal) {
                    check_adjusted_shint(FPDEC_ROUND_HALF_DOWN, literal,
                                         test.dec_prec, test.digits);
                }
            }
        }

        SECTION("Round half even") {
            struct test_data tests[8] = {
                {
                    .literal = "-1926.83",
                    .dec_prec = 4,
                    .digits = {19268300UL, 0UL}
                },
                {
                    .literal = "1926.83",
                    .dec_prec = 2,
                    .digits = {192683UL, 0UL}
                },
                {
                    .literal = "1926.8396",
                    .dec_prec = 1,
                    .digits = {19268UL, 0UL}
                },
                {
                    .literal = "1926.837",
                    .dec_prec = 2,
                    .digits = {192684UL, 0UL}
                },
                {
                    .literal = "0.00015",
                    .dec_prec = 4,
                    .digits = {2UL, 0UL}
                },
                {
                    .literal = "-0.00025",
                    .dec_prec = 4,
                    .digits = {2UL, 0UL}
                },
                {
                    .literal = "0.0001594",
                    .dec_prec = 3,
                    .digits = {0UL, 0UL}
                },
                {
                    .literal = "999999999999999999.9999999",
                    .dec_prec = 5,
                    .digits = {200376420520689664UL, 5421UL}
                },
            };

            for (const auto &test : tests) {
                const char *literal = test.literal.c_str();
                SECTION(test.literal) {
                    check_adjusted_shint(FPDEC_ROUND_HALF_EVEN, literal,
                                         test.dec_prec, test.digits);
                }
            }
        }

        SECTION("Round half up") {
            struct test_data tests[8] = {
                {
                    .literal = "-1926.83",
                    .dec_prec = 4,
                    .digits = {19268300UL, 0UL}
                },
                {
                    .literal = "1926.83",
                    .dec_prec = 2,
                    .digits = {192683UL, 0UL}
                },
                {
                    .literal = "1926.8396",
                    .dec_prec = 1,
                    .digits = {19268UL, 0UL}
                },
                {
                    .literal = "1926.837",
                    .dec_prec = 2,
                    .digits = {192684UL, 0UL}
                },
                {
                    .literal = "0.00015",
                    .dec_prec = 4,
                    .digits = {2UL, 0UL}
                },
                {
                    .literal = "-0.00025",
                    .dec_prec = 4,
                    .digits = {3UL, 0UL}
                },
                {
                    .literal = "0.0001594",
                    .dec_prec = 3,
                    .digits = {0UL, 0UL}
                },
                {
                    .literal = "999999999999999999.9999999",
                    .dec_prec = 5,
                    .digits = {200376420520689664UL, 5421UL}
                },
            };

            for (const auto &test : tests) {
                const char *literal = test.literal.c_str();
                SECTION(test.literal) {
                    check_adjusted_shint(FPDEC_ROUND_HALF_UP, literal,
                                         test.dec_prec, test.digits);
                }
            }
        }

        SECTION("Round up") {
            struct test_data tests[8] = {
                {
                    .literal = "-1926.83",
                    .dec_prec = 4,
                    .digits = {19268300UL, 0UL}
                },
                {
                    .literal = "1926.83",
                    .dec_prec = 2,
                    .digits = {192683UL, 0UL}
                },
                {
                    .literal = "1926.8396",
                    .dec_prec = 1,
                    .digits = {19269UL, 0UL}
                },
                {
                    .literal = "1926.837",
                    .dec_prec = 2,
                    .digits = {192684UL, 0UL}
                },
                {
                    .literal = "0.00015",
                    .dec_prec = 4,
                    .digits = {2UL, 0UL}
                },
                {
                    .literal = "-0.00025",
                    .dec_prec = 4,
                    .digits = {3UL, 0UL}
                },
                {
                    .literal = "0.0001594",
                    .dec_prec = 3,
                    .digits = {1UL, 0UL}
                },
                {
                    .literal = "999999999999999999.9999999",
                    .dec_prec = 5,
                    .digits = {200376420520689664UL, 5421UL}
                },
            };

            for (const auto &test : tests) {
                const char *literal = test.literal.c_str();
                SECTION(test.literal) {
                    check_adjusted_shint(FPDEC_ROUND_UP, literal,
                                         test.dec_prec, test.digits);
                }
            }
        }

    } // end SECTION("Shifted int variant")

    SECTION("Digit array variant") {

        SECTION("Default rounding") {
            struct test_data tests[] = {
                {
                    .literal =
                    "34028236692093846346337460743176.8211551",
                    .dec_prec = 12,
                    .exp = -1,
                    .digits = {8211551000000000000UL,
                               3846346337460743176UL,
                               3402823669209}
                },
                {
                    .literal =
                    "34028236692093846346337460743176.8211552",
                    .dec_prec = 7,
                    .exp = -1,
                    .digits = {8211552000000000000UL,
                               3846346337460743176UL,
                               3402823669209}
                },
                {
                    .literal =
                    "8402823669209384634633.74607431768211553",
                    .dec_prec = 7,
                    .exp = -1,
                    .digits = {7460743000000000000UL,
                               2823669209384634633UL,
                               840}
                },
                {
                    .literal =
                    "-34028236692093.8563463374607431768211554",
                    .dec_prec = 11,
                    .exp = -1,
                    .digits = {8563463374600000000UL,
                               34028236692093UL}
                },
                {
                    .literal =
                    "99999999999999999999999999999999999999.999995",
                    .dec_prec = 5,
                    .exp = 2,
                    .digits = {1UL}
                },
            };

            for (const auto &test : tests) {
                const char *literal = test.literal.c_str();
                SECTION(test.literal) {
                    check_adjusted_digit_array(FPDEC_ROUND_DEFAULT, literal,
                                               test.dec_prec, test.exp,
                                               test.digits);
                }
            }
        }

        SECTION("Round 05 up") {
            struct test_data tests[] = {
                {
                    .literal =
                    "34028236692093846346337460743176.8211551",
                    .dec_prec = 12,
                    .exp = -1,
                    .digits = {8211551000000000000UL,
                               3846346337460743176UL,
                               3402823669209}
                },
                {
                    .literal =
                    "34028236692093846346337460743176.8211552",
                    .dec_prec = 7,
                    .exp = -1,
                    .digits = {8211552000000000000UL,
                               3846346337460743176UL,
                               3402823669209}
                },
                {
                    .literal =
                    "8402823669209384634633.74607431768211553",
                    .dec_prec = 7,
                    .exp = -1,
                    .digits = {7460743000000000000UL,
                               2823669209384634633UL,
                               840}
                },
                {
                    .literal =
                    "-34028236692093.8563463374607431768211554",
                    .dec_prec = 11,
                    .exp = -1,
                    .digits = {8563463374600000000UL,
                               34028236692093UL}
                },
                {
                    .literal =
                    "99999999999999999999999999999999999999.999995",
                    .dec_prec = 5,
                    .exp = -1,
                    .digits = {9999900000000000000UL,
                               9999999999999999999UL,
                               9999999999999999999UL}
                },
            };

            for (const auto &test : tests) {
                const char *literal = test.literal.c_str();
                SECTION(test.literal) {
                    check_adjusted_digit_array(FPDEC_ROUND_05UP,
                                               literal,
                                               test.dec_prec, test.exp,
                                               test.digits);
                }
            }
        }

        SECTION("Round ceiling") {
            struct test_data tests[] = {
                {
                    .literal =
                    "34028236692093846346337460743176.8211551",
                    .dec_prec = 12,
                    .exp = -1,
                    .digits = {8211551000000000000UL,
                               3846346337460743176UL,
                               3402823669209}
                },
                {
                    .literal =
                    "34028236692093846346337460743176.8211552",
                    .dec_prec = 7,
                    .exp = -1,
                    .digits = {8211552000000000000UL,
                               3846346337460743176UL,
                               3402823669209}
                },
                {
                    .literal =
                    "8402823669209384634633.74607431768211553",
                    .dec_prec = 7,
                    .exp = -1,
                    .digits = {7460744000000000000UL,
                               2823669209384634633UL,
                               840}
                },
                {
                    .literal =
                    "-34028236692093.8563463374607431768211554",
                    .dec_prec = 11,
                    .exp = -1,
                    .digits = {8563463374600000000UL,
                               34028236692093UL}
                },
                {
                    .literal =
                    "99999999999999999999999999999999999999.999991",
                    .dec_prec = 5,
                    .exp = 2,
                    .digits = {1UL}
                },
            };

            for (const auto &test : tests) {
                const char *literal = test.literal.c_str();
                SECTION(test.literal) {
                    check_adjusted_digit_array(FPDEC_ROUND_CEILING, literal,
                                               test.dec_prec, test.exp,
                                               test.digits);
                }
            }
        }

        SECTION("Round down") {
            struct test_data tests[] = {
                {
                    .literal =
                    "34028236692093846346337460743176.8211551",
                    .dec_prec = 12,
                    .exp = -1,
                    .digits = {8211551000000000000UL,
                               3846346337460743176UL,
                               3402823669209}
                },
                {
                    .literal =
                    "34028236692093846346337460743176.8211552",
                    .dec_prec = 7,
                    .exp = -1,
                    .digits = {8211552000000000000UL,
                               3846346337460743176UL,
                               3402823669209}
                },
                {
                    .literal =
                    "8402823669209384634633.74607431768211553",
                    .dec_prec = 7,
                    .exp = -1,
                    .digits = {7460743000000000000UL,
                               2823669209384634633UL,
                               840}
                },
                {
                    .literal =
                    "-34028236692093.8563463374607431768211554",
                    .dec_prec = 11,
                    .exp = -1,
                    .digits = {8563463374600000000UL,
                               34028236692093UL}
                },
                {
                    .literal =
                    "99999999999999999999999999999999999999.999999",
                    .dec_prec = 5,
                    .exp = -1,
                    .digits = {9999900000000000000UL,
                               9999999999999999999UL,
                               9999999999999999999UL}
                },
            };

            for (const auto &test : tests) {
                const char *literal = test.literal.c_str();
                SECTION(test.literal) {
                    check_adjusted_digit_array(FPDEC_ROUND_DOWN, literal,
                                               test.dec_prec, test.exp,
                                               test.digits);
                }
            }
        }

        SECTION("Round floor") {
            struct test_data tests[] = {
                {
                    .literal =
                    "34028236692093846346337460743176.8211551",
                    .dec_prec = 12,
                    .exp = -1,
                    .digits = {8211551000000000000UL,
                               3846346337460743176UL,
                               3402823669209}
                },
                {
                    .literal =
                    "34028236692093846346337460743176.8211552",
                    .dec_prec = 7,
                    .exp = -1,
                    .digits = {8211552000000000000UL,
                               3846346337460743176UL,
                               3402823669209}
                },
                {
                    .literal =
                    "8402823669209384634633.74607431768211553",
                    .dec_prec = 7,
                    .exp = -1,
                    .digits = {7460743000000000000UL,
                               2823669209384634633UL,
                               840}
                },
                {
                    .literal =
                    "-34028236692093.8563463374607431768211554",
                    .dec_prec = 11,
                    .exp = -1,
                    .digits = {8563463374700000000UL,
                               34028236692093UL}
                },
                {
                    .literal =
                    "99999999999999999999999999999999999999.999997",
                    .dec_prec = 5,
                    .exp = -1,
                    .digits = {9999900000000000000UL,
                               9999999999999999999UL,
                               9999999999999999999UL}
                },
            };

            for (const auto &test : tests) {
                const char *literal = test.literal.c_str();
                SECTION(test.literal) {
                    check_adjusted_digit_array(FPDEC_ROUND_FLOOR, literal,
                                               test.dec_prec, test.exp,
                                               test.digits);
                }
            }
        }

        SECTION("Round half down") {
            struct test_data tests[] = {
                {
                    .literal =
                    "34028236692093846346337460743176.8211551",
                    .dec_prec = 12,
                    .exp = -1,
                    .digits = {8211551000000000000UL,
                               3846346337460743176UL,
                               3402823669209}
                },
                {
                    .literal =
                    "34028236692093846346337460743176.8211552",
                    .dec_prec = 7,
                    .exp = -1,
                    .digits = {8211552000000000000UL,
                               3846346337460743176UL,
                               3402823669209}
                },
                {
                    .literal =
                    "8402823669209384634633.74607431768211553",
                    .dec_prec = 7,
                    .exp = -1,
                    .digits = {7460743000000000000UL,
                               2823669209384634633UL,
                               840}
                },
                {
                    .literal =
                    "-34028236692093.8563463374607431768211554",
                    .dec_prec = 11,
                    .exp = -1,
                    .digits = {8563463374600000000UL,
                               34028236692093UL}
                },
                {
                    .literal =
                    "99999999999999999999999999999999999999.999996",
                    .dec_prec = 5,
                    .exp = 2,
                    .digits = {1UL}
                },
            };

            for (const auto &test : tests) {
                const char *literal = test.literal.c_str();
                SECTION(test.literal) {
                    check_adjusted_digit_array(FPDEC_ROUND_HALF_DOWN, literal,
                                               test.dec_prec, test.exp,
                                               test.digits);
                }
            }
        }

        SECTION("Round half even") {
            struct test_data tests[] = {
                {
                    .literal =
                    "34028236692093846346337460743176.8211551",
                    .dec_prec = 12,
                    .exp = -1,
                    .digits = {8211551000000000000UL,
                               3846346337460743176UL,
                               3402823669209}
                },
                {
                    .literal =
                    "34028236692093846346337460743176.8211552",
                    .dec_prec = 7,
                    .exp = -1,
                    .digits = {8211552000000000000UL,
                               3846346337460743176UL,
                               3402823669209}
                },
                {
                    .literal =
                    "8402823669209384634633.74607431768211553",
                    .dec_prec = 7,
                    .exp = -1,
                    .digits = {7460743000000000000UL,
                               2823669209384634633UL,
                               840}
                },
                {
                    .literal =
                    "-34028236692093.8563463374607431768211554",
                    .dec_prec = 11,
                    .exp = -1,
                    .digits = {8563463374600000000UL,
                               34028236692093UL}
                },
                {
                    .literal =
                    "99999999999999999999999999999999999999.999996",
                    .dec_prec = 5,
                    .exp = 2,
                    .digits = {1UL}
                },
            };

            for (const auto &test : tests) {
                const char *literal = test.literal.c_str();
                SECTION(test.literal) {
                    check_adjusted_digit_array(FPDEC_ROUND_HALF_EVEN, literal,
                                               test.dec_prec, test.exp,
                                               test.digits);
                }
            }
        }

        SECTION("Round half up") {
            struct test_data tests[] = {
                {
                    .literal =
                    "34028236692093846346337460743176.8211551",
                    .dec_prec = 12,
                    .exp = -1,
                    .digits = {8211551000000000000UL,
                               3846346337460743176UL,
                               3402823669209}
                },
                {
                    .literal =
                    "34028236692093846346337460743176.8211552",
                    .dec_prec = 7,
                    .exp = -1,
                    .digits = {8211552000000000000UL,
                               3846346337460743176UL,
                               3402823669209}
                },
                {
                    .literal =
                    "8402823669209384634633.74607431768211553",
                    .dec_prec = 7,
                    .exp = -1,
                    .digits = {7460743000000000000UL,
                               2823669209384634633UL,
                               840}
                },
                {
                    .literal =
                    "-34028236692093.8563463374607431768211554",
                    .dec_prec = 11,
                    .exp = -1,
                    .digits = {8563463374600000000UL,
                               34028236692093UL}
                },
                {
                    .literal =
                    "99999999999999999999999999999999999999.999996",
                    .dec_prec = 5,
                    .exp = 2,
                    .digits = {1UL}
                },
            };

            for (const auto &test : tests) {
                const char *literal = test.literal.c_str();
                SECTION(test.literal) {
                    check_adjusted_digit_array(FPDEC_ROUND_HALF_UP, literal,
                                               test.dec_prec, test.exp,
                                               test.digits);
                }
            }
        }

        SECTION("Round up") {
            struct test_data tests[] = {
                {
                    .literal =
                    "34028236692093846346337460743176.8211551",
                    .dec_prec = 12,
                    .exp = -1,
                    .digits = {8211551000000000000UL,
                               3846346337460743176UL,
                               3402823669209}
                },
                {
                    .literal =
                    "34028236692093846346337460743176.8211552",
                    .dec_prec = 7,
                    .exp = -1,
                    .digits = {8211552000000000000UL,
                               3846346337460743176UL,
                               3402823669209}
                },
                {
                    .literal =
                    "8402823669209384634633.74607431768211553",
                    .dec_prec = 7,
                    .exp = -1,
                    .digits = {7460744000000000000UL,
                               2823669209384634633UL,
                               840}
                },
                {
                    .literal =
                    "-34028236692093.8563463374607431768211554",
                    .dec_prec = 11,
                    .exp = -1,
                    .digits = {8563463374700000000UL,
                               34028236692093UL}
                },
                {
                    .literal =
                    "99999999999999999999999999999999999999.999996",
                    .dec_prec = 5,
                    .exp = 2,
                    .digits = {1UL}
                },
            };

            for (const auto &test : tests) {
                const char *literal = test.literal.c_str();
                SECTION(test.literal) {
                    check_adjusted_digit_array(FPDEC_ROUND_UP, literal,
                                               test.dec_prec, test.exp,
                                               test.digits);
                }
            }
        }
    } // end SECTION("Digit array variant")

    SECTION("Digit array to shifted int") {

        struct test_data tests[] = {
            {
                .literal = "170000000000000000000000001583e-80",
                .dec_prec = 4,
                .digits = {0UL, 0UL}
            },
            {
                .literal =
                "3402823669209384634633.74607431768211553",
                .dec_prec = 7,
                .digits = {6842914928856769031UL,
                           1844674407UL}
            },
            {
                .literal =
                "-34028236692093.8563463374607431768211554",
                .dec_prec = 1,
                .digits = {340282366920939UL, 0UL}
            },
            {
                .literal =
                "340282366920937.56346337460743176821155",
                .dec_prec = 0,
                .digits = {340282366920938UL, 0UL}
            },
            {
                .literal = "9.999999999999999999999999",
                .dec_prec = 5,
                .digits = {1000000UL, 0UL}
            },
            {
                .literal = "999999999999999999999999.99999",
                .dec_prec = 4,
                .digits = {4477988020393345024UL,
                           542101086UL}
            },
            {
                .literal = "999999.9999999999999999999999",
                .dec_prec = -4,
                .digits = {1000000UL, 0UL}
            },
        };

        for (const auto &test : tests) {
            const char *literal = test.literal.c_str();
            SECTION(test.literal) {
                check_adjusted_shint(FPDEC_ROUND_DEFAULT, literal,
                                     test.dec_prec, test.digits);
            }
        }
    } // end SECTION("Digit array to shifted int")

    SECTION("Shifted int to digit array") {

        struct test_data tests[] = {
            {
                .literal = "0.0000007",
                .dec_prec = 10,
                .exp = -1,
                .digits = {7000000000000UL}
            },
            {
                .literal = "900000000000.000000000",
                .dec_prec = 17,
                .exp = 0,
                .digits = {900000000000UL}
            },
            {
                .literal = "100000000000000000000000.1",
                .dec_prec = 9,
                .exp = -1,
                .digits = {1000000000000000000UL,
                           0UL,
                           10000UL}
            },
            {
                .literal = "100000000000000000000000.00000",
                .dec_prec = 9,
                .exp = 1,
                .digits = {10000UL}
            },
            {
                .literal = "792281625142643375935439.50335",
                .dec_prec = 6,
                .exp = -1,
                .digits = {5033500000000000000UL,
                           1625142643375935439UL,
                           79228UL}
            },
            {
                .literal = "-6e28",
                .dec_prec = -29,
                .exp = 1,
                .digits = {10000000000UL}
            },
        };

        for (const auto &test : tests) {
            const char *literal = test.literal.c_str();
            SECTION(test.literal) {
                check_adjusted_digit_array(FPDEC_ROUND_DEFAULT, literal,
                                           test.dec_prec, test.exp,
                                           test.digits);
            }
        }
    } // end SECTION("Shifted int to digit array")
}

void
check_adjusted_shint(FPDEC_ROUNDING_MODE rnd, const char *literal,
                     const int32_t dec_prec,
                     const std::vector<fpdec_digit_t> &digits) {
    fpdec_t fpdec = FPDEC_ZERO;
    fpdec_t adj = FPDEC_ZERO;
    error_t rc = fpdec_from_ascii_literal(&fpdec, literal);
    REQUIRE(rc == FPDEC_OK);
    rc = fpdec_adjusted(&adj, &fpdec, dec_prec, rnd);
    REQUIRE(rc == FPDEC_OK);
    REQUIRE(is_shint(&adj));
    if (digits[0] == 0 && digits[1] == 0)
        CHECK(FPDEC_SIGN(&adj) == FPDEC_SIGN_ZERO);
    else
        CHECK(FPDEC_SIGN(&adj) == FPDEC_SIGN(&fpdec));
    CHECK(FPDEC_DEC_PREC(&adj) == (dec_prec < 0 ? 0 : dec_prec));
    CHECK(adj.lo == digits[0]);
    CHECK(adj.hi == digits[1]);
    fpdec_reset_to_zero(&fpdec, 0);
}

void
check_adjusted_digit_array(enum FPDEC_ROUNDING_MODE rnd, const char *literal,
                           const int32_t dec_prec, const fpdec_exp_t exp,
                           const std::vector<fpdec_digit_t> &digits) {
    fpdec_t fpdec = FPDEC_ZERO;
    fpdec_t adj = FPDEC_ZERO;
    error_t rc = fpdec_from_ascii_literal(&fpdec, literal);
    REQUIRE(rc == FPDEC_OK);
    rc = fpdec_adjusted(&adj, &fpdec, dec_prec, rnd);
    REQUIRE(rc == FPDEC_OK);
    REQUIRE(is_digit_array(&adj));
    CHECK(FPDEC_SIGN(&adj) == FPDEC_SIGN(&fpdec));
    CHECK(FPDEC_DEC_PREC(&adj) == (dec_prec < 0 ? 0 : dec_prec));
    CHECK(FPDEC_DYN_EXP(&adj) == exp);
    CHECK(FPDEC_DYN_N_DIGITS(&adj) == digits.size());
    for (int i = 0; i < FPDEC_DYN_N_DIGITS(&adj); ++i) {
        CHECK(adj.digit_array->digits[i] == digits[i]);
    }
    fpdec_reset_to_zero(&fpdec, 0);
    fpdec_reset_to_zero(&adj, 0);

}


TEST_CASE("Precision limit exceed") {

    fpdec_t fpdec = FPDEC_ZERO;
    int64_t dec_prec_vals[2] = {FPDEC_MAX_DEC_PREC + 1,
                                -FPDEC_MAX_DEC_PREC - 1};
    std::string name;

    for (const auto &dec_prec : dec_prec_vals) {
        name = dec_prec < 0 ? "LT" : "GT";

        SECTION(name) {
            CHECK(fpdec_adjust(&fpdec, dec_prec, FPDEC_ROUND_DEFAULT) ==
                      FPDEC_PREC_LIMIT_EXCEEDED);
            CHECK(fpdec_adjusted(&fpdec, &FPDEC_ONE, dec_prec,
                                 FPDEC_ROUND_DEFAULT) ==
                      FPDEC_PREC_LIMIT_EXCEEDED);
        }
    }

}


TEST_CASE("Normalize presision") {

    struct test_data {
        std::string x_lit;
        std::string z_lit;
    };

    struct test_data tests[] = {
        {
            .x_lit = "0.0000007",
            .z_lit = "0.0000007"
        },
        {
            .x_lit = "4.00000073400000",
            .z_lit = "4.000000734"
        },
        {
            .x_lit = "-28.09000300",
            .z_lit = "-28.090003"
        },
        {
            .x_lit = "123456789012345678901234567890.0000000",
            .z_lit = "123456789012345678901234567890"
        },
        {
            .x_lit = "100000007000000000000000000000",
            .z_lit = "100000007000000000000000000000"
        },
        {
            .x_lit = "1.0000000700000000000000000000030000000000000000",
            .z_lit = "1.000000070000000000000000000003"
        },
    };

    fpdec_t x = FPDEC_ZERO;
    fpdec_t z = FPDEC_ZERO;
    error_t rc;

    for (const auto &test : tests) {
        const char *x_lit = test.x_lit.c_str();
        const char *z_lit = test.z_lit.c_str();

        SECTION(test.x_lit) {
            rc = fpdec_from_ascii_literal(&x, x_lit);
            REQUIRE(rc == FPDEC_OK);
            rc = fpdec_from_ascii_literal(&z, z_lit);
            REQUIRE(rc == FPDEC_OK);
            rc = fpdec_normalize_prec(&x);
            REQUIRE(rc == FPDEC_OK);
            CHECK(fpdec_compare(&x, &z, false) == 0);
        }
    }
}
