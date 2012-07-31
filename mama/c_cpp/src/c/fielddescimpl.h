/* $Id: fielddescimpl.h,v 1.2.2.2.16.4 2011/09/01 16:34:37 emmapollock Exp $
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

#ifndef FIELDDESCIMPL_H
#define FIELDDESCIMPL_H

/* ************************************************************************* */
/* Includes. */
/* ************************************************************************* */
#include "mama/config.h"
#include "mama/fielddesc.h"
#include "mama/status.h"
#include "mama/types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* ************************************************************************* */
/* Structures. */
/* ************************************************************************* */
/* This structure describes the field descriptor. */
typedef struct mamaFieldDescriptorImpl_
{
    mama_fid_t        mFid;
    mamaFieldType     mType;
    char*             mName;
    void*             mClosure;
} mamaFieldDescriptorImpl;

#if defined(__cplusplus)
}
#endif

#endif /*FIELDDESCIMP_H*/
