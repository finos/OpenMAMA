/* $Id: priceimpl.h,v 1.1.2.3 2011/09/29 08:27:47 emmapollock Exp $
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

#ifndef MAMA_PRICE_IMPL_H
#define MAMA_PRICE_IMPL_H

#if defined(__cplusplus)
extern "C" {
#endif

typedef unsigned char  mama_price_hints_t;

typedef struct mama_price_t_
{
    double              mValue;
    mama_price_hints_t  mHints;
} mama_price_t;


/* The bitmask for the price's precision consists of the 5 (mask=0x1f)
 * lowest order bits of the 8 bit hint. */

#define MAMA_PRICE_IMPL_HINT_PREC_MASK          ((mama_price_hints_t)0x1f)

#define MAMA_PRICE_IMPL_HINT_PREC_UNKNOWN       ((mama_price_hints_t)0x00)
#define MAMA_PRICE_IMPL_HINT_PREC_10            ((mama_price_hints_t)0x01)
#define MAMA_PRICE_IMPL_HINT_PREC_100           ((mama_price_hints_t)0x02)
#define MAMA_PRICE_IMPL_HINT_PREC_1000          ((mama_price_hints_t)0x03)
#define MAMA_PRICE_IMPL_HINT_PREC_10000         ((mama_price_hints_t)0x04)
#define MAMA_PRICE_IMPL_HINT_PREC_100000        ((mama_price_hints_t)0x05)
#define MAMA_PRICE_IMPL_HINT_PREC_1000000       ((mama_price_hints_t)0x06)
#define MAMA_PRICE_IMPL_HINT_PREC_10000000      ((mama_price_hints_t)0x07)
#define MAMA_PRICE_IMPL_HINT_PREC_100000000     ((mama_price_hints_t)0x08)
#define MAMA_PRICE_IMPL_HINT_PREC_1000000000    ((mama_price_hints_t)0x09)
#define MAMA_PRICE_IMPL_HINT_PREC_10000000000   ((mama_price_hints_t)0x0a)
#define MAMA_PRICE_IMPL_HINT_PREC_INT           ((mama_price_hints_t)0x10)
#define MAMA_PRICE_IMPL_HINT_PREC_2             ((mama_price_hints_t)0x11)
#define MAMA_PRICE_IMPL_HINT_PREC_4             ((mama_price_hints_t)0x12)
#define MAMA_PRICE_IMPL_HINT_PREC_8             ((mama_price_hints_t)0x13)
#define MAMA_PRICE_IMPL_HINT_PREC_16            ((mama_price_hints_t)0x14)
#define MAMA_PRICE_IMPL_HINT_PREC_32            ((mama_price_hints_t)0x15)
#define MAMA_PRICE_IMPL_HINT_PREC_64            ((mama_price_hints_t)0x16)
#define MAMA_PRICE_IMPL_HINT_PREC_128           ((mama_price_hints_t)0x17)
#define MAMA_PRICE_IMPL_HINT_PREC_256           ((mama_price_hints_t)0x18)
#define MAMA_PRICE_IMPL_HINT_PREC_512           ((mama_price_hints_t)0x19)
#define MAMA_PRICE_IMPL_HINT_PREC_TICK_32       ((mama_price_hints_t)0x1a)
#define MAMA_PRICE_IMPL_HINT_PREC_HALF_32       ((mama_price_hints_t)0x1b)
#define MAMA_PRICE_IMPL_HINT_PREC_QUARTER_32    ((mama_price_hints_t)0x1c)
#define MAMA_PRICE_IMPL_HINT_PREC_TICK_64       ((mama_price_hints_t)0x1d)
#define MAMA_PRICE_IMPL_HINT_PREC_HALF_64       ((mama_price_hints_t)0x1e)

#define MAMA_PRICE_IMPL_HINT_VALID_PRICE           ((mama_price_hints_t)0x20)

#define mamaPriceImpl_clear(price) \
    ((price)->mValue = 0.0, (price)->mHints = MAMA_PRICE_IMPL_HINT_PREC_UNKNOWN)

#define mamaPriceImpl_copy(lhs,rhs) \
    ((lhs)->mValue = (rhs)->mValue, (lhs)->mHints = (rhs)->mHints)

#define mamaPriceImpl_equal(lhs,rhs)   ((lhs)->mValue == (rhs)->mValue)

#define mamaPriceImpl_getValue(price)  ((price)->mValue)

#define mamaPriceImpl_getHints(price)  ((price)->mHints)

#define mamaPriceImpl_getPrecision(price)  ((mamaPriceImpl_getHints(price) & \
                                      MAMA_PRICE_IMPL_HINT_PREC_MASK))

#define mamaPriceImpl_setValue(price,value) \
    ((price)->mValue = value)

#define mamaPriceImpl_setHints(price,hints) \
    ((price)->mHints = hints)

#define mamaPriceImpl_setWithHints(price,value,hints) \
    (mamaPriceImpl_setValue(price,value), mamaPriceImpl_setHints(price,hints))

#define mamaPriceImpl_clearPrecision(price) \
    ((price)->mHints &= ~MAMA_PRICE_IMPL_HINT_PREC_MASK)

#define mamaPriceImpl_isDecimal(precision) \
    (((precision) >= MAMA_PRICE_IMPL_HINT_PREC_10) && \
     ((precision) <= MAMA_PRICE_IMPL_HINT_PREC_10000000000))

#define mamaPriceImpl_isInteger(precision) \
     ((precision) == MAMA_PRICE_IMPL_HINT_PREC_INT)

#define mamaPriceImpl_isFraction(precision) \
    (((precision) >= MAMA_PRICE_IMPL_HINT_PREC_2) && \
     ((precision) <= MAMA_PRICE_IMPL_HINT_PREC_512))

#define mamaPriceImpl_getDecimals(precision)    (precision)
#define mamaPriceImpl_getFractDenom(precision) \
    (1 << ((precision) & 0x0f))

extern void  mamaPriceImpl_getAsString (const mama_price_t*  price,
                                        char*              buf,
                                        size_t             bufMaxLen);

#if defined(__cplusplus)
}
#endif

#endif
