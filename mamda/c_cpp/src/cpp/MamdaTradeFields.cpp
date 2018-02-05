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

#include <mamda/MamdaTradeFields.h>
#include <mamda/MamdaCommonFields.h>
#include <mama/mamacpp.h>

namespace Wombat
{

    static uint16_t theMaxFid = 0;
    static bool initialised = false;

    const MamaFieldDescriptor*  MamdaTradeFields::TRADE_PRICE               = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::TRADE_DATE                = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::TRADE_SIDE                = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::AGGRESSOR_SIDE            = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::TRADE_TIME                = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::LAST_PRICE                = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::LAST_VOLUME               = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::LAST_DATE_TIME            = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::LAST_PART_ID              = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::LAST_DIRECTION            = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::NET_CHANGE                = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::PCT_CHANGE                = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::TRADE_SIZE                = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::TOTAL_VOLUME              = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::OFF_EXCHANGE_TOTAL_VOLUME = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::ON_EXCHANGE_TOTAL_VOLUME  = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::TRADE_UNITS               = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::HIGH_PRICE                = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::LOW_PRICE                 = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::OPEN_PRICE                = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::CLOSE_PRICE               = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::CLOSE_DATE                = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::PREV_CLOSE_PRICE          = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::PREV_CLOSE_DATE           = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::ADJ_PREV_CLOSE            = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::PREV_VOLUME               = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::TRADE_SEQNUM              = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::TRADE_QUALIFIER           = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::TRADE_PART_ID             = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::TOTAL_VALUE               = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::OFF_EXCHANGE_TOTAL_VALUE  = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::ON_EXCHANGE_TOTAL_VALUE   = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::VWAP                      = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::OFF_EXCHANGE_VWAP         = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::ON_EXCHANGE_VWAP          = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::STD_DEV                   = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::STD_DEV_SUM               = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::STD_DEV_SUM_SQUARES       = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::SALE_CONDITION            = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::SELLERS_SALE_DAYS         = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::IS_IRREGULAR              = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::IRREG_PART_ID             = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::IRREG_PRICE               = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::IRREG_SIZE                = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::IRREG_TIME                = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::ORIG_PART_ID              = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::ORIG_PRICE                = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::ORIG_SIZE                 = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::ORIG_SEQNUM               = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::ORIG_TRADE_QUALIFIER      = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::ORIG_TRADE_ID             = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::ORIG_SALE_CONDITION       = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::ORIG_SELLERS_SALE_DAYS    = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::ORIG_STOP_STOCK_IND       = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::STOP_STOCK_IND            = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::CORR_PART_ID              = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::CORR_PRICE                = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::CORR_SIZE                 = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::CORR_TRADE_QUALIFIER      = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::CORR_TRADE_ID             = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::CORR_SALE_CONDITION       = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::CORR_SELLERS_SALE_DAYS    = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::CORR_STOP_STOCK_IND       = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::CORR_TIME                 = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::CANCEL_TIME               = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::TRADE_ID                  = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::PRIMARY_EXCH              = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::TRADE_COUNT               = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::BLOCK_COUNT               = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::BLOCK_VOLUME              = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::ORDER_ID                  = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::UPDATE_AS_TRADE           = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::CURRENCY_CODE             = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::SETTLE_PRICE              = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::SETTLE_DATE               = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::HIGH_SEQNUM               = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::LOW_SEQNUM                = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::LAST_SEQNUM               = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::TOTAL_VOLUME_SEQNUM       = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::UNIQUE_ID                 = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::TRADE_ACTION              = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::TRADE_EXEC_VENUE          = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::OFF_EXCHANGE_TRADE_PRICE  = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::ON_EXCHANGE_TRADE_PRICE   = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::TRADE_CONTRIBUTORS        = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::GENERIC_FLAG              = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::TRADE_RECAPS              = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::SHORT_SALE_CIRCUIT_BREAKER        = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::ORIG_SHORT_SALE_CIRCUIT_BREAKER   = NULL;
    const MamaFieldDescriptor*  MamdaTradeFields::CORR_SHORT_SALE_CIRCUIT_BREAKER   = NULL;

