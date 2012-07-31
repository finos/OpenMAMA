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

public class MamdaSecurityStatusQual
{
    public static final short SECURITY_STATUS_QUAL_NONE                          = 0;    // toString()  None
    public static final short SECURITY_STATUS_QUAL_OPENING                       = 1;    // toString()  Opening
    public static final short SECURITY_STATUS_QUAL_EXCUSED                       = 7;    // toString()  Excused 
    public static final short SECURITY_STATUS_QUAL_WITHDRAWN                     = 8;    // toString()  Withdrawn 
    public static final short SECURITY_STATUS_QUAL_SUSPENDED                     = 9;    // toString()  Suspended
    public static final short SECURITY_STATUS_QUAL_RESUME                        = 11;   // toString()  Resume
    public static final short SECURITY_STATUS_QUAL_QUOTE_RESUME                  = 12;   // toString()  QuoteResume
    public static final short SECURITY_STATUS_QUAL_TRADE_RESUME                  = 13;   // toString()  TradeResume
    public static final short SECURITY_STATUS_QUAL_RESUME_TIME                   = 14;   // toString()  ResumeTime  
    public static final short SECURITY_STATUS_QUAL_MKT_IMB_BUY                   = 16;   // toString()  MktImbBuy 
    public static final short SECURITY_STATUS_QUAL_MKT_IMB_SELL                  = 17;   // toString()  MktImbSell 
    public static final short SECURITY_STATUS_QUAL_NO_MKT_IMB                    = 18;   // toString()  NoMktImb
    public static final short SECURITY_STATUS_QUAL_MOC_IMB_BUY                   = 19;   // toString()  MocImbBuy
    public static final short SECURITY_STATUS_QUAL_MOC_IMB_SELL                  = 20;   // toString()  MocImbSell
    public static final short SECURITY_STATUS_QUAL_NO_MOC_IMB                    = 21;   // toString()  NoMocImb
    public static final short SECURITY_STATUS_QUAL_ORDER_IMB                     = 22;   // toString()  OrderImb  
    public static final short SECURITY_STATUS_QUAL_ORDER_INF                     = 23;   // toString()  OrderInf 
    public static final short SECURITY_STATUS_QUAL_ORDER_IMB_BUY                 = 24;   // toString()  OrderImbBuy 
    public static final short SECURITY_STATUS_QUAL_ORDER_IMB_SELL                = 25;   // toString()  OrderImbSell
    public static final short SECURITY_STATUS_QUAL_ORDER_IMB_NONE                = 26;   // toString()  OrderImbNone
    public static final short SECURITY_STATUS_QUAL_LOA_IMBALANCE_BUY             = 27;   // toString()  LoaImbBuy
    public static final short SECURITY_STATUS_QUAL_LOA_IMBALANCE_SELL            = 28;   // toString()  LoaImbSell
    public static final short SECURITY_STATUS_QUAL_NO_LOA_IMBALANCE              = 29;   // toString()  LoaNoImb
    public static final short SECURITY_STATUS_QUAL_ORDERS_ELMINATED              = 30;   // toString()  OrdersEliminated
    public static final short SECURITY_STATUS_QUAL_RANGE_ID                      = 31;   // toString()  RangeInd
    public static final short SECURITY_STATUS_QUAL_ITS_PREOPEN                   = 32;   // toString()  ItsPreOpen
    public static final short SECURITY_STATUS_QUAL_RESERVED                      = 33;   // toString()  Reserved  
    public static final short SECURITY_STATUS_QUAL_FROZEN                        = 34;   // toString()  Frozen 
    public static final short SECURITY_STATUS_QUAL_PREOPEN                       = 35;   // toString()  PreOpen 
    public static final short SECURITY_STATUS_QUAL_THO_IMBALANCE_BUY             = 36;   // toString()  ThoImbBuy
    public static final short SECURITY_STATUS_QUAL_THO_IMBALANCE_SELL            = 37;   // toString()  ThoImbSell
    public static final short SECURITY_STATUS_QUAL_NO_THO_IMBALANCE              = 38;   // toString()  ThoNoImb
    public static final short SECURITY_STATUS_QUAL_ADD_INFO                      = 41;   // toString()  AddInfo
    public static final short SECURITY_STATUS_QUAL_IPO_IMBALANCE_BUY             = 42;   // toString()  IpoImbBuy
    public static final short SECURITY_STATUS_QUAL_IPO_IMBALANCE_SELL            = 43;   // toString()  IpoImbSell
    public static final short SECURITY_STATUS_QUAL_NO_IPO_IMBALANCE              = 44;   // toString()  IpoNoImb
    public static final short SECURITY_STATUS_QUAL_EMC_IMB_BUY                   = 46;   // toString()  EmcImbBuy
    public static final short SECURITY_STATUS_QUAL_EMC_IMB_SELL                  = 47;   // toString()  EmcImbSell
    public static final short SECURITY_STATUS_QUAL_EMC_IMB_NONE                  = 48;   // toString()  EmcImbNone
    public static final short SECURITY_STATUS_QUAL_OPEN_DELAY                    = 51;   // toString()  OpenDelay
    public static final short SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME             = 52;   // toString()  NoOpenNoResume
    public static final short SECURITY_STATUS_QUAL_PRICE_IND                     = 53;   // toString()  PriceInd
    public static final short SECURITY_STATUS_QUAL_EQUIPMENT                     = 54;   // toString()  Equipment  
    public static final short SECURITY_STATUS_QUAL_FILINGS                       = 55;   // toString()  Filings 
    public static final short SECURITY_STATUS_QUAL_NEWS                          = 56;   // toString()  News 
    public static final short SECURITY_STATUS_QUAL_NEWS_DISSEM                   = 57;   // toString()  NewsDissem
    public static final short SECURITY_STATUS_QUAL_LISTING                       = 58;   // toString()  Listing
    public static final short SECURITY_STATUS_QUAL_OPERATION                     = 59;   // toString()  Operation
    public static final short SECURITY_STATUS_QUAL_INFO                          = 60;   // toString()  Info
    public static final short SECURITY_STATUS_QUAL_SEC                           = 61;   // toString()  SEC  
    public static final short SECURITY_STATUS_QUAL_TIMES                         = 62;   // toString()  Times 
    public static final short SECURITY_STATUS_QUAL_OTHER                         = 63;   // toString()  Other 
    public static final short SECURITY_STATUS_QUAL_RELATED                       = 64;   // toString()  Related
    public static final short SECURITY_STATUS_QUAL_IPO                           = 65;   // toString()  IPO
    public static final short SECURITY_STATUS_QUAL_PRE_CROSS                     = 66;   // toString()  Pre-Cross
    public static final short SECURITY_STATUS_QUAL_CROSS                         = 67;   // toString()  Cross
    public static final short SECURITY_STATUS_QUAL_RELEASED_FOR_QUOTATION        = 68;   // toString()  Released For Quotation (IPO) 
    public static final short SECURITY_STATUS_QUAL_IPO_WINDOW_EXT                = 69;   // toString()  IPO Window Extension
    public static final short SECURITY_STATUS_QUAL_PRECLOSING                    = 70;   // toString()  PreClosing
    public static final short SECURITY_STATUS_QUAL_AUCTION_EXTENSION             = 71;   // toString()  Auction Extension
    public static final short SECURITY_STATUS_QUAL_VOLATILITY_AUCTION            = 72;   // toString()  Volatility Auction
    public static final short SECURITY_STATUS_QUAL_SECURITY_AUTHORISED           = 73;   // toString()  SecurityAuthorized
    public static final short SECURITY_STATUS_QUAL_SECURITY_FORBIDDEN            = 74;   // toString()  SecurityForbidden
    public static final short SECURITY_STATUS_QUAL_FAST_MARKET                   = 75;   // toString()  FastMarket
    public static final short SECURITY_STATUS_QUAL_SLOW_MARKET                   = 76;   // toString()  SlowMarket
    public static final short SECURITY_STATUS_QUAL_SUB_PENNY_TRADING             = 77;   // toString()  SubPennyTrading
    public static final short SECURITY_STATUS_QUAL_ORDER_INPUT                   = 78;   // toString()  OrderInput
    public static final short SECURITY_STATUS_QUAL_PRE_ORDER_MATCHING            = 79;   // toString()  PreOrderMatching
    public static final short SECURITY_STATUS_QUAL_ORDER_MATCHING                = 80;   // toString()  OrderMatching
    public static final short SECURITY_STATUS_QUAL_BLOCKING                      = 81;   // toString()  Blocking
    public static final short SECURITY_STATUS_QUAL_ORDER_CANCEL                  = 82;   // toString()  OrderCancel
    public static final short SECURITY_STATUS_QUAL_FIXED_PRICE                   = 83;   // toString()  FixedPrice
    public static final short SECURITY_STATUS_QUAL_SALES_INPUT                   = 84;   // toString()  SalesInput
    public static final short SECURITY_STATUS_QUAL_EXCHANGE_INTERVENTION         = 85;   // toString()  ExchangeIntervention
    public static final short SECURITY_STATUS_QUAL_PRE_AUCTION                   = 86;   // toString()  PreAuction
    public static final short SECURITY_STATUS_QUAL_ADJUST                        = 87;   // toString()  Adjust
    public static final short SECURITY_STATUS_QUAL_ADJUST_ON                     = 88;   // toString()  AdjustOn
    public static final short SECURITY_STATUS_QUAL_LATE_TRADING                  = 89;   // toString()  LateTrading
    public static final short SECURITY_STATUS_QUAL_ENQUIRE                       = 90;   // toString()  Enquire
    public static final short SECURITY_STATUS_QUAL_PRE_NIGHT_TRADING             = 91;   // toString()  PreNightTrading
    public static final short SECURITY_STATUS_QUAL_OPEN_NIGHT_TRADING            = 92;   // toString()  OpenNightTrading
    public static final short SECURITY_STATUS_QUAL_SUB_HOLIDAY                   = 93;   // toString()  SubHoliday
    public static final short SECURITY_STATUS_QUAL_BID_ONLY                      = 94;   // toString()  BidOnly
    public static final short SECURITY_STATUS_QUAL_ASK_ONLY                      = 95;   // toString()  AskOnly 
    public static final short SECURITY_STATUS_QUAL_UNKNOWN                       = 99;  
    public static final short SECURITY_STATUS_QUAL_OPENING_DELAY_COMMON          = 101;  // toString()  OpeningDelayCommon
    public static final short SECURITY_STATUS_QUAL_RESUME_COMMON                 = 102;  // toString()  ResumeCommon
    public static final short SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME_COMMON      = 103;  // toString()  NoOpenNoResumeCommon
    public static final short SECURITY_STATUS_QUAL_NEWS_DISSEMINATION_RELATED    = 104;  // toString()  NewsDisseminationRelated
    public static final short SECURITY_STATUS_QUAL_ORDER_INFLUX_RELATED          = 105;  // toString()  OrderInfluxRelated
    public static final short SECURITY_STATUS_QUAL_ORDER_IMBALANCE_RELATED       = 106;  // toString()  OrderImbRelated
    public static final short SECURITY_STATUS_QUAL_INFORMATION_REQUESTED_RELATED = 107;  // toString()  InfoRequestedRelated
    public static final short SECURITY_STATUS_QUAL_NEWS_PENDING_RELATED          = 108;  // toString()  NewsPendingRelated
    public static final short SECURITY_STATUS_QUAL_EQUIPMENT_CHANGEOVER_RELATED  = 109;  // toString()  EquipChangeOverRelated
    public static final short SECURITY_STATUS_QUAL_SUB_PENNY_TRADING_RELATED     = 110;  // toString()  SubPennyTradingRelated
    public static final short SECURITY_STATUS_QUAL_IMBALANCE_PREOPEN_BUY         = 112;  // toString()  ImbPreOpenBuy
    public static final short SECURITY_STATUS_QUAL_IMBALANCE_PREOPEN_SELL        = 113;  // toString()  ImbPreOpenSell
    public static final short SECURITY_STATUS_QUAL_IMBALANCE_PRECLOSE_BUY        = 114;  // toString()  ImbPreCloseBuy
    public static final short SECURITY_STATUS_QUAL_IMBALANCE_PRECLOSE_SELL       = 115;  // toString()  ImbPreCloseSell  
    public static final short SECURITY_STATUS_QUAL_VOLATILITY_PAUSE              = 116;  // toString()  VolatilityPause
    public static final short SECURITY_STATUS_QUAL_VOLATILITY_PAUSE_QUOTE_RESUME = 117;  // toString()  VolatilityPauseQuoteResume
    public static final short SECURITY_STATUS_QUAL_VOLATILITY_GUARD              = 118;  // toString()  VolatilityGuard
    public static final short SECURITY_STATUS_QUAL_VOLATILITY_GUARD_QUOTE_RESUME = 119;  // toString()  VolatilityGuardQuoteResume
    
