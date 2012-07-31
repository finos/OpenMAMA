/* $Id: msgqualifier.c,v 1.3.32.3 2011/08/10 14:53:25 nicholasmarriott Exp $
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
#include <mama/msgqualifier.h>
#include "mamaStrUtils.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define self ((mamaMsgQualImpl*) (msgQual))

/**
 * Wrapper structure for Message Qualifier "bit-map"
 *  MAMA_MSG_QUAL_POSSIBLY_DUPLICATE   (bit 1) 0x0001
 *  MAMA_MSG_QUAL_DEFINATELY_DUPLICATE (bit 2) 0x0002
 *  MAMA_MSG_QUAL_POSSIBLY_DELAYED     (bit 3) 0x0004
 *  MAMA_MSG_QUAL_DEFINATELY_DELAYED   (bit 4) 0x0008
 *  MAMA_MSG_QUAL_OUT_OF_SEQUENCE      (bit 5) 0x0010
 */
typedef struct mamaMsgQualImpl_
{
    mama_u16_t   mValue;

} mamaMsgQualImpl;


mama_status
mamaMsgQual_create (mamaMsgQual*  result)
{
    mamaMsgQualImpl*  msgQual;

    /* Create Impl */
    msgQual = (mamaMsgQualImpl*)calloc(1, sizeof(mamaMsgQualImpl));
    if (self == NULL)
    {
        return MAMA_STATUS_NOMEM;
    }
    
    /* Initialize Impl */
    self->mValue = 0;
 
    *result = (mamaMsgQual)self;
    
    return MAMA_STATUS_OK;
}

mama_status
mamaMsgQual_destroy (mamaMsgQual  msgQual)
{
    if (!self)
        return MAMA_STATUS_NULL_ARG;

    free (self);
    return MAMA_STATUS_OK;
}

mama_status
mamaMsgQual_clear (mamaMsgQual  msgQual)
{
    if (!self)
        return MAMA_STATUS_NULL_ARG;

    self->mValue   = 0;
    return MAMA_STATUS_OK;
}

mama_status
mamaMsgQual_copy (mamaMsgQual        dest,
                  const mamaMsgQual  src)
{
    if (!(mamaMsgQualImpl*)src || !(mamaMsgQualImpl*)dest)
        return MAMA_STATUS_NULL_ARG;

    ((mamaMsgQualImpl*)dest)->mValue = ((mamaMsgQualImpl*)src)->mValue;

    return MAMA_STATUS_OK;
}

int mamaMsgQual_equal (const mamaMsgQual  lhs,
                       const mamaMsgQual  rhs)
{

    if (!(mamaMsgQualImpl*)lhs || !(mamaMsgQualImpl*)rhs)
        return MAMA_STATUS_NULL_ARG;

    if (((mamaMsgQualImpl*)lhs)->mValue == ((mamaMsgQualImpl*)rhs)->mValue)
        return 1;
    else
        return 0;
}

mama_status
mamaMsgQual_setValue (mamaMsgQual           msgQual,
                      mama_u16_t            value)
{
    if (!self)
        return MAMA_STATUS_NULL_ARG;

    self->mValue = value;
    return MAMA_STATUS_OK;
}

mama_status
mamaMsgQual_setIsDefinitelyDuplicate (mamaMsgQual msgQual, int state)
{
    if (!self)
        return MAMA_STATUS_NULL_ARG;
   
    if (state == 0)
        self->mValue &= ~MAMA_MSG_QUAL_DEFINITELY_DUPLICATE;
    else
        self->mValue |= MAMA_MSG_QUAL_DEFINITELY_DUPLICATE;
    return MAMA_STATUS_OK;    
}

mama_status
mamaMsgQual_setIsPossiblyDuplicate (mamaMsgQual msgQual, int state)
{
    if (!self)
        return MAMA_STATUS_NULL_ARG;
   
    if (state == 0)
        self->mValue &= ~MAMA_MSG_QUAL_POSSIBLY_DUPLICATE;
    else
        self->mValue |= MAMA_MSG_QUAL_POSSIBLY_DUPLICATE;
    return MAMA_STATUS_OK;    
}

mama_status
mamaMsgQual_setIsDefinitelyDelayed (mamaMsgQual msgQual, int state)
{
    if (!self)
        return MAMA_STATUS_NULL_ARG;
   
    if (state == 0)
        self->mValue &= ~MAMA_MSG_QUAL_DEFINITELY_DELAYED;
    else
        self->mValue |= MAMA_MSG_QUAL_DEFINITELY_DELAYED;
    return MAMA_STATUS_OK;    
}

mama_status
mamaMsgQual_setIsPossiblyDelayed (mamaMsgQual msgQual, int state)
{
    if (!self)
        return MAMA_STATUS_NULL_ARG;
   
    if (state == 0)
        self->mValue &= ~MAMA_MSG_QUAL_POSSIBLY_DELAYED;
    else
        self->mValue |= MAMA_MSG_QUAL_POSSIBLY_DELAYED;
    return MAMA_STATUS_OK;    
}

