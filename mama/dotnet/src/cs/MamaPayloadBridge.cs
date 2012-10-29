/* $Id: MamaPayloadBridge.cs,v 1.1.2.3 2012/08/24 16:12:01 clintonmcdowell Exp $
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
using System.Runtime.InteropServices;

namespace Wombat
{
	/// <summary>
	/// </summary>
	public class MamaPayloadBridge : MamaWrapper
	{
		public MamaPayloadBridge (string payload): base()
		{
			int code = NativeMethods.mama_loadPayloadBridge (ref nativeHandle, payload);
			CheckResultCode (code);
		}
        
        /// <summary>
		/// Destroy a mamaPayloadBridge object.
		/// </summary>
		public void destroy ()
		{
			Dispose();
		}

		/// <summary>
		/// Implements the destruction of the underlying peer object
		/// <seealso cref="MamaWrapper.DestroyNativePeer"/>
		/// </summary>
		/// <returns>MAMA Status code</returns>
		protected override MamaStatus.mamaStatus DestroyNativePeer()
		{
			return MamaStatus.mamaStatus.MAMA_STATUS_OK;
		}

		#region Implementation details

		private struct NativeMethods
		{
			// export definitions
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mama_loadPayloadBridge (ref IntPtr impl,
				string payload);
        }
		

		#endregion // Implementation details
	}
}
