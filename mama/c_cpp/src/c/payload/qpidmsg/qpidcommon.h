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

#ifndef MAMA_PAYLOAD_QPIDMSG_QPIDMSG_H__
#define MAMA_PAYLOAD_QPIDMSG_QPIDMSG_H__

/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mama/mama.h>
#include <proton/message.h>
#include <proton/error.h>

#include "payloadbridge.h"
#include "msgfieldimpl.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*=========================================================================
  =                              Macros                                   =
  =========================================================================*/

#ifndef PN_OK
#define PN_OK                           0
#endif
#define QPID_FIELDS_PER_MAMA_FIELD      4
#define QPID_BYTE_BUFFER_SIZE           102400
#define QPID_FIELD_BUFFER_POOL_SIZE     64

/*
 * The type of the atom is not necessarily known until runtime when the data
 * is extracted from the pn_data_t type. This macro casts the output from this
 * value to MAMA for castable native types. Notable absentees are included at
 * the end of the switch statement where direct casting is not practical.
 */
#define GET_ATOM_AS_MAMA_TYPE(ATOM,TO_TYPE,TO_RESULT)                              \
do                                                                             \
{                                                                              \
    switch (ATOM.type){                                                        \
    case PN_BOOL:                                                              \
        TO_RESULT = (TO_TYPE)ATOM.u.as_bool;                                   \
        break;                                                                 \
    case PN_UBYTE:                                                             \
        TO_RESULT = (TO_TYPE)ATOM.u.as_ubyte;                                  \
        break;                                                                 \
    case PN_BYTE:                                                              \
        TO_RESULT = (TO_TYPE)ATOM.u.as_byte;                                   \
        break;                                                                 \
    case PN_USHORT:                                                            \
        TO_RESULT = (TO_TYPE)ATOM.u.as_ushort;                                 \
        break;                                                                 \
    case PN_SHORT:                                                             \
        TO_RESULT = (TO_TYPE)ATOM.u.as_short;                                  \
        break;                                                                 \
    case PN_UINT:                                                              \
        TO_RESULT = (TO_TYPE)ATOM.u.as_uint;                                   \
        break;                                                                 \
    case PN_INT:                                                               \
        TO_RESULT = (TO_TYPE)ATOM.u.as_int;                                    \
        break;                                                                 \
    case PN_CHAR:                                                              \
        TO_RESULT = (TO_TYPE)ATOM.u.as_char;                                   \
        break;                                                                 \
    case PN_ULONG:                                                             \
        TO_RESULT = (TO_TYPE)ATOM.u.as_ulong;                                  \
        break;                                                                 \
    case PN_LONG:                                                              \
        TO_RESULT = (TO_TYPE)ATOM.u.as_long;                                   \
        break;                                                                 \
    case PN_TIMESTAMP:                                                         \
        TO_RESULT = (TO_TYPE)ATOM.u.as_timestamp;                              \
        break;                                                                 \
    case PN_FLOAT:                                                             \
        TO_RESULT = (TO_TYPE)ATOM.u.as_float;                                  \
        break;                                                                 \
    case PN_DOUBLE:                                                            \
        TO_RESULT = (TO_TYPE)ATOM.u.as_double;                                 \
        break;                                                                 \
    case PN_DECIMAL32:                                                         \
        TO_RESULT = (TO_TYPE)ATOM.u.as_decimal32;                              \
        break;                                                                 \
    case PN_DECIMAL64:                                                         \
        TO_RESULT = (TO_TYPE)ATOM.u.as_decimal64;                              \
        break;                                                                 \
    case PN_DECIMAL128:                                                        \
    case PN_UUID:                                                              \
    case PN_BINARY:                                                            \
    case PN_STRING:                                                            \
    case PN_SYMBOL:                                                            \
    case PN_DESCRIBED:                                                         \
    case PN_ARRAY:                                                             \
    case PN_LIST:                                                              \
    case PN_MAP:                                                               \
    case PN_NULL:                                                              \
    default:                                                                   \
        status = MAMA_STATUS_WRONG_FIELD_TYPE;                                 \
        break;                                                                 \
    }                                                                          \
} while(0)

/*=========================================================================
  =                Typedefs, structs, enums and globals                   =
  =========================================================================*/

