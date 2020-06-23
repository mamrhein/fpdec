/* ---------------------------------------------------------------------------
Name:        fpdec.hpp

Author:      Michael Amrhein (michael@adrhinum.de)

Copyright:   (c) 2020 ff. Michael Amrhein
License:     This program is part of a larger application. For license
             details please read the file LICENSE.TXT provided together
             with the application.
------------------------------------------------------------------------------
$Source$
$Revision$
*/

#ifndef FPDEC_FPDEC_HPP
#define FPDEC_FPDEC_HPP

#include <stdexcept>
#include <string>
#include "common.h"

namespace fpdec {

    class InternalLimitExceeded : public std::range_error {
    public:
        error_t error;
        InternalLimitExceeded(error_t err) :
            std::range_error("Internal limit exceeded."),
            error(err) {
        };
    };

    class InvalidDecimalLiteral : public std::invalid_argument {
    public:
        std::string invalid_literal;
        InvalidDecimalLiteral(const std::string lit) :
            std::invalid_argument("Invalid Decimal literal"),
            invalid_literal(lit) {
        };
    };

    class DivisionByZero : public std::range_error {
    public:
        DivisionByZero() :
            std::range_error("Division by zero.") {
        };
    };

    class Decimal {
    public:
        Decimal() noexcept;
        Decimal(const Decimal&);
        Decimal(Decimal&&) = default;
        Decimal(const std::string&);
        Decimal(const long long int) noexcept;
        ~Decimal();
        // properties
        fpdec_sign_t sign() const noexcept;
        fpdec_dec_prec_t precision() const noexcept;
        int magnitude() const;
        // operators
        Decimal& operator=(const Decimal&) = default;
        Decimal& operator=(Decimal&&) = default;
        Decimal operator+() const noexcept;
        Decimal operator-() const;
        bool operator==(const Decimal&) const noexcept;
        bool operator!=(const Decimal&) const noexcept;
        bool operator<=(const Decimal&) const noexcept;
        bool operator<(const Decimal&) const noexcept;
        bool operator>=(const Decimal&) const noexcept;
        bool operator>(const Decimal&) const noexcept;

    private:
        fpdec_t fpdec;
        Decimal(const fpdec_t*);
    };

    // interacting with integers
    bool operator==(const long long int, const Decimal&) noexcept;
    bool operator!=(const long long int, const Decimal&) noexcept;
    bool operator<=(const long long int, const Decimal&) noexcept;
    bool operator<(const long long int, const Decimal&) noexcept;
    bool operator>=(const long long int, const Decimal&) noexcept;
    bool operator>(const long long int, const Decimal&) noexcept;

}; // namespace fpdec

#endif //FPDEC_FPDEC_HPP
