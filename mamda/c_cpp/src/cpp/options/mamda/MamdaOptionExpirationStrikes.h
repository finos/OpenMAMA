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

#ifndef MamdaOptionExpirationStrikesH
#define MamdaOptionExpirationStrikesH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaOptionAtTheMoneyCompareType.h>
#include <mamda/MamdaOptionStrikeSet.h>
#include <mamda/MamdaOptionChain.h>
#include <map>

namespace Wombat
{

    class MamaDateTime;

    /**
     * A class that represents a set of strike prices at a particular
     * expiration date.  Each strike price of which contains a set of
     * option contracts, each of which contains exchange-specific
     * contracts.  To access a contract set for a given strike price, use
     * the get method (inherited from TreeMap).
     */
    class MAMDAOPTExpDLL MamdaOptionExpirationStrikes 
        : public std::map <double, MamdaOptionStrikeSet*>
    {

    public:
        MamdaOptionExpirationStrikes ();
        MamdaOptionExpirationStrikes (const MamdaOptionExpirationStrikes&  copy);

        ~MamdaOptionExpirationStrikes ();

        /**
         * Trim the current set of strike prices to the given set.
         *
         * @param strikeSet The set of strikes to trim the current set to.
         */
        void trimStrikes (const StrikeSet&  strikeSet);

    };

} // namespace

#endif // MamdaOptionExpirationStrikesH
