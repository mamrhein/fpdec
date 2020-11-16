/* ---------------------------------------------------------------------------
Name:        formatting_test.cpp

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application. For license
             details please read the file LICENSE.TXT provided together
             with the application.
------------------------------------------------------------------------------
$Source$
$Revision$
*/

#include <cstring>
#include <locale.h>

#include "catch.hpp"
#include "format_spec.h"
#include "fpdec.h"
#include "fpdec_struct.h"


static inline bool
utf8c_eq(const utf8c_t a, const utf8c_t b) {
    if (a.n_bytes != b.n_bytes)
        return false;
    for (int i = 0; i < a.n_bytes; ++i)
        if (a.bytes[i] != b.bytes[i])
            return false;
    return true;
}

static void
validate_format_spec(std::string fmt, const format_spec_t *check) {
    format_spec_t spec;
    int rc;

    rc = parse_format_spec(&spec, (uint8_t *)fmt.c_str());
    REQUIRE(rc == 0);
    CHECK(utf8c_eq(spec.fill, check->fill));
    CHECK(spec.align == check->align);
    CHECK(spec.sign == check->sign);
    CHECK(spec.min_width == check->min_width);
    CHECK(utf8c_eq(spec.thousands_sep, check->thousands_sep));
    for (int i = 0; i < 5; ++i)
        CHECK(spec.grouping[i] == check->grouping[i]);
    CHECK(utf8c_eq(spec.decimal_point, check->decimal_point));
    CHECK(spec.precision == check->precision);
    CHECK(spec.type == check->type);
}

TEST_CASE("Parse valid format specs (locale independent)") {

    struct test_data {
        const std::string fmt;
        const format_spec_t check;
    };

    struct test_data tests[] = {
        {
            .fmt = "",
            .check = DFLT_FORMAT,
        },
        {
            .fmt = "=>",
            .check = {
                {0, ""},                    // fill
                '<',                        // align
                '-',                        // sign
                0,                          // min width
                {0, ""},                    // thousands sep
                {3, 0, 0, 0, 0},            // grouping
                {1, "."},                   // decimal point
                SIZE_MAX,                   // precision
                'f'                         // type
            },
        },
        {
            .fmt = "=>17",
            .check = {
                {1, "="},                   // fill
                '>',                        // align
                '-',                        // sign
                17,                         // min width
                {0, ""},                    // thousands sep
                {3, 0, 0, 0, 0},            // grouping
                {1, "."},                   // decimal point
                SIZE_MAX,                   // precision
                'f'                         // type
            },
        },
        {
            .fmt = "\xe2\x80\xa6=5",
            .check = {
                {3, "\xe2\x80\xa6"},        // fill
                '=',                        // align
                '-',                        // sign
                5,                          // min width
                {0, ""},                    // thousands sep
                {3, 0, 0, 0, 0},            // grouping
                {1, "."},                   // decimal point
                SIZE_MAX,                   // precision
                'f'                         // type
            },
        },
        {
            .fmt = ">+017.4",
            .check = {
                {1, "0"},                   // fill
                '=',                        // align
                '+',                        // sign
                17,                         // min width
                {0, ""},                    // thousands sep
                {3, 0, 0, 0, 0},            // grouping
                {1, "."},                   // decimal point
                4,                          // precision
                'f'                         // type
            },
        },
        {
            .fmt = " 7,.2%",
            .check = {
                {1, " "},                   // fill
                '>',                        // align
                ' ',                        // sign
                7,                          // min width
                {1, ","},                   // thousands sep
                {3, 0, 0, 0, 0},            // grouping
                {1, "."},                   // decimal point
                2,                          // precision
                '%'                         // type
            },
        },
        {
            .fmt = " ^010,.2",
            .check = {
                {1, " "},                   // fill
                '^',                        // align
                '-',                        // sign
                10,                         // min width
                {1, ","},                   // thousands sep
                {3, 0, 0, 0, 0},            // grouping
                {1, "."},                   // decimal point
                2,                          // precision
                'f'                         // type
            },
        },
    };

    for (const auto &test : tests) {
        std::string name = test.fmt == "" ? "empty format" : test.fmt;

        SECTION(name) {
            validate_format_spec(test.fmt, &test.check);
        }
    }
}

