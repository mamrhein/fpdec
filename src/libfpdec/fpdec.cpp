/* ---------------------------------------------------------------------------
Name:        fpdec.cpp

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application. For license
             details please read the file LICENSE.TXT provided together
             with the application.
------------------------------------------------------------------------------
$Source$
$Revision$
*/

#include "fpdec.hpp"
#include "fpdec.h"

using namespace fpdec;

static_assert(sizeof(Decimal) == 16, "Size of Decimal should be 16!");

Decimal::Decimal(): fpdec(FPDEC_ZERO) {}

Decimal::Decimal(const Decimal& src) {
    fpdec_copy(&fpdec, &src.fpdec);
}

Decimal::Decimal(const std::string& val) {
    fpdec = FPDEC_ZERO;
    error_t err = fpdec_from_ascii_literal(&fpdec, val.c_str());
    if (err == FPDEC_INVALID_DECIMAL_LITERAL)
        throw InvalidDecimalLiteral(val);
    if (err != FPDEC_OK)
        throw InternalLimitExceeded();
}

Decimal::Decimal(const long long int val) {
    fpdec = FPDEC_ZERO;
    fpdec_from_long_long(&fpdec, val);
}

Decimal::Decimal(const fpdec_t* src) {
    fpdec_copy(&fpdec, src);
}

Decimal::~Decimal() {
    fpdec_reset_to_zero(&fpdec, 0);
}

// properties

fpdec_sign_t Decimal::sign() const {
    return fpdec.sign;
}

fpdec_dec_prec_t Decimal::precision() const {
    return fpdec.dec_prec;
}

int Decimal::magnitude() const {
    int magn = fpdec_magnitude(&fpdec);
    if (magn == -1 && errno == ERANGE)
        throw std::range_error("Result would be -Infinity.");
    return magn;
}

// operators

Decimal Decimal::operator+() const {
    return *this;
}

Decimal Decimal::operator-() const {
    auto dec = Decimal();
    fpdec_neg(&dec.fpdec, &fpdec);
    return dec;
}

bool Decimal::operator==(const Decimal& rhs) const {
    return fpdec_compare(&fpdec, &(rhs.fpdec), 0) == 0;
}

bool fpdec::operator==(const long long int lhs, const Decimal& rhs) {
    return rhs == lhs;
}

bool Decimal::operator!=(const Decimal& rhs) const {
    return fpdec_compare(&fpdec, &(rhs.fpdec), 0) != 0;
}

bool fpdec::operator!=(const long long int lhs, const Decimal& rhs) {
    return !(rhs == lhs);
}

bool Decimal::operator<=(const Decimal& rhs) const {
    return fpdec_compare(&fpdec, &(rhs.fpdec), 0) <= 0;
}

bool fpdec::operator<=(const long long int lhs, const Decimal& rhs) {
    return rhs >= lhs;
}

bool Decimal::operator<(const Decimal& rhs) const {
    return fpdec_compare(&fpdec, &(rhs.fpdec), 0) < 0;
}

bool fpdec::operator<(const long long int lhs, const Decimal& rhs) {
    return rhs > lhs;
}

bool Decimal::operator>=(const Decimal& rhs) const {
    return fpdec_compare(&fpdec, &(rhs.fpdec), 0) >= 0;
}

bool fpdec::operator>=(const long long int lhs, const Decimal& rhs) {
    return rhs <= lhs;
}

bool Decimal::operator>(const Decimal& rhs) const {
    return fpdec_compare(&fpdec, &(rhs.fpdec), 0) > 0;
}

bool fpdec::operator>(const long long int lhs, const Decimal& rhs) {
    return rhs < lhs;
}
