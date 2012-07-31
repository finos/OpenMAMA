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

#include <mamda/MamdaOptionExpirationDateSet.h>
#include <mama/mamacpp.h>

namespace Wombat
{

    MamdaOptionExpirationDateSet::MamdaOptionExpirationDateSet ()
    {
    }

    MamdaOptionExpirationDateSet::MamdaOptionExpirationDateSet (
        const MamdaOptionExpirationDateSet&  copy)
        : std::map<MamaDateTime, MamdaOptionExpirationStrikes*>(copy)
    {
        copyStrikes (copy);
    }

    MamdaOptionExpirationDateSet::~MamdaOptionExpirationDateSet ()
    {
    }

    MamdaOptionExpirationDateSet& MamdaOptionExpirationDateSet::operator= (
        const MamdaOptionExpirationDateSet&  rhs)
    {
        if (this != &rhs)
        {
            copyStrikes (rhs);
        }
        return *this;
    }

    void MamdaOptionExpirationDateSet::getExpirationsBefore (
        MamdaOptionExpirationDateSet&  result,
        const MamaDateTime&            date) const
    {
        if (this == &result)
            return; 

        result.copyStrikes (*this);
        result.erase (result.upper_bound(date), result.end());
    }

    void MamdaOptionExpirationDateSet::getExpirations (
        MamdaOptionExpirationDateSet&  result,
        int                            numExpirations) const
    {
        if (this == &result)
            return;

        MamaDateTime date;
        const_iterator iterEnd = end();
        const_iterator i;

        for (i = begin();
             (i != iterEnd) && (numExpirations > 0);
             ++i)
        {
            date = i->first;
            --numExpirations;
        }

        if ((numExpirations == 0) && (!date.empty()))
        {
            getExpirationsBefore(result,date);
        }
        else
        {
            result.copyStrikes(*this);
        }
    }

    const char* MamdaOptionExpirationDateSet::toString() const
    {
        return "MamdaOptionExpirationDateSet::toString(): not implemented!";
    }

    void MamdaOptionExpirationDateSet::copyStrikes (
        const MamdaOptionExpirationDateSet&  rhs)
    {
        // Shallow copy is not good enough.  We want a copy that deep
        // copies the MamdaOptionExpirationStrikes, but not the
        // contents of those strike sets.

        MamaDateTime today;
        today.setToNow();

        clear();
        const_iterator iterEnd = rhs.end();
        const_iterator i;

        for (i = rhs.begin(); i != iterEnd; ++i)
        {
            const MamaDateTime& expireDate = i->first;
            MamdaOptionExpirationStrikes* strikes = i->second;
            insert (value_type (expireDate, new MamdaOptionExpirationStrikes(*strikes)));
        }
    }

} // namespace
