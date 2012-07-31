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

#include <mamda/MamdaSecurityStatusQual.h>
#include <string.h>

namespace Wombat
{

    static const char* SECURITY_STATUS_QUAL_STR_NONE                    = "None";
    static const char* SECURITY_STATUS_QUAL_STR_OPENING                 = "Opening";
    static const char* SECURITY_STATUS_QUAL_STR_EXCUSED                 = "Excused";
    static const char* SECURITY_STATUS_QUAL_STR_WITHDRAWN               = "Withdrawn";
    static const char* SECURITY_STATUS_QUAL_STR_SUSPENDED               = "Suspended";
    static const char* SECURITY_STATUS_QUAL_STR_RESUME                  = "Resume";
    static const char* SECURITY_STATUS_QUAL_STR_QUOTE_RESUME            = "QuoteResume";
    static const char* SECURITY_STATUS_QUAL_STR_TRADE_RESUME            = "TradeResume";
    static const char* SECURITY_STATUS_QUAL_STR_RESUME_TIME             = "ResumeTime";
    static const char* SECURITY_STATUS_QUAL_STR_MKT_IMB_BUY             = "MktImbBuy";
    static const char* SECURITY_STATUS_QUAL_STR_MKT_IMB_SELL            = "MktImbSell";
    static const char* SECURITY_STATUS_QUAL_STR_NO_MKT_IMB              = "NoMktImb";
    static const char* SECURITY_STATUS_QUAL_STR_MOC_IMB_BUY             = "MocImbBuy";
    static const char* SECURITY_STATUS_QUAL_STR_MOC_IMB_SELL            = "MocImbSell";
    static const char* SECURITY_STATUS_QUAL_STR_NO_MOC_IMB              = "NoMocImb";
    static const char* SECURITY_STATUS_QUAL_STR_ORDER_IMB               = "OrderImb";
    static const char* SECURITY_STATUS_QUAL_STR_ORDER_INF               = "OrderInf";
    static const char* SECURITY_STATUS_QUAL_STR_ORDER_IMB_BUY           = "OrderImbBuy";
    static const char* SECURITY_STATUS_QUAL_STR_ORDER_IMB_SELL          = "OrderImbSell";
    static const char* SECURITY_STATUS_QUAL_STR_ORDER_IMB_NONE          = "OrderImbNone";
    static const char* SECURITY_STATUS_QUAL_STR_LOA_IMBALANCE_BUY       = "LoaImbBuy";
    static const char* SECURITY_STATUS_QUAL_STR_LOA_IMBALANCE_SELL      = "LoaImbSell";
    static const char* SECURITY_STATUS_QUAL_STR_NO_LOA_IMBALANCE        = "LoaNoImb";
    static const char* SECURITY_STATUS_QUAL_STR_ORDERS_ELIMINATED       = "OrdersEliminated";
    static const char* SECURITY_STATUS_QUAL_STR_RANGE_ID                = "RangeInd";
    static const char* SECURITY_STATUS_QUAL_STR_ITS_PREOPEN             = "ItsPreOpen";
    static const char* SECURITY_STATUS_QUAL_STR_RESERVED                = "Reserved";
    static const char* SECURITY_STATUS_QUAL_STR_FROZEN                  = "Frozen";
    static const char* SECURITY_STATUS_QUAL_STR_PREOPEN                 = "PreOpen";
    static const char* SECURITY_STATUS_QUAL_STR_THO_IMBALANCE_BUY       = "ThoImbBuy";
    static const char* SECURITY_STATUS_QUAL_STR_THO_IMBALANCE_SELL      = "ThoImbSell";
    static const char* SECURITY_STATUS_QUAL_STR_NO_THO_IMBALANCE        = "ThoNoImb";
    static const char* SECURITY_STATUS_QUAL_STR_ADD_INFO                = "AddInfo";
    static const char* SECURITY_STATUS_QUAL_STR_IPO_IMBALANCE_BUY       = "IpoImbBuy";
    static const char* SECURITY_STATUS_QUAL_STR_IPO_IMBALANCE_SELL      = "IpoImbSell";
    static const char* SECURITY_STATUS_QUAL_STR_NO_IPO_IMBALANCE        = "IpoNoImb";
    static const char* SECURITY_STATUS_QUAL_STR_OPEN_DELAY              = "OpenDelay";
    static const char* SECURITY_STATUS_QUAL_STR_NO_OPEN_NO_RESUME       = "NoOpenNoResume";
    static const char* SECURITY_STATUS_QUAL_STR_PRICE_IND               = "PriceInd";
    static const char* SECURITY_STATUS_QUAL_STR_EQUIPMENT               = "Equipment";
    static const char* SECURITY_STATUS_QUAL_STR_FILINGS                 = "Filings";
    static const char* SECURITY_STATUS_QUAL_STR_NEWS                    = "News";
    static const char* SECURITY_STATUS_QUAL_STR_NEWS_DISSEM             = "NewsDissem";
    static const char* SECURITY_STATUS_QUAL_STR_LISTING                 = "Listing";
    static const char* SECURITY_STATUS_QUAL_STR_OPERATION               = "Operation";
    static const char* SECURITY_STATUS_QUAL_STR_INFO                    = "Info";
    static const char* SECURITY_STATUS_QUAL_STR_SEC                     = "SEC";
    static const char* SECURITY_STATUS_QUAL_STR_TIMES                   = "Times";
    static const char* SECURITY_STATUS_QUAL_STR_OTHER                   = "Other";
    static const char* SECURITY_STATUS_QUAL_STR_RELATED                 = "Related";
    static const char* SECURITY_STATUS_QUAL_STR_IPO                     = "IPO";

