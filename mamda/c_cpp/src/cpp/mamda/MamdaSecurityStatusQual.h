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

#ifndef MamdaSecurityStatusQualH
#define MamdaSecurityStatusQualH

#include <mamda/MamdaConfig.h>

namespace Wombat
{

    /**
     * An enumeration representing the status of a security
     * such as whether or not it is halted or closed for trading.
     */
    enum MamdaSecurityStatusQual
    {
        SECURITY_STATUS_QUAL_NONE                   = 0,   // toString() - None
        SECURITY_STATUS_QUAL_OPENING                = 1,   // toString() - Opening
        SECURITY_STATUS_QUAL_EXCUSED                = 7,   // toString() - Excused 
        SECURITY_STATUS_QUAL_WITHDRAWN              = 8,   // toString() - Withdrawn 
        SECURITY_STATUS_QUAL_SUSPENDED              = 9,   // toString() - Suspended
        SECURITY_STATUS_QUAL_RESUME                 = 11,  // toString() - Resume
        SECURITY_STATUS_QUAL_QUOTE_RESUME           = 12,  // toString() - QuoteResume
        SECURITY_STATUS_QUAL_TRADE_RESUME           = 13,  // toString() - TradeResume
        SECURITY_STATUS_QUAL_RESUME_TIME            = 14,  // toString() - ResumeTime  
        SECURITY_STATUS_QUAL_MKT_IMB_BUY            = 16,  // toString() - MktImbBuy 
        SECURITY_STATUS_QUAL_MKT_IMB_SELL           = 17,  // toString() - MktImbSell 
        SECURITY_STATUS_QUAL_NO_MKT_IMB             = 18,  // toString() - NoMktImb
        SECURITY_STATUS_QUAL_MOC_IMB_BUY            = 19,  // toString() - MocImbBuy
        SECURITY_STATUS_QUAL_MOC_IMB_SELL           = 20,  // toString() - MocImbSell
        SECURITY_STATUS_QUAL_NO_MOC_IMB             = 21,  // toString() - NoMocImb
        SECURITY_STATUS_QUAL_ORDER_IMB              = 22,  // toString() - OrderImb  
        SECURITY_STATUS_QUAL_ORDER_INF              = 23,  // toString() - OrderInf 
        SECURITY_STATUS_QUAL_ORDER_IMB_BUY          = 24,  // toString() - OrderImbBuy 
        SECURITY_STATUS_QUAL_ORDER_IMB_SELL         = 25,  // toString() - OrderImbSell
        SECURITY_STATUS_QUAL_ORDER_IMB_NONE         = 26,  // toString() - OrderImbNone
        SECURITY_STATUS_QUAL_LOA_IMBALANCE_BUY      = 27,  // toString() - LoaImbBuy
        SECURITY_STATUS_QUAL_LOA_IMBALANCE_SELL     = 28,  // toString() - LoaImbSell
        SECURITY_STATUS_QUAL_NO_LOA_IMBALANCE       = 29,  // toString() - LoaNoImb
        SECURITY_STATUS_QUAL_ORDERS_ELIMINATED      = 30,  // toString() - OrdersEliminated
        SECURITY_STATUS_QUAL_RANGE_ID               = 31,  // toString() - RangeInd
        SECURITY_STATUS_QUAL_ITS_PREOPEN            = 32,  // toString() - ItsPreOpen
        SECURITY_STATUS_QUAL_RESERVED               = 33,  // toString() - Reserved  
        SECURITY_STATUS_QUAL_FROZEN                 = 34,  // toString() - Frozen 
        SECURITY_STATUS_QUAL_PREOPEN                = 35,  // toString() - PreOpen 
        SECURITY_STATUS_QUAL_THO_IMBALANCE_BUY      = 36,  // toString() - ThoImbBuy
        SECURITY_STATUS_QUAL_THO_IMBALANCE_SELL     = 37,  // toString() - ThoImbSell
        SECURITY_STATUS_QUAL_NO_THO_IMBALANCE       = 38,  // toString() - ThoNoImb
        SECURITY_STATUS_QUAL_ADD_INFO               = 41,  // toString() - AddInfo
        SECURITY_STATUS_QUAL_IPO_IMBALANCE_BUY      = 42,  // toString() - IpoImbBuy
        SECURITY_STATUS_QUAL_IPO_IMBALANCE_SELL     = 43,  // toString() - IpoImbBuy
        SECURITY_STATUS_QUAL_NO_IPO_IMBALANCE       = 44,  // toString() - IpoNoImb
        SECURITY_STATUS_QUAL_EMC_IMB_BUY            = 46,  // toString() - EmcImbBuy
        SECURITY_STATUS_QUAL_EMC_IMB_SELL           = 47,  // toString() - EmcImbSell
        SECURITY_STATUS_QUAL_EMC_IMB_NONE           = 48,  // toString() - EmcImbNone
        SECURITY_STATUS_QUAL_OPEN_DELAY             = 51,  // toString() - OpenDelay
        SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME      = 52,  // toString() - NoOpenNoResume
        SECURITY_STATUS_QUAL_PRICE_IND              = 53,  // toString() - PriceInd
        SECURITY_STATUS_QUAL_EQUIPMENT              = 54,  // toString() - Equipment  
        SECURITY_STATUS_QUAL_FILINGS                = 55,  // toString() - Filings 
        SECURITY_STATUS_QUAL_NEWS                   = 56,  // toString() - News 
        SECURITY_STATUS_QUAL_NEWS_DISSEM            = 57,  // toString() - NewsDissem
        SECURITY_STATUS_QUAL_LISTING                = 58,  // toString() - Listing
        SECURITY_STATUS_QUAL_OPERATION              = 59,  // toString() - Operation
        SECURITY_STATUS_QUAL_INFO                   = 60,  // toString() - Info
        SECURITY_STATUS_QUAL_SEC                    = 61,  // toString() - SEC  
        SECURITY_STATUS_QUAL_TIMES                  = 62,  // toString() - Times 
        SECURITY_STATUS_QUAL_OTHER                  = 63,  // toString() - Other 
        SECURITY_STATUS_QUAL_RELATED                = 64,  // toString() - Related
        SECURITY_STATUS_QUAL_IPO                    = 65,  // toString() - IPO
        SECURITY_STATUS_QUAL_PRE_CROSS              = 66,  // toString() - Pre-Cross
        SECURITY_STATUS_QUAL_CROSS                  = 67,  // toString() - Cross
        SECURITY_STATUS_QUAL_RELEASED_FOR_QUOTATION = 68,  // toString() - Released For Quotation (IPO)
        SECURITY_STATUS_QUAL_IPO_WINDOW_EXT         = 69,  // toString() - IPO Window Extension
        SECURITY_STATUS_QUAL_PRECLOSING             = 70,  // toString() - PreClosing
        SECURITY_STATUS_QUAL_AUCTION_EXTENSION      = 71,  // toString() - Auction Extension
        SECURITY_STATUS_QUAL_VOLATILITY_AUCTION     = 72,  // toString() - Volatility Auction
        SECURITY_STATUS_QUAL_SECURITY_AUTHORISED    = 73,  // toString() - SecurityAuthorized
        SECURITY_STATUS_QUAL_SECURITY_FORBIDDEN     = 74,  // toString() - SecurityForbidden
        SECURITY_STATUS_QUAL_FAST_MARKET            = 75,  // toString() - FastMarket
        SECURITY_STATUS_QUAL_SLOW_MARKET            = 76,  // toString() - SlowMarket
        SECURITY_STATUS_QUAL_SUB_PENNY_TRADING      = 77,  // toString() - SubPennyTrading
        SECURITY_STATUS_QUAL_ORDER_INPUT            = 78,  // toString() - OrderInput
        SECURITY_STATUS_QUAL_PRE_ORDER_MATCHING     = 79,  // toString() - PreOrderMatching
        SECURITY_STATUS_QUAL_ORDER_MATCHING         = 80,  // toString() - OrderMatching
        SECURITY_STATUS_QUAL_BLOCKING               = 81,  // toString() - Blocking
        SECURITY_STATUS_QUAL_ORDER_CANCEL           = 82,  // toString() - OrderCancel
        SECURITY_STATUS_QUAL_FIXED_PRICE            = 83,  // toString() - FixedPrice
        SECURITY_STATUS_QUAL_SALES_INPUT            = 84,  // toString() - SalesInput
        SECURITY_STATUS_QUAL_EXCHANGE_INTERVENTION  = 85,  // toString() - ExchangeIntervention
        SECURITY_STATUS_QUAL_PRE_AUCTION            = 86,  // toString() - PreAuction
        SECURITY_STATUS_QUAL_ADJUST                 = 87,  // toString() - Adjust
        SECURITY_STATUS_QUAL_ADJUST_ON              = 88,  // toString() - AdjustOn
        SECURITY_STATUS_QUAL_LATE_TRADING           = 89,  // toString() - LateTrading
        SECURITY_STATUS_QUAL_ENQUIRE                = 90,  // toString() - Enquire
        SECURITY_STATUS_QUAL_PRE_NIGHT_TRADING      = 91,  // toString() - PreNightTrading
        SECURITY_STATUS_QUAL_OPEN_NIGHT_TRADING     = 92,  // toString() - OpenNightTrading
        SECURITY_STATUS_QUAL_SUB_HOLIDAY            = 93,  // toString() - SubHoliday
        SECURITY_STATUS_QUAL_BID_ONLY               = 94,  // toString() - BidOnly
        SECURITY_STATUS_QUAL_ASK_ONLY               = 95,  // toString() - AskOnly 