TEST_CASE("Parse valid format specs (locale specific)") {

    struct test_data {
        const char *const locale;
        const std::string fmt;
        const format_spec_t check;
    };

    struct test_data tests[] = {
        {
            .locale = "C",
            .fmt = "==14,.3n",
            .check = {
                {1, "="},                   // fill
                '=',                        // align
                '-',                        // sign
                14,                         // min width
                {0, ""},                    // thousands sep
                {0, 0, 0, 0, 0},            // grouping
                {1, "."},                   // decimal point
                3,                          // precision
                'n'                         // type
            },
        },
        {
            .locale = "en_US.UTF-8",
            .fmt = "\xe2\x80\xa6=20n",
            .check = {
                {3, "\xe2\x80\xa6"},        // fill
                '=',                        // align
                '-',                        // sign
                20,                         // min width
                {0, ""},                    // thousands sep
                {3, 3, 0, 0, 0},            // grouping
                {1, "."},                   // decimal point
                SIZE_MAX,                   // precision
                'n'                         // type
            },
        },
        {
            .locale = "de_DE.UTF-8",
            .fmt = ">+017,.4n",
            .check = {
                {1, "0"},                   // fill
                '=',                        // align
                '+',                        // sign
                17,                         // min width
                {1, "."},                   // thousands sep
                {3, 3, 0, 0, 0},            // grouping
                {1, ","},                   // decimal point
                4,                          // precision
                'n'                         // type
            },
        },
    };

    for (const auto &test : tests) {
        char *locale = setlocale(LC_ALL, test.locale);

        SECTION(locale) {
            validate_format_spec(test.fmt, &test.check);
        }
    }
}

TEST_CASE("Parse invalid format specs") {

    std::string fmt_list[] = {
        "\183",
        "_",
        "00.7",
        " .0i",
        "18446744073709551616f",
        ".18446744073709551616f",
        "g",
    };
    format_spec_t spec;

    for (const auto fmt : fmt_list) {

        SECTION(fmt) {
            REQUIRE(parse_format_spec(&spec, (uint8_t *)fmt.c_str()) == -1);
        }
    }
}