    static const char* SECURITY_STATUS_QUAL_STR_EMC_IMB_BUY                   = "EmcImbBuy";
    static const char* SECURITY_STATUS_QUAL_STR_EMC_IMB_SELL                  = "EmcImbSell";
    static const char* SECURITY_STATUS_QUAL_STR_EMC_IMB_NONE                  = "EmcImbNone";
    static const char* SECURITY_STATUS_QUAL_STR_PRE_CROSS                     = "Pre-Cross";
    static const char* SECURITY_STATUS_QUAL_STR_CROSS                         = "Cross";
    static const char* SECURITY_STATUS_QUAL_STR_RELEASED_FOR_QUOTATION        = "Released For Quotation (IPO)";
    static const char* SECURITY_STATUS_QUAL_STR_IPO_WINDOW_EXT                = "IPO Window Extension";
    static const char* SECURITY_STATUS_QUAL_STR_PRECLOSING                    = "PreClosing";
    static const char* SECURITY_STATUS_QUAL_STR_AUCTION_EXTENSION             = "Auction Extension";
    static const char* SECURITY_STATUS_QUAL_STR_VOLATILITY_AUCTION            = "Volatility Auction";
    static const char* SECURITY_STATUS_QUAL_STR_SECURITY_AUTHORISED           = "SecurityAuthorized";
    static const char* SECURITY_STATUS_QUAL_STR_SECURITY_FORBIDDEN            = "SecurityForbidden";
    static const char* SECURITY_STATUS_QUAL_STR_FAST_MARKET                   = "FastMarket";
    static const char* SECURITY_STATUS_QUAL_STR_SLOW_MARKET                   = "SlowMarket";
    static const char* SECURITY_STATUS_QUAL_STR_SUB_PENNY_TRADING             = "SubPennyTrading";
    static const char* SECURITY_STATUS_QUAL_STR_ORDER_INPUT                   = "OrderInput";
    static const char* SECURITY_STATUS_QUAL_STR_PRE_ORDER_MATCHING            = "PreOrderMatching";
    static const char* SECURITY_STATUS_QUAL_STR_ORDER_MATCHING                = "OrderMatching";
    static const char* SECURITY_STATUS_QUAL_STR_BLOCKING                      = "Blocking";
    static const char* SECURITY_STATUS_QUAL_STR_ORDER_CANCEL                  = "OrderCancel";
    static const char* SECURITY_STATUS_QUAL_STR_FIXED_PRICE                   = "FixedPrice";
    static const char* SECURITY_STATUS_QUAL_STR_SALES_INPUT                   = "SalesInput";
    static const char* SECURITY_STATUS_QUAL_STR_EXCHANGE_INTERVENTION         = "ExchangeIntervention";
    static const char* SECURITY_STATUS_QUAL_STR_PRE_AUCTION                   = "PreAuction";
    static const char* SECURITY_STATUS_QUAL_STR_ADJUST                        = "Adjust";
    static const char* SECURITY_STATUS_QUAL_STR_ADJUST_ON                     = "AdjustOn";
    static const char* SECURITY_STATUS_QUAL_STR_LATE_TRADING                  = "LateTrading";
    static const char* SECURITY_STATUS_QUAL_STR_ENQUIRE                       = "Enquire";
    static const char* SECURITY_STATUS_QUAL_STR_PRE_NIGHT_TRADING             = "PreNightTrading";
    static const char* SECURITY_STATUS_QUAL_STR_OPEN_NIGHT_TRADING            = "OpenNightTrading";
    static const char* SECURITY_STATUS_QUAL_STR_SUB_HOLIDAY                   = "SubHoliday";
    static const char* SECURITY_STATUS_QUAL_STR_BID_ONLY                      = "BidOnly";
    static const char* SECURITY_STATUS_QUAL_STR_ASK_ONLY                      = "AskOnly";
    static const char* SECURITY_STATUS_QUAL_STR_OPENING_DELAY_COMMON          = "OpenDelayCommon";
    static const char* SECURITY_STATUS_QUAL_STR_RESUME_COMMON                 = "ResumeCommon";            
    static const char* SECURITY_STATUS_QUAL_STR_NO_OPEN_NO_RESUME_COMMON      = "NoOpenNoResumeCommon";
    static const char* SECURITY_STATUS_QUAL_STR_NEWS_DISSEMINATION_RELATED    = "NewsDissemRelated";
    static const char* SECURITY_STATUS_QUAL_STR_ORDER_INFLUX_RELATED          = "OrdInfluxRelated";
    static const char* SECURITY_STATUS_QUAL_STR_ORDER_IMBALANCE_RELATED       = "OrdImbRelated";
    static const char* SECURITY_STATUS_QUAL_STR_INFORMATION_REQUESTED_RELATED = "InfoRequestRelated";
    static const char* SECURITY_STATUS_QUAL_STR_NEWS_PENDING_RELATED          = "NewsPendingRelated";
    static const char* SECURITY_STATUS_QUAL_STR_EQUIPMENT_CHANGEOVER_RELATED  = "EquipChangeOverRelated";
    static const char* SECURITY_STATUS_QUAL_STR_SUB_PENNY_TRADING_RELATED     = "SubPennyTradingRelated";
    static const char* SECURITY_STATUS_QUAL_STR_IMBALANCE_PREOPEN_BUY         = "ImbPreOpenBuy";
    static const char* SECURITY_STATUS_QUAL_STR_IMBALANCE_PREOPEN_SELL        = "ImbPreOpenSell";
    static const char* SECURITY_STATUS_QUAL_STR_IMBALANCE_PRECLOSE_BUY        = "ImbPreCloseBuy";
    static const char* SECURITY_STATUS_QUAL_STR_IMBALANCE_PRECLOSE_SELL       = "ImbPreCloseSell";
    static const char* SECURITY_STATUS_QUAL_STR_VOLATILITY_PAUSE              = "VolatilityPause";
    static const char* SECURITY_STATUS_QUAL_STR_VOLATILITY_PAUSE_QUOTE_RESUME = "VolatilityPauseQuoteResume";
    static const char* SECURITY_STATUS_QUAL_STR_VOLATILITY_GUARD              = "VolatilityGuard";
    static const char* SECURITY_STATUS_QUAL_STR_VOLATILITY_GUARD_QUOTE_RESUME = "VolatilityGuardQuoteResume";
    static const char* SECURITY_STATUS_QUAL_STR_UNKNOWN                       = "Unknown";

