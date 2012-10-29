/* $Id: MamaIo.cs,v 1.8.12.6 2012/08/24 16:12:01 clintonmcdowell Exp $
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
	/// I/O handler
	/// </summary>
	public class MamaIo : MamaWrapper
	{
        /// <summary>
        /// This delegate allows the native layer to pass IO evnets back to this
        /// object. It is saved in a member variable to prevent it being garbage
        /// collected.
        /// </summary>
        private MamaIoDelegate mIoDelegate;

		/// <summary>
		/// <see cref="M:Wombat.MamaWrapper.#ctor" />
		/// </summary>
		public MamaIo() : base()
		{
            // Create the delegate
            mIoDelegate = new MamaIoDelegate(onIo);
		}

		/// <summary>
		/// <see cref="M:Wombat.MamaWrapper.#ctor(System.IntPtr)" />
		/// </summary>
		internal MamaIo(IntPtr nativeHandle) : base(nativeHandle)
		{
            // Create the delegate
            mIoDelegate = new MamaIoDelegate(onIo);
		}

		/// <summary>
		/// Implements the destruction of the underlying peer object.
		/// <seealso cref="MamaWrapper.DestroyNativePeer"/>
		/// </summary>
		protected override MamaStatus.mamaStatus DestroyNativePeer()
		{
            // Destroy the native object
			int code = NativeMethods.mamaIo_destroy(nativeHandle);

            // Clear the delegate to ensure that it is collected
            mIoDelegate = null;

			return (MamaStatus.mamaStatus)code;
		}

		/// <summary>
		/// Create an IO handler.
		/// </summary>
		/// <remarks>
		/// If the underlying infrastructure does not support the requested mamaIoType,
		/// create throws MamaException(MAMA_STATUS_UNSUPPORTED_IO_TYPE). For example,
		/// RV only supports READ, WRITE, and EXCEPT. LBM supports all types except ERROR
		/// </remarks>
		/// <param name="queue">The event queue for the io events. null specifies the
		/// Mama default queue</param>
		/// <param name="action">The callback to be invoked when an event occurs.</param>
		/// <param name="descriptor">Wait for IO on this descriptor.</param>
		/// <param name="ioType">Wait for occurences of this type.</param>
		/// <param name="closure">The closure that is passed to the callback.</param>
		public void create(
			MamaQueue queue,
			MamaIoCallback action, 
			uint descriptor,
			mamaIoType ioType,
			object closure)
		{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
			if (action == null)
			{
				throw new ArgumentNullException("action");
			}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
			this.callback = action;
			this.closureObject = closure;

			IntPtr queueHandle = queue != null ? queue.NativeHandle : IntPtr.Zero;
			int code = NativeMethods.mamaIo_create(
				ref nativeHandle, 
				queueHandle, 
				descriptor, 
				mIoDelegate,
				(int)ioType,
				IntPtr.Zero);
			CheckResultCode(code);
			
			GC.KeepAlive(queue);
		}

		/// <summary>
		/// <see cref="create(MamaQueue, MamaIoCallback, uint, mamaIoType, System.Object)"/>
		/// </summary>
		public void create(
			MamaQueue queue,
			MamaIoCallback action, 
			uint descriptor,
			mamaIoType ioType)
		{
			create(queue, action, descriptor, ioType, null);
		}

		/// <summary>
		/// CLS compliant version of create
		/// <see cref="create(MamaQueue, MamaIoCallback, uint, mamaIoType, System.Object)"/>
		/// </summary>
		public void create(
			MamaQueue queue,
			MamaIoCallback action, 
			long descriptor,
			mamaIoType ioType,
			object closure)
		{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
			if (descriptor < 0 || descriptor > UInt32.MaxValue)
			{
				throw new ArgumentOutOfRangeException("descriptor", descriptor);
			}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
			create(queue, action, (uint)descriptor, ioType, closure);
		}

		/// <summary>
		/// CLS compliant version of create
		/// <see cref="create(MamaQueue, MamaIoCallback, uint, mamaIoType, System.Object)"/>
		/// </summary>
		public void create(
			MamaQueue queue,
			MamaIoCallback action, 
			long descriptor,
			mamaIoType ioType)
		{
			create(queue, action, descriptor, ioType, null);
		}

		/// <summary>
		/// Get the descriptor.
		/// </summary>
		public uint descriptor
		{
			get
			{
				EnsurePeerCreated();
				uint value = 0;
				int code = NativeMethods.mamaIo_getDescriptor(nativeHandle, ref value);
				CheckResultCode(code);
				return value;
			}
		}

		/// <summary>
		/// CLS compliant version of descriptor
		/// </summary>
		public long descriptorClsCompliant
		{
			get
			{
				return descriptor;
			}
		}

		/// <summary>
		/// Destroy the IO.
		/// A synonym to the <see cref="MamaWrapper.Dispose()">MamaWrapper.Dispose</see> method.
		/// </summary>
		public void destroy()
		{
			Dispose();
		}

		/// <summary>
		/// Return the closure for the IO.
		/// </summary>
		public object closure
		{
			get
			{
				return closureObject;
			}
		}

		#region Implementation details

		// C-like callback used in the interop call
		private delegate void MamaIoDelegate(
			IntPtr io,
			int ioType,
			IntPtr closure);

		// the implementation callback
		private void onIo(IntPtr io, int ioType, IntPtr closure)
		{
			if (callback != null)
			{
				callback.onIo(this, (mamaIoType)ioType);
			}
		}

		// Interop API
		private struct NativeMethods
		{
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaIo_create(
				ref IntPtr result,
				IntPtr queue,
				uint descriptor,
				MamaIoDelegate action, 
				int ioType,
				IntPtr closure);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaIo_getDescriptor(
				IntPtr io,
				ref uint descriptor);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaIo_destroy(IntPtr io);
		}

		private MamaIoCallback callback;
		private object closureObject;

		#endregion Implementation details

	}
}
