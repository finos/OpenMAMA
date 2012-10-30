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
using System.Text;

namespace Wombat
{

   /// <summary>
   /// Represents the types of fields that can be contained in a <see cref="MamaMsg"/>
   /// </summary>
   public enum mamaFieldType
   {
      /// <summary>
      /// MamaMsg
      /// </summary>
      MAMA_FIELD_TYPE_MSG         =   1,
    
      /// <summary>
      /// Binary data
      /// </summary>
      MAMA_FIELD_TYPE_OPAQUE      =   7,
    
      /// <summary>
      /// String
      /// </summary>
      MAMA_FIELD_TYPE_STRING      =   8,
    
      /// <summary>
      /// Boolean
      /// </summary>
      MAMA_FIELD_TYPE_BOOL        =   9, 

      /// <summary>
      /// Character
      /// </summary>
      MAMA_FIELD_TYPE_CHAR        =   10, 

      /// <summary>
      /// Signed 8-bit int
      /// </summary>
      MAMA_FIELD_TYPE_I8          =   14, 

      /// <summary>
      /// Unsigned byte
      /// </summary>
      MAMA_FIELD_TYPE_U8          =   15,
    
      /// <summary>
      /// Signed 16-bit int
      /// </summary>
      MAMA_FIELD_TYPE_I16         =   16,
   
      /// <summary>
      /// Unsigned 16-bit int
      /// </summary>
      MAMA_FIELD_TYPE_U16         =   17,
      
      /// <summary>
      /// Signed 32-bit int
      /// </summary>
      MAMA_FIELD_TYPE_I32         =   18,
    
      /// <summary>
      /// unsigned 32-bit int
      /// </summary>
      MAMA_FIELD_TYPE_U32         =   19,

      /// <summary>
      /// Signed 64-bit int
      /// </summary>
      MAMA_FIELD_TYPE_I64         =   20,

      /// <summary>
      /// Unsigned 64-bit int
      /// </summary>
      MAMA_FIELD_TYPE_U64         =   21,
   
      /// <summary>
      /// 32-bit float
      /// </summary>
      MAMA_FIELD_TYPE_F32         =   24, 

      /// <summary>
      /// 64-bit float
      /// </summary>
      MAMA_FIELD_TYPE_F64         =   25,

      /// <summary>
      /// 64-bit MAMA time
      /// </summary>
      MAMA_FIELD_TYPE_TIME        =   26,

      /// <summary>
      /// 72-bit MAMA price
      /// </summary>
       MAMA_FIELD_TYPE_PRICE       =   27,

      /// <summary>
      /// Array of I8s
      /// </summary>
      MAMA_FIELD_TYPE_VECTOR_I8     =   34, 
      
       /// <summary>
       /// Array of U8s
       /// </summary>
       MAMA_FIELD_TYPE_VECTOR_U8     =   35, 

       /// <summary>
       /// Array of I16s
       /// </summary>
       MAMA_FIELD_TYPE_VECTOR_I16    =   36, 
      
       /// <summary>
       /// Array of U16s
       /// </summary>
       MAMA_FIELD_TYPE_VECTOR_U16    =   37, 
            
       /// <summary>
       /// Array of I32s
       /// </summary>
       MAMA_FIELD_TYPE_VECTOR_I32    =   38, 
            
       /// <summary>
       /// Array of U32s
       /// </summary>
       MAMA_FIELD_TYPE_VECTOR_U32    =   39,
            
       /// <summary>
       /// Array of I64s
       /// </summary>
       MAMA_FIELD_TYPE_VECTOR_I64    =   40, 
            
       /// <summary>
       /// Array of U64s
       /// </summary>
       MAMA_FIELD_TYPE_VECTOR_U64    =   41,
            
       /// <summary>
       /// Array of F32s
       /// </summary>
       MAMA_FIELD_TYPE_VECTOR_F32    =   44, 
            
       /// <summary>
       /// Array of F64s
       /// </summary>
       MAMA_FIELD_TYPE_VECTOR_F64    =   45,
            
       /// <summary>
       /// Array of strings
       /// </summary>
       MAMA_FIELD_TYPE_VECTOR_STRING =   46,
            