    //Only used internally
    private static final String SECURITY_STATUS_QUAL_STR_NONE                               = "None";
    private static final String SECURITY_STATUS_QUAL_STR_OPENING                            = "Opening";
    private static final String SECURITY_STATUS_QUAL_STR_EXCUSED                            = "Excused";
    private static final String SECURITY_STATUS_QUAL_STR_WITHDRAWN                          = "Withdrawn";
    private static final String SECURITY_STATUS_QUAL_STR_SUSPENDED                          = "Suspended";
    private static final String SECURITY_STATUS_QUAL_STR_RESUME                             = "Resume";
    private static final String SECURITY_STATUS_QUAL_STR_QUOTE_RESUME                       = "QuoteResume";
    private static final String SECURITY_STATUS_QUAL_STR_TRADE_RESUME                       = "TradeResume";
    private static final String SECURITY_STATUS_QUAL_STR_RESUME_TIME                        = "ResumeTime";
    private static final String SECURITY_STATUS_QUAL_STR_MKT_IMB_BUY                        = "MktImbBuy";
    private static final String SECURITY_STATUS_QUAL_STR_MKT_IMB_SELL                       = "MktImbSell";
    private static final String SECURITY_STATUS_QUAL_STR_NO_MKT_IMB                         = "NoMktImb";
    private static final String SECURITY_STATUS_QUAL_STR_MOC_IMB_BUY                        = "MocImbBuy";
    private static final String SECURITY_STATUS_QUAL_STR_MOC_IMB_SELL                       = "MocImbSell";
    private static final String SECURITY_STATUS_QUAL_STR_NO_MOC_IMB                         = "NoMocImb";
    private static final String SECURITY_STATUS_QUAL_STR_ORDER_IMB                          = "OrderImb";
    private static final String SECURITY_STATUS_QUAL_STR_ORDER_INF                          = "OrderInf";
    private static final String SECURITY_STATUS_QUAL_STR_ORDER_IMB_BUY                      = "OrderImbBuy";
    private static final String SECURITY_STATUS_QUAL_STR_ORDER_IMB_SELL                     = "OrderImbSell";
    private static final String SECURITY_STATUS_QUAL_STR_ORDER_IMB_NONE                     = "OrderImbNone";
    private static final String SECURITY_STATUS_QUAL_STR_LOA_IMBALANCE_BUY                  = "LoaImbBuy";
    private static final String SECURITY_STATUS_QUAL_STR_LOA_IMBALANCE_SELL                 = "LoaImbSell";
    private static final String SECURITY_STATUS_QUAL_STR_NO_LOA_IMBALANCE                   = "NoLoaImb";
    private static final String SECURITY_STATUS_QUAL_STR_ORDERS_ELIMINATED                  = "OrdersElimated";
    private static final String SECURITY_STATUS_QUAL_STR_RANGE_ID                           = "RangeInd";
    private static final String SECURITY_STATUS_QUAL_STR_ITS_PREOPEN                        = "ItsPreOpen";
    private static final String SECURITY_STATUS_QUAL_STR_RESERVED                           = "Reserved";
    private static final String SECURITY_STATUS_QUAL_STR_FROZEN                             = "Frozen";
    private static final String SECURITY_STATUS_QUAL_STR_PREOPEN                            = "PreOpen";
    private static final String SECURITY_STATUS_QUAL_STR_THO_IMBALANCE_BUY                  = "ThoImbBuy";
    private static final String SECURITY_STATUS_QUAL_STR_THO_IMBALANCE_SELL                 = "ThoImbSell";
    private static final String SECURITY_STATUS_QUAL_STR_NO_THO_IMBALANCE                   = "NoThoImb";
    private static final String SECURITY_STATUS_QUAL_STR_ADD_INFO                           = "AddInfo";
    private static final String SECURITY_STATUS_QUAL_STR_IPO_IMBALANCE_BUY                  = "IpoImbBuy";
    private static final String SECURITY_STATUS_QUAL_STR_IPO_IMBALANCE_SELL                 = "IpoImbSell";
    private static final String SECURITY_STATUS_QUAL_STR_NO_IPO_IMBALANCE                   = "IpoNoImb";
    private static final String SECURITY_STATUS_QUAL_STR_OPEN_DELAY                         = "OpenDelay";
    private static final String SECURITY_STATUS_QUAL_STR_NO_OPEN_NO_RESUME                  = "NoOpenNoResume";
    private static final String SECURITY_STATUS_QUAL_STR_PRICE_IND                          = "PriceInd";
    private static final String SECURITY_STATUS_QUAL_STR_EQUIPMENT                          = "Equipment";
    private static final String SECURITY_STATUS_QUAL_STR_FILINGS                            = "Filings";
    private static final String SECURITY_STATUS_QUAL_STR_NEWS                               = "News";
    private static final String SECURITY_STATUS_QUAL_STR_NEWS_DISSEM                        = "NewsDissem";
    private static final String SECURITY_STATUS_QUAL_STR_LISTING                            = "Listing";
    private static final String SECURITY_STATUS_QUAL_STR_OPERATION                          = "Operation";
    private static final String SECURITY_STATUS_QUAL_STR_INFO                               = "Info";
    private static final String SECURITY_STATUS_QUAL_STR_SEC                                = "SEC";
    private static final String SECURITY_STATUS_QUAL_STR_TIMES                              = "Times";
    private static final String SECURITY_STATUS_QUAL_STR_OTHER                              = "Other";
    private static final String SECURITY_STATUS_QUAL_STR_RELATED                            = "Related";
    private static final String SECURITY_STATUS_QUAL_STR_IPO                                = "IPO";
    
