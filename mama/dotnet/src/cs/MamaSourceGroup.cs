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
using System.Collections;
using System.Runtime.InteropServices;

namespace Wombat
{
	/// <summary>
    /// A MamaSourceGroup monitors a set of <see cref="MamaSource"/>s that presumably
    /// provide a duplicate set of data. 
	/// </summary>
	/// <remarks>
    /// Each member of the mGroup is given
    /// a priority and the member with the highest priority is given the state
    /// <see cref="mamaSourceState.MAMA_SOURCE_STATE_OK"/> the other members of the
    /// mGroup are given the state <see cref="mamaSourceState.MAMA_SOURCE_STATE_OFF"/>
	/// </remarks>
	public class MamaSourceGroup : MamaWrapper, IEnumerable
	{
		/// <summary>
		/// Constructs <b>and</b> creates a new source mGroup object.
		/// <seealso cref="M:Wombat.MamaWrapper.#ctor" />
		/// </summary>
		/// <param name="groupName">The string identifier for the source mGroup.</param>
		public MamaSourceGroup(string groupName) : base()
		{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
			if (groupName == null)
			{
				throw new ArgumentNullException("groupName", groupName);
			}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
			int code = NativeMethods.mamaSourceGroup_create(ref nativeHandle, groupName);
			CheckResultCode(code);
		}

		/// <summary>
		/// <see cref="M:Wombat.MamaWrapper.#ctor(System.IntPtr)" />
		/// </summary>
		internal MamaSourceGroup(IntPtr nativeHandle) : base(nativeHandle)
		{
		}

