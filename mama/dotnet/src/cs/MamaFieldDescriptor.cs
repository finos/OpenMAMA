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
	/// A class that describes a field in a <see cref="MamaMsg"/>
	/// </summary>
	public class MamaFieldDescriptor : MamaWrapper
	{
		internal MamaFieldDescriptor(IntPtr handle) : base(handle)
		{
		}

		/// <summary/>
		public MamaFieldDescriptor(
			int fid,
			mamaFieldType type,
			string name,
			bool trackModState) : base()
		{
			int code = NativeMethods.mamaFieldDescriptor_create(ref nativeHandle, fid, (int)type, name);
			CheckResultCode(code);
			this.mTrackModState = trackModState;
		}

		/// <summary/>
		public MamaFieldDescriptor(
			int fid,
			mamaFieldType type,
			string name) : this(fid, type, name, false)
		{
		}

		/// <summary>
		/// Destroy a MamaFieldDescriptor
		/// </summary>
		public void destroy()
		{
			Dispose();
		}

		/// <summary>
		/// Implements the destruction of the underlying peer object
		/// </summary>
		protected override MamaStatus.mamaStatus DestroyNativePeer()
		{
			return (MamaStatus.mamaStatus)NativeMethods.mamaFieldDescriptor_destroy(nativeHandle);
		}

		/// <summary>
		/// Return the human readable name of the field.
		/// </summary>
		public string getName () 
		{
			EnsurePeerCreated();
			return Marshal.PtrToStringAnsi(NativeMethods.mamaFieldDescriptor_getName(nativeHandle));
		}
	
		/// <summary>
		/// Return the field identifier
		/// </summary>
		public int getFid () 
		{
			EnsurePeerCreated();
			return NativeMethods.mamaFieldDescriptor_getFid(nativeHandle);
		}

		/// <summary>
		/// Return the data type
		/// </summary>
		public mamaFieldType getType () 
		{
			EnsurePeerCreated();
			return (mamaFieldType)NativeMethods.mamaFieldDescriptor_getType(nativeHandle);
		}
	 
		/// <summary>
		/// Return a human readable string for mamaMsgType
		/// </summary>
		public string getTypeName () 
		{
			EnsurePeerCreated();
			return Marshal.PtrToStringAnsi(NativeMethods.mamaFieldDescriptor_getTypeName(nativeHandle));
		}

		/// <summary>
		/// Track the modification state
		/// </summary>
		public bool trackModState
		{
			get
			{
				EnsurePeerCreated();
				return mTrackModState;
			}
			set
			{
				EnsurePeerCreated();
				mTrackModState = value;
			}
		}

		/// <summary>
		/// User-supplied data that is associated with the field descriptor
		/// </summary>
		public object closure
		{
			get
			{
				EnsurePeerCreated();
				return mClosure;
			}
			set
			{
				EnsurePeerCreated();
				mClosure = value;
			}
		}

        
		/// <summary>
		/// Publish name for the field
		/// </summary>
		public string pubName
		{
			get
			{
				EnsurePeerCreated();
				return mPubName != null ? mPubName : getName();
			}
			set
			{
				EnsurePeerCreated();
				mPubName = value;
			}
		}

		#region Implementation details

		private struct NativeMethods
		{
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaFieldDescriptor_destroy(IntPtr handle);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern IntPtr mamaFieldDescriptor_getName(IntPtr handle);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern ushort mamaFieldDescriptor_getFid(IntPtr handle);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaFieldDescriptor_getType(IntPtr handle);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern IntPtr mamaFieldDescriptor_getTypeName(IntPtr handle);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaFieldDescriptor_create(
				ref IntPtr  handle,
				int fid,
				int type,
				[MarshalAs(UnmanagedType.LPStr)] string name);
		}

		// state
		private bool mTrackModState;
		private object mClosure;
		private string mPubName;

		#endregion // Implementation details
	}
}
