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

#include <mamda/MamdaOrderBook.h>
using namespace Wombat;

const int N = 100;
const int M = 1000;
const char* symbol = "MSFT";

int main (int argc, const char** argv)
{
    MamdaOrderBook  book;
    book.setSymbol (symbol);

    // Populate an order book with N price levels ranging from 50.00
    // to 150.00, with M entries in each level.
    char uniqueId[32];
    for (int level = 0; level < N; level++)
    {
        MamdaOrderBookTypes::Side side = (level < N/2) ? 
            MamdaOrderBookTypes::MAMDA_BOOK_SIDE_BID :
            MamdaOrderBookTypes::MAMDA_BOOK_SIDE_ASK;
        double price = 50.0 + (100.0 * level/(double)N);

        for (int entry = 0; entry < M; entry++)
        {
            mama_u64_t   size = 10 * entry;
            MamaDateTime now;
            now.setToNow();
            snprintf(uniqueId, 32, "%s%.4g%c", symbol, price, (char)side);
//            book.addUniqueEntry (uniqueId, price, size, side, now);
        }
    }

    return 0;
}

void usage (int)
{
}
