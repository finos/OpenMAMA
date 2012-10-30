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
    /// Maps field identifiers (FIDs) to human-readable strings
	/// </summary>
    /// <remarks>
    /// Any incoming MamaMsg might contain FIDs but
    /// no field names. The dictionary allows applications to determine the name
    /// associated with a given FID. 
    /// </remarks>
	public class MamaDictionary : MamaWrapper
	{

	    /// <summary>
	    /// Default constructor
	    /// </summary>
		public MamaDictionary () : base()
		{
		}

	  /// <summary>
	  /// Create a data dictionary from a subscription
	  /// </summary>
		public void create(
			MamaQueue queue,
			MamaDictionaryCallback callback,
			MamaSource source,
			int retries,
			double timeout) 
		{
			mCallbackForwarder = new CallbackForwarder(this, callback);

			IntPtr queuePtr = queue != null ? queue.NativeHandle : IntPtr.Zero;
			IntPtr sourcePtr = source != null ? source.NativeHandle : IntPtr.Zero;

			mCallbacks = mCallbackForwarder.getMsgCallbacks();

			int code = NativeMethods.mama_createDictionary(ref nativeHandle, queuePtr, mCallbacks, sourcePtr, timeout, retries, nativeHandle);
			CheckResultCode(code);

			GC.KeepAlive(source);
			GC.KeepAlive(queue);
		}

        public void create(string filename) 
		{
			int code = NativeMethods.mamaDictionary_create(ref nativeHandle);
			CheckResultCode(code);
            code = NativeMethods.mamaDictionary_populateFromFile(nativeHandle, filename);
			CheckResultCode(code);
		}
        
        public void writeToFile(string filename)
        {
            int code = NativeMethods.mamaDictionary_writeToFile(nativeHandle, filename);
			CheckResultCode(code);
        }

        
		/// <summary>
		/// Implements the destruction of the underlying peer object
		/// </summary>
		protected override MamaStatus.mamaStatus DestroyNativePeer()
		{
			return (MamaStatus.mamaStatus)NativeMethods.mamaDictionary_destroy(nativeHandle);
		}
        
        public string getFeedName()
		{
            EnsurePeerCreated();
            string result = null;
			int code = NativeMethods.mamaDictionary_getFeedName(nativeHandle, ref result);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return null;
			}
			return result;
		}
        
        public string getFeedHost()
		{
            EnsurePeerCreated();
            string result = null;
			int code = NativeMethods.mamaDictionary_getFeedHost(nativeHandle, ref result);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return null;
			}
			return result;
		}

        /// <summary>
        /// Return the descriptor of the field with the specified name.
        /// </summary>
        /// <remarks>If there  is more than one field with the same name, 
        /// the one with the lowest field id is returned.
        /// </remarks>
		public MamaFieldDescriptor getFieldByName(string fieldName)
		{
			EnsurePeerCreated();
			IntPtr fieldHandle = IntPtr.Zero;
			int code = NativeMethods.mamaDictionary_getFieldDescriptorByName(
				nativeHandle,
				ref fieldHandle,
				fieldName);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return null;
			}
			return new MamaFieldDescriptor(fieldHandle);
		}

		/// <summary>
		/// Returns the highest field identifier.
		/// </summary>
		public int getMaxFid()
		{
			// NOTE: should return ushort, but it's not CLS-compliant and this is public API
			EnsurePeerCreated();
			ushort value = 0;
			int code = NativeMethods.mamaDictionary_getMaxFid(nativeHandle, ref value);
			CheckResultCode(code);
			return value;
		}
		
		/// <summary>
		/// Returns the the number of entries in the dictionary.
		/// </summary>
		public int getSize()
		{			
			EnsurePeerCreated();
			int value = 0;
			int code = NativeMethods.mamaDictionary_getSize(nativeHandle, ref value); 
			CheckResultCode(code);
			return value;
		}
		
		/// <summary> 
		/// Return the field with the specified field FID. This method is
        /// very efficient.
		/// </summary>
		public MamaFieldDescriptor getFieldByFid(int fid)
		{
			EnsurePeerCreated();
			IntPtr fieldHandle = IntPtr.Zero;
			int code = NativeMethods.mamaDictionary_getFieldDescriptorByFid(
				nativeHandle,
				ref fieldHandle,
				fid);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return null;
			}
			return new MamaFieldDescriptor(fieldHandle);
		}
		
		/// <summary> 
		/// Return the field with the corresponding zero based index. This
        /// method is O (N) with respect to the size of the dictionary.
		/// </summary>
		public MamaFieldDescriptor getFieldByIndex(int index)
		{
			EnsurePeerCreated();
			IntPtr fieldHandle = IntPtr.Zero;
            int code = NativeMethods.mamaDictionary_getFieldDescriptorByIndex(
                nativeHandle,
                ref fieldHandle,
                index);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return null;
			}
			return new MamaFieldDescriptor(fieldHandle);
		}
		
		/// <summary> 
		/// Recreate a data dictionary from the MamaMsg supplied.
		/// The MamaMsg is copied and can therefore be deleted after
		/// the method has returned.
		/// </summary>
		public void buildDictionaryFromMessage(MamaMsg msg)
		{
            int code = NativeMethods.mamaDictionary_create(ref nativeHandle);
            CheckResultCode(code);
            code = NativeMethods.mamaDictionary_buildDictionaryFromMessage(nativeHandle, msg.NativeHandle);
            CheckResultCode(code);
		}

        /// <summary> 
        /// Returns a MamaMsg representing the data dictionary. This message can be
        /// published or used to create a new MamaDictionary object.
        /// A new MamaMsg is created for each invocation of the method. It is the
        /// responsibility of the caller to delete the message when no longer
        /// needed.
        /// </summary>
        public MamaMsg getDictionaryMessage()		
        {
            EnsurePeerCreated();
			MamaMsg myDictMsg;
			IntPtr mamaMsg  = IntPtr.Zero;
            int code = NativeMethods.mamaDictionary_getDictionaryMessage (nativeHandle, ref mamaMsg);
			CheckResultCode(code);
			myDictMsg = new MamaMsg (mamaMsg);
			return myDictMsg;
        }
		#region Implementation details

		private struct NativeMethods
		{
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mama_createDictionary (ref IntPtr dictionary,
				IntPtr queue,
				Callbacks  callbacks,
				IntPtr source,
				double timeout,
				int retries,
				IntPtr closure);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaDictionary_create (ref IntPtr dictionary);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaDictionary_destroy (IntPtr dictionary);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaDictionary_getFeedName (
                IntPtr dictionary,
                [MarshalAs(UnmanagedType.LPStr)] ref string feedName);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaDictionary_getFeedHost (
                IntPtr dictionary,
                [MarshalAs(UnmanagedType.LPStr)] ref string feedHost);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaDictionary_getFieldDescriptorByName(
				IntPtr dictionary, 
				ref IntPtr result,
				[MarshalAs(UnmanagedType.LPStr)] string fname);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaDictionary_getMaxFid(
				IntPtr dictionary,
				ref ushort value);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaDictionary_writeToFile (IntPtr dictionary, string fileName);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaDictionary_populateFromFile (IntPtr dictionary, string fileName);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaDictionary_getSize (IntPtr dictionary, ref int value);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaDictionary_getFieldDescriptorByFid(IntPtr dictionary, ref IntPtr result, int fid);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaDictionary_getFieldDescriptorByIndex(IntPtr dictionary, ref IntPtr result, int index);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaDictionary_buildDictionaryFromMessage(IntPtr dictionary, IntPtr mamaMsg);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaDictionary_getDictionaryMessage (IntPtr dictionary, ref IntPtr mamamsg);
		}

		private delegate void CompleteDelegate(IntPtr dictionary, IntPtr closure);
		private delegate void TimeoutDelegate(IntPtr dictionary, IntPtr closure);
		private delegate void ErrorDelegate(IntPtr dictionary, string message, IntPtr closure);

		[StructLayout(LayoutKind.Sequential)]
		private struct Callbacks 
		{
			public CompleteDelegate onComplete;
			public TimeoutDelegate onTimeout;
			public ErrorDelegate onError;
		}  

		private class CallbackForwarder
		{
			private MamaDictionary mSender;
			private MamaDictionaryCallback mCallback;

			public CallbackForwarder(
				MamaDictionary sender,
				MamaDictionaryCallback callback)
			{
				this.mSender = sender;
				this.mCallback = callback;
			}

			public Callbacks getMsgCallbacks()
			{
				Callbacks callbacks = new Callbacks();
         
				callbacks.onComplete = new CompleteDelegate(this.OnComplete);
				callbacks.onTimeout = new TimeoutDelegate(this.OnTimeout);
				callbacks.onError = new ErrorDelegate(this.OnError);
			
				return callbacks;
			}

			private void OnComplete(IntPtr dictionary, IntPtr closure)
			{
                if (mCallback != null) 
				{
                    mCallback.onComplete(mSender);
				}
			}

			private void OnTimeout(IntPtr dictionary, IntPtr closure)
			{
                if (mCallback != null) 
				{
                    mCallback.onTimeout(mSender);
				}
			}

			private void OnError(IntPtr dictionary, string message, IntPtr closure)
			{
                if (mCallback != null) 
				{
                    mCallback.onError(mSender, message);
				}
			}
		}

		// state
		private CallbackForwarder mCallbackForwarder;
		private Callbacks mCallbacks;

		#endregion // Implementation details

	}
}
