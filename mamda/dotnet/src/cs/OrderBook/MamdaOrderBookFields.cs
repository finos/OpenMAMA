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
	/// Maintains a cache of common orderbook related field descriptors.
	/// </summary>
	public class MamdaOrderBookFields : MamdaFields
	{
		private MamdaOrderBookFields()
		{
		}

		/// <summary>
		/// Set the dictionary for common orderbook fields.
		/// Maintains a cache of MamaFieldDescriptors for common orderbook
		/// related fields. The <code>properties</code> parameter allows users
		/// of the API to map the common dictionary names to something else
		/// if they are beig published under different names.
		/// </summary>
		/// <param name="dictionary">A reference to a valid MamaDictionary</param>
		/// <param name="properties">A Properties object containing field mappings. (See
		/// MamdaFields for more information</param>
		public static void setDictionary(
			MamaDictionary dictionary,
			NameValueCollection properties)
		{
			if (mInitialised)
			{
				return;
			}

			MamdaCommonFields.setDictionary(dictionary, null);

			string wSrcTime       = lookupFieldName(properties, "wSrcTime");
			string wActivityTime  = lookupFieldName(properties, "wActivityTime");
			string wLineTime      = lookupFieldName(properties, "wLineTime");	
			string wPartId        = lookupFieldName(properties, "wPartId");
			string wSeqNum        = lookupFieldName(properties, "wSeqNum");
			string wBookTime      = lookupFieldName(properties, "wBookTime");
			string wNumLevels     = lookupFieldName(properties, "wNumLevels");
			string wPriceLevels   = lookupFieldName(properties, "wPriceLevels");
			string wPlAction      = lookupFieldName(properties, "wPlAction");
			string wPlPrice       = lookupFieldName(properties, "wPlPrice");
			string wPlSide        = lookupFieldName(properties, "wPlSide");
			string wPlSize        = lookupFieldName(properties, "wPlSize");
			string wPlSizeChange  = lookupFieldName(properties, "wPlSizeChange");
			string wPlTime        = lookupFieldName(properties, "wPlTime");
			string wPlNumEntries  = lookupFieldName(properties, "wPlNumEntries");
			string wPlNumAttach   = lookupFieldName(properties, "wPlNumAttach");
			string wPlEntries     = lookupFieldName(properties, "wPlEntries");
			string wEntryId       = lookupFieldName(properties, "wEntryId");
			string wEntryAction   = lookupFieldName(properties, "wEntryAction");
			string wEntryReason   = lookupFieldName(properties, "wEntryReason");
			string wEntrySize     = lookupFieldName(properties, "wEntrySize");
			string wEntryTime     = lookupFieldName(properties, "wEntryTime");
			string wEntryStatus   = lookupFieldName(properties, "wEntryStatus");				

			SRC_TIME       = dictionary.getFieldByName(wSrcTime);
			ACTIVITY_TIME  = dictionary.getFieldByName(wActivityTime);
			LINE_TIME      = dictionary.getFieldByName(wLineTime);
			PART_ID        = dictionary.getFieldByName(wPartId);
			SEQNUM         = dictionary.getFieldByName(wSeqNum);
			BOOK_TIME      = dictionary.getFieldByName(wBookTime);
			NUM_LEVELS     = dictionary.getFieldByName(wNumLevels);
			PRICE_LEVELS   = dictionary.getFieldByName(wPriceLevels);
			PL_ACTION      = dictionary.getFieldByName(wPlAction);
			PL_PRICE       = dictionary.getFieldByName(wPlPrice);
			PL_SIDE        = dictionary.getFieldByName(wPlSide);
			PL_SIZE        = dictionary.getFieldByName(wPlSize);
			PL_SIZE_CHANGE = dictionary.getFieldByName(wPlSizeChange);
			PL_TIME        = dictionary.getFieldByName(wPlTime);
			PL_NUM_ENTRIES = dictionary.getFieldByName(wPlNumEntries);
			PL_NUM_ATTACH  = dictionary.getFieldByName(wPlNumAttach);
			PL_ENTRIES     = dictionary.getFieldByName(wPlEntries);
			ENTRY_ID       = dictionary.getFieldByName(wEntryId);
			ENTRY_ACTION   = dictionary.getFieldByName(wEntryAction);
			ENTRY_REASON   = dictionary.getFieldByName(wEntryReason);
			ENTRY_SIZE     = dictionary.getFieldByName(wEntrySize);
			ENTRY_TIME     = dictionary.getFieldByName(wEntryTime);
			ENTRY_STATUS   = dictionary.getFieldByName(wEntryStatus);

			mNumLevelFields = 0;
			while (true)
			{
				string wPriceLevel = lookupFieldName(
					properties,
					"wPriceLevel" + (mNumLevelFields + 1));
				if (dictionary.getFieldByName(wPriceLevel) == null)
					break;
				mNumLevelFields++;
			}

			PRICE_LEVEL = new MamaFieldDescriptor[mNumLevelFields + 1];
			for (int i = 1; i <= mNumLevelFields; i++)
			{
				string wPriceLevel = lookupFieldName(properties, "wPriceLevel" + i);
				PRICE_LEVEL[i] = dictionary.getFieldByName(wPriceLevel);
			}

			mNumEntryFields = 0;
			while (true)
			{
				string wPlEntry = lookupFieldName(
					properties,
					"wPlEntry" + (mNumEntryFields + 1));
				if (dictionary.getFieldByName(wPlEntry) == null)
					break;
				mNumEntryFields++;
			}

			PL_ENTRY = new MamaFieldDescriptor[mNumEntryFields + 1];
			for (int i = 1; i <= mNumEntryFields; i++)
			{
				string wPlEntry = lookupFieldName(properties, "wPlEntry" + i);
				PL_ENTRY[i] = dictionary.getFieldByName(wPlEntry);
			}

			mInitialised = true;
		}

		/// <summary>
		/// Internal.  Returns the number of fixed (non-vector) price level
		/// fields.
		/// </summary>
		/// <returns></returns>
		public static int getNumLevelFields()
		{
			return mNumLevelFields;
		}

		/// <summary>
		/// Internal.  Returns the number of fixed (non-vector) entry
		/// fields.
		/// </summary>
		/// <returns></returns>
		public static int getNumEntryFields ()
		{
			return mNumEntryFields;
		}

		/// <summary>
		/// Internal.  Returns whether the dictionary contains vector
		/// fields (important when unpacking order book messages).
		/// </summary>
		/// <returns></returns>
		public static bool getHasVectorFields()
		{
			return (PRICE_LEVELS != null);
		}

		/// <summary>
		/// Internal.  Returns whether the dictionary contains fixed price
		/// level fields (important when unpacking order book messages).
		/// </summary>
		/// <returns></returns>
		public static bool getHasFixedLevelFields()
		{
			return (mNumLevelFields > 0);
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
          mInitialised    = false;
          mNumLevelFields = 0;
          mNumEntryFields = 0;
          SRC_TIME       = null;
          ACTIVITY_TIME  = null;
          LINE_TIME      = null;
          PART_ID        = null;
          SEQNUM         = null;
          BOOK_TIME      = null;
          NUM_LEVELS     = null;
          PRICE_LEVELS   = null;
          PL_ACTION      = null;
          PL_PRICE       = null;
          PL_SIDE        = null;
          PL_SIZE        = null;
          PL_SIZE_CHANGE = null;
          PL_TIME        = null;
          PL_NUM_ENTRIES = null;
          PL_NUM_ATTACH  = null;
          PL_ENTRIES     = null;
          ENTRY_ID       = null;
          ENTRY_ACTION   = null;
          ENTRY_REASON   = null;
          ENTRY_SIZE     = null;
          ENTRY_TIME     = null;
          ENTRY_STATUS   = null;
          PRICE_LEVEL    = null;
          PL_ENTRY       = null;
          }
		
		public static MamaFieldDescriptor   SRC_TIME       = null;
		public static MamaFieldDescriptor   ACTIVITY_TIME  = null;
		public static MamaFieldDescriptor   LINE_TIME      = null;
		public static MamaFieldDescriptor   PART_ID        = null;
		public static MamaFieldDescriptor   SEQNUM         = null;
		public static MamaFieldDescriptor   BOOK_TIME      = null;
		public static MamaFieldDescriptor   NUM_LEVELS     = null;
		public static MamaFieldDescriptor   PRICE_LEVELS   = null;
		public static MamaFieldDescriptor   PL_ACTION      = null;
		public static MamaFieldDescriptor   PL_PRICE       = null;
		public static MamaFieldDescriptor   PL_SIDE        = null;
		public static MamaFieldDescriptor   PL_SIZE        = null;
		public static MamaFieldDescriptor   PL_SIZE_CHANGE = null;
		public static MamaFieldDescriptor   PL_TIME        = null;
		public static MamaFieldDescriptor   PL_NUM_ENTRIES = null;
		public static MamaFieldDescriptor   PL_NUM_ATTACH  = null;
		public static MamaFieldDescriptor   PL_ENTRIES     = null;
		public static MamaFieldDescriptor   ENTRY_ID       = null;
		public static MamaFieldDescriptor   ENTRY_ACTION   = null;
		public static MamaFieldDescriptor   ENTRY_REASON   = null;
		public static MamaFieldDescriptor   ENTRY_SIZE     = null;
		public static MamaFieldDescriptor   ENTRY_TIME     = null;
		public static MamaFieldDescriptor   ENTRY_STATUS   = null;
		public static MamaFieldDescriptor[] PRICE_LEVEL    = null;
		public static MamaFieldDescriptor[] PL_ENTRY       = null;

		private static int mNumLevelFields = 0;
		private static int mNumEntryFields = 0;
		private static bool mInitialised = false;
	}
}