    private static final String SECURITY_STATUS_QUAL_STR_EMC_IMB_BUY                        = "EmcImbBuy";
    private static final String SECURITY_STATUS_QUAL_STR_EMC_IMB_SELL                       = "EmcImbSell";
    private static final String SECURITY_STATUS_QUAL_STR_EMC_IMB_NONE                       = "EmcImbNone";
    private static final String SECURITY_STATUS_QUAL_STR_PRE_CROSS                          = "Pre-Cross";
    private static final String SECURITY_STATUS_QUAL_STR_CROSS                              = "Cross";
    private static final String SECURITY_STATUS_QUAL_STR_RELEASED_FOR_QUOTATION             = "Released For Quotation (IPO)"; 
    private static final String SECURITY_STATUS_QUAL_STR_IPO_WINDOW_EXT                     = "IPO Window Extension";
    private static final String SECURITY_STATUS_QUAL_STR_PRECLOSING                         = "PreClosing";
    private static final String SECURITY_STATUS_QUAL_STR_AUCTION_EXTENSION                  = "Auction Extension";
    private static final String SECURITY_STATUS_QUAL_STR_VOLATILITY_AUCTION                 = "Volatility Auction";
    private static final String SECURITY_STATUS_QUAL_STR_SECURITY_AUTHORISED                = "SecurityAuthorized";
    private static final String SECURITY_STATUS_QUAL_STR_SECURITY_FORBIDDEN                 = "SecurityForbidden";
    private static final String SECURITY_STATUS_QUAL_STR_FAST_MARKET                        = "FastMarket";
    private static final String SECURITY_STATUS_QUAL_STR_SLOW_MARKET                        = "SlowMarket";
    private static final String SECURITY_STATUS_QUAL_STR_SUB_PENNY_TRADING                  = "SubPennyTrading";
    private static final String SECURITY_STATUS_QUAL_STR_ORDER_INPUT                        = "OrderInput";
    private static final String SECURITY_STATUS_QUAL_STR_PRE_ORDER_MATCHING                 = "PreOrderMatching";
    private static final String SECURITY_STATUS_QUAL_STR_ORDER_MATCHING                     = "OrderMatching";
    private static final String SECURITY_STATUS_QUAL_STR_BLOCKING                           = "Blocking";
    private static final String SECURITY_STATUS_QUAL_STR_ORDER_CANCEL                       = "OrderCancel";
    private static final String SECURITY_STATUS_QUAL_STR_FIXED_PRICE                        = "FixedPrice";
    private static final String SECURITY_STATUS_QUAL_STR_SALES_INPUT                        = "SalesInput";
    private static final String SECURITY_STATUS_QUAL_STR_EXCHANGE_INTERVENTION              = "ExchangeIntervention";
    private static final String SECURITY_STATUS_QUAL_STR_PRE_AUCTION                        = "PreAuction";
    private static final String SECURITY_STATUS_QUAL_STR_ADJUST                             = "Adjust";
    private static final String SECURITY_STATUS_QUAL_STR_ADJUST_ON                          = "AdjustOn";
    private static final String SECURITY_STATUS_QUAL_STR_LATE_TRADING                       = "LateTrading";
    private static final String SECURITY_STATUS_QUAL_STR_ENQUIRE                            = "Enquire";
    private static final String SECURITY_STATUS_QUAL_STR_PRE_NIGHT_TRADING                  = "PreNightTrading";
    private static final String SECURITY_STATUS_QUAL_STR_OPEN_NIGHT_TRADING                 = "OpenNightTrading";
    private static final String SECURITY_STATUS_QUAL_STR_SUB_HOLIDAY                        = "SubHoliday";
    private static final String SECURITY_STATUS_QUAL_STR_BID_ONLY                           = "BidOnly";
    private static final String SECURITY_STATUS_QUAL_STR_ASK_ONLY                           = "AskOnly";
    private static final String SECURITY_STATUS_QUAL_STR_OPENING_DELAY_COMMON               = "OpeningDelayCommon";
    private static final String SECURITY_STATUS_QUAL_STR_RESUME_COMMON                      = "ResumeCommon";
    private static final String SECURITY_STATUS_QUAL_STR_NO_OPEN_NO_RESUME_COMMON           = "NoOpenNoResumeCommon";
    private static final String SECURITY_STATUS_QUAL_STR_NEWS_DISSEMINATION_RELATED         = "NewsDisseminationRelated";
    private static final String SECURITY_STATUS_QUAL_STR_ORDER_INFLUX_RELATED               = "OrderInfluxRelated";
    private static final String SECURITY_STATUS_QUAL_STR_ORDER_IMBALANCE_RELATED            = "OrderImbRelated";
    private static final String SECURITY_STATUS_QUAL_STR_INFORMATION_REQUESTED_RELATED      = "InfoRequestedRelated";
    private static final String SECURITY_STATUS_QUAL_STR_NEWS_PENDING_RELATED               = "NewsPendingRelated";
    private static final String SECURITY_STATUS_QUAL_STR_EQUIPMENT_CHANGEOVER_RELATED       = "EquipChangeOverRelated";
    private static final String SECURITY_STATUS_QUAL_STR_SUB_PENNY_TRADING_RELATED          = "SubPennyTradingRelated";
    private static final String SECURITY_STATUS_QUAL_STR_IMBALANCE_PREOPEN_BUY              = "ImbPreOpenBuy";
    private static final String SECURITY_STATUS_QUAL_STR_IMBALANCE_PREOPEN_SELL             = "ImbPreOpenSell";
    private static final String SECURITY_STATUS_QUAL_STR_IMBALANCE_PRECLOSE_BUY             = "ImbPreCloseBuy";
    private static final String SECURITY_STATUS_QUAL_STR_IMBALANCE_PRECLOSE_SELL            = "ImbPreCloseSell";
    private static final String SECURITY_STATUS_QUAL_STR_VOLATILITY_PAUSE                   = "VolatilityPause";
    private static final String SECURITY_STATUS_QUAL_STR_VOLATILITY_PAUSE_QUOTE_RESUME      = "VolatilityPauseQuoteResume";
    private static final String SECURITY_STATUS_QUAL_STR_VOLATILITY_GUARD                   = "VolatilityGuard";
    private static final String SECURITY_STATUS_QUAL_STR_VOLATILITY_GUARD_QUOTE_RESUME      = "VolatilityGuardQuoteResume";
    private static final String SECURITY_STATUS_QUAL_STR_UNKNOWN                            = "Unknown";

