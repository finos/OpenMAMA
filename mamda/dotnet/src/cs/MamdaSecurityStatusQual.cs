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
	public class MamdaSecurityStatusQual
	{
		public enum mamdaSecurityStatusQual
		{
			SECURITY_STATUS_QUAL_NONE                          = 0,      // toString None
            SECURITY_STATUS_QUAL_OPENING                       = 1,      // toString Opening
			SECURITY_STATUS_QUAL_EXCUSED                       = 7,      // toString Excused
			SECURITY_STATUS_QUAL_WITHDRAWN                     = 8,      // toString Withdrawn
			SECURITY_STATUS_QUAL_SUSPENDED                     = 9,      // toString Suspended
			SECURITY_STATUS_QUAL_RESUME                        = 11,     // toString Resume
			SECURITY_STATUS_QUAL_QUOTE_RESUME                  = 12,     // toString QuoteResume
			SECURITY_STATUS_QUAL_TRADE_RESUME                  = 13,     // toString TradeResume
			SECURITY_STATUS_QUAL_RESUME_TIME                   = 14,     // toString ResumeTime
			SECURITY_STATUS_QUAL_MKT_IMB_BUY                   = 16,     // toString MktImbBuy
			SECURITY_STATUS_QUAL_MKT_IMB_SELL                  = 17,     // toString MktImbSell
			SECURITY_STATUS_QUAL_NO_MKT_IMB                    = 18,     // toString NoMktImb
			SECURITY_STATUS_QUAL_MOC_IMB_BUY                   = 19,     // toString MocImbBuy
			SECURITY_STATUS_QUAL_MOC_IMB_SELL                  = 20,     // toString MocImbSell
			SECURITY_STATUS_QUAL_NO_MOC_IMB                    = 21,     // toString NoMocImb
			SECURITY_STATUS_QUAL_ORDER_IMB                     = 22,     // toString OrderImb
			SECURITY_STATUS_QUAL_ORDER_INF                     = 23,     // toString OrderInf
			SECURITY_STATUS_QUAL_ORDER_IMB_BUY                 = 24,     // toString OrderImbBuy
			SECURITY_STATUS_QUAL_ORDER_IMB_SELL                = 25,     // toString OrderImbSell
			SECURITY_STATUS_QUAL_ORDER_IMB_NONE                = 26,     // toString OrderImbNone
            SECURITY_STATUS_QUAL_LOA_IMBALANCE_BUY             = 27,     // toString LoaImbBuy
            SECURITY_STATUS_QUAL_LOA_IMBALANCE_SELL            = 28,     // toString LoaImbSell
            SECURITY_STATUS_QUAL_NO_LOA_IMBALANCE              = 29,     // toString LoaNoImb
            SECURITY_STATUS_QUAL_ORDERS_ELIMINATED             = 30,     // toString OrdersElim
			SECURITY_STATUS_QUAL_RANGE_ID                      = 31,     // toString RangeInd
			SECURITY_STATUS_QUAL_ITS_PREOPEN                   = 32,     // toString ItsPreOpen
			SECURITY_STATUS_QUAL_RESERVED                      = 33,     // toString Reserved
			SECURITY_STATUS_QUAL_FROZEN                        = 34,     // toString Frozen
			SECURITY_STATUS_QUAL_PREOPEN                       = 35,     // toString PreOpen
            SECURITY_STATUS_QUAL_THO_IMBALANCE_BUY             = 36,     // toString ThoImbBuy
            SECURITY_STATUS_QUAL_THO_IMBALANCE_SELL            = 37,     // toString ThoImbSell
            SECURITY_STATUS_QUAL_NO_THO_IMBALANCE              = 38,     // toString ThoNoImb
			SECURITY_STATUS_QUAL_ADD_INFO                      = 41,     // toString AddInfo
            SECURITY_STATUS_QUAL_IPO_IMBALANCE_BUY             = 42,     // toString IpoImbBuy
            SECURITY_STATUS_QUAL_IPO_IMBALANCE_SELL            = 43,     // toString IpoImbSell
            SECURITY_STATUS_QUAL_NO_IPO_IMBALANCE              = 44,     // toString IpoNoImb
            SECURITY_STATUS_QUAL_EMC_IMB_BUY                   = 46,     // toString EmcImbBuy
            SECURITY_STATUS_QUAL_EMC_IMB_SELL                  = 47,     // toString EmcImbSell
            SECURITY_STATUS_QUAL_EMC_IMB_NONE                  = 48,     // toString EmcImbNone
            SECURITY_STATUS_QUAL_OPEN_DELAY                    = 51,     // toString OpenDelay
			SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME             = 52,     // toString NoOpenNoResume
			SECURITY_STATUS_QUAL_PRICE_IND                     = 53,     // toString PriceInd
			SECURITY_STATUS_QUAL_EQUIPMENT                     = 54,     // toString Equipment
			SECURITY_STATUS_QUAL_FILINGS                       = 55,     // toString Filings
			SECURITY_STATUS_QUAL_NEWS                          = 56,     // toString News
			SECURITY_STATUS_QUAL_NEWS_DISSEM                   = 57,     // toString NewsDissem
			SECURITY_STATUS_QUAL_LISTING                       = 58,     // toString Listing
			SECURITY_STATUS_QUAL_OPERATION                     = 59,     // toString Operation
			SECURITY_STATUS_QUAL_INFO                          = 60,     // toString Info
			SECURITY_STATUS_QUAL_SEC                           = 61,     // toString SEC
			SECURITY_STATUS_QUAL_TIMES                         = 62,     // toString Times
			SECURITY_STATUS_QUAL_OTHER                         = 63,     // toString Other
			SECURITY_STATUS_QUAL_RELATED                       = 64,     // toString Related
			SECURITY_STATUS_QUAL_IPO                           = 65,     // toString IPO
            SECURITY_STATUS_QUAL_PRE_CROSS                     = 66,     // toString Pre-Cross
            SECURITY_STATUS_QUAL_CROSS                         = 67,     // toString Cross
            SECURITY_STATUS_QUAL_RELEASED_FOR_QUOTATION        = 68,     // toString Released For Quotation (IPO) 
            SECURITY_STATUS_QUAL_IPO_WINDOW_EXT                = 69,     // toString IPO Window Extension
            SECURITY_STATUS_QUAL_PRECLOSING                    = 70,     // toString PreClosing
            SECURITY_STATUS_QUAL_AUCTION_EXTENSION             = 71,     // toString Auction Extension
            SECURITY_STATUS_QUAL_VOLATILITY_AUCTION            = 72,     // toString Volatility Auction
            SECURITY_STATUS_QUAL_SECURITY_AUTHORISED           = 73,     // toString SecurityAuthorized
            SECURITY_STATUS_QUAL_SECURITY_FORBIDDEN            = 74,     // toString SecurityForbidden
            SECURITY_STATUS_QUAL_FAST_MARKET                   = 75,     // toString FastMarket
            SECURITY_STATUS_QUAL_SLOW_MARKET                   = 76,     // toString SlowMarket
            SECURITY_STATUS_QUAL_SUM_PENNY_TRADING             = 77,     // toString SubPennyTrading
            SECURITY_STATUS_QUAL_ORDER_INPUT                   = 78,     // toString OrderInput
            SECURITY_STATUS_QUAL_PRE_ORDER_MATCHING            = 79,     // toString PreOrderMatching
            SECURITY_STATUS_QUAL_ORDER_MATCHING                = 80,     // toString OrderMatching
            SECURITY_STATUS_QUAL_BLOCKING                      = 81,     // toString Blocking
            SECURITY_STATUS_QUAL_ORDER_CANCEL                  = 82,     // toString OrderCancel
            SECURITY_STATUS_QUAL_FIXED_PRICE                   = 83,     // toString FixedPrice
            SECURITY_STATUS_QUAL_SALES_INPUT                   = 84,     // toString SalesInput
            SECURITY_STATUS_QUAL_EXCHANGE_INTERVENTION         = 85,     // toString ExchangeIntervention
            SECURITY_STATUS_QUAL_PRE_AUCTION                   = 86,     // toString PreAuction
            SECURITY_STATUS_QUAL_ADJUST                        = 87,     // toString Adjust
            SECURITY_STATUS_QUAL_ADJUST_ON                     = 88,     // toString AdjustOn
            SECURITY_STATUS_QUAL_LATE_TRADING                  = 89,     // toString LateTrading
            SECURITY_STATUS_QUAL_ENQUIRE                       = 90,     // toString Enquire
            SECURITY_STATUS_QUAL_PRE_NIGHT_TRADING             = 91,     // toString PreNightTrading
            SECURITY_STATUS_QUAL_OPEN_NIGHT_TRADING            = 92,     // toString OpenNightTrading
            SECURITY_STATUS_QUAL_SUB_HOLIDAY                   = 93,     // toString SubHol
            SECURITY_STATUS_QUAL_BID_ONLY                      = 94,     // toString BidOnly
            SECURITY_STATUS_QUAL_ASK_ONLY                      = 95,     // toString AskOnly
            SECURITY_STATUS_QUAL_UNKNOWN                       = 99,     // toString Unknown  
            SECURITY_STATUS_QUAL_OPENING_DELAY_COMMON          = 101,    // toString OpeningDelayCommon
            SECURITY_STATUS_QUAL_RESUME_COMMON                 = 102,    // toString ResumeCommon
            SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME_COMMON      = 103,    // toString NoOpenNoResumeCommon
            SECURITY_STATUS_QUAL_NEWS_DISSEMINATION_RELATED    = 104,    // toString NewsDissemRelated
            SECURITY_STATUS_QUAL_ORDER_INFLUX_RELATED          = 105,    // toString OrderInfluxRelated
            SECURITY_STATUS_QUAL_ORDER_IMBALANCE_RELATED       = 106,    // toString OrderImbRelated
            SECURITY_STATUS_QUAL_INFORMATION_REQUESTED_RELATED = 107,    // toString InfoRequestedRelated
            SECURITY_STATUS_QUAL_NEWS_PENDING_RELATED          = 108,    // toString NewsPendingRelated
            SECURITY_STATUS_QUAL_EQUIPMENT_CHANGEOVER_RELATED  = 109,    // toString EquipChangeOverRelated
            SECURITY_STATUS_QUAL_SUB_PENNY_TRADING_RELATED     = 110,    // toString SubPennyTradingRelated   
            SECURITY_STATUS_QUAL_IMBALANCE_PREOPEN_BUY         = 112,    // toString ImbPreOpenBuy
            SECURITY_STATUS_QUAL_IMBALANCE_PREOPEN_SELL        = 113,    // toString ImbPreOpenSell
            SECURITY_STATUS_QUAL_IMBALANCE_PRECLOSE_BUY        = 114,    // toString ImbPreCloseBuy
            SECURITY_STATUS_QUAL_IMBALANCE_PRECLOSE_SELL       = 115,    // toString ImbPreCloseSell
            SECURITY_STATUS_QUAL_VOLATILITY_PAUSE              = 116,    // toString VolatilityPause
            SECURITY_STATUS_QUAL_VOLATILITY_PAUSE_QUOTE_RESUME = 117,    // toString VolatilityPauseQuoteResume
            SECURITY_STATUS_QUAL_VOLATILITY_GUARD              = 118,    // toString VolatilityGuard
            SECURITY_STATUS_QUAL_VOLATILITY_GUARD_QUOTE_RESUME = 119     // toString VolatilityGuardQuoteResume
		}

		private static string SECURITY_STATUS_QUAL_STR_NONE                           = "None";
        private static string SECURITY_STATUS_QUAL_STR_OPENING                        = "Opening";   
		private static string SECURITY_STATUS_QUAL_STR_EXCUSED                        = "Excused";
		private static string SECURITY_STATUS_QUAL_STR_WITHDRAWN                      = "Withdrawn";
		private static string SECURITY_STATUS_QUAL_STR_SUSPENDED                      = "Suspended";
		private static string SECURITY_STATUS_QUAL_STR_RESUME                         = "Resume";
		private static string SECURITY_STATUS_QUAL_STR_QUOTE_RESUME                   = "QuoteResume";
		private static string SECURITY_STATUS_QUAL_STR_TRADE_RESUME                   = "TradeResume";
		private static string SECURITY_STATUS_QUAL_STR_RESUME_TIME                    = "ResumeTime";
		private static string SECURITY_STATUS_QUAL_STR_MKT_IMB_BUY                    = "MktImbBuy";
		private static string SECURITY_STATUS_QUAL_STR_MKT_IMB_SELL                   = "MktImbSell";
		private static string SECURITY_STATUS_QUAL_STR_NO_MKT_IMB                     = "NoMktImb";
		private static string SECURITY_STATUS_QUAL_STR_MOC_IMB_BUY                    = "MocImbBuy";
		private static string SECURITY_STATUS_QUAL_STR_MOC_IMB_SELL                   = "MocImbSell";
		private static string SECURITY_STATUS_QUAL_STR_NO_MOC_IMB                     = "NoMocImb";
		private static string SECURITY_STATUS_QUAL_STR_ORDER_IMB                      = "OrderImb";
		private static string SECURITY_STATUS_QUAL_STR_ORDER_INF                      = "OrderInf";
		private static string SECURITY_STATUS_QUAL_STR_ORDER_IMB_BUY                  = "OrderImbBuy";
		private static string SECURITY_STATUS_QUAL_STR_ORDER_IMB_SELL                 = "OrderImbSell";
		private static string SECURITY_STATUS_QUAL_STR_ORDER_IMB_NONE                 = "OrderImbNone";
        private static string SECURITY_STATUS_QUAL_STR_LOA_IMBALANCE_BUY              = "LoaImbBuy";
        private static string SECURITY_STATUS_QUAL_STR_LOA_IMBALANCE_SELL             = "LoaImbSell";
        private static string SECURITY_STATUS_QUAL_STR_NO_LOA_IMBALANCE               = "LoaNoImb";
        private static string SECURITY_STATUS_QUAL_STR_ORDERS_ELIMINATED              = "OrderElim";
        private static string SECURITY_STATUS_QUAL_STR_THO_IMBALANCE_BUY              = "ThoImbBuy";
        private static string SECURITY_STATUS_QUAL_STR_THO_IMBALANCE_SELL             = "ThoImbSell";
        private static string SECURITY_STATUS_QUAL_STR_NO_THO_IMBALANCE               = "ThoNoImb";
        private static string SECURITY_STATUS_QUAL_STR_IPO_IMBALANCE_BUY              = "IpoImbBuy";
        private static string SECURITY_STATUS_QUAL_STR_IPO_IMBALANCE_SELL             = "IpoImbSell";
        private static string SECURITY_STATUS_QUAL_STR_NO_IPO_IMBALANCE               = "IpoNoImb";
		private static string SECURITY_STATUS_QUAL_STR_RANGE_ID                       = "RangeInd";
		private static string SECURITY_STATUS_QUAL_STR_ITS_PREOPEN                    = "ItsPreOpen";
		private static string SECURITY_STATUS_QUAL_STR_RESERVED                       = "Reserved";
		private static string SECURITY_STATUS_QUAL_STR_FROZEN                         = "Frozen";
		private static string SECURITY_STATUS_QUAL_STR_PREOPEN                        = "PreOpen";
		private static string SECURITY_STATUS_QUAL_STR_ADD_INFO                       = "AddInfo";
		private static string SECURITY_STATUS_QUAL_STR_OPEN_DELAY                     = "OpenDelay";
		private static string SECURITY_STATUS_QUAL_STR_NO_OPEN_NO_RESUME              = "NoOpenNoResume";
		private static string SECURITY_STATUS_QUAL_STR_PRICE_IND                      = "PriceInd";
		private static string SECURITY_STATUS_QUAL_STR_EQUIPMENT                      = "Equipment";
		private static string SECURITY_STATUS_QUAL_STR_FILINGS                        = "Filings";
		private static string SECURITY_STATUS_QUAL_STR_NEWS                           = "News";
		private static string SECURITY_STATUS_QUAL_STR_NEWS_DISSEM                    = "NewsDissem";
		private static string SECURITY_STATUS_QUAL_STR_LISTING                        = "Listing";
		private static string SECURITY_STATUS_QUAL_STR_OPERATION                      = "Operation";
		private static string SECURITY_STATUS_QUAL_STR_INFO                           = "Info";
		private static string SECURITY_STATUS_QUAL_STR_SEC                            = "SEC";
		private static string SECURITY_STATUS_QUAL_STR_TIMES                          = "Times";
		private static string SECURITY_STATUS_QUAL_STR_OTHER                          = "Other";
		private static string SECURITY_STATUS_QUAL_STR_RELATED                        = "Related";
		private static string SECURITY_STATUS_QUAL_STR_IPO                            = "IPO";
        
        private static  string SECURITY_STATUS_QUAL_STR_EMC_IMB_BUY                   = "EmcImbBuy";
        private static  string SECURITY_STATUS_QUAL_STR_EMC_IMB_SELL                  = "EmcImbSell";
        private static  string SECURITY_STATUS_QUAL_STR_EMC_IMB_NONE                  = "EmcImbNone";
        private static  string SECURITY_STATUS_QUAL_STR_PRE_CROSS                     = "Pre-Cross";
        private static  string SECURITY_STATUS_QUAL_STR_CROSS                         = "Cross";
        private static  string SECURITY_STATUS_QUAL_STR_RELEASED_FOR_QUOTATION        = "Released For Quotation (IPO)"; 
        private static  string SECURITY_STATUS_QUAL_STR_IPO_WINDOW_EXT                = "IPO Window Extension";
        private static  string SECURITY_STATUS_QUAL_STR_PRECLOSING                    = "PreClosing";
        private static  string SECURITY_STATUS_QUAL_STR_AUCTION_EXTENSION             = "Auction Extension";
        private static  string SECURITY_STATUS_QUAL_STR_VOLATILITY_AUCTION            = "Volatility Auction";
        private static  string SECURITY_STATUS_QUAL_STR_SECURITY_AUTHORISED           = "SecurityAuthorized";
        private static  string SECURITY_STATUS_QUAL_STR_SECURITY_FORBIDDEN            = "SecurityForbidden";
        private static  string SECURITY_STATUS_QUAL_STR_FAST_MARKET                   = "FastMarket";
        private static  string SECURITY_STATUS_QUAL_STR_SLOW_MARKET                   = "SlowMarket";
        private static  string SECURITY_STATUS_QUAL_STR_SUM_PENNY_TRADING             = "SubPennyTrading";
        private static  string SECURITY_STATUS_QUAL_STR_ORDER_INPUT                   = "OrderInput";
        private static  string SECURITY_STATUS_QUAL_STR_PRE_ORDER_MATCHING            = "PreOrderMatching";
        private static  string SECURITY_STATUS_QUAL_STR_ORDER_MATCHING                = "OrderMatching";
        private static  string SECURITY_STATUS_QUAL_STR_BLOCKING                      = "Blocking";
        private static  string SECURITY_STATUS_QUAL_STR_ORDER_CANCEL                  = "OrderCancel";
        private static  string SECURITY_STATUS_QUAL_STR_FIXED_PRICE                   = "FixedPrice";
        private static  string SECURITY_STATUS_QUAL_STR_SALES_INPUT                   = "SalesInput";
        private static  string SECURITY_STATUS_QUAL_STR_EXCHANGE_INTERVENTION         = "ExchangeIntervention";
        private static  string SECURITY_STATUS_QUAL_STR_PRE_AUCTION                   = "PreAuction";
        private static  string SECURITY_STATUS_QUAL_STR_ADJUST                        = "Adjust";
        private static  string SECURITY_STATUS_QUAL_STR_ADJUST_ON                     = "AdjustOn";
        private static  string SECURITY_STATUS_QUAL_STR_LATE_TRADING                  = "LateTrading";
        private static  string SECURITY_STATUS_QUAL_STR_ENQUIRE                       = "Enquire";
        private static  string SECURITY_STATUS_QUAL_STR_PRE_NIGHT_TRADING             = "PreNightTrading";
        private static  string SECURITY_STATUS_QUAL_STR_OPEN_NIGHT_TRADING            = "OpenNightTrading";
        private static  string SECURITY_STATUS_QUAL_STR_SUB_HOLIDAY                   = "SubHol";
        private static  string SECURITY_STATUS_QUAL_STR_BID_ONLY                      = "BidOnly";
        private static  string SECURITY_STATUS_QUAL_STR_ASK_ONLY                      = "AskOnly";
        private static  string SECURITY_STATUS_QUAL_STR_OPENING_DELAY_COMMON          = "OpeningDelayCommon";
        private static  string SECURITY_STATUS_QUAL_STR_RESUME_COMMON                 = "ResumeCommon";
        private static  string SECURITY_STATUS_QUAL_STR_NO_OPEN_NO_RESUME_COMMON      = "NoOpenNoResumeCommon";
        private static  string SECURITY_STATUS_QUAL_STR_NEWS_DISSEMINATION_RELATED    = "NewsDissemRelated";
        private static  string SECURITY_STATUS_QUAL_STR_ORDER_INFLUX_RELATED          = "OrderInfluxRelated";
        private static  string SECURITY_STATUS_QUAL_STR_ORDER_IMBALANCE_RELATED       = "OrderImbRelated";
        private static  string SECURITY_STATUS_QUAL_STR_INFORMATION_REQUESTED_RELATED = "InfoRequestedRelated";
        private static  string SECURITY_STATUS_QUAL_STR_NEWS_PENDING_RELATED          = "NewsPendingRelated";
        private static  string SECURITY_STATUS_QUAL_STR_EQUIPMENT_CHANGEOVER_RELATED  = "EquipChangeOverRelated";
        private static  string SECURITY_STATUS_QUAL_STR_SUB_PENNY_TRADING_RELATED     = "SubPennyTradingRelated";
        private static  string SECURITY_STATUS_QUAL_STR_IMBALANCE_PREOPEN_BUY         = "ImbPreOpenBuy";
        private static  string SECURITY_STATUS_QUAL_STR_IMBALANCE_PREOPEN_SELL        = "ImbPreOpenSell";
        private static  string SECURITY_STATUS_QUAL_STR_IMBALANCE_PRECLOSE_BUY        = "ImbPreCloseBuy";
        private static  string SECURITY_STATUS_QUAL_STR_IMBALANCE_PRECLOSE_SELL       = "ImbPreCloseSell";
        private static  string SECURITY_STATUS_QUAL_STR_VOLATILITY_PAUSE              = "VolatilityPause";
        private static  string SECURITY_STATUS_QUAL_STR_VOLATILITY_PAUSE_QUOTE_RESUME = "VolatilityPauseQuoteResume";
        private static  string SECURITY_STATUS_QUAL_STR_VOLATILITY_GUARD              = "VolatilityGuard";
        private static  string SECURITY_STATUS_QUAL_STR_VOLATILITY_GUARD_QUOTE_RESUME = "VolatilityGuardQuoteResume";
		private static string SECURITY_STATUS_QUAL_STR_UNKNOWN                        = "Unknown";

		public string toString (MamdaSecurityStatusQual.mamdaSecurityStatusQual securityStatusQual)
		{
			switch (securityStatusQual)
			{
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NONE:
					return SECURITY_STATUS_QUAL_STR_NONE;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_OPENING:
                    return SECURITY_STATUS_QUAL_STR_OPENING;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_EXCUSED:
					return SECURITY_STATUS_QUAL_STR_EXCUSED;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_WITHDRAWN:
					return SECURITY_STATUS_QUAL_STR_WITHDRAWN;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SUSPENDED:
					return SECURITY_STATUS_QUAL_STR_SUSPENDED;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RESUME:
					return SECURITY_STATUS_QUAL_STR_RESUME;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_QUOTE_RESUME:
					return SECURITY_STATUS_QUAL_STR_QUOTE_RESUME;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_TRADE_RESUME:
					return SECURITY_STATUS_QUAL_STR_TRADE_RESUME;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RESUME_TIME:
					return SECURITY_STATUS_QUAL_STR_RESUME_TIME;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_MKT_IMB_BUY:
					return SECURITY_STATUS_QUAL_STR_MKT_IMB_BUY;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_MKT_IMB_SELL:
					return SECURITY_STATUS_QUAL_STR_MKT_IMB_SELL;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_MKT_IMB:
					return SECURITY_STATUS_QUAL_STR_NO_MKT_IMB;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_MOC_IMB_BUY:
					return SECURITY_STATUS_QUAL_STR_MOC_IMB_BUY;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_MOC_IMB_SELL:
					return SECURITY_STATUS_QUAL_STR_MOC_IMB_SELL;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_MOC_IMB:
					return SECURITY_STATUS_QUAL_STR_NO_MOC_IMB;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_IMB:
					return SECURITY_STATUS_QUAL_STR_ORDER_IMB;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_INF:
					return SECURITY_STATUS_QUAL_STR_ORDER_INF;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_IMB_BUY:
					return SECURITY_STATUS_QUAL_STR_ORDER_IMB_BUY;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_IMB_SELL:
					return SECURITY_STATUS_QUAL_STR_ORDER_IMB_SELL;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_IMB_NONE:
					return SECURITY_STATUS_QUAL_STR_ORDER_IMB_NONE;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_LOA_IMBALANCE_BUY:
                    return SECURITY_STATUS_QUAL_STR_LOA_IMBALANCE_BUY;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_LOA_IMBALANCE_SELL:
                    return SECURITY_STATUS_QUAL_STR_LOA_IMBALANCE_SELL;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_LOA_IMBALANCE:
                    return SECURITY_STATUS_QUAL_STR_NO_LOA_IMBALANCE;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDERS_ELIMINATED:
                    return SECURITY_STATUS_QUAL_STR_ORDERS_ELIMINATED;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RANGE_ID:
					return SECURITY_STATUS_QUAL_STR_RANGE_ID;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ITS_PREOPEN:
					return SECURITY_STATUS_QUAL_STR_ITS_PREOPEN;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RESERVED:
					return SECURITY_STATUS_QUAL_STR_RESERVED;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_FROZEN:
					return SECURITY_STATUS_QUAL_STR_FROZEN;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PREOPEN:
					return SECURITY_STATUS_QUAL_STR_PREOPEN;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_THO_IMBALANCE_BUY:
                    return SECURITY_STATUS_QUAL_STR_THO_IMBALANCE_BUY;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_THO_IMBALANCE_SELL:
                    return SECURITY_STATUS_QUAL_STR_THO_IMBALANCE_SELL;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_THO_IMBALANCE:
                    return SECURITY_STATUS_QUAL_STR_NO_THO_IMBALANCE;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ADD_INFO:
					return SECURITY_STATUS_QUAL_STR_ADD_INFO;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IPO_IMBALANCE_BUY:
                    return SECURITY_STATUS_QUAL_STR_IPO_IMBALANCE_BUY;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IPO_IMBALANCE_SELL:
                    return SECURITY_STATUS_QUAL_STR_IPO_IMBALANCE_SELL;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_IPO_IMBALANCE:
                    return SECURITY_STATUS_QUAL_STR_NO_IPO_IMBALANCE;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_EMC_IMB_BUY:
                    return SECURITY_STATUS_QUAL_STR_EMC_IMB_BUY;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_EMC_IMB_SELL:
                    return SECURITY_STATUS_QUAL_STR_EMC_IMB_SELL;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_EMC_IMB_NONE:
                    return SECURITY_STATUS_QUAL_STR_EMC_IMB_NONE;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_OPEN_DELAY:
					return SECURITY_STATUS_QUAL_STR_OPEN_DELAY;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME:
					return SECURITY_STATUS_QUAL_STR_NO_OPEN_NO_RESUME;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PRICE_IND:
					return SECURITY_STATUS_QUAL_STR_PRICE_IND;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_EQUIPMENT:
					return SECURITY_STATUS_QUAL_STR_EQUIPMENT;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_FILINGS:
					return SECURITY_STATUS_QUAL_STR_FILINGS;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NEWS:
					return SECURITY_STATUS_QUAL_STR_NEWS;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NEWS_DISSEM:
					return SECURITY_STATUS_QUAL_STR_NEWS_DISSEM;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_LISTING:
					return SECURITY_STATUS_QUAL_STR_LISTING;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_OPERATION:
					return SECURITY_STATUS_QUAL_STR_OPERATION;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_INFO:
					return SECURITY_STATUS_QUAL_STR_INFO;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SEC:
					return SECURITY_STATUS_QUAL_STR_SEC;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_TIMES:
					return SECURITY_STATUS_QUAL_STR_TIMES;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_OTHER:
					return SECURITY_STATUS_QUAL_STR_OTHER;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RELATED:
					return SECURITY_STATUS_QUAL_STR_RELATED;
				case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IPO:
					return SECURITY_STATUS_QUAL_STR_IPO; 
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PRE_CROSS:                  
                    return SECURITY_STATUS_QUAL_STR_PRE_CROSS;        
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_CROSS:                      
                    return SECURITY_STATUS_QUAL_STR_CROSS;             
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RELEASED_FOR_QUOTATION:     
                    return SECURITY_STATUS_QUAL_STR_RELEASED_FOR_QUOTATION; 
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IPO_WINDOW_EXT:             
                    return SECURITY_STATUS_QUAL_STR_IPO_WINDOW_EXT;         
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PRECLOSING:                 
                    return SECURITY_STATUS_QUAL_STR_PRECLOSING;             
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_AUCTION_EXTENSION:          
                    return SECURITY_STATUS_QUAL_STR_AUCTION_EXTENSION;      
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_VOLATILITY_AUCTION:         
                    return SECURITY_STATUS_QUAL_STR_VOLATILITY_AUCTION;     
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SECURITY_AUTHORISED:        
                    return SECURITY_STATUS_QUAL_STR_SECURITY_AUTHORISED;    
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SECURITY_FORBIDDEN:         
                    return SECURITY_STATUS_QUAL_STR_SECURITY_FORBIDDEN;     
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_FAST_MARKET:                
                    return SECURITY_STATUS_QUAL_STR_FAST_MARKET;            
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SLOW_MARKET:                
                    return SECURITY_STATUS_QUAL_STR_SLOW_MARKET;            
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SUM_PENNY_TRADING:          
                    return SECURITY_STATUS_QUAL_STR_SUM_PENNY_TRADING;      
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_INPUT:                
                    return SECURITY_STATUS_QUAL_STR_ORDER_INPUT;            
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PRE_ORDER_MATCHING:         
                    return SECURITY_STATUS_QUAL_STR_PRE_ORDER_MATCHING;     
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_MATCHING:             
                    return SECURITY_STATUS_QUAL_STR_ORDER_MATCHING;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_BLOCKING:              
                    return SECURITY_STATUS_QUAL_STR_BLOCKING;      
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_CANCEL:          
                    return SECURITY_STATUS_QUAL_STR_ORDER_CANCEL;  
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_FIXED_PRICE:           
                    return SECURITY_STATUS_QUAL_STR_FIXED_PRICE;   
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SALES_INPUT:           
                    return SECURITY_STATUS_QUAL_STR_SALES_INPUT;   
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_EXCHANGE_INTERVENTION: 
                    return SECURITY_STATUS_QUAL_STR_EXCHANGE_INTERVENTION;  
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PRE_AUCTION:                
                    return SECURITY_STATUS_QUAL_STR_PRE_AUCTION; 
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ADJUST:              
                    return SECURITY_STATUS_QUAL_STR_ADJUST;      
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ADJUST_ON:           
                    return SECURITY_STATUS_QUAL_STR_ADJUST_ON;   
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_LATE_TRADING:        
                    return SECURITY_STATUS_QUAL_STR_LATE_TRADING;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ENQUIRE:             
                    return SECURITY_STATUS_QUAL_STR_ENQUIRE;     
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PRE_NIGHT_TRADING:   
                    return SECURITY_STATUS_QUAL_STR_PRE_NIGHT_TRADING; 
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_OPEN_NIGHT_TRADING:        
                    return SECURITY_STATUS_QUAL_STR_OPEN_NIGHT_TRADING;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SUB_HOLIDAY:        
                    return  SECURITY_STATUS_QUAL_STR_SUB_HOLIDAY;

                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_BID_ONLY:             
                    return SECURITY_STATUS_QUAL_STR_BID_ONLY;     
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ASK_ONLY:        
                    return SECURITY_STATUS_QUAL_STR_ASK_ONLY;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_UNKNOWN:
                    return SECURITY_STATUS_QUAL_STR_UNKNOWN;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_OPENING_DELAY_COMMON:
                    return SECURITY_STATUS_QUAL_STR_OPENING_DELAY_COMMON;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RESUME_COMMON:
                    return SECURITY_STATUS_QUAL_STR_RESUME_COMMON;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME_COMMON:
                    return SECURITY_STATUS_QUAL_STR_NO_OPEN_NO_RESUME_COMMON;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NEWS_DISSEMINATION_RELATED:
                    return SECURITY_STATUS_QUAL_STR_NEWS_DISSEMINATION_RELATED;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_INFLUX_RELATED:
                    return SECURITY_STATUS_QUAL_STR_ORDER_INFLUX_RELATED;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_IMBALANCE_RELATED:
                    return SECURITY_STATUS_QUAL_STR_ORDER_IMBALANCE_RELATED;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_INFORMATION_REQUESTED_RELATED:
                    return SECURITY_STATUS_QUAL_STR_INFORMATION_REQUESTED_RELATED;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NEWS_PENDING_RELATED:
                    return SECURITY_STATUS_QUAL_STR_NEWS_PENDING_RELATED;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_EQUIPMENT_CHANGEOVER_RELATED:
                    return SECURITY_STATUS_QUAL_STR_EQUIPMENT_CHANGEOVER_RELATED;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SUB_PENNY_TRADING_RELATED:
                    return SECURITY_STATUS_QUAL_STR_SUB_PENNY_TRADING_RELATED;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IMBALANCE_PREOPEN_BUY:
                    return SECURITY_STATUS_QUAL_STR_IMBALANCE_PREOPEN_BUY;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IMBALANCE_PREOPEN_SELL:
                    return  SECURITY_STATUS_QUAL_STR_IMBALANCE_PREOPEN_SELL;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IMBALANCE_PRECLOSE_BUY:
                    return  SECURITY_STATUS_QUAL_STR_IMBALANCE_PRECLOSE_BUY;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IMBALANCE_PRECLOSE_SELL:
                    return  SECURITY_STATUS_QUAL_STR_IMBALANCE_PRECLOSE_SELL;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_VOLATILITY_PAUSE:
                    return   SECURITY_STATUS_QUAL_STR_VOLATILITY_PAUSE;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_VOLATILITY_PAUSE_QUOTE_RESUME:
                    return  SECURITY_STATUS_QUAL_STR_VOLATILITY_PAUSE_QUOTE_RESUME;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_VOLATILITY_GUARD:
                    return   SECURITY_STATUS_QUAL_STR_VOLATILITY_GUARD;
                case mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_VOLATILITY_GUARD_QUOTE_RESUME:
                    return  SECURITY_STATUS_QUAL_STR_VOLATILITY_GUARD_QUOTE_RESUME;
			}
			return SECURITY_STATUS_QUAL_STR_UNKNOWN;
		}
		
		public static MamdaSecurityStatusQual.mamdaSecurityStatusQual mamdaSecurityStatusQualFromString (string securityStatusQual)
		{
			if (securityStatusQual == null)
			{
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_UNKNOWN;
			}

			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_NONE)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NONE;
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_OPENING )
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_OPENING;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_EXCUSED)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_EXCUSED;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_WITHDRAWN)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_WITHDRAWN;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_SUSPENDED)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SUSPENDED;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_RESUME)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RESUME;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_QUOTE_RESUME)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_QUOTE_RESUME;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_TRADE_RESUME)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_TRADE_RESUME;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_RESUME_TIME)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RESUME_TIME;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_MKT_IMB_BUY)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_MKT_IMB_BUY;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_MKT_IMB_SELL)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_MKT_IMB_SELL;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_NO_MKT_IMB)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_MKT_IMB;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_MOC_IMB_BUY)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_MOC_IMB_BUY;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_MOC_IMB_SELL)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_MOC_IMB_SELL;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_NO_MOC_IMB)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_MOC_IMB;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_ORDER_IMB)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_IMB;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_ORDER_INF)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_INF;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_ORDER_IMB_BUY)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_IMB_BUY;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_ORDER_IMB_SELL)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_IMB_SELL;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_ORDER_IMB_NONE)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_IMB_NONE;
            if (securityStatusQual ==  SECURITY_STATUS_QUAL_STR_LOA_IMBALANCE_BUY  )
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_LOA_IMBALANCE_BUY;
            if (securityStatusQual ==  SECURITY_STATUS_QUAL_STR_LOA_IMBALANCE_SELL)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_LOA_IMBALANCE_SELL;
            if (securityStatusQual ==  SECURITY_STATUS_QUAL_STR_NO_LOA_IMBALANCE)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_LOA_IMBALANCE;
            if (securityStatusQual ==   SECURITY_STATUS_QUAL_STR_ORDERS_ELIMINATED)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDERS_ELIMINATED;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_RANGE_ID)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RANGE_ID;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_ITS_PREOPEN)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ITS_PREOPEN;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_RESERVED)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RESERVED;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_FROZEN)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_FROZEN;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_PREOPEN)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PREOPEN;
            if (securityStatusQual ==  SECURITY_STATUS_QUAL_STR_THO_IMBALANCE_BUY)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_THO_IMBALANCE_BUY;
            if (securityStatusQual ==  SECURITY_STATUS_QUAL_STR_THO_IMBALANCE_SELL)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_THO_IMBALANCE_SELL;
            if (securityStatusQual ==  SECURITY_STATUS_QUAL_STR_NO_THO_IMBALANCE)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_THO_IMBALANCE;

			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_ADD_INFO)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ADD_INFO;
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_IPO_IMBALANCE_BUY)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IPO_IMBALANCE_BUY;
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_IPO_IMBALANCE_SELL)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IPO_IMBALANCE_SELL;
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_NO_IPO_IMBALANCE)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_IPO_IMBALANCE;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_OPEN_DELAY)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_OPEN_DELAY;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_NO_OPEN_NO_RESUME)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_PRICE_IND)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PRICE_IND;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_EQUIPMENT)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_EQUIPMENT;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_FILINGS)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_FILINGS;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_NEWS)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NEWS;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_NEWS_DISSEM)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NEWS_DISSEM;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_LISTING)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_LISTING;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_OPERATION)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_OPERATION;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_INFO)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_INFO;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_SEC)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SEC;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_TIMES)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_TIMES;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_OTHER)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_OTHER;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_RELATED)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RELATED;
			if (securityStatusQual == SECURITY_STATUS_QUAL_STR_IPO)
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IPO;   
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_PRE_CROSS)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PRE_CROSS;   
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_CROSS)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_CROSS;   
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_RELEASED_FOR_QUOTATION)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RELEASED_FOR_QUOTATION;   
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_IPO_WINDOW_EXT)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IPO_WINDOW_EXT;   
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_PRECLOSING)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PRECLOSING;   
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_AUCTION_EXTENSION )
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_AUCTION_EXTENSION;   
            if (securityStatusQual ==  SECURITY_STATUS_QUAL_STR_VOLATILITY_AUCTION)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_VOLATILITY_AUCTION;   
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_SECURITY_AUTHORISED )
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SECURITY_AUTHORISED;   
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_SECURITY_FORBIDDEN)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SECURITY_FORBIDDEN;   
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_FAST_MARKET)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_FAST_MARKET;   
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_SLOW_MARKET)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SLOW_MARKET;   
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_SUM_PENNY_TRADING)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SUM_PENNY_TRADING;   
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_ORDER_INPUT)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_INPUT;   
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_PRE_ORDER_MATCHING)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PRE_ORDER_MATCHING;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_ORDER_MATCHING)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_MATCHING;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_BLOCKING)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_BLOCKING;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_ORDER_CANCEL)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_CANCEL;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_FIXED_PRICE)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_FIXED_PRICE;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_SALES_INPUT)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SALES_INPUT;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_EXCHANGE_INTERVENTION)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_EXCHANGE_INTERVENTION;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_PRE_AUCTION)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PRE_AUCTION;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_ADJUST)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ADJUST;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_ADJUST_ON)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ADJUST_ON;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_LATE_TRADING)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_LATE_TRADING;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_ENQUIRE)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ENQUIRE;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_PRE_NIGHT_TRADING)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PRE_NIGHT_TRADING;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_OPEN_NIGHT_TRADING)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_OPEN_NIGHT_TRADING;
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_SUB_HOLIDAY)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SUB_HOLIDAY;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_BID_ONLY)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_BID_ONLY;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_ASK_ONLY)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ASK_ONLY;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_OPENING_DELAY_COMMON)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_OPENING_DELAY_COMMON;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_RESUME_COMMON)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RESUME_COMMON;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_NO_OPEN_NO_RESUME_COMMON)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME_COMMON;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_NEWS_DISSEMINATION_RELATED)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NEWS_DISSEMINATION_RELATED;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_ORDER_INFLUX_RELATED)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_INFLUX_RELATED;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_ORDER_IMBALANCE_RELATED)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_IMBALANCE_RELATED;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_INFORMATION_REQUESTED_RELATED)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_INFORMATION_REQUESTED_RELATED;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_NEWS_PENDING_RELATED )
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NEWS_PENDING_RELATED;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_EQUIPMENT_CHANGEOVER_RELATED)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_EQUIPMENT_CHANGEOVER_RELATED;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_SUB_PENNY_TRADING_RELATED)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SUB_PENNY_TRADING_RELATED;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_IMBALANCE_PREOPEN_BUY)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IMBALANCE_PREOPEN_BUY;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_IMBALANCE_PREOPEN_SELL)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IMBALANCE_PREOPEN_SELL;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_IMBALANCE_PRECLOSE_BUY)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IMBALANCE_PRECLOSE_BUY;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_IMBALANCE_PRECLOSE_SELL)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IMBALANCE_PRECLOSE_SELL;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_VOLATILITY_PAUSE)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_VOLATILITY_PAUSE;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_VOLATILITY_PAUSE_QUOTE_RESUME)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_VOLATILITY_PAUSE_QUOTE_RESUME;
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_VOLATILITY_GUARD)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_VOLATILITY_GUARD;  
            if (securityStatusQual == SECURITY_STATUS_QUAL_STR_VOLATILITY_GUARD_QUOTE_RESUME)
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_VOLATILITY_GUARD_QUOTE_RESUME;  

			if (securityStatusQual == "0")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NONE;
            if (securityStatusQual == "1")
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_OPENING;
			if (securityStatusQual == "7")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_EXCUSED;
			if (securityStatusQual == "8")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_WITHDRAWN;
			if (securityStatusQual == "9")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SUSPENDED;
			if (securityStatusQual == "11")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RESUME;
			if (securityStatusQual == "12")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_QUOTE_RESUME;
			if (securityStatusQual == "13")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_TRADE_RESUME;
			if (securityStatusQual == "14")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RESUME_TIME;
			if (securityStatusQual == "16")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_MKT_IMB_BUY;
			if (securityStatusQual == "17")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_MKT_IMB_SELL;
			if (securityStatusQual == "18")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_MKT_IMB;
			if (securityStatusQual == "19")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_MOC_IMB_BUY;
			if (securityStatusQual == "20")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_MOC_IMB_SELL;
			if (securityStatusQual == "21")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_MOC_IMB;
			if (securityStatusQual == "22")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_IMB;
			if (securityStatusQual == "23")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_INF;
			if (securityStatusQual == "24")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_IMB_BUY;
			if (securityStatusQual == "25")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_IMB_SELL;
			if (securityStatusQual == "26")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_IMB_NONE;
            if (securityStatusQual == "27")
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_LOA_IMBALANCE_BUY;
            if (securityStatusQual == "28")
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_LOA_IMBALANCE_SELL;
            if (securityStatusQual == "29")
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_LOA_IMBALANCE;
            if (securityStatusQual == "30")
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDERS_ELIMINATED;
			if (securityStatusQual == "31")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RANGE_ID;
			if (securityStatusQual == "32")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ITS_PREOPEN;
			if (securityStatusQual == "33")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RESERVED;
			if (securityStatusQual == "34")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_FROZEN;	
			if (securityStatusQual == "35")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PREOPEN;
            if (securityStatusQual == "36")
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_THO_IMBALANCE_BUY;
            if (securityStatusQual == "37")
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_THO_IMBALANCE_SELL;
            if (securityStatusQual == "38")
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_THO_IMBALANCE;
			if (securityStatusQual == "41")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ADD_INFO;
            if (securityStatusQual == "42")
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IPO_IMBALANCE_BUY;
            if (securityStatusQual == "43")
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IPO_IMBALANCE_SELL;
            if (securityStatusQual == "44")
                return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_IPO_IMBALANCE;
			if (securityStatusQual == "51")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_OPEN_DELAY;
			if (securityStatusQual == "52")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME;
			if (securityStatusQual == "53")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PRICE_IND;
			if (securityStatusQual == "54")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_EQUIPMENT;
			if (securityStatusQual == "55")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_FILINGS;
			if (securityStatusQual == "56")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NEWS;
			if (securityStatusQual == "57")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NEWS_DISSEM;
			if (securityStatusQual == "58")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_LISTING;
			if (securityStatusQual == "59")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_OPERATION;
			if (securityStatusQual == "60")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_INFO;
			if (securityStatusQual == "61")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SEC;
			if (securityStatusQual == "62")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_TIMES;
			if (securityStatusQual == "62")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_OTHER;
			if (securityStatusQual == "64")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RELATED;
			if (securityStatusQual == "65")
				return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IPO;
            
            if (securityStatusQual == "46")               
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_EMC_IMB_BUY;           
            if (securityStatusQual == "47")              
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_EMC_IMB_SELL;          
            if (securityStatusQual == "48")              
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_EMC_IMB_NONE;          
            if (securityStatusQual == "66")                 
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PRE_CROSS;             
            if (securityStatusQual == "67")                     
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_CROSS;                 
            if (securityStatusQual == "68")    
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RELEASED_FOR_QUOTATION;
            if (securityStatusQual == "69")            
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IPO_WINDOW_EXT;        
            if (securityStatusQual == "70")                
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PRECLOSING;            
            if (securityStatusQual == "71")         
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_AUCTION_EXTENSION;     
            if (securityStatusQual == "72")        
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_VOLATILITY_AUCTION;    
            if (securityStatusQual == "73")       
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SECURITY_AUTHORISED;   
            if (securityStatusQual == "74")        
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SECURITY_FORBIDDEN;    
            if (securityStatusQual == "75")               
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_FAST_MARKET;           
            if (securityStatusQual == "76")           
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SLOW_MARKET;           
            if (securityStatusQual == "77")     
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SUM_PENNY_TRADING;     
            if (securityStatusQual == "78")           
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_INPUT;           
            if (securityStatusQual == "79")    
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PRE_ORDER_MATCHING;    
            if (securityStatusQual == "80")        
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_MATCHING;        
            if (securityStatusQual == "81")              
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_BLOCKING;              
            if (securityStatusQual == "82")          
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_CANCEL;          
            if (securityStatusQual == "83")           
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_FIXED_PRICE;           
            if (securityStatusQual == "84")           
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SALES_INPUT;           
            if (securityStatusQual == "85") 
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_EXCHANGE_INTERVENTION; 
            if (securityStatusQual == "86")           
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PRE_AUCTION;           
            if (securityStatusQual == "87")                
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ADJUST;                
            if (securityStatusQual == "88")             
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ADJUST_ON;             
            if (securityStatusQual == "89")          
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_LATE_TRADING;          
            if (securityStatusQual == "90")               
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ENQUIRE;               
            if (securityStatusQual == "91")     
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_PRE_NIGHT_TRADING;     
            if (securityStatusQual == "92")    
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_OPEN_NIGHT_TRADING;    
            if (securityStatusQual == "93")    
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SUB_HOLIDAY;    
            if (securityStatusQual == "94")              
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_BID_ONLY;              
            if (securityStatusQual == "95")         
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ASK_ONLY;      
            if (securityStatusQual == "101")         
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_OPENING_DELAY_COMMON;      
            if (securityStatusQual == "102")         
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_RESUME_COMMON;
            if (securityStatusQual == "103")         
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME_COMMON;
            if (securityStatusQual == "104")         
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NEWS_DISSEMINATION_RELATED;
            if (securityStatusQual == "105")         
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_INFLUX_RELATED;
            if (securityStatusQual == "106")         
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_ORDER_IMBALANCE_RELATED;
            if (securityStatusQual == "107")         
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_INFORMATION_REQUESTED_RELATED;
            if (securityStatusQual == "108")         
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_NEWS_PENDING_RELATED;
            if (securityStatusQual == "109")         
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_EQUIPMENT_CHANGEOVER_RELATED;
            if (securityStatusQual == "110")         
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_SUB_PENNY_TRADING_RELATED;
            if (securityStatusQual == "112")         
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IMBALANCE_PREOPEN_BUY;
            if (securityStatusQual == "113")         
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IMBALANCE_PREOPEN_SELL;
            if (securityStatusQual == "114")         
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IMBALANCE_PRECLOSE_BUY;
            if (securityStatusQual == "115")         
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_IMBALANCE_PRECLOSE_SELL;
            if (securityStatusQual == "116")         
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_VOLATILITY_PAUSE;
            if (securityStatusQual == "117")         
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_VOLATILITY_PAUSE_QUOTE_RESUME;
            if (securityStatusQual == "118")         
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_VOLATILITY_GUARD;
            if (securityStatusQual == "119")         
              return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_VOLATILITY_GUARD_QUOTE_RESUME;

			return mamdaSecurityStatusQual.SECURITY_STATUS_QUAL_UNKNOWN;
		}
	}
}