        SECURITY_STATUS_QUAL_UNKNOWN                = 99,   // toString() - Unknown
        
        SECURITY_STATUS_QUAL_OPENING_DELAY_COMMON           = 101, // toString() - OpenDelayCommon
        SECURITY_STATUS_QUAL_RESUME_COMMON                  = 102, // toString() - ResumeCommon
        SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME_COMMON       = 103, // toString() - NoOpenNoResumeCommon
        SECURITY_STATUS_QUAL_NEWS_DISSEMINATION_RELATED     = 104, // toString() - NewsDissemRelated
        SECURITY_STATUS_QUAL_ORDER_INFLUX_RELATED           = 105, // toString() - OrdInfluxRelated
        SECURITY_STATUS_QUAL_ORDER_IMBALANCE_RELATED        = 106, // toString() - OrdImbRelated
        SECURITY_STATUS_QUAL_INFORMATION_REQUESTED_RELATED  = 107, // toString() - InfoRequestRelated
        SECURITY_STATUS_QUAL_NEWS_PENDING_RELATED           = 108, // toString() - NewsPendingRelated
        SECURITY_STATUS_QUAL_EQUIPMENT_CHANGEOVER_RELATED   = 109, // toString() - EquipChangeOverRelated
        SECURITY_STATUS_QUAL_SUB_PENNY_TRADING_RELATED      = 110, // toString() - SubPennyTradingRelated
        
