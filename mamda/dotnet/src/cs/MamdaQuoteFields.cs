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
	/// Cache of common Quote related field descriptors.
	/// This is required to be populated if using the MamdaQuoteListener.
	/// </summary>
	public class MamdaQuoteFields : MamdaFields
	{
		/// <summary>
		/// Set the dictionary for common quote fields.
		/// Maintains a cache of MamaFieldDescriptors for common quote
		/// related fields. The <code>properties</code> parameter allows users
		/// of the API to map the common dictionary names to something else
		/// if they are beig published under different names.
		/// </summary>
		/// <param name="dictionary">A reference to a valid MamaDictionary</param>
		/// <param name="properties">A NameValueCollection object containing field mappings. (See
		/// MamdaFields for more information)</param>
		public static void setDictionary(
			MamaDictionary		dictionary,
			NameValueCollection	properties)
		{
			if (mInitialised)
			{
				return;
			}

			string wSymbol           = lookupFieldName (properties, "wSymbol");
			string wIssueSymbol      = lookupFieldName (properties, "wIssueSymbol");
			string wPartId           = lookupFieldName (properties, "wPartId");
			string wSrcTime          = lookupFieldName (properties, "wSrcTime");
			string wActivityTime     = lookupFieldName (properties, "wActivityTime");
			string wLineTime         = lookupFieldName (properties, "wLineTime");
			string wSendTime         = lookupFieldName (properties, "wSendTime");
			string wPubId            = lookupFieldName (properties, "wPubId");

			string wBidPrice         = lookupFieldName (properties, "wBidPrice");
			string wBidSize          = lookupFieldName (properties, "wBidSize");
			string wBidPartId        = lookupFieldName (properties, "wBidPartId");
			string wBidDepth         = lookupFieldName (properties, "wBidDepth");
			string wBidClose         = lookupFieldName (properties, "wBidClose");
			string wBidCloseDate     = lookupFieldName (properties, "wBidCloseDate");
			string wPrevBidClose     = lookupFieldName (properties, "wPrevBidClose");
			string wPrevBidCloseDate = lookupFieldName (properties, "wPrevBidCloseDate");
			string wBidHigh          = lookupFieldName (properties, "wBidHigh");
			string wBidLow           = lookupFieldName (properties, "wBidLow");

			string wAskPrice         = lookupFieldName (properties, "wAskPrice");
			string wAskSize          = lookupFieldName (properties, "wAskSize");
			string wAskPartId        = lookupFieldName (properties, "wAskPartId");
			string wAskDepth         = lookupFieldName (properties, "wAskDepth");
			string wAskClose         = lookupFieldName (properties, "wAskClose");
			string wAskCloseDate     = lookupFieldName (properties, "wAskCloseDate");
			string wPrevAskClose     = lookupFieldName (properties, "wPrevAskClose");
			string wPrevAskCloseDate = lookupFieldName (properties, "wPrevAskCloseDate");
			string wAskHigh          = lookupFieldName (properties, "wAskHigh");
			string wAskLow           = lookupFieldName (properties, "wAskLow");

			string wQuoteSeqNum      = lookupFieldName (properties, "wQuoteSeqNum");
			string wQuoteTime        = lookupFieldName (properties, "wQuoteTime");
            string wQuoteDate        = lookupFieldName (properties, "wQuoteDate");
			string wQuoteQualifier   = lookupFieldName (properties, "wQuoteQualifier");
			string wCondition        = lookupFieldName (properties, "wCondition");
			string wQuoteCount       = lookupFieldName (properties, "wQuoteCount");
            string wConflateCount       = lookupFieldName (properties, "wConflateQuoteCount");
			string wShortSaleBidTick = lookupFieldName (properties, "wShortSaleBidTick");
			string wBidTick          = lookupFieldName (properties,  "wBidTick");

            string wAskTime          = lookupFieldName (properties, "wAskTime");
            string wBidTime          = lookupFieldName (properties, "wBidTime");
            string wAskIndicator     = lookupFieldName (properties, "wAskIndicator");
            string wBidIndicator     = lookupFieldName (properties, "wBidIndicator");
            string wAskUpdateCount   = lookupFieldName (properties, "wAskUpdateCount");
            string wBidUpdateCount   = lookupFieldName (properties, "wBidUpdateCount");
            string wAskYield         = lookupFieldName (properties, "wAskYield");
            string wBidYield         = lookupFieldName (properties, "wBidYield");
            string wShortSaleCircuitBreaker = lookupFieldName(properties, "wShortSaleCircuitBreaker");

			SYMBOL				= dictionary.getFieldByName (wSymbol);
			ISSUE_SYMBOL		= dictionary.getFieldByName (wIssueSymbol);
			PART_ID				= dictionary.getFieldByName (wPartId);
			SRC_TIME			= dictionary.getFieldByName (wSrcTime);
			ACTIVITY_TIME		= dictionary.getFieldByName (wActivityTime);
			LINE_TIME			= dictionary.getFieldByName (wLineTime);
			SEND_TIME			= dictionary.getFieldByName (wSendTime);
			PUB_ID				= dictionary.getFieldByName (wPubId);

			BID_PRICE			= dictionary.getFieldByName (wBidPrice);
			BID_SIZE			= dictionary.getFieldByName (wBidSize);
			BID_PART_ID			= dictionary.getFieldByName (wBidPartId);
			BID_DEPTH			= dictionary.getFieldByName (wBidDepth);
			BID_CLOSE_PRICE		= dictionary.getFieldByName (wBidClose);
			BID_CLOSE_DATE		= dictionary.getFieldByName (wBidCloseDate);
			BID_PREV_CLOSE_PRICE = dictionary.getFieldByName (wPrevBidClose);
			BID_PREV_CLOSE_DATE = dictionary.getFieldByName (wPrevBidCloseDate);
			BID_HIGH			= dictionary.getFieldByName (wBidHigh);
			BID_LOW				= dictionary.getFieldByName (wBidLow);

			ASK_PRICE			= dictionary.getFieldByName (wAskPrice);
			ASK_SIZE			= dictionary.getFieldByName (wAskSize);
			ASK_PART_ID			= dictionary.getFieldByName (wAskPartId);
			ASK_DEPTH			= dictionary.getFieldByName (wAskDepth);
			ASK_CLOSE_PRICE		= dictionary.getFieldByName (wAskClose);
			ASK_CLOSE_DATE		= dictionary.getFieldByName (wAskCloseDate);
			ASK_PREV_CLOSE_PRICE = dictionary.getFieldByName (wPrevAskClose);
			ASK_PREV_CLOSE_DATE = dictionary.getFieldByName (wPrevAskCloseDate);
			ASK_HIGH			= dictionary.getFieldByName (wAskHigh);
			ASK_LOW				= dictionary.getFieldByName (wAskLow);

			QUOTE_SEQ_NUM		= dictionary.getFieldByName (wQuoteSeqNum);
			QUOTE_TIME			= dictionary.getFieldByName (wQuoteTime);
            QUOTE_DATE			= dictionary.getFieldByName (wQuoteDate);
			QUOTE_QUAL			= dictionary.getFieldByName (wQuoteQualifier);
			QUOTE_QUAL_NATIVE	= dictionary.getFieldByName (wCondition);
			QUOTE_COUNT			= dictionary.getFieldByName (wQuoteCount);
            CONFLATE_COUNT      = dictionary.getFieldByName (wConflateCount);

			SHORT_SALE_BID_TICK = dictionary.getFieldByName (wShortSaleBidTick);
			BID_TICK            = dictionary.getFieldByName (wBidTick);


            ASK_TIME            = dictionary.getFieldByName (wAskTime);
            BID_TIME            = dictionary.getFieldByName (wBidTime);
            ASK_INDICATOR       = dictionary.getFieldByName (wAskIndicator);
            BID_INDICATOR       = dictionary.getFieldByName (wBidIndicator);
            ASK_UPDATE_COUNT    = dictionary.getFieldByName (wAskUpdateCount);
            BID_UPDATE_COUNT    = dictionary.getFieldByName (wBidUpdateCount);
            ASK_YIELD           = dictionary.getFieldByName (wAskYield);
            BID_YIELD           = dictionary.getFieldByName (wBidYield);
            SHORT_SALE_CIRCUIT_BREAKER = dictionary.getFieldByName(wShortSaleCircuitBreaker);


			MAX_FID = dictionary.getMaxFid();
			mInitialised = true;
		}

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
            mInitialised           = false;
            MAX_FID               = 0;

            SYMBOL                = null;
            ISSUE_SYMBOL          = null;
            PART_ID               = null;
            SRC_TIME              = null;
            ACTIVITY_TIME         = null;
            LINE_TIME             = null;
            SEND_TIME             = null;
            PUB_ID                = null;
            BID_PRICE             = null;
            BID_SIZE              = null;
            BID_PART_ID           = null;
            BID_DEPTH             = null;
            BID_CLOSE_PRICE       = null;
            BID_CLOSE_DATE        = null;
            BID_PREV_CLOSE_PRICE  = null;
            BID_PREV_CLOSE_DATE   = null;
            BID_HIGH              = null;
            BID_LOW               = null;
            ASK_PRICE             = null;
            ASK_SIZE              = null;
            ASK_PART_ID           = null;
            ASK_DEPTH             = null;
            ASK_CLOSE_PRICE       = null;
            ASK_CLOSE_DATE        = null;
            ASK_PREV_CLOSE_PRICE  = null;
            ASK_PREV_CLOSE_DATE   = null;
            ASK_HIGH              = null;
            ASK_LOW               = null;
            QUOTE_SEQ_NUM         = null;
            QUOTE_TIME            = null;
            QUOTE_DATE            = null;
            QUOTE_QUAL            = null;
            QUOTE_QUAL_NATIVE     = null;
            QUOTE_COUNT           = null;
            CONFLATE_COUNT        = null;
            SHORT_SALE_BID_TICK   = null;
            BID_TICK              = null;
            ASK_TIME              = null;
            BID_TIME              = null;
            ASK_INDICATOR         = null;
            BID_INDICATOR         = null;
            ASK_UPDATE_COUNT      = null;
            BID_UPDATE_COUNT      = null;
            ASK_YIELD             = null;
            BID_YIELD             = null;
            SHORT_SALE_CIRCUIT_BREAKER = null;
        }

		public static MamaFieldDescriptor  SYMBOL           = null;
		public static MamaFieldDescriptor  ISSUE_SYMBOL     = null;
		public static MamaFieldDescriptor  PART_ID          = null;
		public static MamaFieldDescriptor  SRC_TIME         = null;
		public static MamaFieldDescriptor  ACTIVITY_TIME    = null;
		public static MamaFieldDescriptor  LINE_TIME        = null;
		public static MamaFieldDescriptor  SEND_TIME        = null;
		public static MamaFieldDescriptor  PUB_ID           = null;

		public static MamaFieldDescriptor  BID_PRICE        = null;
		public static MamaFieldDescriptor  BID_SIZE         = null;
		public static MamaFieldDescriptor  BID_PART_ID      = null;
		public static MamaFieldDescriptor  BID_DEPTH        = null;
		public static MamaFieldDescriptor  BID_CLOSE_PRICE  = null;
		public static MamaFieldDescriptor  BID_CLOSE_DATE   = null;
		public static MamaFieldDescriptor  BID_PREV_CLOSE_PRICE = null;
		public static MamaFieldDescriptor  BID_PREV_CLOSE_DATE = null;
		public static MamaFieldDescriptor  BID_HIGH         = null;
		public static MamaFieldDescriptor  BID_LOW          = null;

		public static MamaFieldDescriptor  ASK_PRICE        = null;
		public static MamaFieldDescriptor  ASK_SIZE         = null;
		public static MamaFieldDescriptor  ASK_PART_ID      = null;
		public static MamaFieldDescriptor  ASK_DEPTH        = null;
		public static MamaFieldDescriptor  ASK_CLOSE_PRICE  = null;
		public static MamaFieldDescriptor  ASK_CLOSE_DATE   = null;
		public static MamaFieldDescriptor  ASK_PREV_CLOSE_PRICE = null;
		public static MamaFieldDescriptor  ASK_PREV_CLOSE_DATE = null;
		public static MamaFieldDescriptor  ASK_HIGH         = null;
		public static MamaFieldDescriptor  ASK_LOW          = null;

		public static MamaFieldDescriptor  QUOTE_SEQ_NUM    = null;
		public static MamaFieldDescriptor  QUOTE_TIME       = null;
        public static MamaFieldDescriptor  QUOTE_DATE       = null;
		public static MamaFieldDescriptor  QUOTE_QUAL       = null;
		public static MamaFieldDescriptor  QUOTE_QUAL_NATIVE = null;
		public static MamaFieldDescriptor  QUOTE_COUNT      = null;
        public static MamaFieldDescriptor  CONFLATE_COUNT      = null;

		/*Looks like this field can be sent as either from the feeds*/
		public static MamaFieldDescriptor  SHORT_SALE_BID_TICK = null;
		public static MamaFieldDescriptor  BID_TICK = null;

        public static MamaFieldDescriptor  ASK_TIME             = null;
        public static MamaFieldDescriptor  BID_TIME             = null;
        public static MamaFieldDescriptor  ASK_INDICATOR        = null;
        public static MamaFieldDescriptor  BID_INDICATOR        = null;
        public static MamaFieldDescriptor  ASK_UPDATE_COUNT     = null;
        public static MamaFieldDescriptor  BID_UPDATE_COUNT     = null;
        public static MamaFieldDescriptor  ASK_YIELD            = null;
        public static MamaFieldDescriptor  BID_YIELD            = null;
        public static MamaFieldDescriptor  SHORT_SALE_CIRCUIT_BREAKER = null;


		public static int  MAX_FID = 0;
		private static bool mInitialised = false;
	}

}
