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

#ifndef MamdaOptionContractSetH
#define MamdaOptionContractSetH

#include <mamda/MamdaOptionalConfig.h>
#include <map>
#include <string.h>

namespace Wombat
{

    class MamdaOptionContract;

    struct char_str_less_than
    {
        bool operator()(const char* str1, const char* str2) const
        {
            return strcmp(str1,str2) < 0;
        }
    };

    /**
     * A class that represents a set of option contracts at a given strike
     * price.
     */
    class MAMDAOPTExpDLL MamdaOptionContractSet 
        : public std::map <const char*, MamdaOptionContract*, char_str_less_than>
    {
    public:

        MamdaOptionContractSet();

        ~MamdaOptionContractSet();

        /**
         * Set the contract for the best bid and offer.
         */
        void setBboContract (MamdaOptionContract*  contract);

        /**
         * Set the contract for the best bid and offer, as calculated
         * by NYSE Technologies.
         */
        void setWombatBboContract (MamdaOptionContract*  contract);

        /**
         * Set the contract for the particular exchange.
         */
        void setExchangeContract (const char*           exchange,
                                  MamdaOptionContract*  contract);

        /**
         * Return the contract for the best bid and offer.
         */
        MamdaOptionContract* getBboContract () const;

        /**
         * Return the contract for the best bid and offer, as calculated
         * by NYSE Technologies.
         */
        MamdaOptionContract* getWombatBboContract () const;

        /**
         * Return the contract for the particular exchange.
         */
        MamdaOptionContract* getExchangeContract (const char* exchange) const;

    private:
        MamdaOptionContract*  mBboContract;
        MamdaOptionContract*  mWombatBboContract;
    };

} // namespace

#endif // MamdaOptionContractSetH