    const char* toString (MamdaSecurityStatusQual  securityStatusQual)
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
        case SECURITY_STATUS_QUAL_LOA_IMBALANCE_BUY:
            return SECURITY_STATUS_QUAL_STR_LOA_IMBALANCE_BUY;
        case SECURITY_STATUS_QUAL_LOA_IMBALANCE_SELL:
            return SECURITY_STATUS_QUAL_STR_LOA_IMBALANCE_SELL;
        case SECURITY_STATUS_QUAL_NO_LOA_IMBALANCE:
            return SECURITY_STATUS_QUAL_STR_NO_LOA_IMBALANCE; 
        case SECURITY_STATUS_QUAL_ORDERS_ELIMINATED :
            return SECURITY_STATUS_QUAL_STR_ORDERS_ELIMINATED ;       
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
        case SECURITY_STATUS_QUAL_IPO_IMBALANCE_BUY :           
            return SECURITY_STATUS_QUAL_STR_IPO_IMBALANCE_BUY ; 
        case SECURITY_STATUS_QUAL_IPO_IMBALANCE_SELL :           
            return SECURITY_STATUS_QUAL_STR_IPO_IMBALANCE_SELL ;  
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
        case SECURITY_STATUS_QUAL_UNKNOWN:
            return SECURITY_STATUS_QUAL_STR_UNKNOWN;

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
        case SECURITY_STATUS_QUAL_SUB_HOLIDAY  :        
            return SECURITY_STATUS_QUAL_STR_SUB_HOLIDAY;       
        case SECURITY_STATUS_QUAL_BID_ONLY:             
            return SECURITY_STATUS_QUAL_STR_BID_ONLY;     
        case SECURITY_STATUS_QUAL_ASK_ONLY:        
            return SECURITY_STATUS_QUAL_STR_ASK_ONLY;
        case SECURITY_STATUS_QUAL_OPENING_DELAY_COMMON:        
            return SECURITY_STATUS_QUAL_STR_OPENING_DELAY_COMMON; 
        case  SECURITY_STATUS_QUAL_RESUME_COMMON :        
            return  SECURITY_STATUS_QUAL_STR_RESUME_COMMON ; 
        case SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME_COMMON :        
            return  SECURITY_STATUS_QUAL_STR_NO_OPEN_NO_RESUME_COMMON;   
        case SECURITY_STATUS_QUAL_NEWS_DISSEMINATION_RELATED:        
            return SECURITY_STATUS_QUAL_STR_NEWS_DISSEMINATION_RELATED;    
        case SECURITY_STATUS_QUAL_ORDER_INFLUX_RELATED:        
            return SECURITY_STATUS_QUAL_STR_ORDER_INFLUX_RELATED;
        case SECURITY_STATUS_QUAL_ORDER_IMBALANCE_RELATED:        
            return   SECURITY_STATUS_QUAL_STR_ORDER_IMBALANCE_RELATED;  
        case SECURITY_STATUS_QUAL_INFORMATION_REQUESTED_RELATED :        
            return SECURITY_STATUS_QUAL_STR_INFORMATION_REQUESTED_RELATED;   
        case SECURITY_STATUS_QUAL_NEWS_PENDING_RELATED:        
            return SECURITY_STATUS_QUAL_STR_NEWS_PENDING_RELATED;          
        case SECURITY_STATUS_QUAL_EQUIPMENT_CHANGEOVER_RELATED:        
            return SECURITY_STATUS_QUAL_STR_EQUIPMENT_CHANGEOVER_RELATED;          
        case SECURITY_STATUS_QUAL_SUB_PENNY_TRADING_RELATED  :        
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

