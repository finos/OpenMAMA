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
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;

namespace Wombat
{
    /// <summary>
    /// Definition of payload types.
    /// </summary>
    [Obsolete ("mamaPayloadType enum has been deprecated.")]
    public enum mamaPayloadType
    {
        /// <summary>
        /// Solace Message
        /// </summary>
        MAMA_PAYLOAD_SOLACE = '1',

        /// <summary>
        /// V5 Message
        /// </summary>
        MAMA_PAYLOAD_V5 = '5',

        /// <summary>
        /// AVIS Message
        /// </summary>
        MAMA_PAYLOAD_AVIS = 'A',

        /// <summary>
        /// Tick42 BLP Message
        /// </summary>
        MAMA_PAYLOAD_TICK42BLP = 'B',

        /// <summary>
        /// FAST Message
        /// </summary>
        MAMA_PAYLOAD_FAST = 'F',

        /// <summary>
        /// RAI Message
        /// </summary>
        MAMA_PAYLOAD_RAI = 'I',

        /// <summary>
        /// KWANTUM Message
        /// </summary>
        MAMA_PAYLOAD_KWANTUM = 'K',

        /// <summary>
        /// UMS Message
        /// </summary>
        MAMA_PAYLOAD_UMS = 'L',

        /// <summary>
        /// Inrush Message
        /// </summary>
        MAMA_PAYLOAD_INRUSH = 'N',

        /// <summary>
        /// Tick42 RMDS Message
        /// </summary>
        MAMA_PAYLOAD_TICK42RMDS = 'P',

        /// <summary>
        /// QPID Message
        /// </summary>
        MAMA_PAYLOAD_QPID = 'Q',

        /// <summary>
        /// TIBRV Message
        /// </summary>
        MAMA_PAYLOAD_TIBRV = 'R',

        /// <summary>
        /// IBMWFO Message
        /// </summary>
        MAMA_PAYLOAD_IBMWFO = 'S',

        /// <summary>
        /// ACTIV Message
        /// </summary>
        MAMA_PAYLOAD_ACTIV = 'T',

        /// <summary>
        /// Vulcan Message
        /// </summary>
        MAMA_PAYLOAD_VULCAN = 'V',

        /// <summary>
        /// Wombat Message
        /// </summary>
        MAMA_PAYLOAD_WOMBAT_MSG = 'W',

        /// <summary>
        /// Exegy Message
        /// </summary>
        MAMA_PAYLOAD_EXEGY = 'X',

        /// <summary>
        /// Unknown Type
        /// </summary>
        MAMA_PAYLOAD_UNKNOWN = 'U'
    }

    /// <summary>
    /// Enum representing the message type
    /// </summary>
    public enum mamaMsgType
    {
        /// <summary>
        /// General update (includes, funds). For direct feeds, the handler sends
        /// more specific message types for stocks and order books; however,
        /// aggregated feeds will send MAMA_MSG_TYPE_UPDATE for stocks and order
        /// books as well.
        /// </summary>
        MAMA_MSG_TYPE_UPDATE            =   0,

        /// <summary>
        /// Initial value. The initial image (full record) for normal subscriptions.
        /// The initial
        /// value is the first message (several messages for multi-part initial
        /// values) to arrive with all of the information in the cache. Multi-part
        /// initial values only occur on Mama for TIBRV. If the requiresInitial
        /// parameter to createSubscription is 0, no initial value is sent.
        /// </summary>
        MAMA_MSG_TYPE_INITIAL           =   1,

        /// <summary>
        /// Trade cancel
        /// </summary>
        MAMA_MSG_TYPE_CANCEL            =   2,

        /// <summary>
        /// An error occurred
        /// </summary>
        MAMA_MSG_TYPE_ERROR             =   3,

        /// <summary>
        /// Trade correction
        /// </summary>
        MAMA_MSG_TYPE_CORRECTION        =   4,

        /// <summary>
        /// Closing summary
        /// </summary>
        MAMA_MSG_TYPE_CLOSING           =   5,

        /// <summary>
        /// Refresh/recap of some/all fields. When the client detects a sequence
        /// number gap, it requests a recap from the feed handler. The feed handler
        /// may also send recaps in the event of a correction or other event that
        /// requires publishing the full record.
        /// </summary>
        MAMA_MSG_TYPE_RECAP             =   6,

        /// <summary>
        /// The symbol was deleted from the feed handler cache. The feed handler will
        /// not send any more updates for the symbol.
        /// </summary>
        MAMA_MSG_TYPE_DELETE            =   7,

        /// <summary>
        /// Expired option or future
        /// </summary>
        MAMA_MSG_TYPE_EXPIRE            =   8,

        /// <summary>
        /// A snapshot is the same as an initial value; however, the client will not
        /// receive any subsequent updates. Clients request snapshots by creating
        /// snapshot subscriptions.
        /// </summary>
        MAMA_MSG_TYPE_SNAPSHOT          =   9,

        /// <summary>
        /// Pre-opening summary (e.g. morning "roll")
        /// </summary>
        MAMA_MSG_TYPE_PREOPENING        =   12,

        /// <summary>
        /// Quote updates
        /// </summary>
        MAMA_MSG_TYPE_QUOTE             =   13,

        /// <summary>
        /// Trade updates
        /// </summary>
        MAMA_MSG_TYPE_TRADE             =   14,

        /// <summary>
        /// Order updates
        /// </summary>
        MAMA_MSG_TYPE_ORDER             =   15,

        /// <summary>
        /// Order book initial value. This message is sent rather than
        /// MAMA_MSG_TYPE_INITIAL for order books.
        /// </summary>
        MAMA_MSG_TYPE_BOOK_INITIAL      =   16,

        /// <summary>
        /// Order book update. Sent for order books rather than MAMA_MSG_TYPE_UPDATE
        /// </summary>
        MAMA_MSG_TYPE_BOOK_UPDATE       =   17,

        /// <summary>
        /// Order book clear. All the entries should be removed from the book
        /// </summary>
        MAMA_MSG_TYPE_BOOK_CLEAR        =   18,

        /// <summary>
        /// Order book recap. Sent rather than MAMA_MSG_TYPE_RECAP for order books
        /// </summary>
        MAMA_MSG_TYPE_BOOK_RECAP        =   19,

        /// <summary>
        /// Order book recap. Sent rather than MAMA_MSG_TYPE_SNAPSHOT for order books
        /// </summary>
        MAMA_MSG_TYPE_BOOK_SNAPSHOT     =   20,

        /// <summary>
        /// Not permissioned on the feed
        /// </summary>
        MAMA_MSG_TYPE_NOT_PERMISSIONED  =   21,

        /// <summary>
        /// The symbols was not found but may show up later. This indicates
        /// that the symbol is not currently in the feed handler's cache,
        /// but may get added later. The feed handler must be configured
        /// with OrderBookNotFoundAction and RecordNotFoundAction set to
        /// not_found to enable this behaviour.
        /// </summary>
        MAMA_MSG_TYPE_NOT_FOUND         =   22,

        /// <summary>
        /// End of group of initial values.  Marks the last initial value for
        /// group subscriptions.
        /// </summary>
        MAMA_MSG_TYPE_END_OF_INITIALS   =   23,

        /// <summary>
        /// A service request
        /// </summary>
        MAMA_MSG_TYPE_WOMBAT_REQUEST    =   24,

        /// <summary>
        /// A calculated result
        /// </summary>
        MAMA_MSG_TYPE_WOMBAT_CALC       =   25,

        /// <summary>
        /// A Security Status update
        /// </summary>
        MAMA_MSG_TYPE_SEC_STATUS        =   26,

        /// <summary>
        /// Data dictionary. This message contains the data dictionary
        /// </summary>
        MAMA_MSG_TYPE_DDICT_SNAPSHOT    =   50,

        /// <summary>
        /// Miscellaneous
        /// </summary>
        MAMA_MSG_TYPE_MISC              =   100,

        /// <summary>
        /// Returned if an RV error is encountered. The MsgStatus will be
        /// the tibrv_status
        /// </summary>
        MAMA_MSG_TYPE_TIBRV             =   101,

        /// <summary>
        /// The set of features and related params for a particular publisher
        /// This is an internal message type, it does not need to be
        /// processed by MAMA applications
        /// </summary>
        MAMA_MSG_TYPE_FEATURE_SET       =   150,

        /// <summary>
        /// Subscription synchronization request
        /// This is an internal message type, it does not need to be
        /// processed by MAMA applications
        /// </summary>
        MAMA_MSG_TYPE_SYNC_REQUEST      =   170,

        /// <summary>
        /// Subscription refresh
        /// This is an internal message type, it does not need to be
        /// processed by MAMA applications
        /// </summary>
        MAMA_MSG_TYPE_REFRESH           =   171,

        /// <summary>
        /// Worldview message
        /// This is an internal message type, it does not need to be
        /// processed by MAMA applications
        /// </summary>
        MAMA_MSG_TYPE_WORLD_VIEW        =   172,

        /// <summary>
        /// A News Query message
        /// </summary>
        MAMA_MSG_TYPE_NEWS_QUERY        =   173,

        /// <summary>
        /// A "keepalive" message
        /// This is an internal message type, it does not need to be
        /// processed by MAMA applications
        /// </summary>
        MAMA_MSG_TYPE_NULL              =   175,

        /// <summary>
        /// An Entitlements Refresh message
        /// </summary>
        MAMA_MSG_TYPE_ENTITLEMENTS_REFRESH =   176,

        /// <summary>
        /// Unknown message type
        /// This is an internal message type, it does not need to be
        /// processed by MAMA applications
        /// </summary>
        MAMA_MSG_TYPE_UNKNOWN           =   199
    }

    /// <summary>
    /// Represents the status of a <see cref="MamaMsg"/>
    /// </summary>
    public enum mamaMsgStatus
    {
        /// <summary>
        /// All's well
        /// </summary>
        MAMA_MSG_STATUS_OK                  =   0,

        /// <summary>
        /// The feed handler has detected a Line Down
        /// </summary>
        MAMA_MSG_STATUS_LINE_DOWN           =   1,

        /// <summary>
        /// The feed handler does not have any subscribers to the subject
        /// </summary>
        MAMA_MSG_STATUS_NO_SUBSCRIBERS      =   2,

        /// <summary>
        /// The symbol does not exist
        /// </summary>
        MAMA_MSG_STATUS_BAD_SYMBOL          =   3,

        /// <summary>
        /// Expired
        /// </summary>
        MAMA_MSG_STATUS_EXPIRED             =   4,

        /// <summary>
        /// A time out occurred
        /// </summary>
        MAMA_MSG_STATUS_TIMEOUT             =   5,

        /// <summary>
        /// Miscellaneous status. Not an error
        /// </summary>
        MAMA_MSG_STATUS_MISC                =   6,

        /// <summary>
        /// The subject is stale. Messages may have been dropped
        /// </summary>
        MAMA_MSG_STATUS_STALE               =   7,

        //This looks wrong but basdically these 2 are the same at the c layer so we need to stay in step
        /// <summary>
        /// Tibrv status / Middleware error status
        /// </summary>
        MAMA_MSG_STATUS_PLATFORM_STATUS     =   8,
        MAMA_MSG_STATUS_TIBRV_STATUS        =   8,

        /// <summary>
        /// Not entitled to a subject
        /// </summary>
        MAMA_MSG_STATUS_NOT_ENTITLED        =   9,

        /// <summary>
        /// Not found
        /// </summary>
        MAMA_MSG_STATUS_NOT_FOUND           =   10,

        /// <summary>
        /// Messages may have been dropped
        /// </summary>
        MAMA_MSG_STATUS_POSSIBLY_STALE      =   11,

        /// <summary>
        /// Not permissioned for the subject
        /// </summary>
        MAMA_MSG_STATUS_NOT_PERMISSIONED    =   12,

        /// <summary>
        /// Topic renamed
        /// </summary>
        MAMA_MSG_STATUS_TOPIC_CHANGE        =   13,

        /// <summary>
        /// Bandwidth exceeded
        /// </summary>
        MAMA_MSG_STATUS_BANDWIDTH_EXCEEDED  =   14,

        /// <summary>
        /// Message with duplicate sequence number
        /// </summary>
        MAMA_MSG_STATUS_DUPLICATE           =   15,

        /// <summary>
        /// Unknown status
        /// </summary>

        MAMA_MSG_STATUS_UNKNOWN             =   99
    }

	/// <summary>
	/// Class for iterating through the fields in a MamaMsg
	/// </summary>
	public class MamaMsgIterator : MamaWrapper
	{
	    /// <summary>
	    /// Creates and iterator
	    /// </summary>
	    /// <param name="dictionary">Dictionary to use when iterating through the
	    /// fields. Pass <code>null</code> is no dictionary is available</param>
		public MamaMsgIterator (MamaDictionary dictionary)
		{
			IntPtr dictHandle = dictionary != null ? dictionary.NativeHandle : IntPtr.Zero;

			int code = NativeMethods.mamaMsgIterator_create(ref nativeHandle, dictHandle);
			CheckResultCode(code);
		}

        /// <summary>
        /// Implements the destruction of the underlying peer object
        /// <seealso cref="MamaWrapper.DestroyNativePeer"/>
        /// </summary>
		protected override MamaStatus.mamaStatus DestroyNativePeer()
		{



			return 0;
		}

		private struct NativeMethods
		{
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgIterator_create (ref IntPtr iterator, IntPtr dict);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern IntPtr mamaMsgIterator_next (IntPtr iterator);
		}


		public void SetField (IntPtr pField)
		{
			if (mField == null)
				mField = new MamaMsgField (pField);

			mField.setNativeHandle(pField);
		}

		public MamaMsgField getField ()
		{
			if (mField.NativeHandle == (IntPtr) null)
				return null;
			else
				return mField;
		}

		public static MamaMsgIterator operator++ (MamaMsgIterator rhs)
		{
			IntPtr tempfield = NativeMethods.mamaMsgIterator_next(rhs.nativeHandle);
			rhs.mField.setNativeHandle(tempfield);
			return rhs;
		}

		private MamaMsgField mField;

	}

	/// <summary>
    /// Class which encapsulates the messages used in the MAMA infrastructure
    /// </summary>
	public class MamaMsg : MamaWrapper
	{
		/// <summary>
        /// Create a msg using the default payload.
        /// </summary>
        public MamaMsg ()
        {
            int code = NativeMethods.mamaMsg_create(ref nativeHandle);
            CheckResultCode(code);
        }

		/// <summary>
		/// NOTE: This function has been deprecated in line with the
		/// deprecation of the mamaPayloadType enum.
		/// Create a msg using the payload specified.
        /// <param>The payload ID to create the message for</param>
		/// </summary>
        [Obsolete("MamaMsg(MamaPayloadType PayloadId) has been deprecated, "
                 +"use MamaMsg(char payloadId) instead.")]
		public MamaMsg (mamaPayloadType payloadId)
		{
	        int code = NativeMethods.mamaMsg_createForPayload(ref nativeHandle, payloadId);
			CheckResultCode(code);
        }

        /// <summary>
        /// Create a msg using the payload specified.
        /// <param>Char identifying the payload to create the msg for</param>
        /// </summary>
        public MamaMsg (char payloadId)
        {
            int code = NativeMethods.mamaMsg_createForPayload(ref nativeHandle, payloadId);
		    CheckResultCode(code);
        }

        /// <summary>
        /// Create a msg using the payload bridge specified.
        /// </summary>
        /// <param name="payloadBridge">The payload bridge to create the message for</param>
        public MamaMsg (MamaPayloadBridge payloadBridge)
        {
		    int code = NativeMethods.mamaMsg_createForPayloadBridge (ref nativeHandle, payloadBridge.NativeHandle);
			CheckResultCode(code);
		}

