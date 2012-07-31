/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Technologies, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifndef MAMA_PRICE_CPP_H__
#define MAMA_PRICE_CPP_H__

#include <mama/price.h>

static const int MAX_PRICE_STR_LEN = 32;

namespace Wombat 
{

    /**
     * MamaPrice is a special data type for representing floating point
     * numbers that often require special formatting for display purposes,
     * such as prices.  MamaPrice contains the 64-bit (double precision)
     * floating point value and an optional display hint.  The set of
     * display hints includes hints for:
     *  - a number of decimal places,
     *  - a fractional denominator that are powers of two, and
     *  - hints for special denominators used in the finance industry
     *    (e.g., halves of 32nds).
     */
    class MAMACPPExpDLL MamaPrice
    {
    public:
        MamaPrice ();
        MamaPrice (double              value,
                   mamaPricePrecision  precision = MAMA_PRICE_PREC_100);
        MamaPrice (const MamaPrice&    copy);

        ~MamaPrice ();

        MamaPrice&          operator=  (const MamaPrice& rhs);
        MamaPrice&          operator+= (const MamaPrice& rhs);
        MamaPrice&          operator-= (const MamaPrice& rhs);

        bool                operator== (const MamaPrice& rhs) const;
        bool                operator== (double           rhs) const;
        bool                operator!= (const MamaPrice& rhs) const
        {
            return ! operator== (rhs);
        }
        bool                operator!= (double           rhs) const
        {
            return ! operator== (rhs);
        }
        bool                operator<  (const MamaPrice& rhs) const;
        bool                operator<  (double           rhs) const;
        bool                operator>  (const MamaPrice& rhs) const;
        bool                operator>  (double           rhs) const;

        /**
         * Subtraction operator.  Note: this operator creates a temporary
         * object.
         */
        MamaPrice operator- (const MamaPrice& rhs) const
        {
            return MamaPrice (getValue() - rhs.getValue(), 
                              rhs.getPrecision ());
        }

        /**
         * Subtraction operator for double.  Note: this operator creates a
         * temporary object.
         */
        MamaPrice operator- (double rhs) const
	    {
            return MamaPrice (getValue     () - rhs,
                              getPrecision ());
        }

        /**
         * Negation operator.  Note: this operator creates a temporary
         * object.
         */
        MamaPrice operator- () const
        {
            return MamaPrice (-getValue    (),
                              getPrecision ());
        }

        /**
         * Addition operator.  Note: this operator creates a temporary object.
         */
        MamaPrice operator+ (const MamaPrice& rhs) const
        {
            return MamaPrice (getValue         () + rhs.getValue (),
                              rhs.getPrecision ());
        }

        /**
         * Addition operator for double.  Note: this operator creates a
         * temporary object.
         */
        MamaPrice operator+ (double rhs) const
        {
            return MamaPrice (getValue     () + rhs,
                              getPrecision ());
        }

        double              compare         (const MamaPrice&    rhs) const;

        void                clear           ();

        void                set             (double              priceValue,
                                             mamaPriceHints      hints = 0);
        void                setValue        (double              value);
        void                setPrecision    (mamaPricePrecision  precision);
        void                setHints        (mamaPriceHints      hints);
        void                setIsValidPrice (bool                valid);

        double              getValue        () const;
        mamaPricePrecision  getPrecision    () const;
        mamaPriceHints      getHints        () const;
        bool                getIsValidPrice () const;


        void                setFromString   (const char*         str);
        void                getAsString     (char*               result,
                                             mama_size_t         maxLen) const;

        /**
         * Return a string representation of the price.  Note that the
         * alternative getAsString() method is more efficient because
         * this method must allocate a temporary buffer (automatically
         * destroyed upon object destruction).
         */
        const char*         getAsString     () const;

        /**
         * Negate the price value.  Hints and precisions are not affected.
         */
        void negate ();

        /**
         * Return whether the price has a value equivalent to zero.  It
         * may not be exactly 0.0, but we check against +/- epsilon.
         */
        bool isZero () const;

        /**
         * Return the appropriate precision code for a given number of
         * decimal places.
         */
        static mamaPricePrecision decimals2Precision (mama_i32_t  places);

        /**
         * Return the appropriate precision code for a given fractional
         * denominator.
         */
        static mamaPricePrecision denom2Precision (mama_i32_t  denominator);

        /**
         * Return the number of decimal places for a given precision code.
         */
        static mama_i32_t precision2Decimals (mamaPricePrecision  precision);

        /**
         * Return the fractional denominator for a given precision code.
         */
        static mama_i32_t precision2Denom (mamaPricePrecision  precision);

        mamaPrice        getCValue();
        const mamaPrice  getCValue() const;

    private:
        mamaPrice       mPrice;
        mutable char*   mStrRep;
};

} // namespace Wombat


#endif // MAMA_PRICE_CPP_H__
