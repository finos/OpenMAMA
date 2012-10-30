/* $Id: MamdaTradeCorrection.cs,v 1.4.2.5 2012/08/24 16:12:11 clintonmcdowell Exp $
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
	/// MamdaTradeCorrection is an interface that provides access to
	/// fields related to trade corrections.
	/// </summary>
	public interface MamdaTradeCorrection : MamdaBasicEvent
	{
		/// <summary>
		/// Get the original trade sequence number.
		/// <see>MamdaBasicEvent.getEventSeqNum()</see>
		/// </summary>
		/// <returns></returns>
		long getOrigSeqNum();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOrigSeqNumFieldState();

		/// <summary>
		/// Get the original trade price.
		/// <see>MamdaTradeReport.getTradePrice()</see>
		/// </summary>
		/// <returns></returns>
		double getOrigPrice();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOrigPriceFieldState();

		/// <summary>
		/// Get the original trade volume.
		/// <see>MamdaTradeReport.getTradeVolume()</see>
		/// </summary>
		/// <returns></returns>
		long getOrigVolume();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOrigVolumeFieldState();

		/// <summary>
		/// Get the original trade participant identifier.
		/// <see>MamdaTradeReport.getTradePartId()</see>
		/// </summary>
		/// <returns></returns>
		string getOrigPartId();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOrigPartIdFieldState();

		/// <summary>
		/// Get original trade qualifier.
		/// <see>MamdaTradeReport.getTradeQual()</see>
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
		/// Get original trade condition.
		/// <see>MamdaTradeReport.getTradeCondition()</see>
		/// </summary>
		/// <returns></returns>
		string getOrigCondition();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOrigConditionFieldState();

		/// <summary>
		/// Get the original trade sellers days.
		/// <see>MamdaTradeReport.getTradeSellersSaleDays()</see>
		/// </summary>
		/// <returns></returns>
		long getOrigSellersSaleDays();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOrigSellersSaleDaysFieldState();

		/// <summary>
		/// Get the original stock stop indicatior.
		/// <see>MamdaTradeReport.getTradeStopStock()</see>
		/// </summary>
		/// <returns></returns>
		char getOrigStopStock();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getOrigStopStockFieldState();

        /// <summary>
        /// Get the corrected trade id.
        /// <see></see>
        /// </summary>
        /// <returns></returns>
        string getCorrTradeId();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getCorrTradeIdFieldState();

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
		/// Get the corrected trade price.
		/// <see>MamdaTradeReport.getTradePrice()</see>
		/// </summary>
		/// <returns></returns>
		double getCorrPrice();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getCorrPriceFieldState();

		/// <summary>
		/// Get the corrected trade volume.
		/// <see>MamdaTradeReport.getTradeVolume()</see>
		/// </summary>
		/// <returns></returns>
		long getCorrVolume();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getCorrVolumeFieldState();

		/// <summary>
		/// Get the corrected trade participant identifier.
		/// <see>MamdaTradeReport.getTradePartId()</see>
		/// </summary>
		/// <returns></returns>
		string getCorrPartId();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getCorrPartIdFieldState();

		/// <summary>
		/// Get corrected trade qualifier.
		/// <see>MamdaTradeReport.getTradeQual()</see>
		/// </summary>
		/// <returns></returns>
		string getCorrQual();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getCorrQualFieldState();

		/// <summary>
		/// Get corrected trade condition.
		/// <see>MamdaTradeReport.getTradeCondition()</see>
		/// </summary>
		/// <returns></returns>
		string getCorrQualNative();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getCorrQualNativeFieldState();

		/// <summary>
		/// Get corrected trade condition.
		/// <see>MamdaTradeReport.getTradeCondition()</see>
		/// </summary>
		/// <returns></returns>
		string getCorrCondition();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getCorrConditionFieldState();

		/// <summary>
		/// Get the corrected trade sellers days.
		/// <see>MamdaTradeReport.getTradeSellersSaleDays()</see>
		/// </summary>
		/// <returns></returns>
		long getCorrSellersSaleDays();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getCorrSellersSaleDaysFieldState();

		/// <summary>
		/// Get the original stock stop indicatior.
		/// <see>MamdaTradeReport.getTradeStopStock()</see>
		/// </summary>
		/// <returns></returns>
		char getCorrStopStock();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getCorrStopStockFieldState();

		/// <summary>
		/// Get the ShortSaleCircuitBreaker.
		/// <see>MamdaTradeReport.getCorrShortSaleCircuitBreaker()
		/// </summary>
		/// <returns></returns>
         char getCorrShortSaleCircuitBreaker();  

		/// <summary>
		/// Get the field state
		/// </summary>
		/// <returns>Field State</returns>
        MamdaFieldState getCorrShortSaleCircuitBreakerFieldState();   

	}
}

