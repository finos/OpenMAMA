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

#include <mamda/MamdaOrderImbalanceSide.h>

using namespace Wombat;

const char* MamdaOrderImbalanceSide::valueToString (const int value)
{
    switch (value)
    {
        case ASK_SIDE_IMBALANCE_VALUE:  return ASK_SIDE_IMBALANCE;
        case BUY_SIDE_IMBALANCE_VALUE:  return BUY_SIDE_IMBALANCE;
        case NO_IMBALANCE_SIDE_VALUE:   return NO_IMBALANCE_SIDE;
        default:                        return "";
    }
}
