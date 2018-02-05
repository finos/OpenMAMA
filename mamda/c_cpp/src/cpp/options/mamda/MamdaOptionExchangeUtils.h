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

#ifndef MamdaOptionExchangeUtilsH
#define MamdaOptionExchangeUtilsH

#include <mamda/MamdaOptionalConfig.h>

namespace Wombat
{

    /**
     * A class with static utility functions for dealing with exchanges.
     */
    class MAMDAOPTExpDLL MamdaOptionExchangeUtils
    {
    public:

        /**
         * Return whether the exchange ID is the one used to represent the
         * best bid and offer.  Currently it will match "", "BBO" and
         * "Z". 
         *
         * @param exchange The exchange Id.
         *
         * @return Whether the given exchange id is being used to identify the BBO
         * (Best Bid and Offer) record.
         */
        static bool isBbo (const char* exchange);

        /**
         * Return whether the exchange ID is the one used to represent the
         * NYSE Technologies-calculated best bid and offer.  Currently will match 
         * "BBO",
         *
         * @param exchange The exchange id.
         *
         * @return Whether the given exchange id is being used to identify the
         * WombatBBO (Best Bid and Offer) record.
         */
        static bool isWombatBbo (const char* exchange);

    };

} // namespace

#endif // MamdaOptionExchangeUtilsH
