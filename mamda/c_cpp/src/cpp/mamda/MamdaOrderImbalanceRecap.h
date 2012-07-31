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

#ifndef MamdaOrderImbalanceRecapH
#define MamdaOrderImbalanceRecapH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaBasicRecap.h>
#include <mamda/MamdaFieldState.h>
#include <mama/mamacpp.h>
#include <stdio.h>
#include <string.h>

namespace Wombat
{

    class MAMDAExpDLL MamdaOrderImbalanceRecap : MamdaBasicRecap
    {
    public:
            
        /**
         * @return Returns the Auction Time.
         */
        virtual const  MamaDateTime& getAuctionTime() const = 0;

        /**
         * Get the myAuctionTime field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getAuctionTimeFieldState() const = 0;

        /**
         * @return Returns the myBuyVolume.
         */
        virtual int64_t getBuyVolume() const = 0; 

        /**
         * Get the myBuyVolume field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getBuyVolumeFieldState() const = 0;

       /**
        * @return Returns the myCrossType.
        */
        virtual  char getCrossType() const = 0; 

        /**
         * Get the myCrossType field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getCrossTypeFieldState() const = 0;

        /**
         * @return Returns the myEventSeqNum.
         */
        virtual mama_seqnum_t getEventSeqNum() const = 0 ; 

        /**
         * Get the myEventSeqNum field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getEventSeqNumFieldState() const = 0;

        /**
         * @return Returns the myEventTime.
         */
        virtual const MamaDateTime& getEventTime() const = 0; 

        /**
         * Get the myEventTime field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getEventTimeFieldState() const = 0;

        /**
         * @return Returns the myFarClearingPrice.
         */
        virtual const MamaPrice& getFarClearingPrice() const = 0; 

        /**
         * Get the myFarClearingPrice field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getFarClearingPriceFieldState() const = 0;

        /**
         * @return Returns the myHighIndicationPrice.
         */
        virtual const MamaPrice& getHighIndicationPrice() const = 0; 

        /**
         * Get the myHighIndicationPrice field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getHighIndicationPriceFieldState() const = 0;

        /**
         * @return Returns the myIndicationPrice.
         */
        virtual const MamaPrice& getImbalancePrice () const = 0;

        /**
         * Get the myIndicationPrice field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getImbalancePriceFieldState() const = 0;

        /**
         * @return Returns the myInsideMatchPrice.
         */
        virtual const MamaPrice& getMatchPrice() const = 0; 

        /**
         * Get the myInsideMatchPrice field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getMatchPriceFieldState() const = 0;

        /**
         * @return Returns the myIssueSymbol.
         */
        virtual const char* getIssueSymbol() const = 0; 

        /**
         * Get the myIssueSymbol field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getIssueSymbolFieldState() const = 0;

        /**
         * @return Returns the myLowIndicationPrice.
         */
        virtual const MamaPrice& getLowIndicationPrice() const = 0; 

        /**
         * Get the myLowIndicationPrice field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getLowIndicationPriceFieldState() const = 0;

        /**
         * @return Returns the myMatchVolume.
         */
        virtual int64_t getMatchVolume() const = 0; 

        /**
         * Get the myMatchVolume field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getMatchVolumeFieldState() const = 0;

        /**
         * @return Returns the myMsgType.
         */
        virtual int32_t getMsgType() const = 0; 

        /**
         * Get the myMsgType field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getMsgTypeFieldState() const = 0;

        /**
         * @return Returns the myNearClearingPrice.
         */
        virtual const MamaPrice& getNearClearingPrice() const = 0; 

        /**
         * Get the myNearClearingPrice field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getNearClearingPriceFieldState() const = 0;

        /**
         * @return Returns the myNoClearingPrice.
         */
        virtual char getNoClearingPrice() const = 0; 

        /**
         * Get the myNoClearingPrice field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getNoClearingPriceFieldState() const = 0;

        /**
         * @return Returns the myPartId.
         */
        virtual const char* getPartId() const = 0; 

        /**
         * Get the myPartId field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getPartIdFieldState() const = 0;

        /**
         * @return Returns the myPriceVarInd.
         */
        virtual char getPriceVarInd() const = 0;

        /**
         * Get the myPriceVarInd field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getPriceVarIndFieldState() const = 0;

        /**
         * @return Returns the mySecurityStatusOrig.
         */
        virtual const char* getSecurityStatusOrig() const = 0; 

        /**
         * Get the mySecurityStatusOrig field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getSecurityStatusOrigFieldState() const = 0;

        /**
         * @return Returns the mySecurityStatusQual.
         */
        virtual const char* getImbalanceState() const = 0;

        /**
         * Get the mySecurityStatusQual field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getImbalanceStateFieldState() const = 0;

        /**
         * @return Returns the mySecurityStatusTime.
         */
        virtual const MamaDateTime& getSecurityStatusTime() const = 0; 

        /**
         * Get the mySecurityStatusTime field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getSecurityStatusTimeFieldState() const = 0;

        /**
         * @return Returns the mySellVolume.
         */
        virtual int64_t getSellVolume() const = 0; 

        /**
         * Get the mySellVolume field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getSellVolumeFieldState() const = 0;

        /**
         * @return Returns the mySeqNum.
         */
        virtual mama_seqnum_t getSeqNum() const = 0; 

        /**
         * Get the mySeqNum field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getSeqNumFieldState() const = 0;

        virtual ~MamdaOrderImbalanceRecap () {};

    };//class 
 
}//namespace
#endif