       /// <summary>
       /// Array of MamaMsgs
       /// </summary>
       MAMA_FIELD_TYPE_VECTOR_MSG    =   47,
            
       /// <summary>
       /// Array of MamaTimes
       /// </summary>
       MAMA_FIELD_TYPE_VECTOR_TIME   =   48,
            
       /// <summary>
       /// Array of MamaPrices
       /// </summary>
       MAMA_FIELD_TYPE_VECTOR_PRICE  =   49,
      
       /// <summary/>
       MAMA_FIELD_TYPE_QUANTITY		=	50,

       /// <summary/>
       MAMA_FIELD_TYPE_COLLECTION  =   99,
       
       /// <summary>
       /// Unknown type
       /// </summary>
       MAMA_FIELD_TYPE_UNKNOWN     =   100
   }
	/// <summary>
	/// Class that represents a field in a MamaMsg
	/// </summary>
	public class MamaMsgField : MamaWrapper
	{
		/// <summary>
		/// 
		/// </summary>
		/// <param name="nativeHandle"></param>
		internal MamaMsgField(IntPtr nativeHandle) : base(nativeHandle)
		{
		}

        public MamaMsgField()
		{
		}
        
		/// <summary>
		/// Destroy the MamaMsg
		/// </summary>
		public void destroy()
		{ 
			Dispose();
		}

		/// <summary>
        /// Implements the destruction of the underlying peer object
        /// <seealso cref="MamaWrapper.DestroyNativePeer"/>
        /// </summary>
		/// <returns>MAMA Status code</returns>
		protected override MamaStatus.mamaStatus DestroyNativePeer()
		{
			return (MamaStatus.mamaStatus)NativeMethods.mamaMsgField_destroy(nativeHandle);
		}


		/// <summary>
		/// Get the field type for this field.
		/// </summary>
		/// <returns></returns>
		public mamaFieldType getType () 
		{
			EnsurePeerCreated();
			int type = 0;
			int code = NativeMethods.mamaMsgField_getType(nativeHandle, ref type);
			CheckResultCode(code);
			return (mamaFieldType)type;
		}
      
		/// <summary>
		/// Get the field descriptor for this field.
		/// </summary>
		/// <returns></returns>
		public MamaFieldDescriptor getDescriptor () 
		{
			EnsurePeerCreated();
			IntPtr descriptor = IntPtr.Zero;
			int code = NativeMethods.mamaMsgField_getDescriptor(nativeHandle, ref descriptor);
			CheckResultCode(code);
			return new MamaFieldDescriptor(descriptor);
		}

		/// <summary>
		/// Convert the value of the specified field to a string. ( up to  255 characters )
		/// </summary>
		/// <returns></returns>
		public string getAsString () 
		{
			EnsurePeerCreated();
			StringBuilder val = new StringBuilder(255);
			int code = NativeMethods.mamaMsgField_getAsString(nativeHandle, val, 255);
			CheckResultCode(code);
			return val.ToString();
		}


		/// <summary>
		/// Get the field ID for this field.
		/// </summary>
		public int getFid () 
		{
			EnsurePeerCreated();
			ushort ret = 0;
			int code = NativeMethods.mamaMsgField_getFid(nativeHandle,ref ret);
			CheckResultCode(code);
			return ret;
		}
	
		/// <summary>
		/// Get the field name for this field.
		/// </summary>
		public string getName () 
		{
			EnsurePeerCreated();
			IntPtr ret = IntPtr.Zero;
			int code = NativeMethods.mamaMsgField_getName(nativeHandle,ref ret);
			CheckResultCode(code);
			return Marshal.PtrToStringAnsi(ret);
		}

		/// <summary>
		/// Get the type name for this field. This is a human readable 
		/// representation of the type.
		/// </summary>
		public string getTypeName () 
		{
			EnsurePeerCreated();
			IntPtr ret = IntPtr.Zero;
			int code = NativeMethods.mamaMsgField_getTypeName(nativeHandle,ref ret);
			CheckResultCode(code);
			return Marshal.PtrToStringAnsi(ret);
		}

