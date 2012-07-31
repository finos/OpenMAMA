/* $Id: msgqualifier.h,v 1.6.32.3 2011/08/10 14:53:28 nicholasmarriott Exp $
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

#ifndef MSG_QUALIFIER_H__
#define MSG_QUALIFIER_H__

#include <mama/config.h>
#include <mama/types.h>
#include <mama/status.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define MAMA_MSG_QUAL_MAX_STR_LEN (100)

#define MAMA_MSG_QUAL_POSSIBLY_DUPLICATE    0x0001
#define MAMA_MSG_QUAL_DEFINITELY_DUPLICATE  0x0002
#define MAMA_MSG_QUAL_POSSIBLY_DELAYED      0x0004
#define MAMA_MSG_QUAL_DEFINITELY_DELAYED    0x0008
#define MAMA_MSG_QUAL_OUT_OF_SEQUENCE       0x0010
#define MAMA_MSG_QUAL_NOT_USED              0x1000

/**
 * Create a mamaMsgQual object.
 *
 * @param msgQual The location of a mamaMsgQual where to store the result.
 */
MAMAExpDLL
extern mama_status
mamaMsgQual_create (
    mamaMsgQual*            msgQual);

/**
 * Destroy a mamaMsgQual object.
 *
 * @param msgQual  The object to destroy.
 */
MAMAExpDLL
extern mama_status
mamaMsgQual_destroy (
    mamaMsgQual             msgQual);

/**
 * Clear a mamaMsgQual object.
 *
 * @param msgQual  The object to clear.
 */
MAMAExpDLL
extern mama_status
mamaMsgQual_clear (
    mamaMsgQual             msgQual);

/**
 * Copy a mamaMsgQual object.  The destination object must have already
 * been allocated using mamaMsgQual_create().
 * @param dest The destination mamaMsgQual. 
 * @param src The mamaMsgQual to copy.
 */
MAMAExpDLL
extern mama_status
mamaMsgQual_copy (
    mamaMsgQual             dest,
    const mamaMsgQual       src);

/**
 * Check for equality between two objects.
 *  @param lhs  The first object to compare.
 *  @param rhs  The second object to compare.
 */
MAMAExpDLL
extern int
mamaMsgQual_equal (
    const mamaMsgQual       lhs,
    const mamaMsgQual       rhs);

/**
 * Set the value.
 *
 * @param msgQual    The object to set.
 * @param value      The value to set to.
 */
MAMAExpDLL
extern mama_status
mamaMsgQual_setValue (
    mamaMsgQual             msgQual, 
    mama_u16_t              value);

/**
 * Set the bit associated with the Definitely Duplicate condition.
 *
 * @param msgQual    The object to set.
 * @param state      Zero value clears Definitely Duplicate bit.
 *                   Non-zero value sets Definitely Duplicate bit.
 */
MAMAExpDLL
extern mama_status
mamaMsgQual_setIsDefinitelyDuplicate (
    mamaMsgQual             msgQual, 
    int                     state);

/**
 * Set the bit associated with the Possibly Duplicate condition.
 *
 * @param msgQual    The object to set.
 * @param state      Zero value clears Possibly Duplicate bit.
 *                   Non-zero value sets Possibly Duplicate bit.
 */
MAMAExpDLL
extern mama_status
mamaMsgQual_setIsPossiblyDuplicate (
    mamaMsgQual             msgQual,                                 
    int                     state);

/**
 * Set the bit associated with the Definitely Delayed condition.
 *
 * @param msgQual    The object to set.
 * @param state      Zero value clears Definitely Delayed bit.
 *                   Non-zero value sets Definitely Delayed bit.
 */
MAMAExpDLL
extern mama_status
mamaMsgQual_setIsDefinitelyDelayed (
    mamaMsgQual             msgQual, 
    int                     state);

/**
 * Set the bit associated with the Possibly Delayed condition.
 *
 * @param msgQual    The object to set.
 * @param state      Zero value clears Possibly Delayed bit.
 *                   Non-zero value sets Possibly Delayed bit.
 */
MAMAExpDLL
extern mama_status
mamaMsgQual_setIsPossiblyDelayed (
    mamaMsgQual             msgQual, 
    int                     state);