        SECURITY_STATUS_QUAL_IMBALANCE_PREOPEN_BUY          = 112, // toString() - ImbPreOpenBuy
        SECURITY_STATUS_QUAL_IMBALANCE_PREOPEN_SELL         = 113, // toString() - ImbPreOpenSell
        SECURITY_STATUS_QUAL_IMBALANCE_PRECLOSE_BUY         = 114, // toString() - ImbPreCloseBuy
        SECURITY_STATUS_QUAL_IMBALANCE_PRECLOSE_SELL        = 115, // toString() - ImbPreCloseSell    
        
        SECURITY_STATUS_QUAL_VOLATILITY_PAUSE               = 116, // toString() - VolatilityPause
        SECURITY_STATUS_QUAL_VOLATILITY_PAUSE_QUOTE_RESUME  = 117, // toString() - VolatilityPauseQuoteResume
        SECURITY_STATUS_QUAL_VOLATILITY_GUARD               = 118, // toString() - VolatilityGuard
        SECURITY_STATUS_QUAL_VOLATILITY_GUARD_QUOTE_RESUME  = 119  // toString() - VolatilityGuardQuoteResume
    };


    /**
     * Convert a MamdaSecurityStatusQual to an appropriate, displayable
     * string.
     *
     * @param securityStatusQual The security status qualifier as an enumeration.
     * 
     * @return The security status qualifier as a string.
     */
    MAMDAExpDLL const char*  toString (MamdaSecurityStatusQual securityStatusQual);

    /**
     * Convert a string representation of a security status to the
     * enumeration.  This function is used internally for compatibility
     * with older feed handler configurations, which may send the field as
     * a string.
     *
     * @param securityStatusQualStr The security status qualifier as a string.
     * 
     * @return The security status qualifier as an enumeration.
     */
    MAMDAExpDLL MamdaSecurityStatusQual  mamdaSecurityStatusQualFromString (
        const char* securityStatusQualStr);

}

#endif // MamdaSecurityStatusQualH
