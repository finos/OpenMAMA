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

#ifndef MamdaSecStatusH
#define MamdaSecStatusH 

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaBasicEvent.h>

namespace Wombat
{

    /**
     * MamdaSecStatus is an interface that provides access to the 
     * Security Status fields such as symbol announce messages.  
     */

    class MAMDAExpDLL MamdaSecStatus : public MamdaBasicEvent
    {
    public:

        /**
         * The "name" of the instrument (e.g. IBM, CSCO, MSFT.INCA, etc.).
         *
         * @return The symbol name.
         */
        virtual const char*  getIssueSymbol() const = 0;

        /**
         * Get the issue symbol field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getIssueSymbolFieldState() const = 0;

        /**
         * Unnormalized (feed-specific) reason associated with the current status
         * of the security (e.g. halted, delayed, etc.).  For normalized field,
         * see <code>getSecurityStatusQual</code>.
         *
         * @return The reason for the current status.
         * @see getSecurityStatusQual()
         */
        virtual const char*  getReason() const = 0;

        /**
         * Get the reason field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getReasonFieldState() const = 0;

	    /**
         * @see MamdaSecStatusRecap#getShortSaleCircuitBreaker()
         */    
         virtual char  getShortSaleCircuitBreaker() const = 0;

	    /**
         * @see MamdaSecStatusRecap#getShortSaleCircuitBreakerFieldState()
         */
         virtual MamdaFieldState  getShortSaleCircuitBreakerFieldState() const =0;
           
        /**
         * Action related to this security.
         * <ul>
         *   <li>A : Add this new security.</li>
         * </ul>
         *
         * @return The security action.
         */
        virtual char  getSecurityAction() const = 0;

        /**
         * Get the security action field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getSecurityActionFieldState() const = 0;

        /**
         * The security type.
         * <ul>
         *   <li>Equity Option</li>
         *   <li>NEO Option : NEO (Index) Option</li>
         *   <li>ICS Option : ICS (Foreign Currency) Option</li>
         * </ul>
         *
         * @return The security type.
         */
        virtual const char*  getSecurityType() const = 0;

        /**
         * Get the security type field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getSecurityTypeFieldState() const = 0;

        /**
         * NYSE Technologies normalised security status. See getSecurityStatusOrig for
         * the original exchange value.
         *
         * <ul>
         *   <li>None : No security status is known/available for this security.</li>
         *   <li>Normal : Security is open for normal quoting and trading</li>
         *   <li>Closed : Security is closed (usually before or after market
         * open)</li>
         *   <li>Halted : Security has been halted by exchange.</li>
         *   <li>NotExist : Security does not currently exist in cache (but it might
         * in the future).</li>
         *   <li>Deleted : Security has been deleted (e.g., merger, expiration, etc.)
         * </li>
         * </ul>
         *
         * @return The normalized security status.
         */
        virtual const char*  getSecurityStatus() const = 0;

