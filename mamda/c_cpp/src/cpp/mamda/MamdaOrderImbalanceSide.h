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

#ifndef MamdaOrderImbalanceSideH
#define MamdaOrderImbalanceSideH

#include <mamda/MamdaConfig.h>
#include <mama/mamacpp.h>

#define ASK_SIDE_IMBALANCE   "ASK"
#define BUY_SIDE_IMBALANCE   "BID"
#define NO_IMBALANCE_SIDE    "NONE"

namespace Wombat
{ 

    class MAMDAExpDLL MamdaOrderImbalanceSide
    {
    public:
        MamdaOrderImbalanceSide () {};
        ~MamdaOrderImbalanceSide ();

        typedef enum OrderImbalanceSide_
        {
            /**ASK_SIDE_IMBALANCE*/
            ASK_SIDE_IMBALANCE_VALUE = 0,

            /**BID_SIDE_IMBALANCE*/
            BUY_SIDE_IMBALANCE_VALUE,

            /**NO_IMBALANCE*/
            NO_IMBALANCE_SIDE_VALUE

        } OrderImbalanceSide;
        
       const char* valueToString (const int value);

    };//class

} // namespace
#endif
