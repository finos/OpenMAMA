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

/**
 * MamdaFundamentals is an interface that provides access to the 
 * fundamental equity pricing/analysis attributes, indicators and ratios.  
 */

public interface MamdaFundamentals extends MamdaBasicRecap
{
    /**
     * Corporate Action Type.
     */
    String getCorporateActionType();

    /**
     * @return Field State
     */
    short getCorporateActionTypeFieldState();

    /**
     * Dividend price.
     */
    double getDividendPrice();

    /**
     * @return Field State
     */
    short getDividendPriceFieldState();

    /**
     * Frequency of the dividend payments.
     * <ul>
     *  <li> : No dividend payments are made.</li>
     *  <li>1M : Monthly</li>
     *  <li>3M : Quarterly</li>
     *  <li>6M : Semi-Annually</li>
     *  <li>1Y : Annually</li>
     *  <li>SP : Payment frequency is special.</li>
     *  <li>NA : Irregular payment frequency.</li>
     *  <li>ER : Invalid dividend frequency period.</li>
     * </ul>
     */
    String getDividendFrequency();

    /**
     * @return Field State
     */
    short getDividendFrequencyFieldState();

    /**
     * The date on or after which a security is traded without a
     * previously declared dividend or distribution.
     */
    String getDividendExDate();

    /**
     * @return Field State
     */
    short getDividendExDateFieldState();

    /**
     * Date on which corporate action distribution will be paid or
     * effective.
     */
    String getDividendPayDate();

    /**
     * @return Field State
     */
    short getDividendPayDateFieldState();

    /**
     * This is the date on which all shareholders are considered a
     * "holder of record" and ensured the right of a dividend or distribution.
     */
    String getDividendRecordDate();

    /**
     * @return Field State
     */
    short getDividendRecordDateFieldState();

    /**
     * Currency of dividend.
     */
    String getDividendCurrency();

    /**
     * @return Field State
     */
    short getDividendCurrencyFieldState();

    /**
     * Shares outstanding. The number of authorized shares in a company
     * that are held by investors, including employees and
     * executives of that company. (Un-issued shares and treasury shares are
     * not included in this figure).
     */
    long   getSharesOut();

    /**
     * @return Field State
     */    short getSharesOutFieldState();

    /**
     * The number of shares of a security that are outstanding and
     * available for trading by the public.
     */
    long   getSharesFloat();

    /**
     * @return Field State
     */
    short getSharesFloatFieldState();

    /**
     * Authorized shares. The number of shares that a corporation is
     * permitted to issue.
     */
    long   getSharesAuthorized();

    /**
     * @return Field State
     */
    short getSharesAuthorizedFieldState();

    /**
     * Earnings per share, including common stock, preferred stock,
     * unexercised stock options, unexercised warrants, and some
     * convertible debt. In companies with a large amount of convertibles,
     * warrants and stock options, diluted earnings per share are
     * usually a more accurate measure of the company's real
     * earning power than earnings per share.
     */
    double getEarningsPerShare();

    /**
     * @return Field State
     */
    short getEarningsPerShareFieldState();

    /**
     * The relative rate at which the price of a security moves up and
     * down.
     */
    double getVolatility();

    /**
     * @return Field State
     */
    short getVolatilityFieldState();

    /**
     * The most common measure of how expensive a stock is. The P/E
     * ratio is equal to a stock's market capitalization divided by its
     * after-tax earnings over a 12-month period, usually the
     * trailing period but occasionally the current or forward period.
     */
    double getPriceEarningsRatio();

    /**
     * @return Field State
     */
    short getPriceEarningsRatioFieldState();

    /**
     * Yield, for cash instruments where prices are published
     */
    double getYield();

    /**
     * @return Field State
     */
    short getYieldFieldState();

    /**
     * Feed-specific market segment code in native feed format.
     */
    String getMarketSegmentNative();

    /**
     * @return Field State
     */
    short getMarketSegmentNativeFieldState();

    /**
     * Feed-specific market sector code in native feed format.
     */
    String getMarketSectorNative();

    /**
     * @return Field State
     */
    short getMarketSectorNativeFieldState();

    /**
     * Market subgroup.
     */
    String getMarketSegment();

    /**
     * @return Field State
     */
    short getMarketSegmentFieldState();

    /**
     * A distinct subset of a market, society, industry, or economy,
     * whose components share similar characteristics
     */
    String getMarketSector();

    /**
     * @return Field State
     */
    short getMarketSectorFieldState();

    /**
     * Volatility estimated from historical data
     */
    double getHistoricalVolatility();

    /**
     * @return Field State
     */
    short getHistoricalVolatilityFieldState();

    /**
     * Theoretical interest rate at which an investment may earn
     * interest without incurring any risk
     */
    double getRiskFreeRate();

    /**
     * @return Field State
     */
    short getRiskFreeRateFieldState();

}
