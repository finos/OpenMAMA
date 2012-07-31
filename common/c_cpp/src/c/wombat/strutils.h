/* $Id: strutils.h,v 1.10.16.3 2011/08/10 14:53:24 nicholasmarriott Exp $
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

#ifndef _WOMBAT_STRUTILS_H
#define _WOMBAT_STRUTILS_H

#include <wombat/wincompat.h>

#if defined(__cplusplus)
extern "C" {
#endif


typedef unsigned long int       w_u32_t;
typedef long int                w_i32_t;
typedef float                   w_f32_t;
typedef double                  w_f64_t;
typedef unsigned long long int  w_u64_t;
typedef long long int           w_i64_t;

COMMONExpDLL
int wmFastCopyAndShiftStr (
    char**       result,
    size_t*      maxLen,
    const char*  value);

COMMONExpDLL
int wmFastCopyAndShiftChar  (
    char**       result,
    size_t*      maxLen,
    char         value);

COMMONExpDLL
int wmFastPrintAndShiftF64  (
    char**       result,
    size_t*      maxLen,
    w_f64_t      value,
    size_t       places);

COMMONExpDLL
size_t wmStrSizeCpy (
    char*          dst, 
    const char*    src, 
    size_t         siz);

COMMONExpDLL
int wmFastPrintU32 (
    char*         result,
    size_t        maxLen,
    w_u32_t       value,
    size_t        minWidth);

COMMONExpDLL
int wmFastPrintI32 (
    char*         result,
    size_t        maxLen,
    w_i32_t       value,
    size_t        minWidth);

COMMONExpDLL
int wmFastPrintU64 (
    char*         result,
    size_t        maxLen,
    w_u64_t       value,
    size_t        minWidth);

COMMONExpDLL
int wmFastPrintI64 (
    char*         result,
    size_t        maxLen,
    w_i64_t       value,
    size_t        minWidth);

COMMONExpDLL
int wmFastPrintF32 (
    char*         result,
    size_t        maxLen,
    w_f32_t       value,
    size_t        decimalPlaces);

COMMONExpDLL
int wmFastPrintF64 (
    char*         result,
    size_t        maxLen,
    w_f64_t       value,
    size_t        decimalPlaces);

/*
 * change all characters in a string to lowercase
 */
COMMONExpDLL
char* toLowerCase(char* s);

/*
 * Allocate a sufficiently large string and print into it
 */
COMMONExpDLL
char* printfAlloc(const char *fmt, ...);

/*
 * Similar to standard strlen(), but will return 0 for s=NULL,
 * instead of crashing
 */
COMMONExpDLL
int strlenEx(const char* s);

/*
 * concatenate two strings, allocating a sufficient amount of memory
 */
COMMONExpDLL
int strcatAlloc(char** s, const char* ct);

/**
 * convert string (e.g. "true", "false", "yes, "no", etc) to boolean
 * Return zero for the falsies, non-zero otherwise
 */
COMMONExpDLL 
int strtobool(const char* s);

#if defined(__cplusplus)
}
#endif

#endif /* _WOMBAT_STRUTILS_H */
