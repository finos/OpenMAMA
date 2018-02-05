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

#include <mamda/MamdaOptionStrikeSet.h>
#include <mama/mamacpp.h>
#include <cstring>

using std::strcmp;

namespace Wombat
{

    struct MamdaOptionStrikeSet::MamdaOptionStrikeSetImpl
    {
        MamdaOptionStrikeSetImpl (
            const MamaDateTime&  expireDate,
            double               strikePrice)
            : mExpireDate  (expireDate)
            , mStrikePrice (strikePrice)
        {}

        MamaDateTime           mExpireDate;
        double                 mStrikePrice;
        MamdaOptionContractSet mCallSet;
        MamdaOptionContractSet mPutSet;
        char                   mExpireDateStr[32];
    };


    MamdaOptionStrikeSet::MamdaOptionStrikeSet (
        const MamaDateTime&  expireDate,
        double               strikePrice)
        : mImpl (*new MamdaOptionStrikeSetImpl (expireDate, strikePrice))
    {
    }

    MamdaOptionStrikeSet::~MamdaOptionStrikeSet ()
    {
        delete &mImpl;
    }

    MamdaOptionContractSet* MamdaOptionStrikeSet::getCallSet()
    {
        return &mImpl.mCallSet;
    }

    const MamdaOptionContractSet* MamdaOptionStrikeSet::getCallSet() const
    {
        return &mImpl.mCallSet;
    }

    MamdaOptionContractSet* MamdaOptionStrikeSet::getPutSet()
    {
        return &mImpl.mPutSet;
    }

    const MamdaOptionContractSet* MamdaOptionStrikeSet::getPutSet() const
    {
        return &mImpl.mPutSet;
    }

    const MamaDateTime& MamdaOptionStrikeSet::getExpireDate () const
    {
        return mImpl.mExpireDate;
    }

    const char* MamdaOptionStrikeSet::getExpireDateStr () const
    {
        const char*  str = mImpl.mExpireDate.getAsString();
        if (str && strlen(str)>=7)
        {
            // Copy mm/YY (from YYYY/mm/...)
            mImpl.mExpireDateStr[0] = str[5];
            mImpl.mExpireDateStr[1] = str[6];
            mImpl.mExpireDateStr[2] = '/';
            mImpl.mExpireDateStr[3] = str[2];
            mImpl.mExpireDateStr[4] = str[3];
            mImpl.mExpireDateStr[5] = '\0';
        }

        return mImpl.mExpireDateStr;
    }

    double MamdaOptionStrikeSet::getStrikePrice () const
    {
        return mImpl.mStrikePrice;
    }

} // namespace
