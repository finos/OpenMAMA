/* $Id: MamaFtMember.cs,v 1.6.12.5 2012/08/24 16:12:01 clintonmcdowell Exp $
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
	public enum mamaFtState
	{
		MAMA_FT_STATE_STANDBY  = 0,
		MAMA_FT_STATE_ACTIVE   = 1,
		MAMA_FT_STATE_UNKNOWN  = 99
	};

	public enum mamaFtType
	{
		MAMA_FT_TYPE_MULTICAST = 1,
		MAMA_FT_TYPE_BRIDGE	   = 2,
		MAMA_FT_TYPE_MAX	   = 3
	};

    /// <summary>
    /// A member of a MAMA fault tolerance group 
    /// </summary>
	public class MamaFtMember : MamaWrapper
	{
		public MamaFtMember(): base()
		{
		}

		/// <summary>
		/// Create object. This function is
		/// typically followed by <see cref="MamaFtMember.setup"/> and
		/// <see cref="MamaFtMember.activate"/>
		/// </summary>
		public void create()
		{
			int code = NativeMethods.mamaFtMember_create(ref nativeHandle);
			CheckResultCode(code);
		}

		protected void setupFtType(
			mamaFtType ftType,
			MamaQueue queue,
			MamaFtMemberCallback callback,
			MamaTransport transport,
			string groupName,
			uint weight,
			double heartbeatInterval,
			double timeoutInterval,
			object closure)
		{
			EnsurePeerCreated();
			if (callback == null)
			{
				mCallbackForwarder = null;
				mCallback = null;
			}
			else
			{
				mCallbackForwarder = new CallbackForwarder(this, callback, closure);
				mCallback = new CallbackForwarder.FtMemberCallbackDelegate(mCallbackForwarder.OnFtStateChange);
			}
			IntPtr queueHandle = queue != null ? queue.NativeHandle : IntPtr.Zero;
			IntPtr transportHandle = transport != null ? transport.NativeHandle : IntPtr.Zero;
			int code = NativeMethods.mamaFtMember_setup(
				nativeHandle,
				mamaFtType.MAMA_FT_TYPE_MULTICAST,
				queueHandle,
				mCallback,
				transportHandle,
				groupName,
				weight,
				heartbeatInterval,
				timeoutInterval,
				null);
			CheckResultCode(code);
		}

		public void setup(
			MamaQueue queue,
			MamaFtMemberCallback callback,
			MamaTransport transport,
			string groupName,
			uint weight,
			double heartbeatInterval,
			double timeoutInterval,
			object closure)
		{
			setupFtType (
				mamaFtType.MAMA_FT_TYPE_MULTICAST,
				queue,
				callback,
				transport,
				groupName,
				weight,
				heartbeatInterval,
				timeoutInterval,
				closure);
		}

		/// <summary>
		/// Activate the MAMA fault tolerance group member.
		/// </summary>
		public void activate()
		{
			int code = NativeMethods.mamaFtMember_activate(nativeHandle);
			CheckResultCode(code);
		}

		/// <summary>
		/// Deactivate the MAMA fault tolerance group member.
		/// </summary>
		public void deactivate()
		{
			int code = NativeMethods.mamaFtMember_deactivate(nativeHandle);
			CheckResultCode(code);
		}

		/// <summary>
		/// Get whether the MAMA fault tolerance member is actively running
		/// (not related to its state).  Use mamaFtMember.getState() to
		/// determine the actual state of the member.
		/// </summary>
		public bool isActive()
		{
			int result = 0;
			int code = NativeMethods.mamaFtMember_isActive(nativeHandle, ref result);
			CheckResultCode(code);
			return result != 0;
		}

		/// <summary>
		/// Get the group name to which this MAMA FT member belongs.
		/// </summary>
		public string getGroupName()
		{
			IntPtr result = IntPtr.Zero;
			int code = NativeMethods.mamaFtMember_getGroupName(nativeHandle, ref result);
			CheckResultCode(code);
			return Marshal.PtrToStringAnsi(result);
		}

		/// <summary>
		/// Get the fault tolerance weight of the MAMA FT member.
		/// </summary>
		public uint getWeight()
		{
			uint result = 0;
			int code = NativeMethods.mamaFtMember_getWeight(nativeHandle, ref result);
			CheckResultCode(code);
			return result;
		}

		/// <summary>
		/// Get the fault tolerance heartbeat interval of the MAMA FT member.
		/// </summary>
		public double getHeartbeatInterval()
		{
			double result = 0.0;
			int code = NativeMethods.mamaFtMember_getHeartbeatInterval(nativeHandle, ref result);
			CheckResultCode(code);
			return result;
		}

		/// <summary>
		/// Get the fault tolerance timeout interval of the MAMA FT member.
		/// </summary>
		public double getTimeoutInterval()
		{
			double result = 0.0;
			int code = NativeMethods.mamaFtMember_getTimeoutInterval(nativeHandle, ref result);
			CheckResultCode(code);
			return result;
		}

		/// <summary>
		/// Get the closure argument (provided in the mamaFtMember.create()
		/// function) of the MAMA FT member.
		/// </summary>
		public object getClosure()
		{
			return mCallbackForwarder.GetClosure();
		}

		/// <summary>
		/// Set the fault tolerance weight of the MAMA FT member. The FT
		/// weight can be changed dynamically, if desired. The member with the
		/// highest weight will become the active member.
		/// </summary>
		public void setWeight(uint value)
		{
			int code = NativeMethods.mamaFtMember_setWeight(nativeHandle, value);
			CheckResultCode(code);
		}

		/// <summary>
		/// Set the instance ID of the MAMA FT member
		/// </summary>
		/// <remarks>
        /// The instance ID is used
		/// to uniquely identify members of a fault tolerant group.  Most
		/// applications should allow the MAMA API to automatically set the
		/// instance ID and this function would not be called. If not set
		/// explicitly before activation, then the instance ID is automatically
		/// set according to the following format:
        /// <para>
		///    {group-name}.{hex-ip-addr}.{hex-pid}</para>
        /// <para>
		/// For example, if the group name is "FOO", the IP address is
		/// 192.168.187.9, and the PId is 10777, the default instance ID 
        /// would be:</para>
		///    FOO.c0a8bb9.2a19
		/// </remarks>
		public void setInstanceId(string id)
		{
			int code = NativeMethods.mamaFtMember_setInstanceId(nativeHandle, id);
			CheckResultCode(code);
		}

		/// <summary>
		/// Convert a fault tolerant state to a string
		/// </summary>
		public static string mamaFtStateToString(mamaFtState state)
		{
			return Marshal.PtrToStringAnsi(NativeMethods.mamaFtStateToString(state));
		}

		/// <summary>
		/// Implements the destruction of the underlying peer object
		/// <seealso cref="MamaWrapper.DestroyNativePeer"/>
		/// </summary>
		protected override MamaStatus.mamaStatus DestroyNativePeer()
		{
			return (MamaStatus.mamaStatus)NativeMethods.mamaFtMember_destroy(nativeHandle);
		}

		#region Implementation details

		// Interop bridge
		private sealed class CallbackForwarder
		{
			public delegate void FtMemberCallbackDelegate(
				IntPtr ftMember,
				string groupName,
				mamaFtState state,
				IntPtr closure);

			internal void OnFtStateChange(
				IntPtr ftMember,
				string groupName,
				mamaFtState state,
				IntPtr closure)
			{
				if (mCallback != null)
				{
					mCallback.onFtStateChange(mTarget, groupName, state, mClosure);
				}
			}

			public CallbackForwarder(MamaFtMember target, MamaFtMemberCallback callback, object closure)
			{
				mTarget = target;
				mCallback = callback;
				mClosure = closure;
			}

			public object GetClosure()
			{
				return mClosure;
			}

			private MamaFtMember mTarget;
			private MamaFtMemberCallback mCallback;
			private object mClosure;
		}

		private struct NativeMethods
		{
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaFtMember_create(
				ref IntPtr  member);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaFtMember_setup(
				IntPtr member,
				mamaFtType fttype,
				IntPtr eventQueue,
				CallbackForwarder.FtMemberCallbackDelegate callback,
				IntPtr transport,
				string groupName,
				uint weight,
				double heartbeatInterval,
				double timeoutInterval,
				object closure);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaFtMember_destroy(
				IntPtr member);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaFtMember_activate(
				IntPtr member);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaFtMember_deactivate(
				IntPtr member);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaFtMember_isActive(
				IntPtr  member,
			    ref int result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaFtMember_getGroupName(
				IntPtr member,
				ref IntPtr result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaFtMember_getWeight(
				IntPtr member,
				ref uint result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaFtMember_getHeartbeatInterval(
				IntPtr member,
				ref double result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaFtMember_getTimeoutInterval(
				IntPtr member,
				ref double result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaFtMember_setWeight(
				IntPtr member,
				uint value);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaFtMember_setInstanceId(
				IntPtr member,
				string id);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern IntPtr mamaFtStateToString (mamaFtState state);
		}

		private CallbackForwarder mCallbackForwarder;
		private CallbackForwarder.FtMemberCallbackDelegate mCallback;

		#endregion // Implementation details
	}

    /// <summary>
    /// A member of a MAMA fault tolerance multicast group
	/// </summary>
	public class MamaMulticastFtMember : MamaFtMember
	{
		/// <summary>
		/// Set up the object. This is only an
		/// initialization function. In order to actually start the fault
		/// tolerance monitoring, use <see cref="MamaFtMember.activate"/>
		/// </summary>
		public new void setup(
			MamaQueue queue,
			MamaFtMemberCallback callback,
			MamaTransport transport,
			string groupName,
			uint weight,
			double heartbeatInterval,
			double timeoutInterval,
			object closure)
		{
			setupFtType (
				mamaFtType.MAMA_FT_TYPE_MULTICAST,
				queue,
				callback,
				transport,
				groupName,
				weight,
				heartbeatInterval,
				timeoutInterval,
				closure);
		}
	}

    /// <summary>
    /// A member of a MAMA fault tolerance bridge group
	/// </summary>
	public class MamaBridgeFtMember : MamaFtMember
	{
		/// <summary>
		/// Set up the object. This is only an
		/// initialization function. In order to actually start the fault
		/// tolerance monitoring, use <see cref="MamaFtMember.activate"/>
		/// </summary>
		public new void setup(
			MamaQueue queue,
			MamaFtMemberCallback callback,
			MamaTransport transport,
			string groupName,
			uint weight,
			double heartbeatInterval,
			double timeoutInterval,
			object closure)
		{
			setupFtType (
				mamaFtType.MAMA_FT_TYPE_BRIDGE,
				queue,
				callback,
				transport,
				groupName,
				weight,
				heartbeatInterval,
				timeoutInterval,
				closure);
		}
	}
}
