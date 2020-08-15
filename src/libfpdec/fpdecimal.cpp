/* ---------------------------------------------------------------------------
Name:        fpdecimal.cpp

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application. For license
             details please read the file LICENSE.TXT provided together
             with the application.
------------------------------------------------------------------------------
$Source$
$Revision$
*/

#include <sstream>
#include "fpdecimal.hpp"
#include "fpdec.h"
#include "digit_array_.h"

using namespace fpdec;

static_assert(sizeof(Decimal) == 16, "Size of Decimal should be 16!");

static inline void
throw_exc(const error_t err, const std::string &val = {}) {
    switch (err) {
        case FPDEC_PREC_LIMIT_EXCEEDED:
        case FPDEC_EXP_LIMIT_EXCEEDED:
        case FPDEC_N_DIGITS_LIMIT_EXCEEDED:
            throw InternalLimitExceeded(err);
        case FPDEC_INVALID_DECIMAL_LITERAL:
            throw InvalidDecimalLiteral(val);
        case FPDEC_DIVIDE_BY_ZERO:
            throw DivisionByZero();
        case ENOMEM:
            throw std::bad_alloc();
        default:
            throw std::invalid_argument("Unknown error code: " +
                                        std::to_string(err));
    }
}

// *** class Decimal *** -----------------------------------------------------

// constructors

Decimal::Decimal() noexcept: fpdec(FPDEC_ZERO) {
}

Decimal::Decimal(const Decimal &src) {
    error_t err = fpdec_copy(&fpdec, &src.fpdec);
    if (err != FPDEC_OK)
        throw_exc(err);
}

Decimal::Decimal(const Decimal &src, const fpdec_dec_prec_t adjust_to_prec,
                 const Rounding rnd) {
    fpdec = FPDEC_ZERO;
    error_t err = fpdec_adjusted(&fpdec, &src.fpdec, adjust_to_prec,
                                 (FPDEC_ROUNDING_MODE)rnd);
    if (err != FPDEC_OK)
        throw_exc(err);
}

Decimal::Decimal(Decimal &&src) noexcept {
    fpdec = std::move(src.fpdec);
    // steal digit array
    src.fpdec.dyn_alloc = false;
}

Decimal::Decimal(const std::string &val) {
    fpdec = FPDEC_ZERO;
    error_t err = fpdec_from_ascii_literal(&fpdec, val.c_str());
    if (err != FPDEC_OK)
        throw_exc(err);
}

Decimal::Decimal(const long long int val) noexcept {
    fpdec = FPDEC_ZERO;
    fpdec_from_long_long(&fpdec, val);
}

Decimal::Decimal(const fpdec_t *src) {
    fpdec_copy(&fpdec, src);
}

Decimal::~Decimal() {
    fpdec_reset_to_zero(&fpdec, 0);
}

// properties

fpdec_sign_t Decimal::sign() const noexcept {
    return fpdec.sign;
}

fpdec_dec_prec_t Decimal::precision() const noexcept {
    return fpdec.dec_prec;
}

int Decimal::magnitude() const {
    int magn = fpdec_magnitude(&fpdec);
    if (magn == -1 && errno == ERANGE)
        throw std::range_error("Result would be -Infinity.");
    return magn;
}

// operators

Decimal Decimal::operator+() const noexcept {
    return *this;
}

Decimal Decimal::operator-() const {
    auto dec = Decimal();
    error_t err = fpdec_neg(&dec.fpdec, &fpdec);
    if (err != FPDEC_OK)
        throw_exc(err);
    return dec;
}

bool Decimal::operator==(const Decimal &rhs) const noexcept {
    return fpdec_compare(&fpdec, &(rhs.fpdec), 0) == 0;
}

bool Decimal::operator!=(const Decimal &rhs) const noexcept {
    return fpdec_compare(&fpdec, &(rhs.fpdec), 0) != 0;
}

bool Decimal::operator<=(const Decimal &rhs) const noexcept {
    return fpdec_compare(&fpdec, &(rhs.fpdec), 0) <= 0;
}

bool Decimal::operator<(const Decimal &rhs) const noexcept {
    return fpdec_compare(&fpdec, &(rhs.fpdec), 0) < 0;
}

bool Decimal::operator>=(const Decimal &rhs) const noexcept {
    return fpdec_compare(&fpdec, &(rhs.fpdec), 0) >= 0;
}

bool Decimal::operator>(const Decimal &rhs) const noexcept {
    return fpdec_compare(&fpdec, &(rhs.fpdec), 0) > 0;
}

Decimal Decimal::operator+(const Decimal &rhs) const {
    auto dec = Decimal();
    error_t err = fpdec_add(&dec.fpdec, &fpdec, &rhs.fpdec);
    if (err != FPDEC_OK)
        throw_exc(err);
    return dec;
}

Decimal Decimal::operator-(const Decimal &rhs) const {
    auto dec = Decimal();
    error_t err = fpdec_sub(&dec.fpdec, &fpdec, &rhs.fpdec);
    if (err != FPDEC_OK)
        throw_exc(err);
    return dec;
}

Decimal Decimal::operator*(const Decimal &rhs) const {
    auto dec = Decimal();
    error_t err = fpdec_mul(&dec.fpdec, &fpdec, &rhs.fpdec);
    if (err != FPDEC_OK)
        throw_exc(err);
    return dec;
}

Decimal Decimal::operator/(const Decimal &rhs) const {
    auto dec = Decimal();
    error_t err = fpdec_div(&dec.fpdec, &fpdec, &rhs.fpdec, -1,
                            FPDEC_ROUND_DEFAULT);
    if (err != FPDEC_OK)
        throw_exc(err);
    return dec;
}

// member functions

std::string Decimal::dump() {
    std::ostringstream buf;
    buf << "flags:" << std::endl
        << "  dyn_alloc:  " << FPDEC_IS_DYN_ALLOC(&fpdec) << std::endl
        << "  normalized: " << FPDEC_IS_NORMALIZED(&fpdec) << std::endl
        << "sign: " << (int)FPDEC_SIGN(&fpdec) << std::endl
        << "dec_prec: " << FPDEC_DEC_PREC(&fpdec) << std::endl;
    if (FPDEC_IS_DYN_ALLOC(&fpdec)) {
        buf << "exp: " << FPDEC_EXP(&fpdec) << std::endl
            << "n digits: " << fpdec.digit_array->n_alloc << std::endl
            << "digits: ";
        for (int i = 0; i < fpdec.digit_array->n_alloc; ++i)
            buf << fpdec.digit_array->digits[i] << ", ";
    }
    else {
        buf << "digits: " << fpdec.lo << ", " << fpdec.hi;
    }
    buf << std::endl;
    return buf.str();
}

// interacting with integers

bool fpdec::operator==(const long long int lhs, const Decimal &rhs) noexcept {
    return rhs == (Decimal)lhs;
}

bool fpdec::operator!=(const long long int lhs, const Decimal &rhs) noexcept {
    return !(rhs == (Decimal)lhs);
}

bool fpdec::operator<=(const long long int lhs, const Decimal &rhs) noexcept {
    return rhs >= (Decimal)lhs;
}

bool fpdec::operator<(const long long int lhs, const Decimal &rhs) noexcept {
    return rhs > (Decimal)lhs;
}

bool fpdec::operator>=(const long long int lhs, const Decimal &rhs) noexcept {
    return rhs <= (Decimal)lhs;
}

bool fpdec::operator>(const long long int lhs, const Decimal &rhs) noexcept {
    return rhs < (Decimal)lhs;
}