        /**
         * Get the security status field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getSecurityStatusFieldState() const = 0;

        /**
         * Security status qualifier.
         *
         * <ul>
         *   <li>None : No security status qualifier is known/available for this
         * security.</li>
         *   <li>Excused : An Excused withdrawal from the market.</li>
         *   <li>Withdrawn : Non-excused withdrawal by the market maker.</li>
         *   <li>Suspended : Suspended Trading</li>
         *   <li>Resume : Resume trading/quoting after halt.</li>
         *   <li>QuoteResume : Resume quoting after halt. NASDAQ distinguishes between
         * resumption of quoting versus trading, although these appear to always
         * occur one after the other.</li>
         *   <li>TradeResume : Resume trading after halt. NASDAQ distinguishes between
         * resumption of quoting versus trading, although these appear to always
         * occur one after the other.</li>
         *   <li>ResumeTime : When the security is expected to resume trading.</li>
         *   <li>MktImbBuy : Market Imbalance - Buy.  A 50,000 share or more excess of
         * market orders to buy over market orders to sell as of 9:00am on
         * expiration days.  A MktImbBuy implies an ordinary order imbalance
         * (OrdImbBuy).</li>
         *   <li>MktImbSell : Market Imbalance - Sell.  A 50,000 share or more excess
         * of market orders to sell over market orders to buy as of 9:00am on
         * expiration days.  A MktImbSell implies an ordinary order imbalance
         * (OrdImbSell).</li>
         *   <li>NoMktImb : No Market Imbalance.  Indicates that the imbalance of
         * market orders for a security is less than 50,000 shares as of 9:00am on
         * expiration days.</li>
         *   <li>MocImbBuy : Market On Close Imbalance - Buy.  An excess of 50,000
         * share or more of MOC orders to buy over MOC orders to sell (including MOC
         * sell plus and MOC sell short orders).  A MocImbBuy implies an ordinary
         * order imbalance (OrdImbBuy).</li>
         *   <li>MocImbSell : Market On Close Imbalance - Sell.  An excess of 50,000
         * share or more of MOC orders to sell (not including MOC sell short and MOC
         * sell plus orders) over orders to buy (including MOC orders to buy minus).
         * A MocImbSell implies an ordinary order imbalance (OrdImbSell).</li>
         *   <li>NoMocImb : No Market On Close (MOC) Imbalance.  The difference
         * between the number of shares to buy MOC and the number of shares to sell
         * MOC is less than 50,000.</li>
         *   <li>OrderImb : Non-regulatory condition: a significant imbalance of buy
         * or sell orders exists for this security.</li>
         *   <li>OrderInf : Non-regulatory condition where there is a significant
         * influx of orders.</li>
         *   <li>OrderImbBuy : Non-regulatory condition: a significant imbalance of
         * buy orders exists for this security.</li>
         *   <li>OrderImbSell : Non-regulatory condition: a significant imbalance of
         * sell orders exists for this security.</li>
         *   <li>OrderImbNone : The earlier imbalance of buy or sell orders no longer
         * exists for this security. It also might mean that there is no imbalance
         * to begin with.</li>
         *   <li>RangeInd : Trading Range Indication.  Not an Opening Delay or Trading
         * Halted condition: this condition is used prior to the opening of a
         * security to denote a probable trading range (bid and offer prices, no
         * sizes).</li>
         *   <li>ItsPreOpen : ITS pre-opening indication.</li>
         *   <li>Reserved : Reserved (e.g., CME).</li>
         *   <li>Frozen : Frozen (e.g., CME).</li>
         *   <li>PreOpen : Preopening state (e.g., CME).</li>
         *   <li>AddInfo : Additional Information.  For a security that is Opening
         * Delayed or Trading Halted, if inadequate information is disclosed during
         * a "news dissemination or news pending" Opening Delay or Trading Halt, the
         * Opening Delay or Trading Halt reason could be subsequently reported as
         * "Additional Information."</li>
         *   <li>OpenDelay : Security's opening has been delayed by exchange. This
         * value is usually followed by another value specifying the reason for the
         * opening delay.</li>
         *   <li>NoOpenNoResume : Indicates that trading halt or opening delay will be
         * in effect for the remainder of the trading day.</li>
         *   <li>PriceInd : An approximation of what a security's opening or
         * re-opening price range (bid and offer prices, no sizes) will be when
         * trading resumes after a delayed opening or after a trading halt.</li>
         *   <li>Equipment : Non-regulatory condition: the ability to trade this
         * security by a participant is temporarily inhibited due to a systems,
         * equipment or communications facility problem, or for other technical
         * reasons.</li>
         *   <li>Filings : Not current in regulatory filings.</li>
         *   <li>News : News pending. Denotes a regulatory trading halt due to an
         * expected news announcement which influence the security.  An Opening
         * Delay or Trading Halt may be continued once the news has been
         * disseminated.</li>
         *   <li>NewsDissem : News Dissemination.  Denotes a regulatory trading halt
         * when relevant news influencing the security is being disseminated.
         * Trading is suspended until the primary market determines that an adequate
         * publication or disclosure of information has occurred.</li>
         *   <li>Listing : Listing Noncompliance.</li>
         *   <li>Operation : Operational Halt</li>
         *   <li>Info : Information Requested. Regulatory condition: more disclosure
         * of information is requested by the exchange for this security.</li>
         *   <li>SEC : SEC Suspension.</li>
         *   <li>Times : News Resumption Times.</li>
         *   <li>Other : Other Regulatory Halt.</li>
         *   <li>Related : Related Security; In View Of Common.  Non-regulatory
         * condition: the halt or opening delay in this security is due to its
         * relationship with another security. This condition also applies to
         * non-common associated securities (e.g. warrants, rights, preferreds,
         * classes, etc.) in view of the common stock.</li>
         *   <li>IPO : Upcoming IPO issue not yet trading.</li>
         * </ul>
         *
         * @return The normalized security status qualifier.
         */ 
        virtual const char*  getSecurityStatusQual() const = 0;

        /**
         * Get the security status qualifier field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getSecurityStatusQualFieldState() const = 0;

        /**
         * Original "security status" field sent by the feed.
         * NYSE Technologies feed handlers also send normalized security status. 
         * 
         * @return The exchange provided security status.
         * @see getSecurityStatus()
         */
        virtual const char*  getSecurityStatusNative() const = 0;

        /**
         * Get the native security status field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState  getSecurityStatusNativeFieldState() const = 0;

        /**
         * Arbitrary free text associated with the security status change.
         *
         * @return Free text associated with the security status change.
         */
        virtual const char*  getFreeText() const = 0;

        /**
         * Get the free text field field state
         *
         * @return MamdaFieldState.  An enumeration representing field state.
         */
        virtual MamdaFieldState getFreeTextFieldState() const = 0;

        virtual ~MamdaSecStatus() {};
    };

} // namespace

#endif // MamdaSecStatusH
