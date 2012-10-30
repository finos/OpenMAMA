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
	/// Maintains a cache of common update field descriptors.
	/// </summary>
	public class MamdaCommonFields : MamdaFields
	{
		private MamdaCommonFields()
		{
		}

		/// <summary>
		/// Set the dictionary for common update fields.
		/// Maintains a cache of MamaFieldDescriptors for common update
		/// related fields.
		/// The <code>properties</code> parameter allows users
		/// of the API to map the common dictionary names to
		/// something else if they are beig published under different
		/// names.
		/// </summary>
		/// <param name="dictionary">A reference to a valid MamaDictionary</param>
		/// <param name="properties">A NameValueCollection object containing field mappings. (See
		/// MamdaFields for more information)</param>
		public static void setDictionary(
			MamaDictionary dictionary,
			NameValueCollection properties)
		{
			if (mInitialised)
			{
				return;
			}

			String wSymbol = lookupFieldName(properties, "wSymbol");
			String wIssueSymbol = lookupFieldName(properties, "wIssueSymbol");
            String wIndexSymbol = lookupFieldName(properties, "wIndexSymbol");
            String wPartId = lookupFieldName(properties, "wPartId");
            String wSeqNum = lookupFieldName(properties, "wSeqNum");
            String wSrcTime = lookupFieldName(properties, "wSrcTime");
            String wLineTime = lookupFieldName(properties, "wLineTime");
            String wActivityTime = lookupFieldName(properties, "wActivityTime");
            String wPubId = lookupFieldName(properties, "wPubId");
            String wMsgQual = lookupFieldName(properties, "wMsgQual");
            
			SYMBOL = dictionary.getFieldByName(wSymbol);
			ISSUE_SYMBOL = dictionary.getFieldByName(wIssueSymbol);
            INDEX_SYMBOL = dictionary.getFieldByName(wIndexSymbol);
			PART_ID = dictionary.getFieldByName(wPartId);
            SEQ_NUM = dictionary.getFieldByName(wSeqNum);
			SRC_TIME = dictionary.getFieldByName(wSrcTime);
            LINE_TIME = dictionary.getFieldByName(wLineTime);
			ACTIVITY_TIME = dictionary.getFieldByName(wActivityTime);
            PUB_ID = dictionary.getFieldByName(wPubId);
			MSG_QUAL = dictionary.getFieldByName(wMsgQual);
        
			mInitialised = true;
		}

		public static bool isSet()
		{
			return mInitialised;
		}
        public static void reset ()
        {
            INDEX_SYMBOL  = null;
            PART_ID       = null;
            SEQ_NUM       = null;
            SRC_TIME      = null;
            LINE_TIME     = null;
            ACTIVITY_TIME = null;
            SEND_TIME     = null;
            PUB_ID        = null;
            MSG_QUAL      = null;
            MSG_SEQ_NUM   = null;
            MSG_NUM       = null;
            MSG_TOTAL     = null;
            SENDER_ID     = null;
            mInitialised  = false;
        }

		public static MamaFieldDescriptor  SYMBOL        = null;
		public static MamaFieldDescriptor  ISSUE_SYMBOL  = null;
        public static MamaFieldDescriptor  INDEX_SYMBOL  = null;
        public static MamaFieldDescriptor  PART_ID       = null;
        public static MamaFieldDescriptor  SEQ_NUM       = null;
        public static MamaFieldDescriptor  SRC_TIME      = null;
        public static MamaFieldDescriptor  LINE_TIME     = null;
        public static MamaFieldDescriptor  ACTIVITY_TIME = null;
        public static MamaFieldDescriptor  SEND_TIME     = null;
        public static MamaFieldDescriptor  PUB_ID        = null;
        public static MamaFieldDescriptor  MSG_QUAL      = null;
        public static MamaFieldDescriptor  MSG_SEQ_NUM   = null;
        public static MamaFieldDescriptor  MSG_NUM       = null;
        public static MamaFieldDescriptor  MSG_TOTAL     = null;
        public static MamaFieldDescriptor  SENDER_ID     = null;
    
    
		private static bool mInitialised = false;
	}
    

}
