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
using System.Globalization;

namespace Wombat
{
	/// <summary>
	/// Wraps a date formatting settings
	/// </summary>
	public sealed class SimpleDateFormat
	{
		/// <summary>
		/// Creates a date format, using the specified date/time format, and an invariant culture
		/// <seealso cref="System.DateTime.ToString(System.String)" />
		/// </summary>
		public SimpleDateFormat(string format) : this(format, CultureInfo.InvariantCulture)
		{
		}

		/// <summary>
		/// Creates a date format, using the specified date/time format and culture (format provider)
		/// <seealso cref="System.DateTime.ToString(System.String, System.IFormatProvider)"/>
		/// </summary>
		public SimpleDateFormat(string format, IFormatProvider formatProvider)
		{
			mFormat = format;
			mFormatProvider= formatProvider;
		}

		public string FormatString
		{
			get
			{
				return mFormat;
			}
		}

		public IFormatProvider FormatProvider
		{
			get
			{
				return mFormatProvider;
			}
		}

		public string Format(DateTime dateTime)
		{
			return Format(dateTime, mFormat, mFormatProvider);
		}

		public static string Format(DateTime dateTime, string format, IFormatProvider formatProvider)
		{
			return dateTime.ToString(format, formatProvider);
		}

		public DateTime Parse(string dateTimeString)
		{
			return Parse(dateTimeString, mFormat, mFormatProvider);
		}

		public static DateTime Parse(string dateTimeString, string format, IFormatProvider formatProvider)
		{
			return DateTime.ParseExact(dateTimeString, format, formatProvider);
		}

		private string mFormat;
		private IFormatProvider mFormatProvider;
	}
}
