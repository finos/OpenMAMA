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

#include "mama/MamaPrice.h"
#include "mamacppinternal.h"

namespace Wombat
{

    MamaPrice::MamaPrice ()
        : mStrRep (0)
    {
        mamaPrice_create (&mPrice);
    }

    MamaPrice::MamaPrice (double              value,
                          mamaPricePrecision  precision)
        : mStrRep (0)
    {
        mamaPrice_create       (&mPrice);
        mamaPrice_setValue     (mPrice, value);
        mamaPrice_setPrecision (mPrice, precision);
    }

    MamaPrice::MamaPrice (const MamaPrice& copy)
        : mStrRep (0)
    {
        mamaPrice_create (&mPrice);
        mamaPrice_copy   (mPrice, copy.mPrice);
    }

    MamaPrice::~MamaPrice ()
    {
        delete[] mStrRep;
        mamaPrice_destroy (mPrice);
    }

    MamaPrice& MamaPrice::operator= (const MamaPrice& rhs)
    {
        if (this != &rhs)
        {
            mamaPrice_copy (mPrice, rhs.mPrice);
        }
        return *this;
    }

    MamaPrice& MamaPrice::operator+= (const MamaPrice& rhs)
    {
        mamaPrice_add (mPrice, rhs.mPrice);
        return *this;
    }

    MamaPrice& MamaPrice::operator-= (const MamaPrice& rhs)
    {
        mamaPrice_subtract (mPrice, rhs.mPrice);
        return *this;
    }

    double MamaPrice::compare (const MamaPrice&  rhs) const
    {
        return mamaPrice_compare (mPrice, rhs.mPrice);
    }

    void MamaPrice::clear ()
    {
        mamaPrice_clear (mPrice);
    }

    void MamaPrice::set (double          priceValue,
                         mamaPriceHints  hints)
    {
        mamaPrice_setWithHints (mPrice, priceValue, hints);
    }

    void MamaPrice::setValue (double value)
    {
        mamaPrice_setValue (mPrice, value);
    }

    void MamaPrice::setPrecision (mamaPricePrecision precision)
    {
        mamaPrice_setPrecision (mPrice, precision);
    }

    void MamaPrice::setHints (mamaPriceHints hints)
    {
        mamaPrice_setHints (mPrice, hints);
    }

    double MamaPrice::getValue () const
    {
        double result = 0.0;
        mamaTry (mamaPrice_getValue (mPrice, &result));
        return result;
    }

    mamaPricePrecision MamaPrice::getPrecision () const
    {
        mamaPricePrecision result = MAMA_PRICE_PREC_UNKNOWN;
        mamaTry (mamaPrice_getPrecision (mPrice, &result));
        return result;
    }

    mamaPriceHints MamaPrice::getHints () const
    {
        mamaPriceHints result = 0;
        mamaTry (mamaPrice_getHints (mPrice, &result));
        return result;
    }

    void MamaPrice::setFromString (const char*  str)
    {
        mamaPrice_setFromString (mPrice, str);
    }

    void MamaPrice::getAsString (char*        result,
                                 mama_size_t  maxLen) const
    {
        mamaPrice_getAsString (mPrice, result, maxLen);
    }

    const char* MamaPrice::getAsString () const
    {
        if (!mStrRep)
        {
            mStrRep = new char [MAMA_PRICE_MAX_STR_LEN + 1];
        }
        getAsString (mStrRep, MAMA_PRICE_MAX_STR_LEN);
        return mStrRep;
    }

    bool MamaPrice::operator== (const MamaPrice& rhs) const
    {
        return (this == &rhs) || mamaPrice_equal (mPrice, rhs.mPrice);
    }

    bool MamaPrice::operator== (double  rhs) const
    {
        return (getValue() == rhs);
    }

    bool MamaPrice::operator> (const MamaPrice& rhs) const
    {
        return mamaPrice_compare (mPrice, rhs.mPrice) > 0;
    }

    bool MamaPrice::operator> (double  rhs) const
    {
        return (getValue() > rhs);
    }

    bool MamaPrice::operator< (const MamaPrice& rhs) const
    {
        return mamaPrice_compare (mPrice, rhs.mPrice) < 0;
    }

    bool MamaPrice::operator< (double  rhs) const
    {
        return (getValue() < rhs);
    }

    void MamaPrice::negate ()
    {
        mamaPrice_negate (mPrice);
    }

    bool MamaPrice::getIsValidPrice () const
    {
        mama_bool_t result = 0;
        mamaPrice_getIsValidPrice (mPrice, &result);
        return result;
    }

    void MamaPrice::setIsValidPrice (bool valid)
    {
        mamaPrice_setIsValidPrice (mPrice, valid);
    }

    bool MamaPrice::isZero () const
    {
        mama_bool_t result = 0;
        mamaPrice_isZero (mPrice, &result);
        return result;
    }

    mamaPrice MamaPrice::getCValue ()
    {
        return mPrice;
    }

    const mamaPrice MamaPrice::getCValue() const
    {
        return mPrice;
    }

    mamaPricePrecision MamaPrice::decimals2Precision (mama_i32_t decimalPlaces)
    {
        return mamaPrice_decimals2Precision (decimalPlaces);
    }

    mamaPricePrecision MamaPrice::denom2Precision (mama_i32_t denominator)
    {
        return mamaPrice_denom2Precision (denominator);
    }

    mama_i32_t  MamaPrice::precision2Decimals (mamaPricePrecision  precision)
    {
        return mamaPrice_precision2Decimals (precision);
    }

    mama_i32_t  MamaPrice::precision2Denom (mamaPricePrecision  precision)
    {
        return mamaPrice_precision2Denom (precision);
    }

} /* namespace  Wombat */
