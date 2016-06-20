/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Inc.
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

#include "qpidcommon.h"
#include "qpidpayloadfunctions.h"

#include "field.h"
#include "payload.h"


/*=========================================================================
  =                              Macros                                   =
  =========================================================================*/

#define GET_SCALAR_FIELD(TYPE,MAMATYPE)                                        \
do                                                                             \
{                                                                              \
    qpidmsgFieldPayloadImpl* impl   = (qpidmsgFieldPayloadImpl*) field;        \
    mama_status              status = MAMA_STATUS_OK;                          \
                                                                               \
    if (NULL == impl || NULL == result) return MAMA_STATUS_NULL_ARG;           \
                                                                               \
    if (impl->mFid == 0 && impl->mName.start == NULL)                          \
    {                                                                          \
    	return MAMA_STATUS_INVALID_ARG;                                        \
    }                                                                          \
                                                                               \
    GET_ATOM_AS_MAMA_TYPE (impl->mData, MAMATYPE, *result);                    \
                                                                               \
    return status;                                                             \
} while(0)


#define UPDATE_SCALAR(TYPE,UCTYPE)                                             \
do                                                                             \
{                                                                              \
    qpidmsgFieldPayloadImpl*    impl   = (qpidmsgFieldPayloadImpl*)field;      \
    pn_atom_t                   atom;                                          \
                                                                               \
    if (NULL == impl || NULL == impl->mParentBody) return MAMA_STATUS_NULL_ARG;\
                                                                               \
    /* Enter into the field definition */                                      \
    pn_data_enter(impl->mParentBody);                                          \
                                                                               \
    /* Skip over name and fid */                                               \
    pn_data_next (impl->mParentBody);                                          \
    pn_data_next (impl->mParentBody);                                          \
                                                                               \
    /* Move on to actual value */                                              \
    pn_data_next (impl->mParentBody);                                          \
                                                                               \
    /* Verify underlying type is correct */                                    \
    atom = pn_data_get_atom (impl->mParentBody);                               \
    if (PN##UCTYPE != atom.type)                                               \
    {                                                                          \
        /* Exit list */                                                        \
        pn_data_exit (impl->mParentBody);                                      \
        return MAMA_STATUS_WRONG_FIELD_TYPE;                                   \
    }                                                                          \
                                                                               \
    /* Return to before the value for insertion */                             \
    pn_data_prev (impl->mParentBody);                                          \
                                                                               \
    /* Insert in the new field value */                                        \
    pn_data_put##TYPE(impl->mParentBody, value);                               \
                                                                               \
    /* Exit list */                                                            \
    pn_data_exit (impl->mParentBody);                                          \
                                                                               \
    return MAMA_STATUS_OK;                                                     \
} while(0)


#define GET_VECTOR_FIELD(TYPE,MAMATYPE)                                        \
do                                                                             \
{                                                                              \
    MAMATYPE*                temp = NULL;                                      \
    qpidmsgFieldPayloadImpl* impl = (qpidmsgFieldPayloadImpl*)field;           \
    mama_size_t              i    = 0;                                         \
                                                                               \
    if (NULL == impl || NULL == result || 0 == impl->mDataArrayCount)          \
    {                                                                          \
        return MAMA_STATUS_NULL_ARG;                                           \
    }                                                                          \
                                                                               \
    qpidmsgPayloadImpl_allocateBufferMemory (&(impl->mBuffer),                 \
                                             &(impl->mBufferSize),             \
            impl->mDataArrayCount * sizeof(MAMATYPE));                         \
    temp = (MAMATYPE*) impl->mBuffer;                                                      \
                                                                               \
    for (i = 0; i < impl->mDataArrayCount; i++)                                \
    {                                                                          \
        temp[i] = (MAMATYPE) impl->mDataArray[i].u.as##TYPE;                   \
    }                                                                          \
                                                                               \
    *size   = impl->mDataArrayCount;                                           \
    *result = temp;                                                            \
    return MAMA_STATUS_OK;                                                     \
                                                                               \
} while(0)


#define EXPAND_PRINT_VECTOR_MACROS(TYPE,SUBSET,FORMAT,CASTTO)                  \
do                                                                             \
{                                                                              \
    const TYPE * result = NULL;                                                \
    mama_size_t  size   = 0;                                                   \
    mama_size_t  i      = 0;                                                   \
    mama_status  status = mamaMsgField_getVector##SUBSET (                     \
                                  (const mamaMsgField) field,                  \
                                  &result,                                     \
                                  &size);                                      \
                                                                               \
    if (MAMA_STATUS_OK != status)                                              \
    {                                                                          \
        snprintf (buffer, len, "{failed with status: %d}", status);            \
        return status;                                                         \
    }                                                                          \
    snprintf (buffer, len, "{");                                               \
    for (i = 0; i < size; i++)                                                 \
    {                                                                          \
       snprintf (buffer, len, FORMAT, (CASTTO) result[i]);                     \
       snprintf (buffer, len, ",");                                            \
       if (i == (size -1))                                                     \
           snprintf (buffer, len, "\b");                                       \
    }                                                                          \
    snprintf (buffer, len, "}");                                               \
} while (0)


#define EXPAND_PRINT_SCALAR_MACROS(TYPE,INITIAL,SUBSET,FORMAT,CASTTO)          \
do                                                                             \
{                                                                              \
    TYPE result = INITIAL;                                                     \
    qpidmsgFieldPayload_get##SUBSET (field, &result);                          \
    snprintf (buffer, len, FORMAT, (CASTTO)result);                            \
} while (0)


/*=========================================================================
  =                   Public interface functions                          =
  =========================================================================*/