TEST_CASE("Format decimal number") {

    SECTION("Shifted int variant") {

        struct test_data {
            const std::string literal;
            const std::string fmt;
            const std::string formatted;
        };

        struct test_data tests[] = {
            {
                .literal = "15006.357",
                .fmt = "",
                .formatted = "15006.357"
            },
            {
                .literal = "-15006.357",
                .fmt = ".1",
                .formatted = "-15006.4"
            },
            {
                .literal = "700.9",
                .fmt = ".0",
                .formatted = "701"
            },
            {
                .literal = "3715020.359",
                .fmt = ">+20,.5",
                .formatted = "    +3,715,020.35900"
            },
            {
                .literal = "12715020.35",
                .fmt = "\xe2\x80\xa4>+20,",
                .formatted = "\xE2\x80\xA4\xE2\x80\xA4\xE2\x80\xA4"
                             "\xE2\x80\xA4\xE2\x80\xA4\xE2\x80\xA4"
                             "+12,715,020.35"
            },
            {
                .literal = "853715020.3594",
                .fmt = "^-25,.2",
                .formatted = "     853,715,020.36      "
            },
            {
                .literal = "715020.594",
                .fmt = "_=+20,.4",
                .formatted = "+_______715,020.5940"
            },
            {
                .literal = "9535.000",
                .fmt = "012",
                .formatted = "00009535.000"
            },
            {
                .literal = "-415000",
                .fmt = "+020,.4",
                .formatted = "-00,000,415,000.0000"
            },
            {
                .literal = "0",
                .fmt = "012,",
                .formatted = "0,000,000,000"
            },
            {
                .literal = "0.0380",
                .fmt = " 012,.4",
                .formatted = " 00,000.0380"
            },
            {
                .literal = "15000.03",
                .fmt = "020,.0",
                .formatted = "0,000,000,000,015,000"
            },
            {
                .literal = "0.055",
                .fmt = "%",
                .formatted = "5.500%"
            },
            {
                .literal = "3.074",
                .fmt = "_>10.2%",
                .formatted = "___307.40%"
            },
            {
                .literal = "0.3074",
                .fmt = "_>10.2%",
                .formatted = "____30.74%"
            },
            {
                .literal = "0.03075",
                .fmt = "_>10.2%",
                .formatted = "_____3.08%"
            },
            {
                .literal = "0.1319",
                .fmt = " 010,.2%",
                .formatted = " 0,013.19%"
            },
            {
                .literal = "0.03186",
                .fmt = " 010,.3%",
                .formatted = " 0,003.186%"
            },
            {
                .literal = "0.7",
                .fmt = "+.2%",
                .formatted = "+70.00%"
            },
            {
                .literal = "7",
                .fmt = ".2%",
                .formatted = "700.00%"
            },
            {
                .literal = "70",
                .fmt = ",.2%",
                .formatted = "7,000.00%"
            },
            {
                .literal = "-0.0008",
                .fmt = " .2%",
                .formatted = "-0.08%"
            },
            {
                .literal = "-0.00008",
                .fmt = " .2%",
                .formatted = "-0.01%"
            },
            {
                .literal = "-0.000008",
                .fmt = " .2%",
                .formatted = " 0.00%"
            },
        };

        for (auto test : tests) {
            fpdec_t dec = FPDEC_ZERO;
            error_t rc;
            uint8_t *formatted;

            SECTION(test.literal) {

                rc = fpdec_from_ascii_literal(&dec, test.literal.c_str());
                REQUIRE(rc == FPDEC_OK);
                formatted = fpdec_formatted(&dec,
                                            (uint8_t *)test.fmt.c_str());
                REQUIRE(formatted != NULL);
                CHECK(strcmp((char *)formatted, test.formatted.c_str()) == 0);
                fpdec_mem_free(formatted);
            }
        }
    }

    SECTION("Digit array variant") {

        struct test_data {
            const std::string literal;
            const std::string fmt;
            const std::string formatted;
        };

        struct test_data tests[] = {
            {
                .literal = "15006.3575548888",
                .fmt = "",
                .formatted = "15006.3575548888"
            },
            {
                .literal = "-15006.3572777532",
                .fmt = ".1",
                .formatted = "-15006.4"
            },
            {
                .literal = "100000000000000000000000000700.9",
                .fmt = ".0",
                .formatted = "100000000000000000000000000701"
            },
            {
                .literal = "88793825633715020003856983.359",
                .fmt = ">+45,.5",
                .formatted = "    +88,793,825,633,715,020,003,856,983.35900"
            },
            {
                .literal = "0.00999999999",
                .fmt = "\xe2\x80\xa4>+20,",
                .formatted = "\xE2\x80\xA4\xE2\x80\xA4\xE2\x80\xA4"
                             "\xE2\x80\xA4\xE2\x80\xA4\xE2\x80\xA4"
                             "+0.00999999999"
            },
            {
                .literal = "8537150203594e-34",
                .fmt = "^-45.34",
                .formatted = "    0.0000000000000000000008537150203594     "
            },
            {
                .literal = "570000000000000000000000020.594",
                .fmt = "_=+45,.4",
                .formatted = "+____570,000,000,000,000,000,000,000,020.5940"
            },
            {
                .literal = "-415e27",
                .fmt = "+050,.4",
                .formatted = "-0,000,415,000,000,000,000,000,000,000,000,000"
                             ".0000"
            },
            {
                .literal = "123456789.1234567890",
                .fmt = "025",
                .formatted = "00000123456789.1234567890"
            },
            {
                .literal = "-0.0930000001",
                .fmt = "%",
                .formatted = "-9.3000000100%"
            },
            {
                .literal = "-0.0740000001",
                .fmt = ".10%",
                .formatted = "-7.4000000100%"
            },
            {
                .literal = "0.307400000333",
                .fmt = ".10%",
                .formatted = "30.7400000333%"
            },
            {
                .literal = "0.0070000000057",
                .fmt = ".10%",
                .formatted = "0.7000000006%"
            },
            {
                .literal = "4.1319999999",
                .fmt = " 020,.8%",
                .formatted = " 0,000,413.19999999%"
            },
            {
                .literal = "0.0428600000",
                .fmt = " 020,.9%",
                .formatted = " 0,000,004.286000000%"
            },
            {
                .literal = "1234567890.12345678901234567890",
                .fmt = ".2%",
                .formatted = "123456789012.35%"
            },
        };

        for (auto test : tests) {
            fpdec_t dec = FPDEC_ZERO;
            error_t rc;
            uint8_t *formatted;

            SECTION(test.literal) {

                rc = fpdec_from_ascii_literal(&dec, test.literal.c_str());
                REQUIRE(rc == FPDEC_OK);
                CHECK(FPDEC_IS_DYN_ALLOC(&dec));
                formatted = fpdec_formatted(&dec,
                                            (uint8_t *)test.fmt.c_str());
                REQUIRE(formatted != NULL);
                CHECK(strcmp((char *)formatted, test.formatted.c_str()) == 0);
                fpdec_mem_free(formatted);
            }
        }
    }
}
