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

#ifndef MamdaOptionExpirationDateSetH
#define MamdaOptionExpirationDateSetH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaOptionAtTheMoneyCompareType.h>
#include <mamda/MamdaOptionExpirationStrikes.h>
#include <map>
#include <mama/mamacpp.h>

namespace Wombat
{

    class MamaDateTime;

    /**
     * A class that represents a set of expiration dates, each of which
     * contains a set of strike prices, each of which contains a set of
     * option contracts, each of which contains exchange-specific
     * contracts.  To access a set of strike prices for a given expiration
     * date, use the find method (inherited from std::map).
     */
    class MAMDAOPTExpDLL MamdaOptionExpirationDateSet
        : public std::map <MamaDateTime, MamdaOptionExpirationStrikes*>
    {

    public:
        MamdaOptionExpirationDateSet ();
        MamdaOptionExpirationDateSet (const MamdaOptionExpirationDateSet&  copy);

        ~MamdaOptionExpirationDateSet ();

        MamdaOptionExpirationDateSet& operator= (
            const MamdaOptionExpirationDateSet&  rhs);

        void getExpirationsBefore (
            MamdaOptionExpirationDateSet&  result,
            const MamaDateTime&            date) const;

        void getExpirations (
            MamdaOptionExpirationDateSet&  result,
            int                            numExpirations) const;

        const char* toString() const;

    private:
        void copyStrikes (const MamdaOptionExpirationDateSet&  rhs);
    };

} // namespace

#endif // MamdaOptionExpirationDateSetH
