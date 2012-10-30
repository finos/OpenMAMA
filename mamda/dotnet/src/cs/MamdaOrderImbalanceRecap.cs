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

using System;

namespace Wombat
{
	/// <summary>
	/// MamdaOrderImbalanceRecap is an interface that provides access
	/// to order imbalance  related fields.
	/// </summary>
	public interface MamdaOrderImbalanceRecap : MamdaBasicRecap
	{
		/// <summary>
		/// Returns the AuctionTime.
		/// </summary>
		/// <returns>
		/// Returns the AuctionTime.
		/// </returns>
		DateTime getAuctionTime();

        /// <summary>
        /// Returns the FieldState.
        /// </summary>
        /// <returns>
        /// Returns the FieldState.
        /// </returns>
        MamdaFieldState getAuctionTimeFieldState();

		/// <summary>
		/// Returns the BuyVolume.
		/// </summary>
		/// <returns>
		/// Returns the BuyVolume.
		/// </returns>
		long getBuyVolume(); 

        /// <summary>
        /// Returns the FieldState.
        /// </summary>
        /// <returns>
        /// Returns the FieldState.
        /// </returns>
        MamdaFieldState getBuyVolumeFieldState();

		/// <summary>
		/// Returns the CrossType.
		/// </summary>
		/// <returns>
		/// Returns the CrossType.
		/// </returns>
		char getCrossType(); 

        /// <summary>
        /// Returns the FieldState.
        /// </summary>
        /// <returns>
        /// Returns the FieldState.
        /// </returns>
        MamdaFieldState getCrossTypeFieldState();

		/// <summary>
		/// Returns the EventSeqNum.
		/// </summary>
		/// <returns>
		/// Returns the EventSeqNum.
		/// </returns>
		long getEventSeqNum(); 

        /// <summary>
        /// Returns the FieldState.
        /// </summary>
        /// <returns>
        /// Returns the FieldState.
        /// </returns>
        MamdaFieldState getEventSeqNumFieldState();

		/// <summary>
		/// Returns the EventTime.
		/// </summary>
		/// <returns>
		/// Returns the EventTime.
		/// </returns>
		DateTime getEventTime(); 

        /// <summary>
        /// Returns the FieldState.
        /// </summary>
        /// <returns>
        /// Returns the FieldState.
        /// </returns>
        MamdaFieldState getEventTimeFieldState();

		/// <summary>
		/// Returns the FarClearingPrice.
		/// </summary>
		/// <returns>
		/// Returns the FarClearingPrice.
		/// </returns>
		MamaPrice getFarClearingPrice(); 

        /// <summary>
        /// Returns the FieldState.
        /// </summary>
        /// <returns>
        /// Returns the FieldState.
        /// </returns>
        MamdaFieldState getFarClearingPriceFieldState();

		/// <summary>
		/// Returns the HighIndicationPrice.
		/// </summary>
		/// <returns>
		/// Returns the HighIndicationPrice.
		/// </returns>
		MamaPrice getHighIndicationPrice(); 

        /// <summary>
        /// Returns the FieldState.
        /// </summary>
        /// <returns>
        /// Returns the FieldState.
        /// </returns>
        MamdaFieldState getHighIndicationPriceFieldState();

		/// <summary>
		/// Returns the IndicationPrice.
		/// </summary>
		/// <returns>
		/// Returns the IndicationPrice.
		/// </returns>
		MamaPrice getImbalancePrice ();

        /// <summary>
        /// Returns the FieldState.
        /// </summary>
        /// <returns>
        /// Returns the FieldState.
        /// </returns>
        MamdaFieldState getImbalancePriceFieldState();

		/// <summary>
		/// Returns the InsideMatchPrice.
		/// </summary>
		/// <returns>
		/// Returns the InsideMatchPrice.
		/// </returns>
		MamaPrice getMatchPrice(); 

        /// <summary>
        /// Returns the FieldState.
        /// </summary>
        /// <returns>
        /// Returns the FieldState.
        /// </returns>
        MamdaFieldState getMatchPriceFieldState();

		/// <summary>
		/// Returns the IssueSymbol.
		/// </summary>
		/// <returns>
		/// Returns the IssueSymbol.
		/// </returns>
		string getIssueSymbol(); 

        /// <summary>
        /// Returns the FieldState.
        /// </summary>
        /// <returns>
        /// Returns the FieldState.
        /// </returns>
        MamdaFieldState getIssueSymbolFieldState();

