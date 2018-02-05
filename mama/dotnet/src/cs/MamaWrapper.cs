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
	/// Base class for all wrapped native objects. Provides common facilities to
	/// the base classes.
	/// </summary>
	public abstract class MamaWrapper : IDisposable
	{
		/// <summary>
		/// Constructs a new wrapper object but doesn't create it.
		/// If the object is not explicitly disposed it will remain registered
		/// for finalization and will attempt the destroy the native peer object
		/// when finalized.
		/// </summary>
		protected MamaWrapper()
		{
			this.nativeHandle = IntPtr.Zero;
			this.mOwnsThePeer = true;
		}

		/// <summary>
		/// Constructs a managed wrapper from an existing native peer.
		/// The peer object will not be automatically finalized
		/// </summary>
		/// <param name="nativeHandle">The opaque native handle to the peer object</param>
		protected MamaWrapper(IntPtr nativeHandle)
		{
			this.nativeHandle = nativeHandle;
			this.mOwnsThePeer = false;
			GC.SuppressFinalize(this);
		}

		/// <summary>
		/// Finalizes the managed wrapper
		/// </summary>
		~MamaWrapper()
		{
			Dispose(false);
		}

		/// <summary>
		/// Destroys the underlying peer object and unbinds the wrapper from it.
		/// </summary>
		public void Dispose()
		{
			Dispose(true);
		}

		/// <summary>
		/// The actual implementation of Dispose - called by both the Dispose method and the finalizer.
		/// </summary>
		/// <param name="disposing">true if the object is being disposed (false if being finalized)</param>
		protected virtual void Dispose(bool disposing, bool destroyNativeHandle = true)
		{
            MamaStatus.mamaStatus status = MamaStatus.mamaStatus.MAMA_STATUS_OK;
			if (disposing)
			{
                OnDispose();
                if (mOwnsThePeer)
				// prevent the object from being finalized later
                GC.SuppressFinalize(this);
			}


			if (!mOwnsThePeer)
			{
				return;
			}
			if (nativeHandle != IntPtr.Zero)
			{
				try
				{

                    if (Mama.opened)
                    {
                       status = DestroyNativePeer();
                    }
					if (disposing)
					{
						// only throw exception if the object is not being finalized
						if (status != MamaStatus.mamaStatus.MAMA_STATUS_OK)
						{
							throw new MamaException(status);
						}
					}
				}
				finally
				{
                    if (destroyNativeHandle)
                    {
                        nativeHandle = IntPtr.Zero;
                    }
				}
			}
		}

		/// <summary>
		/// Returns the native handle to the native peer object.
		/// Should only be used by subclasses and in the same assembly.
		/// </summary>
		protected internal IntPtr NativeHandle
		{
			get
			{
				return nativeHandle;
			}

            set
            {
                nativeHandle = value;
            }
		}

		/// <summary>
		/// Sets the native handle of the native peer object.
		/// Should only be used by subclasses and in the same assembly.
		/// </summary>
		public void setNativeHandle(IntPtr nHandle)
		{
			nativeHandle = nHandle;
		}


		/// <summary>
		/// In some situations, wrappers are created elsewhere, but need to automatically
		/// destroy themselves, otherwise would be leaked; as a general rule, all .NET
		/// wrapper objects that were created from a native handle are not supposed to manage its
		/// own life time and will not be destroyed - this is an exception that needs to be handled
		/// (example - MamaMsg uses this method)
		/// </summary>
		protected internal void SelfManageLifeTime(bool value)
		{
            if (mOwnsThePeer  != value)
            {
                if (mOwnsThePeer)
                    GC.SuppressFinalize(this);
                else
                    GC.ReRegisterForFinalize(this);
            }

            mOwnsThePeer  = value;
		}

		/// <summary>
		/// Subclasses must do whatever is necessary with the native handle in the
		/// overridden method in order to destroy the native peer object. If the
		/// destruction went successfully, the overridden method must return
		/// MamaStatus.mamaStatus.MAMA_STATUS_OK, or a suitable error code otherwise
		/// </summary>
		/// <returns>MamaStatus.mamaStatus.MAMA_STATUS_OK on success, one of the
		/// MamaStatus.mamaStatus.MAMA_STATUS_OK error codes otherwise</returns>
		protected abstract MamaStatus.mamaStatus DestroyNativePeer();

		/// <summary>
		/// </summary>
		protected virtual void OnDispose()
		{
		}

		/// <summary>
		/// Throws an exception if the native object has not been created.
		/// </summary>
        /// <exception cref="InvalidOperationException">
        /// Thrown if the native object has not been created.
        /// </exception>
		protected internal void EnsurePeerCreated()
		{
			if (nativeHandle == IntPtr.Zero)
			{
				throw new InvalidOperationException("Peer native object not created");
			}
		}

		/// <summary>
		/// Helper method for the sub classes
		/// </summary>
		/// <param name="code">the MamaStatus.mamaStatus returned from the native call (as integer)</param>
		protected internal static void CheckResultCode(int code)
		{
			if ( (MamaStatus.mamaStatus)code != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				throw new MamaException((MamaStatus.mamaStatus)code);
			}
		}

		/// <summary>
		/// Helper method for the sub classes which will not throw exception for not found
		/// </summary>
		/// <param name="code">the MamaStatus.mamaStatus returned from the native call (as integer)</param>
		protected internal static MamaStatus.mamaStatus CheckResultCodeIgnoreNotFound(int code)
		{
			if ((MamaStatus.mamaStatus)code != MamaStatus.mamaStatus.MAMA_STATUS_NOT_FOUND &&
				(MamaStatus.mamaStatus)code != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				throw new MamaException((MamaStatus.mamaStatus)code);
			}
			return (MamaStatus.mamaStatus)code;
		}

		/// <summary>
		/// the handle to the native peer object
		/// </summary>
		protected IntPtr nativeHandle;
		private bool mOwnsThePeer;
	}
}
