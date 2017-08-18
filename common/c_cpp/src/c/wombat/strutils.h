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

#ifndef _WOMBAT_STRUTILS_H
#define _WOMBAT_STRUTILS_H

#include <wombat/wincompat.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define VERSION_INFO_STR_MAX    64
#define VERSION_INFO_EXTRA_MAX  16

typedef unsigned long int       w_u32_t;
typedef long int                w_i32_t;
typedef float                   w_f32_t;
typedef double                  w_f64_t;
typedef unsigned long long int  w_u64_t;
typedef long long int           w_i64_t;

typedef struct versionInfo {
    int mMajor;
    int mMinor;
    int mRelease;
    char mExtra[VERSION_INFO_EXTRA_MAX];
} versionInfo;

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

/**
 * Convert string (e.g. "2.3.3rc1") to versionInfo struct and return:
 * {
 *   int mMajor   = 2;
 *   int mMinor   = 3;
 *   int mRelease = 3;
 *   int mExtra   = "rc1";
 * }
 */
COMMONExpDLL
int strToVersionInfo(const char* s, versionInfo* version);

/* 
 * Replace any environment variables with their
 * equivalents in the string passed in.
 *
 * @param value [I] The string that may contain environment variables.
 *
 * @return NULL if the value string did not containing any environment variables.
 * A valid string if variables were replaced.  This string must be freed with free().
 */
COMMONExpDLL
char* strReplaceEnvironmentVariable(const char* value);

#if defined(__cplusplus)
}
#endif

#endif /* _WOMBAT_STRUTILS_H */