		/// <summary>
		/// Returns the LowIndicationPrice.
		/// </summary>
		/// <returns>
		/// Returns the LowIndicationPrice.
		/// </returns>
		MamaPrice getLowIndicationPrice(); 

        /// <summary>
        /// Returns the FieldState.
        /// </summary>
        /// <returns>
        /// Returns the FieldState.
        /// </returns>
        MamdaFieldState getLowIndicationPriceFieldState();

		/// <summary>
		/// Returns the MatchVolume.
		/// </summary>
		/// <returns>
		/// Returns the MatchVolume.
		/// </returns>
		long getMatchVolume(); 

        /// <summary>
        /// Returns the FieldState.
        /// </summary>
        /// <returns>
        /// Returns the FieldState.
        /// </returns>
        MamdaFieldState getMatchVolumeFieldState();

		/// <summary>
		/// Returns the MsgType.
		/// </summary>
		/// <returns>
		/// Returns the MsgType.
		/// </returns>
		int getMsgType(); 

        /// <summary>
        /// Returns the FieldState.
        /// </summary>
        /// <returns>
        /// Returns the FieldState.
        /// </returns>
        MamdaFieldState getMsgTypeFieldState();

		/// <summary>
		/// Returns the NearClearingPrice.
		/// </summary>
		/// <returns>
		/// Returns the NearClearingPrice.
		/// </returns>
		MamaPrice getNearClearingPrice(); 

        /// <summary>
        /// Returns the FieldState.
        /// </summary>
        /// <returns>
        /// Returns the FieldState.
        /// </returns>
        MamdaFieldState getNearClearingPriceFieldState();

		/// <summary>
		/// Returns the NoClearingPrice.
		/// </summary>
		/// <returns>
		/// Returns the NoClearingPrice.
		/// </returns>
		char getNoClearingPrice();  

        /// <summary>
        /// Returns the FieldState.
        /// </summary>
        /// <returns>
        /// Returns the FieldState.
        /// </returns>
        MamdaFieldState getNoClearingPriceFieldState();

		/// <summary>
		/// Returns the PriceVarInd.
		/// </summary>
		/// <returns>
		/// Returns the PriceVarInd.
		/// </returns>
		char getPriceVarInd();

        /// <summary>
        /// Returns the FieldState.
        /// </summary>
        /// <returns>
        /// Returns the FieldState.
        /// </returns>
        MamdaFieldState getPriceVarIndFieldState();

		/// <summary>
		/// Returns the SecurityStatusOrig.
		/// </summary>
		/// <returns>
		/// Returns the SecurityStatusOrig.
		/// </returns>
		string getSecurityStatusOrig(); 

        /// <summary>
        /// Returns the FieldState.
        /// </summary>
        /// <returns>
        /// Returns the FieldState.
        /// </returns>
        MamdaFieldState getSecurityStatusOrigFieldState();

		/// <summary>
		/// Returns the SecurityStatusQual.
		/// </summary>
		/// <returns>
		/// Returns the SecurityStatusQual.
		/// </returns>
		string getImbalanceState();

        /// <summary>
        /// Returns the FieldState.
        /// </summary>
        /// <returns>
        /// Returns the FieldState.
        /// </returns>
        MamdaFieldState getImbalanceStateFieldState();

		/// <summary>
		/// Returns the SecurityStatusTime.
		/// </summary>
		/// <returns>
		/// Returns the SecurityStatusTime.
		/// </returns>
		DateTime getSecurityStatusTime(); 

        /// <summary>
        /// Returns the FieldState.
        /// </summary>
        /// <returns>
        /// Returns the FieldState.
        /// </returns>
        MamdaFieldState getSecurityStatusTimeFieldState();

		/// <summary>
		/// Returns the SellVolume.
		/// </summary>
		/// <returns>
		/// Returns the SellVolume.
		/// </returns>
		long getSellVolume(); 

        /// <summary>
        /// Returns the FieldState.
        /// </summary>
        /// <returns>
        /// Returns the FieldState.
        /// </returns>
        MamdaFieldState getSellVolumeFieldState();

		/// <summary>
		/// Returns the SeqNum.
		/// </summary>
		/// <returns>
		/// Returns the SeqNum.
		/// </returns>
		int getSeqNum();

        /// <summary>
        /// Returns the FieldState.
        /// </summary>
        /// <returns>
        /// Returns the FieldState.
        /// </returns>
        MamdaFieldState getSeqNumFieldState();

	}
}