		/// <summary>
		/// Implements the destruction of the underlying peer object.
		/// <seealso cref="MamaWrapper.DestroyNativePeer" />
		/// </summary>
		protected override MamaStatus.mamaStatus DestroyNativePeer()
		{
			int code = NativeMethods.mamaSourceGroup_destroy(nativeHandle);
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
		/// Return the name string identifier for the specified source mGroup.
		/// </summary>
		public string name
		{
			get
			{
				EnsurePeerCreated();
				IntPtr value = IntPtr.Zero;
				int code = NativeMethods.mamaSourceGroup_getName(nativeHandle, ref value);
				CheckResultCode(code);
				return Marshal.PtrToStringAnsi(value);
			}
		}

		/// <summary>
		/// Find a source with the given name in the mGroup.
		/// </summary>
		/// <param name="sourceName">The name identifier for the source being located.</param>
		/// <returns>The source if found, null otherwise</returns>
		public MamaSource find(string sourceName)
		{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
			if (sourceName == null)
			{
				throw new ArgumentNullException("sourceName", sourceName);
			}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
			EnsurePeerCreated();
			IntPtr value = IntPtr.Zero;
			int code = NativeMethods.mamaSourceGroup_findSource(nativeHandle, sourceName, ref value);
			CheckResultCode(code);
			if (value != IntPtr.Zero)
			{
				return new MamaSource(value);
			}
			else
			{
				return null;
			}
		}

		/// <summary>
		/// Add a mamaSource to the specified mGroup with the specified weighting.
		/// </summary>
		/// <remarks>The source id will be used by the mGroup to uniquely identify the source.</remarks>
		/// <param name="source">The mamaSource being added to the mamaSourceGroup.</param>
		/// <param name="weight">The weighing to apply to the source being added.</param>
		public void add(
			MamaSource source, 
			uint weight)
		{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
			if (source == null)
			{
				throw new ArgumentNullException("source", source);
			}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
			EnsurePeerCreated();
			int code = NativeMethods.mamaSourceGroup_addSource(nativeHandle, source.NativeHandle, weight);
			CheckResultCode(code);
			GC.KeepAlive(source);
		}

		/// <summary>
		/// CLS compliant version of add(MamaSource, UInt32)
		/// </summary>
		public void add(
			MamaSource source, 
			int weight)
		{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
			if (weight < 0)
			{
				throw new ArgumentOutOfRangeException("weight", weight);
			}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
			add(source, (uint)weight);
		}

		/// <summary>
		/// Add a mamaSource to the specified mGroup with the specified weighting and string name identifier.
		/// </summary>
		/// <param name="sourceName">The unique identifier for this source in this mGroup.</param>
		/// <param name="weight">The weighing to apply to the source being added.</param>
		/// <param name="source">The mamaSource being added to the mamaSourceGroup.</param>
		public void add(
			string sourceName,
			uint weight,
			MamaSource source)
		{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
			if (sourceName == null)
			{
				throw new ArgumentNullException("sourceName", sourceName);
			}
			if (source == null)
			{
				throw new ArgumentNullException("source", source);
			}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
			EnsurePeerCreated();
			int code = NativeMethods.mamaSourceGroup_addSourceWithName(nativeHandle, source.NativeHandle, sourceName, weight);
			CheckResultCode(code);
			GC.KeepAlive(source);
		}

		/// <summary>
		/// CLS compliant version of add(System.String, UInt32, MamaSource)
		/// </summary>
		public void add(
			string sourceName,
			int weight,
			MamaSource source)
		{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
			if (weight < 0)
			{
				throw new ArgumentOutOfRangeException("weight", weight);
			}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
			add(sourceName, (uint)weight, source);
		}

		/// <summary>
		/// Set the weight for an existing mamaSource in the specified mGroup.
		/// </summary>
		/// <param name="sourceName">The name of the source whose weight is being updated.</param>
		/// <param name="weight">The new weight value for the specified source.</param>
		public void setWeight(
			string sourceName, 
			uint weight)
		{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
			if (sourceName == null)
			{
				throw new ArgumentNullException("sourceName", sourceName);
			}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
			EnsurePeerCreated();
			int code = NativeMethods.mamaSourceGroup_setSourceWeight(nativeHandle, sourceName, weight);
			CheckResultCode(code);
		}

		/// <summary>
		/// CLS compliant version of setWeight(System.String, UInt32)
		/// </summary>
		public void setWeight(
			string sourceName, 
			int weight)
		{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
			if (weight < 0)
			{
				throw new ArgumentOutOfRangeException("weight", weight);
			}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
			setWeight(sourceName, (uint)weight);
		}

		/// <summary>
		/// Returns the number of sources in this source mGroup
		/// </summary>
		/// <remarks>The size is evaluated every time, so cache it before entering a loop</remarks>
		public uint size
		{
			get
			{
				EnsurePeerCreated();
				uint value = 0;
				foreach (MamaWeightedSource source in this)
				{
					//source;
					++value;
				}
				return value;
			}
		}

		/// <summary>
		/// CLS compliant version of size
		/// </summary>
		public int sizeClsCompliant
		{
			get
			{
				return (int)(long)size;
			}
		}

		/// <summary>
		/// The mCallback interface registered here will receive a mCallback whenever the 
		/// state of sources in the mGroup changes.
		/// </summary>
		/// <param name="mCallback">The mCallback object</param>
		public StateChangedRegistration registerStateChangeCallback(MamaSourceStateChangeCallback mCallback)
		{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
			if (mCallback == null)
			{
				throw new ArgumentNullException("mCallback");
			}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
			EnsurePeerCreated();
			IntPtr cookieKey = new IntPtr(mNextKey);
			IntPtr regCookie = IntPtr.Zero;
			int code = NativeMethods.mamaSourceGroup_registerStageChangeCallback(
				nativeHandle,
				new MamaSourceGroupOnStateChangeDelegate(onStateChange),
				ref regCookie,
				cookieKey);
			CheckResultCode(code);
			StateChangedRegistration reg = new Cookie(this, mCallback, regCookie, cookieKey);
			GC.KeepAlive(mCallback);
			mRegistrations[mNextKey] = reg;
			++mNextKey;
			return reg;
		}

		/// <summary>
		/// Re-evaluate the mGroup by checking all of the relative weights
		/// and changing the state of each MamaSource in the mGroup as
		/// appropriate.
		/// </summary>
		/// <returns>Returns true if any states were changed; otherwise false.</returns>
		public bool reevaluate()
		{
			EnsurePeerCreated();
			byte changedState = 0;
			int code = NativeMethods.mamaSourceGroup_reevaluate(nativeHandle, ref changedState);
			CheckResultCode(code);
			return changedState != 0;
		}

		/// <summary>
		/// Return the top weighted source for this source mGroup.
		/// </summary>
		public MamaSource topWeightSource
		{
			get
			{
				EnsurePeerCreated();
				IntPtr mHandle = IntPtr.Zero;
				int code = NativeMethods.mamaSourceGroup_getTopWeightSource(nativeHandle, ref mHandle);
				CheckResultCode(code);
				return new MamaSource(mHandle);
			}
		}

		/// <summary>
		/// Encapsulates a registration of a state-change mCallback for a particular source mGroup
		/// </summary>
		public interface StateChangedRegistration
		{
			/// <summary>
			/// Remove interest in callbacks when the state of sources within a mGroup change.
			/// </summary>
			void unregister();
			/// <summary>
			/// Returns the source mGroup which the mCallback object is observing.
			/// </summary>
			MamaSourceGroup SourceGroup { get; }
			/// <summary>
			/// Returns the observing mCallback
			/// </summary>
			MamaSourceStateChangeCallback SourceStateChangeCallback { get; }
		}

		/// <summary>
		/// Provides an enumerator over the sources in the source mGroup
		/// </summary>
		/// <returns></returns>
		public IEnumerator GetEnumerator()
		{
			return new Enumerator(this);
		}

		#region Implementation details

		private sealed class Enumerator : IEnumerator
		{
			public Enumerator(MamaSourceGroup that)
			{
				mMamaSourceGroup = that;
				mItems = new ArrayList();
                mGroupIteratorDelegate = new MamaSourceGroupIterateDelegate(this.onIterate);

				int code = NativeMethods.mamaSourceGroup_iterateSources(
					mMamaSourceGroup.NativeHandle,
                    mGroupIteratorDelegate,
					IntPtr.Zero);
				MamaWrapper.CheckResultCode(code);
				mIndex = 0;
				GC.KeepAlive(this);
			}
			public void Reset()
			{
				mIndex = 0;
			}
			public object Current
			{
				get
				{
					if (mIndex >= mItems.Count)
					{
						throw new InvalidOperationException();
					}
					return mItems[mIndex];
				}
			}
			public bool MoveNext()
			{
				if (mIndex < mItems.Count - 1)
				{
					++mIndex;
					return true;
				}
				return false;
			}

			// the mCallback implementation
			private void onIterate(IntPtr sourceGroup, IntPtr source, uint weight, IntPtr closure)
			{
				MamaSource ms = new MamaSource(source);
				MamaWeightedSource ws = new MamaWeightedSource(ms, weight);
				mItems.Add(ws);
				GC.KeepAlive(ms);
				GC.KeepAlive(ws);
			}

			private MamaSourceGroup mMamaSourceGroup;
			private ArrayList mItems;
			private int mIndex;
            private MamaSourceGroupIterateDelegate mGroupIteratorDelegate;
		}

		// registration cookie wrapper
		private sealed class Cookie : StateChangedRegistration
		{
			public Cookie(
				MamaSourceGroup mGroup, 
				MamaSourceStateChangeCallback mCallback, 
				IntPtr registrationHandle,
				IntPtr mKey)
			{
				this.mGroup = mGroup;
				this.mCallback = mCallback;
				this.mHandle = registrationHandle;
				this.mKey = mKey;
			}

			public void unregister()
			{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
				if (mHandle == IntPtr.Zero)
				{
					throw new InvalidOperationException("Already unregistered");
				}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
				if (mHandle != IntPtr.Zero)
				{
					mGroup.EnsurePeerCreated();
					int code = NativeMethods.mamaSourceGroup_unregisterStateChangeCallback(mGroup.NativeHandle, mHandle);
					MamaStatus.mamaStatus status = (MamaStatus.mamaStatus)code;
					if (status != MamaStatus.mamaStatus.MAMA_STATUS_OK)
					{
						throw new MamaException(status);
					}
					mGroup.mRegistrations.Remove(mKey.ToInt32());
					mHandle = IntPtr.Zero;
					mKey = IntPtr.Zero;
				}
			}

			public MamaSourceGroup SourceGroup
			{
				get
				{
					return mGroup;
				}
			}

			public MamaSourceStateChangeCallback SourceStateChangeCallback
			{
				get
				{
					return mCallback;
				}
			}

			public int Key
			{
				get
				{
					return mKey.ToInt32();
				}
			}

			private MamaSourceGroup mGroup;
			private MamaSourceStateChangeCallback mCallback;
			private IntPtr mHandle;
			private IntPtr mKey;
		}

		// C-like callbacks used in the interop calls

		private delegate void MamaSourceGroupIterateDelegate(
			IntPtr sourceGroup,
			IntPtr source,
			uint weight,
			IntPtr closure);

		private delegate void MamaSourceGroupOnStateChangeDelegate(
			IntPtr sourceGroup,
			IntPtr topWeightSource,
			IntPtr closure);

		private void onStateChange(IntPtr sourceGroup, IntPtr topWeightSource, IntPtr closure)
		{
			int mKey = closure.ToInt32();
			Cookie cookie = mRegistrations[mKey] as Cookie;
			if (cookie != null && cookie.Key != 0)
			{
				MamaSourceStateChangeCallback mCallback = cookie.SourceStateChangeCallback;
				if (mCallback != null)
				{
					mCallback.onStateChanged(this, new MamaSource(topWeightSource));
				}
			}
		}

		// Interop API
		private struct NativeMethods
		{
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSourceGroup_create(
				ref IntPtr sourceGroup,
				[MarshalAs(UnmanagedType.LPStr)] string groupName);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSourceGroup_getName(
				IntPtr sourceGroup,
				ref IntPtr groupName);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSourceGroup_findSource(
				IntPtr sourceGroup,
				[MarshalAs(UnmanagedType.LPStr)] string sourceName,
				ref IntPtr source);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSourceGroup_addSource(
				IntPtr sourceGroup,
				IntPtr source,
				uint weight);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSourceGroup_addSourceWithName(
				IntPtr sourceGroup,
				IntPtr source,
				[MarshalAs(UnmanagedType.LPStr)] string sourceName,
				uint weight);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSourceGroup_setSourceWeight(
				IntPtr sourceGroup,
				[MarshalAs(UnmanagedType.LPStr)] string sourceName,
				uint weight);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSourceGroup_reevaluate(
				IntPtr sourceGroup,
				ref byte changedStates);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSourceGroup_iterateSources(
				IntPtr sourceGroup,
				MamaSourceGroupIterateDelegate mCallback,
				IntPtr closure);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSourceGroup_getTopWeightSource(
				IntPtr sourceGroup,
				ref IntPtr topWeightSource);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSourceGroup_registerStageChangeCallback(
				IntPtr sourceGroup,
				MamaSourceGroupOnStateChangeDelegate mCallback,
				ref IntPtr registrationCookie,
				IntPtr closure);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSourceGroup_unregisterStateChangeCallback(
				IntPtr sourceGroup,
				IntPtr registrationCookie);

			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaSourceGroup_destroy(IntPtr sourceGroup);
		}

		internal Hashtable mRegistrations = new Hashtable();
		private int mNextKey = 1;

		#endregion Implementation details

	}
}