		/// <summary>
		/// Get a bool field.
		/// </summary>
		public bool getBool () 
		{
			EnsurePeerCreated();
			bool ret = false;
			int code = NativeMethods.mamaMsgField_getBool(nativeHandle,ref ret);
			CheckResultCode(code);
			return ret;
		}

		/// <summary>
		/// Get a character field.
		/// </summary>
		public char getChar () 
		{
			EnsurePeerCreated();
			char ret = '\0';
			int code = NativeMethods.mamaMsgField_getChar(nativeHandle,ref ret);
			CheckResultCode(code);
			return ret;
		}

		/// <summary>
		/// Get a I8, signed 8 bit integer, field.
		/// </summary>
		public sbyte getI8 () 
		{
			EnsurePeerCreated();
			sbyte ret = 0;
			int code = NativeMethods.mamaMsgField_getI8(nativeHandle,ref ret);
			CheckResultCode(code);
			return ret;
		}

		/// <summary>
		/// Get a U8, unsigned 8 bit integer, field.
		/// </summary>
		public byte getU8 ()
		{
			EnsurePeerCreated();
			byte ret = 0;
			int code = NativeMethods.mamaMsgField_getU8(nativeHandle,ref ret);
			CheckResultCode(code);
			return ret;
		}

		/// <summary>
		/// Get a I16, signed 16 bit integer, field.
		/// </summary>
		public short getI16 () 
		{
			EnsurePeerCreated();
			short ret = 0;
			int code = NativeMethods.mamaMsgField_getI16(nativeHandle,ref ret);
			CheckResultCode(code);
			return ret;
		}

		/// <summary>
		/// Get a U16, unsigned 16 bit integer, field.
		/// </summary>
		public ushort getU16 () 
		{
			EnsurePeerCreated();
			ushort ret = 0;
			int code = NativeMethods.mamaMsgField_getU16(nativeHandle,ref ret);
			CheckResultCode(code);
			return ret;
		}

		/// <summary>
		/// Get a I32, signed 32 bit integer, field.
		/// </summary>
		public int getI32 () 
		{
			EnsurePeerCreated();
			int ret = 0;
			int code = NativeMethods.mamaMsgField_getI32(nativeHandle,ref ret);
			CheckResultCode(code);
			return ret;
		}

		/// <summary>
		/// Get a U32, unsigned 32 bit integer, field.
		/// </summary>
		public uint getU32 ()
		{
			EnsurePeerCreated();
			uint ret = 0;
			int code = NativeMethods.mamaMsgField_getU32(nativeHandle,ref ret);
			CheckResultCode(code);
			return ret;
		}

		/// <summary>
		/// Get a I64, signed 64 bit integer, field.
		/// </summary>
		public long getI64 ()
		{
			EnsurePeerCreated();
			long ret = 0;
			int code = NativeMethods.mamaMsgField_getI64(nativeHandle,ref ret);
			CheckResultCode(code);
			return ret;
		}

		/// <summary>
		/// Get a U64, unsigned 64 bit integer, field.
		/// </summary>
		public ulong getU64 ()
		{
			EnsurePeerCreated();
			ulong ret = 0;
			int code = NativeMethods.mamaMsgField_getU64(nativeHandle,ref ret);
			CheckResultCode(code);
			return ret;
		}

		/// <summary>
		/// Get a F32, floating point 32 bit integer, field.
		/// </summary>
		public float getF32 ()
		{
			EnsurePeerCreated();
			float ret = 0;
			int code = NativeMethods.mamaMsgField_getF32(nativeHandle,ref ret);
			CheckResultCode(code);
			return ret;
		}

		/// <summary>
		/// Get a F64, floating point 64 bit integer, field.
		/// </summary>
		public double getF64 ()
		{
			EnsurePeerCreated();
			double ret = 0;
			int code = NativeMethods.mamaMsgField_getF64(nativeHandle,ref ret);
			CheckResultCode(code);
			return ret;
		}

