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
	/// Common base class for all the MamdaXXXFields classes.
	/// 
	/// Each of the subclasses maintains a cache of relevant MamaFieldDescriptors 
	/// obtained from the MamaDictionary using the fields standard name.
	/// If the dictionary is being published with non standard field name mappings
	/// users can pass a <code>NameValueCollection</code> object to the corresponding 
	/// <code>setDictionary()</code> method which contains field mappings.
	/// <br />
	/// Each mapping should have the following format:<br />
	/// mamda.field.&lt;common field name&gt;=&lt;mapped field name&gt; e.g.<br />
	/// mamda.field.wTradePrice=mMappedTradePrice where mMappedTradePrice is the
	/// name of the field under which the trade price is being published by the
	/// feedhandlers and which is being published by the dictionary to describe the
	/// trade price field.
	/// </summary>
	public class MamdaFields
	{
		/// <summary>
		/// </summary>
		/// <param name="properties"></param>
		/// <param name="defaultFieldName"></param>
		/// <returns></returns>
		protected static string lookupFieldName(
			NameValueCollection properties,
			string defaultFieldName)
		{
			string result = defaultFieldName;
			if (properties != null)
			{
				string possibleFieldName = 
					properties["mamda.field." + defaultFieldName];
				if (possibleFieldName != null)
					result = possibleFieldName;
			}
			return result;
		}	
	}
}
