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

#ifndef MamdaOptionChainViewRangeHandlerH
#define MamdaOptionChainViewRangeHandlerH

#include <mamda/MamdaOptionalConfig.h>

namespace Wombat
{

    class MamdaOptionChainView;

    /**
     * Class to handle change in a MamdaOptionChainView range.
     */
    class MAMDAOPTExpDLL MamdaOptionChainViewRangeHandler
    {
        /**
         * Action to take when the strike price range is reset to a new
         * range.
         *
         * @param view          The option chain view in which the range changed.
         * @param lowStrike     The low strike price in the range.
         * @param highStrike    The high strike price in the range.
         */
        void onOptionViewStrikeRangeReset (
            MamdaOptionChainView  view,
            double                lowStrike,
            double                highStrike) = 0;
            
        virtual ~MamdaOptionChainViewRangeHandler() {};
    };

} // namespace

#endif // MamdaOptionChainViewRangeHandlerH
