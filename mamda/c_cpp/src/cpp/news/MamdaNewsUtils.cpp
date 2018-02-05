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

#include "MamdaNewsUtils.h"
#include <string.h>

void copyStringArray (
    const char**&      dest,
    mama_size_t&       destNum,
    const char*        strArray[],
    mama_size_t        strArrayNum)
{
    dest    = new const char* [strArrayNum];
    destNum = strArrayNum;

    for (mama_size_t i = 0; i < strArrayNum; i++)
    {
        mama_size_t len = strlen (strArray[i]);
        char* copy      = new char[len+1];

        strcpy (copy, strArray[i]);

        dest[i] = copy;
    }
}

void destroyStringArray (
    const char*       strArray[],
    mama_size_t       strArrayNum)
{
    for (mama_size_t i = 0; i < strArrayNum; i++)
    {
        delete[] (char*)strArray[i];
    }

    delete[] strArray;
    strArray = NULL;
}
