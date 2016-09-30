/* $Id: MamdaTradeCancelOrError.cs,v 1.4.2.5 2012/08/24 16:12:11 clintonmcdowell Exp $
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
	/// MamdaTradeCancelOrError is an interface that provides access to
	/// fields related to trade cancellations and errors.
	/// </summary>
	public interface MamdaTradeCancelOrError : MamdaBasicEvent
	{
		/// <summary>
		/// Return whether this event is a trade cancel.  If false, the
		/// event is a trade error.
		/// </summary>
		/// <returns></returns>
		bool getIsCancel();

        /// <summary>
        /// Original Trade Id for cancel/error.
        /// </summary>
        /// <returns></returns>
        string getOrigTradeId();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOrigTradeIdFieldState();

		/// <summary>
		/// Original feed-generated sequence for a correction/cancel/error.
		/// </summary>
		/// <returns></returns>
		long getOrigSeqNum();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOrigSeqNumFieldState();

		/// <summary>
		/// Original trade price in a correction/cancel/error.
		/// </summary>
		/// <returns></returns>
		double getOrigPrice();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOrigPriceFieldState();

		/// <summary>
		/// Original trade size in a correction/cancel/error.
		/// </summary>
		/// <returns></returns>
		long getOrigVolume();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOrigVolumeFieldState();

		/// <summary>
		/// Original trade participant identifier in a
		/// correction/cancel/error.
		/// </summary>
		/// <returns></returns>
		string getOrigPartId();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOrigPartIdFieldState();

		/// <summary>
		/// A normalized set of qualifiers for the original trade for the
		/// security in a correction/cancel/error.
		/// </summary>
		/// <returns></returns>
		string getOrigQual();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOrigQualFieldState();

		/// <summary>
		/// Get original trade qualifier (non normalized).
		/// <see>MamdaTradeReport.getTradeQual()</see>
		/// </summary>
		/// <returns></returns>
		string getOrigQualNative();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOrigQualNativeFieldState();

		/// <summary>
		/// Feed-specific trade qualifier code(s) for original trade
		/// </summary>
		/// <returns></returns>
		string getOrigCondition();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOrigConditionFieldState();

		/// <summary>
		/// Seller's sale days for original trade. Used when the trade
		/// qualifier is "Seller". Specifies the number of days that may
		/// elapse before delivery of the security.
		/// </summary>
		/// <returns></returns>
		long getOrigSellersSaleDays();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOrigSellersSaleDaysFieldState();

		/// <summary>
		/// Stopped stock indicator for original trade.
		/// Condition related to certain NYSE trading rules.
		/// This is not related to a halted security
		/// status.  (0 == N/A; 1 == Applicable)
		/// </summary>
		/// <returns></returns>
		char getOrigStopStock();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOrigStopStockFieldState();

		/// <summary>
		/// Returns the ShortSaleCircuitBreaker
		/// </summary>
		/// <returns></returns>
        char getOrigShortSaleCircuitBreaker();

		/// <summary>
		/// Returns the FieldState, always MODIFIED
		/// </summary>
		/// <returns></returns>
        MamdaFieldState getOrigShortSaleCircuitBreakerFieldState();
	}
}
