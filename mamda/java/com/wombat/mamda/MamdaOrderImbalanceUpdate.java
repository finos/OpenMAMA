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

package com.wombat.mamda;

import com.wombat.mama.*;

/**
 * MamdaOrderImbalanceUpdate is an interface that provides access
 * to order imbalance  related fields.
 */

public interface MamdaOrderImbalanceUpdate extends MamdaBasicEvent
{
    /**
     * @return Returns the ActivityTime.
     */
    public MamaDateTime getActivityTime();

    /**
     * @return Returns the ActivityTime Field State.
     */
    public short getActivityTimeFieldState();

    /**
     * @return Returns the AuctionTime.
     */
    public MamaDateTime getAuctionTime();

    /**
     * @return Returns the AuctionTime Field State.
     */
    public short getAuctionTimeFieldState();

    /**
     * @return Returns the BuyVolume.
     */
    public long getBuyVolume();

    /**
     * @return Returns the BuyVolume Field State.
     */
    public short getBuyVolumeFieldState();

    /**
     * @return Returns the CrossType.
     */
    public char getCrossType(); 

    /**
     * @return Returns the CrossType Field State.
     */
    public short getCrossTypeFieldState();

    /**
     * @return Returns the EventSeqNum.
     */
    public long getEventSeqNum(); 

    /**
     * @return Returns the EventSeqNum Field State.
     */
    public short getEventSeqNumFieldState();

    /**
     * @return Returns the EventTime.
     */
    public MamaDateTime getEventTime(); 

    /**
     * @return Returns the EventTime Field State.
     */
    public short getEventTimeFieldState();

    /**
     * @return Returns the FarClearingPrice.
     */
    public MamaPrice getFarClearingPrice(); 

    /**
     * @return Returns the FarClearingPrice Field State.
     */
    public short getFarClearingPriceFieldState();

    /**
     * @return Returns the HighIndicationPrice.
     */
    public MamaPrice getHighIndicationPrice(); 

    /**
     * @return Returns the HighIndicationPrice Field State.
     */
    public short getHighIndicationPriceFieldState();

    /**
     * @return Returns the IndicationPrice.
     */
    public MamaPrice getImbalancePrice ();

    /**
     * @return Returns the ImbalancePrice Field State.
     */
    public short getImbalancePriceFieldState();

    /**
     * @return Returns the InsideMatchPrice.
     */
    public MamaPrice getMatchPrice(); 

    /**
     * @return Returns the MatchPrice Field State.
     */
    public short getMatchPriceFieldState();

    /**
     * @return Returns the IssueSymbol.
     */
    public String getIssueSymbol(); 

    /**
     * @return Returns the IssueSymbol Field State.
     */
    public short getIssueSymbolFieldState();

    /**
     * @return Returns the LowIndicationPrice.
     */
    public MamaPrice getLowIndicationPrice(); 

    /**
     * @return Returns the LowIndicationPrice Field State.
     */
    public short getLowIndicationPriceFieldState();

    /**
     * @return Returns the MatchVolume.
     */
    public long getMatchVolume(); 

    /**
     * @return Returns the MatchVolume Field State.
     */
    public short getMatchVolumeFieldState();

    /**
     * @return Returns the MsgType.
     */
    public int getMsgType(); 

    /**
     * @return Returns the MsgType Field State.
     */
    public short getMsgTypeFieldState();

    /**
     * @return Returns the NearClearingPrice.
     */
    public MamaPrice getNearClearingPrice(); 

    /**
     * @return Returns the NearClearingPrice Field State.
     */
    public short getNearClearingPriceFieldState();

    /**
     * @return Returns the NoClearingPrice.
     */
    public char getNoClearingPrice(); 

    /**
     * @return Returns the NoClearingPrice Field State.
     */
    public short getNoClearingPriceFieldState();

    /**
     * @return Returns the PartId.
     */
    public String getPartId(); 

    /**
     * @return Returns the PartId Field State.
     */
    public short getPartIdFieldState();

    /**
     * @return Returns the PriceVarInd.
     */
    public char getPriceVarInd();

    /**
     * @return Returns the PriceVarInd Field State.
     */
    public short getPriceVarIndFieldState();

    /**
     * @return Returns the SecurityStatusOrig.
     */
    public String getSecurityStatusOrig(); 

    /**
     * @return Returns the SecurityStatusOrig Field State.
     */
    public short getSecurityStatusOrigFieldState();

    /**
     * @return Returns the SecurityStatusQual.
     */
    public String getImbalanceState();

    /**
     * @return Returns the ImbalanceState Field State.
     */
    public short getImbalanceStateFieldState();

    /**
     * @return Returns the SecurityStatusTime.
     */
    public MamaDateTime getSecurityStatusTime(); 

    /**
     * @return Returns the SecurityStatusTime Field State.
     */
    public short getSecurityStatusTimeFieldState();

    /**
     * @return Returns the SellVolume.
     */
    public long getSellVolume(); 

    /**
     * @return Returns the SellVolume Field State.
     */
    public short getSellVolumeFieldState();

    /**
     * @return Returns the SeqNum.
     */
    public int getSeqNum(); 

    /**
     * @return Returns the SeqNum Field State.
     */
    public short getSeqNumFieldState();

}