    MamdaSecurityStatusQual 
    mamdaSecurityStatusQualFromString (const char*  securityStatusQual)
    {
        if (securityStatusQual == NULL)
        {
           return SECURITY_STATUS_QUAL_UNKNOWN;
        }

        // Older FH configurations sent strings:
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_NONE) == 0)
            return SECURITY_STATUS_QUAL_NONE;
          if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_OPENING) == 0)
            return SECURITY_STATUS_QUAL_OPENING;
         if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_OPENING) == 0)
            return SECURITY_STATUS_QUAL_OPENING;    
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_EXCUSED) == 0)
            return SECURITY_STATUS_QUAL_EXCUSED; 
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_WITHDRAWN) == 0)
            return SECURITY_STATUS_QUAL_WITHDRAWN; 
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_SUSPENDED) == 0)
            return SECURITY_STATUS_QUAL_SUSPENDED;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_RESUME) == 0)
            return SECURITY_STATUS_QUAL_RESUME;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_QUOTE_RESUME) == 0)
            return SECURITY_STATUS_QUAL_QUOTE_RESUME;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_TRADE_RESUME) == 0)
            return SECURITY_STATUS_QUAL_TRADE_RESUME;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_RESUME_TIME) == 0)
            return SECURITY_STATUS_QUAL_RESUME_TIME; 
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_MKT_IMB_BUY) == 0)
            return SECURITY_STATUS_QUAL_MKT_IMB_BUY; 
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_MKT_IMB_SELL) == 0)
            return SECURITY_STATUS_QUAL_MKT_IMB_SELL; 
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_NO_MKT_IMB) == 0)
            return SECURITY_STATUS_QUAL_NO_MKT_IMB;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_MOC_IMB_BUY) == 0)
            return SECURITY_STATUS_QUAL_MOC_IMB_BUY;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_MOC_IMB_SELL) == 0)
            return SECURITY_STATUS_QUAL_MOC_IMB_SELL;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_NO_MOC_IMB) == 0)
            return SECURITY_STATUS_QUAL_NO_MOC_IMB;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_ORDER_IMB) == 0)
            return SECURITY_STATUS_QUAL_ORDER_IMB;  
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_ORDER_INF) == 0)
            return SECURITY_STATUS_QUAL_ORDER_INF; 
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_ORDER_IMB_BUY) == 0)
            return SECURITY_STATUS_QUAL_ORDER_IMB_BUY;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_ORDER_IMB_SELL) == 0)
            return SECURITY_STATUS_QUAL_ORDER_IMB_SELL;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_ORDER_IMB_NONE) == 0)
            return SECURITY_STATUS_QUAL_ORDER_IMB_NONE;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_LOA_IMBALANCE_BUY) == 0)
            return SECURITY_STATUS_QUAL_LOA_IMBALANCE_BUY;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_LOA_IMBALANCE_SELL) == 0)
            return SECURITY_STATUS_QUAL_LOA_IMBALANCE_SELL;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_NO_LOA_IMBALANCE) == 0)
            return SECURITY_STATUS_QUAL_NO_LOA_IMBALANCE;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_ORDERS_ELIMINATED) == 0)
            return SECURITY_STATUS_QUAL_ORDERS_ELIMINATED;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_RANGE_ID) == 0)
            return SECURITY_STATUS_QUAL_RANGE_ID;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_ITS_PREOPEN) == 0)
            return SECURITY_STATUS_QUAL_ITS_PREOPEN;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_RESERVED) == 0)
            return SECURITY_STATUS_QUAL_RESERVED;  
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_FROZEN) == 0)
            return SECURITY_STATUS_QUAL_FROZEN; 
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_PREOPEN) == 0)
            return SECURITY_STATUS_QUAL_PREOPEN; 
        if (strcmp (securityStatusQual,  SECURITY_STATUS_QUAL_STR_THO_IMBALANCE_BUY) == 0)
            return  SECURITY_STATUS_QUAL_THO_IMBALANCE_BUY ;
        if (strcmp (securityStatusQual,  SECURITY_STATUS_QUAL_STR_THO_IMBALANCE_SELL) == 0)
            return  SECURITY_STATUS_QUAL_THO_IMBALANCE_SELL;
        if (strcmp (securityStatusQual,  SECURITY_STATUS_QUAL_STR_NO_THO_IMBALANCE ) == 0)
            return  SECURITY_STATUS_QUAL_NO_THO_IMBALANCE;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_ADD_INFO) == 0)
            return SECURITY_STATUS_QUAL_ADD_INFO;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_IPO_IMBALANCE_BUY) == 0)
            return SECURITY_STATUS_QUAL_IPO_IMBALANCE_BUY;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_IPO_IMBALANCE_SELL) == 0)
            return SECURITY_STATUS_QUAL_IPO_IMBALANCE_SELL;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_NO_IPO_IMBALANCE) == 0)
            return SECURITY_STATUS_QUAL_NO_IPO_IMBALANCE;    
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_OPEN_DELAY) == 0)
            return SECURITY_STATUS_QUAL_OPEN_DELAY;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_NO_OPEN_NO_RESUME) == 0)
            return SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_PRICE_IND) == 0)
            return SECURITY_STATUS_QUAL_PRICE_IND;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_EQUIPMENT) == 0)
            return SECURITY_STATUS_QUAL_EQUIPMENT;  
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_FILINGS) == 0)
            return SECURITY_STATUS_QUAL_FILINGS; 
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_NEWS) == 0)
            return SECURITY_STATUS_QUAL_NEWS; 
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_NEWS_DISSEM) == 0)
            return SECURITY_STATUS_QUAL_NEWS_DISSEM;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_LISTING) == 0)
            return SECURITY_STATUS_QUAL_LISTING;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_OPERATION) == 0)
            return SECURITY_STATUS_QUAL_OPERATION;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_INFO) == 0)
            return SECURITY_STATUS_QUAL_INFO;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_SEC) == 0)
            return SECURITY_STATUS_QUAL_SEC;  
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_TIMES) == 0)
            return SECURITY_STATUS_QUAL_TIMES; 
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_OTHER) == 0)
            return SECURITY_STATUS_QUAL_OTHER; 
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_RELATED) == 0)
            return SECURITY_STATUS_QUAL_RELATED;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_IPO) == 0)
            return SECURITY_STATUS_QUAL_IPO;

        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_EMC_IMB_BUY) == 0)               
            return SECURITY_STATUS_QUAL_EMC_IMB_BUY;           
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_EMC_IMB_SELL) == 0)              
            return SECURITY_STATUS_QUAL_EMC_IMB_SELL;          
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_EMC_IMB_NONE) == 0)              
            return SECURITY_STATUS_QUAL_EMC_IMB_NONE;          
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_PRE_CROSS) == 0)                 
            return SECURITY_STATUS_QUAL_PRE_CROSS;             
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_CROSS) == 0)                     
            return SECURITY_STATUS_QUAL_CROSS;                 
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_RELEASED_FOR_QUOTATION) == 0)    
            return SECURITY_STATUS_QUAL_RELEASED_FOR_QUOTATION;
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_IPO_WINDOW_EXT) == 0)            
            return SECURITY_STATUS_QUAL_IPO_WINDOW_EXT;        
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_PRECLOSING) == 0)                
            return SECURITY_STATUS_QUAL_PRECLOSING;            
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_AUCTION_EXTENSION) == 0)         
            return SECURITY_STATUS_QUAL_AUCTION_EXTENSION;     
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_VOLATILITY_AUCTION) == 0)        
            return SECURITY_STATUS_QUAL_VOLATILITY_AUCTION;    
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_SECURITY_AUTHORISED) == 0)       
            return SECURITY_STATUS_QUAL_SECURITY_AUTHORISED;   
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_SECURITY_FORBIDDEN) == 0)        
            return SECURITY_STATUS_QUAL_SECURITY_FORBIDDEN;    
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_FAST_MARKET) == 0)               
            return SECURITY_STATUS_QUAL_FAST_MARKET;           
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_SLOW_MARKET) == 0)           
            return SECURITY_STATUS_QUAL_SLOW_MARKET;           
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_SUB_PENNY_TRADING) == 0)     
            return SECURITY_STATUS_QUAL_SUB_PENNY_TRADING;     
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_ORDER_INPUT) == 0)           
            return SECURITY_STATUS_QUAL_ORDER_INPUT;           
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_PRE_ORDER_MATCHING) == 0)    
            return SECURITY_STATUS_QUAL_PRE_ORDER_MATCHING;    
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_ORDER_MATCHING) == 0)        
            return SECURITY_STATUS_QUAL_ORDER_MATCHING;        
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_BLOCKING) == 0)              
            return SECURITY_STATUS_QUAL_BLOCKING;              
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_ORDER_CANCEL) == 0)          
            return SECURITY_STATUS_QUAL_ORDER_CANCEL;          
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_FIXED_PRICE) == 0)           
            return SECURITY_STATUS_QUAL_FIXED_PRICE;           
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_SALES_INPUT) == 0)           
            return SECURITY_STATUS_QUAL_SALES_INPUT;           
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_EXCHANGE_INTERVENTION) == 0) 
            return SECURITY_STATUS_QUAL_EXCHANGE_INTERVENTION; 
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_PRE_AUCTION) == 0)           
            return SECURITY_STATUS_QUAL_PRE_AUCTION;           
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_ADJUST) == 0)                
            return SECURITY_STATUS_QUAL_ADJUST;                
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_ADJUST_ON) == 0)             
            return SECURITY_STATUS_QUAL_ADJUST_ON;             
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_LATE_TRADING) == 0)          
            return SECURITY_STATUS_QUAL_LATE_TRADING;          
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_ENQUIRE) == 0)               
            return SECURITY_STATUS_QUAL_ENQUIRE;               
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_PRE_NIGHT_TRADING) == 0)     
            return SECURITY_STATUS_QUAL_PRE_NIGHT_TRADING;     
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_OPEN_NIGHT_TRADING) == 0)    
            return SECURITY_STATUS_QUAL_OPEN_NIGHT_TRADING; 
        if (strcmp (securityStatusQual,SECURITY_STATUS_QUAL_STR_SUB_HOLIDAY) == 0)    
            return SECURITY_STATUS_QUAL_SUB_HOLIDAY; 
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_BID_ONLY) == 0)              
            return SECURITY_STATUS_QUAL_BID_ONLY;              
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_ASK_ONLY) == 0)         
            return SECURITY_STATUS_QUAL_ASK_ONLY;   
        if (strcmp (securityStatusQual,SECURITY_STATUS_QUAL_STR_OPENING_DELAY_COMMON) == 0)         
            return  SECURITY_STATUS_QUAL_OPENING_DELAY_COMMON ;  
        if (strcmp (securityStatusQual,SECURITY_STATUS_QUAL_STR_RESUME_COMMON) == 0)         
            return  SECURITY_STATUS_QUAL_RESUME_COMMON ;    
        if (strcmp (securityStatusQual,SECURITY_STATUS_QUAL_STR_NO_OPEN_NO_RESUME_COMMON) == 0)         
            return  SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME_COMMON ;    
        if (strcmp (securityStatusQual,SECURITY_STATUS_QUAL_STR_NEWS_DISSEMINATION_RELATED) == 0)         
            return  SECURITY_STATUS_QUAL_NEWS_DISSEMINATION_RELATED ;    
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_ORDER_INFLUX_RELATED) == 0)         
            return  SECURITY_STATUS_QUAL_ORDER_INFLUX_RELATED;  
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_ORDER_IMBALANCE_RELATED) == 0)         
            return  SECURITY_STATUS_QUAL_ORDER_IMBALANCE_RELATED ;  
        if (strcmp (securityStatusQual,SECURITY_STATUS_QUAL_STR_INFORMATION_REQUESTED_RELATED) == 0)         
            return  SECURITY_STATUS_QUAL_INFORMATION_REQUESTED_RELATED;  
        if (strcmp (securityStatusQual,SECURITY_STATUS_QUAL_STR_NEWS_PENDING_RELATED) == 0)         
            return  SECURITY_STATUS_QUAL_NEWS_PENDING_RELATED;   
        if (strcmp (securityStatusQual,SECURITY_STATUS_QUAL_STR_EQUIPMENT_CHANGEOVER_RELATED) == 0)         
            return  SECURITY_STATUS_QUAL_EQUIPMENT_CHANGEOVER_RELATED;   
        if (strcmp (securityStatusQual,SECURITY_STATUS_QUAL_STR_SUB_PENNY_TRADING_RELATED) == 0)         
            return  SECURITY_STATUS_QUAL_SUB_PENNY_TRADING_RELATED;   
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_IMBALANCE_PREOPEN_BUY) == 0)         
            return  SECURITY_STATUS_QUAL_IMBALANCE_PREOPEN_BUY;       
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_IMBALANCE_PREOPEN_SELL) == 0)         
            return  SECURITY_STATUS_QUAL_IMBALANCE_PREOPEN_SELL;     
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_IMBALANCE_PRECLOSE_BUY) == 0)         
            return  SECURITY_STATUS_QUAL_IMBALANCE_PRECLOSE_BUY;       
        if (strcmp (securityStatusQual, SECURITY_STATUS_QUAL_STR_IMBALANCE_PRECLOSE_SELL) == 0)         
            return  SECURITY_STATUS_QUAL_IMBALANCE_PRECLOSE_SELL; 
        if(strcmp(securityStatusQual, SECURITY_STATUS_QUAL_STR_VOLATILITY_PAUSE) == 0)
            return SECURITY_STATUS_QUAL_VOLATILITY_PAUSE;
        if(strcmp(securityStatusQual,SECURITY_STATUS_QUAL_STR_VOLATILITY_PAUSE_QUOTE_RESUME) == 0)
            return SECURITY_STATUS_QUAL_VOLATILITY_PAUSE_QUOTE_RESUME;
        if(strcmp(securityStatusQual, SECURITY_STATUS_QUAL_STR_VOLATILITY_GUARD) == 0)
            return SECURITY_STATUS_QUAL_VOLATILITY_GUARD;
        if(strcmp(securityStatusQual,SECURITY_STATUS_QUAL_STR_VOLATILITY_GUARD_QUOTE_RESUME) == 0)
            return SECURITY_STATUS_QUAL_VOLATILITY_GUARD_QUOTE_RESUME; 
                 
            

        // A misconfigured FH might send numbers as strings:
        if (strcmp (securityStatusQual, "0") == 0)
            return SECURITY_STATUS_QUAL_NONE;
        if (strcmp (securityStatusQual, "7") == 0)
            return SECURITY_STATUS_QUAL_EXCUSED; 
        if (strcmp (securityStatusQual, "8") == 0)
            return SECURITY_STATUS_QUAL_WITHDRAWN; 
        if (strcmp (securityStatusQual, "9") == 0)
            return SECURITY_STATUS_QUAL_SUSPENDED;
        if (strcmp (securityStatusQual, "11") == 0)
            return SECURITY_STATUS_QUAL_RESUME;
        if (strcmp (securityStatusQual, "12") == 0)
            return SECURITY_STATUS_QUAL_QUOTE_RESUME;
        if (strcmp (securityStatusQual, "13") == 0)
            return SECURITY_STATUS_QUAL_TRADE_RESUME;
        if (strcmp (securityStatusQual, "14") == 0)
            return SECURITY_STATUS_QUAL_RESUME_TIME; 
        if (strcmp (securityStatusQual, "16") == 0)
            return SECURITY_STATUS_QUAL_MKT_IMB_BUY; 
        if (strcmp (securityStatusQual, "17") == 0)
            return SECURITY_STATUS_QUAL_MKT_IMB_SELL; 
        if (strcmp (securityStatusQual, "18") == 0)
            return SECURITY_STATUS_QUAL_NO_MKT_IMB;
        if (strcmp (securityStatusQual, "19") == 0)
            return SECURITY_STATUS_QUAL_MOC_IMB_BUY;
        if (strcmp (securityStatusQual, "20") == 0)
            return SECURITY_STATUS_QUAL_MOC_IMB_SELL;
        if (strcmp (securityStatusQual, "21") == 0)
            return SECURITY_STATUS_QUAL_NO_MOC_IMB;
        if (strcmp (securityStatusQual, "22") == 0)
            return SECURITY_STATUS_QUAL_ORDER_IMB;  
        if (strcmp (securityStatusQual, "23") == 0)
            return SECURITY_STATUS_QUAL_ORDER_INF; 
        if (strcmp (securityStatusQual, "24") == 0)
            return SECURITY_STATUS_QUAL_ORDER_IMB_BUY;
        if (strcmp (securityStatusQual, "25") == 0)
            return SECURITY_STATUS_QUAL_ORDER_IMB_SELL;
        if (strcmp (securityStatusQual, "26") == 0)
            return SECURITY_STATUS_QUAL_ORDER_IMB_NONE;
        if (strcmp (securityStatusQual, "27") == 0)
            return SECURITY_STATUS_QUAL_LOA_IMBALANCE_BUY;
        if (strcmp (securityStatusQual, "28") == 0)
            return SECURITY_STATUS_QUAL_LOA_IMBALANCE_SELL;
        if (strcmp (securityStatusQual, "29") == 0)
            return SECURITY_STATUS_QUAL_NO_LOA_IMBALANCE;
        if (strcmp (securityStatusQual, "30") == 0)
            return SECURITY_STATUS_QUAL_ORDERS_ELIMINATED;
        if (strcmp (securityStatusQual, "31") == 0)
            return SECURITY_STATUS_QUAL_RANGE_ID;
        if (strcmp (securityStatusQual, "32") == 0)
            return SECURITY_STATUS_QUAL_ITS_PREOPEN;
        if (strcmp (securityStatusQual, "33") == 0)
            return SECURITY_STATUS_QUAL_RESERVED;  
        if (strcmp (securityStatusQual, "34") == 0)
            return SECURITY_STATUS_QUAL_FROZEN; 
        if (strcmp (securityStatusQual, "35") == 0)
            return SECURITY_STATUS_QUAL_PREOPEN; 
        if (strcmp (securityStatusQual, "36") == 0)
            return  SECURITY_STATUS_QUAL_THO_IMBALANCE_BUY;
        if (strcmp (securityStatusQual, "37") == 0)
            return  SECURITY_STATUS_QUAL_THO_IMBALANCE_SELL;
        if (strcmp (securityStatusQual, "38") == 0)
            return  SECURITY_STATUS_QUAL_NO_THO_IMBALANCE;
        if (strcmp (securityStatusQual, "41") == 0)
            return SECURITY_STATUS_QUAL_ADD_INFO;
        if (strcmp (securityStatusQual, "42") == 0)
            return SECURITY_STATUS_QUAL_IPO_IMBALANCE_BUY;
        if (strcmp (securityStatusQual, "43") == 0)
            return SECURITY_STATUS_QUAL_IPO_IMBALANCE_SELL; 
        if (strcmp (securityStatusQual, "44") == 0)
            return SECURITY_STATUS_QUAL_NO_IPO_IMBALANCE; 
        if (strcmp (securityStatusQual, "51") == 0)
            return SECURITY_STATUS_QUAL_OPEN_DELAY;
        if (strcmp (securityStatusQual, "52") == 0)
            return SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME;
        if (strcmp (securityStatusQual, "53") == 0)
            return SECURITY_STATUS_QUAL_PRICE_IND;
        if (strcmp (securityStatusQual, "54") == 0)
            return SECURITY_STATUS_QUAL_EQUIPMENT;  
        if (strcmp (securityStatusQual, "55") == 0)
            return SECURITY_STATUS_QUAL_FILINGS; 
        if (strcmp (securityStatusQual, "56") == 0)
            return SECURITY_STATUS_QUAL_NEWS; 
        if (strcmp (securityStatusQual, "57") == 0)
            return SECURITY_STATUS_QUAL_NEWS_DISSEM;
        if (strcmp (securityStatusQual, "58") == 0)
            return SECURITY_STATUS_QUAL_LISTING;
        if (strcmp (securityStatusQual, "59") == 0)
            return SECURITY_STATUS_QUAL_OPERATION;
        if (strcmp (securityStatusQual, "60") == 0)
            return SECURITY_STATUS_QUAL_INFO;
        if (strcmp (securityStatusQual, "61") == 0)
            return SECURITY_STATUS_QUAL_SEC;  
        if (strcmp (securityStatusQual, "62") == 0)
            return SECURITY_STATUS_QUAL_TIMES; 
        if (strcmp (securityStatusQual, "62") == 0)
            return SECURITY_STATUS_QUAL_OTHER; 
        if (strcmp (securityStatusQual, "64") == 0)
            return SECURITY_STATUS_QUAL_RELATED;
        if (strcmp (securityStatusQual, "65") == 0)
            return SECURITY_STATUS_QUAL_IPO;
        if (strcmp (securityStatusQual, "46") == 0)               
            return SECURITY_STATUS_QUAL_EMC_IMB_BUY;           
        if (strcmp (securityStatusQual, "47") == 0)              
            return SECURITY_STATUS_QUAL_EMC_IMB_SELL;          
        if (strcmp (securityStatusQual, "48") == 0)              
            return SECURITY_STATUS_QUAL_EMC_IMB_NONE;          
        if (strcmp (securityStatusQual, "66") == 0)                 
            return SECURITY_STATUS_QUAL_PRE_CROSS;             
        if (strcmp (securityStatusQual, "67") == 0)                     
            return SECURITY_STATUS_QUAL_CROSS;                 
        if (strcmp (securityStatusQual, "68") == 0)    
            return SECURITY_STATUS_QUAL_RELEASED_FOR_QUOTATION;
        if (strcmp (securityStatusQual, "69") == 0)            
            return SECURITY_STATUS_QUAL_IPO_WINDOW_EXT;        
        if (strcmp (securityStatusQual, "70") == 0)                
            return SECURITY_STATUS_QUAL_PRECLOSING;            
        if (strcmp (securityStatusQual, "71") == 0)         
            return SECURITY_STATUS_QUAL_AUCTION_EXTENSION;     
        if (strcmp (securityStatusQual, "72") == 0)        
            return SECURITY_STATUS_QUAL_VOLATILITY_AUCTION;    
        if (strcmp (securityStatusQual, "73") == 0)       
            return SECURITY_STATUS_QUAL_SECURITY_AUTHORISED;   
        if (strcmp (securityStatusQual, "74") == 0)        
            return SECURITY_STATUS_QUAL_SECURITY_FORBIDDEN;    
        if (strcmp (securityStatusQual, "75") == 0)               
            return SECURITY_STATUS_QUAL_FAST_MARKET;           
        if (strcmp (securityStatusQual, "76") == 0)           
            return SECURITY_STATUS_QUAL_SLOW_MARKET;           
        if (strcmp (securityStatusQual, "77") == 0)     
            return SECURITY_STATUS_QUAL_SUB_PENNY_TRADING;     
        if (strcmp (securityStatusQual, "78") == 0)           
            return SECURITY_STATUS_QUAL_ORDER_INPUT;           
        if (strcmp (securityStatusQual, "79") == 0)    
            return SECURITY_STATUS_QUAL_PRE_ORDER_MATCHING;    
        if (strcmp (securityStatusQual, "80") == 0)        
            return SECURITY_STATUS_QUAL_ORDER_MATCHING;        
        if (strcmp (securityStatusQual, "81") == 0)              
            return SECURITY_STATUS_QUAL_BLOCKING;              
        if (strcmp (securityStatusQual, "82") == 0)          
            return SECURITY_STATUS_QUAL_ORDER_CANCEL;          
        if (strcmp (securityStatusQual, "83") == 0)           
            return SECURITY_STATUS_QUAL_FIXED_PRICE;           
        if (strcmp (securityStatusQual, "84") == 0)           
            return SECURITY_STATUS_QUAL_SALES_INPUT;           
        if (strcmp (securityStatusQual, "85") == 0) 
            return SECURITY_STATUS_QUAL_EXCHANGE_INTERVENTION; 
        if (strcmp (securityStatusQual, "86") == 0)           
            return SECURITY_STATUS_QUAL_PRE_AUCTION;           
        if (strcmp (securityStatusQual, "87") == 0)                
            return SECURITY_STATUS_QUAL_ADJUST;                
        if (strcmp (securityStatusQual, "88") == 0)             
            return SECURITY_STATUS_QUAL_ADJUST_ON;             
        if (strcmp (securityStatusQual, "89") == 0)          
            return SECURITY_STATUS_QUAL_LATE_TRADING;          
        if (strcmp (securityStatusQual, "90") == 0)               
            return SECURITY_STATUS_QUAL_ENQUIRE;               
        if (strcmp (securityStatusQual, "91") == 0)     
            return SECURITY_STATUS_QUAL_PRE_NIGHT_TRADING;     
        if (strcmp (securityStatusQual, "92") == 0)    
            return SECURITY_STATUS_QUAL_OPEN_NIGHT_TRADING;   
        if (strcmp (securityStatusQual, "93") == 0)    
            return  SECURITY_STATUS_QUAL_SUB_HOLIDAY;   
        if (strcmp (securityStatusQual, "94") == 0)              
            return SECURITY_STATUS_QUAL_BID_ONLY;              
        if (strcmp (securityStatusQual, "95") == 0)         
            return SECURITY_STATUS_QUAL_ASK_ONLY; 
        if (strcmp (securityStatusQual, "101") == 0)         
            return  SECURITY_STATUS_QUAL_OPENING_DELAY_COMMON;     
        if (strcmp (securityStatusQual, "102") == 0)         
            return  SECURITY_STATUS_QUAL_RESUME_COMMON ; 
        if (strcmp (securityStatusQual, "103") == 0)         
            return  SECURITY_STATUS_QUAL_NO_OPEN_NO_RESUME_COMMON; 
        if (strcmp (securityStatusQual, "104") == 0)         
            return SECURITY_STATUS_QUAL_NEWS_DISSEMINATION_RELATED;     
        if (strcmp (securityStatusQual, "105") == 0)         
            return SECURITY_STATUS_QUAL_ORDER_INFLUX_RELATED;     
        if (strcmp (securityStatusQual, "106") == 0)         
            return SECURITY_STATUS_QUAL_ORDER_IMBALANCE_RELATED;     
        if (strcmp (securityStatusQual, "107") == 0)         
            return  SECURITY_STATUS_QUAL_INFORMATION_REQUESTED_RELATED;    
        if (strcmp (securityStatusQual, "108") == 0)         
            return  SECURITY_STATUS_QUAL_NEWS_PENDING_RELATED; 
        if (strcmp (securityStatusQual, "109") == 0)         
            return SECURITY_STATUS_QUAL_EQUIPMENT_CHANGEOVER_RELATED; 
        if (strcmp (securityStatusQual, "110") == 0)         
            return SECURITY_STATUS_QUAL_SUB_PENNY_TRADING_RELATED;    
        if (strcmp (securityStatusQual, "112") == 0)         
            return  SECURITY_STATUS_QUAL_IMBALANCE_PREOPEN_BUY;  
        if (strcmp (securityStatusQual, "113") == 0)         
            return  SECURITY_STATUS_QUAL_IMBALANCE_PREOPEN_SELL;  
        if (strcmp (securityStatusQual, "114") == 0)         
            return  SECURITY_STATUS_QUAL_IMBALANCE_PRECLOSE_BUY;  
        if (strcmp (securityStatusQual, "115") == 0)         
            return  SECURITY_STATUS_QUAL_IMBALANCE_PRECLOSE_SELL;  
        if(strcmp(securityStatusQual, "116") == 0)
            return SECURITY_STATUS_QUAL_VOLATILITY_PAUSE;
        if(strcmp(securityStatusQual, "117") == 0)
            return SECURITY_STATUS_QUAL_VOLATILITY_PAUSE_QUOTE_RESUME; 
        if(strcmp(securityStatusQual, "118") == 0)
            return SECURITY_STATUS_QUAL_VOLATILITY_GUARD;
        if(strcmp(securityStatusQual, "119") == 0)
            return SECURITY_STATUS_QUAL_VOLATILITY_GUARD_QUOTE_RESUME; 

        return SECURITY_STATUS_QUAL_UNKNOWN;
    }

}
