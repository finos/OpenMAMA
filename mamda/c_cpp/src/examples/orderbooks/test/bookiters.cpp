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
#include <mamda/MamdaOrderBookEntry.h>
#include <assert.h>

using namespace Wombat;

const int N = 200;
const int M = 1000;
const char* symbol = "MSFT";

int main (int argc, const char** argv)
{
    MamaDateTime now;
    MamdaOrderBook  book;
    book.setSymbol (symbol);

    now.setToNow();
    mama_u64_t  beginMicrosecs = now.getEpochTimeMicroseconds();
    printf("creating book with %d price levels and %d entries per level\n",
           N, M);

    // Populate an order book with N price levels ranging from 50.00
    // to 150.00, with M entries in each level.
    char uniqueId[32];
    for (int level = 0; level < N; level++)
    {
        MamdaOrderBookPriceLevel::Side side = (level < N/2) ? 
            MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID : 
            MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK;
        double price = 50.0 + (100.0 * level/(double)N);

        for (int entry = 0; entry < M; entry++)
        {
            mama_quantity_t  size = 10 * entry;
            now.setToNow();
            snprintf (uniqueId, 32, "%s%.4g%c%g",
                      symbol, price, (char)side, size);
            book.addEntry (uniqueId, size, price, side, now, NULL, NULL);
        }
    }

    now.setToNow();
    mama_u64_t  endMicrosecs = now.getEpochTimeMicroseconds();
    printf("created  book with %d price levels and %d entries per level in %llu microsecs\n",
           N, M, endMicrosecs-beginMicrosecs);
    now.setToNow();

    int levelDepth = (40*N)/100;  /* 80% deep */
    int entryDepth = (80*M)/100;  /* 80% deep */
    int levelTimes = 100000;
    int entryTimes = 1000;
    int searchTimes = 100000;
    mama_quantity_t dummy = 0.0;

    beginMicrosecs = now.getEpochTimeMicroseconds();
    for (int i = 0; i < levelTimes; i++)
    {
        MamdaOrderBook::constBidIterator bidEnd  = book.bidEnd();
        MamdaOrderBook::constBidIterator bidIter = book.bidBegin();
        for (int j = 0;
             (bidIter != bidEnd) && (j < levelDepth);
             ++bidIter, ++j)
        {
            const MamdaOrderBookPriceLevel* bidLevel = *bidIter;
            dummy += bidLevel->getSize();
        }
    }
    now.setToNow();
    endMicrosecs = now.getEpochTimeMicroseconds();
//  printf ("dummy: %llu\n", dummy);
    printf("iterated to the %dth bid price level %d times in %llu microsecs\n",
           levelDepth, levelTimes, endMicrosecs-beginMicrosecs);

    beginMicrosecs = now.getEpochTimeMicroseconds();
    for (int i = 0; i < levelTimes; i++)
    {
        MamdaOrderBook::constAskIterator askEnd  = book.askEnd();
        MamdaOrderBook::constAskIterator askIter = book.askBegin();
        for (int j = 0;
             (askIter != askEnd) && (j < levelDepth);
             ++askIter, ++j)
        {
            const MamdaOrderBookPriceLevel* askLevel = *askIter;
            dummy += askLevel->getSize();
        }
    }
    now.setToNow();
    endMicrosecs = now.getEpochTimeMicroseconds();
//  printf ("dummy: %llu\n", dummy);
    printf("iterated to the %dth ask price level %d times in %llu microsecs\n",
           levelDepth, levelTimes, endMicrosecs-beginMicrosecs);

    const MamdaOrderBookPriceLevel*  targetLevel = NULL;
    const MamdaOrderBookEntry*       targetEntry = NULL;
    beginMicrosecs = now.getEpochTimeMicroseconds();
    for (int i = 0; i < entryTimes; i++)
    {
        MamdaOrderBook::constBidIterator bidEnd  = book.bidEnd();
        MamdaOrderBook::constBidIterator bidIter = book.bidBegin();
        for (int j = 0;
             (bidIter != bidEnd) && (j < levelDepth);
             ++bidIter, ++j)
        {
            targetLevel = *bidIter;
            MamdaOrderBookPriceLevel::const_iterator entEnd  = targetLevel->end();
            MamdaOrderBookPriceLevel::const_iterator entIter = targetLevel->begin();
            for (int k = 0;
                 (entIter != entEnd) && (k < entryDepth);
                 ++entIter, ++k)
            {
                targetEntry = *entIter;
                dummy += targetEntry->getSize();
            }
        }
    }
    now.setToNow();
    endMicrosecs = now.getEpochTimeMicroseconds();
//  printf ("dummy: %llu\n", dummy);
    printf("iterated to the %dth entry of each of %d bid price levels %d times in %llu microsecs\n",
           entryDepth, levelDepth, entryTimes, endMicrosecs-beginMicrosecs);

    /* We can now use targetLevel & targetEntry for the next tests: */
    beginMicrosecs = now.getEpochTimeMicroseconds();
    for (int i = 0; i < searchTimes; i++)
    {
        const MamdaOrderBookPriceLevel* foundLevel = 
            book.getLevelAtPrice (targetLevel->getPrice(),
                                  targetLevel->getSide());
        assert (foundLevel == targetLevel);
    }
    now.setToNow();
    endMicrosecs = now.getEpochTimeMicroseconds();
    printf("searched for the %dth bid price level (%g) %d times in %llu microsecs\n",
           levelDepth, targetLevel->getPrice(), searchTimes,
           endMicrosecs-beginMicrosecs);

    beginMicrosecs = now.getEpochTimeMicroseconds();
    for (int i = 0; i < searchTimes; i++)
    {
        const MamdaOrderBookEntry*  foundEntry =
            targetLevel->findEntry (targetEntry->getId());
        assert (foundEntry == targetEntry);
    }
    now.setToNow();
    endMicrosecs = now.getEpochTimeMicroseconds();
    printf("searched for the ID (%s) of the %dth entry in a price level %d times in %llu microsecs\n",
           targetEntry->getId(), entryDepth, searchTimes,
           endMicrosecs-beginMicrosecs);


    return 0;
}

void usage (int)
{
}
