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
using System.Runtime.InteropServices;

namespace Wombat
{
    /// <summary>
    /// A class containing various information about the MAMA client
    /// </summary>
	public class MamaClientProfile : MamaWrapper
	{
		public MamaClientProfile(): base()
		{
		}

		/// <summary>
		/// Create a mamaClientProfile object. Attribute data will be copied into
		/// allocated memory.
		/// </summary>
		public void create()
		{
			int code = NativeMethods.mamaClientProfile_create (ref nativeHandle);
			CheckResultCode(code);
		}

		/// <summary>
		/// Create a mamaClientProfile object. The attribute data will not be copied,
		/// only the user supplied pointers to the attributes be stored.
		/// </summary>
		public void createNoCopyData()
		{
			int code = NativeMethods.mamaClientProfile_createNoCopyData (nativeHandle);
			CheckResultCode(code);
		}

		/// <summary>
		/// Return whether or not the profile copies attribute data.
		/// </summary>
		public bool getCopyData()
		{
			bool result = false;
			int code = NativeMethods.mamaClientProfile_getCopyData (nativeHandle, ref result);
			CheckResultCode(code);
			return result;
		}

		/// <summary>
		/// Clear a mamaClientProfile object.
		/// </summary>
		public void clear()
		{
			int code = NativeMethods.mamaClientProfile_clear (nativeHandle);
			CheckResultCode(code);
		}

		/// <summary>
		/// Set the ID of a mamaClientProfile object.
		/// </summary>
		public void setInterfaceVersion (byte id)
		{
			int code = NativeMethods.mamaClientProfile_setInterfaceVersion (nativeHandle, id);
			CheckResultCode(code);
		}

		/// <summary>
		/// Get the ID of a mamaClientProfile object.
		/// </summary>
		public byte getInterfaceVersion()
		{
			byte result = 0;
			int code = NativeMethods.mamaClientProfile_getInterfaceVersion (nativeHandle, ref result);
			CheckResultCode(code);
			return result;
		}

		/// <summary>
		/// Set the user name.
		/// </summary>
		public void setUserName (string userName)
		{
			int code = NativeMethods.mamaClientProfile_setUserName (nativeHandle, userName);
			CheckResultCode(code);
		}

		/// <summary>
		/// Get the user name.
		/// </summary>
		public string getUserName()
		{
			IntPtr result = IntPtr.Zero;
			int code = NativeMethods.mamaClientProfile_getUserName (nativeHandle, ref result);
			CheckResultCode(code);
			return Marshal.PtrToStringAnsi(result);
		}

		/// <summary>
		/// Implements the destruction of the underlying peer object.
		/// <seealso cref="MamaWrapper.DestroyNativePeer"/>
		/// </summary>
		protected override MamaStatus.mamaStatus DestroyNativePeer()
		{
			return (MamaStatus.mamaStatus)NativeMethods.mamaClientProfile_destroy (nativeHandle);
		}

		#region Implementation details

		private struct NativeMethods
		{
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaClientProfile_create (ref IntPtr profile);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaClientProfile_createNoCopyData (IntPtr profile);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaClientProfile_getCopyData (IntPtr profile,
				ref bool copyData);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaClientProfile_destroy (IntPtr profile);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaClientProfile_clear (IntPtr profile);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaClientProfile_setInterfaceVersion (IntPtr profile,
				byte id);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaClientProfile_getInterfaceVersion (IntPtr profile,
				ref byte id);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaClientProfile_setUserName (IntPtr profile,
				string userName);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaClientProfile_getUserName (IntPtr profile,
				ref IntPtr userName);
		}

		#endregion // Implementation details
	}
}