		/// <summary>
		/// Get a string field.
		/// </summary>
		public string getString()
		{
			EnsurePeerCreated();
			IntPtr ret = IntPtr.Zero;
			int code = NativeMethods.mamaMsgField_getString(nativeHandle,ref ret);
			CheckResultCode(code);
            string strRet = Marshal.PtrToStringAnsi(ret);
            return strRet;
		}
    
		/// <summary>
		/// Get an opaque field.
		/// </summary>
		public byte[] getOpaque() 
		{   
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsgField_getOpaque(nativeHandle,ref array, ref size);
			CheckResultCode(code);
			byte[] ret = new byte[size];
			Marshal.Copy(array, ret, 0, (int)size);

			return ret;
		}

		/// <summary>
		/// Get a MAMA date/time field.
		/// </summary>
		public DateTime getDateTime()
		{
			EnsurePeerCreated();
			int code;
			ulong dateTime = 0;
			code = NativeMethods.mamaMsgField_getDateTime(nativeHandle,ref dateTime);
			CheckResultCode(code);
        
			ulong dateTimeMsec = 0;
			code = NativeMethods.mamaDateTime_getEpochTimeMilliseconds (ref dateTime, ref dateTimeMsec);
			CheckResultCode(code);
			DateTime ret = new DateTime(((long)dateTimeMsec * TimeSpan.TicksPerMillisecond) + 621355968000000000);

			return ret;
		}
       
		/// <summary>
		/// Get a MAMA price field.
		/// </summary>
		public MamaPrice getPrice()
		{
			EnsurePeerCreated();
            if (price_ == null)
            {
                price_ = new MamaPrice ();
            }
			int code = NativeMethods.mamaMsgField_getPrice(nativeHandle,price_.NativeHandle);
			MamaStatus.mamaStatus status = (MamaStatus.mamaStatus)code;
			if (status != MamaStatus.mamaStatus.MAMA_STATUS_OK)
			{
				throw new MamaException(status);
			}
			return price_;
		}

        /// <summary>
        /// Get a MamaMsg field
        /// </summary>
        public MamaMsg getMsg()
        {
            EnsurePeerCreated();

            IntPtr myP = IntPtr.Zero;
            
            int code = NativeMethods.mamaMsgField_getMsg(nativeHandle, ref myP);
            MamaStatus.mamaStatus status = (MamaStatus.mamaStatus)code;
            if (status != MamaStatus.mamaStatus.MAMA_STATUS_OK)
            {
                throw new MamaException(status);
            }
            
            if (msg_ == null)
            {
                msg_ = new MamaMsg ();
            }

            msg_.setNativeHandle(myP);
            
            return msg_;
        }

        /// <summary>
		/// Get a vector of booleans.
		/// </summary>
		public bool[] getVectorBool ()
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsgField_getVectorBool(nativeHandle,ref array, ref size);
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
		/// Get a vector of characters.
		/// </summary>
		public char[] getVectorChar ()
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsgField_getVectorChar(nativeHandle,ref array, ref size);
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
		/// Get a vector of signed 8 bit integers.
		/// </summary>
		public sbyte[] getVectorI8 ()
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsgField_getVectorI8(nativeHandle,ref array, ref size);
			CheckResultCode(code);
			byte[] bRet = new byte[size];
			sbyte[] ret = new sbyte[size];
			Marshal.Copy(array, bRet, 0, (int)size);

			for (int i = 0; i <  ret.Length; i++)
			{
				ret[i] = (sbyte)bRet[i];
			}

			return ret;
		}


		/// <summary>
		/// Get a vector of unsigned 8 bit integers.
		/// </summary>
		public byte[] getVectorU8 ()
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsgField_getVectorU8(nativeHandle,ref array, ref size);
			CheckResultCode(code);
			byte[] ret = new byte[size];
			Marshal.Copy(array, ret, 0, (int)size);

