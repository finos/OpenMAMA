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

#ifndef MamdaOptionStrikeSetH
#define MamdaOptionStrikeSetH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaOptionContractSet.h>

namespace Wombat
{

    class MamaDateTime;

    /**
     * A class that represents the call and put contract sets at a given
     * strike price.
     */
    class MAMDAOPTExpDLL MamdaOptionStrikeSet
    {
    public:

        MamdaOptionStrikeSet (const MamaDateTime&  expireDate,
                              double               strikePrice);

        ~MamdaOptionStrikeSet ();

        /**
         * Return a set of call contracts at the given strike price.
         *
         * @return All call contracts at the given strike price.
         */
        MamdaOptionContractSet*        getCallSet();

        /**
         * Return a set of call contracts at the given strike price.
         *
         * @return All call contracts at the given strike price. (const pointer)
         */
        const MamdaOptionContractSet*  getCallSet() const;

        /**
         * Return a set of put contracts at the given strike price.
         *
         * @return All put contracts at the given strike price.
         */
        MamdaOptionContractSet*        getPutSet();

        /**
         * Return a set of put contracts at the given strike price.
         *
         * @return All put contracts at the given strike price. (const pointer)
         */
        const MamdaOptionContractSet*  getPutSet() const;

        /**
         * Return the expiration date for the contracts at this strike
         * price.
         *
         * @return The expiration date for the contracts at this strike price.
         */
        const MamaDateTime& getExpireDate    () const;

        /**
         * Return the expiration date for the contracts at this strike
         * price as a string.
         *
         * @return The expiration date for the contracts at this strike price.
         */
        const char*         getExpireDateStr () const;

        /**
         * Return the strike price.
         *
         * @return The strike price represented by this object.
         */
        double              getStrikePrice   () const;

    private:
        struct MamdaOptionStrikeSetImpl;
        MamdaOptionStrikeSetImpl& mImpl;
    };

} // namespace

#endif // MamdaOptionStrikeSetH