    void MamdaTradeFields::setDictionary (const MamaDictionary&  dictionary)
    {
        // already initialised
        if (initialised)
        {
            return;
        }

        //set the Common Fields
        if (!MamdaCommonFields::isSet())
        {
            MamdaCommonFields::setDictionary (dictionary);
        }

        TRADE_PRICE                 = dictionary.getFieldByName ("wTradePrice");
        TRADE_DATE                  = dictionary.getFieldByName ("wTradeDate");
        TRADE_SIDE                  = dictionary.getFieldByName ("wTradeSide");
        AGGRESSOR_SIDE              = dictionary.getFieldByName ("wAggressorSide");
        TRADE_TIME                  = dictionary.getFieldByName ("wTradeTime");
        LAST_PRICE                  = dictionary.getFieldByName ("wLastPrice");
        LAST_VOLUME                 = dictionary.getFieldByName ("wLastVolume");
        LAST_DATE_TIME              = dictionary.getFieldByName ("wLastTime");
        LAST_PART_ID                = dictionary.getFieldByName ("wLastPartId");
        LAST_DIRECTION              = dictionary.getFieldByName ("wTradeTick");
        NET_CHANGE                  = dictionary.getFieldByName ("wNetChange");
        PCT_CHANGE                  = dictionary.getFieldByName ("wPctChange");
        TRADE_SIZE                  = dictionary.getFieldByName ("wTradeVolume");
        TOTAL_VOLUME                = dictionary.getFieldByName ("wTotalVolume");
        OFF_EXCHANGE_TOTAL_VOLUME   = dictionary.getFieldByName ("wOffExchangeTotalVolume");
        ON_EXCHANGE_TOTAL_VOLUME    = dictionary.getFieldByName ("wOnExchangeTotalVolume");
        TRADE_UNITS                 = dictionary.getFieldByName ("wTradeUnits");
        HIGH_PRICE                  = dictionary.getFieldByName ("wHighPrice");
        LOW_PRICE                   = dictionary.getFieldByName ("wLowPrice");
        OPEN_PRICE                  = dictionary.getFieldByName ("wOpenPrice");
        CLOSE_PRICE                 = dictionary.getFieldByName ("wClosePrice");
        CLOSE_DATE                  = dictionary.getFieldByName ("wCloseDate");
        PREV_CLOSE_PRICE            = dictionary.getFieldByName ("wPrevClosePrice");
        PREV_CLOSE_DATE             = dictionary.getFieldByName ("wPrevCloseDate");
        ADJ_PREV_CLOSE              = dictionary.getFieldByName ("wAdjPrevClose");
        PREV_VOLUME                 = dictionary.getFieldByName ("wPrevVolume");
        TRADE_SEQNUM                = dictionary.getFieldByName ("wTradeSeqNum");
        TRADE_QUALIFIER             = dictionary.getFieldByName ("wTradeQualifier");
        TRADE_PART_ID               = dictionary.getFieldByName ("wTradePartId");
        TOTAL_VALUE                 = dictionary.getFieldByName ("wTotalValue");
        OFF_EXCHANGE_TOTAL_VALUE    = dictionary.getFieldByName ("wOffExchangeTotalValue");
        ON_EXCHANGE_TOTAL_VALUE     = dictionary.getFieldByName ("wOnExchangeTotalValue");
        VWAP                        = dictionary.getFieldByName ("wVwap");
        OFF_EXCHANGE_VWAP           = dictionary.getFieldByName ("wOffExchangeVWAP");
        ON_EXCHANGE_VWAP            = dictionary.getFieldByName ("wOnExchangeVWAP");
        STD_DEV                     = dictionary.getFieldByName ("wStdDev");
        STD_DEV_SUM                 = dictionary.getFieldByName ("wStdDevSum");
        STD_DEV_SUM_SQUARES         = dictionary.getFieldByName ("wStdDevSumSquares");
        SALE_CONDITION              = dictionary.getFieldByName ("wSaleCondition");
        SELLERS_SALE_DAYS           = dictionary.getFieldByName ("wSellersSaleDays");
        IS_IRREGULAR                = dictionary.getFieldByName ("wIsIrregular");
        IRREG_PART_ID               = dictionary.getFieldByName ("wIrregPartId");
        IRREG_PRICE                 = dictionary.getFieldByName ("wIrregPrice");
        IRREG_SIZE                  = dictionary.getFieldByName ("wIrregSize");
        IRREG_TIME                  = dictionary.getFieldByName ("wIrregTime");
        ORIG_PART_ID                = dictionary.getFieldByName ("wOrigPartId");
        ORIG_PRICE                  = dictionary.getFieldByName ("wOrigPrice");
        ORIG_SIZE                   = dictionary.getFieldByName ("wOrigSize");
        ORIG_SEQNUM                 = dictionary.getFieldByName ("wOrigSeqNum");
        ORIG_TRADE_QUALIFIER        = dictionary.getFieldByName ("wOrigQualifier");
        ORIG_TRADE_ID               = dictionary.getFieldByName ("wOrigTradeId");
        ORIG_SALE_CONDITION         = dictionary.getFieldByName ("wOrigCondition");
        ORIG_SELLERS_SALE_DAYS      = dictionary.getFieldByName ("wOrigSaleDays");
        ORIG_STOP_STOCK_IND         = dictionary.getFieldByName ("wOrigStopStockInd");
        STOP_STOCK_IND              = dictionary.getFieldByName ("wStopStockIndicator");
        CORR_PART_ID                = dictionary.getFieldByName ("wCorrPartId");
        CORR_PRICE                  = dictionary.getFieldByName ("wCorrPrice");
        CORR_SIZE                   = dictionary.getFieldByName ("wCorrSize");
        CORR_TRADE_QUALIFIER        = dictionary.getFieldByName ("wCorrQualifier");
        CORR_TRADE_ID               = dictionary.getFieldByName ("wCorrTradeId");
        CORR_SALE_CONDITION         = dictionary.getFieldByName ("wCorrCondition");
        CORR_SELLERS_SALE_DAYS      = dictionary.getFieldByName ("wCorrSaleDays");
        CORR_STOP_STOCK_IND         = dictionary.getFieldByName ("wCorrStopStockInd");
        CORR_TIME                   = dictionary.getFieldByName ("wCorrTime");
        CANCEL_TIME                 = dictionary.getFieldByName ("wCancelTime");
        TRADE_ID                    = dictionary.getFieldByName ("wTradeId");
        PRIMARY_EXCH                = dictionary.getFieldByName ("wPrimExch");
        TRADE_COUNT                 = dictionary.getFieldByName ("wTradeCount");
        BLOCK_COUNT                 = dictionary.getFieldByName ("wBlockCount");
        BLOCK_VOLUME                = dictionary.getFieldByName ("wBlockVolume");
        ORDER_ID                    = dictionary.getFieldByName ("wOrderId");
        UPDATE_AS_TRADE             = dictionary.getFieldByName ("wUpdateAsTrade");
        CURRENCY_CODE               = dictionary.getFieldByName ("wCurrency");
        SETTLE_PRICE                = dictionary.getFieldByName ("wSettlePrice");
        SETTLE_DATE                 = dictionary.getFieldByName ("wSettleDate");
        HIGH_SEQNUM                 = dictionary.getFieldByName ("wHighSeqNum");
        LOW_SEQNUM                  = dictionary.getFieldByName ("wLowSeqNum");
        LAST_SEQNUM                 = dictionary.getFieldByName ("wLastTradeSeqNum");
        TOTAL_VOLUME_SEQNUM         = dictionary.getFieldByName ("wTotalVolumeSeqNum"); 
        UNIQUE_ID                   = dictionary.getFieldByName ("wUniqueId");
        TRADE_ACTION                = dictionary.getFieldByName ("wTradeAction");
        TRADE_EXEC_VENUE            = dictionary.getFieldByName ("wTradeExecVenueEnum");
        OFF_EXCHANGE_TRADE_PRICE    = dictionary.getFieldByName ("wOffExchangeTradePrice");
        ON_EXCHANGE_TRADE_PRICE     = dictionary.getFieldByName ("wOnExchangeTradePrice");
        TRADE_CONTRIBUTORS          = dictionary.getFieldByName ("wTradeContributors");
        GENERIC_FLAG                = dictionary.getFieldByName ("wGenericFlag");
        TRADE_RECAPS                = dictionary.getFieldByName ("wTradeRecaps");

        SHORT_SALE_CIRCUIT_BREAKER          = dictionary.getFieldByName("wShortSaleCircuitBreaker");
        ORIG_SHORT_SALE_CIRCUIT_BREAKER     = dictionary.getFieldByName("wOrigShortSaleCircuitBreaker");
        CORR_SHORT_SALE_CIRCUIT_BREAKER     = dictionary.getFieldByName("wCorrShortSaleCircuitBreaker");

        theMaxFid = dictionary.getMaxFid();
        initialised = true;
    }

