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

#include <mamda/MamdaOrderBookBasicDelta.h>
#include <iostream>

namespace Wombat
{

    MamdaOrderBookBasicDelta::MamdaOrderBookBasicDelta(
        const MamdaOrderBookBasicDelta&  copy)
    {
        mPriceLevel  = copy.mPriceLevel;
        mEntry       = copy.mEntry;
        mPlDeltaSize = copy.mPlDeltaSize;
        mPlAction    = copy.mPlAction;
        mEntryAction = copy.mEntryAction;
    }

    void MamdaOrderBookBasicDelta::clear()
    {
        mPriceLevel  = NULL;
        mEntry       = NULL;
        mPlDeltaSize = 0.0;
        mPlAction    = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UNKNOWN;
        mEntryAction = MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UNKNOWN;
    }

    void MamdaOrderBookBasicDelta::set (
        MamdaOrderBookEntry*              entry,
        MamdaOrderBookPriceLevel*         level,
        mama_quantity_t                   plDeltaSize,
        MamdaOrderBookPriceLevel::Action  plAction,
        MamdaOrderBookEntry::Action       entryAction)
    {
        mPriceLevel  = level;
        mEntry       = entry;
        if (plDeltaSize) mPlDeltaSize = plDeltaSize;
            else mPlDeltaSize = 0.0;
        mPlAction    = plAction;
        mEntryAction = entryAction;
    }

    const MamdaOrderBook* MamdaOrderBookBasicDelta::getOrderBook () const
    {
        if (mPriceLevel)
            return mPriceLevel->getOrderBook();
        else
            return NULL;
    }

    void MamdaOrderBookBasicDelta::dump (ostream&  output) const
    {
        output << "Price="           << mPriceLevel->getPrice()
               << ", Side="          << (char)mPriceLevel->getSide()
               << ", PlDeltaAction=" << (char)mPlAction
               << ", EntryId="       << (mEntry ? mEntry->getId(): "NULL")
               << ", EntryAction="   << (char)mEntryAction
               << ", PlSize = "      << mPriceLevel->getSize()
               << ", NumEntriesTotal = "  << mPriceLevel->getNumEntriesTotal()
               << ", EntrySize = "   << (mEntry ? mEntry->getSize(): 0)
               << "\n";
    }

} // namespace