		/// <summary>
		/// Create a deep copy of a message.
		/// </summary>
		public MamaMsg (MamaMsg mamaMsgSrc)
		{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
			if (mamaMsgSrc == null)
			{
				throw new ArgumentNullException("mamaMsgSrc");
			}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
			int code = NativeMethods.mamaMsg_copy(mamaMsgSrc.NativeHandle, ref nativeHandle);
			CheckResultCode(code);

			GC.KeepAlive(mamaMsgSrc);
		}

		internal MamaMsg(IntPtr nativeHandle) : base(nativeHandle)
		{
		}


		public void getByteBuffer (ref byte[] byteBuffer, ref int size)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			int code = NativeMethods.mamaMsg_getByteBuffer(nativeHandle, ref array, ref size);
			CheckResultCode(code);
            if ((myByteBuffer == null) || (myByteBuffer.Length < size) )
                myByteBuffer = new byte[size];
			Marshal.Copy(array, myByteBuffer, 0, (int)size);
            byteBuffer = myByteBuffer;
		}


		public void createFromBuffer (byte[] byteBuffer, int size)
		{
			EnsurePeerCreated();

            if (buffer != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(buffer);
                buffer = IntPtr.Zero;
            }

            buffer = Marshal.AllocHGlobal(size);
            Marshal.Copy(byteBuffer, 0, buffer, size);
            int code = NativeMethods.mamaMsg_createFromByteBuffer(ref nativeHandle, buffer, size);
			CheckResultCode(code);
		}

		public void createForBridgeFromBuffer (byte[] byteBuffer, int size, MamaBridge bridgeImpl)
		{
			EnsurePeerCreated();
            if (buffer != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(buffer);
                buffer = IntPtr.Zero;
            }
			Marshal.Copy(byteBuffer, 0, buffer, size);
			int code = NativeMethods.mamaMsg_createForBridgeFromByteBuffer(ref nativeHandle, buffer, size, bridgeImpl.NativeHandle);
			CheckResultCode(code);
		}

        public MamaMsg detach ()
        {
            int code = NativeMethods.mamaMsg_detach(nativeHandle);

            MamaMsg result = new MamaMsg ();
            code = NativeMethods.mamaMsg_destroy(result.nativeHandle);
            result.setNativeHandle (nativeHandle);

            return result;
        }
		private ulong convertToMamaDateTime (DateTime val)
		{
			ulong dateTimeMsec = (ulong)((val.Ticks - 621355968000000000) / TimeSpan.TicksPerMillisecond);
			ulong dateTime = 0;
			int code = NativeMethods.mamaDateTime_setEpochTimeMilliseconds (ref dateTime, dateTimeMsec);
			CheckResultCode(code);
			return dateTime;
		}

		private long convertFromMamaDateTime (ulong val)
		{
			ulong dateTimeMsec = 0;
			int code = NativeMethods.mamaDateTime_getEpochTimeMilliseconds (ref val, ref dateTimeMsec);
			CheckResultCode(code);
			return ((long)dateTimeMsec * TimeSpan.TicksPerMillisecond) + 621355968000000000;
		}

		/// <summary>
		/// Extract the type
		/// </summary>
		/// <returns></returns>
		public mamaMsgType getType ()
		{
			EnsurePeerCreated();
			return (mamaMsgType) NativeMethods.mamaMsgType_typeForMsg(nativeHandle);
		}

		/// <summary>
		/// Return the status
		/// </summary>
		/// <returns></returns>
		public mamaMsgStatus getStatus ()
		{
			EnsurePeerCreated();
            return (mamaMsgStatus)getI32(null, (ushort)MamaReservedFields.MsgStatus.getFid());
		}

        ///<summary>
        /// Return the payload type
        ///</summary>
        public mamaPayloadType getPayloadType()
        {
            EnsurePeerCreated();
            mamaPayloadType type = mamaPayloadType.MAMA_PAYLOAD_WOMBAT_MSG;
            int code = NativeMethods.mamaMsg_getPayloadType(nativeHandle, ref type);
            CheckResultCode(code);

            return type;
        }

		/// <summary>
		/// Invoke the specified callback for each field in the message.
		/// </summary>
		/// <param name="iterator"></param>
		/// <param name="dictionary"></param>
		/// <param name="closure"></param>
		public void iterateFields(
			MamaMsgFieldIterator iterator,
			MamaDictionary  dictionary,
			object closure)
		{
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
			if (iterator == null)
			{
				throw new ArgumentNullException("iterator");
			}
#endif // MAMA_WRAPPERS_CHECK_ARGUMENTS
			EnsurePeerCreated();

			IntPtr dictHandle = dictionary != null ? dictionary.NativeHandle : IntPtr.Zero;
			if (forwarder == null)
			{
				forwarder = new CallbackForwarder(this, iterator, closure);
				callback =
					new CallbackForwarder.MamaMessageIteratorDelegate(forwarder.OnField);
				GC.SuppressFinalize(forwarder);
				GC.SuppressFinalize(callback);
			}
			else
			{
				forwarder.mCallback = iterator;
				forwarder.mClosure = closure;
			}
			int code = NativeMethods.mamaMsg_iterateFields(nativeHandle, callback, dictHandle, nativeHandle);
			CheckResultCode(code);

			GC.KeepAlive(iterator);
			GC.KeepAlive(dictionary);
		}

