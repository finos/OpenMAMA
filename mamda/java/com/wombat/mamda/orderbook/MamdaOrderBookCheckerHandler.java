/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2012 NYSE Technologies, Inc.
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

package com.wombat.mamda.orderbook;

import com.wombat.mamda.*;
import com.wombat.mama.MamaMsg;

/**
 * MamdaOrderBookCheckerHandler is an interface for applications 
 * that want to handle the results of the MamdaOrderBookChecker. Callback
 * interfaces are provided for correct and erroneous checks. 
 */

public interface MamdaOrderBookCheckerHandler
{
    /**
     * Method invoked when a successful check is completed.
     *
     * @param checkType     Types of MamdaOrderBookChecks.
     */
    void onSuccess (
        MamdaOrderBookCheckType  checkType,
        final MamdaOrderBook     realTimeBook);
    
    /**
     * Method invoked when check is completed inconclusively.  An
     * attempt to check the order book may be inconclusive if the
     * order book sequence numbers do not match up.
     *
     * @param checkType     Types of MamdaOrderBookChecks.
     * @param reason        Reason for the sequence numbers mismatch.
     */
    void onInconclusive (
        MamdaOrderBookCheckType  checkType,
        final String             reason);
    
    /**
     * Method invoked when a failed check is completed.  The message
     * provided, if non-NULL, is the one received for the snapshot or
     * delta, depending upon the value of checkType.
     *
     * @param checkType     Types of MamdaOrderBookChecks.
     * @param reason        Reason for the failure.
     * @param msg           The MamaMsg which caused failure.
     * @param realTimeBook  The Real Time Order Book which is compared against.
     * @param checkBook     The Aggregated Book which is compared.
     */
    void onFailure (
        MamdaOrderBookCheckType  checkType,
        final String             reason,
        final MamaMsg            msg,
        final MamdaOrderBook     realTimeBook,
        final MamdaOrderBook     checkBook);
}


