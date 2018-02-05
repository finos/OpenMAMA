/* $Id: MamaSymbolMapFile.cs,v 1.4.20.4 2012/07/26 11:27:10 stuartbeattie Exp $
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
	/// A file-based symbol mapper
	/// </summary>
    /// <remarks>
    /// Use MamaSymbolMapFile load() to
    /// open the file. If the file exists, its contents should be a two
    /// columns of data, with the columns separated by white space.  A
    /// matching symbol of the left column is mapped to the symbol in the
    /// right column. When performing the mapping, if the symbol does not
    /// match anything in the file (or the file cannot be found), then the
    /// original symbol is used (i.e. no mapping)
	/// </remarks>
	public class MamaSymbolMapFile : MamaWrapper
	{
		/// <summary>
		/// <see cref="M:Wombat.MamaWrapper.#ctor" />
		/// </summary>
		public MamaSymbolMapFile() : base()
		{
		}

		/// <summary>
        /// Create the object
		/// </summary>
		public void create()
		{
			int code = NativeMethods.mamaSymbolMapFile_create(ref nativeHandle);
			CheckResultCode(code);
		}

		/// <summary>
		/// (Re)Load the map for the file-based symbol mapper.
		/// </summary>
		public void load(string fileName)
		{
			int code = NativeMethods.mamaSymbolMapFile_load(nativeHandle, fileName);
			CheckResultCode(code);
		}

		/// <summary>
		/// Add additional individual symbology mapping.
		/// </summary>
		public void addMap(
			string fromSymbol,
			string toSymbol)
		{
			NativeMethods.mamaSymbolMapFile_addMap(nativeHandle, fromSymbol, toSymbol);
		}

		/// <summary>
		/// Perform a mapping from the file-based symbol mapper.
		/// </summary>
		public int map(
			ref string result,
			string symbol,
			int maxLen)
		{
			string resultString = new string ('\x0000', maxLen);
			IntPtr resultPtr = Marshal.StringToHGlobalAnsi (resultString);
			int code = NativeMethods.mamaSymbolMapFile_map(nativeHandle, resultPtr, symbol, maxLen);
			result = Marshal.PtrToStringAnsi (resultPtr);
			Marshal.FreeHGlobal (resultPtr);
			return code;
		}

		/// <summary>
		/// Perform a reverse mapping from the file-based symbol mapper.
		/// </summary>
		public int revMap(
			ref string result,
			string symbol,
			int maxLen)
		{
			string resultString = new string ('\x0000', maxLen);
			IntPtr resultPtr = Marshal.StringToHGlobalAnsi (resultString);
			int code = NativeMethods.mamaSymbolMapFile_revMap(nativeHandle, resultPtr, symbol, maxLen);
			result = Marshal.PtrToStringAnsi (resultPtr);
			Marshal.FreeHGlobal (resultPtr);
			return code;
		}

		/// <summary>
		/// Implements the destruction of the underlying peer object.
		/// <seealso cref="MamaWrapper.DestroyNativePeer"/>
		/// </summary>
		protected override MamaStatus.mamaStatus DestroyNativePeer()
		{
			return (MamaStatus.mamaStatus)NativeMethods.mamaSymbolMapFile_destroy(nativeHandle);
		}

		private struct NativeMethods
		{
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSymbolMapFile_create(
				ref IntPtr fileMap);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSymbolMapFile_destroy(
				IntPtr fileMap);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSymbolMapFile_load(
				IntPtr fileMap,
				string fileName);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void mamaSymbolMapFile_addMap(
				IntPtr fileMap,
				string fromSymbol,
				string toSymbol);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSymbolMapFile_map(
				IntPtr fileMap,
				IntPtr result,
				string symbol,
				int maxLen);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSymbolMapFile_revMap(
				IntPtr fileMap,
				IntPtr result,
				string symbol,
				int maxLen);
		}
	}
}