		/// <summary>
		/// Clear a msg. All fields are removed.
		/// </summary>
		public void clear ()
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_clear(nativeHandle);
			CheckResultCode(code);
		}

        protected override void OnDispose ()
        {
            if (tempMsgVector != null)
                tempMsgVector[0].Dispose ();

            if (price_ != null)
                price_.Dispose ();

            if (mField != null)
                mField.Dispose ();

            if (msg_ != null)
                msg_.Dispose ();

        }

		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		protected override MamaStatus.mamaStatus DestroyNativePeer()
		{
            return (MamaStatus.mamaStatus)NativeMethods.mamaMsg_destroy(nativeHandle);
		}

		/// <summary>
		/// Destroy a message and free any resources associated with it.
		/// </summary>
		public void destroy ()
		{
            if (buffer != IntPtr.Zero)
            {
                Marshal.FreeHGlobal(buffer);
            }
			GC.KeepAlive(forwarder);
			GC.KeepAlive(callback);
			Dispose();
		}

		/// <summary>
		/// Get the size of the underlying message in bytes.
		/// Only works for tibrvMsg and wombatmsg types.
		/// </summary>
		public int getByteSize ()
		{
			EnsurePeerCreated();
			int size = 0;
			int code = NativeMethods.mamaMsg_getByteSize(nativeHandle, ref size);
			CheckResultCode(code);
			return size;
		}

		/// <summary>
		/// Add a new bool field.
		/// </summary>
		public void addBool(
			string name,
			ushort fid,
			bool val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_addBool(nativeHandle, name, fid, val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Add a new char field.
		/// </summary>
		public void addChar(
			string name,
			ushort fid,
			char val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_addChar(nativeHandle, name, fid,val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Add a new 8 bit signed int field.
		/// </summary>
		public void addI8(
			string name,
			ushort fid,
			sbyte val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_addI8(nativeHandle, name, fid,val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Add a new 8 bit unsigned int field.
		/// </summary>
		public void addU8(
			string name,
			ushort fid,
			byte val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_addU8(nativeHandle, name, fid,val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Add a new 16 bit signed int field.
		/// </summary>
		public void addI16(
			string name,
			ushort fid,
			short val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_addI16(nativeHandle, name, fid, val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Add a new 16 bit unsigned int field.
		/// </summary>
		public void addU16(
			string name,
			ushort fid,
			ushort val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_addU16(nativeHandle, name, fid, val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Add a new 32 bit unsigned int field.
		/// </summary>
		public void addI32(
			string name,
			ushort fid,
			int val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_addI32(nativeHandle, name, fid, val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Add a new 32 bit signed int field.
		/// </summary>
		public void addU32(
			string name,
			ushort fid,
			uint val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_addU32(nativeHandle, name, fid, val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Add a new I64 field.
		/// </summary>
		public void addI64(
			string name,
			ushort fid,
			long val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_addI64(nativeHandle, name, fid, val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Add a new U64 field.
		/// </summary>
		public void addU64(
			string name,
			ushort fid,
			ulong val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_addU64(nativeHandle, name, fid, val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Add a new float (f32) field.
		/// </summary>
		public void addF32(
			string name,
			ushort fid,
			float val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_addF32(nativeHandle, name, fid, val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Add a new f64 field.
		/// </summary>
		public void addF64(
			string name,
			ushort fid,
			double val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_addF64(nativeHandle, name, fid, val);
			CheckResultCode(code);
		}


		/// <summary>
		/// Add a string field.
		/// </summary>
		public void addString(
			string name,
			ushort fid,
			string val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_addString(nativeHandle, name, fid, val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Add an opaque field.
		/// </summary>
		public void addOpaque(
			string name,
			ushort fid,
			byte[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length);
			try
			{
				Marshal.Copy(val, 0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_addOpaque(nativeHandle, name, fid, ptr, val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Add a MAMA date/time field.
		/// </summary>
		public void addDateTime (
			string name,
			ushort fid,
			DateTime val)
		{
			EnsurePeerCreated();
			ulong put = convertToMamaDateTime (val);
			int code = NativeMethods.mamaMsg_addDateTime(nativeHandle, name, fid, ref put);
			CheckResultCode(code);
		}

		/// <summary>
		/// Add a MAMA price field.
		/// </summary>
		public void addPrice (
			string name,
			ushort fid,
			MamaPrice val)
		{
			EnsurePeerCreated();
			IntPtr put = val.NativeHandle;
			int code = NativeMethods.mamaMsg_addPrice(nativeHandle, name, fid, put);
			CheckResultCode(code);
		}

        /// <summary>
        /// Add a MamaMsg to the msg
        /// </summary>
        public void addMsg(string name, ushort fid, MamaMsg val)
        {
            EnsurePeerCreated();
            IntPtr put = val.NativeHandle;
            int code = NativeMethods.mamaMsg_addMsg(nativeHandle, name, fid, put);
            CheckResultCode(code);
        }

        /// <summary>
		/// Add an array of booleans to the message.
		/// </summary>
		public void addVectorBool (
			string name,
			ushort fid,
			bool[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length);
			try
			{
				byte[] loc = new byte[val.Length];
				for (int i = 0; i < loc.Length; ++i)
				{
					loc[i] = (byte)(val[i] ? 1 : 0);
				}
				Marshal.Copy(loc, 0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_addVectorBool(nativeHandle, name, fid, ptr,val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Add an array of characters to the message.
		/// </summary>
		public void addVectorChar (
			string name,
			ushort fid,
			char[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length);
			try
			{
				byte[] loc = new byte[val.Length];
				for (int i = 0; i < val.Length; i++)
				{
					loc[i] = (byte)val[i];
				}
				Marshal.Copy(loc,0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_addVectorChar(nativeHandle, name, fid, ptr,val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Add an array of signed 8 bit integers to the message.
		/// </summary>
		public void addVectorI8 (
			string name,
			ushort fid,
			sbyte[]val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length);
			try
			{
				byte[] loc = new byte[val.Length];
				for (int i = 0; i < val.Length; i++)
				{
					loc[i] = (byte)val[i];
				}
				Marshal.Copy(loc,0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_addVectorI8(nativeHandle, name, fid, ptr,val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Add an array of unsigned 8 bit integers to the message.
		/// </summary>
		public void addVectorU8 (
			string name,
			ushort fid,
			byte[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length);
			try
			{
				Marshal.Copy(val,0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_addVectorU8(nativeHandle, name, fid, ptr,val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Add an array of signed 16 bit integers to the message.
		/// </summary>
		public void addVectorI16 (
			string name,
			ushort fid,
			short[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length * 2);
			try
			{
				Marshal.Copy(val,0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_addVectorI16(nativeHandle, name, fid, ptr,val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Add an array of unsigned 16 bit integers to the message.
		/// </summary>
		public void addVectorU16 (
			string name,
			ushort fid,
			ushort[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length * 2);
			try
			{
				short[] loc = new short[val.Length];
				for (int i = 0; i < val.Length; i++)
				{
					loc[i] = (short)val[i];
				}
				Marshal.Copy(loc,0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_addVectorU16(nativeHandle, name, fid, ptr,val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Add an array of signed 32 bit integers to the message.
		/// </summary>
		public void addVectorI32 (
			string name,
			ushort fid,
			int[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length * 4);
			try
			{
				Marshal.Copy(val,0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_addVectorI32(nativeHandle, name, fid, ptr,val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Add an array of unsigned 32 bit integers to the message.
		/// </summary>
		public void addVectorU32 (
			string name,
			ushort fid,
			uint[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length * 4);
			try
			{
				int[] loc = new int[val.Length];
				for (int i = 0; i < val.Length; i++)
				{
					loc[i] = (int)val[i];
				}
				Marshal.Copy(loc,0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_addVectorU32(nativeHandle, name, fid, ptr,val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Add an array of signed 64 bit integers to the message.
		/// </summary>
		public void addVectorI64 (
			string name,
			ushort fid,
			long[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length * 8);
			try
			{
				Marshal.Copy(val,0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_addVectorI64(nativeHandle, name, fid, ptr,val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Add an array of unsigned 64 bit integers to the message.
		/// </summary>
		public void addVectorU64 (
			string name,
			ushort fid,
			ulong[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length * 8);
			try
			{
				long[] loc = new long[val.Length];
				for (int i = 0; i < val.Length; i++)
				{
					loc[i] = (long)val[i];
				}
				Marshal.Copy(loc,0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_addVectorU64(nativeHandle, name, fid, ptr,val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Add an array of 32 bit floating point numbers to the message.
		/// </summary>
		public void addVectorF32 (
			string name,
			ushort fid,
			float[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length * 4);
			try
			{
				Marshal.Copy(val,0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_addVectorF32(nativeHandle, name, fid, ptr,val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Add an array of 64 bit floating point numbers to the message.
		/// </summary>
		public void addVectorF64 (
			string name,
			ushort fid,
			double[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length * 8);
			try
			{
				Marshal.Copy(val,0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_addVectorF64(nativeHandle, name, fid, ptr,val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Add an array of timestamps to the message.
		/// </summary>
		public void addVectorDateTime (
			string name,
			ushort fid,
			DateTime[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length * 8);
			try
			{
				long[] array = new long[val.Length];
				for (int i = 0; i < val.Length; i++)
				{
					array[i] = (long)convertToMamaDateTime (val[i]);
				}
				Marshal.Copy(array,0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_addVectorDateTime(nativeHandle, name, fid, ptr,val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}



		/// <summary>
		/// Update the value of an existing bool field. If the field does not exist
		/// it is added.
		/// </summary>
		public void updateBool(
			string name,
			ushort fid,
			bool val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_updateBool(nativeHandle, name, fid,val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Update the value of an existing char field. If the field does not exist
		/// it is added.
		/// </summary>
		public void updateChar(
			string name,
			ushort fid,
			char val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_updateChar(nativeHandle, name, fid,val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Update the value of an existing I8 field. If the field does not exist
		/// it is added.
		/// </summary>
		public void updateI8(
			string name,
			ushort fid,
			sbyte val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_updateI8(nativeHandle, name, fid,val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Update the value of an existing U8 field. If the field does not exist
		/// it is added.
		/// </summary>
		public void updateU8(
			string name,
			ushort fid,
			byte val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_updateU8(nativeHandle, name, fid,val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Update the value of an existing I16 field. If the field does not exist
		/// it is added.
		/// </summary>
		public void updateI16(
			string name,
			ushort fid,
			short val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_updateI16(nativeHandle, name, fid,val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Update the value of an existing U16 field. If the field does not exist
		/// it is added.
		/// </summary>
		public void updateU16(
			string name,
			ushort fid,
			ushort val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_updateU16(nativeHandle, name, fid,val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Update the value of an existing I32 field. If the field does not exist
		/// it is added.
		/// </summary>
		public void updateI32(
			string name,
			ushort fid,
			int val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_updateI32(nativeHandle, name, fid,val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Update the value of an existing U32 field. If the field does not exist
		/// it is added.
		/// </summary>
		public void updateU32(
			string name,
			ushort fid,
			uint val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_updateU32(nativeHandle, name, fid,val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Update the value of an existing I64 field. If the field does not exist
		/// it is added.
		/// </summary>
		public void updateI64(
			string name,
			ushort fid,
			long val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_updateI64(nativeHandle, name, fid,val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Update the value of an existing U64 field. If the field does not exist
		/// it is added.
		/// </summary>
		public void updateU64(
			string name,
			ushort fid,
			ulong val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_updateU64(nativeHandle, name, fid,val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Update the value of an existing float field. If the field does not exist
		/// it is added.
		/// </summary>
		public void updateF32(
			string name,
			ushort fid,
			float val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_updateF32(nativeHandle, name, fid,val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Update the value of an existing f64 field. If the field does not exist
		/// it is added.
		/// </summary>
		public void updateF64(
			string name,
			ushort fid,
			double val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_updateF64(nativeHandle, name, fid,val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Update the value of an existing string field. If the field
		/// does not exist it is added.
		/// </summary>
		public void updateString(
			string name,
			ushort fid,
			string val)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaMsg_updateString(nativeHandle, name, fid, val);
			CheckResultCode(code);
		}

		/// <summary>
		/// Update an opaque field.
		/// </summary>
		public void updateOpaque(
			string name,
			ushort fid,
			byte[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length);
			try
			{
				Marshal.Copy(val,0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_updateOpaque(nativeHandle, name, fid, ptr,val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Update a MAMA date/time field.
		/// </summary>
		public void updateDateTime(
			string name,
			ushort fid,
			DateTime val)
		{
			EnsurePeerCreated();
			ulong put = convertToMamaDateTime (val);
			int code = NativeMethods.mamaMsg_updateDateTime(nativeHandle, name, fid,ref put);
			CheckResultCode(code);
		}

		/// <summary>
		/// Update a MAMA price field.
		/// </summary>
		public void updatePrice(
			string name,
			ushort fid,
			MamaPrice val)
		{
			EnsurePeerCreated();
			IntPtr put = val.NativeHandle;
			int code = NativeMethods.mamaMsg_updatePrice(nativeHandle, name, fid,put);
			CheckResultCode(code);
		}

		/// <summary>
		/// Update an array of booleans.
		/// </summary>
		public void updateVectorBool(
			string name,
			ushort fid,
			bool[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length);
			try
			{
				byte[] loc = new byte[val.Length];
				for (int i = 0; i < loc.Length; ++i)
				{
					loc[i] = (byte)(val[i] ? 1 : 0);
				}
				Marshal.Copy(loc, 0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_updateVectorBool(nativeHandle, name, fid, ptr, val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Update an array of characters.
		/// </summary>
		public void updateVectorChar(
			string name,
			ushort fid,
			char[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length);
			try
			{
				byte[] loc = new byte[val.Length];
				for (int i = 0; i < val.Length; i++)
				{
					loc[i] = (byte)val[i];
				}
				Marshal.Copy(loc, 0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_updateVectorChar(nativeHandle, name, fid, ptr, val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Update an array of signed 8 bit integers.
		/// </summary>
		public void updateVectorI8(
			string name,
			ushort fid,
			sbyte[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length);
			try
			{
				byte[] loc = new byte[val.Length];
				for (int i = 0; i < val.Length; i++)
				{
					loc[i] = (byte)val[i];
				}
				Marshal.Copy(loc, 0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_updateVectorI8(nativeHandle, name, fid, ptr, val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Update an array of unsigned 8 bit integers.
		/// </summary>
		public void updateVectorU8(
			string name,
			ushort fid,
			byte[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length);
			try
			{
				Marshal.Copy(val, 0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_updateVectorU8(nativeHandle, name, fid, ptr, val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Update an array of signed 16 bit integers.
		/// </summary>
		public void updateVectorI16(
			string name,
			ushort fid,
			short[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length * 2);
			try
			{
				Marshal.Copy(val, 0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_updateVectorI16(nativeHandle, name, fid, ptr, val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Update an array of unsigned 16 bit integers.
		/// </summary>
		public void updateVectorU16(
			string name,
			ushort fid,
			ushort[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length * 2);
			try
			{
				short[] loc = new short[val.Length];
				for (int i = 0; i < val.Length; i++)
				{
					loc[i] = (short)val[i];
				}
				Marshal.Copy(loc, 0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_updateVectorU16(nativeHandle, name, fid, ptr, val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Update an array of signed 32 bit integers.
		/// </summary>
		public void updateVectorI32(
			string name,
			ushort fid,
			int[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length * 4);
			try
			{
				Marshal.Copy(val, 0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_updateVectorI32(nativeHandle, name, fid, ptr, val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Update an array of unsigned 32 bit integers.
		/// </summary>
		public void updateVectorU32(
			string name,
			ushort fid,
			uint[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length * 4);
			try
			{
				int[] loc = new int[val.Length];
				for (int i = 0; i < val.Length; i++)
				{
					loc[i] = (int)val[i];
				}
				Marshal.Copy(loc, 0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_updateVectorU32(nativeHandle, name, fid, ptr, val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Update an array of signed 64 bit integers.
		/// </summary>
		public void updateVectorI64(
			string name,
			ushort fid,
			long[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length * 8);
			try
			{
				Marshal.Copy(val, 0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_updateVectorI64(nativeHandle, name, fid, ptr, val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Update an array of unsigned 64 bit integers.
		/// </summary>
		public void updateVectorU64(
			string name,
			ushort fid,
			ulong[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length * 8);
			try
			{
				long[] loc = new long[val.Length];
				for (int i = 0; i < val.Length; i++)
				{
					loc[i] = (long)val[i];
				}
				Marshal.Copy(loc, 0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_updateVectorU64(nativeHandle, name, fid, ptr, val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Update an array of 32 bit floating point numbers.
		/// </summary>
		public void updateVectorF32(
			string name,
			ushort fid,
			float[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length * 4);
			try
			{
				Marshal.Copy(val, 0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_updateVectorF32(nativeHandle, name, fid, ptr, val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Update an array of 64 bit floating point numbers.
		/// </summary>
		public void updateVectorF64(
			string name,
			ushort fid,
			double[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length * 8);
			try
			{
				Marshal.Copy(val, 0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_updateVectorF64(nativeHandle, name, fid, ptr, val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}

		/// <summary>
		/// Update an array of timestamps.
		/// </summary>
		public void updateVectorDateTime(
			string name,
			ushort fid,
			DateTime[] val)
		{
			EnsurePeerCreated();
			IntPtr ptr = Marshal.AllocHGlobal(val.Length * 8);
			try
			{
				long[] array = new long[val.Length];
				for (int i = 0; i < val.Length; i++)
				{
					array[i] = (long)convertToMamaDateTime (val[i]);
				}
				Marshal.Copy(array,0, ptr, val.Length);
				int code = NativeMethods.mamaMsg_updateVectorDateTime(nativeHandle, name, fid, ptr, val.Length);
				CheckResultCode(code);
			}
			finally
			{
				Marshal.FreeHGlobal(ptr);
			}
		}



		/// <summary>
		/// Get a bool field.
		/// </summary>
		public bool getBool(
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			bool ret = false;
			int code = NativeMethods.mamaMsg_getBool(nativeHandle, name, fid,ref ret);
			CheckResultCode(code);
			return ret;
		}

		/// <summary>
		/// Get a bool field.
		/// </summary>
		public bool getBool(
			MamaFieldDescriptor descriptor)
		{
			return getBool (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a bool field.
		/// </summary>
		public bool getBool(
			string name,
			ushort fid,
			bool valueIfMissing)
		{
			bool result = false;
			if (tryBool (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a bool field.
		/// </summary>
		public bool getBool(
			MamaFieldDescriptor descriptor,
			bool valueIfMissing)
		{
			return getBool (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a char field.
		/// </summary>
		public char getChar(
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			char ret = (char)0;
			int code = NativeMethods.mamaMsg_getChar(nativeHandle, name, fid,ref ret);
			CheckResultCode(code);
			return ret;
		}

		/// <summary>
		/// Get a char field.
		/// </summary>
		public char getChar(
			MamaFieldDescriptor descriptor)
		{
			return getChar (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a char field.
		/// </summary>
		public char getChar(
			string name,
			ushort fid,
			char valueIfMissing)
		{
			char result = '\0';
			if (tryChar (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a char field.
		/// </summary>
		public char getChar(
			MamaFieldDescriptor descriptor,
			char valueIfMissing)
		{
			return getChar (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a I8, signed 8 bit integer, field.
		/// </summary>
		public sbyte getI8(
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			sbyte ret = 0;
			int code = NativeMethods.mamaMsg_getI8(nativeHandle, name, fid,ref ret);
			CheckResultCode(code);
			return ret;
		}

		/// <summary>
		/// Get a I8, signed 8 bit integer, field.
		/// </summary>
		public sbyte getI8(
			MamaFieldDescriptor descriptor)
		{
			return getI8 (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a I8, signed 8 bit integer, field.
		/// </summary>
		public sbyte getI8(
			string name,
			ushort fid,
			sbyte valueIfMissing)
		{
			sbyte result = 0;
			if (tryI8 (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a I8, signed 8 bit integer, field.
		/// </summary>
		public sbyte getI8(
			MamaFieldDescriptor descriptor,
			sbyte valueIfMissing)
		{
			return getI8 (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

        /// <summary>
        /// Get a U8, unsigned 8 bit integer, field.
        /// </summary>
		public byte getU8(
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			byte ret = 0;
			int code = NativeMethods.mamaMsg_getU8(nativeHandle, name, fid,ref ret);
			CheckResultCode(code);
			return ret;
		}

		/// <summary>
		/// Get a U8, unsigned 8 bit integer, field.
		/// </summary>
		public byte getU8(
			MamaFieldDescriptor descriptor)
		{
			return getU8 (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a U8, unsigned 8 bit integer, field.
		/// </summary>
		public byte getU8(
			string name,
			ushort fid,
			byte valueIfMissing)
		{
			byte result = 0;
			if (tryU8 (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a U8, unsigned 8 bit integer, field.
		/// </summary>
		public byte getU8(
			MamaFieldDescriptor descriptor,
			byte valueIfMissing)
		{
			return getU8 (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a I16, signed 16 bit integer, field.
		/// </summary>
		public short getI16(
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			short ret = 0;
			int code = NativeMethods.mamaMsg_getI16(nativeHandle, name, fid,ref ret);
			CheckResultCode(code);
			return ret;
		}

		/// <summary>
		/// Get a I16, signed 16 bit integer, field.
		/// </summary>
		public short getI16(
			MamaFieldDescriptor descriptor)
		{
			return getI16 (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a I16, signed 16 bit integer, field.
		/// </summary>
		public short getI16(
			string name,
			ushort fid,
			short valueIfMissing)
		{
			short result = 0;
			if (tryI16 (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a I16, signed 16 bit integer, field.
		/// </summary>
		public short getI16(
			MamaFieldDescriptor descriptor,
			short valueIfMissing)
		{
			return getI16 (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a U16, unsigned 16 bit integer, field.
		/// </summary>
		public ushort getU16(
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			ushort ret = 0;
			int code = NativeMethods.mamaMsg_getU16(nativeHandle, name, fid,ref ret);
			CheckResultCode(code);
			return ret;
		}

		/// <summary>
		/// Get a U16, unsigned 16 bit integer, field.
		/// </summary>
		public ushort getU16(
			MamaFieldDescriptor descriptor)
		{
			return getU16 (descriptor.getName(), (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a U16, unsigned 16 bit integer, field.
		/// </summary>
		public ushort getU16(
			string name,
			ushort fid,
			ushort valueIfMissing)
		{
			ushort result = 0;
			if (tryU16 (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a U16, unsigned 16 bit integer, field.
		/// </summary>
		public ushort getU16(
			MamaFieldDescriptor descriptor,
			ushort valueIfMissing)
		{
			return getU16 (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a I32, signed 32 bit integer, field.
		/// </summary>
		public int getI32(
			string name,
			ushort fid)
		{
			int result = 0;
			tryI32Impl(name, fid, ref result, true);
			return result;
        }

		/// <summary>
		/// Get a I32, signed 32 bit integer, field.
		/// </summary>
		public int getI32(
			MamaFieldDescriptor descriptor)
		{
			return getI32 (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a I32, signed 32 bit integer, field.
		/// </summary>
		public int getI32(
			string name,
			ushort fid,
			int valueIfMissing)
		{
			int result = 0;
			if (tryI32 (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a I32, signed 32 bit integer, field.
		/// </summary>
		public int getI32(
			MamaFieldDescriptor descriptor,
			int valueIfMissing)
		{
			return getI32 (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a U32, unsigned 32 bit integer, field.
		/// </summary>
		public uint getU32(
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			uint ret = 0;
			int code = NativeMethods.mamaMsg_getU32(nativeHandle, name, fid,ref ret);
			CheckResultCode(code);
			return ret;
		}

		/// <summary>
		/// Get a U32, unsigned 32 bit integer, field.
		/// </summary>
		public uint getU32(
			MamaFieldDescriptor descriptor)
		{
			return getU32 (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a U32, unsigned 32 bit integer, field.
		/// </summary>
		public uint getU32(
			string name,
			ushort fid,
			uint valueIfMissing)
		{
			uint result = 0;
			if (tryU32 (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a U32, unsigned 32 bit integer, field.
		/// </summary>
		public uint getU32(
			MamaFieldDescriptor descriptor,
			uint valueIfMissing)
		{
			return getU32 (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a I64, signed 64 bit integer, field.
		/// </summary>
		public long getI64(
			string name,
			ushort fid)
		{
			long result = 0;
			tryI64Impl(name, fid, ref result, true);
			return result;
        }

		/// <summary>
		/// Get a I64, signed 64 bit integer, field.
		/// </summary>
		public long getI64(
			MamaFieldDescriptor descriptor)
		{
			return getI64 (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a I64, signed 64 bit integer, field.
		/// </summary>
		public long getI64(
			string name,
			ushort fid,
			long valueIfMissing)
		{
			long result = 0;
			if (tryI64 (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a I64, signed 64 bit integer, field.
		/// </summary>
		public long getI64(
			MamaFieldDescriptor descriptor,
			long valueIfMissing)
		{
			return getI64 (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a U64, unsigned 64 bit integer, field.
		/// </summary>
		public ulong getU64(
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			ulong ret = 0;
			int code = NativeMethods.mamaMsg_getU64(nativeHandle, name, fid,ref ret);
			CheckResultCode(code);
			return ret;
		}

		/// <summary>
		/// Get a U64, unsigned 64 bit integer, field.
		/// </summary>
		public ulong getU64(
			MamaFieldDescriptor descriptor)
		{
			return getU64 (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a U64, unsigned 64 bit integer, field.
		/// </summary>
		public ulong getU64(
			string name,
			ushort fid,
			ulong valueIfMissing)
		{
			ulong result = 0;
			if (tryU64 (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a U64, unsigned 64 bit integer, field.
		/// </summary>
		public ulong getU64(
			MamaFieldDescriptor descriptor,
			ulong valueIfMissing)
		{
			return getU64 (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a float (f32) field.
		/// </summary>
		public float getF32(
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			float ret = 0;
			int code = NativeMethods.mamaMsg_getF32(nativeHandle, name, fid,ref ret);
			CheckResultCode(code);
			return ret;
		}

		/// <summary>
		/// Get a float (f32) field.
		/// </summary>
		public float getF32(
			MamaFieldDescriptor descriptor)
		{
			return getF32 (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a float (f32) field.
		/// </summary>
		public float getF32(
			string name,
			ushort fid,
			float valueIfMissing)
		{
			float result = 0.0f;
			if (tryF32 (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a float (f32) field.
		/// </summary>
		public float getF32(
			MamaFieldDescriptor descriptor,
			float valueIfMissing)
		{
			return getF32 (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a F64 field.
		/// </summary>
		public double getF64(
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			double ret = 0;
			int code = NativeMethods.mamaMsg_getF64(nativeHandle, name, fid,ref ret);
			CheckResultCode(code);
			return ret;
		}

		/// <summary>
		/// Get a F64 field.
		/// </summary>
		public double getF64(
			MamaFieldDescriptor descriptor)
		{
			return getF64 (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a F64 field.
		/// </summary>
		public double getF64(
			string name,
			ushort fid,
			double valueIfMissing)
		{
			double result = 0.0;
			if (tryF64 (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a F64 field.
		/// </summary>
		public double getF64(
			MamaFieldDescriptor descriptor,
			double valueIfMissing)
		{
			return getF64 (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

        /// <summary>
		/// Get a string field.
		/// </summary>
		public string getString(
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
            IntPtr ret = IntPtr.Zero;
			int code = NativeMethods.mamaMsg_getString(nativeHandle, name, fid,ref ret);
			CheckResultCode(code);
            return Marshal.PtrToStringAnsi(ret);
		}

        public bool tryStringAnsi(string name, ushort fid, ref IntPtr result)
        {
            result = IntPtr.Zero;
            if (MamaWrapper.CheckResultCodeIgnoreNotFound(
                NativeMethods.mamaMsg_getString(base.nativeHandle, name, fid, ref result))
                != MamaStatus.mamaStatus.MAMA_STATUS_OK)
            {
                return false;
            }
            return true;
        }

        public bool tryStringAnsi(
			MamaFieldDescriptor descriptor,
			ref IntPtr result)
		{
			return tryStringAnsi (null, (ushort)descriptor.getFid(), ref result);
		}


		/// <summary>
		/// Get a string field.
		/// </summary>
		public string getString(
			MamaFieldDescriptor descriptor)
		{
			return getString (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a string field.
		/// </summary>
		public string getString(
			string name,
			ushort fid,
			string valueIfMissing)
		{
			string result = null;
			if (tryString (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a string field.
		/// </summary>
		public string getString(
			MamaFieldDescriptor descriptor,
			string valueIfMissing)
		{
			return getString (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

        /// <summary>
		/// Get an opaque field.
		/// </summary>
		public byte[] getOpaque(
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getOpaque(nativeHandle, name, fid,ref array, ref size);
			CheckResultCode(code);
			byte[] ret = new byte[size];
			Marshal.Copy(array, ret, 0, (int)size);
			return ret;
		}

		/// <summary>
		/// Get a opaque field.
		/// </summary>
		public byte[] getOpaque(
			MamaFieldDescriptor descriptor)
		{
			return getOpaque (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a opaque field.
		/// </summary>
		public byte[] getOpaque(
			string name,
			ushort fid,
			byte[] valueIfMissing)
		{
			byte[] result = null;
			if (tryOpaque (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a opaque field.
		/// </summary>
		public byte[] getOpaque(
			MamaFieldDescriptor descriptor,
			byte[] valueIfMissing)
		{
			return getOpaque (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a MAMA date/time field.
		/// </summary>
		public DateTime getDateTime(
			string name,
			ushort fid)
		{

			DateTime result = DateTime.MinValue;
			tryDateTimeImpl(name, fid, ref result, true);
            return result;
        }

		/// <summary>
		/// Get a MAMA date/time field.
		/// </summary>
		public DateTime getDateTime(
			MamaFieldDescriptor descriptor)
		{
			return getDateTime (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a MAMA date/time field.
		/// </summary>
		public DateTime getDateTime(
			string name,
			ushort fid,
			DateTime valueIfMissing)
		{
			DateTime result = DateTime.MinValue;
			if (tryDateTime (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a MAMA date/time field.
		/// </summary>
		public DateTime getDateTime(
			MamaFieldDescriptor descriptor,
			DateTime valueIfMissing)
		{
			return getDateTime (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a MAMA price field.
		/// </summary>
		public MamaPrice getPrice(
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
            if (price_ == null)
            {
                price_ = new MamaPrice ();
                price_.SelfManageLifeTime(false);
            }
			int code = NativeMethods.mamaMsg_getPrice(nativeHandle, name, fid, price_.NativeHandle);
			MamaStatus.mamaStatus status = (MamaStatus.mamaStatus)code;
			if (status != MamaStatus.mamaStatus.MAMA_STATUS_OK)
            {
				throw new MamaException(status);
            }

            return price_;
		}

		/// <summary>
		/// Get a MAMA price field.
		/// </summary>
		public MamaPrice getPrice(
			MamaFieldDescriptor descriptor)
		{
			return getPrice (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a MAMA price field.
		/// </summary>
		public MamaPrice getPrice(
			string name,
			ushort fid,
			MamaPrice valueIfMissing)
		{
			if (price_ == null)
            {
                price_ = new MamaPrice ();
                price_.SelfManageLifeTime(false);
            }
			if (tryPrice (name, fid, ref price_))
				return price_;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a MAMA price field.
		/// </summary>
		public MamaPrice getPrice(
			MamaFieldDescriptor descriptor,
			MamaPrice valueIfMissing)
		{
			return getPrice (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

        /// <summary>
		/// Get a submessage.
		/// </summary>
		public MamaMsg getMsg(
			string name,
			ushort fid)
		{
			if (msg_ == null)
            {
                msg_ = new MamaMsg ();
                msg_.SelfManageLifeTime(false);
            }
			tryMsgImpl(name, fid, ref msg_, true);
			return msg_;
        }

		/// <summary>
		/// Get a submessage.
		/// </summary>
		public MamaMsg getMsg(
			MamaFieldDescriptor descriptor)
		{
			return getMsg(null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a submessage.
		/// </summary>
		/// <param name="name"></param>
		/// <param name="fid"></param>
		/// <param name="valueIfMissing"></param>
		/// <returns></returns>
		public MamaMsg getMsg(
			string name,
			ushort fid,
			MamaMsg valueIfMissing)
		{
			if (msg_ == null)
            {
                msg_ = new MamaMsg ();
                msg_.SelfManageLifeTime(false);
            }
			if (tryMsgImpl(name, fid, ref msg_, false))
				return msg_;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a submessage.
		/// </summary>
		/// <param name="descriptor"></param>
		/// <param name="valueIfMissing"></param>
		/// <returns></returns>
		public MamaMsg getMsg(
			MamaFieldDescriptor descriptor,
			MamaMsg valueIfMissing)
		{
			return getMsg(null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a MAMA msg field.
		/// The result contains the reusable field object of the
		/// nativeHandle object. Applications calling this method will receive the same
		/// reusable object for repeated calls on same nativeHandle object.
		/// </summary>
		public MamaMsgField getField(
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			IntPtr field = IntPtr.Zero;
			int code = NativeMethods.mamaMsg_getField(nativeHandle, name, fid,ref field);
			CheckResultCode(code);

            if (mField == null)
            {
				//See MamaWrapper constructor - this is fine as ownsThePeer will be set false
                mField = new MamaMsgField (field);
            }
            else
            {
				mField.setNativeHandle(field);

            }
            return mField;
		}

		/// <summary>
		/// Get a MAMA msg field.
		/// The result contains the reusable field object of the
		/// nativeHandle object. Applications calling this method will receive the same
		/// reusable object for repeated calls on same nativeHandle object.
		/// </summary>
		public MamaMsgField getField(
			MamaFieldDescriptor descriptor)
		{
			return getField (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a MAMA msg field.
		/// The result contains the reusable field object of the
		/// nativeHandle object. Applications calling this method will receive the same
		/// reusable object for repeated calls on same nativeHandle object.
		/// </summary>
		public MamaMsgField getField(
			string name,
			ushort fid,
			MamaMsgField valueIfMissing)
		{
			if (mField == null)
            {
                mField = new MamaMsgField ();
				mField.SelfManageLifeTime(false);
            }
			if (tryField (name, fid, ref mField))
				return mField;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a MAMA msg field.
		/// The result contains the reusable field object of the
		/// nativeHandle object. Applications calling this method will receive the same
		/// reusable object for repeated calls on same nativeHandle object.
		/// </summary>
		public MamaMsgField getField(
			MamaFieldDescriptor descriptor,
			MamaMsgField valueIfMissing)
		{
			return getField (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a vector of booleans.
		/// </summary>
		public bool[] getVectorBool(
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorBool(nativeHandle, name, fid,ref array, ref size);
			CheckResultCode(code);

			byte[] bRet = new byte[size];
			bool[] ret = new bool[size];
			Marshal.Copy(array, bRet, 0, (int)size);

			for (int i = 0; i <  ret.Length; i++)
			{
				ret[i] = (bRet[i] != 0);
			}

			return ret;
		}

		/// <summary>
		/// Get a vector of booleans.
		/// </summary>
		public bool[] getVectorBool(
			MamaFieldDescriptor descriptor)
		{
			return getVectorBool (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a vector of booleans.
		/// </summary>
		public bool[] getVectorBool(
			string name,
			ushort fid,
			bool[] valueIfMissing)
		{
			bool[] result = null;
			if (tryVectorBool (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a vector of booleans.
		/// </summary>
		public bool[] getVectorBool(
			MamaFieldDescriptor descriptor,
			bool[] valueIfMissing)
		{
			return getVectorBool (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a vector of chars.
		/// </summary>
		public char[] getVectorChar (
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorChar(nativeHandle, name, fid,ref array, ref size);
			CheckResultCode(code);

			byte[] bRet = new byte[size];
			Marshal.Copy(array, bRet, 0, (int)size);

			char[] ret = new char[size];

			for (int i = 0; i < ret.Length; i++)
			{
				ret[i] = (char)bRet[i];
			}

			return ret;
		}

		/// <summary>
		/// Get a vector of chars.
		/// </summary>
		public char[] getVectorChar(
			MamaFieldDescriptor descriptor)
		{
			return getVectorChar (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a vector of chars.
		/// </summary>
		public char[] getVectorChar(
			string name,
			ushort fid,
			char[] valueIfMissing)
		{
			char[] result = null;
			if (tryVectorChar (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a vector of chars.
		/// </summary>
		public char[] getVectorChar(
			MamaFieldDescriptor descriptor,
			char[] valueIfMissing)
		{
			return getVectorChar (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a vector of signed 8 bit integers.
		/// </summary>
		public sbyte[] getVectorI8 (
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorI8(nativeHandle, name, fid,ref array, ref size);
			CheckResultCode(code);
			byte[] bRet = new byte[size];
			sbyte[] ret = new sbyte[size];
			Marshal.Copy(array, bRet, 0, (int)size);

			for (int i = 0; i < ret.Length; i++)
			{
				ret[i] = (sbyte)bRet[i];
			}

			return ret;
		}

		/// <summary>
		/// Get a vector of signed 8 bit integers.
		/// </summary>
		public sbyte[] getVectorI8(
			MamaFieldDescriptor descriptor)
		{
			return getVectorI8 (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a vector of signed 8 bit integers.
		/// </summary>
		public sbyte[] getVectorI8(
			string name,
			ushort fid,
			sbyte[] valueIfMissing)
		{
			sbyte[] result = null;
			if (tryVectorI8 (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a vector of signed 8 bit integers.
		/// </summary>
		public sbyte[] getVectorI8(
			MamaFieldDescriptor descriptor,
			sbyte[] valueIfMissing)
		{
			return getVectorI8 (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a vector of unsigned 8 bit integers.
		/// </summary>
		public byte[] getVectorU8 (
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorU8(nativeHandle, name, fid,ref array, ref size);
			CheckResultCode(code);
			byte[] ret = new byte[size];
			Marshal.Copy(array, ret, 0, (int)size);

			return ret;
		}

		/// <summary>
		/// Get a vector of unsigned 8 bit integers.
		/// </summary>
		public byte[] getVectorU8(
			MamaFieldDescriptor descriptor)
		{
			return getVectorU8 (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a vector of unsigned 8 bit integers.
		/// </summary>
		public byte[] getVectorU8(
			string name,
			ushort fid,
			byte[] valueIfMissing)
		{
			byte[] result = null;
			if (tryVectorU8 (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a vector of unsigned 8 bit integers.
		/// </summary>
		public byte[] getVectorU8(
			MamaFieldDescriptor descriptor,
			byte[] valueIfMissing)
		{
			return getVectorU8 (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a vector of signed 16 bit integers.
		/// </summary>
		public short[] getVectorI16 (
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorI16(nativeHandle, name, fid,ref array, ref size);
			CheckResultCode(code);
			short[] ret = new short[size];
			Marshal.Copy(array, ret, 0, (int)size);

			return ret;
		}

		/// <summary>
		/// Get a vector of signed 16 bit integers.
		/// </summary>
		public short[] getVectorI16(
			MamaFieldDescriptor descriptor)
		{
			return getVectorI16 (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a vector of signed 16 bit integers.
		/// </summary>
		public short[] getVectorI16(
			string name,
			ushort fid,
			short[] valueIfMissing)
		{
			short[] result = null;
			if (tryVectorI16 (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a vector of signed 16 bit integers.
		/// </summary>
		public short[] getVectorI16(
			MamaFieldDescriptor descriptor,
			short[] valueIfMissing)
		{
			return getVectorI16 (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a vector of unsigned 16 bit integers.
		/// </summary>
		public ushort[] getVectorU16 (
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorU16(nativeHandle, name, fid,ref array, ref size);
			CheckResultCode(code);
			short[] sRet = new short[size];
			ushort[] ret = new ushort[size];
			Marshal.Copy(array, sRet, 0, (int)size);

			for (int i = 0; i < ret.Length; i++)
			{
				ret[i] = (ushort)sRet[i];
			}

			return ret;
		}

		/// <summary>
		/// Get a vector of unsigned 16 bit integers.
		/// </summary>
		public ushort[] getVectorU16(
			MamaFieldDescriptor descriptor)
		{
			return getVectorU16 (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a vector of unsigned 16 bit integers.
		/// </summary>
		public ushort[] getVectorU16(
			string name,
			ushort fid,
			ushort[] valueIfMissing)
		{
			ushort[] result = null;
			if (tryVectorU16 (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a vector of unsigned 16 bit integers.
		/// </summary>
		public ushort[] getVectorU16(
			MamaFieldDescriptor descriptor,
			ushort[] valueIfMissing)
		{
			return getVectorU16 (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a vector of signed 32 bit integers.
		/// </summary>
		public int[] getVectorI32 (
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorI32(nativeHandle, name, fid,ref array, ref size);
			CheckResultCode(code);
			int[] ret = new int[size];
			Marshal.Copy(array, ret, 0, (int)size);

			return ret;
		}

		/// <summary>
		/// Get a vector of signed 32 bit integers.
		/// </summary>
		public int[] getVectorI32(
			MamaFieldDescriptor descriptor)
		{
			return getVectorI32 (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a vector of signed 32 bit integers.
		/// </summary>
		public int[] getVectorI32(
			string name,
			ushort fid,
			int[] valueIfMissing)
		{
			int[] result = null;
			if (tryVectorI32 (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a vector of signed 32 bit integers.
		/// </summary>
		public int[] getVectorI32(
			MamaFieldDescriptor descriptor,
			int[] valueIfMissing)
		{
			return getVectorI32 (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a vector of unsigned 32 bit integers.
		/// </summary>
		public uint[] getVectorU32 (
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorU32(nativeHandle, name, fid,ref array, ref size);
			CheckResultCode(code);
			int[] iRet = new int[size];
			uint[] ret = new uint[size];
			Marshal.Copy(array, iRet, 0, (int)size);

			for (int i = 0; i < ret.Length; i++)
			{
				ret[i] = (uint)iRet[i];
			}

			return ret;
		}

		/// <summary>
		/// Get a vector of unsigned 32 bit integers.
		/// </summary>
		public uint[] getVectorU32(
			MamaFieldDescriptor descriptor)
		{
			return getVectorU32 (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a vector of unsigned 32 bit integers.
		/// </summary>
		public uint[] getVectorU32(
			string name,
			ushort fid,
			uint[] valueIfMissing)
		{
			uint[] result = null;
			if (tryVectorU32 (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a vector of unsigned 32 bit integers.
		/// </summary>
		public uint[] getVectorU32(
			MamaFieldDescriptor descriptor,
			uint[] valueIfMissing)
		{
			return getVectorU32 (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a vector of signed 64 bit integers.
		/// </summary>
		public long[] getVectorI64 (
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorI64(nativeHandle, name, fid,ref array, ref size);
			CheckResultCode(code);
			long[] ret = new long[size];
			Marshal.Copy(array, ret, 0, (int)size);

			return ret;
		}

		/// <summary>
		/// Get a vector of signed 64 bit integers.
		/// </summary>
		public long[] getVectorI64(
			MamaFieldDescriptor descriptor)
		{
			return getVectorI64 (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a vector of signed 64 bit integers.
		/// </summary>
		public long[] getVectorI64(
			string name,
			ushort fid,
			long[] valueIfMissing)
		{
			long[] result = null;
			if (tryVectorI64 (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a vector of signed 64 bit integers.
		/// </summary>
		public long[] getVectorI64(
			MamaFieldDescriptor descriptor,
			long[] valueIfMissing)
		{
			return getVectorI64 (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a vector of unsigned 64 bit integers.
		/// </summary>
		public ulong[] getVectorU64 (
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorU64(nativeHandle, name, fid,ref array, ref size);
			CheckResultCode(code);
			long[] lRet = new long[size];
			ulong[] ret = new ulong[size];
			Marshal.Copy(array, lRet, 0, (int)size);

			for (int i = 0; i < ret.Length; i++)
			{
				ret[i] = (ulong)lRet[i];
			}

			return ret;
		}

		/// <summary>
		/// Get a vector of unsigned 64 bit integers.
		/// </summary>
		public ulong[] getVectorU64(
			MamaFieldDescriptor descriptor)
		{
			return getVectorU64 (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a vector of unsigned 64 bit integers.
		/// </summary>
		public ulong[] getVectorU64(
			string name,
			ushort fid,
			ulong[] valueIfMissing)
		{
			ulong[] result = null;
			if (tryVectorU64 (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a vector of unsigned 64 bit integers.
		/// </summary>
		public ulong[] getVectorU64(
			MamaFieldDescriptor descriptor,
			ulong[] valueIfMissing)
		{
			return getVectorU64 (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a vector of 32 bit floating point numbers.
		/// </summary>
		public float[] getVectorF32 (
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorF32(nativeHandle, name, fid,ref array, ref size);
			CheckResultCode(code);
			float[] ret = new float[size];
			Marshal.Copy(array, ret, 0, (int)size);

			return ret;
		}

		/// <summary>
		/// Get a vector of 32 bit floating point numbers.
		/// </summary>
		public float[] getVectorF32(
			MamaFieldDescriptor descriptor)
		{
			return getVectorF32 (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a vector of 32 bit floating point numbers.
		/// </summary>
		public float[] getVectorF32(
			string name,
			ushort fid,
			float[] valueIfMissing)
		{
			float[] result = null;
			if (tryVectorF32 (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a vector of 32 bit floating point numbers.
		/// </summary>
		public float[] getVectorF32(
			MamaFieldDescriptor descriptor,
			float[] valueIfMissing)
		{
			return getVectorF32 (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a vector of 64 bit floating point numbers.
		/// </summary>
		public double[] getVectorF64 (
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorF64(nativeHandle, name, fid,ref array, ref size);
			CheckResultCode(code);
			double[] ret = new double[size];
			Marshal.Copy(array, ret, 0, (int)size);

			return ret;
		}

		/// <summary>
		/// Get a vector of 64 bit floating point numbers.
		/// </summary>
		public double[] getVectorF64(
			MamaFieldDescriptor descriptor)
		{
			return getVectorF64 (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a vector of 64 bit floating point numbers.
		/// </summary>
		public double[] getVectorF64(
			string name,
			ushort fid,
			double[] valueIfMissing)
		{
			double[] result = null;
			if (tryVectorF64 (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a vector of 64 bit floating point numbers.
		/// </summary>
		public double[] getVectorF64(
			MamaFieldDescriptor descriptor,
			double[] valueIfMissing)
		{
			return getVectorF64 (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a vector of strings (char*).
		/// </summary>
        /// <param name="fid">
        /// The field id, can be 0 if the name is specified and field names are turned on.
        /// </param>
        /// <param name="name">
        /// The field name, can be null or blank if the field Id is supplied.
        /// </param>
        /// <exception cref="MamaException">
        /// Thrown if the vector could not be found.
        /// </exception>
		public string[] getVectorString (
			string name,
			ushort fid)
		{
            // Returns
            string[] ret = null;

            // Try to get the vector
            bool tvs = tryVectorString(name, fid, ref ret);

            // Throw an exception if it could not be found
            if (!tvs)
			{
                throw new MamaException((MamaStatus.mamaStatus)MamaStatus.mamaStatus.MAMA_STATUS_NOT_FOUND);
			}

            return ret;
		}

		/// <summary>
		/// Get a vector of strings (char*).
		/// </summary>
		public string[] getVectorString(
			MamaFieldDescriptor descriptor)
		{
			return getVectorString (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a vector of strings (char*).
		/// </summary>
		public string[] getVectorString(
			string name,
			ushort fid,
			string[] valueIfMissing)
		{
			string[] result = null;
			if (tryVectorString (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a vector of strings (char*).
		/// </summary>
		public string[] getVectorString(
			MamaFieldDescriptor descriptor,
			string[] valueIfMissing)
		{
			return getVectorString (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a vector of timestamps.
		/// </summary>
		public DateTime[] getVectorDateTime (
			string name,
			ushort fid)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorDateTime(nativeHandle, name, fid,ref array, ref size);
			CheckResultCode(code);
			long[] lRet = new long[size];
			DateTime[] dtRet = new DateTime[size];

			for (int i = 0; i < size; i++)
			{
				dtRet[i] = new DateTime(convertFromMamaDateTime ((ulong)lRet[i]));
			}

			return dtRet;
		}

		/// <summary>
		/// Get a vector of timestamps.
		/// </summary>
		public DateTime[] getVectorDateTime(
			MamaFieldDescriptor descriptor)
		{
			return getVectorDateTime (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a vector of timestamps.
		/// </summary>
		public DateTime[] getVectorDateTime(
			string name,
			ushort fid,
			DateTime[] valueIfMissing)
		{
			DateTime[] result = null;
			if (tryVectorDateTime (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a vector of timestamps.
		/// </summary>
		public DateTime[] getVectorDateTime(
			MamaFieldDescriptor descriptor,
			DateTime[] valueIfMissing)
		{
			return getVectorDateTime (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a vector of prices.
		/// </summary>
		public MamaPrice[] getVectorPrice (
			string name,
			ushort fid)
		{
            // Returns
            MamaPrice[] ret = null;

            // Try to get the vector
            bool tvs = tryVectorPrice(name, fid, ref ret);

            // Throw an exception if it could not be found
            if (!tvs)
			{
                throw new MamaException((MamaStatus.mamaStatus)MamaStatus.mamaStatus.MAMA_STATUS_NOT_FOUND);
			}

			return ret;

		}

		/// <summary>
		/// Get a vector of prices.
		/// </summary>
		public MamaPrice[] getVectorPrice(
			MamaFieldDescriptor descriptor)
		{
			return getVectorPrice (null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a vector of prices.
		/// </summary>
		public MamaPrice[] getVectorPrice(
			string name,
			ushort fid,
			MamaPrice[] valueIfMissing)
		{
			MamaPrice[] result = null;
			if (tryVectorPrice (name, fid, ref result))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a vector of prices.
		/// </summary>
		public MamaPrice[] getVectorPrice(
			MamaFieldDescriptor descriptor,
			MamaPrice[] valueIfMissing)
		{
			return getVectorPrice (null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Get a vector of submessages.
		/// </summary>
		public MamaMsg[] getVectorMsg (
			string name,
			ushort fid)
		{
			MamaMsg[] result = null;
			tryVectorMsgImpl(name, fid, ref result, true);
			return result;
        }

		/// <summary>
		/// Get a vector of submessages.
		/// </summary>
		public MamaMsg[] getVectorMsg(
			MamaFieldDescriptor descriptor)
		{
			return getVectorMsg(null, (ushort)descriptor.getFid());
		}

		/// <summary>
		/// Get a vector of submessages.
		/// </summary>
		/// <param name="name"></param>
		/// <param name="fid"></param>
		/// <param name="valueIfMissing"></param>
		/// <returns></returns>
		public MamaMsg[] getVectorMsg(
			string name,
			ushort fid,
			MamaMsg[] valueIfMissing)
		{
			MamaMsg[] result = null;
			if (tryVectorMsgImpl(name, fid, ref result, false))
				return result;
			else
				return valueIfMissing;
		}

		/// <summary>
		/// Get a vector of submessages.
		/// </summary>
		/// <param name="descriptor"></param>
		/// <param name="valueIfMissing"></param>
		/// <returns></returns>
		public MamaMsg[] getVectorMsg(
			MamaFieldDescriptor descriptor,
			MamaMsg[] valueIfMissing)
		{
			return getVectorMsg(null, (ushort)descriptor.getFid(), valueIfMissing);
		}

		/// <summary>
		/// Try to get a bool field.
		/// </summary>
		public bool tryBool (
			string name,
			ushort fid,
			ref bool result)
		{
			EnsurePeerCreated();
			bool val = false;
			int code = NativeMethods.mamaMsg_getBool(nativeHandle, name, fid, ref val);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
			result = val;
			return true;
		}

		/// <summary>
		/// Try to get a bool field.
		/// </summary>
		public bool tryBool (
			MamaFieldDescriptor descriptor,
			ref bool result)
		{
			return tryBool (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a char field.
		/// </summary>
		public bool tryChar(
			string name,
			ushort fid,
			ref char result)
		{
			EnsurePeerCreated();
			char val = '\0';
			int code = NativeMethods.mamaMsg_getChar(nativeHandle, name, fid,ref val);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
			result = val;
			return true;
		}

		/// <summary>
		/// Try to get a char field.
		/// </summary>
		public bool tryChar(
			MamaFieldDescriptor descriptor,
			ref char result)
		{
			return tryChar (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a I8, signed 8 bit integer, field.
		/// </summary>
		public bool tryI8(
			string name,
			ushort fid,
			ref sbyte result)
		{
			EnsurePeerCreated();
			sbyte val = 0;
			int code = NativeMethods.mamaMsg_getI8(nativeHandle, name, fid,ref val);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
			result = val;
			return true;
		}

		/// <summary>
		/// Try to get a I8, signed 8 bit integer, field.
		/// </summary>
		public bool tryI8(
			MamaFieldDescriptor descriptor,
			ref sbyte result)
		{
			return tryI8 (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a I8, signed 8 bit integer, field.
		/// </summary>
		public bool tryU8(
			string name,
			ushort fid,
			ref byte result)
		{
			EnsurePeerCreated();
			byte val = 0;
			int code = NativeMethods.mamaMsg_getU8(nativeHandle, name, fid,ref val);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
			result = val;
			return true;
		}

		/// <summary>
		/// Try to get a I8, signed 8 bit integer, field.
		/// </summary>
		public bool tryU8(
			MamaFieldDescriptor descriptor,
			ref byte result)
		{
			return tryU8 (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a I16, signed 16 bit integer, field.
		/// </summary>
		public bool tryI16(
			string name,
			ushort fid,
			ref short result)
		{
			EnsurePeerCreated();
			short val = 0;
			int code = NativeMethods.mamaMsg_getI16(nativeHandle, name, fid,ref val);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
			result = val;
			return true;
		}

		/// <summary>
		/// Try to get a I16, signed 16 bit integer, field.
		/// </summary>
		public bool tryI16(
			MamaFieldDescriptor descriptor,
			ref short result)
		{
			return tryI16 (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a U16, unsigned 16 bit integer, field.
		/// </summary>
		public bool tryU16(
			string name,
			ushort fid,
			ref ushort result)
		{
			EnsurePeerCreated();
			ushort val = 0;
			int code = NativeMethods.mamaMsg_getU16(nativeHandle, name, fid,ref val);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
			result = val;
			return true;
		}

		/// <summary>
		/// Try to get a U16, unsigned 16 bit integer, field.
		/// </summary>
		public bool tryU16(
			MamaFieldDescriptor descriptor,
			ref ushort result)
		{
			return tryU16 (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a U32, unsigned 32 bit integer, field.
		/// </summary>
		public bool tryU32(
			string name,
			ushort fid,
			ref uint result)
		{
			EnsurePeerCreated();
			uint val = 0;
			int code = NativeMethods.mamaMsg_getU32(nativeHandle, name, fid,ref val);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
			result = val;
			return true;
		}

		/// <summary>
		/// Try to get a U32, unsigned 32 bit integer, field.
		/// </summary>
		public bool tryU32(
			MamaFieldDescriptor descriptor,
			ref uint result)
		{
			return tryU32 (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a I32, signed 32 bit integer, field.
		/// </summary>
		/// <param name="name"></param>
		/// <param name="fid"></param>
		/// <param name="result"></param>
		/// <returns></returns>
		public bool tryI32(
			string name,
			ushort fid,
			ref int result)
		{
			return tryI32Impl(name, fid, ref result, false);
		}

		/// <summary>
		/// Try to get a I32, signed 32 bit integer, field.
		/// </summary>
		/// <param name="descriptor"></param>
		/// <param name="result"></param>
		/// <returns></returns>
		public bool tryI32(
			MamaFieldDescriptor descriptor,
			ref int result)
		{
			return tryI32(null, (ushort)descriptor.getFid(), ref result);
		}

    /// <overloads>Try to get a signed 64-bit integer field</overloads>
		public bool tryI64(
			string name,
			ushort fid,
			ref long result)
		{
			return tryI64Impl(name, fid, ref result, false);
		}

		public bool tryI64(
			MamaFieldDescriptor descriptor,
			ref long result)
		{
			return tryI64(null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a U64, unsigned 64 bit integer, field.
		/// </summary>
		public bool tryU64(
			string name,
			ushort fid,
			ref ulong result)
		{
			EnsurePeerCreated();
			ulong val = 0;
			int code = NativeMethods.mamaMsg_getU64(nativeHandle, name, fid,ref val);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
			result = val;
			return true;
		}

		/// <summary>
		/// Try to get a U64, unsigned 64 bit integer, field.
		/// </summary>
		public bool tryU64(
			MamaFieldDescriptor descriptor,
			ref ulong result)
		{
			return tryU64 (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a float (f32) field.
		/// </summary>
		public bool tryF32(
			string name,
			ushort fid,
			ref float result)
		{
			EnsurePeerCreated();
			float val = 0;
			int code = NativeMethods.mamaMsg_getF32(nativeHandle, name, fid,ref val);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
			result = val;
			return true;
		}

		/// <summary>
		/// Try to get a float (f32) field.
		/// </summary>
		public bool tryF32(
			MamaFieldDescriptor descriptor,
			ref float result)
		{
			return tryF32 (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a f64 field.
		/// </summary>
		public bool tryF64(
			string name,
			ushort fid,
			ref double result)
		{
			EnsurePeerCreated();
			double val = 0;
			int code = NativeMethods.mamaMsg_getF64(nativeHandle, name, fid,ref val);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
			result = val;
			return true;
		}

		/// <summary>
		/// Try to get a f64 field.
		/// </summary>
		public bool tryF64(
			MamaFieldDescriptor descriptor,
			ref double result)
		{
			return tryF64 (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a string field.
		/// </summary>
		public bool tryString(
			string name,
			ushort fid,
			ref string result)
		{
            EnsurePeerCreated();
            IntPtr ret = IntPtr.Zero;
			int code = NativeMethods.mamaMsg_getString(nativeHandle, name, fid,ref ret);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
            result = Marshal.PtrToStringAnsi(ret);
			return true;
		}

		/// <summary>
		/// Try to get a string field.
		/// </summary>
		public bool tryString(
			MamaFieldDescriptor descriptor,
			ref string result)
		{
			return tryString (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get an opaque field.
		/// </summary>
		public bool tryOpaque(
			string name,
			ushort fid,
			ref byte[] result)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getOpaque(nativeHandle, name, fid,ref array, ref size);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
			byte[] val = new byte[size];
			Marshal.Copy(array, val, 0, (int)size);
			result = val;
			return true;
		}

		/// <summary>
		/// Try to get an opaque field.
		/// </summary>
		public bool tryOpaque(
			MamaFieldDescriptor descriptor,
			ref byte[] result)
		{
			return tryOpaque (null, (ushort)descriptor.getFid(), ref result);
		}


        /// <overloads>
        /// Try to get a DateTime object
        /// </overloads>
		public bool tryDateTime(
			string name,
			ushort fid,
			ref DateTime result)
		{
			return tryDateTimeImpl(name, fid, ref result, false);
		}

		public bool tryDateTime(
			MamaFieldDescriptor descriptor,
			ref DateTime result)
		{
			return tryDateTime(null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a MAMA price field.
		/// </summary>
		public bool tryPrice(
			string name,
			ushort fid,
			ref MamaPrice result)
		{
			EnsurePeerCreated();

            if (result == null)
                result = new MamaPrice();

			int code = NativeMethods.mamaMsg_getPrice(nativeHandle, name, fid, result.NativeHandle);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}

			return true;
		}

        		/// <summary>
		/// Try to get a MAMA price field.
		/// </summary>
		public bool tryPrice(
			string name,
			ushort fid,
			MamaPrice result)
		{
			EnsurePeerCreated();

			int code = NativeMethods.mamaMsg_getPrice(nativeHandle, name, fid, result.NativeHandle);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}

			return true;
		}

		/// <summary>
		/// Try to get a MAMA price field.
		/// </summary>
		public bool tryPrice(
			MamaFieldDescriptor descriptor,
			ref MamaPrice result)
		{
			return tryPrice (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a submessage.
		/// </summary>
		public bool tryMsg(
			string name,
			ushort fid,
			ref MamaMsg result)
		{
			EnsurePeerCreated();


            IntPtr myp = IntPtr.Zero;
			int code = NativeMethods.mamaMsg_getMsg(nativeHandle, name, fid, ref myp);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}

            if (result == null)
                result = new MamaMsg();

            result.setNativeHandle(myp);
			return true;
		}

		/// <summary>
		/// Try to get a submessage.
		/// </summary>
		public bool tryMsg(
			MamaFieldDescriptor descriptor,
			ref MamaMsg result)
		{
			return tryMsg (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a vector of booleans.
		/// </summary>
		public bool tryVectorBool(
			string name,
			ushort fid,
			ref bool[] result)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorBool(nativeHandle, name, fid,ref array, ref size);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}

			byte[] bVal = new byte[size];
			bool[] val = new bool[size];
			Marshal.Copy(array, bVal, 0, (int)size);

			for (int i = 0; i <  val.Length; i++)
			{
				val[i] = (bVal[i] != 0);
			}

			result = val;
			return true;
		}

		/// <summary>
		/// Try to get a vector of booleans.
		/// </summary>
		public bool tryVectorBool(
			MamaFieldDescriptor descriptor,
			ref bool[] result)
		{
			return tryVectorBool (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a vector of chars.
		/// </summary>
		public bool tryVectorChar (
			string name,
			ushort fid,
			ref char[] result)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorChar(nativeHandle, name, fid,ref array, ref size);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}

			byte[] bRet = new byte[size];
			Marshal.Copy(array, bRet, 0, (int)size);

			char[] val = new char[size];

			for (int i = 0; i < val.Length; i++)
			{
				val[i] = (char)bRet[i];
			}

			result = val;
			return true;
		}

		/// <summary>
		/// Try to get a vector of chars.
		/// </summary>
		public bool tryVectorChar (
			MamaFieldDescriptor descriptor,
			ref char[] result)
		{
			return tryVectorChar (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a vector of signed 8 bit integers.
		/// </summary>
		public bool tryVectorI8 (
			string name,
			ushort fid,
			ref sbyte[] result)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorI8(nativeHandle, name, fid,ref array, ref size);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
			byte[] bVal = new byte[size];
			sbyte[] val = new sbyte[size];
			Marshal.Copy(array, bVal, 0, (int)size);

			for (int i = 0; i < val.Length; i++)
			{
				val[i] = (sbyte)bVal[i];
			}

			result = val;
			return true;
		}

		/// <summary>
		/// Try to get a vector of signed 8 bit integers.
		/// </summary>
		public bool tryVectorI8 (
			MamaFieldDescriptor descriptor,
			ref sbyte[] result)
		{
			return tryVectorI8 (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a vector of unsigned 8 bit integers.
		/// </summary>
		public bool tryVectorU8 (
			string name,
			ushort fid,
			ref byte[] result)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorU8(nativeHandle, name, fid,ref array, ref size);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
			byte[] val = new byte[size];
			Marshal.Copy(array, val, 0, (int)size);

			result = val;
			return true;
		}

		/// <summary>
		/// Try to get a vector of unsigned 8 bit integers.
		/// </summary>
		public bool tryVectorU8 (
			MamaFieldDescriptor descriptor,
			ref byte[] result)
		{
			return tryVectorU8 (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a vector of signed 16 bit integers.
		/// </summary>
		public bool tryVectorI16 (
			string name,
			ushort fid,
			ref short[] result)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorI16(nativeHandle, name, fid,ref array, ref size);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
			short[] val = new short[size];
			Marshal.Copy(array, val, 0, (int)size);

			result = val;
			return true;
		}

		/// <summary>
		/// Try to get a vector of signed 16 bit integers.
		/// </summary>
		public bool tryVectorI16 (
			MamaFieldDescriptor descriptor,
			ref short[] result)
		{
			return tryVectorI16 (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a vector of unsigned 16 bit integers.
		/// </summary>
		public bool tryVectorU16 (
			string name,
			ushort fid,
			ref ushort[] result)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorU16(nativeHandle, name, fid,ref array, ref size);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
			short[] sVal = new short[size];
			ushort[] val = new ushort[size];
			Marshal.Copy(array, sVal, 0, (int)size);

			for (int i = 0; i < val.Length; i++)
			{
				val[i] = (ushort)sVal[i];
			}

			result = val;
			return true;
		}

		/// <summary>
		/// Try to get a vector of unsigned 16 bit integers.
		/// </summary>
		public bool tryVectorU16 (
			MamaFieldDescriptor descriptor,
			ref ushort[] result)
		{
			return tryVectorU16 (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a vector of signed 32 bit integers.
		/// </summary>
		public bool tryVectorI32 (
			string name,
			ushort fid,
			ref int[] result)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorI32(nativeHandle, name, fid,ref array, ref size);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
			int[] val = new int[size];
			Marshal.Copy(array, val, 0, (int)size);

			result = val;
			return true;
		}

		/// <summary>
		/// Try to get a vector of signed 32 bit integers.
		/// </summary>
		public bool tryVectorI32 (
			MamaFieldDescriptor descriptor,
			ref int[] result)
		{
			return tryVectorI32 (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a vector of unsigned 32 bit integers.
		/// </summary>
		public bool tryVectorU32 (
			string name,
			ushort fid,
			ref uint[] result)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorU32(nativeHandle, name, fid,ref array, ref size);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
			int[] iVal = new int[size];
			uint[] val = new uint[size];
			Marshal.Copy(array, iVal, 0, (int)size);

			for (int i = 0; i < val.Length; i++)
			{
				val[i] = (uint)iVal[i];
			}

			result = val;
			return true;
		}

		/// <summary>
		/// Try to get a vector of unsigned 32 bit integers.
		/// </summary>
		public bool tryVectorU32 (
			MamaFieldDescriptor descriptor,
			ref uint[] result)
		{
			return tryVectorU32 (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try get a vector of signed 64 bit integers.
		/// </summary>
		public bool tryVectorI64 (
			string name,
			ushort fid,
			ref long[] result)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorI64(nativeHandle, name, fid,ref array, ref size);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
			long[] val = new long[size];
			Marshal.Copy(array, val, 0, (int)size);

			result = val;
			return true;
		}

		/// <summary>
		/// Try get a vector of signed 64 bit integers.
		/// </summary>
		public bool tryVectorI64 (
			MamaFieldDescriptor descriptor,
			ref long[] result)
		{
			return tryVectorI64 (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a vector of unsigned 64 bit integers.
		/// </summary>
		public bool tryVectorU64 (
			string name,
			ushort fid,
			ref ulong[] result)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorU64(nativeHandle, name, fid,ref array, ref size);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
			long[] lVal = new long[size];
			ulong[] val = new ulong[size];
			Marshal.Copy(array, lVal, 0, (int)size);

			for (int i = 0; i < val.Length; i++)
			{
				val[i] = (ulong)lVal[i];
			}

			result = val;
			return true;
		}

		/// <summary>
		/// Try to get a vector of unsigned 64 bit integers.
		/// </summary>
		public bool tryVectorU64 (
			MamaFieldDescriptor descriptor,
			ref ulong[] result)
		{
			return tryVectorU64 (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a vector of 32 bit floating point numbers.
		/// </summary>
		public bool tryVectorF32 (
			string name,
			ushort fid,
			ref float[] result)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorF32(nativeHandle, name, fid,ref array, ref size);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
			float[] val = new float[size];
			Marshal.Copy(array, val, 0, (int)size);

			result = val;
			return true;
		}

		/// <summary>
		/// Try to get a vector of 32 bit floating point numbers.
		/// </summary>
		public bool tryVectorF32 (
			MamaFieldDescriptor descriptor,
			ref float[] result)
		{
			return tryVectorF32 (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a vector of 64 bit floating point numbers.
		/// </summary>
		public bool tryVectorF64 (
			string name,
			ushort fid,
			ref double[] result)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorF64(nativeHandle, name, fid,ref array, ref size);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
			double[] val = new double[size];
			Marshal.Copy(array, val, 0, (int)size);

			result = val;
			return true;
		}

		/// <summary>
		/// Try to get a vector of 64 bit floating point numbers.
		/// </summary>
		public bool tryVectorF64 (
			MamaFieldDescriptor descriptor,
			ref double[] result)
		{
			return tryVectorF64 (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a vector of strings (char*).
		/// </summary>
		public bool tryVectorString (
			MamaFieldDescriptor descriptor,
			ref string[] result)
		{
			return tryVectorString (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a vector of timestamps.
		/// </summary>
		public bool tryVectorDateTime (
			string name,
			ushort fid,
			ref DateTime[] result)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorDateTime(nativeHandle, name, fid,ref array, ref size);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}
			long[] lVal = new long[size];
			DateTime[] dtVal = new DateTime[size];

			for (int i = 0; i < size; i++)
			{
				dtVal[i] = new DateTime(convertFromMamaDateTime ((ulong)lVal[i]));
			}

			result = dtVal;
			return true;
		}

		/// <summary>
		/// Try to get a vector of timestamps.
		/// </summary>
		public bool tryVectorDateTime (
			MamaFieldDescriptor descriptor,
			ref DateTime[] result)
		{
			return tryVectorDateTime (null, (ushort)descriptor.getFid(), ref result);
		}

		/// <summary>
		/// Try to get a vector of prices.
		/// </summary>
		public bool tryVectorPrice (
			MamaFieldDescriptor descriptor,
			ref MamaPrice[] result)
		{
			return tryVectorPrice (null, (ushort)descriptor.getFid(), ref result);
		}

        /// <overloads>
        /// Try to get a vector of messages
        /// </overloads>
		public bool tryVectorMsg(
			string name,
			ushort fid,
			ref MamaMsg[] result)
		{
			return tryVectorMsgImpl(name, fid, ref result, false);
		}

		public bool tryVectorMsg(
			MamaFieldDescriptor descriptor,
			ref MamaMsg[] result)
		{
			return tryVectorMsgImpl (null, (ushort)descriptor.getFid(), ref result, false);
		}

		/// <summary>
		/// Try to get a MAMA msg field.
		/// The result contains the reusable field object of the
		/// nativeHandle object. Applications calling this method will receive the same
		/// reusable object for repeated calls on same nativeHandle object.
		/// </summary>
		public bool tryField(
			string name,
			ushort fid,
			ref MamaMsgField result)
		{
			EnsurePeerCreated();

            IntPtr myP = IntPtr.Zero;
			int code = NativeMethods.mamaMsg_getField(nativeHandle, name, fid, ref myP);
			if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				return false;
			}

			if (mField == null)
			{
				mField = new MamaMsgField();
				mField.SelfManageLifeTime(false);
			}

			mField.setNativeHandle(myP);
            result = mField;
			return true;
		}

		/// <summary>
		/// Try to get a MAMA msg field.
		/// The result contains the reusable field object of the
		/// nativeHandle object. Applications calling this method will receive the same
		/// reusable object for repeated calls on same nativeHandle object.
		/// </summary>
		public bool tryField(
			MamaFieldDescriptor descriptor,
			ref MamaMsgField result)
		{
			return tryField (null, (ushort)descriptor.getFid(), ref result);
		}

        /// <overloads>Try to get a signed 32-bit integer field</overloads>
		public bool tryI32(
			string name,
			ushort fid,
			ref NullableInt result)
		{
			int val = 0;
			bool ret = tryI32(name, fid, ref val);
			if (ret)
			{
				result.Value = val;
			}
			return ret;
		}

		public bool tryI32(
			MamaFieldDescriptor descriptor,
			ref NullableInt result)
		{
			return tryI32(null, (ushort)descriptor.getFid(), ref result);
		}

        /// <overloads>Try to get a signed 64-bit integer field</overloads>
		public bool tryI64(
			string name,
			ushort fid,
			ref NullableLong result)
		{
			long val = 0;
			bool ret = tryI64(name, fid, ref val);
			if (ret)
			{
				result.Value = val;
			}
			return ret;
		}

		public bool tryI64(
			MamaFieldDescriptor descriptor,
			ref NullableLong result)
		{
			return tryI64(null, (ushort)descriptor.getFid(), ref result);
		}

		private bool tryI32Impl(
			string name,
			ushort fid,
			ref int result,
			bool throwOnError)
		{
			EnsurePeerCreated();
			int ret = 0;
			int code = NativeMethods.mamaMsg_getI32(nativeHandle, name, fid, ref ret);
			if ((MamaStatus.mamaStatus)code != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				if (throwOnError)
				{
					CheckResultCode(code);
				}
				else
				{
					return false;
				}
			}
			result = ret;
			return true;
		}

		private bool tryI64Impl(
			string name,
			ushort fid,
			ref long result,
			bool throwOnError)
		{
			EnsurePeerCreated();
			long ret = 0;
			int code = NativeMethods.mamaMsg_getI64(nativeHandle, name, fid, ref ret);
			if ((MamaStatus.mamaStatus)code != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				if (throwOnError)
				{
					CheckResultCode(code);
				}
				else
				{
					return false;
				}
			}
			result = ret;
			return true;
		}

		private bool tryDateTimeImpl(
			string name,
			ushort fid,
			ref DateTime result,
			bool throwOnError)
		{
			EnsurePeerCreated();
			ulong dateTimeMsec = 0;
			int code = NativeMethods.mamaMsg_getDateTimeMSec(nativeHandle, name, fid, ref dateTimeMsec);
			if ((MamaStatus.mamaStatus)code != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				if (throwOnError)
				{
					CheckResultCode(code);
				}
				else
				{
					return false;
				}
			}

            long ret = ((long)dateTimeMsec * TimeSpan.TicksPerMillisecond) + 621355968000000000;
			result = new DateTime(ret);
			return true;
		}

		private bool tryMsgImpl(
			string name,
			ushort fid,
			ref MamaMsg result,
			bool throwOnError)
		{
			EnsurePeerCreated();

            IntPtr myP = IntPtr.Zero;

			int code = NativeMethods.mamaMsg_getMsg(nativeHandle, name, fid, ref myP);

            if (throwOnError)
            {
                CheckResultCode(code);
            }
            else
            {
                if (CheckResultCodeIgnoreNotFound(code) != MamaStatus.mamaStatus.MAMA_STATUS_OK)
                    return false;
            }


            if (result == null)
                result = new MamaMsg();

            result.setNativeHandle(myP);

			return true;
		}

		private bool tryVectorMsgImpl(
			string name,
			ushort fid,
			ref MamaMsg[] result,
			bool throwOnError)
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsg_getVectorMsg(nativeHandle,name,fid,ref array, ref size);
			if ((MamaStatus.mamaStatus)code != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				if (throwOnError)
				{
					CheckResultCode(code);
				}
				else
				{
					return false;
				}
			}

            MamaMsg[] ret;
            if (size == 1)
            {
                if (tempMsgVector == null)
                {
                    tempMsgVector = new MamaMsg[1];
                    tempMsgVector[0] = new MamaMsg();
                    tempMsgVector[0].SelfManageLifeTime(false);
                }

                 ret = tempMsgVector;
                 ret[0].setNativeHandle(Marshal.ReadIntPtr(array));
            }
            else
            {
                ret = new MamaMsg[size];

                for (int i = 0 ; i < size; i++)
                {
                    ret[i] = new MamaMsg(Marshal.ReadIntPtr(array, i * Marshal.SizeOf(typeof(IntPtr))));
                }
            }

			result = ret;
			return true;
		}



		/// <summary>
        /// Returns the total number of fields in the message. Sub-messages count as
        /// a single field.
        /// </summary>
        public int getNumFields()
        {
			EnsurePeerCreated();
            int num = 0;
			int code = NativeMethods.mamaMsg_getNumFields(nativeHandle,ref num);
			CheckResultCode(code);
            return num;
        }

        /// <summary>
        /// Return a string representation the message.
        /// </summary>
        public override string ToString()
        {
			EnsurePeerCreated();
            string strRet =  Marshal.PtrToStringAnsi(NativeMethods.mamaMsg_toString(nativeHandle));
            string val;
            val = string.Copy(strRet);
            return val;
        }

        /// <summary>
        /// Get the entitle code for this message. The result defaults to 0 (no
        /// entitlement) if the field is not present.
        /// </summary>
        public int getEntitleCode()
        {
			EnsurePeerCreated();
            int result = 0;
			int code = NativeMethods.mamaMsg_getEntitleCode(nativeHandle, ref result);
			CheckResultCode(code);
            return result;
        }

        /// <summary>
        /// Return the sequence
        /// </summary>
        /// <returns></returns>
        public uint getSeqNum()
        {
			EnsurePeerCreated();
            uint seqNum = 0;
			int code = NativeMethods.mamaMsg_getSeqNum(nativeHandle,ref seqNum);
			CheckResultCode(code);
            return seqNum;
        }

        /// <summary>
        /// Extract the type from the supplied message.
        /// </summary>
        public mamaMsgType typeForMsg ()
        {
			EnsurePeerCreated();
            return (mamaMsgType)NativeMethods.mamaMsgType_typeForMsg (nativeHandle);
        }



        /// <summary>
        /// Return the string for this message
        /// </summary>
        public string stringForMsg ()
        {
			EnsurePeerCreated();
            return Marshal.PtrToStringAnsi(NativeMethods.mamaMsgType_stringForMsg(nativeHandle));
        }

        /// <summary>
        /// Return the type name.
        /// </summary>
        public string stringForType (mamaMsgType type)
        {
			EnsurePeerCreated();
            return Marshal.PtrToStringAnsi(NativeMethods.mamaMsgType_stringForType((int)type));
        }

        /// <summary>
        /// Return the status string
        /// </summary>
        public string stringForStatus (mamaMsgStatus type)
        {
			EnsurePeerCreated();
            return Marshal.PtrToStringAnsi(NativeMethods.mamaStatus_stringForStatus((int)type));
        }

        /// <summary>
        /// Return the field value as string
        /// </summary>
        public string getFieldAsString (
			string name,
            ushort fid)
        {
			EnsurePeerCreated();
            StringBuilder val = new StringBuilder(255);
			int code = NativeMethods.mamaMsg_getFieldAsString(nativeHandle, name, fid,val,255);
			CheckResultCode(code);
            return val.ToString();
        }

		public bool isFromInbox
		{
			get
			{
				int val = NativeMethods.mamaMsg_isFromInbox(nativeHandle);
				return val != 0;
			}
		}

		public void begin (ref MamaMsgIterator iterator)
		{
			NativeMethods.mamaMsgIterator_associate (iterator.NativeHandle, nativeHandle);

            IntPtr tempfield = NativeMethods.mamaMsgIterator_begin (iterator.NativeHandle);
            iterator.SetField (tempfield);
            iterator++;
		}

		public void end (ref MamaMsgIterator iterator)
		{
			IntPtr field = NativeMethods.mamaMsgIterator_end (iterator.NativeHandle);
			iterator.SetField (field);
		}

        private MamaMsgField mField = null;
        private MamaMsg[] tempMsgVector = null;
        private MamaPrice price_ = null;
        private MamaMsg msg_ = null;
		private CallbackForwarder forwarder = null;
        private CallbackForwarder.MamaMessageIteratorDelegate callback = null;
        private IntPtr buffer = IntPtr.Zero;
        private byte[] myByteBuffer = null;
		#region Implementation details
		private class CallbackForwarder
		{
			public delegate void MamaMessageIteratorDelegate(IntPtr msg, IntPtr field, IntPtr closure);

			public CallbackForwarder(
				MamaMsg sender,
				MamaMsgFieldIterator callback,
				object closure)
			{
				this.mSender = sender;
				this.mCallback = callback;
				this.mClosure = closure;
			}

			internal void OnField(IntPtr msg, IntPtr field, IntPtr closure)
			{
				if (mCallback != null)
				{
                    if (mField == null)
                    {
                        mField = new MamaMsgField(field);
                    }
                    else
                    {
                        mField.setNativeHandle(field);
                    }
					mCallback.onField(mSender, mField, mClosure);
				}
			}


			public MamaMsgFieldIterator mCallback;
			private MamaMsg mSender;
            private MamaMsgField mField;
			public object mClosure;

		}

		private struct NativeMethods
		{
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_iterateFields(IntPtr msg,
				CallbackForwarder.MamaMessageIteratorDelegate callback,
				IntPtr dict,
				IntPtr closure);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_create(ref IntPtr mamaMsg);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_createForPayload(ref IntPtr mamaMsg, mamaPayloadType id);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_createForPayload(ref IntPtr mamaMsg, char id);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaMsg_createForPayloadBridge(ref IntPtr mamaMsg, IntPtr mamaPayloadBridge);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_copy (IntPtr mamaMsgSrc,
				ref IntPtr mamaMsgCopy );
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_clear (IntPtr mamaMsg);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_destroy (IntPtr mamaMsg);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getByteSize (IntPtr mamaMsg,
				ref int size);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addBool(IntPtr mamaMsg,
				string name,
				ushort fid,
				bool val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addChar(IntPtr mamaMsg,
				string name,
				ushort fid,
				char val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addI8(IntPtr mamaMsg,
				string name,
				ushort fid,
				sbyte val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addU8(IntPtr mamaMsg,
				string name,
				ushort fid,
				byte val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addI16(IntPtr mamaMsg,
				string name,
				ushort fid,
				short val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addU16(IntPtr mamaMsg,
				string name,
				ushort fid,
				ushort val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addI32(IntPtr mamaMsg,
				string name,
				ushort fid,
				int val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addU32(IntPtr mamaMsg,
				string name,
				ushort fid,
				uint val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addI64(IntPtr mamaMsg,
				string name,
				ushort fid,
				long val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addU64(IntPtr mamaMsg,
				string name,
				ushort fid,
				ulong val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addF32(IntPtr mamaMsg,
				string name,
				ushort fid,
				float val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addF64(IntPtr mamaMsg,
				string name,
				ushort fid,
				double val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addString(IntPtr mamaMsg,
				string name,
				ushort fid,
				string val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addOpaque (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int size);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addDateTime (IntPtr mamaMsg,
				string name,
				ushort fid,
				ref ulong val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addPrice (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaMsg_addMsg(IntPtr mamaMsg,
                string name,
                ushort fid,
                IntPtr val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addVectorBool (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addVectorChar (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addVectorI8 (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addVectorU8 (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addVectorI16 (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addVectorU16 (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addVectorI32 (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addVectorU32 (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addVectorI64 (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addVectorU64 (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addVectorF32 (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addVectorF64 (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addVectorString (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addVectorMsg(IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addVectorDateTime (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_addVectorPrice (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateBool(IntPtr mamaMsg,
				string name,
				ushort fid,
				bool val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateChar(IntPtr mamaMsg,
				string name,
				ushort fid,
				char val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateI8(IntPtr mamaMsg,
				string name,
				ushort fid,
				sbyte val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateU8(IntPtr mamaMsg,
				string name,
				ushort fid,
				byte val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateI16(IntPtr mamaMsg,
				string name,
				ushort fid,
				short val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateU16(IntPtr mamaMsg,
				string name,
				ushort fid,
				ushort val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateI32(IntPtr mamaMsg,
				string name,
				ushort fid,
				int val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateU32(IntPtr mamaMsg,
				string name,
				ushort fid,
				uint val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateI64(IntPtr mamaMsg,
				string name,
				ushort fid,
				long val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateU64(IntPtr mamaMsg,
				string name,
				ushort fid,
				ulong val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateF32(IntPtr mamaMsg,
				string name,
				ushort fid,
				float val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateF64(IntPtr mamaMsg,
				string name,
				ushort fid,
				double val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateString(IntPtr mamaMsg,
				string name,
				ushort fid,
				string val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateOpaque(IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int size);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateDateTime(IntPtr mamaMsg,
				string name,
				ushort fid,
				ref ulong val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updatePrice(IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateVectorBool (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateVectorChar (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateVectorI8 (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateVectorU8 (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateVectorI16 (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateVectorU16 (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateVectorI32 (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateVectorU32 (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateVectorI64 (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateVectorU64 (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateVectorF32 (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateVectorF64 (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateVectorString (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateVectorDateTime (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateVectorPrice (IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_updateVectorMsg(IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr val,
				int numElements);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getBool(IntPtr mamaMsg,
				string name,
				ushort fid,
				ref bool result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getChar(IntPtr mamaMsg,
				string name,
				ushort fid,
				ref char result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getI8(IntPtr mamaMsg,
				string name,
				ushort fid,
				ref sbyte result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getU8(IntPtr mamaMsg,
				string name,
				ushort fid,
				ref byte result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getI16(IntPtr mamaMsg,
				string name,
				ushort fid,
				ref short result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getU16(IntPtr mamaMsg,
				string name,
				ushort fid,
				ref ushort result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getI32(IntPtr mamaMsg,
				string name,
				ushort fid,
				ref int result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getU32(IntPtr mamaMsg,
				string name,
				ushort fid,
				ref uint result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getI64(IntPtr mamaMsg,
				string name,
				ushort fid,
				ref long result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getU64(IntPtr mamaMsg,
				string name,
				ushort fid,
				ref ulong result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getF32(IntPtr mamaMsg,
				string name,
				ushort fid,
				ref float result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getF64(IntPtr mamaMsg,
				string name,
				ushort fid,
				ref double result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getString(IntPtr mamaMsg,
				string name,
				ushort fid,
                ref IntPtr result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getOpaque(IntPtr mamaMsg,
				string name,
				ushort fid,
				ref IntPtr result,
				ref uint size);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getField(IntPtr mamaMsg,
				string name,
				ushort fid,
				ref IntPtr result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getDateTime(IntPtr mamaMsg,
				string name,
				ushort fid,
				ref ulong result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getDateTimeMSec(IntPtr mamaMsg,
				string name,
				ushort fid,
				ref ulong result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getPrice(IntPtr mamaMsg,
				string name,
				ushort fid,
				IntPtr result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getMsg (IntPtr mamaMsg,
				string name,
				ushort fid,
				ref IntPtr result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getVectorBool (IntPtr mamaMsg,
				string name,
				ushort fid,
				ref IntPtr result,
				ref uint resultLen);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getVectorChar (IntPtr mamaMsg,
				string name,
				ushort fid,
				ref IntPtr result,
				ref uint resultLen);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getVectorI8 (IntPtr mamaMsg,
				string name,
				ushort fid,
				ref IntPtr result,
				ref uint resultLen);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getVectorU8 (IntPtr mamaMsg,
				string name,
				ushort fid,
				ref IntPtr result,
				ref uint resultLen);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getVectorI16 (IntPtr mamaMsg,
				string name,
				ushort fid,
				ref IntPtr result,
				ref uint resultLen);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getVectorU16 (IntPtr mamaMsg,
				string name,
				ushort fid,
				ref IntPtr result,
				ref uint resultLen);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getVectorI32 (IntPtr mamaMsg,
				string name,
				ushort fid,
				ref IntPtr result,
				ref uint resultLen);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getVectorU32 (IntPtr mamaMsg,
				string name,
				ushort fid,
				ref IntPtr result,
				ref uint resultLen);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getVectorI64 (IntPtr mamaMsg,
				string name,
				ushort fid,
				ref IntPtr result,
				ref uint resultLen);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getVectorU64 (IntPtr mamaMsg,
				string name,
				ushort fid,
				ref IntPtr result,
				ref uint resultLen);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getVectorF32 (IntPtr mamaMsg,
				string name,
				ushort fid,
				ref IntPtr result,
				ref uint resultLen);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getVectorF64 (IntPtr mamaMsg,
				string name,
				ushort fid,
				ref IntPtr result,
				ref uint resultLen);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getVectorString (IntPtr mamaMsg,
				string name,
				ushort fid,
				ref IntPtr result,
				ref uint resultLen);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getVectorDateTime (IntPtr mamaMsg,
				string name,
				ushort fid,
				ref IntPtr result,
				ref uint resultLen);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getVectorPrice (IntPtr mamaMsg,
				string name,
				ushort fid,
				ref IntPtr result,
				ref uint resultLen);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getVectorMsg (IntPtr mamaMsg,
				string name,
				ushort fid,
				ref IntPtr result,
				ref uint resultLen);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getNumFields(IntPtr mamaMsg,
				ref int numFields);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern IntPtr mamaMsg_toString(IntPtr mamaMsg);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getEntitleCode(IntPtr mamaMsg,
				ref int code);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaMsg_getPayloadType(IntPtr mamaMsg,
                ref mamaPayloadType payload);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getSeqNum(IntPtr mamaMsg,
				ref uint seqNum);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgType_typeForMsg (IntPtr mamaMsg);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern IntPtr mamaMsgType_stringForMsg (IntPtr mamaMsg);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern IntPtr mamaMsgType_stringForType (int type);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern IntPtr mamaStatus_stringForStatus (int type);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getFieldAsString (IntPtr mamaMsg,
				string fieldName,
				ushort fid,
				StringBuilder  buf,
				int length);

            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_isFromInbox(IntPtr mamaMsg);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_detach(IntPtr mamaMsg);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaDateTime_setEpochTimeMilliseconds(ref ulong dateTime,
				ulong milliseconds);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaDateTime_getEpochTimeMilliseconds(ref ulong dateTime,
				ref ulong milliseconds);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_create(ref IntPtr nativeHandle);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_destroy(IntPtr  nativeHandle);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern IntPtr mamaMsgIterator_begin(IntPtr mamaMsgIterator);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern IntPtr mamaMsgIterator_end(IntPtr mamaMsgIterator);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern IntPtr mamaMsgIterator_associate(IntPtr nativeHandle, IntPtr mamaMsgIterator);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_getByteBuffer(IntPtr nativeHandle, ref IntPtr array, ref int size);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_createFromByteBuffer(ref IntPtr nativeHandle, IntPtr array, int size);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsg_createForBridgeFromByteBuffer(ref IntPtr nativeHandle, IntPtr array, int size, IntPtr nativeBridgeHandle);

		}

		#endregion // Implementation details


        /// <summary>
        /// Add an array of strings (char*) to the message.
        /// </summary>
        /// <param name="fid">
        /// The field id, can be 0 if the name is specified and field names are turned on.
        /// </param>
        /// <param name="name">
        /// The field name, can be null or blank if the field Id is supplied.
        /// </param>
        /// <param name="val">
        /// The array of strings to add.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// Throw if the string array is null.
        /// </exception>
        /// <exception cref="ArgumentOutOfRangeException">
        /// Throw if there are no items in the string array.
        /// </exception>
        public void addVectorString(
            string name,
            ushort fid,
            string[] val)
        {
            // Check that the arguments are valid
            if (val == null)
            {
                throw new ArgumentNullException("val");
            }

            // Check the array length
            int numberStrings = val.Length;
            if (numberStrings < 1)
            {
                throw new ArgumentOutOfRangeException("val");
            }

            // Verify that the native message has been created
            EnsurePeerCreated();

            // Allocate memory on the global native heap to pass the messages to the native layer
            IntPtr globalHeapMemory = Marshal.AllocHGlobal(numberStrings * IntPtr.Size);
            try
            {
                // Create a new array to hold all of the native handles
                IntPtr[] nativeHandleArray = new IntPtr[numberStrings];
                try
                {
                    // Copy the strings over with a conversion to ANSI
                    for (int nextItem = 0; nextItem < numberStrings; nextItem++)
                    {
                        nativeHandleArray[nextItem] = Marshal.StringToHGlobalAnsi(val[nextItem]);
                    }

                    // Copy this to the global heap
                    Marshal.Copy(nativeHandleArray, 0, globalHeapMemory, numberStrings);

                    // Call the native function
                    int code = NativeMethods.mamaMsg_addVectorString(nativeHandle, name, fid, globalHeapMemory, numberStrings);
                    CheckResultCode(code);
                }

                finally
                {
                    // Free the individual strings
                    foreach (IntPtr nativeHandle in nativeHandleArray)
                    {
                        if (nativeHandle != null)
                        {
                            Marshal.FreeHGlobal(nativeHandle);
                        }
                    }
                }
            }

            finally
            {
                // Free the memory on the global heap now that the function has completed
                Marshal.FreeHGlobal(globalHeapMemory);
            }
        }

        /// <summary>
        /// Add an array of prices to the message.
        /// </summary>
        /// <param name="fid">
        /// The field id, can be 0 if the name is specified and field names are turned on.
        /// </param>
        /// <param name="name">
        /// The field name, can be null or blank if the field Id is supplied.
        /// </param>
        /// <param name="val">
        /// The array of messages to add.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// Throw if the message array is null.
        /// </exception>
        /// <exception cref="ArgumentOutOfRangeException">
        /// Throw if there are no items in the message array.
        /// </exception>
        public void addVectorPrice(
            string name,
            ushort fid,
            MamaPrice[] val)
        {
            // Check that the arguments are valid
            if (val == null)
            {
                throw new ArgumentNullException("val");
            }

            // Check the array length
            int numberPrices = val.Length;
            if (numberPrices < 1)
            {
                throw new ArgumentOutOfRangeException("val");
            }

            // Verify that the native message has been created
            EnsurePeerCreated();

            // Allocate memory on the global native heap to pass the messages to the native layer
            IntPtr globalHeapMemory = Marshal.AllocHGlobal(numberPrices * IntPtr.Size);
            try
            {
                // Create a new array to hold all of the native handles
                IntPtr[] nativeHandleArray = new IntPtr[numberPrices];

                // Copy the prices over
                for (int nextItem = 0; nextItem < numberPrices; nextItem++)
                {
                    nativeHandleArray[nextItem] = val[nextItem].NativeHandle;
                }

                // Copy this to the global heap
                Marshal.Copy(nativeHandleArray, 0, globalHeapMemory, numberPrices);

                // Call the native function
                int code = NativeMethods.mamaMsg_addVectorPrice(nativeHandle, name, fid, globalHeapMemory, numberPrices);
                CheckResultCode(code);
            }

            finally
            {
                // Free the memory on the global heap now that the function has completed
                Marshal.FreeHGlobal(globalHeapMemory);
            }
        }

        /// <summary>
        /// Add an array of nativeHandle objects to the message.
        /// </summary>
        /// <param name="fid">
        /// The field id, can be 0 if the name is specified and field names are turned on.
        /// </param>
        /// <param name="name">
        /// The field name, can be null or blank if the field Id is supplied.
        /// </param>
        /// <param name="val">
        /// The array of messages to add.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// Throw if the message array is null.
        /// </exception>
        /// <exception cref="ArgumentOutOfRangeException">
        /// Throw if there are no items in the message array.
        /// </exception>
        public void addVectorMsg(
            string name,
            ushort fid,
            MamaMsg[] val)
        {
            // Check that the arguments are valid
            if (val == null)
            {
                throw new ArgumentNullException("val");
            }

            // Check the array length
            int numberMessages = val.Length;
            if (numberMessages < 1)
            {
                throw new ArgumentOutOfRangeException("val");
            }

            // Verify that the native message has been created
            EnsurePeerCreated();

            // Allocate memory on the global native heap to pass the messages to the native layer
            IntPtr globalHeapMemory = Marshal.AllocHGlobal(numberMessages * IntPtr.Size);
            try
            {
                // Create a new array to hold all of the native handles
                IntPtr[] nativeHandleArray = new IntPtr[numberMessages];

                // Copy the native handles over
                for (int nextItem = 0; nextItem < numberMessages; nextItem++)
                {
                    nativeHandleArray[nextItem] = val[nextItem].nativeHandle;
                }

                // Copy this to the global heap
                Marshal.Copy(nativeHandleArray, 0, globalHeapMemory, numberMessages);

                // Call the native function
                int code = NativeMethods.mamaMsg_addVectorMsg(nativeHandle, name, fid, globalHeapMemory, numberMessages);
                CheckResultCode(code);
            }
            finally
            {
                // Free the memory on the global heap now that the function has completed
                Marshal.FreeHGlobal(globalHeapMemory);
            }
        }

        /// <summary>
        /// Update an array of strings (char*).
        /// </summary>
        /// <param name="name">The field name, can be null or blank if the field Id is supplied.</param>
        /// <param name="fid"></param>
        /// <param name="val">The array of strings to add.</param>
        /// <exception cref="ArgumentNullException">
        /// Throw if the string array is null.
        /// </exception>
        /// <exception cref="ArgumentOutOfRangeException">
        /// Throw if there are no items in the string array.
        /// </exception>
        public void updateVectorString(
            string name,
            ushort fid,
            string[] val)
        {
            // Check that the arguments are valid
            if (val == null)
            {
                throw new ArgumentNullException("val");
            }

            // Check the array length
            int numberStrings = val.Length;
            if (numberStrings < 1)
            {
                throw new ArgumentOutOfRangeException("val");
            }

            // Verify that the native message has been created
            EnsurePeerCreated();

            // Allocate memory on the global native heap to pass the messages to the native layer
            IntPtr globalHeapMemory = Marshal.AllocHGlobal(numberStrings * IntPtr.Size);
            try
            {
                // Create a new array to hold all of the native handles
                IntPtr[] nativeHandleArray = new IntPtr[numberStrings];
                try
                {
                    // Copy the strings over with a conversion to ANSI
                    for (int nextItem = 0; nextItem < numberStrings; nextItem++)
                    {
                        nativeHandleArray[nextItem] = Marshal.StringToHGlobalAnsi(val[nextItem]);
                    }

                    // Copy this to the global heap
                    Marshal.Copy(nativeHandleArray, 0, globalHeapMemory, numberStrings);

                    // Call the native function
                    int code = NativeMethods.mamaMsg_updateVectorString(nativeHandle, name, fid, globalHeapMemory, numberStrings);
                    CheckResultCode(code);
                }

                finally
                {
                    // Free the individual strings
                    foreach (IntPtr nativeHandle in nativeHandleArray)
                    {
                        if (nativeHandle != null)
                        {
                            Marshal.FreeHGlobal(nativeHandle);
                        }
                    }
                }
            }

            finally
            {
                // Free the memory on the global heap now that the function has completed
                Marshal.FreeHGlobal(globalHeapMemory);
            }
        }

        /// <summary>
        /// Update an array of prices.
        /// </summary>
        /// <param name="fid">
        /// The field id, can be 0 if the name is specified and field names are turned on.
        /// </param>
        /// <param name="name">
        /// The field name, can be null or blank if the field Id is supplied.
        /// </param>
        /// <param name="val">
        /// The array of messages to add.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// Throw if the message array is null.
        /// </exception>
        /// <exception cref="ArgumentOutOfRangeException">
        /// Throw if there are no items in the message array.
        /// </exception>
        public void updateVectorPrice(
            string name,
            ushort fid,
            MamaPrice[] val)
        {
            // Check that the arguments are valid
            if (val == null)
            {
                throw new ArgumentNullException("val");
            }

            // Check the array length
            int numberPrices = val.Length;
            if (numberPrices < 1)
            {
                throw new ArgumentOutOfRangeException("val");
            }

            // Verify that the native message has been created
            EnsurePeerCreated();

            // Allocate memory on the global native heap to pass the messages to the native layer
            IntPtr globalHeapMemory = Marshal.AllocHGlobal(numberPrices * IntPtr.Size);
            try
            {
                // Create a new array to hold all of the native handles
                IntPtr[] nativeHandleArray = new IntPtr[numberPrices];

                // Copy the prices over
                for (int nextItem = 0; nextItem < numberPrices; nextItem++)
                {
                    nativeHandleArray[nextItem] = val[nextItem].NativeHandle;
                }

                // Copy this to the global heap
                Marshal.Copy(nativeHandleArray, 0, globalHeapMemory, numberPrices);

                // Call the native function
                int code = NativeMethods.mamaMsg_updateVectorPrice(nativeHandle, name, fid, globalHeapMemory, numberPrices);
                CheckResultCode(code);
            }

            finally
            {
                // Free the memory on the global heap now that the function has completed
                Marshal.FreeHGlobal(globalHeapMemory);
            }
        }

        /// <summary>
        /// Update an array of nativeHandle objects.
        /// </summary>
        /// <param name="fid">
        /// The field id, can be 0 if the name is specified and field names are turned on.
        /// </param>
        /// <param name="name">
        /// The field name, can be null or blank if the field Id is supplied.
        /// </param>
        /// <param name="val">
        /// The array of messages to add.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// Throw if the message array is null.
        /// </exception>
        /// <exception cref="ArgumentOutOfRangeException">
        /// Throw if there are no items in the message array.
        /// </exception>
        public void updateVectorMsg(
            string name,
            ushort fid,
            MamaMsg[] val)
        {
            // Check that the arguments are valid
            if (val == null)
            {
                throw new ArgumentNullException("val");
            }

            // Check the array length
            int numberMessages = val.Length;
            if (numberMessages < 1)
            {
                throw new ArgumentOutOfRangeException("val");
            }

            // Verify that the native message has been created
            EnsurePeerCreated();

            // Allocate memory on the global native heap to pass the messages to the native layer
            IntPtr globalHeapMemory = Marshal.AllocHGlobal(numberMessages * IntPtr.Size);
            try
            {
                // Create a new array to hold all of the native handles
                IntPtr[] nativeHandleArray = new IntPtr[numberMessages];

                // Copy the native handles over
                for (int nextItem = 0; nextItem < numberMessages; nextItem++)
                {
                    nativeHandleArray[nextItem] = val[nextItem].nativeHandle;
                }

                // Copy this to the global heap
                Marshal.Copy(nativeHandleArray, 0, globalHeapMemory, numberMessages);

                // Call the native function
                int code = NativeMethods.mamaMsg_updateVectorMsg(nativeHandle, name, fid, globalHeapMemory, numberMessages);
                CheckResultCode(code);
            }

            finally
            {
                // Free the memory on the global heap now that the function has completed
                Marshal.FreeHGlobal(globalHeapMemory);
            }
        }

        /// <summary>
        /// Try to get a vector of strings (char*).
        /// </summary>
        /// <param name="fid">
        /// The field id, can be 0 if the name is specified and field names are turned on.
        /// </param>
        /// <param name="name">
        /// The field name, can be null or blank if the field Id is supplied.
        /// </param>
        /// <param name="result">
        /// To return the array of strings.
        /// </param>
        /// <returns>
        /// false if the field could not be found. Note that true is returned if an empty
        /// array is returned.
        /// </returns>
        public bool tryVectorString(
            string name,
            ushort fid,
            ref string[] result)
        {
            // Returns
            bool ret = false;

            // The following arraylist wil contain all of the strings
            ArrayList retStrings = new ArrayList();

            // Enusre that the native message has been created
            EnsurePeerCreated();

            // Get the strings from the native message
            IntPtr globalMemoryPointer = IntPtr.Zero;
            uint numberElements = 0;
            int code = NativeMethods.mamaMsg_getVectorString(nativeHandle, name, fid, ref globalMemoryPointer, ref numberElements);
            if ((MamaStatus.mamaStatus)code == MamaStatus.mamaStatus.MAMA_STATUS_OK)
            {
                // Only continue if there are actually any strings in the array
                if (numberElements > 0)
                {
                    // Create a new array of pointers to hold the native string pointers
                    IntPtr[] nativeStringArray = new IntPtr[numberElements];

                    // Copy the entire array over
                    Marshal.Copy(globalMemoryPointer, nativeStringArray, 0, (int)numberElements);

                    // Extract each string, one at a time
                    foreach (IntPtr nextNativeString in nativeStringArray)
                    {
                        retStrings.Add(Marshal.PtrToStringAnsi(nextNativeString));
                    }
                }

                // The function has succeeded even if there were 0 strings
                ret = true;
            }

            // Return the array of strings
            result = (string[])retStrings.ToArray(typeof(string));

            return ret;
        }

        /// <summary>
        /// Try to get a vector of prices.
        /// </summary>
        /// <param name="fid">
        /// The field id, can be 0 if the name is specified and field names are turned on.
        /// </param>
        /// <param name="name">
        /// The field name, can be null or blank if the field Id is supplied.
        /// </param>
        /// <param name="result">
        /// To return the array of prices.
        /// </param>
        /// <returns>
        /// false if the field could not be found. Note that true is returned if an empty
        /// array is returned.
        /// </returns>
        public bool tryVectorPrice(
            string name,
            ushort fid,
            ref MamaPrice[] result)
        {
            // Returns
            bool ret = false;

            // The following arraylist wil contain all of the prices
            ArrayList retPrices = new ArrayList();

            // Enusre that the native message has been created
            EnsurePeerCreated();

            // Get the strings from the native message
            IntPtr globalMemoryPointer = IntPtr.Zero;
            uint numberElements = 0;
            int code = NativeMethods.mamaMsg_getVectorPrice(nativeHandle, name, fid, ref globalMemoryPointer, ref numberElements);
            if ((MamaStatus.mamaStatus)code == MamaStatus.mamaStatus.MAMA_STATUS_OK)
            {
                // Only continue if there are actually any prices in the array
                if (numberElements > 0)
                {
                    // Create a new array of pointers to hold the native price pointers
                    IntPtr[] nativePriceArray = new IntPtr[numberElements];

                    // Copy the entire array over
                    Marshal.Copy(globalMemoryPointer, nativePriceArray, 0, (int)numberElements);

                    // Extract each price, one at a time
                    foreach (IntPtr nextNativePrice in nativePriceArray)
                    {
                        retPrices.Add(new MamaPrice(nextNativePrice));
                    }
                }

                // The function has succeeded even if there were 0 strings
                ret = true;
            }

            // Return the array of strings
            result = (MamaPrice[])retPrices.ToArray(typeof(MamaPrice));

            return ret;
        }
    }
}