typedef struct qpidmsgFieldPayloadImpl_
{
    /* Data field used for scalar types */
    pn_atom_t       mData;

    /* Data buffer used for storage of variable length field data */
    void*           mBuffer;
    mama_size_t     mBufferSize;

    /* Data buffer used for vectors of scalar types */
    pn_atom_t*      mDataArray;
    /* Number of elements in vector */
    mama_size_t     mDataArrayCount;
    /* Biggest number of elements in vector */
    mama_size_t     mDataMaxArrayCount;

    /* Data buffer used for vectors of messages */
    void**          mDataVector;
    /* Number of elements in vector */
    mama_size_t     mDataVectorCount;
    /* Biggest number of elements in vector */
    mama_size_t     mDataMaxVectorCount;

    /* Data buffer used for vectors of date times */
    mamaDateTime*   mDataVectorDateTime;
    /* Number of elements in vector */
    mama_size_t     mDataVectorDateTimeCount;
    /* Biggest number of elements in vector */
    mama_size_t     mDataMaxVectorDateTimeCount;

    /* Data buffer used for vectors of prices */
    mamaPrice*      mDataVectorPrice;
    /* Number of elements in vector */
    mama_size_t     mDataVectorPriceCount;
    /* Biggest number of elements in vector */
    mama_size_t     mDataMaxVectorPriceCount;

    /* Name of the field */
    pn_bytes_t      mName;

    /* Field identifier */
    uint16_t        mFid;

    /* MAMA Field type of current field */
    mamaFieldType   mMamaType;

    /* Data element from the parent message - NULL indicates no valid parent */
    pn_data_t       *mParentBody;

} qpidmsgFieldPayloadImpl;

typedef enum
{
    QPID_INSERT_MODE_MAIN_LIST  =   0,
    QPID_INSERT_MODE_INLINE     =   1,
} qpidInsertMode;

typedef struct qpidmsgPayloadImpl_
{
    /* Wrapper for QPID messages */
    pn_message_t*               mQpidMsg;

    /* Underlying QPID message structure */
    pn_data_t*                  mBody;

    /* Reusable field impl*/
    qpidmsgFieldPayloadImpl*    mField;

    /* Reusable general buffer */
    void*                       mBuffer;

    /* Max size of reusable buffer */
    mama_size_t                 mBufferSize;

    /* Reusable message buffer */
    mamaMsg*                    mNestedMsgBuffer;

    /* Max elements of message buffer */
    mama_size_t                 mNestedMsgBufferCount;

    /* Current size of message buffer */
    mama_size_t                 mNestedMsgBufferSize;

    /* Defines how to deal with inserts */
    qpidInsertMode              mInsertMode;

    /* Offset of data body during iteration */
    int16_t                     mDataIteratorOffset;

    /* Parent MAMA message */
    mamaMsg                     mParent;

    /* Child payload for use with getMsg */
    struct qpidmsgPayloadImpl_*   mChildMsg;

} qpidmsgPayloadImpl;


/*=========================================================================
  =                   Public implementation prototypes                    =
  =========================================================================*/

/**
 * This function will advise the caller what the MAMA field type equivalent of
 * the provided proton data type is. Note that this includes types which
 * are unsupported for vectors. Where the type does not have a currently
 * supported MAMA equivalent, MAMA_FIELD_TYPE_UNKNOWN will be returned.
 *
 * @param type      The proton type to evaluate
 *
 * @return mamaFieldType containing the MAMA equivalent field type.
 */
mamaFieldType
qpidmsgPayloadInternal_toMamaType (pn_type_t type);

/**
 * This function will translate proton error codes to mama_status equivalents.
 *
 * @param status    The proton error status to evaluate.
 *
 * @return mamaFieldType containing the MAMA equivalent field type.
 */
mama_status
qpidmsgPayloadInternal_toMamaStatus (int status);

/*
 * Proton doesn't really adhere to the MAMA way of defining fields with names
 * and fids so its toString() format will not match that of (for example)
 * wombatmsg.
 */

/**
 * This function will examine a provided proton data buffer and walk over its
 * contents, printing a string version of its contents to screen. Note that
 * proton doesn't really adhere to the MAMA way of defining fields with names
 * and fids so its toString() format will not match that of (for example)
 * wombatmsg.
 *
 * @param payload   The start position in the buffer iteration.
 * @param atom      The current atom to translate to a string.
 * @param dest      The location to write the string contents to.
 * @param len       The length of the destination buffer.
 *
 * @return mamaFieldType containing the MAMA equivalent field type.
 */
mama_size_t
qpidmsgPayloadInternal_elementToString (pn_data_t*  payload,
                                        pn_atom_t   atom,
                                        char*       dest,
                                        mama_size_t len);

#if defined(__cplusplus)
}
#endif

#endif /* MAMA_PAYLOAD_QPIDMSG_QPIDMSG_H__ */