     public static String toString (short securityStatusQual)
    {
        switch (securityStatusQual) 
        {
            case SECURITY_STATUS_QUAL_NONE:
                return SECURITY_STATUS_QUAL_STR_NONE;
            case SECURITY_STATUS_QUAL_OPENING:
                return SECURITY_STATUS_QUAL_STR_OPENING;
            case SECURITY_STATUS_QUAL_EXCUSED:
                return SECURITY_STATUS_QUAL_STR_EXCUSED;     
            case SECURITY_STATUS_QUAL_WITHDRAWN:
                return SECURITY_STATUS_QUAL_STR_WITHDRAWN;
            case SECURITY_STATUS_QUAL_SUSPENDED:
                return SECURITY_STATUS_QUAL_STR_SUSPENDED;
            case SECURITY_STATUS_QUAL_RESUME:
                return SECURITY_STATUS_QUAL_STR_RESUME;
            case SECURITY_STATUS_QUAL_QUOTE_RESUME:
                return SECURITY_STATUS_QUAL_STR_QUOTE_RESUME;
            case SECURITY_STATUS_QUAL_TRADE_RESUME:
                return SECURITY_STATUS_QUAL_STR_TRADE_RESUME;
            case SECURITY_STATUS_QUAL_RESUME_TIME:
                return SECURITY_STATUS_QUAL_STR_RESUME_TIME;
            case SECURITY_STATUS_QUAL_MKT_IMB_BUY:
                return SECURITY_STATUS_QUAL_STR_MKT_IMB_BUY;
            case SECURITY_STATUS_QUAL_MKT_IMB_SELL:
                return SECURITY_STATUS_QUAL_STR_MKT_IMB_SELL;
            case SECURITY_STATUS_QUAL_NO_MKT_IMB:
                return SECURITY_STATUS_QUAL_STR_NO_MKT_IMB;
            case SECURITY_STATUS_QUAL_MOC_IMB_BUY:
                return SECURITY_STATUS_QUAL_STR_MOC_IMB_BUY;
            case SECURITY_STATUS_QUAL_MOC_IMB_SELL:
                return SECURITY_STATUS_QUAL_STR_MOC_IMB_SELL;
            case SECURITY_STATUS_QUAL_NO_MOC_IMB:
                return SECURITY_STATUS_QUAL_STR_NO_MOC_IMB;
            case SECURITY_STATUS_QUAL_ORDER_IMB:
                return SECURITY_STATUS_QUAL_STR_ORDER_IMB;
            case SECURITY_STATUS_QUAL_ORDER_INF:
                return SECURITY_STATUS_QUAL_STR_ORDER_INF;
            case SECURITY_STATUS_QUAL_ORDER_IMB_BUY:
                return SECURITY_STATUS_QUAL_STR_ORDER_IMB_BUY;
            case SECURITY_STATUS_QUAL_ORDER_IMB_SELL:
                return SECURITY_STATUS_QUAL_STR_ORDER_IMB_SELL;
            case SECURITY_STATUS_QUAL_ORDER_IMB_NONE:
                return SECURITY_STATUS_QUAL_STR_ORDER_IMB_NONE;
            case SECURITY_STATUS_QUAL_LOA_IMBALANCE_BUY :
                return SECURITY_STATUS_QUAL_STR_LOA_IMBALANCE_BUY;
            case SECURITY_STATUS_QUAL_LOA_IMBALANCE_SELL:
                return SECURITY_STATUS_QUAL_STR_LOA_IMBALANCE_SELL;
            case SECURITY_STATUS_QUAL_NO_LOA_IMBALANCE:
                return SECURITY_STATUS_QUAL_STR_NO_LOA_IMBALANCE;
            case SECURITY_STATUS_QUAL_ORDERS_ELMINATED:
                return SECURITY_STATUS_QUAL_STR_ORDERS_ELIMINATED;
            case SECURITY_STATUS_QUAL_RANGE_ID:
                return SECURITY_STATUS_QUAL_STR_RANGE_ID;
            case SECURITY_STATUS_QUAL_ITS_PREOPEN:
                return SECURITY_STATUS_QUAL_STR_ITS_PREOPEN;
            case SECURITY_STATUS_QUAL_RESERVED:
                return SECURITY_STATUS_QUAL_STR_RESERVED;
            case SECURITY_STATUS_QUAL_FROZEN:
                return SECURITY_STATUS_QUAL_STR_FROZEN;
            case SECURITY_STATUS_QUAL_PREOPEN:
                return SECURITY_STATUS_QUAL_STR_PREOPEN;
            case SECURITY_STATUS_QUAL_THO_IMBALANCE_BUY:
                return SECURITY_STATUS_QUAL_STR_THO_IMBALANCE_BUY;
            case SECURITY_STATUS_QUAL_THO_IMBALANCE_SELL:
                return SECURITY_STATUS_QUAL_STR_THO_IMBALANCE_SELL;
            case SECURITY_STATUS_QUAL_NO_THO_IMBALANCE:
                return SECURITY_STATUS_QUAL_STR_NO_THO_IMBALANCE;
            case SECURITY_STATUS_QUAL_ADD_INFO:
                return SECURITY_STATUS_QUAL_STR_ADD_INFO;
            case SECURITY_STATUS_QUAL_IPO_IMBALANCE_BUY:
                return SECURITY_STATUS_QUAL_STR_IPO_IMBALANCE_BUY;
            case SECURITY_STATUS_QUAL_IPO_IMBALANCE_SELL:
                return SECURITY_STATUS_QUAL_STR_IPO_IMBALANCE_SELL;
            case SECURITY_STATUS_QUAL_NO_IPO_IMBALANCE:
                return SECURITY_STATUS_QUAL_STR_NO_IPO_IMBALANCE;
            case SECURITY_STATUS_QUAL_OPEN_DELAY:
                return SECURITY_STATUS_QUAL_STR_OPEN_DELAY;
            case SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME:
                return SECURITY_STATUS_QUAL_STR_NO_OPEN_NO_RESUME;
            case SECURITY_STATUS_QUAL_PRICE_IND:
                return SECURITY_STATUS_QUAL_STR_PRICE_IND;
            case SECURITY_STATUS_QUAL_EQUIPMENT:
                return SECURITY_STATUS_QUAL_STR_EQUIPMENT;
            case SECURITY_STATUS_QUAL_FILINGS:
                return SECURITY_STATUS_QUAL_STR_FILINGS;
            case SECURITY_STATUS_QUAL_NEWS:
                return SECURITY_STATUS_QUAL_STR_NEWS;
            case SECURITY_STATUS_QUAL_NEWS_DISSEM:
                return SECURITY_STATUS_QUAL_STR_NEWS_DISSEM;
            case SECURITY_STATUS_QUAL_LISTING:      
                return SECURITY_STATUS_QUAL_STR_LISTING;
            case SECURITY_STATUS_QUAL_OPERATION:
                return SECURITY_STATUS_QUAL_STR_OPERATION;    
            case SECURITY_STATUS_QUAL_INFO:
                return SECURITY_STATUS_QUAL_STR_INFO;
            case SECURITY_STATUS_QUAL_SEC:
                return SECURITY_STATUS_QUAL_STR_SEC;
            case SECURITY_STATUS_QUAL_TIMES:
                return SECURITY_STATUS_QUAL_STR_TIMES;
            case SECURITY_STATUS_QUAL_OTHER:
                return SECURITY_STATUS_QUAL_STR_OTHER;
            case SECURITY_STATUS_QUAL_RELATED:
                return SECURITY_STATUS_QUAL_STR_RELATED;
            case SECURITY_STATUS_QUAL_IPO:
                return SECURITY_STATUS_QUAL_STR_IPO;
            case SECURITY_STATUS_QUAL_EMC_IMB_BUY:                
                return SECURITY_STATUS_QUAL_STR_EMC_IMB_BUY;       
            case SECURITY_STATUS_QUAL_EMC_IMB_SELL:               
                return SECURITY_STATUS_QUAL_STR_EMC_IMB_SELL;      
            case SECURITY_STATUS_QUAL_EMC_IMB_NONE:               
                return SECURITY_STATUS_QUAL_STR_EMC_IMB_NONE;      
            case SECURITY_STATUS_QUAL_PRE_CROSS:                  
                return SECURITY_STATUS_QUAL_STR_PRE_CROSS;        
            case SECURITY_STATUS_QUAL_CROSS:                      
                return SECURITY_STATUS_QUAL_STR_CROSS;             
            case SECURITY_STATUS_QUAL_RELEASED_FOR_QUOTATION:     
                return SECURITY_STATUS_QUAL_STR_RELEASED_FOR_QUOTATION; 
            case SECURITY_STATUS_QUAL_IPO_WINDOW_EXT:             
                return SECURITY_STATUS_QUAL_STR_IPO_WINDOW_EXT;         
            case SECURITY_STATUS_QUAL_PRECLOSING:                 
                return SECURITY_STATUS_QUAL_STR_PRECLOSING;             
            case SECURITY_STATUS_QUAL_AUCTION_EXTENSION:          
                return SECURITY_STATUS_QUAL_STR_AUCTION_EXTENSION;      
            case SECURITY_STATUS_QUAL_VOLATILITY_AUCTION:         
                return SECURITY_STATUS_QUAL_STR_VOLATILITY_AUCTION;     
            case SECURITY_STATUS_QUAL_SECURITY_AUTHORISED:        
                return SECURITY_STATUS_QUAL_STR_SECURITY_AUTHORISED;    
            case SECURITY_STATUS_QUAL_SECURITY_FORBIDDEN:         
                return SECURITY_STATUS_QUAL_STR_SECURITY_FORBIDDEN;     
            case SECURITY_STATUS_QUAL_FAST_MARKET:                
                return SECURITY_STATUS_QUAL_STR_FAST_MARKET;            
            case SECURITY_STATUS_QUAL_SLOW_MARKET:                
                return SECURITY_STATUS_QUAL_STR_SLOW_MARKET;            
            case SECURITY_STATUS_QUAL_SUB_PENNY_TRADING:          
                return SECURITY_STATUS_QUAL_STR_SUB_PENNY_TRADING;      
            case SECURITY_STATUS_QUAL_ORDER_INPUT:                
                return SECURITY_STATUS_QUAL_STR_ORDER_INPUT;            
            case SECURITY_STATUS_QUAL_PRE_ORDER_MATCHING:         
                return SECURITY_STATUS_QUAL_STR_PRE_ORDER_MATCHING;     
            case SECURITY_STATUS_QUAL_ORDER_MATCHING:             
                return SECURITY_STATUS_QUAL_STR_ORDER_MATCHING;
            case SECURITY_STATUS_QUAL_BLOCKING:              
                return SECURITY_STATUS_QUAL_STR_BLOCKING;      
            case SECURITY_STATUS_QUAL_ORDER_CANCEL:          
                return SECURITY_STATUS_QUAL_STR_ORDER_CANCEL;  
            case SECURITY_STATUS_QUAL_FIXED_PRICE:           
                return SECURITY_STATUS_QUAL_STR_FIXED_PRICE;   
            case SECURITY_STATUS_QUAL_SALES_INPUT:           
                return SECURITY_STATUS_QUAL_STR_SALES_INPUT;   
            case SECURITY_STATUS_QUAL_EXCHANGE_INTERVENTION: 
                return SECURITY_STATUS_QUAL_STR_EXCHANGE_INTERVENTION;  
            case SECURITY_STATUS_QUAL_PRE_AUCTION:                
                return SECURITY_STATUS_QUAL_STR_PRE_AUCTION; 
            case SECURITY_STATUS_QUAL_ADJUST:              
                return SECURITY_STATUS_QUAL_STR_ADJUST;      
            case SECURITY_STATUS_QUAL_ADJUST_ON:           
                return SECURITY_STATUS_QUAL_STR_ADJUST_ON;   
            case SECURITY_STATUS_QUAL_LATE_TRADING:        
                return SECURITY_STATUS_QUAL_STR_LATE_TRADING;
            case SECURITY_STATUS_QUAL_ENQUIRE:             
                return SECURITY_STATUS_QUAL_STR_ENQUIRE;     
            case SECURITY_STATUS_QUAL_PRE_NIGHT_TRADING:   
                return SECURITY_STATUS_QUAL_STR_PRE_NIGHT_TRADING; 
            case SECURITY_STATUS_QUAL_OPEN_NIGHT_TRADING:        
                return SECURITY_STATUS_QUAL_STR_OPEN_NIGHT_TRADING;
            case SECURITY_STATUS_QUAL_SUB_HOLIDAY:        
                return SECURITY_STATUS_QUAL_STR_SUB_HOLIDAY;
            case SECURITY_STATUS_QUAL_BID_ONLY:             
                return SECURITY_STATUS_QUAL_STR_BID_ONLY;     
            case SECURITY_STATUS_QUAL_ASK_ONLY:        
                return SECURITY_STATUS_QUAL_STR_ASK_ONLY;
            case SECURITY_STATUS_QUAL_UNKNOWN:
                return SECURITY_STATUS_QUAL_STR_UNKNOWN;
            case SECURITY_STATUS_QUAL_OPENING_DELAY_COMMON:
                return SECURITY_STATUS_QUAL_STR_OPENING_DELAY_COMMON;
            case SECURITY_STATUS_QUAL_RESUME_COMMON :
                return  SECURITY_STATUS_QUAL_STR_RESUME_COMMON;          
            case  SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME_COMMON:
                return  SECURITY_STATUS_QUAL_STR_NO_OPEN_NO_RESUME_COMMON;
            case SECURITY_STATUS_QUAL_NEWS_DISSEMINATION_RELATED:
                return SECURITY_STATUS_QUAL_STR_NEWS_DISSEMINATION_RELATED;
            case SECURITY_STATUS_QUAL_ORDER_INFLUX_RELATED :
                return SECURITY_STATUS_QUAL_STR_ORDER_INFLUX_RELATED ;
            case SECURITY_STATUS_QUAL_ORDER_IMBALANCE_RELATED:
                return SECURITY_STATUS_QUAL_STR_ORDER_IMBALANCE_RELATED;
            case SECURITY_STATUS_QUAL_INFORMATION_REQUESTED_RELATED:
                return SECURITY_STATUS_QUAL_STR_INFORMATION_REQUESTED_RELATED;
            case SECURITY_STATUS_QUAL_NEWS_PENDING_RELATED:
                return SECURITY_STATUS_QUAL_STR_NEWS_PENDING_RELATED;
            case SECURITY_STATUS_QUAL_EQUIPMENT_CHANGEOVER_RELATED:
                return SECURITY_STATUS_QUAL_STR_EQUIPMENT_CHANGEOVER_RELATED;
            case SECURITY_STATUS_QUAL_SUB_PENNY_TRADING_RELATED:
                return SECURITY_STATUS_QUAL_STR_SUB_PENNY_TRADING_RELATED;
            case SECURITY_STATUS_QUAL_IMBALANCE_PREOPEN_BUY:
                return SECURITY_STATUS_QUAL_STR_IMBALANCE_PREOPEN_BUY;
            case SECURITY_STATUS_QUAL_IMBALANCE_PREOPEN_SELL:
                return SECURITY_STATUS_QUAL_STR_IMBALANCE_PREOPEN_SELL;
            case SECURITY_STATUS_QUAL_IMBALANCE_PRECLOSE_BUY:
                return SECURITY_STATUS_QUAL_STR_IMBALANCE_PRECLOSE_BUY;
            case SECURITY_STATUS_QUAL_IMBALANCE_PRECLOSE_SELL:
                return SECURITY_STATUS_QUAL_STR_IMBALANCE_PRECLOSE_SELL;
            case SECURITY_STATUS_QUAL_VOLATILITY_PAUSE:
                return SECURITY_STATUS_QUAL_STR_VOLATILITY_PAUSE;
            case SECURITY_STATUS_QUAL_VOLATILITY_PAUSE_QUOTE_RESUME:
                return SECURITY_STATUS_QUAL_STR_VOLATILITY_PAUSE_QUOTE_RESUME;
            case SECURITY_STATUS_QUAL_VOLATILITY_GUARD:
                return SECURITY_STATUS_QUAL_STR_VOLATILITY_GUARD;
            case SECURITY_STATUS_QUAL_VOLATILITY_GUARD_QUOTE_RESUME:
                return SECURITY_STATUS_QUAL_STR_VOLATILITY_GUARD_QUOTE_RESUME;
        }
        return SECURITY_STATUS_QUAL_STR_UNKNOWN;
    }

