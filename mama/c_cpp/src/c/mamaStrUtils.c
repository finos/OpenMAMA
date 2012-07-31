/* $Id: mamaStrUtils.c,v 1.2.32.2 2011/08/10 14:53:25 nicholasmarriott Exp $
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

#include "mamaStrUtils.h"
#include <string.h>
#include <assert.h>


void printChar (
    char*         result,
    mama_size_t*  resLen,
    mama_size_t   maxLen,
    char          c)
{
    if (*resLen < maxLen)
    {
        result[(*resLen)++] = c;
    }
}

void printDigit (
    char*         result,
    mama_size_t*  resLen,
    mama_size_t   maxLen,
    mama_u32_t    digit)
{
    if (*resLen < maxLen)
    {
        result[(*resLen)++] = digit + '0';
    }
}

void printString (
    char*         result,
    mama_size_t*  resLen,
    mama_size_t   maxLen,
    const char*   str)
{
    while (*str && (*resLen < maxLen))
    {
        result[(*resLen)++] = *str;
        ++str;
    }
}
