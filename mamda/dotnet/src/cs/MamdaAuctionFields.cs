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
	public class MamdaAuctionFields : MamdaFields
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
			if (initialised)
			{
				return;
			}

			string wUncrossPrice    = lookupFieldName (properties, "wUncrossPrice");
			string wUncrossVolume   = lookupFieldName (properties, "wUncrossVolume");
			string wUncrossPriceInd = lookupFieldName (properties, "wUncrossPriceInd");
			

			UNCROSS_PRICE       = dictionary.getFieldByName (wUncrossPrice);
			UNCROSS_VOLUME      = dictionary.getFieldByName (wUncrossVolume);
			UNCROSS_PRICE_IND   = dictionary.getFieldByName (wUncrossPriceInd);
			
			MAX_FID = dictionary.getMaxFid();
			initialised = true;
		}

		public static int getMaxFid()
		{
			return MAX_FID;
		}

		public static bool isSet()
		{
			return initialised;
		}

        public static void reset ()
        {
            if (MamdaCommonFields.isSet())
            {
                MamdaCommonFields.reset();
            }
            initialised           = false;
            MAX_FID               = 0;
        
            UNCROSS_PRICE         = null;
            UNCROSS_VOLUME        = null;
            UNCROSS_PRICE_IND     = null;
            
        }
        
		public static MamaFieldDescriptor  UNCROSS_PRICE        = null;
		public static MamaFieldDescriptor  UNCROSS_VOLUME       = null;
		public static MamaFieldDescriptor  UNCROSS_PRICE_IND    = null;

		public  static int      MAX_FID     = 0;
		private static bool     initialised = false;
	}

}

