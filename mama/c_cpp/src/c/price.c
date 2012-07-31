/* $Id: price.c,v 1.7.6.2.14.4 2011/08/24 13:13:15 emmapollock Exp $
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

#include <mama/log.h>
#include <mama/price.h>
#include "mamaStrUtils.h"
#include "priceimpl.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>


mama_status
mamaPrice_create (mamaPrice*  price)
{
    mama_price_t* wPrice = (mamaPrice) malloc (sizeof(mama_price_t));
    if (wPrice == NULL)
    {
        return MAMA_STATUS_NOMEM;
    }
    else
    {
        mamaPriceImpl_clear(wPrice);
        *price = (mamaPrice)wPrice;
        return MAMA_STATUS_OK;
    }
}

mama_status
mamaPrice_destroy (mamaPrice  price)
{
    if (!price)
        return MAMA_STATUS_INVALID_ARG;

    free ((mama_price_t*)price);
    return MAMA_STATUS_OK;
}

mama_status
mamaPrice_clear (mamaPrice  price)
{
    if (!price)
        return MAMA_STATUS_INVALID_ARG;

    mamaPriceImpl_clear((mama_price_t*)price);
    return MAMA_STATUS_OK;
}

mama_status
mamaPrice_copy (mamaPrice        dest,
                const mamaPrice  src)
{
    if (!src || !dest)
        return MAMA_STATUS_INVALID_ARG;

    mamaPriceImpl_copy ((mama_price_t*)dest, (const mama_price_t*)src);
    return MAMA_STATUS_OK;
}

mama_status
mamaPrice_add (mamaPrice        dest,
               const mamaPrice  rhs)
{
    if (!dest || !rhs)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    else
    {
        double lhsValue = mamaPriceImpl_getValue((mama_price_t*)dest);
        double rhsValue = mamaPriceImpl_getValue((mama_price_t*)rhs);
        mamaPriceImpl_setValue ((mama_price_t*)dest, lhsValue + rhsValue);
        return MAMA_STATUS_OK;
    }
}

mama_status
mamaPrice_subtract (mamaPrice        dest,
                    const mamaPrice  rhs)
{
    if (!dest || !rhs)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    else
    {
        double lhsValue = mamaPriceImpl_getValue((mama_price_t*)dest);
        double rhsValue = mamaPriceImpl_getValue((mama_price_t*)rhs);
        mamaPriceImpl_setValue ((mama_price_t*)dest, lhsValue - rhsValue);
        return MAMA_STATUS_OK;
    }
}

int mamaPrice_equal (const mamaPrice  lhs,
                     const mamaPrice  rhs)
{
    if (lhs == rhs)
        return 1;
    if (!lhs || !rhs)
        return 0;
    return mamaPriceImpl_equal ((mama_price_t*)lhs, (mama_price_t*)rhs);
}

double mamaPrice_compare (const mamaPrice  lhs,
                          const mamaPrice  rhs)
{
    double lhsValue = mamaPriceImpl_getValue((mama_price_t*)lhs);
    double rhsValue = mamaPriceImpl_getValue((mama_price_t*)rhs);
    return (lhsValue - rhsValue);
}

mama_status
mamaPrice_setValue (mamaPrice           price,
                    double              value)
{
    if (!price)
        return MAMA_STATUS_INVALID_ARG;
    mamaPriceImpl_setValue ((mama_price_t*)price, value);
    mamaPrice_setIsValidPrice(price,1);
    return MAMA_STATUS_OK;
}

mama_status
mamaPrice_setPrecision (mamaPrice           price,
                        mamaPricePrecision  precision)
{
    mama_price_hints_t hints = 0;
    if (!price)
        return MAMA_STATUS_INVALID_ARG;
    hints = mamaPriceImpl_getHints((mama_price_t*)price);
    hints &= ~MAMA_PRICE_IMPL_HINT_PREC_MASK;
    hints |= (mama_price_hints_t)precision;
    mamaPriceImpl_setHints ((mama_price_t*)price, hints);
    return MAMA_STATUS_OK;
}

mama_status
mamaPrice_setIsValidPrice (mamaPrice    price,
                           mama_bool_t  valid)
{
    mama_price_hints_t hints = 0;
    if (!price)
        return MAMA_STATUS_INVALID_ARG;
    hints = mamaPriceImpl_getHints((mama_price_t*)price);
    if (valid)
    {
        hints |= MAMA_PRICE_IMPL_HINT_VALID_PRICE;
    }
    else
    {
        hints &= ~MAMA_PRICE_IMPL_HINT_VALID_PRICE;
    }
    mamaPriceImpl_setHints ((mama_price_t*)price, hints);
    return MAMA_STATUS_OK;
}



mama_status
mamaPrice_setHints (mamaPrice       price,
                    mamaPriceHints  hints)
{
    if (!price)
        return MAMA_STATUS_INVALID_ARG;
    mamaPriceImpl_setHints ((mama_price_t*)price, hints);
    return MAMA_STATUS_OK;
}

mama_status
mamaPrice_setWithHints (mamaPrice           price,
                        double              value,
                        mamaPriceHints      hints)
{
    if (!price)
        return MAMA_STATUS_INVALID_ARG;

    mamaPriceImpl_setWithHints ((mama_price_t*)price, value, hints);
    return MAMA_STATUS_OK;
}

mama_status
mamaPrice_setFromString(mamaPrice    price,
                        const char*  str)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
mamaPrice_getValue(const mamaPrice      price,
                   double*              value)
{
    if (!price || !value)
        return MAMA_STATUS_INVALID_ARG;
    *value = mamaPriceImpl_getValue ((mama_price_t*)price);
    return MAMA_STATUS_OK;
}

mama_status
mamaPrice_getRoundedValue(const mamaPrice           price,
                          const mamaPricePrecision  precision,
                          double*                   value)
{
	int multiplier=1;
	int i;
	mama_u8_t hints;
	double tempval = 0.0;

    if (!price || !value)
        return MAMA_STATUS_INVALID_ARG;

    if (precision)
    	hints = (mama_u8_t) precision;
    else
    	hints = mamaPriceImpl_getPrecision ((mama_price_t*)price);

    if (hints <= 10 && hints >0)
    {
    	for (i=0;i<hints;i++)
    		multiplier = multiplier*10;


    	tempval = mamaPriceImpl_getValue ((mama_price_t*)price);

		*value = floor(multiplier*(tempval) + 0.5) / multiplier;
    }
    else
    	*value = mamaPriceImpl_getValue ((mama_price_t*)price);

    return MAMA_STATUS_OK;
}

mama_status
mamaPrice_getPrecision(const mamaPrice      price,
                       mamaPricePrecision*  precision)
{
    if (!price || !precision)
        return MAMA_STATUS_INVALID_ARG;
    *precision = mamaPriceImpl_getHints ((mama_price_t*)price) & 
                 MAMA_PRICE_IMPL_HINT_PREC_MASK;
    return MAMA_STATUS_OK;
}

mama_status
mamaPrice_getIsValidPrice(const mamaPrice      price,
                          mama_bool_t*         valid)
{
    if (!price || !valid)
        return MAMA_STATUS_INVALID_ARG;
    *valid = mamaPriceImpl_getHints ((mama_price_t*)price) & 
                 MAMA_PRICE_IMPL_HINT_VALID_PRICE;
    return MAMA_STATUS_OK;
}

mama_status
mamaPrice_getHints(const mamaPrice  price,
                   mamaPriceHints*  hints)
{
    if (!price || !hints)
        return MAMA_STATUS_INVALID_ARG;
    *hints = mamaPriceImpl_getHints ((mama_price_t*)price);
    return MAMA_STATUS_OK;
}

mama_status
mamaPrice_getWithHints(const mamaPrice      price,
                       double*              value,
                       mamaPricePrecision*  precision)
{
    if (!price || !value || !precision)
        return MAMA_STATUS_INVALID_ARG;

    *value = mamaPriceImpl_getValue ((mama_price_t*)price);
    return mamaPrice_getPrecision(price, precision);
}

mama_status
mamaPrice_getAsString (const mamaPrice      price,
                       char*                buf,
                       mama_size_t          bufMaxLen)
{
    if (!price || !buf)
        return MAMA_STATUS_INVALID_ARG;

    mamaPriceImpl_getAsString ((mama_price_t*)price, buf, bufMaxLen);
    return MAMA_STATUS_OK;
}

mama_status
mamaPrice_negate (mamaPrice  price)
{
    if (!price)
    {
        return MAMA_STATUS_INVALID_ARG;
    }	
    else
    {
    	mamaPriceImpl_setValue ((mama_price_t*)price, -mamaPriceImpl_getValue ((mama_price_t*)price));
    	return MAMA_STATUS_OK;
    }
}

mama_status
mamaPrice_isZero (const mamaPrice  price,
                  mama_bool_t*     result)
{
    if (!price)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    else
    {
        double value = mamaPriceImpl_getValue ((mama_price_t*)price);
        *result =((value < MAMA_PRICE_EPSILON) &&
                  (value > -MAMA_PRICE_EPSILON));
        return MAMA_STATUS_OK;
    }
}

mamaPricePrecision
mamaPrice_decimals2Precision (mama_i32_t  places)
{
    if (places == 0)
        return MAMA_PRICE_PREC_INT;
    else if ((places > 0) && (places <= 10))
        return (mamaPricePrecision) places;
    else
        return MAMA_PRICE_PREC_UNKNOWN;
}

mamaPricePrecision
mamaPrice_denom2Precision (mama_i32_t  denominator)
{
    switch (denominator)
    {
    case 1:   return MAMA_PRICE_PREC_INT;
    case 2:   return MAMA_PRICE_PREC_DIV_2;
    case 4:   return MAMA_PRICE_PREC_DIV_4;
    case 8:   return MAMA_PRICE_PREC_DIV_8;
    case 16:  return MAMA_PRICE_PREC_DIV_16;
    case 32:  return MAMA_PRICE_PREC_DIV_32;
    case 64:  return MAMA_PRICE_PREC_DIV_64;
    case 128: return MAMA_PRICE_PREC_DIV_128;
    case 256: return MAMA_PRICE_PREC_DIV_256;
    case 512: return MAMA_PRICE_PREC_DIV_512;
    default:  return MAMA_PRICE_PREC_UNKNOWN;
    }
}

mama_i32_t
mamaPrice_precision2Decimals (mamaPricePrecision  precision)
{
    mama_i32_t  result = (mama_i32_t)precision;
    if ((result < 0) || (result > 10))
        result = 10;
    return result;
}

mama_i32_t
mamaPrice_precision2Denom (mamaPricePrecision  precision)
{
    switch (precision)
    {
    case MAMA_PRICE_PREC_INT:     return 1;
    case MAMA_PRICE_PREC_DIV_2:   return 2;
    case MAMA_PRICE_PREC_DIV_4:   return 4;
    case MAMA_PRICE_PREC_DIV_8:   return 8;
    case MAMA_PRICE_PREC_DIV_16:  return 16;
    case MAMA_PRICE_PREC_DIV_32:  return 32;
    case MAMA_PRICE_PREC_DIV_64:  return 64;
    case MAMA_PRICE_PREC_DIV_128: return 128;
    case MAMA_PRICE_PREC_DIV_256: return 256;
    case MAMA_PRICE_PREC_DIV_512: return 512;
    default:                      return 0;
    }
}