mama_status
mamaMsgQual_setIsOutOfSequence (mamaMsgQual msgQual, int state)
{
    if (!self)
        return MAMA_STATUS_NULL_ARG;
   
    if (state == 0)
        self->mValue &= ~MAMA_MSG_QUAL_OUT_OF_SEQUENCE;
    else
        self->mValue |= MAMA_MSG_QUAL_OUT_OF_SEQUENCE;
    return MAMA_STATUS_OK;    
}

mama_status
mamaMsgQual_getIsDefinitelyDuplicate (mamaMsgQual msgQual, int* result)
{
    if (!self)
        return MAMA_STATUS_NULL_ARG;

    *result = self->mValue & MAMA_MSG_QUAL_DEFINITELY_DUPLICATE;
    return MAMA_STATUS_OK;    
}

mama_status
mamaMsgQual_getIsPossiblyDuplicate (mamaMsgQual  msgQual, int* result)
{
    if (!self)
        return MAMA_STATUS_NULL_ARG;

    *result = self->mValue & MAMA_MSG_QUAL_POSSIBLY_DUPLICATE;
    return MAMA_STATUS_OK;    
}

mama_status
mamaMsgQual_getIsDefinitelyDelayed (mamaMsgQual  msgQual, int* result)
{
    if (!self)
        return MAMA_STATUS_NULL_ARG;

    *result = self->mValue & MAMA_MSG_QUAL_DEFINITELY_DELAYED;
    return MAMA_STATUS_OK;    
}

mama_status
mamaMsgQual_getIsPossiblyDelayed (mamaMsgQual  msgQual, int* result)
{
    if (!self)
        return MAMA_STATUS_NULL_ARG;

    *result = self->mValue & MAMA_MSG_QUAL_POSSIBLY_DELAYED;
    return MAMA_STATUS_OK;    
}

mama_status
mamaMsgQual_getIsOutOfSequence (mamaMsgQual  msgQual, int* result)
{
    if (!self)
        return MAMA_STATUS_NULL_ARG;

    *result = self->mValue & MAMA_MSG_QUAL_OUT_OF_SEQUENCE;
    return MAMA_STATUS_OK;    
}

mama_status
mamaMsgQual_getValue(
    const mamaMsgQual      msgQual,
    mama_u16_t*            value)
{
    if (!self || !value)
        return MAMA_STATUS_NULL_ARG;

    *value = self->mValue;
    return MAMA_STATUS_OK;
}

mama_status mamaMsgQual_getAsString (
    const mamaMsgQual    msgQual,
    const char*          delimiter,
    char*                buf,
    mama_size_t          bufMaxLen)
{
    mama_u16_t  value = 0;

    if (!self || !buf)
        return MAMA_STATUS_NULL_ARG;

    (void) mamaMsgQual_getValue(msgQual, &value);    
    return mamaMsgQual_valueToString(value, delimiter, buf, bufMaxLen);
}


mama_status mamaMsgQual_valueToString (
    mama_u16_t           value,
    const char*          delimiter,
    char*                buf,
    mama_size_t          bufMaxLen)
{
    mama_status  status  = MAMA_STATUS_OK;
    size_t       tmpSize = 0;
 
    /**
     * MAMA_MSG_QUAL_MAX_STR_LEN takes into consideration individual string 
     * token lengths (see below; Definately Duplicate, Possibly duplicate...)
     * and the default delimiter, "delim" ( : ). The caller can supply their
     * own delimiter. This function should check the size of the delimiter
     * arguement passed and resize, or check available space before calls 
     * to strcat. I've opted, for now to have a sufficiently big buffer.
     */ 
    const char*  delim   = " : ";
    char         tmpBuf[2*MAMA_MSG_QUAL_MAX_STR_LEN];
    tmpBuf[0]=   '\0';

    if (!buf)
        return MAMA_STATUS_NULL_ARG;

    /* Use the delimiter suplied by caller */
    if (delimiter) delim = delimiter;
    
    /* Build up string */
    if (value & MAMA_MSG_QUAL_DEFINITELY_DUPLICATE)
    {
        char* tmp = "Definately Duplicate";
        strcat(tmpBuf, tmp);
        strcat(tmpBuf, delim);
    }
    if (value & MAMA_MSG_QUAL_POSSIBLY_DUPLICATE)
    {
        strcat(tmpBuf, "Possibly duplicate");
        strcat(tmpBuf, delim);
    }
    if (value & MAMA_MSG_QUAL_DEFINITELY_DELAYED)
    {
        strcat(tmpBuf, "Definately delayed");
        strcat(tmpBuf, delim);
    }
    if (value & MAMA_MSG_QUAL_POSSIBLY_DELAYED)
    {
        strcat(tmpBuf, "Possibly delayed");
        strcat(tmpBuf, delim);
    }
    if ( value & MAMA_MSG_QUAL_OUT_OF_SEQUENCE)
    {
        strcat(tmpBuf, "Out of sequence");
        strcat(tmpBuf, delim);
    }

    /* Adjust size for unwanted last delimiter */
    tmpSize = strlen(tmpBuf) - strlen(delim);
    tmpBuf[tmpSize] = '\0';

    /* If string to big for available memory then cut short */
    if (tmpSize >= bufMaxLen)
    {
        tmpBuf[bufMaxLen] = '\0';
        status  = MAMA_STATUS_NOMEM;
    }

    /* copy to buffer */
    strcpy(buf, tmpBuf);

    return status;
}
