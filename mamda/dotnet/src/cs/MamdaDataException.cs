/* $Id: MamdaDataException.cs,v 1.1.40.5 2012/08/24 16:12:11 clintonmcdowell Exp $
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
using System.Runtime.Serialization;

namespace Wombat
{
	/// <summary>
	/// MAMDA data exceptions.
	/// </summary>
	public class MamdaDataException : Exception
	{
		/// <summary>
		/// </summary>
		public MamdaDataException() : base()
		{
		}

		/// <summary>
		/// </summary>
		/// <param name="message"></param>
		public MamdaDataException(string message) : base(message)
		{
		}

		/// <summary>
		/// </summary>
		/// <param name="message"></param>
		/// <param name="innerException"></param>
		public MamdaDataException(string message, Exception innerException) : base(message, innerException)
		{
		}

		/// <summary>
		/// </summary>
		/// <param name="innerException"></param>
		public MamdaDataException(Exception innerException) : base(innerException.Message, innerException)
		{
		}

		/// <summary>
		/// </summary>
		/// <param name="info"></param>
		/// <param name="context"></param>
		protected MamdaDataException(SerializationInfo info, StreamingContext context) : 
			base(info, context)
		{
		}

		/// <summary>
		/// </summary>
		/// <param name="info"></param>
		/// <param name="context"></param>
		public override void GetObjectData(SerializationInfo info, StreamingContext context)
		{
			base.GetObjectData(info, context);
		}
	}
}
