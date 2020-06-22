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

    class InternalLimitExceeded : std::range_error {
    public:
        InternalLimitExceeded() :
            std::range_error("Internal limit exceeded.") {
        };
    };

    class InvalidDecimalLiteral : std::invalid_argument {
    public:
        std::string invalid_literal;
        InvalidDecimalLiteral(const std::string lit) :
            std::invalid_argument("Invalid Decimal literal"),
            invalid_literal(lit) {
        };
    };

    class DivisionByZero : std::range_error {
    public:
        DivisionByZero() :
            std::range_error("Division by zero.") {
        };
    };

    class Decimal {
    public:
        Decimal();
        Decimal(const Decimal&);
        Decimal(Decimal&&) = default;
        Decimal(const std::string&);
        Decimal(const long long int);
        ~Decimal();
        // properties
        fpdec_sign_t sign() const;
        fpdec_dec_prec_t precision() const;
        int magnitude() const;
        // operators
        Decimal& operator=(const Decimal&) = default;
        Decimal& operator=(Decimal&&) = default;
        Decimal operator+() const;
        Decimal operator-() const;
        bool operator==(Decimal&);
        template<typename T>
        friend bool operator==(Decimal&, T&);
        template<typename T>
        friend bool operator==(T&, Decimal&);
        template<typename T>
        friend bool operator!=(Decimal&, T&);
        template<typename T>
        friend bool operator!=(T&, Decimal&);

    private:
        fpdec_t fpdec;
        Decimal(const fpdec_t*);
    };

}; // namespace fpdec

#endif //FPDEC_FPDEC_HPP