    public static short mamdaSecurityStatusQualFromString (String securityStatusQual)
    {
        if (securityStatusQual == null)
        {
            return SECURITY_STATUS_QUAL_UNKNOWN;
        }
        // Older FH configurations sent strings:
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_NONE))
            return SECURITY_STATUS_QUAL_NONE;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_OPENING))
            return SECURITY_STATUS_QUAL_OPENING;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_EXCUSED))
            return SECURITY_STATUS_QUAL_EXCUSED;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_WITHDRAWN))
            return SECURITY_STATUS_QUAL_WITHDRAWN;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_SUSPENDED))
            return SECURITY_STATUS_QUAL_SUSPENDED;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_RESUME))
            return SECURITY_STATUS_QUAL_RESUME;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_QUOTE_RESUME))
            return SECURITY_STATUS_QUAL_QUOTE_RESUME;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_TRADE_RESUME))
            return SECURITY_STATUS_QUAL_TRADE_RESUME;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_RESUME_TIME))
            return SECURITY_STATUS_QUAL_RESUME_TIME;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_MKT_IMB_BUY))
            return SECURITY_STATUS_QUAL_MKT_IMB_BUY;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_MKT_IMB_SELL))
            return SECURITY_STATUS_QUAL_MKT_IMB_SELL;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_NO_MKT_IMB))
            return SECURITY_STATUS_QUAL_NO_MKT_IMB;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_MOC_IMB_BUY))
            return SECURITY_STATUS_QUAL_MOC_IMB_BUY;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_MOC_IMB_SELL))
            return SECURITY_STATUS_QUAL_MOC_IMB_SELL;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_NO_MOC_IMB))
            return SECURITY_STATUS_QUAL_NO_MOC_IMB;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_ORDER_IMB))
            return SECURITY_STATUS_QUAL_ORDER_IMB;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_ORDER_INF))
            return SECURITY_STATUS_QUAL_ORDER_INF;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_ORDER_IMB_BUY))
            return SECURITY_STATUS_QUAL_ORDER_IMB_BUY;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_ORDER_IMB_SELL))
            return SECURITY_STATUS_QUAL_ORDER_IMB_SELL;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_ORDER_IMB_NONE))
            return SECURITY_STATUS_QUAL_ORDER_IMB_NONE;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_LOA_IMBALANCE_BUY))
            return SECURITY_STATUS_QUAL_LOA_IMBALANCE_BUY;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_LOA_IMBALANCE_SELL))
            return SECURITY_STATUS_QUAL_LOA_IMBALANCE_SELL;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_NO_LOA_IMBALANCE))
            return SECURITY_STATUS_QUAL_NO_LOA_IMBALANCE;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_ORDERS_ELIMINATED))
            return SECURITY_STATUS_QUAL_ORDERS_ELMINATED;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_RANGE_ID))
            return SECURITY_STATUS_QUAL_RANGE_ID;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_ITS_PREOPEN))
            return SECURITY_STATUS_QUAL_ITS_PREOPEN;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_RESERVED))
            return SECURITY_STATUS_QUAL_RESERVED;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_FROZEN))
            return SECURITY_STATUS_QUAL_FROZEN;
        if (securityStatusQual.equals ( SECURITY_STATUS_QUAL_STR_PREOPEN))
            return SECURITY_STATUS_QUAL_PREOPEN;
        if (securityStatusQual.equals ( SECURITY_STATUS_QUAL_STR_THO_IMBALANCE_BUY))
            return SECURITY_STATUS_QUAL_THO_IMBALANCE_BUY;
        if (securityStatusQual.equals ( SECURITY_STATUS_QUAL_STR_THO_IMBALANCE_SELL))
            return SECURITY_STATUS_QUAL_THO_IMBALANCE_SELL;
        if (securityStatusQual.equals ( SECURITY_STATUS_QUAL_STR_NO_THO_IMBALANCE))
            return SECURITY_STATUS_QUAL_NO_THO_IMBALANCE;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_ADD_INFO))
            return SECURITY_STATUS_QUAL_ADD_INFO;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_IPO_IMBALANCE_BUY))
            return SECURITY_STATUS_QUAL_IPO_IMBALANCE_BUY;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_IPO_IMBALANCE_SELL))
            return SECURITY_STATUS_QUAL_IPO_IMBALANCE_SELL;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_NO_IPO_IMBALANCE))
            return SECURITY_STATUS_QUAL_NO_IPO_IMBALANCE;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_EMC_IMB_BUY))
            return SECURITY_STATUS_QUAL_EMC_IMB_BUY;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_EMC_IMB_SELL))
            return SECURITY_STATUS_QUAL_EMC_IMB_SELL;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_EMC_IMB_NONE))
            return SECURITY_STATUS_QUAL_EMC_IMB_NONE;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_OPEN_DELAY))
            return SECURITY_STATUS_QUAL_OPEN_DELAY;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_NO_OPEN_NO_RESUME))
            return SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_PRICE_IND))
            return SECURITY_STATUS_QUAL_PRICE_IND;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_EQUIPMENT))
            return SECURITY_STATUS_QUAL_EQUIPMENT;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_FILINGS))
            return SECURITY_STATUS_QUAL_FILINGS;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_NEWS))
            return SECURITY_STATUS_QUAL_NEWS;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_NEWS_DISSEM))
            return SECURITY_STATUS_QUAL_NEWS_DISSEM;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_LISTING))
            return SECURITY_STATUS_QUAL_LISTING;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_OPERATION))
            return SECURITY_STATUS_QUAL_OPERATION;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_INFO))
            return SECURITY_STATUS_QUAL_INFO;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_SEC))
            return SECURITY_STATUS_QUAL_SEC;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_TIMES))
            return SECURITY_STATUS_QUAL_TIMES;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_OTHER))
            return SECURITY_STATUS_QUAL_OTHER;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_RELATED))
            return SECURITY_STATUS_QUAL_RELATED;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_IPO))
            return SECURITY_STATUS_QUAL_IPO;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_PRE_CROSS))
            return SECURITY_STATUS_QUAL_PRE_CROSS;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_CROSS))
            return SECURITY_STATUS_QUAL_CROSS;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_RELEASED_FOR_QUOTATION))
            return SECURITY_STATUS_QUAL_RELEASED_FOR_QUOTATION;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_IPO_WINDOW_EXT))
            return SECURITY_STATUS_QUAL_IPO_WINDOW_EXT;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_PRECLOSING))
            return SECURITY_STATUS_QUAL_PRECLOSING;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_AUCTION_EXTENSION))
            return SECURITY_STATUS_QUAL_AUCTION_EXTENSION;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_VOLATILITY_AUCTION))
            return SECURITY_STATUS_QUAL_VOLATILITY_AUCTION;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_SECURITY_AUTHORISED))
            return SECURITY_STATUS_QUAL_SECURITY_AUTHORISED;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_SECURITY_FORBIDDEN))
            return SECURITY_STATUS_QUAL_SECURITY_FORBIDDEN;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_FAST_MARKET))
            return SECURITY_STATUS_QUAL_FAST_MARKET;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_SLOW_MARKET))
            return SECURITY_STATUS_QUAL_SLOW_MARKET;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_SUB_PENNY_TRADING))
            return SECURITY_STATUS_QUAL_SUB_PENNY_TRADING;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_ORDER_INPUT))
            return SECURITY_STATUS_QUAL_ORDER_INPUT;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_PRE_ORDER_MATCHING))
            return SECURITY_STATUS_QUAL_PRE_ORDER_MATCHING;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_ORDER_MATCHING))
            return SECURITY_STATUS_QUAL_ORDER_MATCHING;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_BLOCKING))
            return SECURITY_STATUS_QUAL_BLOCKING;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_ORDER_CANCEL))
            return SECURITY_STATUS_QUAL_ORDER_CANCEL;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_FIXED_PRICE))
            return SECURITY_STATUS_QUAL_FIXED_PRICE;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_SALES_INPUT))
            return SECURITY_STATUS_QUAL_SALES_INPUT;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_EXCHANGE_INTERVENTION))
            return SECURITY_STATUS_QUAL_EXCHANGE_INTERVENTION;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_PRE_AUCTION))
            return SECURITY_STATUS_QUAL_PRE_AUCTION;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_ADJUST))
            return SECURITY_STATUS_QUAL_ADJUST;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_ADJUST_ON))
            return SECURITY_STATUS_QUAL_ADJUST_ON;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_LATE_TRADING))
            return SECURITY_STATUS_QUAL_LATE_TRADING;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_ENQUIRE))
            return SECURITY_STATUS_QUAL_ENQUIRE;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_PRE_NIGHT_TRADING))
            return SECURITY_STATUS_QUAL_PRE_NIGHT_TRADING;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_OPEN_NIGHT_TRADING))
            return SECURITY_STATUS_QUAL_OPEN_NIGHT_TRADING;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_SUB_HOLIDAY))
            return SECURITY_STATUS_QUAL_SUB_HOLIDAY;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_BID_ONLY))
            return SECURITY_STATUS_QUAL_BID_ONLY;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_ASK_ONLY))
            return SECURITY_STATUS_QUAL_ASK_ONLY;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_OPENING_DELAY_COMMON))
            return SECURITY_STATUS_QUAL_OPENING_DELAY_COMMON;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_RESUME_COMMON))
            return SECURITY_STATUS_QUAL_RESUME_COMMON;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_NO_OPEN_NO_RESUME_COMMON))
            return SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME_COMMON;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_NEWS_DISSEMINATION_RELATED))
            return SECURITY_STATUS_QUAL_NEWS_DISSEMINATION_RELATED;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_ORDER_INFLUX_RELATED))
            return SECURITY_STATUS_QUAL_ORDER_INFLUX_RELATED;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_ORDER_IMBALANCE_RELATED))
            return SECURITY_STATUS_QUAL_ORDER_IMBALANCE_RELATED;
        if (securityStatusQual.equals ( SECURITY_STATUS_QUAL_STR_INFORMATION_REQUESTED_RELATED))
            return  SECURITY_STATUS_QUAL_INFORMATION_REQUESTED_RELATED;
        if (securityStatusQual.equals ( SECURITY_STATUS_QUAL_STR_NEWS_PENDING_RELATED))
            return  SECURITY_STATUS_QUAL_NEWS_PENDING_RELATED;                                       
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_EQUIPMENT_CHANGEOVER_RELATED))
            return  SECURITY_STATUS_QUAL_EQUIPMENT_CHANGEOVER_RELATED;
        if (securityStatusQual.equals ( SECURITY_STATUS_QUAL_STR_SUB_PENNY_TRADING_RELATED))
            return SECURITY_STATUS_QUAL_SUB_PENNY_TRADING_RELATED;
        if (securityStatusQual.equals ( SECURITY_STATUS_QUAL_STR_IMBALANCE_PREOPEN_BUY))
            return SECURITY_STATUS_QUAL_IMBALANCE_PREOPEN_BUY;
        if (securityStatusQual.equals ( SECURITY_STATUS_QUAL_STR_IMBALANCE_PREOPEN_SELL))
            return SECURITY_STATUS_QUAL_IMBALANCE_PREOPEN_SELL;
        if (securityStatusQual.equals ( SECURITY_STATUS_QUAL_STR_IMBALANCE_PRECLOSE_BUY))
            return SECURITY_STATUS_QUAL_IMBALANCE_PRECLOSE_BUY;
        if (securityStatusQual.equals ( SECURITY_STATUS_QUAL_STR_IMBALANCE_PRECLOSE_SELL ))
            return SECURITY_STATUS_QUAL_IMBALANCE_PRECLOSE_SELL;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_VOLATILITY_PAUSE))
            return SECURITY_STATUS_QUAL_VOLATILITY_PAUSE;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_VOLATILITY_PAUSE_QUOTE_RESUME))
            return SECURITY_STATUS_QUAL_VOLATILITY_PAUSE_QUOTE_RESUME;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_VOLATILITY_GUARD))
            return SECURITY_STATUS_QUAL_VOLATILITY_GUARD;
        if (securityStatusQual.equals (SECURITY_STATUS_QUAL_STR_VOLATILITY_GUARD_QUOTE_RESUME))
            return SECURITY_STATUS_QUAL_VOLATILITY_GUARD_QUOTE_RESUME;

        // A misconfigured FH might send numbers as strings:
        if (securityStatusQual.equals ("0"))
            return SECURITY_STATUS_QUAL_NONE;
        if (securityStatusQual.equals ("1"))
            return SECURITY_STATUS_QUAL_OPENING;
        if (securityStatusQual.equals ("7"))
            return SECURITY_STATUS_QUAL_EXCUSED;
        if (securityStatusQual.equals ("8"))
            return SECURITY_STATUS_QUAL_WITHDRAWN;
        if (securityStatusQual.equals ("9"))
            return SECURITY_STATUS_QUAL_SUSPENDED;
        if (securityStatusQual.equals ("11"))
            return SECURITY_STATUS_QUAL_RESUME;
        if (securityStatusQual.equals ("12"))
            return SECURITY_STATUS_QUAL_QUOTE_RESUME;
        if (securityStatusQual.equals ("13"))
            return SECURITY_STATUS_QUAL_TRADE_RESUME;
        if (securityStatusQual.equals ("14"))
            return SECURITY_STATUS_QUAL_RESUME_TIME;
        if (securityStatusQual.equals ("16"))
            return SECURITY_STATUS_QUAL_MKT_IMB_BUY;
        if (securityStatusQual.equals ("17"))
            return SECURITY_STATUS_QUAL_MKT_IMB_SELL;
        if (securityStatusQual.equals ("18"))
            return SECURITY_STATUS_QUAL_NO_MKT_IMB;
        if (securityStatusQual.equals ("19"))
            return SECURITY_STATUS_QUAL_MOC_IMB_BUY;
        if (securityStatusQual.equals ("20"))
            return SECURITY_STATUS_QUAL_MOC_IMB_SELL;
        if (securityStatusQual.equals ("21"))
            return SECURITY_STATUS_QUAL_NO_MOC_IMB;
        if (securityStatusQual.equals ("22"))
            return SECURITY_STATUS_QUAL_ORDER_IMB;
        if (securityStatusQual.equals ("23"))
            return SECURITY_STATUS_QUAL_ORDER_INF;
        if (securityStatusQual.equals ("24"))
            return SECURITY_STATUS_QUAL_ORDER_IMB_BUY;
        if (securityStatusQual.equals ("25"))
            return SECURITY_STATUS_QUAL_ORDER_IMB_SELL;
        if (securityStatusQual.equals ("26"))
            return SECURITY_STATUS_QUAL_ORDER_IMB_NONE;
        if (securityStatusQual.equals ("27"))
            return  SECURITY_STATUS_QUAL_LOA_IMBALANCE_BUY;
        if (securityStatusQual.equals ("28"))
            return  SECURITY_STATUS_QUAL_LOA_IMBALANCE_SELL;
        if (securityStatusQual.equals ("29"))
            return  SECURITY_STATUS_QUAL_NO_LOA_IMBALANCE;
        if (securityStatusQual.equals ("30"))
            return  SECURITY_STATUS_QUAL_ORDERS_ELMINATED;
        if (securityStatusQual.equals ("31"))
            return SECURITY_STATUS_QUAL_RANGE_ID;
        if (securityStatusQual.equals ("32"))
            return SECURITY_STATUS_QUAL_ITS_PREOPEN;
        if (securityStatusQual.equals ("33"))
            return SECURITY_STATUS_QUAL_RESERVED;
        if (securityStatusQual.equals ("34"))
            return SECURITY_STATUS_QUAL_FROZEN;
        if (securityStatusQual.equals ("35"))
            return SECURITY_STATUS_QUAL_PREOPEN;
        if (securityStatusQual.equals ("36"))
            return SECURITY_STATUS_QUAL_THO_IMBALANCE_BUY;
        if (securityStatusQual.equals ("37"))
            return SECURITY_STATUS_QUAL_THO_IMBALANCE_SELL;
        if (securityStatusQual.equals ("38"))
            return SECURITY_STATUS_QUAL_NO_THO_IMBALANCE;
        if (securityStatusQual.equals ("41"))
            return SECURITY_STATUS_QUAL_ADD_INFO;
        if (securityStatusQual.equals ("42"))
            return SECURITY_STATUS_QUAL_IPO_IMBALANCE_BUY;
        if (securityStatusQual.equals ("43"))
            return SECURITY_STATUS_QUAL_IPO_IMBALANCE_SELL;
        if (securityStatusQual.equals ("44"))
            return SECURITY_STATUS_QUAL_NO_IPO_IMBALANCE;
        if (securityStatusQual.equals ("46"))               
          return SECURITY_STATUS_QUAL_EMC_IMB_BUY;           
        if (securityStatusQual.equals ("47"))              
          return SECURITY_STATUS_QUAL_EMC_IMB_SELL;          
        if (securityStatusQual.equals ("48"))              
          return SECURITY_STATUS_QUAL_EMC_IMB_NONE;  
        if (securityStatusQual.equals ("51"))
            return SECURITY_STATUS_QUAL_OPEN_DELAY;
        if (securityStatusQual.equals ("52"))
            return SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME;
        if (securityStatusQual.equals ("53"))
            return SECURITY_STATUS_QUAL_PRICE_IND;
        if (securityStatusQual.equals ("54"))
            return SECURITY_STATUS_QUAL_EQUIPMENT;
        if (securityStatusQual.equals ("55"))
            return SECURITY_STATUS_QUAL_FILINGS;
        if (securityStatusQual.equals ("56"))
            return SECURITY_STATUS_QUAL_NEWS;
        if (securityStatusQual.equals ("57"))
            return SECURITY_STATUS_QUAL_NEWS_DISSEM;
        if (securityStatusQual.equals ("58"))
            return SECURITY_STATUS_QUAL_LISTING;
        if (securityStatusQual.equals ("59"))
            return SECURITY_STATUS_QUAL_OPERATION;
        if (securityStatusQual.equals ("60"))
            return SECURITY_STATUS_QUAL_INFO;
        if (securityStatusQual.equals ("61"))
            return SECURITY_STATUS_QUAL_SEC;
        if (securityStatusQual.equals ("62"))
            return SECURITY_STATUS_QUAL_TIMES;
        if (securityStatusQual.equals ("62"))
            return SECURITY_STATUS_QUAL_OTHER;
        if (securityStatusQual.equals ("64"))
            return SECURITY_STATUS_QUAL_RELATED;
        if (securityStatusQual.equals ("65"))
            return SECURITY_STATUS_QUAL_IPO;         
        if (securityStatusQual.equals ("66"))                 
          return SECURITY_STATUS_QUAL_PRE_CROSS;             
        if (securityStatusQual.equals ("67"))                     
          return SECURITY_STATUS_QUAL_CROSS;                 
        if (securityStatusQual.equals ("68"))    
          return SECURITY_STATUS_QUAL_RELEASED_FOR_QUOTATION;
        if (securityStatusQual.equals ("69"))            
          return SECURITY_STATUS_QUAL_IPO_WINDOW_EXT;        
        if (securityStatusQual.equals ("70"))                
          return SECURITY_STATUS_QUAL_PRECLOSING;            
        if (securityStatusQual.equals ("71"))         
          return SECURITY_STATUS_QUAL_AUCTION_EXTENSION;     
        if (securityStatusQual.equals ("72"))        
          return SECURITY_STATUS_QUAL_VOLATILITY_AUCTION;    
        if (securityStatusQual.equals ("73"))       
          return SECURITY_STATUS_QUAL_SECURITY_AUTHORISED;   
        if (securityStatusQual.equals ("74"))        
          return SECURITY_STATUS_QUAL_SECURITY_FORBIDDEN;    
        if (securityStatusQual.equals ("75"))               
          return SECURITY_STATUS_QUAL_FAST_MARKET;           
        if (securityStatusQual.equals ("76"))           
          return SECURITY_STATUS_QUAL_SLOW_MARKET;           
        if (securityStatusQual.equals ("77"))     
          return SECURITY_STATUS_QUAL_SUB_PENNY_TRADING;     
        if (securityStatusQual.equals ("78"))           
          return SECURITY_STATUS_QUAL_ORDER_INPUT;           
        if (securityStatusQual.equals ("79"))    
          return SECURITY_STATUS_QUAL_PRE_ORDER_MATCHING;    
        if (securityStatusQual.equals ("80"))        
          return SECURITY_STATUS_QUAL_ORDER_MATCHING;        
        if (securityStatusQual.equals ("81"))              
          return SECURITY_STATUS_QUAL_BLOCKING;              
        if (securityStatusQual.equals ("82"))          
          return SECURITY_STATUS_QUAL_ORDER_CANCEL;          
        if (securityStatusQual.equals ("83"))           
          return SECURITY_STATUS_QUAL_FIXED_PRICE;           
        if (securityStatusQual.equals ("84"))           
          return SECURITY_STATUS_QUAL_SALES_INPUT;           
        if (securityStatusQual.equals ("85")) 
          return SECURITY_STATUS_QUAL_EXCHANGE_INTERVENTION; 
        if (securityStatusQual.equals ("86"))           
          return SECURITY_STATUS_QUAL_PRE_AUCTION;           
        if (securityStatusQual.equals ("87"))                
          return SECURITY_STATUS_QUAL_ADJUST;                
        if (securityStatusQual.equals ("88"))             
          return SECURITY_STATUS_QUAL_ADJUST_ON;             
        if (securityStatusQual.equals ("89"))          
          return SECURITY_STATUS_QUAL_LATE_TRADING;          
        if (securityStatusQual.equals ("90"))               
          return SECURITY_STATUS_QUAL_ENQUIRE;               
        if (securityStatusQual.equals ("91"))     
          return SECURITY_STATUS_QUAL_PRE_NIGHT_TRADING;     
        if (securityStatusQual.equals ("92"))    
          return SECURITY_STATUS_QUAL_OPEN_NIGHT_TRADING; 
        if (securityStatusQual.equals ("93"))    
          return  SECURITY_STATUS_QUAL_SUB_HOLIDAY; 
        if (securityStatusQual.equals ("94"))              
          return SECURITY_STATUS_QUAL_BID_ONLY;              
        if (securityStatusQual.equals ("95"))         
          return SECURITY_STATUS_QUAL_ASK_ONLY; 
        if(securityStatusQual.equals("101"))
          return SECURITY_STATUS_QUAL_OPENING_DELAY_COMMON;
        if(securityStatusQual.equals("102"))
          return SECURITY_STATUS_QUAL_RESUME_COMMON;
        if(securityStatusQual.equals("103"))
          return  SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME_COMMON;
        if(securityStatusQual.equals("104"))
          return SECURITY_STATUS_QUAL_NEWS_DISSEMINATION_RELATED;
        if(securityStatusQual.equals("105"))
          return SECURITY_STATUS_QUAL_ORDER_INFLUX_RELATED;
        if(securityStatusQual.equals("106"))
          return  SECURITY_STATUS_QUAL_ORDER_IMBALANCE_RELATED;
        if(securityStatusQual.equals("107"))
          return  SECURITY_STATUS_QUAL_INFORMATION_REQUESTED_RELATED;
        if(securityStatusQual.equals("108"))
          return SECURITY_STATUS_QUAL_NEWS_PENDING_RELATED;
        if(securityStatusQual.equals("109"))
          return SECURITY_STATUS_QUAL_EQUIPMENT_CHANGEOVER_RELATED;
        if(securityStatusQual.equals("110"))
          return SECURITY_STATUS_QUAL_SUB_PENNY_TRADING_RELATED;
        if(securityStatusQual.equals("112"))
          return SECURITY_STATUS_QUAL_IMBALANCE_PREOPEN_BUY;
        if(securityStatusQual.equals("113"))
          return  SECURITY_STATUS_QUAL_IMBALANCE_PREOPEN_SELL;
        if(securityStatusQual.equals("114"))
          return SECURITY_STATUS_QUAL_IMBALANCE_PRECLOSE_BUY;
        if(securityStatusQual.equals("115"))
          return SECURITY_STATUS_QUAL_IMBALANCE_PRECLOSE_SELL;
        if(securityStatusQual.equals("116"))
          return SECURITY_STATUS_QUAL_VOLATILITY_PAUSE;
        if(securityStatusQual.equals("117"))
          return SECURITY_STATUS_QUAL_VOLATILITY_PAUSE_QUOTE_RESUME;
        if(securityStatusQual.equals("118"))
          return SECURITY_STATUS_QUAL_VOLATILITY_GUARD;
        if(securityStatusQual.equals("119"))
          return SECURITY_STATUS_QUAL_VOLATILITY_GUARD_QUOTE_RESUME;

        return SECURITY_STATUS_QUAL_UNKNOWN;
    }
}
