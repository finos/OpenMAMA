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
	/// Maintains a cache of common security status related FieldDescriptors.
	/// </summary>
	public class MamdaSecurityStatusFields : MamdaFields
	{
		/// <summary>
		/// Set the dictionary for common security status fields.
		/// Maintains a cache of MamaFieldDescriptors for common security status
		/// related fields. The <code>properties</code> parameter allows users
		/// of the API to map the common dictionary names to something else
		/// if they are being published under different names.
		/// </summary>
		/// <param name="dictionary">A reference to a valid MamaDictionary</param>
		/// <param name="properties">A NameValueCollection object containing field mappings.
		/// (See MamdaFields for more information)</param>
		public static void setDictionary(
			MamaDictionary dictionary,
			NameValueCollection properties)
		{
			if (mInitialised)
			{
				return;
			}

			string wSrcTime = lookupFieldName(properties, "wSrcTime");
			string wActivityTime = lookupFieldName(properties, "wActivityTime");
			string wSecurityStatus = lookupFieldName(properties, "wSecurityStatus");
			string wSecStatusQual = lookupFieldName(properties, "wSecStatusQual");
			string wSecurityStatusTime = lookupFieldName(properties, "wSecurityStatusTime");
			string wSecurityStatusOrig = lookupFieldName(properties, "wSecurityStatusOrig");
            string wShortSaleCircuitBreaker = lookupFieldName(properties,"wShortSaleCircuitBreaker");
			string wSeqNum = lookupFieldName(properties, "wSeqNum");
			string wReason = lookupFieldName(properties, "wReason");

			SRC_TIME					= dictionary.getFieldByName(wSrcTime);
			ACTIVITY_TIME				= dictionary.getFieldByName(wActivityTime);
			SECURITY_STATUS				= dictionary.getFieldByName(wSecurityStatus);
			SECURITY_STATUS_QUAL		= dictionary.getFieldByName(wSecStatusQual);
			SECURITY_STATUS_TIME		= dictionary.getFieldByName(wSecurityStatusTime);
			SECURITY_STATUS_ORIG		= dictionary.getFieldByName(wSecurityStatusOrig);
            SHORT_SALE_CIRCUIT_BREAKER	= dictionary.getFieldByName(wShortSaleCircuitBreaker);
			SEQNUM						= dictionary.getFieldByName(wSeqNum);
			REASON						= dictionary.getFieldByName(wReason);

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
            mInitialised				= false;
            MAX_FID					= 0;
            SRC_TIME				= null;
            ACTIVITY_TIME			= null;
            SECURITY_STATUS			= null;
            SECURITY_STATUS_QUAL	= null;
            SECURITY_STATUS_TIME	= null;
            SECURITY_STATUS_ORIG	= null;
            SHORT_SALE_CIRCUIT_BREAKER = null;
            SEQNUM					= null;
            REASON					= null;
        }

		public static MamaFieldDescriptor  SRC_TIME               = null;
		public static MamaFieldDescriptor  ACTIVITY_TIME          = null;
		public static MamaFieldDescriptor  SECURITY_STATUS        = null;
		public static MamaFieldDescriptor  SECURITY_STATUS_QUAL   = null;
		public static MamaFieldDescriptor  SECURITY_STATUS_TIME   = null;
		public static MamaFieldDescriptor  SECURITY_STATUS_ORIG   = null;
        public static MamaFieldDescriptor  SHORT_SALE_CIRCUIT_BREAKER = null;
		public static MamaFieldDescriptor  SEQNUM                 = null;
		public static MamaFieldDescriptor  REASON				  = null;
		public static int  MAX_FID = 0;
		private static bool mInitialised = false;
	}
}
