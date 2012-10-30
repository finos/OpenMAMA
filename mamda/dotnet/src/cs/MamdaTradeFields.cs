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
using System.Collections.Specialized;

namespace Wombat
{
    /// <summary>
    /// Cache of common trade related field descriptors.
    /// This is required to be populated if using the MamdaTradeListener.
    /// </summary>
    public class MamdaTradeFields : MamdaFields
    {
        private MamdaTradeFields()
        {
        }

        /// <summary>
        /// Set the dictionary for common trade fields.
        /// Maintains a cache of MamaFieldDescriptors for common trade related
        /// fields. The <code>properties</code> parameter allows users of the API
        /// to map the common dictionary names to something else if they are being
        /// published under different names.
        /// </summary>
        /// <param name="dictionary">A reference to a valid MamaDictionary</param>
        /// <param name="properties">A NameValueCollection object containing field
        /// mappings. (See MamdaFields for further details)</param>
        public static void setDictionary(
            MamaDictionary  dictionary,
            NameValueCollection properties)
        {
            if (mInitialised)
            {
                return;
            }

            string wSymbol           = lookupFieldName(properties, "wSymbol");
            string wIssueSymbol      = lookupFieldName(properties, "wIssueSymbol");
            string wPartId           = lookupFieldName(properties, "wPartId");
            string wSrcTime          = lookupFieldName(properties, "wSrcTime");
            string wActivityTime     = lookupFieldName(properties, "wActivityTime");
            string wLineTime         = lookupFieldName(properties, "wLineTime");
            string MamaSendTime      = lookupFieldName(properties, MamaReservedFields.SendTime.getName());
            string wPubId            = lookupFieldName(properties, "wPubId");
            string wTradePrice       = lookupFieldName(properties, "wTradePrice");
            string wTradeDate        = lookupFieldName(properties, "wTradeDate");
            string wTradeTime        = lookupFieldName(properties, "wTradeTime");
            string wTradeTick        = lookupFieldName(properties, "wTradeTick");
            string wLastPrice        = lookupFieldName(properties, "wLastPrice");
            string wLastVolume       = lookupFieldName(properties, "wLastVolume");
            string wLastPartId       = lookupFieldName(properties, "wLastPartId");
            string wLastTime         = lookupFieldName(properties, "wLastTime");
            string wNetChange        = lookupFieldName(properties, "wNetChange");
            string wPctChange        = lookupFieldName(properties, "wPctChange");
            string wTradeVolume      = lookupFieldName(properties, "wTradeVolume");
            string wTotalVolume      = lookupFieldName(properties, "wTotalVolume");
            string wOffExTotalVolume = lookupFieldName(properties, "wOffExchangeTotalVolume");
            string wOnExTotalVolume  = lookupFieldName(properties, "wOnExchangeTotalVolume");
            string wTradeUnits       = lookupFieldName(properties, "wTradeUnits");
            string wHighPrice        = lookupFieldName(properties, "wHighPrice");
            string wLowPrice         = lookupFieldName(properties, "wLowPrice");
            string wOpenPrice        = lookupFieldName(properties, "wOpenPrice");
            string wClosePrice       = lookupFieldName(properties, "wClosePrice");
            string wCloseDate        = lookupFieldName(properties, "wCloseDate");
            string wPrevClosePrice   = lookupFieldName(properties, "wPrevClosePrice");
            string wPrevCloseDate    = lookupFieldName(properties, "wPrevCloseDate");
            string wAdjPrevClose     = lookupFieldName(properties, "wAdjPrevClose");
            string wPrevVolume       = lookupFieldName(properties, "wPrevVolume");
            string wTradeSeqNum      = lookupFieldName(properties, "wTradeSeqNum");
            string wTradeQualifier   = lookupFieldName(properties, "wTradeQualifier");
            string wTradePartId      = lookupFieldName(properties, "wTradePartId");
            string wAggressorSide    = lookupFieldName(properties, "wAggressorSide");
            string wTradeSide        = lookupFieldName(properties, "wTradeSide");
            string wTotalValue       = lookupFieldName(properties, "wTotalValue");
            string wOffExTotalValue  = lookupFieldName(properties, "wOffExchangeTotalValue");
            string wOnExTotalValue   = lookupFieldName(properties, "wOnExchangeTotalValue");
            string wVwap             = lookupFieldName(properties, "wVwap");
            string wOffExVwap        = lookupFieldName(properties, "wOffExchangeVwap");
            string wOnExVwap         = lookupFieldName(properties, "wOnExchangeVwap");
            string wStdDev           = lookupFieldName(properties, "wStdDev");
            string wStdDevSum        = lookupFieldName(properties, "wStdDevSum");
            string wStdDevSumSquares = lookupFieldName(properties, "wStdDevSumSquares");
            string wOrderId          = lookupFieldName(properties, "wOrderId");
            string wSettlePrice      = lookupFieldName(properties, "wSettlePrice");
            string wSettleDate       = lookupFieldName(properties, "wSettleDate");
            string wSaleCondition    = lookupFieldName(properties, "wSaleCondition");
            string wSellersSaleDays  = lookupFieldName(properties, "wSellersSaleDays");
            string wStopStockInd     = lookupFieldName(properties, "wStopStockIndicator");
            string wTradeExecVenue   = lookupFieldName(properties, "wTradeExecVenueEnum");
            string wOffExTradePrice  = lookupFieldName(properties, "wOffExchangeTradePrice");
            string wOnExTradePrice   = lookupFieldName(properties, "wOnExchangeTradePrice");
            string wIsIrregular      = lookupFieldName(properties, "wIsIrregular");
            string wIrregPartId      = lookupFieldName(properties, "wIrregPartId");
            string wIrregPrice       = lookupFieldName(properties, "wIrregPrice");
            string wIrregSize        = lookupFieldName(properties, "wIrregSize");
            string wIrregTime        = lookupFieldName(properties, "wIrregTime");
            string wOrigPartId       = lookupFieldName(properties, "wOrigPartId");
            string wOrigPrice        = lookupFieldName(properties, "wOrigPrice");
            string wOrigSize         = lookupFieldName(properties, "wOrigSize");
            string wOrigSeqNum       = lookupFieldName(properties, "wOrigSeqNum");
            string wOrigQualifier    = lookupFieldName(properties, "wOrigQualifier");
            string wOrigCondition    = lookupFieldName(properties, "wOrigCondition");
            string wOrigSaleDays     = lookupFieldName(properties, "wOrigSaleDays");
            string wOrigStopStockInd = lookupFieldName(properties, "wOrigStopStockInd");
            string wCorrPartId       = lookupFieldName(properties, "wCorrPartId");
            string wCorrPrice        = lookupFieldName(properties, "wCorrPrice");
            string wCorrSize         = lookupFieldName(properties, "wCorrSize");
            string wCorrQualifier    = lookupFieldName(properties, "wCorrQualifier");
            string wCorrCondition    = lookupFieldName(properties, "wCorrCondition");
            string wCorrSaleDays     = lookupFieldName(properties, "wCorrSaleDays");
            string wCorrStopStockInd = lookupFieldName(properties, "wCorrStopStockInd");
            string wCorrTime         = lookupFieldName(properties, "wCorrTime");
            string wCancelTime       = lookupFieldName(properties, "wCancelTime");
            string wTradeId          = lookupFieldName(properties, "wTradeId");
            string wOrigTradeId      = lookupFieldName(properties, "wOrigTradeId");
            string wCorrTradeId      = lookupFieldName(properties, "wCorrTradeId");
            string wPrimExch         = lookupFieldName(properties, "wPrimExch");
            string wTradeCount       = lookupFieldName(properties, "wTradeCount");
            string wBlockCount       = lookupFieldName(properties, "wBlockCount");
            string wBlockVolume      = lookupFieldName(properties, "wBlockVolume");
            string wUpdateAsTrade    = lookupFieldName(properties, "wUpdateAsTrade");
            string wLastTradeSeqNum  = lookupFieldName(properties, "wLastTradeSeqNum");
            string wHighSeqNum       = lookupFieldName(properties, "wHighSeqNum");
            string wLowSeqNum        = lookupFieldName(properties, "wLowSeqNum");
            string wTotalVolumeSeqNum= lookupFieldName(properties, "wTotalVolumeSeqNum");
            string wCurrencyCode     = lookupFieldName(properties, "wCurrencyCode");
            string wConflateCount    = lookupFieldName (properties, "wConflateTradeCount");
            string wShortSaleCircuitBreaker   = lookupFieldName (properties, "wShortSaleCircuitBreaker");
            string wOrigShortSaleCircuitBreaker   = lookupFieldName (properties, "wOrigShortSaleCircuitBreaker");
            string wCorrShortSaleCircuitBreaker   = lookupFieldName (properties, "wCorrShortSaleCircuitBreaker");

            SYMBOL                      = dictionary.getFieldByName(wSymbol);
            ISSUE_SYMBOL                = dictionary.getFieldByName(wIssueSymbol);
            PART_ID                     = dictionary.getFieldByName(wPartId);
            SRC_TIME                    = dictionary.getFieldByName(wSrcTime);
            ACTIVITY_TIME               = dictionary.getFieldByName(wActivityTime);
            LINE_TIME                   = dictionary.getFieldByName(wLineTime);
            SEND_TIME                   = dictionary.getFieldByName(MamaSendTime);
            PUB_ID                      = dictionary.getFieldByName(wPubId);
            TRADE_PRICE                 = dictionary.getFieldByName(wTradePrice);
            TRADE_DATE                  = dictionary.getFieldByName(wTradeDate);
            TRADE_TIME                  = dictionary.getFieldByName(wTradeTime);
            TRADE_DIRECTION             = dictionary.getFieldByName(wTradeTick);
            LAST_PRICE                  = dictionary.getFieldByName(wLastPrice);
            LAST_VOLUME                 = dictionary.getFieldByName(wLastVolume);
            LAST_PART_ID                = dictionary.getFieldByName(wLastPartId);
            LAST_TIME                   = dictionary.getFieldByName(wLastTime);
            NET_CHANGE                  = dictionary.getFieldByName(wNetChange);
            PCT_CHANGE                  = dictionary.getFieldByName(wPctChange);
            TRADE_SIZE                  = dictionary.getFieldByName(wTradeVolume);
            TOTAL_VOLUME                = dictionary.getFieldByName(wTotalVolume);
            OFF_EXCHANGE_TOTAL_VOLUME   = dictionary.getFieldByName(wOffExTotalVolume);
            ON_EXCHANGE_TOTAL_VOLUME    = dictionary.getFieldByName(wOnExTotalVolume);
            TRADE_UNITS                 = dictionary.getFieldByName(wTradeUnits);
            HIGH_PRICE                  = dictionary.getFieldByName(wHighPrice);
            LOW_PRICE                   = dictionary.getFieldByName(wLowPrice);
            OPEN_PRICE                  = dictionary.getFieldByName(wOpenPrice);
            CLOSE_PRICE                 = dictionary.getFieldByName(wClosePrice);
            CLOSE_DATE                  = dictionary.getFieldByName(wCloseDate);
            PREV_CLOSE_PRICE            = dictionary.getFieldByName(wPrevClosePrice);
            PREV_CLOSE_DATE             = dictionary.getFieldByName(wPrevCloseDate);
            ADJ_PREV_CLOSE              = dictionary.getFieldByName(wAdjPrevClose);
            PREV_VOLUME                 = dictionary.getFieldByName(wPrevVolume);
            TRADE_SEQNUM                = dictionary.getFieldByName(wTradeSeqNum);
            TRADE_QUALIFIER             = dictionary.getFieldByName(wTradeQualifier);
            TRADE_PART_ID               = dictionary.getFieldByName(wTradePartId);
            AGGRESSOR_SIDE              = dictionary.getFieldByName(wAggressorSide);
            TRADE_SIDE                  = dictionary.getFieldByName(wTradeSide);
            TOTAL_VALUE                 = dictionary.getFieldByName(wTotalValue);
            OFF_EXCHANGE_TOTAL_VALUE    = dictionary.getFieldByName(wTotalValue);
            ON_EXCHANGE_TOTAL_VALUE     = dictionary.getFieldByName(wTotalValue);
            VWAP                        = dictionary.getFieldByName(wVwap);
            OFF_EXCHANGE_VWAP           = dictionary.getFieldByName(wVwap);
            ON_EXCHANGE_VWAP            = dictionary.getFieldByName(wVwap);
            STD_DEV                     = dictionary.getFieldByName(wStdDev);
            STD_DEV_SUM                 = dictionary.getFieldByName(wStdDevSum);
            STD_DEV_SUM_SQUARES         = dictionary.getFieldByName(wStdDevSumSquares);
            ORDER_ID                    = dictionary.getFieldByName(wOrderId);
            SETTLE_PRICE                = dictionary.getFieldByName(wSettlePrice);
            SETTLE_DATE                 = dictionary.getFieldByName(wSettleDate);
            SALE_CONDITION              = dictionary.getFieldByName(wSaleCondition);
            SELLERS_SALE_DAYS           = dictionary.getFieldByName(wSellersSaleDays);
            STOP_STOCK_IND              = dictionary.getFieldByName(wStopStockInd);
            TRADE_EXEC_VENUE            = dictionary.getFieldByName(wTradeExecVenue);
            OFF_EXCHANGE_TRADE_PRICE    = dictionary.getFieldByName(wOffExTradePrice);
            ON_EXCHANGE_TRADE_PRICE     = dictionary.getFieldByName(wOnExTradePrice);
            IS_IRREGULAR                = dictionary.getFieldByName(wIsIrregular);
            IRREG_PART_ID               = dictionary.getFieldByName(wIrregPartId);
            IRREG_PRICE                 = dictionary.getFieldByName(wIrregPrice);
            IRREG_SIZE                  = dictionary.getFieldByName(wIrregSize);
            IRREG_TIME                  = dictionary.getFieldByName(wIrregTime);
            ORIG_PART_ID                = dictionary.getFieldByName(wOrigPartId);
            ORIG_PRICE                  = dictionary.getFieldByName(wOrigPrice);
            ORIG_SIZE                   = dictionary.getFieldByName(wOrigSize);
            ORIG_SEQNUM                 = dictionary.getFieldByName(wOrigSeqNum);
            ORIG_TRADE_QUALIFIER        = dictionary.getFieldByName(wOrigQualifier);
            ORIG_SALE_CONDITION         = dictionary.getFieldByName(wOrigCondition);
            ORIG_SELLERS_SALE_DAYS      = dictionary.getFieldByName(wOrigSaleDays);
            ORIG_STOP_STOCK_IND         = dictionary.getFieldByName(wOrigStopStockInd);
            CORR_PART_ID                = dictionary.getFieldByName(wCorrPartId);
            CORR_PRICE                  = dictionary.getFieldByName(wCorrPrice);
            CORR_SIZE                   = dictionary.getFieldByName(wCorrSize);
            CORR_TRADE_QUALIFIER        = dictionary.getFieldByName(wCorrQualifier);
            CORR_SALE_CONDITION         = dictionary.getFieldByName(wCorrCondition);
            CORR_SELLERS_SALE_DAYS      = dictionary.getFieldByName(wCorrSaleDays);
            CORR_STOP_STOCK_IND         = dictionary.getFieldByName(wCorrStopStockInd);
            CORR_TIME                   = dictionary.getFieldByName(wCorrTime);
            CANCEL_TIME                 = dictionary.getFieldByName(wCancelTime);
            TRADE_ID                    = dictionary.getFieldByName(wTradeId);
            ORIG_TRADE_ID               = dictionary.getFieldByName(wOrigTradeId);
            CORR_TRADE_ID               = dictionary.getFieldByName(wCorrTradeId);
            PRIMARY_EXCH                = dictionary.getFieldByName(wPrimExch);
            TRADE_COUNT                 = dictionary.getFieldByName(wTradeCount);
            BLOCK_COUNT                 = dictionary.getFieldByName(wBlockCount);
            BLOCK_VOLUME                = dictionary.getFieldByName(wBlockVolume);
            UPDATE_AS_TRADE             = dictionary.getFieldByName(wUpdateAsTrade);
            LAST_SEQNUM                 = dictionary.getFieldByName(wLastTradeSeqNum);
            HIGH_SEQNUM                 = dictionary.getFieldByName(wHighSeqNum);
            LOW_SEQNUM                  = dictionary.getFieldByName(wLowSeqNum);
            TOTAL_VOLUME_SEQNUM         = dictionary.getFieldByName(wTotalVolumeSeqNum);
            CURRENCY_CODE               = dictionary.getFieldByName(wCurrencyCode);
            CONFLATE_COUNT              = dictionary.getFieldByName(wConflateCount);
            SHORT_SALE_CIRCUIT_BREAKER	= dictionary.getFieldByName (wShortSaleCircuitBreaker);
            ORIG_SHORT_SALE_CIRCUIT_BREAKER = dictionary.getFieldByName (wOrigShortSaleCircuitBreaker);
            CORR_SHORT_SALE_CIRCUIT_BREAKER = dictionary.getFieldByName (wCorrShortSaleCircuitBreaker);

            MAX_FID                     = dictionary.getMaxFid();
            mInitialised                = true;
        }

