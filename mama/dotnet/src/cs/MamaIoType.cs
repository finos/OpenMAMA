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

namespace Wombat
{
	/// <summary>
	/// IO Types. Not all implementations support all mamaIoTypes.
	/// </summary>
	public enum mamaIoType
	{
		/// <summary>
		/// the socket is readable
		/// </summary>
		MAMA_IO_READ,
		/// <summary>
		/// the socket is writeable
		/// </summary>
		MAMA_IO_WRITE,
		/// <summary>
		/// the socket is connected
		/// </summary>
		MAMA_IO_CONNECT,
		/// <summary>
		/// the socket accepted a connection
		/// </summary>
		MAMA_IO_ACCEPT,
		/// <summary>
		/// the socket was closed
		/// </summary>
		MAMA_IO_CLOSE,
		/// <summary>
		/// an error occurred
		/// </summary>
		MAMA_IO_ERROR,
		/// <summary>
		/// An exceptional event like out of band data occurred
		/// </summary>
		MAMA_IO_EXCEPT
	}
}
