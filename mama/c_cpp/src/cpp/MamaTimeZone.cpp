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

#include <mama/MamaTimeZone.h>
#include <stdio.h>

namespace Wombat
{

    static const MamaTimeZone* sTzLocal   = NULL;
    static const MamaTimeZone* sTzUtc     = NULL;
    static const MamaTimeZone* sTzEastern = NULL;

    const MamaTimeZone&  MamaTimeZone::local()
    {
        if (!sTzLocal)
        {
            sTzLocal = new MamaTimeZone (mamaTimeZone_local());
        }
        return *sTzLocal;
    }

    const MamaTimeZone&  MamaTimeZone::utc()
    {
        if (!sTzUtc)
        {
            sTzUtc = new MamaTimeZone (mamaTimeZone_utc());
        }
        return *sTzUtc;
    }

    const MamaTimeZone&  MamaTimeZone::usEastern()
    {
        if (!sTzEastern)
        {
            sTzEastern = new MamaTimeZone (mamaTimeZone_usEastern());
        }
        return *sTzEastern;
    }

    MamaTimeZone::MamaTimeZone()
        : myCimpl(NULL)
    {
        mamaTimeZone_create (&myCimpl);
    }

    /* Note: this constructor is only used in conjunction with the static
     * member methods local(), utc() and usEastern(), which should use
     * their C API eqivalent. It should never be required to invoke the 
     * destructor on these objects */
    MamaTimeZone::MamaTimeZone (mamaTimeZone  cimpl)
        : myCimpl(cimpl)
    {
    }

    MamaTimeZone::MamaTimeZone (const char*  tz)
        : myCimpl(NULL)
    {
        mamaTimeZone_createFromTz (&myCimpl, tz);
    }

    MamaTimeZone::MamaTimeZone (const MamaTimeZone&  copy)
        : myCimpl(NULL)
    {
        mamaTimeZone_createCopy (&myCimpl, copy.myCimpl);
    }

    MamaTimeZone::~MamaTimeZone ()
    {
        mamaTimeZone_destroy (myCimpl);
    }

    MamaTimeZone& MamaTimeZone::operator= (const MamaTimeZone&  rhs)
    {
        mamaTimeZone_copy (myCimpl, rhs.myCimpl);
        return *this;
    }

    void MamaTimeZone::set (const char*  tz)
    {
        mamaTimeZone_set (myCimpl, tz);
    }

    void MamaTimeZone::clear ()
    {
        mamaTimeZone_clear (myCimpl);
    }

    const char* MamaTimeZone::tz() const
    {
        const char* result = "";
        mamaTimeZone_getTz (myCimpl, &result);
        return result;
    }

    mama_i32_t MamaTimeZone::offset() const
    {
        mama_i32_t result = 0;
        mamaTimeZone_getOffset (myCimpl, &result);
        return result;
    }

    void MamaTimeZone::check ()
    {
        mamaTimeZone_check (myCimpl);
    }

    void MamaTimeZone::setScanningInterval (mama_f64_t  seconds)
    {
        mamaTimeZone_setScanningInterval (seconds);
    }

} /* namespace  Wombat */
