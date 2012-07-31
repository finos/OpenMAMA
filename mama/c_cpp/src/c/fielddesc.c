/* $Id: fielddesc.c,v 1.17.14.1.16.3 2011/08/10 14:53:25 nicholasmarriott Exp $
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

#include <mama/mama.h>
#include <mama/fielddesc.h>
#include "fielddescimpl.h"

mama_status
mamaFieldDescriptor_create (
    mamaFieldDescriptor*  descriptor, 
    mama_fid_t            fid,
    mamaFieldType         wType,
    const char*           name)
{
    mamaFieldDescriptorImpl* impl = (mamaFieldDescriptorImpl*) 
                            calloc (1, sizeof(mamaFieldDescriptorImpl));
   
    if (impl == NULL)
    {
        return MAMA_STATUS_NOMEM;
    }
    *descriptor = NULL;

    if(name == NULL) name = "";
    impl->mFid  = fid;
    impl->mType = wType;
    impl->mName = (char *)calloc (strlen (name)+1, sizeof (char));
    strncpy (impl->mName, name, strlen (name) + 1);

    if (!impl->mName)
    {
        free(impl);
        return MAMA_STATUS_NOMEM;
    }

    impl->mClosure = NULL;
    
    *descriptor = (mamaFieldDescriptor) impl;
    return MAMA_STATUS_OK;

}

mama_status
mamaFieldDescriptor_destroy (mamaFieldDescriptor descriptor)
{
    mamaFieldDescriptorImpl* impl = (mamaFieldDescriptorImpl*) descriptor;

    if (impl == NULL)
        return MAMA_STATUS_OK;

    free (impl->mName);
    free (impl);

    descriptor = NULL;
    return MAMA_STATUS_OK;
}


mama_fid_t
mamaFieldDescriptor_getFid (const mamaFieldDescriptor descriptor)
{
    mamaFieldDescriptorImpl* impl = (mamaFieldDescriptorImpl*) descriptor;

    if (impl == NULL)
    {
        return -1;
    }

    return impl->mFid;
}

mamaFieldType 
mamaFieldDescriptor_getType (const mamaFieldDescriptor descriptor)
{
    mamaFieldDescriptorImpl* impl = (mamaFieldDescriptorImpl*) descriptor;

    if (impl == NULL)
    {
        return -1;
    }

    return impl->mType;
}

const char*
mamaFieldDescriptor_getName (const mamaFieldDescriptor descriptor)
{
    mamaFieldDescriptorImpl* impl = (mamaFieldDescriptorImpl*) descriptor;

    if (impl == NULL)
    {
        return NULL;
    }

    return (impl->mName);
}


const char*
mamaFieldDescriptor_getTypeName (const mamaFieldDescriptor descriptor)
{
    mamaFieldDescriptorImpl* impl = (mamaFieldDescriptorImpl*) descriptor;

    if (impl == NULL)
    {
        return "UNKNOWN";
    }

    return mamaFieldTypeToString (impl->mType);
}

mama_status
mamaFieldDescriptor_setClosure(
                    mamaFieldDescriptor  descriptor,
                    void*                closure)
{
    mamaFieldDescriptorImpl* impl = (mamaFieldDescriptorImpl*) descriptor;
    if (!impl) return MAMA_STATUS_NULL_ARG;
  
    impl->mClosure = closure;
    
    return MAMA_STATUS_OK;
}
                                                                                                              
mama_status
mamaFieldDescriptor_getClosure(
                mamaFieldDescriptor  descriptor,
                void**               closure)
{
    mamaFieldDescriptorImpl* impl = (mamaFieldDescriptorImpl*) descriptor;
    if (!impl) return MAMA_STATUS_NULL_ARG;
    
    *closure = impl->mClosure;

    return MAMA_STATUS_OK;
}

const char*
mamaFieldTypeToString (mamaFieldType type)
{
    switch (type)
    {
    case MAMA_FIELD_TYPE_MSG:
        return "MSG";
    case MAMA_FIELD_TYPE_OPAQUE:
        return "OPAQUE";
    case MAMA_FIELD_TYPE_STRING:
        return "STRING";
    case MAMA_FIELD_TYPE_BOOL:
        return "BOOLEAN";
    case MAMA_FIELD_TYPE_CHAR:
        return "CHAR";
    case MAMA_FIELD_TYPE_I8:
        return "I8";
    case MAMA_FIELD_TYPE_U8:
        return "U8";
    case MAMA_FIELD_TYPE_I16:
        return "I16";
    case MAMA_FIELD_TYPE_U16:
        return "U16";
    case MAMA_FIELD_TYPE_I32:
        return "I32";
    case MAMA_FIELD_TYPE_U32:
        return "U32";
    case MAMA_FIELD_TYPE_I64:
        return "I64";
    case MAMA_FIELD_TYPE_U64:
        return "U64";
    case MAMA_FIELD_TYPE_F32:
        return "F32";
    case MAMA_FIELD_TYPE_F64:
        return "F64";
    case MAMA_FIELD_TYPE_TIME:
        return "TIME";
    case MAMA_FIELD_TYPE_PRICE:
        return "PRICE";
    case MAMA_FIELD_TYPE_VECTOR_I8:
        return "VECTOR_I8";
    case MAMA_FIELD_TYPE_VECTOR_U8:
        return "VECTOR_U8";
    case MAMA_FIELD_TYPE_VECTOR_I16:
        return "VECTOR_I16";
    case MAMA_FIELD_TYPE_VECTOR_U16:
        return "VECTOR_U16";
    case MAMA_FIELD_TYPE_VECTOR_I32:
        return "VECTOR_I32";
    case MAMA_FIELD_TYPE_VECTOR_U32:
        return "VECTOR_U32";
    case MAMA_FIELD_TYPE_VECTOR_I64:
        return "VECTOR_I64";
    case MAMA_FIELD_TYPE_VECTOR_U64:
        return "VECTOR_U64";
    case MAMA_FIELD_TYPE_VECTOR_F32:
        return "VECTOR_F32";
    case MAMA_FIELD_TYPE_VECTOR_F64:
        return "VECTOR_F64";
    case MAMA_FIELD_TYPE_VECTOR_STRING:
        return "VECTOR_STRING";
    case MAMA_FIELD_TYPE_VECTOR_MSG:
        return "VECTOR_MSG";
    case MAMA_FIELD_TYPE_VECTOR_TIME:
        return "VECTOR_TIME";
    case MAMA_FIELD_TYPE_VECTOR_PRICE:
        return "VECTOR_PRICE";
    case MAMA_FIELD_TYPE_QUANTITY:
        return "QUANTITY";
    case MAMA_FIELD_TYPE_COLLECTION:
        return "COLLECTION";
    default:
        return "UNKNOWN";
    }
}

mamaFieldType
stringToMamaFieldType (const char* str)
{
    if (strcmp(str, "MSG") == 0)
        return MAMA_FIELD_TYPE_MSG;
    if (strcmp(str, "OPAQUE") == 0)
        return MAMA_FIELD_TYPE_OPAQUE;
    if (strcmp(str, "STRING") == 0)
        return MAMA_FIELD_TYPE_STRING;
    if (strcmp(str, "BOOLEAN") == 0)
        return MAMA_FIELD_TYPE_BOOL;
    if (strcmp(str, "CHAR") == 0)
        return MAMA_FIELD_TYPE_CHAR;
    if (strcmp(str, "I8") == 0)
        return MAMA_FIELD_TYPE_I8;
    if (strcmp(str, "U8") == 0)
        return MAMA_FIELD_TYPE_U8;
    if (strcmp(str, "I16") == 0)
        return MAMA_FIELD_TYPE_I16;
    if (strcmp(str, "U16") == 0)
        return MAMA_FIELD_TYPE_U16;
    if (strcmp(str, "I32") == 0)
        return MAMA_FIELD_TYPE_I32;
    if (strcmp(str, "U32") == 0)
        return MAMA_FIELD_TYPE_U32;
    if (strcmp(str, "I64") == 0)
        return MAMA_FIELD_TYPE_I64;
    if (strcmp(str, "U64") == 0)
        return MAMA_FIELD_TYPE_U64;
    if (strcmp(str, "F32") == 0)
        return MAMA_FIELD_TYPE_F32;
    if (strcmp(str, "F64") == 0)
        return MAMA_FIELD_TYPE_F64;
    if (strcmp(str, "TIME") == 0)
        return MAMA_FIELD_TYPE_TIME;
    if (strcmp(str, "PRICE") == 0)
        return MAMA_FIELD_TYPE_PRICE;
    if (strcmp(str, "VECTOR_I8") == 0)
        return MAMA_FIELD_TYPE_VECTOR_I8;
    if (strcmp(str, "VECTOR_U8") == 0)
        return MAMA_FIELD_TYPE_VECTOR_U8;
    if (strcmp(str, "VECTOR_I16") == 0)
        return MAMA_FIELD_TYPE_VECTOR_I16;
    if (strcmp(str, "VECTOR_U16") == 0)
        return MAMA_FIELD_TYPE_VECTOR_U16;
    if (strcmp(str, "VECTOR_I32") == 0)
        return MAMA_FIELD_TYPE_VECTOR_I32;
    if (strcmp(str, "VECTOR_U32") == 0)
        return MAMA_FIELD_TYPE_VECTOR_U32;
    if (strcmp(str, "VECTOR_I64") == 0)
        return MAMA_FIELD_TYPE_VECTOR_I64;
    if (strcmp(str, "VECTOR_U64") == 0)
        return MAMA_FIELD_TYPE_VECTOR_U64;
    if (strcmp(str, "VECTOR_F32") == 0)
        return MAMA_FIELD_TYPE_VECTOR_F32;
    if (strcmp(str, "VECTOR_F64") == 0)
        return MAMA_FIELD_TYPE_VECTOR_F64;
    if (strcmp(str, "VECTOR_STRING") == 0)
        return MAMA_FIELD_TYPE_VECTOR_STRING;
    if (strcmp(str, "VECTOR_MSG") == 0)
        return MAMA_FIELD_TYPE_VECTOR_MSG;
    if (strcmp(str, "VECTOR_TIME") == 0)
        return MAMA_FIELD_TYPE_VECTOR_TIME;
    if (strcmp(str, "VECTOR_PRICE") == 0)
        return MAMA_FIELD_TYPE_VECTOR_PRICE;
    if (strcmp(str, "QUANTITY") == 0)
        return MAMA_FIELD_TYPE_QUANTITY;
    if (strcmp(str, "COLLECTION") == 0)
        return MAMA_FIELD_TYPE_COLLECTION;

    return MAMA_FIELD_TYPE_UNKNOWN;
}

