/* $Id: avismsgimpl.c,v 1.1.2.8 2011/09/30 09:26:40 emmapollock Exp $
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include <avis/elvin.h>

#include "wombat/port.h"

#include <mama/mama.h>
#include "avismsgimpl.h"

short
avis2MamaType( ValueType valueType )
{
    switch( valueType )
    {
    case TYPE_INT32:    return MAMA_FIELD_TYPE_I32;
    case TYPE_INT64:    return MAMA_FIELD_TYPE_I64;
    case TYPE_REAL64:   return MAMA_FIELD_TYPE_F64;
    case TYPE_STRING:   return MAMA_FIELD_TYPE_STRING;
    case TYPE_OPAQUE:   return MAMA_FIELD_TYPE_OPAQUE;
    default:            return MAMA_FIELD_TYPE_UNKNOWN;
    }
}

mama_status
avisMsg_setBool(
        Attributes*     attributes,
        const char*     name,
        mama_fid_t      fid,
        mama_bool_t     value)
{
	char tempName[64];
	char* id = (char*)name;
	if (fid!=0)
	{
		snprintf (tempName, 63, "%d", fid);
		id=tempName;
	}
    attributes_set_int32(attributes, id, value);

	return MAMA_STATUS_OK;
}

mama_status
avisMsg_setChar(
        Attributes*     attributes,
        const char*     name,
        mama_fid_t      fid,
        char            value)
{
	char tempName[64];
	char tempstr[2];
	char* id = (char*) name;
	if (fid!=0)
	{
		snprintf (tempName, 63, "%d", fid);
		id=tempName;
	}
	snprintf (tempstr, 2, "%c", value);
    attributes_set_string(attributes, id, tempstr);
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_setI8(
        Attributes*     attributes,
        const char*     name,
        mama_fid_t      fid,
        int8_t          value)
{
	char tempName[64];
	char* id = (char*) name;
		if (fid!=0)
		{
		snprintf (tempName, 63, "%d", fid);
		id=tempName;
		}
    attributes_set_int32(attributes, id, value);
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_setU8(
        Attributes*     attributes,
        const char*     name,
        mama_fid_t      fid,
        uint8_t         value)
{
	char tempName[64];
	char* id = (char*) name;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		id=tempName;
	}
    attributes_set_int32(attributes, id, value);
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_setI16(
        Attributes*     attributes,
        const char*     name,
        mama_fid_t      fid,
        int16_t         value)
{
	char tempName[64];
	char* id = (char*) name;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		id=tempName;
	}
    attributes_set_int32(attributes, id, value);
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_setU16(
        Attributes*     attributes,
        const char*     name,
        mama_fid_t      fid,
        uint16_t        value)
{
	char tempName[64];
	char* id = (char*) name;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		id=tempName;
	}
    attributes_set_int32(attributes, id, value);
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_setI32(
    Attributes*         attributes,
    const char*         name,
    mama_fid_t          fid,
    int32_t             value)
{
	char tempName[64];
	char* id = (char*) name;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		id=tempName;
	}
    attributes_set_int32(attributes, id, value);
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_setU32(
        Attributes*     attributes,
        const char*     name,
        mama_fid_t      fid,
        uint32_t        value)
{
	char tempName[64];
	char* id = (char*) name;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		id=tempName;
	}
    attributes_set_int64(attributes, id, (uint64_t)value);
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_setI64(
    Attributes*         attributes,
    const char*         name,
    mama_fid_t          fid,
    int64_t             value)
{
	char tempName[64];
	char* id = (char*) name;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		id=tempName;
	}
	attributes_set_int64(attributes, id, value);
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_setU64(
        Attributes*     attributes,
        const char*     name,
        mama_fid_t      fid,
        uint64_t        value)
{
	char tempName[64];
	char* id = (char*) name;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		id=tempName;
	}
	attributes_set_int64(attributes, id, (int64_t)value);
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_setF32(
   Attributes*     attributes,
   const char*     name,
   mama_fid_t      fid,
   mama_f32_t      value)
{
	char tempName[64];
	char* id = (char*) name;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		id=tempName;
	}
    attributes_set_real64(attributes, id, (real64_t)value);
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_setF64(
    Attributes*  attributes,
    const char*  name,
    mama_fid_t   fid,
    mama_f64_t   value)
{
	char tempName[64];
	char* id = (char*) name;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		id=tempName;
	}
    attributes_set_real64(attributes, id, value);
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_setString(
    Attributes*  attributes,
    const char*  name,
    mama_fid_t   fid,
    const char*  value)
{
	char tempName[64];
	char* id = (char*) name;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		id=tempName;
	}
    attributes_set_string(attributes, id, value);
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_setOpaque(
        Attributes*  attributes,
        const char*  name,
        mama_fid_t   fid,
        const void*  value,
        size_t       size)
{
    Array* pArray = array_create(int8_t, size);
	char tempName[64];
	char* id = (char*) name;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		id=tempName;
	}
	pArray->items = avis_memdup(value, size);
    attributes_set_opaque(attributes, id, *pArray);
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_setVectorBool (
    Attributes*  attributes,
    const char*        name,
    mama_fid_t         fid,
    const mama_bool_t  value[],
    size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_setVectorChar (
    Attributes*  attributes,
    const char*        name,
    mama_fid_t         fid,
    const char         value[],
    size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_setVectorI8 (
        Attributes*  attributes,
        const char*        name,
        mama_fid_t         fid,
        const int8_t       value[],
        size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_setVectorU8 (
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const uint8_t      value[],
        size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_setVectorI16 (
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const int16_t      value[],
        size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_setVectorU16 (
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const uint16_t     value[],
        size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_setVectorI32 (
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const int32_t      value[],
        size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_setVectorU32 (
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const uint32_t     value[],
        size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_setVectorI64 (
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const int64_t      value[],
        size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_setVectorU64 (
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const uint64_t     value[],
        size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_setVectorF32 (
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const mama_f32_t   value[],
        size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_setVectorF64 (
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const mama_f64_t   value[],
        size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_setVectorString (
        Attributes*  attributes,
        const char*  name,
        mama_fid_t   fid,
        const char*  value[],
        size_t       numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_setVectorDateTime (
        Attributes*  attributes,
    const char*         name,
    mama_fid_t          fid,
    const mamaDateTime  value[],
    size_t              numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_setMsg(
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const msgPayload   value)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_setVectorMsg(
        Attributes*        attributes,
        const char*        name,
        mama_fid_t         fid,
        const mamaMsg      value[],
        size_t             numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_getBool(
        Attributes*   attributes,
        const char*   name,
        mama_fid_t    fid,
        mama_bool_t*  result)
{
	char tempName[64];
	Value* pValue = NULL;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		pValue = attributes_get(attributes, tempName);
	}

    if ((!pValue) && (name))
		pValue = attributes_get(attributes, name);
    if (!pValue)
      return MAMA_STATUS_NOT_FOUND;
    return avisValue_getBool(pValue, result);
}

mama_status avisValue_getBool(const Value* pValue, mama_bool_t* result)
{
    if (!pValue)
      return MAMA_STATUS_INVALID_ARG;
    if (pValue->type != TYPE_INT32)
      return MAMA_STATUS_WRONG_FIELD_TYPE;
    *result = (mama_bool_t) pValue->value.int32;
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_getChar(
        Attributes*   attributes,
        const char*   name,
        mama_fid_t    fid,
        char*         result)
{
	char tempName[64];
	Value* pValue = NULL;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		pValue = attributes_get(attributes, tempName);
	}
    if ((!pValue) && (name))
		pValue = attributes_get(attributes, name);
    if (!pValue)
      return MAMA_STATUS_NOT_FOUND;
    return avisValue_getChar(pValue, result);
}

mama_status avisValue_getChar(const Value* pValue, char* result)
{
    if (!pValue)
      return MAMA_STATUS_INVALID_ARG;

    if (pValue->type != TYPE_STRING)
      return MAMA_STATUS_WRONG_FIELD_TYPE;
    *result =  (char) pValue->value.str[0];
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_getI8(
        Attributes*   attributes,
        const char*   name,
        mama_fid_t    fid,
        int8_t*       result)
{
	char tempName[64];
	Value* pValue = NULL;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		pValue = attributes_get(attributes, tempName);
	}
    if ((!pValue) && (name))
		pValue = attributes_get(attributes, name);
    if (!pValue)
       return MAMA_STATUS_NOT_FOUND;
    return avisValue_getI8(pValue, result);
}

mama_status avisValue_getI8(const Value* pValue, int8_t* result)
{
    if (!pValue)
      return MAMA_STATUS_INVALID_ARG;

    if (pValue->type != TYPE_INT32)
      return MAMA_STATUS_WRONG_FIELD_TYPE;
    *result = (int8_t) pValue->value.int32;
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_getU8(
        Attributes*   attributes,
        const char*   name,
        mama_fid_t    fid,
        uint8_t*      result)
{
	char tempName[64];
	Value* pValue = NULL;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		pValue = attributes_get(attributes, tempName);
	}
    if ((!pValue) && (name))
		pValue = attributes_get(attributes, name);
    if (!pValue)
       return MAMA_STATUS_NOT_FOUND;
    return avisValue_getU8(pValue, result);
}

mama_status avisValue_getU8(const Value* pValue, uint8_t* result)
{
    if (!pValue)
      return MAMA_STATUS_INVALID_ARG;

    if (pValue->type != TYPE_INT32)
      return MAMA_STATUS_WRONG_FIELD_TYPE;
    *result = (uint8_t) pValue->value.int32;
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_getI16(
        Attributes*     attributes,
        const char*     name,
        mama_fid_t      fid,
        int16_t*        result)
{
	char tempName[64];
	Value* pValue = NULL;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		pValue = attributes_get(attributes, tempName);
	}
    if ((!pValue) && (name))
		pValue = attributes_get(attributes, name);
    if (!pValue)
       return MAMA_STATUS_NOT_FOUND;
    return avisValue_getI16(pValue, result);
}

mama_status avisValue_getI16(const Value* pValue, int16_t* result)
{
    if (!pValue)
      return MAMA_STATUS_INVALID_ARG;

    if (pValue->type != TYPE_INT32)
      return MAMA_STATUS_WRONG_FIELD_TYPE;
    *result = (int16_t) pValue->value.int32;
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_getU16(
        Attributes*     attributes,
        const char*     name,
        mama_fid_t      fid,
        uint16_t*       result)
{
	char tempName[64];
	Value* pValue = NULL;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		pValue = attributes_get(attributes, tempName);
	}
    if ((!pValue) && (name))
		pValue = attributes_get(attributes, name);
    if (!pValue)
      return MAMA_STATUS_NOT_FOUND;
    return avisValue_getU16(pValue, result);
}

mama_status avisValue_getU16(const Value* pValue, uint16_t* result)
{
    if (!pValue)
      return MAMA_STATUS_INVALID_ARG;

    if (pValue->type != TYPE_INT32)
      return MAMA_STATUS_WRONG_FIELD_TYPE;
    *result = (uint16_t) pValue->value.int32;
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_getI32(
    Attributes*  attributes,
    const char*  name,
    mama_fid_t   fid,
    int32_t*     result)
{
	char tempName[64];
	Value* pValue = NULL;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		pValue = attributes_get(attributes, tempName);
	}
    if ((!pValue) && (name))
		pValue = attributes_get(attributes, name);
    if (!pValue)
      return MAMA_STATUS_NOT_FOUND;
    return avisValue_getI32(pValue, result);
}

mama_status avisValue_getI32(const Value* pValue, int32_t* result)
{
    if (!pValue)
      return MAMA_STATUS_INVALID_ARG;

    if (pValue->type != TYPE_INT32)
      return MAMA_STATUS_WRONG_FIELD_TYPE;
    *result = pValue->value.int32;
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_getU32(
        Attributes*    attributes,
        const char*    name,
        mama_fid_t     fid,
        uint32_t*      result)
{
	char tempName[64];
	Value* pValue = NULL;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		pValue = attributes_get(attributes, tempName);
	}
    if ((!pValue) && (name))
		pValue = attributes_get(attributes, name);
    if (!pValue)
      return MAMA_STATUS_NOT_FOUND;
    return avisValue_getU32(pValue, result);
}

mama_status avisValue_getU32(const Value* pValue, uint32_t* result)
{
    if (!pValue)
      return MAMA_STATUS_INVALID_ARG;

    switch (pValue->type)
    {
    	case TYPE_INT32:  *result = (uint32_t) pValue->value.int32; break;
    	case TYPE_INT64:  *result = (uint32_t) pValue->value.int64; break;
    	default: return MAMA_STATUS_WRONG_FIELD_TYPE; break;
    }

    return MAMA_STATUS_OK;
}

mama_status
avisMsg_getI64(
    Attributes*  attributes,
    const char*  name,
    mama_fid_t   fid,
    int64_t*     result)
{
	char tempName[64];
	Value* pValue = NULL;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		pValue = attributes_get(attributes, tempName);
	}
    if ((!pValue) && (name))
		pValue = attributes_get(attributes, name);
    if (!pValue)
      return MAMA_STATUS_NOT_FOUND;
    return avisValue_getI64(pValue, result);
}

mama_status avisValue_getI64(const Value* pValue, int64_t* result)
{
    if (!pValue)
      return MAMA_STATUS_INVALID_ARG;

    switch (pValue->type)
    {
    	case TYPE_INT32:  *result = (int64_t) pValue->value.int32; break;
    	case TYPE_INT64:  *result = pValue->value.int64; break;
    	default: return MAMA_STATUS_WRONG_FIELD_TYPE; break;
    }

    return MAMA_STATUS_OK;
}

mama_status
avisMsg_getU64(
        Attributes*    attributes,
        const char*    name,
        mama_fid_t     fid,
        uint64_t*      result)
{
	char tempName[64];
	Value* pValue = NULL;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		pValue = attributes_get(attributes, tempName);
	}
    if ((!pValue) && (name))
		pValue = attributes_get(attributes, name);
    if (!pValue)
      return MAMA_STATUS_NOT_FOUND;
    return avisValue_getU64(pValue, result);
}

mama_status avisValue_getU64(const Value* pValue, uint64_t* result)
{
    if (!pValue)
      return MAMA_STATUS_INVALID_ARG;

    switch (pValue->type)
    {
    	case TYPE_INT32:  *result = (uint64_t) pValue->value.int32; break;
    	case TYPE_INT64:  *result = (uint64_t) pValue->value.int64; break;
    	default: return MAMA_STATUS_WRONG_FIELD_TYPE; break;
    }

    return MAMA_STATUS_OK;
}

mama_status
avisMsg_getF32(
        Attributes*    attributes,
        const char*    name,
        mama_fid_t     fid,
        mama_f32_t*    result)
{
	char tempName[64];
	Value* pValue = NULL;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		pValue = attributes_get(attributes, tempName);
	}
    if ((!pValue) && (name))
		pValue = attributes_get(attributes, name);
    if (!pValue)
      return MAMA_STATUS_NOT_FOUND;
    return avisValue_getF32(pValue, result);
}

mama_status avisValue_getF32(const Value* pValue, mama_f32_t* result)
{
    if (!pValue)
      return MAMA_STATUS_INVALID_ARG;

    if (pValue->type != TYPE_REAL64)
      return MAMA_STATUS_WRONG_FIELD_TYPE;
    *result = (mama_f32_t)pValue->value.real64;
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_getF64(
    Attributes*  attributes,
    const char*  name,
    mama_fid_t   fid,
    mama_f64_t*  result)
{
	char tempName[64];
	Value* pValue = NULL;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		pValue = attributes_get(attributes, tempName);
	}
    if ((!pValue) && (name))
		pValue = attributes_get(attributes, name);
    if (!pValue)
      return MAMA_STATUS_NOT_FOUND;
    return avisValue_getF64(pValue, result);
}

mama_status avisValue_getF64(const Value* pValue, mama_f64_t* result)
{
    if (!pValue)
      return MAMA_STATUS_INVALID_ARG;

    if (pValue->type != TYPE_REAL64)
      return MAMA_STATUS_WRONG_FIELD_TYPE;
    *result = pValue->value.real64;
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_getString(
    Attributes*  attributes,
    const char*  name,
    mama_fid_t   fid,
    const char** result)
{
	char tempName[64];
	Value* pValue = NULL;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		pValue = attributes_get(attributes, tempName);
	}
    if ((!pValue) && (name))
		pValue = attributes_get(attributes, name);
    if (!pValue)
      return MAMA_STATUS_NOT_FOUND;
    return avisValue_getString(pValue, result);
}

mama_status avisValue_getString(const Value* pValue, const char** result)
{
    if (!pValue)
      return MAMA_STATUS_INVALID_ARG;

    if (pValue->type != TYPE_STRING)
      return MAMA_STATUS_WRONG_FIELD_TYPE;
    *result = pValue->value.str;
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_getOpaque(
        Attributes*  attributes,
        const char*    name,
        mama_fid_t     fid,
        const void**   result,
        size_t*        size)
{
	char tempName[64];
	Value* pValue = NULL;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		pValue = attributes_get(attributes, tempName);
	}
    if ((!pValue) && (name))
		pValue = attributes_get(attributes, name);
    if (!pValue)
       return MAMA_STATUS_NOT_FOUND;
    return avisValue_getOpaque(pValue, result, size);
}

mama_status avisValue_getOpaque(const Value* pValue, const void** result, size_t* size)
{
    if (!pValue) return MAMA_STATUS_INVALID_ARG;

    if (!result || !size) return MAMA_STATUS_NULL_ARG;

    if (pValue->type != TYPE_OPAQUE)
       return MAMA_STATUS_WRONG_FIELD_TYPE;

    *result = pValue->value.bytes.items;
    *size = pValue->value.bytes.item_count;
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_getDateTime(
        Attributes*    attributes,
        const char*    name,
        mama_fid_t     fid,
        mamaDateTime   result)
{
	char tempName[64];
	Value* pValue = NULL;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		pValue = attributes_get(attributes, tempName);
	}
    if ((!pValue) && (name))
		pValue = attributes_get(attributes, name);
    if (!pValue)
      return MAMA_STATUS_NOT_FOUND;
    return avisValue_getDateTime(pValue, result);
}

mama_status avisValue_getDateTime(const Value* pValue, mamaDateTime result)
{
    if (!pValue) return MAMA_STATUS_NULL_ARG;
    switch (pValue->type)
    {
    	case TYPE_STRING:  mamaDateTime_setFromString (result, pValue->value.str); break;
    	case TYPE_REAL64:  mamaDateTime_setEpochTimeF64 (result, pValue->value.real64); break;
    	case TYPE_INT64:  mamaDateTime_setEpochTimeMilliseconds (result, pValue->value.int64); break;
    	default: return MAMA_STATUS_WRONG_FIELD_TYPE; break;
}
    return MAMA_STATUS_OK;
}

mama_status
avisMsg_setDateTime(
        Attributes*         attributes,
        const char*         name,
        mama_fid_t          fid,
        const mamaDateTime  value)
{
	uint64_t tempu64;
	mamaDateTime_getEpochTimeMicroseconds(value, &tempu64);
    return avisMsg_setU64(attributes, name, fid, tempu64);
}

mama_status
avisMsg_getVectorDateTime (
        Attributes*           attributes,
        const char*           name,
        mama_fid_t            fid,
        const mamaDateTime*   result,
        size_t*               resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_getVectorPrice (
        Attributes*           attributes,
        const char*           name,
        mama_fid_t            fid,
        const mamaPrice*      result,
        size_t*               resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_setPrice(
    Attributes*         attributes,
    const char*         name,
    mama_fid_t          fid,
    const mamaPrice     value)
{
	mama_f64_t tempF64;
	mamaPrice_getValue(value, &tempF64);
    return avisMsg_setF64(attributes,name,fid,tempF64);
}

mama_status
avisMsg_setVectorPrice (
    Attributes*  attributes,
    const char*         name,
    mama_fid_t          fid,
    const mamaPrice     value[],
    size_t              numElements)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_getPrice(
        Attributes*    attributes,
        const char*    name,
        mama_fid_t     fid,
        mamaPrice      result)
{
	char tempName[64];
	Value* pValue = NULL;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		pValue = attributes_get(attributes, tempName);
	}
    if ((!pValue) && (name))
		pValue = attributes_get(attributes, name);
    if (!pValue)
      return MAMA_STATUS_NOT_FOUND;
    return avisValue_getPrice(pValue, result);
}

mama_status avisValue_getPrice(const Value* pValue, mamaPrice result)
{
    if (!pValue) return MAMA_STATUS_NULL_ARG;
    if (pValue->type != TYPE_REAL64)
          return MAMA_STATUS_WRONG_FIELD_TYPE;

    mamaPrice_setWithHints (result, pValue->value.real64, MAMA_PRICE_HINTS_NONE);
    return MAMA_STATUS_NOT_IMPLEMENTED;
}


mama_status
avisMsg_getMsg (
    Attributes*         attributes,
    const char*         name,
    mama_fid_t          fid,
    msgPayload*         result)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status avisValue_getMsg(const Value* pValue, mamaMsg* result)
{
    if (!pValue) return MAMA_STATUS_NULL_ARG;
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_getVectorBool (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const mama_bool_t**  result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_getVectorChar (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const char**         result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_getVectorI8 (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const int8_t**       result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_getVectorU8 (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const uint8_t**      result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_getVectorI16 (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const int16_t**      result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_getVectorU16 (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const uint16_t**     result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_getVectorI32 (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const int32_t**      result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_getVectorU32 (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const uint32_t**     result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_getVectorI64 (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const int64_t**      result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_getVectorU64 (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const uint64_t**     result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_getVectorF32 (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const mama_f32_t**   result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_getVectorF64 (
        Attributes*          attributes,
        const char*          name,
        mama_fid_t           fid,
        const mama_f64_t**   result,
        size_t*              resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_getVectorString (
        Attributes*    attributes,
        const char*    name,
        mama_fid_t     fid,
        const char***  result,
        size_t*        resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_getVectorMsg (
    Attributes*         attributes,
    const char*         name,
    mama_fid_t          fid,
    const msgPayload**  result,
    size_t*             resultLen)
{
    return MAMA_STATUS_NOT_IMPLEMENTED;
}

mama_status
avisMsg_getFieldAsString(
    Attributes*  attributes,
    const char*  name,
    mama_fid_t   fid,
    char*        buf,
    size_t       len)
{
	char tempName[64];
	Value* pValue = NULL;
	if(fid != 0)
	{
		snprintf (tempName, 63, "%d", fid);
		pValue = attributes_get(attributes, tempName);
	}
    if ((!pValue) && (name))
		pValue = attributes_get(attributes, name);

    if (!pValue)
      return MAMA_STATUS_NOT_FOUND;

    return avisValue_getFieldAsString(pValue, name, fid, buf, len);
}

mama_status avisValue_getFieldAsString(const Value* pValue, const char* name, mama_fid_t fid, char* buf, size_t len)
{
    if (!pValue || !buf) return MAMA_STATUS_NULL_ARG;

    switch( pValue->type )
    {
    case TYPE_INT32:
        snprintf( buf, len, "%d", pValue->value.int32 );
        break;
    case TYPE_INT64:
        snprintf( buf, len, "%lld", (long long) pValue->value.int64 );
        break;
    case TYPE_REAL64:
        snprintf( buf, len, "%f", pValue->value.real64);
        break;
    case TYPE_STRING:
        snprintf( buf, len, "%s", pValue->value.str);
        break;
    case TYPE_OPAQUE:
        snprintf( buf, len, "%s", "<OPAQUE>");
        break;
    default:
        snprintf( buf, len, "unknown type: %d fid:%d name:%s", pValue->type, fid, name );
        break;
    }

    /*As users do not know how long the string will be we always add a NULL
      terminator to the string in case the buffer is not big enough.
      TODO: Return a value to the user indicating the the buffer was too small.*/
    buf [len-1] = '\0';

    return MAMA_STATUS_OK;
}