        /// <summary>
        /// Returns the maximum field descriptor identifier in the dictionary
        /// </summary>
        /// <returns></returns>
        public static int getMaxFid()
        {
            return MAX_FID;
        }

        public static bool isSet()
        {
            return mInitialised;
        }
        
        public static void reset ()
        {
          if (MamdaCommonFields.isSet())
          {
            MamdaCommonFields.reset();
          }
          mInitialised					= false;
          MAX_FID						= 0;
          SYMBOL						= null;
          ISSUE_SYMBOL					= null;
          PART_ID						= null;
          SRC_TIME						= null;
          ACTIVITY_TIME					= null;
          LINE_TIME						= null;
          SEND_TIME						= null;
          PUB_ID						= null;
          TRADE_PRICE					= null;
          TRADE_DATE					= null;
          TRADE_TIME					= null;
          TRADE_DIRECTION				= null;
          LAST_PRICE					= null;
          LAST_VOLUME					= null;
          LAST_PART_ID					= null;
          LAST_TIME						= null;
          NET_CHANGE					= null;
          PCT_CHANGE					= null;
          TRADE_SIZE					= null;
          TOTAL_VOLUME					= null;
          OFF_EXCHANGE_TOTAL_VOLUME		= null;
          ON_EXCHANGE_TOTAL_VOLUME		= null;
          TRADE_UNITS					= null;
          HIGH_PRICE					= null;
          LOW_PRICE						= null;
          OPEN_PRICE					= null;
          CLOSE_PRICE					= null;
          CLOSE_DATE					= null;
          PREV_CLOSE_PRICE				= null;
          PREV_CLOSE_DATE				= null;
          ADJ_PREV_CLOSE				= null;
          PREV_VOLUME					= null;
          TRADE_SEQNUM					= null;
          TRADE_QUALIFIER				= null;
          TRADE_PART_ID					= null;
          AGGRESSOR_SIDE				= null;
          TRADE_SIDE					= null;
          TOTAL_VALUE					= null;
          OFF_EXCHANGE_TOTAL_VALUE		= null;
          ON_EXCHANGE_TOTAL_VALUE		= null;
          VWAP							= null;
          OFF_EXCHANGE_VWAP				= null;
          ON_EXCHANGE_VWAP				= null;
          STD_DEV						= null;
          STD_DEV_SUM					= null;
          STD_DEV_SUM_SQUARES			= null;
          ORDER_ID						= null;
          SETTLE_PRICE					= null;
          SETTLE_DATE					= null;
          SALE_CONDITION				= null;
          SELLERS_SALE_DAYS				= null;
          STOP_STOCK_IND				= null;
          TRADE_EXEC_VENUE				= null;
          OFF_EXCHANGE_TRADE_PRICE		= null;
          ON_EXCHANGE_TRADE_PRICE		= null;
          IS_IRREGULAR					= null;
          IRREG_PART_ID					= null;
          IRREG_PRICE					= null;
          IRREG_SIZE					= null;
          IRREG_TIME					= null;
          ORIG_PART_ID					= null;
          ORIG_PRICE					= null;
          ORIG_SIZE						= null;
          ORIG_SEQNUM					= null;
          ORIG_TRADE_QUALIFIER			= null;
          ORIG_SALE_CONDITION			= null;
          ORIG_SELLERS_SALE_DAYS		= null;
          ORIG_STOP_STOCK_IND			= null;
          CORR_PART_ID					= null;
          CORR_PRICE					= null;
          CORR_SIZE						= null;
          CORR_TRADE_QUALIFIER			= null;
          CORR_SALE_CONDITION			= null;
          CORR_SELLERS_SALE_DAYS		= null;
          CORR_STOP_STOCK_IND			= null;
          CORR_TIME						= null;
          CANCEL_TIME					= null;
          TRADE_ID						= null;
          ORIG_TRADE_ID					= null;
          CORR_TRADE_ID					= null;
          PRIMARY_EXCH					= null;
          TRADE_COUNT					= null;
          BLOCK_COUNT					= null;
          BLOCK_VOLUME					= null;
          UPDATE_AS_TRADE				= null;
          LAST_SEQNUM					= null;
          HIGH_SEQNUM					= null;
          LOW_SEQNUM					= null;
          TOTAL_VOLUME_SEQNUM			= null;
          CURRENCY_CODE					= null;
          CONFLATE_COUNT				= null;
          SHORT_SALE_CIRCUIT_BREAKER	= null;
          ORIG_SHORT_SALE_CIRCUIT_BREAKER = null;
          CORR_SHORT_SALE_CIRCUIT_BREAKER = null;
        }
        public static MamaFieldDescriptor  SYMBOL                 = null;
        public static MamaFieldDescriptor  ISSUE_SYMBOL           = null;
        public static MamaFieldDescriptor  PART_ID                = null;
        public static MamaFieldDescriptor  SRC_TIME               = null;
        public static MamaFieldDescriptor  ACTIVITY_TIME          = null;
        public static MamaFieldDescriptor  LINE_TIME              = null;
        public static MamaFieldDescriptor  SEND_TIME              = null;
        public static MamaFieldDescriptor  PUB_ID                 = null;
        public static MamaFieldDescriptor  TRADE_PRICE            = null;
        public static MamaFieldDescriptor  TRADE_DATE             = null;
        public static MamaFieldDescriptor  TRADE_TIME             = null;
        public static MamaFieldDescriptor  TRADE_DIRECTION        = null;
        public static MamaFieldDescriptor  LAST_PRICE             = null;
        public static MamaFieldDescriptor  LAST_VOLUME            = null;
        public static MamaFieldDescriptor  LAST_PART_ID           = null;
        public static MamaFieldDescriptor  LAST_TIME              = null;
        public static MamaFieldDescriptor  NET_CHANGE             = null;
        public static MamaFieldDescriptor  PCT_CHANGE             = null;
        public static MamaFieldDescriptor  TRADE_SIZE             = null;
        public static MamaFieldDescriptor  TOTAL_VOLUME           = null;
        public static MamaFieldDescriptor  OFF_EXCHANGE_TOTAL_VOLUME = null;
        public static MamaFieldDescriptor  ON_EXCHANGE_TOTAL_VOLUME  = null;
        public static MamaFieldDescriptor  TRADE_UNITS            = null;
        public static MamaFieldDescriptor  HIGH_PRICE             = null;
        public static MamaFieldDescriptor  LOW_PRICE              = null;
        public static MamaFieldDescriptor  OPEN_PRICE             = null;
        public static MamaFieldDescriptor  CLOSE_PRICE            = null;
        public static MamaFieldDescriptor  CLOSE_DATE             = null;
        public static MamaFieldDescriptor  PREV_CLOSE_PRICE       = null;
        public static MamaFieldDescriptor  PREV_CLOSE_DATE        = null;
        public static MamaFieldDescriptor  ADJ_PREV_CLOSE         = null;
        public static MamaFieldDescriptor  PREV_VOLUME            = null;
        public static MamaFieldDescriptor  TRADE_SEQNUM           = null;
        public static MamaFieldDescriptor  TRADE_QUALIFIER        = null;
        public static MamaFieldDescriptor  TRADE_PART_ID          = null;
        public static MamaFieldDescriptor  AGGRESSOR_SIDE         = null;
        public static MamaFieldDescriptor  TRADE_SIDE             = null;
        public static MamaFieldDescriptor  TOTAL_VALUE            = null;
        public static MamaFieldDescriptor  OFF_EXCHANGE_TOTAL_VALUE = null;
        public static MamaFieldDescriptor  ON_EXCHANGE_TOTAL_VALUE  = null;
        public static MamaFieldDescriptor  VWAP                   = null;
        public static MamaFieldDescriptor  OFF_EXCHANGE_VWAP      = null;
        public static MamaFieldDescriptor  ON_EXCHANGE_VWAP       = null;
        public static MamaFieldDescriptor  STD_DEV                = null;
        public static MamaFieldDescriptor  STD_DEV_SUM            = null;
        public static MamaFieldDescriptor  STD_DEV_SUM_SQUARES    = null;
        public static MamaFieldDescriptor  ORDER_ID               = null;
        public static MamaFieldDescriptor  SETTLE_PRICE           = null;
        public static MamaFieldDescriptor  SETTLE_DATE            = null;
        public static MamaFieldDescriptor  SALE_CONDITION         = null;
        public static MamaFieldDescriptor  SELLERS_SALE_DAYS      = null;
        public static MamaFieldDescriptor  STOP_STOCK_IND         = null;
        public static MamaFieldDescriptor  TRADE_EXEC_VENUE       = null;
        public static MamaFieldDescriptor  OFF_EXCHANGE_TRADE_PRICE = null;
        public static MamaFieldDescriptor  ON_EXCHANGE_TRADE_PRICE  = null;
        public static MamaFieldDescriptor  IS_IRREGULAR           = null;
        public static MamaFieldDescriptor  IRREG_PART_ID          = null;
        public static MamaFieldDescriptor  IRREG_PRICE            = null;
        public static MamaFieldDescriptor  IRREG_SIZE             = null;
        public static MamaFieldDescriptor  IRREG_TIME             = null;
        public static MamaFieldDescriptor  ORIG_PART_ID           = null;
        public static MamaFieldDescriptor  ORIG_PRICE             = null;
        public static MamaFieldDescriptor  ORIG_SIZE              = null;
        public static MamaFieldDescriptor  ORIG_SEQNUM            = null;
        public static MamaFieldDescriptor  ORIG_TRADE_QUALIFIER   = null;
        public static MamaFieldDescriptor  ORIG_SALE_CONDITION    = null;
        public static MamaFieldDescriptor  ORIG_SELLERS_SALE_DAYS = null;
        public static MamaFieldDescriptor  ORIG_STOP_STOCK_IND    = null;
        public static MamaFieldDescriptor  CORR_PART_ID           = null;
        public static MamaFieldDescriptor  CORR_PRICE             = null;
        public static MamaFieldDescriptor  CORR_SIZE              = null;
        public static MamaFieldDescriptor  CORR_TRADE_QUALIFIER   = null;
        public static MamaFieldDescriptor  CORR_SALE_CONDITION    = null;
        public static MamaFieldDescriptor  CORR_SELLERS_SALE_DAYS = null;
        public static MamaFieldDescriptor  CORR_STOP_STOCK_IND    = null;
        public static MamaFieldDescriptor  CORR_TIME              = null;
        public static MamaFieldDescriptor  CANCEL_TIME            = null;
        public static MamaFieldDescriptor  TRADE_ID               = null;
        public static MamaFieldDescriptor  ORIG_TRADE_ID          = null;
        public static MamaFieldDescriptor  CORR_TRADE_ID          = null;
        public static MamaFieldDescriptor  PRIMARY_EXCH           = null;
        public static MamaFieldDescriptor  TRADE_COUNT            = null;
        public static MamaFieldDescriptor  BLOCK_COUNT            = null;
        public static MamaFieldDescriptor  BLOCK_VOLUME           = null;
        public static MamaFieldDescriptor  UPDATE_AS_TRADE        = null;
        public static MamaFieldDescriptor  LAST_SEQNUM            = null;
        public static MamaFieldDescriptor  HIGH_SEQNUM            = null;
        public static MamaFieldDescriptor  LOW_SEQNUM             = null;
        public static MamaFieldDescriptor  TOTAL_VOLUME_SEQNUM    = null;
        public static MamaFieldDescriptor  CURRENCY_CODE          = null;
        public static MamaFieldDescriptor  CONFLATE_COUNT         = null;

        public static MamaFieldDescriptor  SHORT_SALE_CIRCUIT_BREAKER = null; 
        public static MamaFieldDescriptor  ORIG_SHORT_SALE_CIRCUIT_BREAKER = null; 
        public static MamaFieldDescriptor  CORR_SHORT_SALE_CIRCUIT_BREAKER = null;

        public static int MAX_FID = 0;
        private static bool mInitialised = false;
    }
}

