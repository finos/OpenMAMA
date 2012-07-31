/* $Id: priceimpl.c,v 1.1.2.3 2011/10/02 19:02:17 ianbell Exp $
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

#include <mama/mama.h>
#include "priceimpl.h"

static void mamaPriceImpl_getAsStringDecimal  (double    value,
                                         mama_size_t    decimals,
                                         char*     buf,
                                         mama_size_t    bufMaxLen);

static void mamaPriceImpl_getAsStringFraction (double    value,
                                         uint32_t  denom,
                                         char*     buf,
                                         mama_size_t    bufMaxLen);

static void mamaPriceImpl_getAsStringSpecFraction (double    value,
                                             uint32_t  denom1,
                                             uint32_t  denom2,
                                             int       zeroPart,
                                             char*     buf,
                                             mama_size_t    bufMaxLen);

static void  mamaPriceImpl_getAsStringInteger     (double     value,
                                             char*      buf,
                                             mama_size_t     bufMaxLen);

void  mamaPriceImpl_getAsString (const mama_price_t*  price,
                           char*              buf,
                           mama_size_t             bufMaxLen)
{
    double            value     = price->mValue;
    mama_price_hints_t  precision = mamaPriceImpl_getPrecision (price);
    if (mamaPriceImpl_isDecimal(precision))
    {
        uint8_t dp = mamaPriceImpl_getDecimals (precision);
        mamaPriceImpl_getAsStringDecimal (value, dp, buf, bufMaxLen);
    }
    else if (mamaPriceImpl_isInteger (precision))
    {
        mamaPriceImpl_getAsStringInteger (value, buf, bufMaxLen);
    }
    else if (mamaPriceImpl_isFraction(precision))
    {
        uint32_t denom = mamaPriceImpl_getFractDenom (precision);
        mamaPriceImpl_getAsStringFraction (value, denom, buf, bufMaxLen);
    }
    else
    {
        switch (precision)
        {
        case MAMA_PRICE_IMPL_HINT_PREC_TICK_32:
            mamaPriceImpl_getAsStringSpecFraction (value, 32, 1, 1, buf, bufMaxLen);
            break;
        case MAMA_PRICE_IMPL_HINT_PREC_HALF_32:
            mamaPriceImpl_getAsStringSpecFraction (value, 32, 2, 1, buf, bufMaxLen);
            break;
        case MAMA_PRICE_IMPL_HINT_PREC_QUARTER_32:
            mamaPriceImpl_getAsStringSpecFraction (value, 32, 4, 0, buf, bufMaxLen);
            break;
        case MAMA_PRICE_IMPL_HINT_PREC_TICK_64:
            mamaPriceImpl_getAsStringSpecFraction (value, 64, 1, 1, buf, bufMaxLen);
            break;
        case MAMA_PRICE_IMPL_HINT_PREC_HALF_64:
            mamaPriceImpl_getAsStringSpecFraction (value, 64, 2, 0, buf, bufMaxLen);
            break;
        default:
            snprintf (buf, bufMaxLen, "%g", value);
        }
    }
}

void mamaPriceImpl_getAsStringDecimal  (double    value,
                                  mama_size_t    decimals,
                                  char*     buf,
                                  mama_size_t    bufMaxLen)
{
    snprintf (buf, bufMaxLen, "%.*f", (int)decimals, value);


}

void mamaPriceImpl_getAsStringInteger (double     value,
                                 char*      buf,
                                 mama_size_t     bufMaxLen)
{
    int64_t  integer    = (int64_t)value;
    snprintf (buf, bufMaxLen, "%lld", integer);
}


void mamaPriceImpl_getAsStringFraction (double    value,
                                  uint32_t  denom,
                                  char*     buf,
                                  mama_size_t    bufMaxLen)
{
    int64_t  integer  = (int64_t)value;
    double   fraction = value - (double)integer;
    int64_t  numer    = (int64_t)((double)denom * fraction);
    if (numer == 0)
    {
        snprintf (buf, bufMaxLen, "%lld", integer);
    }
    else if (integer == 0)
    {
		char formatSpecifier[20] = "";
		sprintf(formatSpecifier, "%s/%s", "%lld", "%ld");
        snprintf (buf, bufMaxLen, formatSpecifier, numer, denom);
    }
    else
    {
		char formatSpecifier[20] = "";
		sprintf(formatSpecifier, "%s %s/%s", "%lld", "%lld", "%ld");
        if (integer < 0)
		{
            numer = -numer;
		}
        snprintf (buf, bufMaxLen, formatSpecifier, integer, numer, denom);
    }
}

void mamaPriceImpl_getAsStringSpecFraction (double    value,
                                      uint32_t  denom1,
                                      uint32_t  denom2,
                                      int       zeroPart,
                                      char*     buf,
                                      mama_size_t    bufMaxLen)
{
    int32_t  integer   = (int32_t)value;
    double   fraction1 = value - (double)integer;
    int32_t  numer1    = (double)denom1 * fraction1;
    double   fraction2 = fraction1 - ((double)numer1/(double)denom1);
    int32_t  numer2    = 10 * (double)denom1 * fraction2;

    if (value < 0)
    {
        if (zeroPart && (numer2 == 0))
        {
            snprintf (buf, bufMaxLen, "-%d\'%02d", -integer, -numer1);
        }
        else
        {
            snprintf (buf, bufMaxLen, "-%d\'%02d%d", -integer, -numer1, -numer2);
        }
    }
    else
    {
        if (zeroPart && (numer2 == 0))
        {
            snprintf (buf, bufMaxLen, "%d\'%02d", integer, numer1);
        }
        else
        {
            snprintf (buf, bufMaxLen, "%d\'%02d%d", integer, numer1, numer2);
        }
    }
}
