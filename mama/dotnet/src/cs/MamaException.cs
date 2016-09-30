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
using System.Runtime.Serialization;

namespace Wombat
{
	/// <summary>
	/// Provides a MAMA API-specific exception which captures the
	/// status code of the last MAMA API call and a textual
	/// description of the error.
	/// </summary>
	public class MamaException : Exception
	{
		/// <summary>
		/// Constructs a new MamaException with the specified status code.
		/// Automatically populates the textual description of the error.
		/// </summary>
		/// <param name="status">the MAMA status code</param>
		public MamaException(MamaStatus.mamaStatus status) :
			this(status, MamaStatus.stringForStatus(status))
		{
		}

		/// <summary>
		/// Constructs a new MamaException with the specified status code
		/// and a custom error message.
		/// </summary>
		/// <param name="status">the MAMA status code</param>
		/// <param name="message">the custom error message</param>
		public MamaException(MamaStatus.mamaStatus status, string message) :
			base(message != null ? message : String.Empty)
		{
            this.mStatus = status;
		}

		/// <summary>
		/// Returns the MAMA status code.
		/// </summary>
		public MamaStatus.mamaStatus Status
		{
			get
			{
                return mStatus;
			}
		}

		#region Required serialization support (for Remoting, etc.)

		protected MamaException(
			SerializationInfo info,
			StreamingContext context) :
				base(info, context)
		{
            mStatus = (MamaStatus.mamaStatus)info.GetInt32(statusKey);
		}

		public override void GetObjectData(SerializationInfo info, StreamingContext context)
		{
			base.GetObjectData(info, context);
            info.AddValue(statusKey, (int)mStatus);
		}

		#endregion

		private readonly MamaStatus.mamaStatus mStatus;
		private const string statusKey = "mama.status";

	}
}
