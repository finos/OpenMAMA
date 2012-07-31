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

#ifndef MamdaQuoteCheckerH
#define MamdaQuoteCheckerH

#include <mamda/MamdaConfig.h>
#include <mama/mamacpp.h>
#include <mamda/MamdaCheckerHandler.h>

namespace Wombat
{

    /**
     * MamdaTradeChecker is a class that provides trades sanity
     * checking by periodically requesting snapshots of the trades
     * from the publisher and comparing that with an trades being
     * maintained in real time.  This class is purely for testing
     * purposes, to test for possible configuration or programming 
     * errors in the trades publisher and in MAMDA trades management
     * code.
     *
     * The developer registers a handler that contains callbacks for
     * successful, inconclusive and failure events.  The developer also
     * provides an interval representing the frequency of the snapshot
     * checks.  The first check will take place at some random point in
     * time between zero and the interval.
     */
     
     class MAMDAExpDLL MamdaTradeChecker
     {
     public:

        /**
         * Constructor
         */
        MamdaTradeChecker(MamaQueue*            queue,
                          MamdaCheckerHandler*  handler,
                          MamaSource*           source,
                          const char*           symbol,
                          mama_f64_t            intervalSeconds);    

        /**
         * Desctructor.
         */
        ~MamdaTradeChecker();
        
        /**
         * Perform an ad hoc snapshot check now.  This may be useful if
         * the checking is to be performed by some external trigger event.
         */
        void checkSnapShotNow();
        
        /**
         * @return The number of successful checks.
         */
        mama_u32_t getSuccessCount() const;
        
        /**
         * Return the nmber of inconclusive counts.  An attempt to check
         * the order book may be inconclusive if the order book sequence
         * numbers do not match up.
         *
         * @return The number of inconclusive checks.
         */
        mama_u32_t getInconclusiveCount() const;

        /**
         * Return the number of failed checks.  This should be zero, of course.
         *
         * @return The number of failed checks.
         */
        mama_u32_t getFailureCount() const;
        
        struct MamdaTradeCheckerImpl;
        
    private:
        MamdaTradeCheckerImpl& mImpl;
        
     };
 
} //namespace

#endif // MamdaQuoteCheckerH  
