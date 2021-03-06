/* ---------------------------------------------------------------------------
Name:        add_sub_test.cpp

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

#include "catch.hpp"
#include "fpdec.h"
#include "checks.hpp"


TEST_CASE("Addition / Subtraction") {

    struct test_data {
        std::string lit_x;
        std::string lit_y;
        std::string lit_sum;
        std::string lit_diff;
    };

    struct test_data tests[] = {
            {
                    .lit_x = "0.00",
                    .lit_y = "0.00",
                    .lit_sum = "0.00",
                    .lit_diff = "0.00",
            },
            {
                    .lit_x = "0.00",
                    .lit_y = "1e-83",
                    .lit_sum = "1e-83",
                    .lit_diff = "-1e-83",
            },
            {
                    .lit_x = "0.00",
                    .lit_y = "-0.28363e-7",
                    .lit_sum = "-0.000000028363",
                    .lit_diff = "0.000000028363",
            },
            {
                    .lit_x = "0.00",
                    .lit_y = "-0.28364e-7",
                    .lit_sum = "-0.000000028364",
                    .lit_diff = "0.000000028364",
            },
            {
                    .lit_x = "0.00",
                    .lit_y = "1",
                    .lit_sum = "1.00",
                    .lit_diff = "-1.00",
            },
            {
                    .lit_x = "0.00",
                    .lit_y = "-12345678901234567890.12345",
                    .lit_sum = "-12345678901234567890.12345",
                    .lit_diff = "12345678901234567890.12345",
            },
            {
                    .lit_x = "0.00",
                    .lit_y = "5.30951e42",
                    .lit_sum = "5309510000000000000000000000000000000000000.00",
                    .lit_diff = "-5309510000000000000000000000000000000000000.00",
            },
            {
                    .lit_x = "0.00",
                    .lit_y = "-5.309511e42",
                    .lit_sum = "-5309511000000000000000000000000000000000000.00",
                    .lit_diff = "5309511000000000000000000000000000000000000.00",
            },
            {
                    .lit_x = "0.00",
                    .lit_y = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_sum = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_diff = "-11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888",
            },
            {
                    .lit_x = "1e-83",
                    .lit_y = "0.00",
                    .lit_sum = "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000001",
                    .lit_diff = "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000001",
            },
            {
                    .lit_x = "1e-83",
                    .lit_y = "1e-83",
                    .lit_sum = "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000002",
                    .lit_diff = "0.00000000000000000000000000000000000000000000000000000000000000000000000000000000000",
            },
            {
                    .lit_x = "1e-83",
                    .lit_y = "-0.28363e-7",
                    .lit_sum = "-0.00000002836299999999999999999999999999999999999999999999999999999999999999999999999",
                    .lit_diff = "0.00000002836300000000000000000000000000000000000000000000000000000000000000000000001",
            },
            {
                    .lit_x = "1e-83",
                    .lit_y = "-0.28364e-7",
                    .lit_sum = "-0.00000002836399999999999999999999999999999999999999999999999999999999999999999999999",
                    .lit_diff = "0.00000002836400000000000000000000000000000000000000000000000000000000000000000000001",
            },
            {
                    .lit_x = "1e-83",
                    .lit_y = "1",
                    .lit_sum = "1.00000000000000000000000000000000000000000000000000000000000000000000000000000000001",
                    .lit_diff = "-0.99999999999999999999999999999999999999999999999999999999999999999999999999999999999",
            },
            {
                    .lit_x = "1e-83",
                    .lit_y = "-12345678901234567890.12345",
                    .lit_sum = "-12345678901234567890.12344999999999999999999999999999999999999999999999999999999999999999999999999999999",
                    .lit_diff = "12345678901234567890.12345000000000000000000000000000000000000000000000000000000000000000000000000000001",
            },
            {
                    .lit_x = "1e-83",
                    .lit_y = "5.30951e42",
                    .lit_sum = "5309510000000000000000000000000000000000000.00000000000000000000000000000000000000000000000000000000000000000000000000000000001",
                    .lit_diff = "-5309509999999999999999999999999999999999999.99999999999999999999999999999999999999999999999999999999999999999999999999999999999",
            },
            {
                    .lit_x = "1e-83",
                    .lit_y = "-5.309511e42",
                    .lit_sum = "-5309510999999999999999999999999999999999999.99999999999999999999999999999999999999999999999999999999999999999999999999999999999",
                    .lit_diff = "5309511000000000000000000000000000000000000.00000000000000000000000000000000000000000000000000000000000000000000000000000000001",
            },
            {
                    .lit_x = "1e-83",
                    .lit_y = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_sum = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888000000000000000000000000000000000000000001",
                    .lit_diff = "-11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777887999999999999999999999999999999999999999999",
            },
            {
                    .lit_x = "-0.28363e-7",
                    .lit_y = "0.00",
                    .lit_sum = "-0.000000028363",
                    .lit_diff = "-0.000000028363",
            },
            {
                    .lit_x = "-0.28363e-7",
                    .lit_y = "1e-83",
                    .lit_sum = "-0.00000002836299999999999999999999999999999999999999999999999999999999999999999999999",
                    .lit_diff = "-0.00000002836300000000000000000000000000000000000000000000000000000000000000000000001",
            },
            {
                    .lit_x = "-0.28363e-7",
                    .lit_y = "-0.28363e-7",
                    .lit_sum = "-0.000000056726",
                    .lit_diff = "0.000000000000",
            },
            {
                    .lit_x = "-0.28363e-7",
                    .lit_y = "-0.28364e-7",
                    .lit_sum = "-0.000000056727",
                    .lit_diff = "0.000000000001",
            },
            {
                    .lit_x = "-0.28363e-7",
                    .lit_y = "1",
                    .lit_sum = "0.999999971637",
                    .lit_diff = "-1.000000028363",
            },
            {
                    .lit_x = "-0.28363e-7",
                    .lit_y = "-12345678901234567890.12345",
                    .lit_sum = "-12345678901234567890.123450028363",
                    .lit_diff = "12345678901234567890.123449971637",
            },
            {
                    .lit_x = "-0.28363e-7",
                    .lit_y = "5.30951e42",
                    .lit_sum = "5309509999999999999999999999999999999999999.999999971637",
                    .lit_diff = "-5309510000000000000000000000000000000000000.000000028363",
            },
            {
                    .lit_x = "-0.28363e-7",
                    .lit_y = "-5.309511e42",
                    .lit_sum = "-5309511000000000000000000000000000000000000.000000028363",
                    .lit_diff = "5309510999999999999999999999999999999999999.999999971637",
            },
            {
                    .lit_x = "-0.28363e-7",
                    .lit_y = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_sum = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666663830366666667777777777777777777888",
                    .lit_diff = "-11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666669502966666667777777777777777777888",
            },
            {
                    .lit_x = "-0.28364e-7",
                    .lit_y = "0.00",
                    .lit_sum = "-0.000000028364",
                    .lit_diff = "-0.000000028364",
            },
            {
                    .lit_x = "-0.28364e-7",
                    .lit_y = "1e-83",
                    .lit_sum = "-0.00000002836399999999999999999999999999999999999999999999999999999999999999999999999",
                    .lit_diff = "-0.00000002836400000000000000000000000000000000000000000000000000000000000000000000001",
            },
            {
                    .lit_x = "-0.28364e-7",
                    .lit_y = "-0.28363e-7",
                    .lit_sum = "-0.000000056727",
                    .lit_diff = "-0.000000000001",
            },
            {
                    .lit_x = "-0.28364e-7",
                    .lit_y = "-0.28364e-7",
                    .lit_sum = "-0.000000056728",
                    .lit_diff = "0.000000000000",
            },
            {
                    .lit_x = "-0.28364e-7",
                    .lit_y = "1",
                    .lit_sum = "0.999999971636",
                    .lit_diff = "-1.000000028364",
            },
            {
                    .lit_x = "-0.28364e-7",
                    .lit_y = "-12345678901234567890.12345",
                    .lit_sum = "-12345678901234567890.123450028364",
                    .lit_diff = "12345678901234567890.123449971636",
            },
            {
                    .lit_x = "-0.28364e-7",
                    .lit_y = "5.30951e42",
                    .lit_sum = "5309509999999999999999999999999999999999999.999999971636",
                    .lit_diff = "-5309510000000000000000000000000000000000000.000000028364",
            },
            {
                    .lit_x = "-0.28364e-7",
                    .lit_y = "-5.309511e42",
                    .lit_sum = "-5309511000000000000000000000000000000000000.000000028364",
                    .lit_diff = "5309510999999999999999999999999999999999999.999999971636",
            },
            {
                    .lit_x = "-0.28364e-7",
                    .lit_y = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_sum = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666663830266666667777777777777777777888",
                    .lit_diff = "-11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666669503066666667777777777777777777888",
            },
            {
                    .lit_x = "1",
                    .lit_y = "0.00",
                    .lit_sum = "1.00",
                    .lit_diff = "1.00",
            },
            {
                    .lit_x = "1",
                    .lit_y = "1e-83",
                    .lit_sum = "1.00000000000000000000000000000000000000000000000000000000000000000000000000000000001",
                    .lit_diff = "0.99999999999999999999999999999999999999999999999999999999999999999999999999999999999",
            },
            {
                    .lit_x = "1",
                    .lit_y = "-0.28363e-7",
                    .lit_sum = "0.999999971637",
                    .lit_diff = "1.000000028363",
            },
            {
                    .lit_x = "1",
                    .lit_y = "-0.28364e-7",
                    .lit_sum = "0.999999971636",
                    .lit_diff = "1.000000028364",
            },
            {
                    .lit_x = "1",
                    .lit_y = "1",
                    .lit_sum = "2",
                    .lit_diff = "0",
            },
            {
                    .lit_x = "1",
                    .lit_y = "-12345678901234567890.12345",
                    .lit_sum = "-12345678901234567889.12345",
                    .lit_diff = "12345678901234567891.12345",
            },
            {
                    .lit_x = "1",
                    .lit_y = "5.30951e42",
                    .lit_sum = "5309510000000000000000000000000000000000001",
                    .lit_diff = "-5309509999999999999999999999999999999999999",
            },
            {
                    .lit_x = "1",
                    .lit_y = "-5.309511e42",
                    .lit_sum = "-5309510999999999999999999999999999999999999",
                    .lit_diff = "5309511000000000000000000000000000000000001",
            },
            {
                    .lit_x = "1",
                    .lit_y = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_sum = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555556.66666666666666666667777777777777777777888",
                    .lit_diff = "-11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555554.66666666666666666667777777777777777777888",
            },
            {
                    .lit_x = "-12345678901234567890.12345",
                    .lit_y = "0.00",
                    .lit_sum = "-12345678901234567890.12345",
                    .lit_diff = "-12345678901234567890.12345",
            },
            {
                    .lit_x = "-12345678901234567890.12345",
                    .lit_y = "1e-83",
                    .lit_sum = "-12345678901234567890.12344999999999999999999999999999999999999999999999999999999999999999999999999999999",
                    .lit_diff = "-12345678901234567890.12345000000000000000000000000000000000000000000000000000000000000000000000000000001",
            },
            {
                    .lit_x = "-12345678901234567890.12345",
                    .lit_y = "-0.28363e-7",
                    .lit_sum = "-12345678901234567890.123450028363",
                    .lit_diff = "-12345678901234567890.123449971637",
            },
            {
                    .lit_x = "-12345678901234567890.12345",
                    .lit_y = "-0.28364e-7",
                    .lit_sum = "-12345678901234567890.123450028364",
                    .lit_diff = "-12345678901234567890.123449971636",
            },
            {
                    .lit_x = "-12345678901234567890.12345",
                    .lit_y = "1",
                    .lit_sum = "-12345678901234567889.12345",
                    .lit_diff = "-12345678901234567891.12345",
            },
            {
                    .lit_x = "-12345678901234567890.12345",
                    .lit_y = "-12345678901234567890.12345",
                    .lit_sum = "-24691357802469135780.24690",
                    .lit_diff = "0.00000",
            },
            {
                    .lit_x = "-12345678901234567890.12345",
                    .lit_y = "5.30951e42",
                    .lit_sum = "5309509999999999999999987654321098765432109.87655",
                    .lit_diff = "-5309510000000000000000012345678901234567890.12345",
            },
            {
                    .lit_x = "-12345678901234567890.12345",
                    .lit_y = "-5.309511e42",
                    .lit_sum = "-5309511000000000000000012345678901234567890.12345",
                    .lit_diff = "5309510999999999999999987654321098765432109.87655",
            },
            {
                    .lit_x = "-12345678901234567890.12345",
                    .lit_y = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_sum = "11111111111111111112222222222222222222333333333333333333344444444444444444433209876654320987665.54321666666666666667777777777777777777888",
                    .lit_diff = "-11111111111111111112222222222222222222333333333333333333344444444444444444457901234456790123445.79011666666666666667777777777777777777888",
            },
            {
                    .lit_x = "5.30951e42",
                    .lit_y = "0.00",
                    .lit_sum = "5309510000000000000000000000000000000000000.00",
                    .lit_diff = "5309510000000000000000000000000000000000000.00",
            },
            {
                    .lit_x = "5.30951e42",
                    .lit_y = "1e-83",
                    .lit_sum = "5309510000000000000000000000000000000000000.00000000000000000000000000000000000000000000000000000000000000000000000000000000001",
                    .lit_diff = "5309509999999999999999999999999999999999999.99999999999999999999999999999999999999999999999999999999999999999999999999999999999",
            },
            {
                    .lit_x = "5.30951e42",
                    .lit_y = "-0.28363e-7",
                    .lit_sum = "5309509999999999999999999999999999999999999.999999971637",
                    .lit_diff = "5309510000000000000000000000000000000000000.000000028363",
            },
            {
                    .lit_x = "5.30951e42",
                    .lit_y = "-0.28364e-7",
                    .lit_sum = "5309509999999999999999999999999999999999999.999999971636",
                    .lit_diff = "5309510000000000000000000000000000000000000.000000028364",
            },
            {
                    .lit_x = "5.30951e42",
                    .lit_y = "1",
                    .lit_sum = "5309510000000000000000000000000000000000001",
                    .lit_diff = "5309509999999999999999999999999999999999999",
            },
            {
                    .lit_x = "5.30951e42",
                    .lit_y = "-12345678901234567890.12345",
                    .lit_sum = "5309509999999999999999987654321098765432109.87655",
                    .lit_diff = "5309510000000000000000012345678901234567890.12345",
            },
            {
                    .lit_x = "5.30951e42",
                    .lit_y = "5.30951e42",
                    .lit_sum = "10619020000000000000000000000000000000000000",
                    .lit_diff = "0",
            },
            {
                    .lit_x = "5.30951e42",
                    .lit_y = "-5.309511e42",
                    .lit_sum = "-1000000000000000000000000000000000000",
                    .lit_diff = "10619021000000000000000000000000000000000000",
            },
            {
                    .lit_x = "5.30951e42",
                    .lit_y = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_sum = "11111111111111111112222222222222222222333333333333338642854444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_diff = "-11111111111111111112222222222222222222333333333333328023834444444444444444445555555555555555555.66666666666666666667777777777777777777888",
            },
            {
                    .lit_x = "-5.309511e42",
                    .lit_y = "0.00",
                    .lit_sum = "-5309511000000000000000000000000000000000000.00",
                    .lit_diff = "-5309511000000000000000000000000000000000000.00",
            },
            {
                    .lit_x = "-5.309511e42",
                    .lit_y = "1e-83",
                    .lit_sum = "-5309510999999999999999999999999999999999999.99999999999999999999999999999999999999999999999999999999999999999999999999999999999",
                    .lit_diff = "-5309511000000000000000000000000000000000000.00000000000000000000000000000000000000000000000000000000000000000000000000000000001",
            },
            {
                    .lit_x = "-5.309511e42",
                    .lit_y = "-0.28363e-7",
                    .lit_sum = "-5309511000000000000000000000000000000000000.000000028363",
                    .lit_diff = "-5309510999999999999999999999999999999999999.999999971637",
            },
            {
                    .lit_x = "-5.309511e42",
                    .lit_y = "-0.28364e-7",
                    .lit_sum = "-5309511000000000000000000000000000000000000.000000028364",
                    .lit_diff = "-5309510999999999999999999999999999999999999.999999971636",
            },
            {
                    .lit_x = "-5.309511e42",
                    .lit_y = "1",
                    .lit_sum = "-5309510999999999999999999999999999999999999",
                    .lit_diff = "-5309511000000000000000000000000000000000001",
            },
            {
                    .lit_x = "-5.309511e42",
                    .lit_y = "-12345678901234567890.12345",
                    .lit_sum = "-5309511000000000000000012345678901234567890.12345",
                    .lit_diff = "-5309510999999999999999987654321098765432109.87655",
            },
            {
                    .lit_x = "-5.309511e42",
                    .lit_y = "5.30951e42",
                    .lit_sum = "-1000000000000000000000000000000000000",
                    .lit_diff = "-10619021000000000000000000000000000000000000",
            },
            {
                    .lit_x = "-5.309511e42",
                    .lit_y = "-5.309511e42",
                    .lit_sum = "-10619022000000000000000000000000000000000000",
                    .lit_diff = "0",
            },
            {
                    .lit_x = "-5.309511e42",
                    .lit_y = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_sum = "11111111111111111112222222222222222222333333333333328023833444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_diff = "-11111111111111111112222222222222222222333333333333338642855444444444444444445555555555555555555.66666666666666666667777777777777777777888",
            },
            {
                    .lit_x = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_y = "0.00",
                    .lit_sum = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_diff = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888",
            },
            {
                    .lit_x = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_y = "1e-83",
                    .lit_sum = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888000000000000000000000000000000000000000001",
                    .lit_diff = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777887999999999999999999999999999999999999999999",
            },
            {
                    .lit_x = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_y = "-0.28363e-7",
                    .lit_sum = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666663830366666667777777777777777777888",
                    .lit_diff = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666669502966666667777777777777777777888",
            },
            {
                    .lit_x = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_y = "-0.28364e-7",
                    .lit_sum = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666663830266666667777777777777777777888",
                    .lit_diff = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666669503066666667777777777777777777888",
            },
            {
                    .lit_x = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_y = "1",
                    .lit_sum = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555556.66666666666666666667777777777777777777888",
                    .lit_diff = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555554.66666666666666666667777777777777777777888",
            },
            {
                    .lit_x = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_y = "-12345678901234567890.12345",
                    .lit_sum = "11111111111111111112222222222222222222333333333333333333344444444444444444433209876654320987665.54321666666666666667777777777777777777888",
                    .lit_diff = "11111111111111111112222222222222222222333333333333333333344444444444444444457901234456790123445.79011666666666666667777777777777777777888",
            },
            {
                    .lit_x = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_y = "5.30951e42",
                    .lit_sum = "11111111111111111112222222222222222222333333333333338642854444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_diff = "11111111111111111112222222222222222222333333333333328023834444444444444444445555555555555555555.66666666666666666667777777777777777777888",
            },
            {
                    .lit_x = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_y = "-5.309511e42",
                    .lit_sum = "11111111111111111112222222222222222222333333333333328023833444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_diff = "11111111111111111112222222222222222222333333333333338642855444444444444444445555555555555555555.66666666666666666667777777777777777777888",
            },
            {
                    .lit_x = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_y = "11111111111111111112222222222222222222333333333333333333344444444444444444445555555555555555555.66666666666666666667777777777777777777888",
                    .lit_sum = "22222222222222222224444444444444444444666666666666666666688888888888888888891111111111111111111.33333333333333333335555555555555555555776",
                    .lit_diff = "0.00000000000000000000000000000000000000000",
            },
    };
    error_t rc;

    for (const auto &test : tests) {
        fpdec_t x = FPDEC_ZERO;
        fpdec_t y = FPDEC_ZERO;
        fpdec_t z = FPDEC_ZERO;
        fpdec_t s = FPDEC_ZERO;
        fpdec_t d = FPDEC_ZERO;
        
        const std::string section_name = test.lit_x + " / " + test.lit_y;

        SECTION(section_name) {
            rc = fpdec_from_ascii_literal(&x, test.lit_x.c_str());
            REQUIRE(rc == FPDEC_OK);
            rc = fpdec_from_ascii_literal(&y, test.lit_y.c_str());
            REQUIRE(rc == FPDEC_OK);
            rc = fpdec_from_ascii_literal(&s, test.lit_sum.c_str());
            REQUIRE(rc == FPDEC_OK);
            rc = fpdec_from_ascii_literal(&d, test.lit_diff.c_str());
            REQUIRE(rc == FPDEC_OK);

            fpdec_add(&z, &x, &y);
            REQUIRE(fpdec_compare(&z, &s, false) == 0);
            fpdec_reset_to_zero(&z, 0);

            fpdec_sub(&z, &x, &y);
            REQUIRE(fpdec_compare(&z, &d, false) == 0);
            fpdec_reset_to_zero(&z, 0);
        }

        fpdec_reset_to_zero(&x, 0);
        fpdec_reset_to_zero(&y, 0);
        fpdec_reset_to_zero(&s, 0);
        fpdec_reset_to_zero(&d, 0);
    }
}

TEST_CASE("Subtraction with change of variant") {

    struct test_data {
        std::string lit_x;
        std::string lit_y;
        std::string lit_diff;
    };

    struct test_data tests[] = {
            {
                    .lit_x = "1792281625142643375935439503.35",
                    .lit_y = "1000000000000000000000000000.00",
                    .lit_diff = "792281625142643375935439503.35",
            },
            {
                    .lit_x = "1000000000000000000000000000",
                    .lit_y = "792281625142643375935439504.11",
                    .lit_diff = "207718374857356624064560495.89",
            },
            {
                    .lit_x = "1000000000000000000000000000.25",
                    .lit_y = "1000000000000000000000000000.25",
                    .lit_diff = "0",
            },
            {
                    .lit_x = "1000000000000000000000000000.25",
                    .lit_y = "1000000000000000000000000000.05",
                    .lit_diff = "0.2",
            },
            {
                    .lit_x = "1001000000000000000000000000.25",
                    .lit_y = "1000000000000000000000000000.20",
                    .lit_diff = "1000000000000000000000000.05",
            },
    };
    error_t rc;

    for (const auto &test : tests) {
        fpdec_t x = FPDEC_ZERO;
        fpdec_t y = FPDEC_ZERO;
        fpdec_t z = FPDEC_ZERO;
        fpdec_t d = FPDEC_ZERO;

        const std::string section_name = test.lit_x + " / " + test.lit_y;

        SECTION(section_name) {
            rc = fpdec_from_ascii_literal(&x, test.lit_x.c_str());
            REQUIRE(rc == FPDEC_OK);
            rc = fpdec_from_ascii_literal(&y, test.lit_y.c_str());
            REQUIRE(rc == FPDEC_OK);
            REQUIRE((is_digit_array(&x) || is_digit_array(&y)));
            rc = fpdec_from_ascii_literal(&d, test.lit_diff.c_str());
            REQUIRE(rc == FPDEC_OK);

            fpdec_sub(&z, &x, &y);
            REQUIRE(fpdec_compare(&z, &d, false) == 0);
            REQUIRE(is_shint(&z));
        }

        fpdec_reset_to_zero(&x, 0);
        fpdec_reset_to_zero(&y, 0);
        fpdec_reset_to_zero(&z, 0);
        fpdec_reset_to_zero(&d, 0);
    }
}