			return ret;
		}

		/// <summary>
		/// Get a vector of signed 16 bit integers.
		/// </summary>
		public short[] getVectorI16 ()
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsgField_getVectorI16(nativeHandle,ref array, ref size);
			CheckResultCode(code);
			short[] ret = new short[size];
			Marshal.Copy(array, ret, 0, (int)size);

			return ret;
		}


		/// <summary>
		/// Get a vector of unsigned 16 bit integers.
		/// </summary>
		public ushort[] getVectorU16 ()
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsgField_getVectorU16(nativeHandle,ref array, ref size);
			CheckResultCode(code);
			short[] sRet = new short[size];
			ushort[] ret = new ushort[size];
			Marshal.Copy(array, sRet, 0, (int)size);

			for (int i = 0; i <  ret.Length; i++)
			{
				ret[i] = (ushort)sRet[i];
			}

			return ret;
		}


		/// <summary>
		/// Get a vector of signed 32 bit integers.
		/// </summary>
		public int[] getVectorI32 ()
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsgField_getVectorI32(nativeHandle,ref array, ref size);
			CheckResultCode(code);
			int[] ret = new int[size];
			Marshal.Copy(array, ret, 0, (int)size);

			return ret;
		}
    
		/// <summary>
		/// Get a vector of unsigned 32 bit integers.
		/// </summary>
		public uint[] getVectorU32 ()
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsgField_getVectorU32(nativeHandle,ref array, ref size);
			CheckResultCode(code);
			int[] iRet = new int[size];
			uint[] ret = new uint[size];
			Marshal.Copy(array, iRet, 0, (int)size);

			for (int i = 0; i <  ret.Length; i++)
			{
				ret[i] = (uint)iRet[i];
			}

			return ret;
		}

		/// <summary>
		/// Get a vector of signed 64 bit integers.
		/// </summary>
		public long[] getVectorI64 ()
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsgField_getVectorI64(nativeHandle,ref array, ref size);
			CheckResultCode(code);
			long[] ret = new long[size];
			Marshal.Copy(array, ret, 0, (int)size);

			return ret;
		}
    
		/// <summary>
		/// Get a vector of unsigned 64 bit integers.
		/// </summary>
		public ulong[] getVectorU64 ()
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsgField_getVectorU64(nativeHandle,ref array, ref size);
			CheckResultCode(code);
			long[] lRet = new long[size];
			ulong[] ret = new ulong[size];
			Marshal.Copy(array, lRet, 0, (int)size);

			for (int i = 0; i <  ret.Length; i++)
			{
				ret[i] = (ulong)lRet[i];
			}

			return ret;
		}


		/// <summary>
		/// Get a vector of 32 bit floating point numbers.
		/// </summary>
		public float[] getVectorF32 ()
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsgField_getVectorF32(nativeHandle,ref array, ref size);
			CheckResultCode(code);
			float[] ret = new float[size];
			Marshal.Copy(array, ret, 0, (int)size);

			return ret;
		}


		/// <summary>
		/// Get a vector of 64 bit floating point numbers.
		/// </summary>
		public double[] getVectorF64 ()
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsgField_getVectorF64(nativeHandle,ref array, ref size);
			CheckResultCode(code);
			double[] ret = new double[size];
			Marshal.Copy(array, ret, 0, (int)size);

			return ret;
		}

        /// <summary>
        /// Get a vector of strings
        /// </summary>
        /// <returns>
        /// Arrat if strings.
        /// </returns>
        public string[] getVectorString()
        {
            // Returns
            ArrayList ret = new ArrayList();

            // Ensure that the peer message is created
            EnsurePeerCreated();

            // Call the native method which returns a native memory pointer
            IntPtr nativeArrayPointer = IntPtr.Zero;
            uint size = 0;
            int code = NativeMethods.mamaMsgField_getVectorString(nativeHandle, ref nativeArrayPointer, ref size);
            CheckResultCode(code);

            /* The native pointer is to an array of native strings, copy all these pointers over
             * so that the strings can be converted individually.
             */
            IntPtr[] nativeStringPointers = new IntPtr[size];
            Marshal.Copy(nativeArrayPointer, nativeStringPointers, 0, (int)size);

            // Translate each string in turn
            foreach (IntPtr nativeStringPointer in nativeStringPointers)
            {
                ret.Add(Marshal.PtrToStringAnsi(nativeStringPointer));
            }

            // Convert the array list to an array of strings            
            return (string[])ret.ToArray(typeof(string));

        }

		/// <summary>
		/// Get a vector of submessages.
		/// </summary>
		public MamaMsg[] getVectorMsg ()
		{
			EnsurePeerCreated();
			IntPtr array = IntPtr.Zero;
			uint size = 0;
			int code = NativeMethods.mamaMsgField_getVectorMsg(nativeHandle,ref array, ref size);
			CheckResultCode(code);

			MamaMsg[] ret = new MamaMsg[size];
			for (int i = 0 ; i < size; i++) 
			{
                ret[i] = new MamaMsg(Marshal.ReadIntPtr(array, i * Marshal.SizeOf(typeof(IntPtr))));
			}

			return ret;
		}


        private MamaPrice price_ = null;
        private MamaMsg msg_ = null;
        
		#region Implementation details

		private struct NativeMethods
		{
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_destroy (IntPtr nativeHandle);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getType(IntPtr nativeHandle,ref int result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getDescriptor(IntPtr nativeHandle,
				ref IntPtr result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getFid(IntPtr nativeHandle, 
				ref ushort result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getName(IntPtr nativeHandle,
				ref IntPtr result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getTypeName(IntPtr nativeHandle,
				ref IntPtr result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getBool(IntPtr nativeHandle,
				ref bool result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getChar(IntPtr nativeHandle,
				ref char result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getI8(IntPtr nativeHandle, 
				ref sbyte result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getU8(IntPtr nativeHandle,
				ref byte result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getI16(IntPtr nativeHandle,
				ref short result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getU16(IntPtr nativeHandle, 
				ref ushort result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getI32(IntPtr nativeHandle,
				ref int result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getU32(IntPtr nativeHandle,
				ref uint result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getI64(IntPtr nativeHandle,
				ref long result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getU64(IntPtr nativeHandle,
				ref ulong result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getF32(IntPtr nativeHandle,
				ref float result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getF64(IntPtr nativeHandle,
				ref double result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getString(IntPtr nativeHandle,
				ref IntPtr result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getOpaque(IntPtr nativeHandle,
				ref IntPtr result,
				ref uint size);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getDateTime(IntPtr nativeHandle,
				ref ulong result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getPrice(IntPtr nativeHandle,
				IntPtr result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mamaMsgField_getMsg(IntPtr nativeHandle,
                ref IntPtr result);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getVectorBool(IntPtr nativeHandle,
				ref IntPtr  result,
				ref uint size);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getVectorChar (IntPtr nativeHandle,
				ref IntPtr result,
				ref uint size);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getVectorI8 (IntPtr nativeHandle,
				ref IntPtr result,
				ref uint size);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getVectorU8 (IntPtr nativeHandle,
				ref IntPtr result,
				ref uint size);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getVectorI16 (IntPtr nativeHandle, 
				ref IntPtr result,
				ref uint size);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getVectorU16 (IntPtr nativeHandle,
				ref IntPtr result,
				ref uint size);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getVectorI32 (IntPtr nativeHandle,
				ref IntPtr result,
				ref uint size);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getVectorU32 (IntPtr nativeHandle,
				ref IntPtr result,
				ref uint size);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getVectorI64 (IntPtr nativeHandle,
				ref IntPtr result,
				ref uint size);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getVectorU64 (IntPtr nativeHandle,
				ref IntPtr result,
				ref uint size);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getVectorF32 (IntPtr nativeHandle,
				ref IntPtr result,
				ref uint size);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getVectorF64 (IntPtr nativeHandle, 
				ref IntPtr result,
				ref uint size);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getVectorString (IntPtr nativeHandle, 
				ref IntPtr result,
				ref uint size);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getVectorMsg (IntPtr nativeHandle, 
				ref IntPtr result,
				ref uint size);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaMsgField_getAsString(IntPtr nativeHandle,
				StringBuilder  buf,
				int length);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaDateTime_getEpochTimeMilliseconds(ref ulong dateTime,
				ref ulong milliseconds);
            [DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_destroy(IntPtr  nativeHandle);
        }

		#endregion // Implementation details

	}
}