    void MamdaTradeFields::reset ()
    {
        initialised                 = false;

        //set the Common Fields
        if (MamdaCommonFields::isSet())
        {
            MamdaCommonFields::reset ();
        }

        theMaxFid                   = 0;
        TRADE_PRICE                 = NULL;
        TRADE_DATE                  = NULL;
        TRADE_SIDE                  = NULL;
        AGGRESSOR_SIDE              = NULL;
        TRADE_TIME                  = NULL;
        LAST_PRICE                  = NULL;
        LAST_VOLUME                 = NULL;
        LAST_DATE_TIME              = NULL;
        LAST_PART_ID                = NULL;
        LAST_DIRECTION              = NULL;
        NET_CHANGE                  = NULL;
        PCT_CHANGE                  = NULL;
        TRADE_SIZE                  = NULL;
        TOTAL_VOLUME                = NULL;
        OFF_EXCHANGE_TOTAL_VOLUME   = NULL;
        ON_EXCHANGE_TOTAL_VOLUME    = NULL;
        TRADE_UNITS                 = NULL;
        HIGH_PRICE                  = NULL;
        LOW_PRICE                   = NULL;
        OPEN_PRICE                  = NULL;
        CLOSE_PRICE                 = NULL;
        CLOSE_DATE                  = NULL;
        PREV_CLOSE_PRICE            = NULL;
        PREV_CLOSE_DATE             = NULL;
        ADJ_PREV_CLOSE              = NULL;
        PREV_VOLUME                 = NULL;
        TRADE_SEQNUM                = NULL;
        TRADE_QUALIFIER             = NULL;
        TRADE_PART_ID               = NULL;
        TOTAL_VALUE                 = NULL;
        OFF_EXCHANGE_TOTAL_VALUE    = NULL;
        ON_EXCHANGE_TOTAL_VALUE     = NULL;
        VWAP                        = NULL;
        OFF_EXCHANGE_VWAP           = NULL;
        ON_EXCHANGE_VWAP            = NULL;
        STD_DEV                     = NULL;
        STD_DEV_SUM                 = NULL;
        STD_DEV_SUM_SQUARES         = NULL;
        SALE_CONDITION              = NULL;
        SELLERS_SALE_DAYS           = NULL;
        IS_IRREGULAR                = NULL;
        IRREG_PART_ID               = NULL;
        IRREG_PRICE                 = NULL;
        IRREG_SIZE                  = NULL;
        IRREG_TIME                  = NULL;
        ORIG_PART_ID                = NULL;
        ORIG_PRICE                  = NULL;
        ORIG_SIZE                   = NULL;
        ORIG_SEQNUM                 = NULL;
        ORIG_TRADE_QUALIFIER        = NULL;
        ORIG_TRADE_ID               = NULL;
        ORIG_SALE_CONDITION         = NULL;
        ORIG_SELLERS_SALE_DAYS      = NULL;
        ORIG_STOP_STOCK_IND         = NULL;
        STOP_STOCK_IND              = NULL;
        CORR_PART_ID                = NULL;
        CORR_PRICE                  = NULL;
        CORR_SIZE                   = NULL;
        CORR_TRADE_QUALIFIER        = NULL;
        CORR_TRADE_ID               = NULL;
        CORR_SALE_CONDITION         = NULL;
        CORR_SELLERS_SALE_DAYS      = NULL;
        CORR_STOP_STOCK_IND         = NULL;
        CORR_TIME                   = NULL;
        CANCEL_TIME                 = NULL;
        TRADE_ID                    = NULL;
        PRIMARY_EXCH                = NULL;
        TRADE_COUNT                 = NULL;
        BLOCK_COUNT                 = NULL;
        BLOCK_VOLUME                = NULL;
        ORDER_ID                    = NULL;
        UPDATE_AS_TRADE             = NULL;
        CURRENCY_CODE               = NULL;
        SETTLE_PRICE                = NULL;
        SETTLE_DATE                 = NULL;
        HIGH_SEQNUM                 = NULL;
        LOW_SEQNUM                  = NULL;
        LAST_SEQNUM                 = NULL;
        TOTAL_VOLUME_SEQNUM         = NULL;
        UNIQUE_ID                   = NULL;
        TRADE_ACTION                = NULL;
        TRADE_EXEC_VENUE            = NULL;
        OFF_EXCHANGE_TRADE_PRICE    = NULL;
        ON_EXCHANGE_TRADE_PRICE     = NULL;
        TRADE_CONTRIBUTORS          = NULL;
        GENERIC_FLAG                = NULL;
        TRADE_RECAPS                = NULL;

        SHORT_SALE_CIRCUIT_BREAKER       = NULL;
        ORIG_SHORT_SALE_CIRCUIT_BREAKER  = NULL;
        CORR_SHORT_SALE_CIRCUIT_BREAKER  = NULL;
    }

    bool MamdaTradeFields::isSet ()
    {
        return initialised;
    }

    uint16_t  MamdaTradeFields::getMaxFid()
    {
        return theMaxFid;
    }

}
