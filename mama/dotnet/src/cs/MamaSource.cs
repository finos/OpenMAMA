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
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace Wombat
{
	/// <summary>
    /// A MamaSource maintains information about a data source, including
    /// the quality of the data coming from that source
	/// </summary>
	public class MamaSource : MamaWrapper, ISourceManager
	{

		/// <summary>
		/// Constructs <b>and</b> creates a new source object.
		/// <seealso cref="M:Wombat.MamaWrapper.#ctor" />
		/// </summary>
		public MamaSource() : base()
		{
			int code = NativeMethods.mamaSource_create(ref nativeHandle);
			CheckResultCode(code);
		}

		/// <summary>
		/// <see cref="M:Wombat.MamaWrapper.#ctor(System.IntPtr)" />
		/// </summary>
		internal MamaSource(IntPtr nativeHandle) : base(nativeHandle)
		{
		}

        /// <summary>
        /// Implements the destruction of the underlying peer object
        /// <seealso cref="MamaWrapper.DestroyNativePeer"/>
        /// </summary>
        /// <returns>MAMA Status code</returns>
        protected override MamaStatus.mamaStatus DestroyNativePeer()
		{
			int code = NativeMethods.mamaSource_destroy(nativeHandle);
			return (MamaStatus.mamaStatus)code;
		}

		/// <summary>
		/// Destroy the source.
		/// A synonym to the <see cref="MamaWrapper.Dispose()">MamaWrapper.Dispose</see> method.
		/// </summary>
		public void destroy()
		{
			Dispose();
		}

		/// <summary>
		/// Clear the source
		/// </summary>
		public void clear()
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaSource_clear(nativeHandle);
			CheckResultCode(code);
			++mContainerVersion;
		}

		/// <summary>
		/// Gets or sets the ID of the source
		/// </summary>
		public string id
		{
			get
			{
				EnsurePeerCreated();
				IntPtr value = IntPtr.Zero;
				int code = NativeMethods.mamaSource_getId(nativeHandle, ref value);
				CheckResultCode(code);
				return Marshal.PtrToStringAnsi(value);
			}
			set
			{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
				if (value == null)
				{
					throw new ArgumentNullException("id");
				}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
				EnsurePeerCreated();
				int code = NativeMethods.mamaSource_setId(nativeHandle, value);
				CheckResultCode(code);
			}
		}

		/// <summary>
		/// Gets or sets the display ID of the source
		/// </summary>
		public string displayId
		{
			get
			{
				EnsurePeerCreated();
				IntPtr value = IntPtr.Zero;
				int code = NativeMethods.mamaSource_getDisplayId(nativeHandle, ref value);
				CheckResultCode(code);
				return  Marshal.PtrToStringAnsi(value);
			}
			set
			{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
				if (value == null)
				{
					throw new ArgumentNullException("displayId");
				}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
				EnsurePeerCreated();
				int code = NativeMethods.mamaSource_setDisplayId(nativeHandle, value);
				CheckResultCode(code);
			}
		}

		/// <summary>
		/// Gets or sets the mapped ID of the source
		/// The mapped ID is the ID that parent source mManager has mapped this source as.
		/// </summary>
		public string mappedId
		{
			get
			{
				EnsurePeerCreated();
				IntPtr value = IntPtr.Zero;
				int code = NativeMethods.mamaSource_getMappedId(nativeHandle, ref value);
				CheckResultCode(code);
				return Marshal.PtrToStringAnsi(value);
			}
			set
			{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
				if (value == null)
				{
					throw new ArgumentNullException("mappedId");
				}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
				EnsurePeerCreated();
				int code = NativeMethods.mamaSource_setMappedId(nativeHandle, value);
				CheckResultCode(code);
			}
		}

		/// <summary>
		/// Gets or sets the quality of the source
		/// </summary>
		public mamaQuality quality
		{
			get
			{
				EnsurePeerCreated();
				int value = 0;
				int code = NativeMethods.mamaSource_getQuality(nativeHandle, ref value);
				CheckResultCode(code);
				Debug.Assert(Enum.IsDefined(typeof(mamaQuality), value));
				return (mamaQuality)value;
			}
			set
			{
				EnsurePeerCreated();
				int code = NativeMethods.mamaSource_setQuality(nativeHandle, (int)value);
				CheckResultCode(code);
			}
		}

		/// <summary>
		/// Gets or sets the state of the source
		/// </summary>
		public mamaSourceState state
		{
			get
			{
				EnsurePeerCreated();
				int value = 0;
				int code = NativeMethods.mamaSource_getState(nativeHandle, ref value);
				CheckResultCode(code);
				Debug.Assert(Enum.IsDefined(typeof(mamaSourceState), value));
				return (mamaSourceState)value;
			}
			set
			{
				EnsurePeerCreated();
				int code = NativeMethods.mamaSource_setQuality(nativeHandle, (int)value);
				CheckResultCode(code);
			}
		}

		/// <summary>
		/// Get the stringified representation of the source state.
		/// </summary>
		public string stateAsString
		{
			get
			{
				EnsurePeerCreated();
				string value = Marshal.PtrToStringAnsi(NativeMethods.mamaSource_getStateAsString(nativeHandle));
				return value;
			}
		}

		/// <summary>
		/// Gets or sets the parent for the source. When the parent is set
		/// the source is added to the parent source mManager.
		/// </summary>
		public MamaSource parent
		{
			get
			{
				EnsurePeerCreated();
				IntPtr parentHandle = IntPtr.Zero;
				int code = NativeMethods.mamaSource_getParent(nativeHandle, ref parentHandle);
				CheckResultCode(code);
				return new MamaSource(parentHandle);
			}
			set
			{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
				if (value == null)
				{
					throw new ArgumentNullException("parent");
				}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
				EnsurePeerCreated();
				int code = NativeMethods.mamaSource_setParent(nativeHandle, value.NativeHandle);
				CheckResultCode(code);
				GC.KeepAlive(value);
			}
		}

		/// <summary>
		/// Gets or sets the publisher specific source name for this source. E.g. This could be
		/// "NASDAQ" for a UTP PAPA publisher.
		/// </summary>
		public string symbolNamespace
		{
			get
			{
				EnsurePeerCreated();
				IntPtr value = IntPtr.Zero;
				int code = NativeMethods.mamaSource_getSymbolNamespace(nativeHandle, ref value);
				CheckResultCode(code);
				return Marshal.PtrToStringAnsi(value);
			}
			set
			{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
				if (value == null)
				{
					throw new ArgumentNullException("symbolNamespace");
				}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
				EnsurePeerCreated();
				int code = NativeMethods.mamaSource_setSymbolNamespace(nativeHandle, value);
				CheckResultCode(code);
			}
		}

		/// <summary>
		/// Gets or sets the name of the MamaTransport on which this describes a valid source of
		/// </summary>
		public string transportName
		{
			get
			{
				EnsurePeerCreated();
				IntPtr value = IntPtr.Zero;
				int code = NativeMethods.mamaSource_getTransportName(nativeHandle, ref value);
				CheckResultCode(code);
				return Marshal.PtrToStringAnsi(value);
			}
			set
			{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
				if (value == null)
				{
					throw new ArgumentNullException("transportName");
				}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
				EnsurePeerCreated();
				int code = NativeMethods.mamaSource_setTransportName(nativeHandle, value);
				CheckResultCode(code);
			}
		}

		/// <summary>
		/// Returns or associates a transport object with the source.
		/// </summary>
		public MamaTransport transport
		{
			get
			{
				EnsurePeerCreated();
				IntPtr handle = IntPtr.Zero;
				int code = NativeMethods.mamaSource_getTransport(nativeHandle, ref handle);
				CheckResultCode(code);
				mTransport.setNativeHandle (handle);
				return mTransport;
			}
			set
			{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
				if (value == null)
				{
					throw new ArgumentNullException("transport");
				}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
				EnsurePeerCreated();
				mTransport = value;
				int code = NativeMethods.mamaSource_setTransport(nativeHandle, value.NativeHandle);
				CheckResultCode(code);
			}
		}

		#region ISourceManager implementation

		/// <summary>
		/// Implements <see cref="ISourceManager.create">ISourceManager.create</see>
		/// </summary>
		public MamaSource create(string name)
		{
			return Manager.create(name);
		}

		/// <summary>
		/// Implements <see cref="ISourceManager.findOrCreate">ISourceManager.findOrCreate</see>
		/// </summary>
		public MamaSource findOrCreate(string name)
		{
			return Manager.findOrCreate(name);
		}

		/// <summary>
		/// Implements <see cref="ISourceManager.find">ISourceManager.find</see>
		/// </summary>
		public MamaSource find(string name)
		{
			return Manager.find(name);
		}

		/// <summary>
		/// Implements <see cref="ISourceManager.add(MamaSource)">ISourceManager.add(MamaSource)</see>
		/// </summary>
		public void add(MamaSource source)
		{
			Manager.add(source);
		}

		/// <summary>
		/// Implements <see cref="ISourceManager.add(System.String, MamaSource)">ISourceManager.add(System.String, MamaSource)</see>
		/// </summary>
		public void add(string name, MamaSource source)
		{
			Manager.add(name, source);
		}

		#endregion ISourceManager implementation

		#region Implementation details

		/// <summary>
		/// There's no need to expose a separate ISourceManager property as the source already
		/// implements the interface
		/// </summary>
		private MamaSourceManager Manager
		{
			get
			{
				if (!mAttemptedToLoadSubSourceManager)
				{
					mAttemptedToLoadSubSourceManager = true;
					IntPtr subSourceManager = IntPtr.Zero;
					int code = NativeMethods.mamaSource_getSubSourceManager(nativeHandle, ref subSourceManager);
					CheckResultCode(code);
					mManager = new MamaSourceManager(subSourceManager);
				}
				Debug.Assert(mManager != null);
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
				if (mManager == null)
				{
					throw new InvalidOperationException("SubSourceManager could not be obtained");
				}
#endif
				return mManager;
			}
		}

		// Interop API
		private struct NativeMethods
		{
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_create(ref IntPtr source);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_destroy(IntPtr source);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_clear(IntPtr source);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_setId(
				IntPtr source,
				[MarshalAs(UnmanagedType.LPStr)] string id);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_setMappedId(
				IntPtr source,
				[MarshalAs(UnmanagedType.LPStr)] string id);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_setDisplayId(
				IntPtr source,
				[MarshalAs(UnmanagedType.LPStr)] string id);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_setQuality(
				IntPtr source,
				int quality);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_setState(
				IntPtr source,
				int sourceState);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_setParent(
				IntPtr source,
				IntPtr parent);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_setSymbolNamespace(
				IntPtr source,
				[MarshalAs(UnmanagedType.LPStr)] string symbolNamespace);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_setTransportName(
				IntPtr source,
				[MarshalAs(UnmanagedType.LPStr)] string transportName);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_setTransport(
				IntPtr source,
				IntPtr transport);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_setSymbology(
				IntPtr source,
				IntPtr symbology);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_getId(
				IntPtr source,
				ref IntPtr id);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_getMappedId(
				IntPtr source,
				ref IntPtr id);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_getDisplayId(
				IntPtr source,
				ref IntPtr id);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_getQuality(
				IntPtr source,
				ref int quality);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_getState(
				IntPtr source,
				ref int state);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern IntPtr mamaSource_getStateAsString(IntPtr source);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_getParent(
				IntPtr source,
				ref IntPtr parent);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_getSymbolNamespace(
				IntPtr source,
				ref IntPtr symbolNamespace);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_getTransportName(
				IntPtr source,
				ref IntPtr transportName);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_getTransport(
				IntPtr source,
				ref IntPtr transport);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_getSymbology(
				IntPtr source,
				ref IntPtr symbology);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSource_getSubSourceManager(
				IntPtr source,
				ref IntPtr subSourceManager);
		}

		private bool mAttemptedToLoadSubSourceManager;
		private MamaSourceManager mManager;
		private MamaTransport mTransport;
		private int mContainerVersion; // to check whether container was modified during enumeration

		#endregion Implementation details

	}
}
