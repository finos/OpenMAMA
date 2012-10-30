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
	/// MamdaSecurityStatusUpdate is an interface that provides access to security status
	/// related fields.
	/// </summary>
	public interface MamdaSecurityStatusUpdate
	{
		/// <summary>
		/// Return the security status as a long.
		/// </summary>
		/// <returns></returns>
		[Obsolete("Deprecated.  Use getSecurityStatusEnum() to return the security status as an enumerated type.  Use getSecurityStatusStr to return it as a string")]
		long getSecurityStatus();

		/// <summary>
		/// Return the security status qualifier as a long.
		/// </summary>
		/// <returns></returns>
		[Obsolete("Deprecated.  Use getSecurityStatusEnum() to return the security status qualifier as an enumerated type.  Use getSecurityStatusQualifierStr to return it as a string")]
		long getSecurityStatusQualifier();

		/// <summary>
		/// Wombat normalised security status. See getSecurityStatusOrig for
		/// the original exchange value.
		/// </summary>
		/// <remarks>
		/// <ul>
		/// <li>mamdaSecurityStatus.SECURITY_STATUS_NONE : No security status is known/available for
		/// this security.</li>
		/// <li>mamdaSecurityStatus.SECURITY_STATUS_NORMAL : Security is open for normal quoting and
		/// trading</li>
		/// <li>mamdaSecurityStatus.SECURITY_STATUS_CLOSED : Security is closed (usually before
		/// or after market open)</li>
		/// <li>mamdaSecurityStatus.SECURITY_STATUS_HALTED : Security has been halted by exchange.</li>
		/// <li>mamdaSecurityStatus.SECURITY_STATUS_NOT_EXIST : Security does not currently exist in cache
		/// (but it might in the future).</li>
		/// <li>mamdaSecurityStatus.SECURITY_STATUS_DELETED : Security has been deleted (e.g., merger,
		/// expiration, etc.)</li>
		/// <li>mamdaSecurityStatus.SECURITY_STATUS_AUCTION : Security has gone into auction.</li>
		/// <li>mamdaSecurityStatus.SECURITY_STATUS_CROSSING : Security crossing.</li>
		/// <li>mamdaSecurityStatus.SECURITY_STATUS_UNKNOWN : Security status is currently unknown.</li>
		/// </ul>
		/// </remarks>
		/// <returns></returns>
		MamdaSecurityStatus.mamdaSecurityStatus getSecurityStatusEnum();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getSecurityStatusEnumFieldState();

		/// <summary>
		/// Security status qualifier.
		/// </summary>
		/// <remarks>
		/// <ul>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NONE : No security status qualifier is known/available for this
		/// security.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_EXCUSED : An Excused withdrawl from the market.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_WITHDRAWN : Non-excused withdrawl by the market maker.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QIAL_SUSPENDED : Suspended Trading</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RESUME : Resume trading/quoting after halt.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_QUOTE_RESUME : Resume quoting after halt. Nasdaq distinguishes between
		/// resumption of quoting versus trading, although these appear to always
		/// occur one after the other.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_TRADE_RESUME : Resume trading after halt. Nasdaq distinguishes between
		/// resumption of quoting versus trading, although these appear to always
		/// occur one after the other.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RESUME_TIME : When the security is expected to resume trading.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_MKT_IMB_BUY : Market Imbalance - Buy.  A 50,000 share or more excess of
		/// market orders to buy over market orders to sell as of 9:00am on
		/// expiration days.  A MktImbBuy implies an ordinary order imbalance
		/// (OrdImbBuy).</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_MKT_IMB_SELL : Market Imbalance - Sell.  A 50,000 share or more excess
		/// of market orders to sell over market orders to buy as of 9:00am on
		/// expiration days.  A MktImbSell implies an ordinary order imbalance
		/// (OrdImbSell).</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_MKT_IMB : No Market Imbalance.  Indicates that the imbalance of
		/// market orders for a security is less than 50,000 shares as of 9:00am on
		/// expiration days.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_MOC_IMB_BUY : Market On Close Imbalance - Buy.  An excess of 50,000
		/// share or more of MOC orders to buy over MOC orders to sell (including MOC
		/// sell plus and MOC sell short orders).  A MocImbBuy implies an ordinary
		/// order imbalance (OrdImbBuy).</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_MOC_IMB_SELL : Market On Close Imbalance - Sell.  An excess of 50,000
		/// share or more of MOC orders to sell (not including MOC sell short and MOC
		/// sell plus orders) over orders to buy (including MOC orders to buy minus).
		/// A MocImbSell implies an ordinary order imbalance (OrdImbSell).</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_MOC_IMB : No Market On Close (MOC) Imbalance.  The difference
		/// between the number of shares to buy MOC and the number of shares to sell
		/// MOC is less than 50,000.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_IMB : Non-regulatory condition: a significant imbalance of buy
		/// or sell orders exists for this security.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_INF : Non-regulatory condition where there is a significant
		/// influx of orders.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_IMB_BUY : Non-regulatory condition: a significant imbalance of
		/// buy orders exists for this security.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_IMB_SELL : Non-regulatory condition: a significant imbalance of
		/// sell orders exists for this security.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_IMB_NONE : The earlier imbalance of buy or sell orders no longer
		/// exists for this security. It also might mean that there is no imbalance
		/// to begin with.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RANGE_IND : Trading Range Indication.  Not an Opening Delay or Trading
		/// Halted condition: this condition is used prior to the opening of a
		/// security to denote a probable trading range (bid and offer prices, no
		/// sizes).</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ITS_PREOPEN : ITS pre-opening indication.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RESERVED : Reserved (e.g., CME).</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_FROZEN : Frozen (e.g., CME).</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PREOPEN : Preopening state (e.g., CME).</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ADD_INFO : Additional Information.  For a security that is Opening
		/// Delayed or Trading Halted, if inadequate information is disclosed during
		/// a "news dissemination or news pending" Opening Delay or Trading Halt, the
		/// Opening Delay or Trading Halt reason could be subsequently reported as
		/// "Additional Information."</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_OPEN_DELAY : Security's opening has been delayed by exchange. This
		/// value is usually followed by another value specifying the reason for the
		/// opening delay.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME : Indicates that trading halt or opening delay will be
		/// in effect for the remainder of the trading day.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PRICE_IND : An approximation of what a security's opening or
		/// re-opening price range (bid and offer prices, no sizes) will be when
		/// trading resumes after a delayed opening or after a trading halt.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_EQUIPMENT : Non-regulatory condition: the ability to trade this
		/// security by a participant is temporarily inhibited due to a systems,
		/// equipment or communications facility problem, or for other technical
		/// reasons.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_FILINGS : Not current in regulatory filings.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NEWS : News pending. Denotes a regulatory trading halt due to an
		/// expected news announcement which influence the security.  An Opening
		/// Delay or Trading Halt may be continued once the news has been
		/// disseminated.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NEWS_DISSEM : News Dissemination.  Denotes a regulatory trading halt
		/// when relevant news influencing the security is being disseminated.
		/// Trading is suspended until the primary market determines that an adequate
		/// publication or disclosure of information has occurred.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_LISTING : Listing Noncompliance.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_OPERATION : Operational Halt</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_INFO : Information Requested. Regulatory condition: more disclosure
		/// of information is requested by the exchange for this security.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SEC : SEC Suspension.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_TIMES : News Resumption Times.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_OTHER : Other Regulatory Halt.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RELATED : Related Security; In View Of Common.  Non-regulatory
		/// condition: the halt or opening delay in this security is due to its
		/// relationship with another security. This condition also applies to
		/// non-common associated securities (e.g. warrants, rights, preferreds,
		/// classes, etc.) in view of the common stock.</li>
		/// <li>mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IPO : Upcoming IPO issue not yet trading.</li>
		/// </ul>
		/// </remarks>
		/// <returns></returns>
		MamdaSecurityStatusQual.mamdaSecurityStatusQual getSecurityStatusQualifierEnum();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getSecurityStatusQualifierEnumFieldState();

		/// <summary>
		/// Wombat normalised security status. See getSecurityStatusOrig for
		/// the original exchange value.
		/// </summary>
		/// <remarks>
		/// <ul>
		/// <li>None : No security status is known/available for
		/// this security.</li>
		/// <li>Normal : Security is open for normal quoting and
		/// trading</li>
		/// <li>Closed : Security is closed (usually before
		/// or after market open)</li>
		/// <li>Halted : Security has been halted by exchange.</li>
		/// <li>NotExist : Security does not currently exist in cache
		/// (but it might in the future).</li>
		/// <li>Deleted : Security has been deleted (e.g., merger,
		/// expiration, etc.)</li>
		/// <li>Auction : Security has gone into auction.</li>
		/// <li>Crossing : Security crossing.</li>
		/// <li>Unknown : Security status is currently unknown.</li>
		/// </ul>
		/// </remarks>
		/// <returns></returns>
		string getSecurityStatusStr();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getSecurityStatusStrFieldState();

		/// <summary>
		/// Security status qualifier.
		/// </summary>
		/// <remarks>
		/// <ul>
		/// <li>None : No security status qualifier is known/available for this
		/// security.</li>
		/// <li>Excused : An Excused withdrawl from the market.</li>
		/// <li>Withdrawn : Non-excused withdrawl by the market maker.</li>
		/// <li>Suspended : Suspended Trading</li>
		/// <li>Resume : Resume trading/quoting after halt.</li>
		/// <li>QuoteResume : Resume quoting after halt. Nasdaq distinguishes between
		/// resumption of quoting versus trading, although these appear to always
		/// occur one after the other.</li>
		/// <li>TradeResume : Resume trading after halt. Nasdaq distinguishes between
		/// resumption of quoting versus trading, although these appear to always
		/// occur one after the other.</li>
		/// <li>ResumeTime : When the security is expected to resume trading.</li>
		/// <li>MktImbBuy : Market Imbalance - Buy.  A 50,000 share or more excess of
		/// market orders to buy over market orders to sell as of 9:00am on
		/// expiration days.  A MktImbBuy implies an ordinary order imbalance
		/// (OrdImbBuy).</li>
		/// <li>MktImbSell : Market Imbalance - Sell.  A 50,000 share or more excess
		/// of market orders to sell over market orders to buy as of 9:00am on
		/// expiration days.  A MktImbSell implies an ordinary order imbalance
		/// (OrdImbSell).</li>
		/// <li>NoMktImb : No Market Imbalance.  Indicates that the imbalance of
		/// market orders for a security is less than 50,000 shares as of 9:00am on
		/// expiration days.</li>
		/// <li>MocImbBuy : Market On Close Imbalance - Buy.  An excess of 50,000
		/// share or more of MOC orders to buy over MOC orders to sell (including MOC
		/// sell plus and MOC sell short orders).  A MocImbBuy implies an ordinary
		/// order imbalance (OrdImbBuy).</li>
		/// <li>MocImbSell : Market On Close Imbalance - Sell.  An excess of 50,000
		/// share or more of MOC orders to sell (not including MOC sell short and MOC
		/// sell plus orders) over orders to buy (including MOC orders to buy minus).
		/// A MocImbSell implies an ordinary order imbalance (OrdImbSell).</li>
		/// <li>NoMocImb : No Market On Close (MOC) Imbalance.  The difference
		/// between the number of shares to buy MOC and the number of shares to sell
		/// MOC is less than 50,000.</li>
		/// <li>OrderImb : Non-regulatory condition: a significant imbalance of buy
		/// or sell orders exists for this security.</li>
		/// <li>OrderInf : Non-regulatory condition where there is a significant
		/// influx of orders.</li>
		/// <li>OrderImbBuy : Non-regulatory condition: a significant imbalance of
		/// buy orders exists for this security.</li>
		/// <li>OrderImbSell : Non-regulatory condition: a significant imbalance of
		/// sell orders exists for this security.</li>
		/// <li>OrderImbNone : The earlier imbalance of buy or sell orders no longer
		/// exists for this security. It also might mean that there is no imbalance
		/// to begin with.</li>
		/// <li>RangeInd : Trading Range Indication.  Not an Opening Delay or Trading
		/// Halted condition: this condition is used prior to the opening of a
		/// security to denote a probable trading range (bid and offer prices, no
		/// sizes).</li>
		/// <li>ItsPreOpen : ITS pre-opening indication.</li>
		/// <li>Reserved : Reserved (e.g., CME).</li>
		/// <li>Frozen : Frozen (e.g., CME).</li>
		/// <li>PreOpen : Preopening state (e.g., CME).</li>
		/// <li>AddInfo : Additional Information.  For a security that is Opening
		/// Delayed or Trading Halted, if inadequate information is disclosed during
		/// a "news dissemination or news pending" Opening Delay or Trading Halt, the
		/// Opening Delay or Trading Halt reason could be subsequently reported as
		/// "Additional Information."</li>
		/// <li>OpenDelay : Security's opening has been delayed by exchange. This
		/// value is usually followed by another value specifying the reason for the
		/// opening delay.</li>
		/// <li>NoOpenNoResume : Indicates that trading halt or opening delay will be
		/// in effect for the remainder of the trading day.</li>
		/// <li>PriceInd : An approximation of what a security's opening or
		/// re-opening price range (bid and offer prices, no sizes) will be when
		/// trading resumes after a delayed opening or after a trading halt.</li>
		/// <li>Equipment : Non-regulatory condition: the ability to trade this
		/// security by a participant is temporarily inhibited due to a systems,
		/// equipment or communications facility problem, or for other technical
		/// reasons.</li>
		/// <li>Filings : Not current in regulatory filings.</li>
		/// <li>News : News pending. Denotes a regulatory trading halt due to an
		/// expected news announcement which influence the security.  An Opening
		/// Delay or Trading Halt may be continued once the news has been
		/// disseminated.</li>
		/// <li>NewsDissem : News Dissemination.  Denotes a regulatory trading halt
		/// when relevant news influencing the security is being disseminated.
		/// Trading is suspended until the primary market determines that an adequate
		/// publication or disclosure of information has occurred.</li>
		/// <li>Listing : Listing Noncompliance.</li>
		/// <li>Operation : Operational Halt</li>
		/// <li>Info : Information Requested. Regulatory condition: more disclosure
		/// of information is requested by the exchange for this security.</li>
		/// <li>SEC : SEC Suspension.</li>
		/// <li>Times : News Resumption Times.</li>
		/// <li>Other : Other Regulatory Halt.</li>
		/// <li>Related : Related Security; In View Of Common.  Non-regulatory
		/// condition: the halt or opening delay in this security is due to its
		/// relationship with another security. This condition also applies to
		/// non-common associated securities (e.g. warrants, rights, preferreds,
		/// classes, etc.) in view of the common stock.</li>
		/// <li>IPO : Upcoming IPO issue not yet trading.</li>
		/// </ul>
		/// </remarks>
		/// <returns></returns>
		string getSecurityStatusQualifierStr();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getSecurityStatusQualifierStrFieldState();

        /// <summary>
		/// Return the native security status qualifier
		/// </summary>
		/// <returns></returns>
        string getSecurityStatusOrigStr();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getSecurityStatusOrigStrFieldState();

		/// <summary>
		/// Unnormalized (feed-specific) reason associated with the current status
		/// of the security (e.g. halted, delayed, etc.). 
		/// </summary>
		/// <returns></returns>
		string getReason();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getReasonFieldState();

		/// <summary>
		/// ShortSaleCircuitBreaker
		/// </summary>
		/// <returns>return values: 
		/// Blank: Short Sale Restriction Not in Effect.
		/// A: Short Sale Restriction Activiated.
		/// C: Short Sale Restriction Continued.
		/// D: Sale Restriction Deactivated.
		/// E: Sale Restriction in Effect.</returns>
        char getShortSaleCircuitBreaker();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getShortSaleCircuitBreakerFieldState();

	}
}