/**
 * Set the bit associated with the Out Of Sequence condition.
 *
 * @param msgQual    The object to set.
 * @param state      Zero value clears Out Of Sequence bit.
 *                   Non-zero value sets Out Of Sequence bit.
 */
MAMAExpDLL
extern mama_status
mamaMsgQual_setIsOutOfSequence (
    mamaMsgQual             msgQual, 
    int                     state);

/**
 * Determine whether the message qualifier indicates
 * that the "msg" is Definitely Duplicate.
 *
 * @param msgQual      The mamaMsgQual object.
 * @param result (out) Pointer to the result.
 *                     "1" indicates a positive result.
 *                     "0" indicates a negative result. 
 */
MAMAExpDLL
extern mama_status
mamaMsgQual_getIsDefinitelyDuplicate (
    mamaMsgQual             msgQual, 
    int*                    result);

/**
 * Determine whether the message qualifier indicates
 * that the "msg" is Possibly Duplicate.
 *
 * @param msgQual      The mamaMsgQual object.
 * @param result (out) Pointer to the result.
 *                     "1" indicates a positive result.
 *                     "0" indicates a negative result.
 */
MAMAExpDLL
extern mama_status
mamaMsgQual_getIsPossiblyDuplicate (
    mamaMsgQual             msgQual, 
    int*                    result);

/**
 * Determine whether the message qualifier indicates
 * that the "msg" is Definitely Delayed.
 *
 * @param msgQual      The mamaMsgQual object.
 * @param result (out) Pointer to the result.
 *                     "1" indicates a positive result.
 *                     "0" indicates a negative result.
 */
MAMAExpDLL
extern mama_status
mamaMsgQual_getIsDefinitelyDelayed (
    mamaMsgQual             msgQual, 
    int*                    result);

/**
 * Determine whether the message qualifier indicates
 * that the "msg" is Possibly Delayed.
 *
 * @param msgQual      The mamaMsgQual object.
 * @param result (out) Pointer to the result.
 *                     "1" indicates a positive result.
 *                     "0" indicates a negative result.
 */
MAMAExpDLL
extern mama_status
mamaMsgQual_getIsPossiblyDelayed (
    mamaMsgQual             msgQual, 
    int*                    result);

/**
 * Determine whether the message qualifier indicates
 * that the "msg" is Out Of Sequence Delayed.
 *
 * @param msgQual      The mamaMsgQual object.
 * @param result (out) Pointer to the result.  
 *                     "1" indicates a positive result.
 *                     "0" indicates a negative result. 
 */
MAMAExpDLL
extern mama_status
mamaMsgQual_getIsOutOfSequence (
    mamaMsgQual             msgQual, 
    int*                    result);


/**
 * Get the message qualifier value.
 *
 * @param msgQual       The mamaMsgQual object.
 * @param value         The integer value of the message qualifier.
 */
MAMAExpDLL
extern mama_status
mamaMsgQual_getValue(
    const mamaMsgQual       msgQual,
    mama_u16_t*             value);

/**
 * Get the message qualifier value as a delimited string.
 * 
 * @param msgQual    The mamaMsgQual object.
 * @param delimiter  The delimiter to separate individual condition strings.
 * @param str        The string buffer to update.
 * @param maxLen     The maximum size of the string buffer 
 *                   (including trailing '\\0').
 */
MAMAExpDLL
extern mama_status
mamaMsgQual_getAsString(
    const mamaMsgQual      msgQual,
    const char*            delimiter,
    char*                  str,
    mama_size_t            maxLen);

/**
 * Get the message qualifier value as a delimited string.
 * 
 * @param value      The value of the message qualifier.
 * @param delimiter  The delimiter to separate individual conditions strings.
 * @param str        The string buffer to update.
 * @param maxLen     The maximum size of the string buffer 
 *                   (including trailing '\\0').
 */
MAMAExpDLL
extern mama_status
mamaMsgQual_valueToString(
    mama_u16_t             value,
    const char*            delimiter,
    char*                  str,
    mama_size_t            maxLen);


#if defined(__cplusplus)
}
#endif

#endif /* MSG_QUALIFIER_H__ */