mama_status
qpidmsgFieldPayload_create (msgFieldPayload* field)
{
    qpidmsgFieldPayloadImpl* impl = NULL;

    if (NULL == field)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    impl = (qpidmsgFieldPayloadImpl*) calloc (1,
                                              sizeof (qpidmsgFieldPayloadImpl));

    impl->mDataVector         = NULL;
    impl->mDataVectorCount    = 0;
    impl->mDataMaxVectorCount = 0;
    impl->mDataArray          = NULL;
    impl->mDataArrayCount     = 0;
    impl->mDataMaxArrayCount  = 0;
    impl->mParentBody         = NULL;

    if (NULL == impl)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "qpidmsgFieldPayload_create() failed. Out of memory");
        return MAMA_STATUS_NOMEM;
    }

    *field = (msgFieldPayload)impl;
    return MAMA_STATUS_OK;
}

mama_status
qpidmsgFieldPayload_destroy (msgFieldPayload field)
{
    mama_size_t              i        = 0;
    qpidmsgFieldPayloadImpl* impl     = (qpidmsgFieldPayloadImpl*) field;
    qpidmsgPayloadImpl*      payload  = NULL;
    mamaDateTime             dateTime = NULL;
    mamaPrice                price    = NULL;

    if (NULL == field)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Free up the reusable buffer */
    if (NULL != impl->mBuffer)
    {
        free (impl->mBuffer);
    }

    if (NULL != impl->mDataArray)
    {
        free (impl->mDataArray);
    }

    for (i = 0; i < impl->mDataVectorCount; i++)
    {
        payload = (qpidmsgPayloadImpl*) impl->mDataVector[i];
        qpidmsgPayload_destroy(payload);
    }

    for (i = 0; i < impl->mDataMaxVectorDateTimeCount / sizeof(mamaDateTime); i++)
    {
        mamaDateTime_destroy (impl->mDataVectorDateTime[i]);
    }

    for (i = 0; i < impl->mDataMaxVectorPriceCount / sizeof(mamaPrice); i++)
    {
        mamaPrice_destroy (impl->mDataVectorPrice[i]);
    }

    /* Free the ** map */
    if (NULL != impl->mDataVector)
    {
        free (impl->mDataVector);
    }
    /* Free the ** map */
    if (NULL != impl->mDataVectorDateTime)
    {
        free (impl->mDataVectorDateTime);
    }
    /* Free the ** map */
    if (NULL != impl->mDataVectorPrice)
    {
        free (impl->mDataVectorPrice);
    }


    /* Free the temporary buffers allocated */
    free (impl);
    impl = NULL;
    return MAMA_STATUS_OK;
}

