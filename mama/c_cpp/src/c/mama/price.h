/* $Id: price.h,v 1.7.14.2.10.3 2011/08/10 14:53:28 nicholasmarriott Exp $
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

#ifndef MamaPriceH__
#define MamaPriceH__

#include <mama/config.h>
#include <mama/status.h>
#include <mama/types.h>


#if defined(__cplusplus)
extern "C" {
#endif


typedef enum mamaPricePrecision_
{
    MAMA_PRICE_PREC_UNKNOWN      = 0,
    MAMA_PRICE_PREC_10           = 1,
    MAMA_PRICE_PREC_100          = 2,
    MAMA_PRICE_PREC_1000         = 3,
    MAMA_PRICE_PREC_10000        = 4,
    MAMA_PRICE_PREC_100000       = 5,
    MAMA_PRICE_PREC_1000000      = 6,
    MAMA_PRICE_PREC_10000000     = 7,
    MAMA_PRICE_PREC_100000000    = 8,
    MAMA_PRICE_PREC_1000000000   = 9,
    MAMA_PRICE_PREC_10000000000  = 10,
    MAMA_PRICE_PREC_INT          = 16,
    MAMA_PRICE_PREC_DIV_2        = 17,
    MAMA_PRICE_PREC_DIV_4        = 18,
    MAMA_PRICE_PREC_DIV_8        = 19,
    MAMA_PRICE_PREC_DIV_16       = 20,
    MAMA_PRICE_PREC_DIV_32       = 21,
    MAMA_PRICE_PREC_DIV_64       = 22,
    MAMA_PRICE_PREC_DIV_128      = 23,
    MAMA_PRICE_PREC_DIV_256      = 24,
    MAMA_PRICE_PREC_DIV_512      = 25,
    MAMA_PRICE_PREC_TICK_32      = 26,
    MAMA_PRICE_PREC_HALF_32      = 27,
    MAMA_PRICE_PREC_QUARTER_32   = 28,
    MAMA_PRICE_PREC_TICK_64      = 29,
    MAMA_PRICE_PREC_HALF_64      = 30,
    MAMA_PRICE_PREC_CENTS        = MAMA_PRICE_PREC_100,
    MAMA_PRICE_PREC_PENNIES      = MAMA_PRICE_PREC_100
} mamaPricePrecision;

typedef mama_u8_t  mamaPriceHints;
#define MAMA_PRICE_HINTS_NONE     ((mamaPriceHints)0x00)

#define MAMA_PRICE_MAX_STR_LEN (32)


/**
 * MAMA_PRICE_EPSILON a value that is considered equivalent to zero.
 */
#define MAMA_PRICE_EPSILON   ((mama_f64_t)0.00000000001)

                           
/**
 * Create a mamaPrice object.
 *
 * @param price The location of a mamaPrice where to store the result.
 */
MAMAExpDLL
extern mama_status
mamaPrice_create (mamaPrice*  price);

/**
 * Destroy a mamaPrice object.
 *
 * @param price  The price object to destroy.
 */
MAMAExpDLL
extern mama_status
mamaPrice_destroy (mamaPrice  price);

/**
 * Clear a mamaPrice object.
 *
 * @param price  The price object to clear.
 */
MAMAExpDLL
extern mama_status
mamaPrice_clear (mamaPrice  price);

/**
 * Copy a price object. The destination object must have already
 * been allocated using mamaPrice_create().
 *
 * @param copy The destination price. 
 * @param src The price to copy.
 */
MAMAExpDLL
extern mama_status
mamaPrice_copy (mamaPrice         dest,
                const mamaPrice   src);

/**
 * Add to a price object. The destination object must have already
 * been allocated using mamaPrice_create().
 *
 * @param dest The price to add to.
 * @param rhs The price to add.
 */
MAMAExpDLL
extern mama_status
mamaPrice_add (mamaPrice         dest,
               const mamaPrice   rhs);

/**
 * Subtract from a price object. The destination object must have already
 * been allocated using mamaPrice_create().
 *
 * @param dest The price to subtract from.
 * @param src The price to subtract.
 */
MAMAExpDLL
extern mama_status
mamaPrice_subtract (mamaPrice         dest,
                    const mamaPrice   rhs);

/**
 * Check for equality between two price objects.
 *
 *  @param lhs The first price to compare.
 *  @param rhs The second price to compare.
 */
MAMAExpDLL
extern int
mamaPrice_equal (const mamaPrice   lhs,
                 const mamaPrice   rhs);

/**
 * Compare two price objects.  The return value is negative if lhs
 * is earlier than rhs, positive if lhs is greater than rhs and zero
 * if the two are equal.
 *
 *  @param lhs The first price to compare.
 *  @param rhs The second price to compare.
 */
MAMAExpDLL
extern double
mamaPrice_compare (const mamaPrice   lhs,
                   const mamaPrice   rhs);

/**
 * Set the price value only (no hint information).
 *
 * @param price      The price to set.
 * @param value      The value to set.
 */
