/* $Id: MamdaOptionFields.cs,v 1.5.30.5 2012/08/24 16:12:12 clintonmcdowell Exp $
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
	/// Maintains a cache of common options related field descriptors.
	/// </summary>
	public class MamdaOptionFields : MamdaFields
	{
		/// <summary>
		/// Set the dictionary for common options fields.
		/// Maintains a cache of MamaFieldDescriptors for common options
		/// related fields. The <code>properties</code> parameter allows users
		/// of the API to map the common dictionary names to something else
		/// if they are beig published under different names.
		/// </summary>
		/// <param name="dictionary">A reference to a valid MamaDictionary</param>
		/// <param name="properties">A NameValueCollection object containing field mappings.
		/// (See MamdaFields for more information)</param>
		public static void setDictionary(
			MamaDictionary		dictionary,
			NameValueCollection	properties)
		{
			if (mInitialised)
			{
				return;
			}

			string wIssueSymbol      = lookupFieldName(properties, "wIssueSymbol");
			string wUnderlyingSymbol = lookupFieldName(properties, "wUnderlyingSymbol");
			string wExpirationDate   = lookupFieldName(properties, "wExpirationDate");
			string wStrikePrice      = lookupFieldName(properties, "wStrikePrice");
			string wPutCall          = lookupFieldName(properties, "wPutCall");
            string wExerciseStyle    = lookupFieldName(properties, "wExerciseStyle");
            string wOpenInterest     = lookupFieldName(properties, "wOpenInterest");

			CONTRACT_SYMBOL			 = dictionary.getFieldByName(wIssueSymbol);
			UNDERLYING_SYMBOL		 = dictionary.getFieldByName(wUnderlyingSymbol);
			EXPIRATION_DATE			 = dictionary.getFieldByName(wExpirationDate);
			STRIKE_PRICE			 = dictionary.getFieldByName(wStrikePrice);
            PUT_CALL    			 = dictionary.getFieldByName(wPutCall);
            EXERCISE_STYLE           = dictionary.getFieldByName(wExerciseStyle);
            OPEN_INTEREST            = dictionary.getFieldByName(wOpenInterest);

			mInitialised = true;
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
            CONTRACT_SYMBOL       = null;
            UNDERLYING_SYMBOL     = null;
            EXPIRATION_DATE       = null;
            STRIKE_PRICE          = null;
            PUT_CALL              = null;
            EXERCISE_STYLE        = null;
            OPEN_INTEREST         = null;
        }
		public static MamaFieldDescriptor CONTRACT_SYMBOL;
		public static MamaFieldDescriptor UNDERLYING_SYMBOL;
		public static MamaFieldDescriptor EXPIRATION_DATE;
		public static MamaFieldDescriptor STRIKE_PRICE;
		public static MamaFieldDescriptor PUT_CALL;
		public static MamaFieldDescriptor EXERCISE_STYLE;
		public static MamaFieldDescriptor OPEN_INTEREST;
		private static bool mInitialised = false;
	}
}