mama_status
qpidmsgFieldPayload_getType (const msgFieldPayload   field,
                             mamaFieldType*          result)
{
    qpidmsgFieldPayloadImpl* impl = (qpidmsgFieldPayloadImpl*) field;

    if (NULL == impl || NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    *result = impl->mMamaType;

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgFieldPayload_getName (msgFieldPayload         field,
                             mamaDictionary          dict,
                             mamaFieldDescriptor     desc,
                             const char**            result)
{ 
    qpidmsgFieldPayloadImpl* impl          = (qpidmsgFieldPayloadImpl*) field;
    const char*              fieldDescName = NULL;

    if (NULL == impl || NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* If a name is part of field and it's not a NULL string */
    if (NULL != impl->mName.start && strlen (impl->mName.start) > 0)
    {
        *result = impl->mName.start;
    }

    /* If there is a dictionary, but no descriptor */
    if (NULL != dict && NULL == desc)
    {
        qpidmsgFieldPayload_getDescriptor (field, dict, &desc);
    }

    /* If a descriptor was provided, use it to obtain the name */
    if (NULL != desc)
    {
        /* get the name from descriptor. If that fails, move on */
        fieldDescName = mamaFieldDescriptor_getName (desc);
        if (NULL != fieldDescName)
        {
            *result = fieldDescName;
        }
    }

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgFieldPayload_getFid (const msgFieldPayload   field,
                            mamaDictionary          dict,
                            mamaFieldDescriptor     desc,
                            uint16_t*               result)
{
    qpidmsgFieldPayloadImpl* impl = (qpidmsgFieldPayloadImpl*) field;

    if (NULL == impl || NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    *result = impl->mFid;

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgFieldPayload_getDescriptor(const msgFieldPayload  field,
                                  mamaDictionary        dict,
                                  mamaFieldDescriptor*  result)
{
    qpidmsgFieldPayloadImpl*    impl        = (qpidmsgFieldPayloadImpl*) field;
    mamaFieldDescriptor         tmpResult   = NULL;
    mama_status                 status      = MAMA_STATUS_OK;

    if (NULL == impl || NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (0 != impl->mFid)
    {
        status = mamaDictionary_getFieldDescriptorByFid (dict,
                                                         &tmpResult,
                                                         impl->mFid);
        if (MAMA_STATUS_OK == status)
        {
            *result = tmpResult;
        }
        return status;
    }

    if (NULL != impl->mName.start)
    {
        status = mamaDictionary_getFieldDescriptorByName (dict,
                                                          &tmpResult,
                                                          impl->mName.start);
        if (MAMA_STATUS_OK == status)
        {
            *result = tmpResult;
        }
        return status;
    }
    else
    {
        return MAMA_STATUS_INVALID_ARG;
    }

}

mama_status
qpidmsgFieldPayload_updateBool (msgFieldPayload         field,
                                msgPayload              msg,
                                mama_bool_t             value)
{
    UPDATE_SCALAR (_bool, _BOOL);
}

mama_status
qpidmsgFieldPayload_updateChar (msgFieldPayload         field,
                                msgPayload              msg,
                                char                    value)
{
    UPDATE_SCALAR (_char, _CHAR);
}

mama_status
qpidmsgFieldPayload_updateI8    (msgFieldPayload         field,
                                 msgPayload              msg,
                                 mama_i8_t               value)
{
    UPDATE_SCALAR (_byte, _BYTE);
}

mama_status
qpidmsgFieldPayload_updateU8    (msgFieldPayload         field,
                                 msgPayload              msg,
                                 mama_u8_t               value)
{
    UPDATE_SCALAR (_ubyte, _UBYTE);
}

mama_status
qpidmsgFieldPayload_updateI16   (msgFieldPayload         field,
                                 msgPayload              msg,
                                 mama_i16_t              value)
{
    UPDATE_SCALAR (_short, _SHORT);
}

mama_status
qpidmsgFieldPayload_updateU16   (msgFieldPayload         field,
                                 msgPayload              msg,
                                 mama_u16_t              value)
{
    UPDATE_SCALAR (_ushort, _USHORT);
}

mama_status
qpidmsgFieldPayload_updateI32   (msgFieldPayload         field,
                                 msgPayload              msg,
                                 mama_i32_t              value)
{
    UPDATE_SCALAR (_int, _INT);
}

mama_status
qpidmsgFieldPayload_updateU32   (msgFieldPayload         field,
                                 msgPayload              msg,
                                 mama_u32_t              value)
{
    UPDATE_SCALAR (_uint, _UINT);
}

mama_status
qpidmsgFieldPayload_updateI64   (msgFieldPayload         field,
                                 msgPayload              msg,
                                 mama_i64_t              value)
{
    UPDATE_SCALAR (_long, _LONG);
}

mama_status
qpidmsgFieldPayload_updateU64   (msgFieldPayload         field,
                                 msgPayload              msg,
                                 mama_u64_t              value)
{
    UPDATE_SCALAR (_ulong, _ULONG);
}

mama_status
qpidmsgFieldPayload_updateF32   (msgFieldPayload         field,
                                 msgPayload              msg,
                                 mama_f32_t              value)
{
    UPDATE_SCALAR (_float, _FLOAT);
}

mama_status
qpidmsgFieldPayload_updateF64   (msgFieldPayload         field,
                                 msgPayload              msg,
                                 mama_f64_t              value)
{
    UPDATE_SCALAR (_double, _DOUBLE);
}

mama_status
qpidmsgFieldPayload_updateString(msgFieldPayload         field,
                                 msgPayload              msg,
                                 const char*             str)
{
    pn_bytes_t value;

    /* Pass Proton strings as a pn_bytes_t struct */
    value.size  = strlen (str);
    value.start = (char*) str;

    UPDATE_SCALAR(_string, _STRING);
}

mama_status
qpidmsgFieldPayload_updateDateTime
                               (msgFieldPayload         field,
                                msgPayload              msg,
                                const mamaDateTime      value)
{
    qpidmsgFieldPayloadImpl*    impl         = (qpidmsgFieldPayloadImpl*) field;
    mama_status                 status       = MAMA_STATUS_OK;
    mama_u32_t                  datetime_s   = 0;
    mama_u32_t                  datetime_us  = 0;
    mamaDateTimeHints           dt_hints     = 0;
    mamaDateTimePrecision       dt_precision = MAMA_DATE_TIME_PREC_UNKNOWN;
    pn_timestamp_t              dt_stamp     = 0;
    pn_atom_t                   atom;

    if (NULL == impl || NULL == value || NULL == impl->mParentBody)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Enter into the field definition */
    pn_data_enter(impl->mParentBody);

    /* Skip over name and fid */
    pn_data_next (impl->mParentBody);
    pn_data_next (impl->mParentBody);

    /* Move on to actual value */
    pn_data_next (impl->mParentBody);

    /* Verify underlying type is correct */
    atom = pn_data_get_atom (impl->mParentBody);
    if (PN_TIMESTAMP != atom.type || MAMA_FIELD_TYPE_TIME != impl->mMamaType)
    {
        /* Exit list */
        pn_data_exit (impl->mParentBody);
        return MAMA_STATUS_WRONG_FIELD_TYPE;
    }

    /* Return to before the value for insertion */
    pn_data_prev (impl->mParentBody);

    status = mamaDateTime_getWithHints(value,
                                       &datetime_s,
                                       &datetime_us,
                                       &dt_precision,
                                       &dt_hints);
    if (MAMA_STATUS_OK != status)
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    /*
     * The timestamp is simply 64 bits of data. Place seconds in leftmost and
     * microseconds in rightmost 32 bits of format. Expected to be faster than
     * multiplication.
     */
    dt_stamp = datetime_us | ((mama_u64_t) datetime_s << 32);

    /* add the price value */
    pn_data_put_timestamp(impl->mParentBody, dt_stamp);

    /* add the hints value */
    pn_data_put_ubyte (impl->mParentBody, (mama_u8_t) dt_hints);

    /* add the precision value */
    pn_data_put_ubyte (impl->mParentBody, (mama_u8_t) dt_precision);

    /* Exit list */
    pn_data_exit (impl->mParentBody);

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgFieldPayload_updatePrice (msgFieldPayload         field,
                                 msgPayload              msg,
                                 const mamaPrice         value)
{
    qpidmsgFieldPayloadImpl*    impl        = (qpidmsgFieldPayloadImpl*)field;
    mama_status                 status      = MAMA_STATUS_OK;
    double                      price_value = 0;
    mamaPriceHints              price_hints = 0;
    pn_atom_t                   atom;

    if (NULL == impl || NULL == value)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (NULL == impl->mParentBody)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Enter into the field definition */
    pn_data_enter (impl->mParentBody);

    /* Skip over name and fid */
    pn_data_next  (impl->mParentBody);
    pn_data_next  (impl->mParentBody);

    /* Move on to actual value */
    pn_data_next  (impl->mParentBody);

    /* Verify underlying type is correct */
    atom = pn_data_get_atom (impl->mParentBody);

    if (PN_DOUBLE != atom.type || MAMA_FIELD_TYPE_PRICE != impl->mMamaType)
    {
        /* Exit list */
        pn_data_exit (impl->mParentBody);
        return MAMA_STATUS_WRONG_FIELD_TYPE;
    }

    /* Return to before the value for insertion */
    pn_data_prev (impl->mParentBody);

    status = mamaPrice_getValue (value, &price_value);
    if (MAMA_STATUS_OK != status)
    {
        return status;
    }

    status = mamaPrice_getHints (value, &price_hints);
    if (MAMA_STATUS_OK != status)
    {
        return status;
    }

    /* add the price value */
    pn_data_put_double (impl->mParentBody, price_value);

    /* add the hints value */
    pn_data_put_ubyte  (impl->mParentBody, (mama_u8_t) price_hints);

    /* Exit list */
    pn_data_exit (impl->mParentBody);

    return status;
}

mama_status
qpidmsgFieldPayload_updateSubMsg(msgFieldPayload         field,
                                 msgPayload              msg,
                                 const msgPayload        subMsg)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
qpidmsgFieldPayload_getBool (const msgFieldPayload   field,
                             mama_bool_t*            result)
{
    GET_SCALAR_FIELD (_bool,mama_bool_t);
}

mama_status
qpidmsgFieldPayload_getChar (const msgFieldPayload   field,
                             char*                   result)
{
    GET_SCALAR_FIELD (_char, char);
}

mama_status
qpidmsgFieldPayload_getI8   (const msgFieldPayload   field,
                             mama_i8_t*              result)
{
    GET_SCALAR_FIELD (_byte, mama_i8_t);
}

mama_status
qpidmsgFieldPayload_getU8   (const msgFieldPayload   field,
                             mama_u8_t*              result)
{
    GET_SCALAR_FIELD (_ubyte, mama_i8_t);
}

mama_status
qpidmsgFieldPayload_getI16  (const msgFieldPayload   field,
                             mama_i16_t*             result)
{
    GET_SCALAR_FIELD (_short, mama_i16_t);
}

mama_status
qpidmsgFieldPayload_getU16  (const msgFieldPayload   field,
                             mama_u16_t*            result)
{
    GET_SCALAR_FIELD (_ushort, mama_u16_t);
}

mama_status
qpidmsgFieldPayload_getI32  (const msgFieldPayload   field,
                             mama_i32_t*             result)
{
    GET_SCALAR_FIELD (_int, mama_i32_t);
}

mama_status
qpidmsgFieldPayload_getU32  (const msgFieldPayload   field,
                             mama_u32_t*             result)
{
    GET_SCALAR_FIELD (_uint, mama_u32_t);
}

mama_status
qpidmsgFieldPayload_getI64  (const msgFieldPayload   field,
                             mama_i64_t*             result)
{
    GET_SCALAR_FIELD (_long, mama_i64_t);
}

mama_status
qpidmsgFieldPayload_getU64  (const msgFieldPayload   field,
                             mama_u64_t*             result)
{
    GET_SCALAR_FIELD (_ulong, mama_u64_t);
}

mama_status
qpidmsgFieldPayload_getF32  (const msgFieldPayload   field,
                             mama_f32_t*             result)
{
    qpidmsgFieldPayloadImpl* impl = (qpidmsgFieldPayloadImpl*) field;
    mama_status status = MAMA_STATUS_OK;

    if (NULL == impl || NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (impl->mFid == 0 && impl->mName.start == NULL)
    {
    	return MAMA_STATUS_INVALID_ARG;
    }

    GET_ATOM_AS_MAMA_TYPE (impl->mData, mama_f32_t, *result);

    return status;
}

mama_status
qpidmsgFieldPayload_getF64  (const msgFieldPayload   field,
                             mama_f64_t*             result)
{
    GET_SCALAR_FIELD (_double, mama_f64_t);
}

mama_status
qpidmsgFieldPayload_getString (const msgFieldPayload   field,
                               const char**            result)
{
    qpidmsgFieldPayloadImpl* impl = (qpidmsgFieldPayloadImpl*) field;

    if (NULL == impl || NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (impl->mFid == 0 && impl->mName.start == NULL)
    {
    	return MAMA_STATUS_INVALID_ARG;
    }

    if (PN_STRING != impl->mData.type)
    {
    	return MAMA_STATUS_WRONG_FIELD_TYPE;
    }

    *result = impl->mData.u.as_bytes.start;

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgFieldPayload_getOpaque (const msgFieldPayload   field,
                               const void**            result,
                               mama_size_t*            size)
{
    qpidmsgFieldPayloadImpl* impl = (qpidmsgFieldPayloadImpl*) field;

    if (NULL == impl || NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (NULL == impl->mParentBody)
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    if (MAMA_FIELD_TYPE_OPAQUE != impl->mMamaType)
    {
    	return MAMA_STATUS_WRONG_FIELD_TYPE;
    }

    *result = impl->mData.u.as_bytes.start;
    *size   = (mama_size_t) impl->mData.u.as_bytes.size;

    return MAMA_STATUS_OK;
}

/*
 * getDateTime is expected to be able to handle casting from strings,
 * F64, I64, U64 and Date Time input data types.
 */
mama_status
qpidmsgFieldPayload_getDateTime (const msgFieldPayload   field,
                                 mamaDateTime            result)
{
    qpidmsgFieldPayloadImpl* impl = (qpidmsgFieldPayloadImpl*) field;

    if (NULL == impl || NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    switch(impl->mMamaType)
    {
    case MAMA_FIELD_TYPE_TIME:
    {
        /* Pull out the 64 bit time stamp */
        pn_timestamp_t stamp       = impl->mDataArray[0].u.as_timestamp;

        /* Extract the hints */
        mama_u8_t      hints       = impl->mDataArray[1].u.as_ubyte;

        /* Extract the precision */
        mama_u8_t      precision   = impl->mDataArray[2].u.as_ubyte;

        /* Perform casts / bitwise operators to extract timestamps */
        mama_u32_t     micros      = (mama_u32_t) stamp;
        mama_u32_t     seconds     = (mama_u32_t) (stamp >> 32);

        mamaDateTime_setWithHints (result,
                                   seconds,
                                   micros,
                                   (mamaDateTimePrecision) precision,
                                   (mamaDateTimeHints) hints);
        break;
    }
    case MAMA_FIELD_TYPE_STRING:
    {
        pn_atom_t atom = impl->mData;
        mamaDateTime_setFromString (result, atom.u.as_bytes.start);
        break;
    }
    case MAMA_FIELD_TYPE_F64:
    {
        pn_atom_t atom = impl->mData;
        mamaDateTime_setEpochTimeF64 (result, atom.u.as_double);
        break;
    }
    case MAMA_FIELD_TYPE_I64:
    {
        pn_atom_t atom = impl->mData;
        mamaDateTime_setEpochTimeMilliseconds (result, atom.u.as_long);
        break;
    }
    case MAMA_FIELD_TYPE_U64:
    {
        pn_atom_t atom = impl->mData;
        mamaDateTime_setEpochTimeMicroseconds (result, atom.u.as_ulong);
        break;
    }
    default:
        return MAMA_STATUS_WRONG_FIELD_TYPE;
        break;
    }

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgFieldPayload_getPrice (const msgFieldPayload   field,
                              mamaPrice               result)
{
    qpidmsgFieldPayloadImpl* impl = (qpidmsgFieldPayloadImpl*) field;

    if (NULL == impl || NULL == result)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    switch(impl->mMamaType)
    {
    case MAMA_FIELD_TYPE_PRICE:
    {
        double    value        = impl->mDataArray[0].u.as_double;
        mama_u8_t displayHints = impl->mDataArray[1].u.as_ubyte;

        mamaPrice_setValue (result, value);
        mamaPrice_setHints (result, displayHints);
        break;
    }
    case MAMA_FIELD_TYPE_F64:
    {
        double value = impl->mData.u.as_double;
        mamaPrice_setValue (result, value);
        break;
    }
    default:
        return MAMA_STATUS_WRONG_FIELD_TYPE;
        break;
    }

    return MAMA_STATUS_OK;
}

/*
 * NOTE: the MAMA method which calls this lesser-used function contains a memory
 * leak as a parent message is created and then never freed.
 */
mama_status
qpidmsgFieldPayload_getMsg (const msgFieldPayload   field,
                            msgPayload*             result)
{
    qpidmsgFieldPayloadImpl* impl = (qpidmsgFieldPayloadImpl*) field;
    if (NULL == impl)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (NULL == impl->mParentBody)
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    if (MAMA_FIELD_TYPE_MSG != impl->mMamaType)
    {
        return MAMA_STATUS_WRONG_FIELD_TYPE;
    }

    *result = impl->mDataVector[0];

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgFieldPayload_getVectorBool (const msgFieldPayload   field,
                                   const mama_bool_t**     result,
                                   mama_size_t*            size)
{
    GET_VECTOR_FIELD (_bool, mama_bool_t);
}

mama_status
qpidmsgFieldPayload_getVectorChar (const msgFieldPayload   field,
                                   const char**            result,
                                   mama_size_t*            size)
{
    GET_VECTOR_FIELD (_char, char);
}

mama_status
qpidmsgFieldPayload_getVectorI8   (const msgFieldPayload   field,
                                   const mama_i8_t**       result,
                                   mama_size_t*            size)
{
    GET_VECTOR_FIELD (_byte, mama_i8_t);
}

mama_status
qpidmsgFieldPayload_getVectorU8   (const msgFieldPayload   field,
                                   const mama_u8_t**       result,
                                   mama_size_t*            size)
{
    GET_VECTOR_FIELD (_ubyte, mama_u8_t);
}

mama_status
qpidmsgFieldPayload_getVectorI16  (const msgFieldPayload   field,
                                   const mama_i16_t**      result,
                                   mama_size_t*            size)
{
    GET_VECTOR_FIELD (_short, mama_i16_t);
}

mama_status
qpidmsgFieldPayload_getVectorU16  (const msgFieldPayload   field,
                                   const mama_u16_t**      result,
                                   mama_size_t*            size)
{
    GET_VECTOR_FIELD (_ushort, mama_u16_t);
}

mama_status
qpidmsgFieldPayload_getVectorI32  (const msgFieldPayload   field,
                                   const mama_i32_t**      result,
                                   mama_size_t*            size)
{

    GET_VECTOR_FIELD (_int,mama_i32_t);
}

mama_status
qpidmsgFieldPayload_getVectorU32  (const msgFieldPayload   field,
                                   const mama_u32_t**      result,
                                   mama_size_t*            size)
{
    GET_VECTOR_FIELD (_uint,mama_u32_t);
}

mama_status
qpidmsgFieldPayload_getVectorI64  (const msgFieldPayload   field,
                                   const mama_i64_t**      result,
                                   mama_size_t*            size)
{
    GET_VECTOR_FIELD (_long,mama_i64_t);
}

mama_status
qpidmsgFieldPayload_getVectorU64  (const msgFieldPayload   field,
                                   const mama_u64_t**      result,
                                   mama_size_t*            size)
{
    GET_VECTOR_FIELD (_ulong,mama_u64_t);
}

mama_status
qpidmsgFieldPayload_getVectorF32  (const msgFieldPayload   field,
                                   const mama_f32_t**      result,
                                   mama_size_t*            size)
{
    GET_VECTOR_FIELD (_float,mama_f32_t);
}

mama_status
qpidmsgFieldPayload_getVectorF64  (const msgFieldPayload   field,
                                   const mama_f64_t**      result,
                                   mama_size_t*            size)
{
    GET_VECTOR_FIELD (_double,mama_f64_t);
}

mama_status
qpidmsgFieldPayload_getVectorString (const msgFieldPayload   field,
                                     const char***           result,
                                     mama_size_t*            size)
{
    const char**             temp       = NULL;
    qpidmsgFieldPayloadImpl* impl       = (qpidmsgFieldPayloadImpl*) field;
    mama_size_t              i          = 0;

    if (   NULL == impl
        || NULL == result
        || impl->mDataArrayCount == 0)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    qpidmsgPayloadImpl_allocateBufferMemory (
            &impl->mBuffer,
            &impl->mBufferSize,
            impl->mDataArrayCount * sizeof (char*));

    temp = (const char**) impl->mBuffer;

    for (i = 0; i < impl->mDataArrayCount; i++)
    {
        temp[i] = impl->mDataArray[i].u.as_bytes.start;
    }

    *result = temp;
    *size   = impl->mDataArrayCount;

    return MAMA_STATUS_OK;
}

/*
 * Postponing implementation until this type of vectors has a standard protocol
 * or is removed from the implementation
 */
mama_status
qpidmsgFieldPayload_getVectorDateTime (const msgFieldPayload   field,
                                       const mamaDateTime**    result,
                                       mama_size_t*            size)
{
    qpidmsgFieldPayloadImpl* impl       = (qpidmsgFieldPayloadImpl*) field;
    mama_size_t              i          = 0;
    mama_size_t              index      = 0;
    pn_timestamp_t           stamp      = 0;
    mama_u8_t                hints      = 0;
    mama_u8_t                precision  = 0;
    mama_u32_t               micros     = 0;
    mama_u32_t               seconds    = 0;

    if (   NULL == impl
        || NULL == result
        || NULL == size)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    *size   = impl->mDataArrayCount / 3;

    /* allocate space for resulting array*/
    qpidmsgPayloadImpl_allocateBufferMemory ((void**)&impl->mDataVectorDateTime,
                                             &impl->mDataVectorDateTimeCount,
                                             *size * sizeof (mamaDateTime));

    if (impl->mDataVectorDateTimeCount > impl->mDataMaxVectorDateTimeCount)
    {
        impl->mDataMaxVectorDateTimeCount = impl->mDataVectorDateTimeCount;
    }

    for (i = 0; i < impl->mDataArrayCount; )
    {

        /* Pull out the 64 bit time stamp */
        stamp = impl->mDataArray[i++].u.as_timestamp;

        /* Extract the hints */
        hints = impl->mDataArray[i++].u.as_ubyte;

        /* Extract the precision */
        precision = impl->mDataArray[i++].u.as_ubyte;

        index = (i / 3) - 1;

        /* Perform casts / bitwise operators to extract timestamps */
        micros  = (mama_u32_t) stamp;
        seconds = (mama_u32_t) (stamp >> 32);

        if (NULL == impl->mDataVectorDateTime[index])
        {
            mamaDateTime_create(&impl->mDataVectorDateTime[index]);
        }
        else
        {
            mamaDateTime_clear(impl->mDataVectorDateTime[index]);
        }

        mamaDateTime_setWithHints (impl->mDataVectorDateTime[index],
                                   seconds,
                                   micros,
                                   (mamaDateTimePrecision) precision,
                                   hints);
    }

    *result = impl->mDataVectorDateTime;

    return MAMA_STATUS_OK;
}

/*
 * Postponing implementation until this type of vectors has a standard protocol
 * or is removed from the implementation
 */
mama_status
qpidmsgFieldPayload_getVectorPrice    (const msgFieldPayload   field,
                                       const mamaPrice**       result,
                                       mama_size_t*            size)
{
    qpidmsgFieldPayloadImpl* impl       = (qpidmsgFieldPayloadImpl*) field;
    mama_size_t              i          = 0;
    mama_size_t              index      = 0;
    double                   price      = 0;
    uint8_t                  hints      = 0;

    if (   NULL == impl
        || NULL == result
        || NULL == size)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    *size   = impl->mDataArrayCount / 2;

    /* allocate space for resulting array*/
    qpidmsgPayloadImpl_allocateBufferMemory ((void**)&impl->mDataVectorPrice,
                                             &impl->mDataVectorPriceCount,
                                             *size * sizeof (mamaPrice));

    if (impl->mDataVectorPriceCount > impl->mDataMaxVectorPriceCount)
    {
        impl->mDataMaxVectorPriceCount = impl->mDataVectorPriceCount;
    }

    for (i = 0; i < impl->mDataArrayCount; )
    {

        /* Pull out the price */
        price = impl->mDataArray[i++].u.as_double;

        /* Extract the hints */
        hints = impl->mDataArray[i++].u.as_ubyte;

        index = (i / 2) - 1;

        if (NULL == impl->mDataVectorPrice[index])
        {
            mamaPrice_create(&impl->mDataVectorPrice[index]);
        }
        else
        {
            mamaPrice_clear(impl->mDataVectorPrice[index]);
        }

        mamaPrice_setWithHints (impl->mDataVectorPrice[index],
                                price,
                                (mamaPriceHints) hints);
    }

    *result = impl->mDataVectorPrice;

    return MAMA_STATUS_OK;
}


mama_status
qpidmsgFieldPayload_getVectorMsg      (const msgFieldPayload   field,
                                       const msgPayload**      result,
                                       mama_size_t*            size)
{
    msgPayload*               temp  = NULL;
    qpidmsgFieldPayloadImpl*  impl  = (qpidmsgFieldPayloadImpl*)field;
    mama_size_t               i     = 0;

    if (   NULL == impl
        || NULL == result
        || 0    == impl->mDataVectorCount)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    *size = impl->mDataVectorCount;

    qpidmsgPayloadImpl_allocateBufferMemory (
            &impl->mBuffer,
            &impl->mBufferSize,
            (*size) * sizeof (qpidmsgFieldPayloadImpl**));

    temp = (msgPayload*) impl->mBuffer;

    for (i = 0; i < *size; i++)
    {
        temp[i] = impl->mDataVector[i];
    }

    *result = temp;
    *size   = impl->mDataVectorCount;

    return MAMA_STATUS_OK;
}

mama_status
qpidmsgFieldPayload_getAsString       (const msgFieldPayload   field,
                                       const msgPayload        msg,
                                       char*                   buffer,
                                       mama_size_t             len)
{
    mama_status status = MAMA_STATUS_OK;

    qpidmsgFieldPayloadImpl* impl = (qpidmsgFieldPayloadImpl*)field;

    if (NULL == buffer)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    switch (impl->mMamaType)
    {
    case MAMA_FIELD_TYPE_BOOL:
    {
        EXPAND_PRINT_SCALAR_MACROS (mama_bool_t,
                                    (mama_bool_t) 0,
                                    Bool,
                                    "%u",
                                    mama_bool_t);
        break;
    }
    case MAMA_FIELD_TYPE_CHAR:
    {
        EXPAND_PRINT_SCALAR_MACROS (char, '\0', Char, "%c", char);
        break;
    }
    case MAMA_FIELD_TYPE_I8:
    {
        EXPAND_PRINT_SCALAR_MACROS (mama_i8_t, 0, I8, "%d", mama_i8_t);
        break;
    }
    case MAMA_FIELD_TYPE_U8:
    {
        EXPAND_PRINT_SCALAR_MACROS (mama_u8_t, 0, U8, "%u", mama_u8_t);
        break;
    }
    case MAMA_FIELD_TYPE_I16:
    {
        EXPAND_PRINT_SCALAR_MACROS (mama_i16_t, 0, I16, "%d", mama_i16_t);
        break;
    }
    case MAMA_FIELD_TYPE_U16:
    {
        EXPAND_PRINT_SCALAR_MACROS (mama_u16_t, 0, U16, "%u", mama_u16_t);
        break;
    }
    case MAMA_FIELD_TYPE_I32:
    {
        EXPAND_PRINT_SCALAR_MACROS (mama_i32_t, 0, I32, "%d", mama_i32_t);
        break;
    }
    case MAMA_FIELD_TYPE_U32:
    {
        EXPAND_PRINT_SCALAR_MACROS (mama_u32_t, 0, U32, "%u", mama_u32_t);
        break;
    }
    case MAMA_FIELD_TYPE_I64:
    {
        EXPAND_PRINT_SCALAR_MACROS (mama_i64_t,
                                    0,
                                    I64,
                                    "%lld",
                                    long long);
        break;
    }
    case MAMA_FIELD_TYPE_U64:
    {
        EXPAND_PRINT_SCALAR_MACROS (mama_u64_t,
                                    0,
                                    U64,
                                    "%llu",
                                    long long unsigned);
        break;
    }
    case MAMA_FIELD_TYPE_F32:
    {
        EXPAND_PRINT_SCALAR_MACROS (mama_f32_t,
                                    (mama_f32_t)0.0,
                                    F32,
                                    "%f",
                                    mama_f32_t);
        break;
    }
    case MAMA_FIELD_TYPE_F64:
    {
        EXPAND_PRINT_SCALAR_MACROS (mama_f64_t,
                                    (mama_f64_t)0.0,
                                    F64,
                                    "%f",
                                    mama_f64_t);
        break;
    }
    case MAMA_FIELD_TYPE_STRING:
    {
        EXPAND_PRINT_SCALAR_MACROS (const char*, "", String, "%s", const char*);
        break;
    }
    case MAMA_FIELD_TYPE_TIME:
    {
        mamaDateTime result = NULL;
        char         dateTimeString[56];

        mamaDateTime_create (&result);
        qpidmsgFieldPayload_getDateTime (field, result);

        status = mamaDateTime_getAsString (result, dateTimeString, 56);
        snprintf (buffer, len, "%s", dateTimeString);

        mamaDateTime_destroy (result);
        break;
    }
    case MAMA_FIELD_TYPE_PRICE:
    {
        mamaPrice   result = NULL;
        char        priceString[56];

        mamaPrice_create             (&result);
        qpidmsgFieldPayload_getPrice (field, result);

        status = mamaPrice_getAsString (result, priceString, 56);
        snprintf (buffer, len, "%s", priceString);

        mamaPrice_destroy (result);
        break;
    }
    case MAMA_FIELD_TYPE_VECTOR_STRING:
    {
        EXPAND_PRINT_VECTOR_MACROS (char*, String, "%s", const char*);
        break;
    }
    case MAMA_FIELD_TYPE_VECTOR_U8:
    {
        EXPAND_PRINT_VECTOR_MACROS (mama_u8_t, U8, "%u", mama_u8_t);
        break;
    }
    case MAMA_FIELD_TYPE_VECTOR_U16:
    {
        EXPAND_PRINT_VECTOR_MACROS (mama_u16_t, U16, "%u", mama_u16_t);
        break;
    }
    case MAMA_FIELD_TYPE_VECTOR_U32:
    {
        EXPAND_PRINT_VECTOR_MACROS (mama_u32_t, U32, "%u", mama_u32_t);
        break;
    }
    case MAMA_FIELD_TYPE_VECTOR_U64:
    {
        EXPAND_PRINT_VECTOR_MACROS (mama_u64_t,
                                    U64,
                                    "%llu",
                                    long long unsigned);
        break;
    }
    case MAMA_FIELD_TYPE_VECTOR_BOOL:
    {
        EXPAND_PRINT_VECTOR_MACROS (mama_bool_t, Bool, "%u", mama_bool_t);
        break;
    }
    case MAMA_FIELD_TYPE_VECTOR_CHAR:
    {
        EXPAND_PRINT_VECTOR_MACROS (char, Char, "%c", char);
        break;
    }
    case MAMA_FIELD_TYPE_VECTOR_I8:
    {
        EXPAND_PRINT_VECTOR_MACROS (mama_i8_t, I8, "%d", mama_i8_t);
        break;
    }
    case MAMA_FIELD_TYPE_VECTOR_I16:
    {
        EXPAND_PRINT_VECTOR_MACROS (mama_i16_t, I16, "%d", mama_i16_t);
        break;
    }
    case MAMA_FIELD_TYPE_VECTOR_I32:
    {
        EXPAND_PRINT_VECTOR_MACROS (mama_i32_t, I32, "%d", mama_i32_t);
        break;
    }
    case MAMA_FIELD_TYPE_VECTOR_I64:
    {
        EXPAND_PRINT_VECTOR_MACROS (mama_i64_t, I64, "%lld", long long);
        break;
    }
    case MAMA_FIELD_TYPE_VECTOR_F64:
    {
        EXPAND_PRINT_VECTOR_MACROS (mama_f64_t, F64, "%f", mama_f64_t);
        break;
    }
    case MAMA_FIELD_TYPE_VECTOR_F32:
    {
        EXPAND_PRINT_VECTOR_MACROS (mama_f32_t, F32, "%f", mama_f32_t);
        break;
    }
    case MAMA_FIELD_TYPE_OPAQUE:
    {
        const void* result   = NULL;
        mama_size_t dataSize = 0;
        mama_size_t s        = 0;

        status = qpidmsgFieldPayload_getOpaque (field, &result, &dataSize);

        for (s = 0; s < dataSize; s++)
        {
            snprintf (buffer, len, "%#x ", ((char*) result)[s]);
        }
        break;
    }
    case MAMA_FIELD_TYPE_MSG:
    case MAMA_FIELD_TYPE_VECTOR_MSG:
    case MAMA_FIELD_TYPE_VECTOR_TIME:
    case MAMA_FIELD_TYPE_VECTOR_PRICE:
    {
        snprintf (buffer, len, "{...}");
        break;
    }
    default:
    {
        snprintf (buffer, len, "Unknown");
        break;
    }
    }
    return status;
}

/*=========================================================================
  =                   Public implementation functions                     =
  =========================================================================*/

mama_status
qpidmsgFieldPayloadImpl_setDataVectorSize (qpidmsgFieldPayloadImpl* impl,
                                           mama_size_t size)
{
    int                 i          = 0;
    mama_size_t         targetSize = sizeof (void**) * size;
    mama_size_t         oldSize    = sizeof (void**) * impl->mDataVectorCount;
    qpidmsgPayloadImpl* payload    = NULL;
    mama_status         status     = MAMA_STATUS_OK;

    status = qpidmsgPayloadImpl_allocateBufferMemory (
                     (void**) &impl->mDataVector,
                     &oldSize,
                     targetSize);

    if (MAMA_STATUS_OK != status)
    {
        return status;
    }

    /* Destroy any previous objects inside */
    for (i = 0; i < impl->mDataVectorCount; i++)
    {
        if (NULL != impl->mDataVector[i])
        {
            payload = (qpidmsgPayloadImpl*) impl->mDataVector[i];
            qpidmsgPayload_destroy (payload);
        }
    }

    /* Update maximum to keep track of memory if required */
    if (size > impl->mDataMaxVectorCount)
    {
        impl->mDataMaxVectorCount = size;
    }

    /* Update the vector count to reflect latest requested size */
    impl->mDataVectorCount = size;

    return status;
}

/* Use this item for setting array sizes for data arrays (scalar vectors) */
mama_status
qpidmsgFieldPayloadImpl_setDataArraySize (qpidmsgFieldPayloadImpl* impl,
                                          mama_size_t size)
{
    /* Update maximum to keep track of memory if required */
    if (size > impl->mDataMaxArrayCount)
    {
        /* Update maximum */
        impl->mDataMaxArrayCount = size;

        /* Reallocate data vector reference memory if required */
        impl->mDataArray = (pn_atom_t*) realloc (impl->mDataArray,
                                   size * sizeof (pn_atom_t));
    }

    /* Update the vector count */
    impl->mDataArrayCount = size;

    if(NULL == impl->mDataArray)
    {
        return MAMA_STATUS_NOMEM;
    }
    else
    {
        return MAMA_STATUS_OK;
    }
}