MAMAExpDLL
extern mama_status
mamaPrice_setValue (mamaPrice  price,
                    double     value);

/**
 * Set the price precision.
 *
 * @param price      The price to set.
 * @param precision  The precision for the price.
 */
MAMAExpDLL
extern mama_status
mamaPrice_setPrecision (mamaPrice           price,
                        mamaPricePrecision  precision);

/**
 * Set if price contains a valid value. (0 may be a valid value)
 *
 * @param price      The price to set.
 * @param valid      If price contains a valid value.
 */
MAMAExpDLL
extern mama_status
mamaPrice_setIsValidPrice (mamaPrice   price,
                           mama_bool_t valid);

                        
/**
 * Set all of the price hints.
 *
 * @param price      The price to set.
 * @param hints      The hints for the price.
 */
MAMAExpDLL
extern mama_status
mamaPrice_setHints (mamaPrice       price,
                    mamaPriceHints  hints);

/**
 * Set the price with hint information.
 *
 * @param price      The price to set.
 * @param value      The value to set.
 * @param hints      The price hints.
 */
MAMAExpDLL
extern mama_status
mamaPrice_setWithHints (mamaPrice       price,
                        double          value,
                        mamaPriceHints  hints);

/**
 * Set the price from a string representation, preserving any
 * detectable hints.
 *
 * @param price  The price to set.
 * @param str    The string representation of some price.
 */
MAMAExpDLL
extern mama_status
mamaPrice_setFromString(mamaPrice    price,
                        const char*  str);

/**
 * Get the price value only.
 *
 * @param price         The price to get.
 * @param value         The the decimal value of the price.
 */
MAMAExpDLL
extern mama_status
mamaPrice_getValue(const mamaPrice      price,
                   double*              value);

/**
 * This function obtains the double value of a price rounded to the supplied precision. 
 *
 * @param[in]	price       The price to round.
 * @param[in] 	precision 	The precision to use for rounding.
 * @param[out] 	value    	The rounded value.
 * @return Indicates whether the function succeeded or failed and could be one of:
 *				- MAMA_STATUS_INVALID_ARG
 *				- MAMA_STATUS_OK
 */
MAMAExpDLL
mama_status
mamaPrice_getRoundedValue(const mamaPrice      price,
                          const mamaPricePrecision  precision,
                          double*              value);

/**
 * Get the price precision.
 *
 * @param price          The price to get.
 * @param precision      The price precision hint.
 */
MAMAExpDLL
extern mama_status
mamaPrice_getPrecision(const mamaPrice      price,
                       mamaPricePrecision*  precision);

/**
 * Check if price contains a valid value.
 *
 * @param price          The price to get.
 * @param valid          If the value is valid.
 */    
MAMAExpDLL
extern mama_status
mamaPrice_getIsValidPrice (const mamaPrice price,
                           mama_bool_t*    valid);

/**
 * Get the price hints.
 *
 * @param price         The price to get.
 * @param hints          The price hints.
 */
MAMAExpDLL
extern mama_status
mamaPrice_getHints(const mamaPrice      price,
                   mamaPriceHints*      hints);

/**
 * Get the price with special, optional hints.
 *
 * @param price         The price to get.
 * @param value         The decimal value of the price.
 * @param precision     The precision of the price.
 */
MAMAExpDLL
extern mama_status
mamaPrice_getWithHints(const mamaPrice      price,
                       double*              value,
                       mamaPricePrecision*  precision);

/**
 * Get the price as a string, formatted according to the hints provided.
 *
 * @param price      The price to set.
 * @param str        The string buffer to update.
 * @param maxLen     The maximum size of the string buffer (including trailing
 * '\\0').
 */
MAMAExpDLL
extern mama_status
mamaPrice_getAsString(const mamaPrice      price,
                      char*                str,
                      mama_size_t          maxLen);

/**
 * Negate the price value.  Hints and precisions are not affected.
 */
MAMAExpDLL
extern mama_status
mamaPrice_negate(mamaPrice  price);

/**
 * Return whether the price has a value equivalent to zero.  It
 * may not be exactly 0.0, but we check against +/- epsilon.
 */
MAMAExpDLL
extern mama_status
mamaPrice_isZero(const mamaPrice  price,
                 mama_bool_t*     result);

/**
 * Return the appropriate precision code for a given number of
 * decimal places.
 */
MAMAExpDLL
extern  mamaPricePrecision
mamaPrice_decimals2Precision (mama_i32_t  places);

/**
 * Return the appropriate precision code for a given fractional
 * denominator.
 */
MAMAExpDLL
extern  mamaPricePrecision
mamaPrice_denom2Precision (mama_i32_t  denominator);

/**
 * Return the number of decimal places for a given precision code.
 */
MAMAExpDLL
extern  mama_i32_t
mamaPrice_precision2Decimals (mamaPricePrecision  precision);

/**
 * Return the fractional denominator for a given precision code.
 */
MAMAExpDLL
extern  mama_i32_t
mamaPrice_precision2Denom (mamaPricePrecision  precision);

#if defined(__cplusplus)
}
#endif

#endif
