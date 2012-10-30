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
	/// MamdaFundamentals is an interface that provides access to the 
	/// fundamental equity pricing/analysis attributes, indicators and ratios.
	/// </summary>
	public interface MamdaFundamentals
	{
		/// <summary>
		/// Corporate Action Type.
		/// </summary>
		/// <returns></returns>
		string getCorporateActionType();

        /// <summary>
        /// Field State
        /// </summary>
        /// <returns></returns>
        MamdaFieldState getCorporateActionTypeFieldState();
        
		/// <summary>
		/// Dividend price.
		/// </summary>
		/// <returns></returns>
		double getDividendPrice();
        /// <summary>
        /// Field State
        /// </summary>
        /// <returns></returns>
        MamdaFieldState getDividendPriceFieldState();
        
		/// <summary>
		/// Frequency of the dividend payments.
		/// <ul>
		///  <li> : No dividend payments are made.</li>
		///  <li>1M : Monthly</li>
		///  <li>3M : Quarterly</li>
		///  <li>6M : Semi-Annually</li>
		///  <li>1Y : Annually</li>
		///  <li>SP : Payment frequency is special.</li>
		///  <li>NA : Irregular payment frequency.</li>
		///  <li>ER : Invalid dividend frequency period.</li>
		/// </ul>
		/// </summary>
		/// <returns></returns>
		string getDividendFrequency();
        /// <summary>
        /// Field State
        /// </summary>
        /// <returns></returns>
        MamdaFieldState getDividendFrequencyFieldState();
        
		/// <summary>
		/// The date on or after which a security is traded without a
		/// previously declared dividend or distribution.
		/// </summary>
		/// <returns></returns>
		string getDividendExDate();

        /// <summary>
        /// Field State
        /// </summary>
        /// <returns></returns>
        MamdaFieldState getDividendExDateFieldState();
        
		/// <summary>
		/// Date on which corporate action distribution will be paid or
		/// effective.
		/// </summary>
		/// <returns></returns>
		string getDividendPayDate();

        /// <summary>
        /// Field State
        /// </summary>
        /// <returns></returns>
        MamdaFieldState getDividendPayDateFieldState();
        
		/// <summary>
		/// This is the date on which all shareholders are considered a
		/// "holder of record" and ensured the right of a dividend or distribution.
		/// </summary>
		/// <returns></returns>
		string getDividendRecordDate();

        /// <summary>
        /// Field State
        /// </summary>
        /// <returns></returns>
        MamdaFieldState getDividendRecordDateFieldState();
        
		/// <summary>
		/// Currency of dividend.
		/// </summary>
		/// <returns></returns>
		string getDividendCurrency();

        /// <summary>
        /// Field State
        /// </summary>
        /// <returns></returns>
        MamdaFieldState getDividendCurrencyFieldState();
        
		/// <summary>
		/// Shares outstanding. The number of authorized shares in a company
		/// that are held by investors, including employees and
		/// executives of that company. (Un-issued shares and treasury shares are
		/// not included in this figure).
		/// </summary>
		/// <returns></returns>
		long   getSharesOut();

        /// <summary>
        /// Field State
        /// </summary>
        /// <returns></returns>
        MamdaFieldState getSharesOutFieldState();
        
		/// <summary>
		/// The number of shares of a security that are outstanding and
		/// available for trading by the public.
		/// </summary>
		/// <returns></returns>
		long   getSharesFloat();

        /// <summary>
        /// Field State
        /// </summary>
        /// <returns></returns>
        MamdaFieldState getSharesFloatFieldState();
        
		/// <summary>
		/// Authorized shares. The number of shares that a corporation is
		/// permitted to issue.
		/// </summary>
		/// <returns></returns>
		long   getSharesAuthorized();

        /// <summary>
        /// Field State
        /// </summary>
        /// <returns></returns>
        MamdaFieldState getSharesAuthorizedFieldState();
        
		/// <summary>
		/// Earnings per share, including common stock, preferred stock,
		/// unexercised stock options, unexercised warrants, and some
		/// convertible debt. In companies with a large amount of convertibles,
		/// warrants and stock options, diluted earnings per share are
		/// usually a more accurate measure of the company's real
		/// earning power than earnings per share.
		/// </summary>
		/// <returns></returns>
		double getEarningsPerShare();

        /// <summary>
        /// Field State
        /// </summary>
        /// <returns></returns>
        MamdaFieldState getEarningsPerShareFieldState();
        
		/// <summary>
		/// The relative rate at which the price of a security moves up and
		/// down.
		/// </summary>
		/// <returns></returns>
		double getVolatility();

        /// <summary>
        /// Field State
        /// </summary>
        /// <returns></returns>
        MamdaFieldState getVolatilityFieldState();
        
		/// <summary>
		/// The most common measure of how expensive a stock is. The P/E
		/// ratio is equal to a stock's market capitalization divided by its
		/// after-tax earnings over a 12-month period, usually the
		/// trailing period but occasionally the current or forward period.
		/// </summary>
		/// <returns></returns>
		double getPriceEarningsRatio();

        /// <summary>
        /// Field State
        /// </summary>
        /// <returns></returns>
        MamdaFieldState getPriceEarningsRatioFieldState();
        
		/// <summary>
		/// Yield, for cash instruments where prices are published
		/// </summary>
		/// <returns></returns>
		double getYield();

        /// <summary>
        /// Field State
        /// </summary>
        /// <returns></returns>
        MamdaFieldState getYieldFieldState();
        
		/// <summary>
		/// Feed-specific market segment code in native feed format.
		/// </summary>
		/// <returns></returns>
		string getMarketSegmentNative();

        /// <summary>
        /// Field State
        /// </summary>
        /// <returns></returns>
        MamdaFieldState getMarketSegmentNativeFieldState();
        
		/// <summary>
		/// Feed-specific market sector code in native feed format.
		/// </summary>
		/// <returns></returns>
		string getMarketSectorNative();

        /// <summary>
        /// Field State
        /// </summary>
        /// <returns></returns>
        MamdaFieldState getMarketSectorNativeFieldState();
        
		/// <summary>
		/// Market subgroup.
		/// </summary>
		/// <returns></returns>
		string getMarketSegment();

        /// <summary>
        /// Field State
        /// </summary>
        /// <returns></returns>
        MamdaFieldState getMarketSegmentFieldState();
        
		/// <summary>
		/// A distinct subset of a market, society, industry, or economy,
		/// whose components share similar characteristics
		/// </summary>
		/// <returns></returns>
		string getMarketSector();

        /// <summary>
        /// Field State
        /// </summary>
        /// <returns></returns>
        MamdaFieldState getMarketSectorFieldState();
        
		/// <summary>
		/// Volatility estimated from historical data
		/// </summary>
		/// <returns></returns>
		double getHistoricalVolatility();

        /// <summary>
        /// Field State
        /// </summary>
        /// <returns></returns>
        MamdaFieldState getHistoricalVolatilityFieldState();
        
		/// <summary>
		/// Theoretical interest rate at which an investment may earn
		/// interest without incurring any risk
		/// </summary>
		/// <returns></returns>
		double getRiskFreeRate();
        
        /// <summary>
        /// Field State
        /// </summary>
        /// <returns></returns>
        MamdaFieldState getRiskFreeRateFieldState();
        
	}
}
