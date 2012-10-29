/* $Id: MamaPrice.cs,v 1.8.12.5 2012/08/24 16:12:01 clintonmcdowell Exp $
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
using System.Text;

namespace Wombat
{
    /// <summary>
    /// Enum defining how decimals are represented for the price. 
    /// PREC_10 to PREC_10000000000 represent decimal places. 
    /// PREC_DIV_2, _4, etc. represent fractional 1/2's, 1/4's, etc. 
    /// TICK_32, HALF_32, etc. are for feeds that send prices in 
    /// 1/32's and half 1/32's.
    /// </summary>
    public enum mamaPricePrecision
    {
        MAMA_PRICE_PREC_UNKNOWN      = 0,
        MAMA_PRICE_PREC_10           = 1,
        MAMA_PRICE_PREC_100          = 2,
        MAMA_PRICE_PREC_1000         = 3,
        MAMA_PRICE_PREC_10000        = 4,
        MAMA_PRICE_PREC_100000       = 5,
        MAMA_PRICE_PREC_1000000      = 6,
        MAMA_PRICE_PREC_10000000     = 7,
        MAMA_PRICE_PREC_100000000    = 8,
        MAMA_PRICE_PREC_1000000000   = 9,
        MAMA_PRICE_PREC_10000000000  = 10,
        MAMA_PRICE_PREC_INT          = 16,
        MAMA_PRICE_PREC_DIV_2        = 17,
        MAMA_PRICE_PREC_DIV_4        = 18,
        MAMA_PRICE_PREC_DIV_8        = 19,
        MAMA_PRICE_PREC_DIV_16       = 20,
        MAMA_PRICE_PREC_DIV_32       = 21,
        MAMA_PRICE_PREC_DIV_64       = 22,
        MAMA_PRICE_PREC_DIV_128      = 23,
        MAMA_PRICE_PREC_DIV_256      = 24,
        MAMA_PRICE_PREC_DIV_512      = 25,
        MAMA_PRICE_PREC_TICK_32      = 26,
        MAMA_PRICE_PREC_HALF_32      = 27,
        MAMA_PRICE_PREC_QUARTER_32   = 28,
        MAMA_PRICE_PREC_TICK_64      = 29,
        MAMA_PRICE_PREC_HALF_64      = 30,
        MAMA_PRICE_PREC_CENTS        = MAMA_PRICE_PREC_100,
        MAMA_PRICE_PREC_PENNIES      = MAMA_PRICE_PREC_100
    }

	/// <summary>
	/// A class for manipulating monetary prices
	/// </summary>
	public class MamaPrice : MamaWrapper, IComparable
	{
		/// <summary>
		/// 
		/// </summary>
		/// <param name="nativeHandle"></param>
        internal MamaPrice (IntPtr nativeHandle) : base(nativeHandle)
        {
        }

        /// <summary>
        /// Constructor
        /// </summary>
        public MamaPrice() : base()
        {
            int code = NativeMethods.mamaPrice_create(ref this.nativeHandle);
			CheckResultCode(code);
        }

        /// <summary>
        /// Copy constructor
        /// </summary>
        public MamaPrice(MamaPrice price) : this()
        {
			copy(price);
        }
        /// <summary>
        /// Destroy a mamaPrice object.
        /// </summary>
        public void destroy ()
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
			return (MamaStatus.mamaStatus)NativeMethods.mamaPrice_destroy(nativeHandle);
		}

		public void copy(MamaPrice rhs)
		{
			EnsurePeerCreated();
			int code = NativeMethods.mamaPrice_copy(this.nativeHandle, rhs.nativeHandle);
			CheckResultCode(code);
			GC.KeepAlive(rhs);
		}

        /// <summary>
        /// Clear a MamaPrice object.
        /// </summary>
        public void clear ()
        {
			EnsurePeerCreated();
			int code = NativeMethods.mamaPrice_clear(nativeHandle);
			CheckResultCode(code);
        }

        /// <summary>
        /// Add a price object
        /// </summary>
        public void add (MamaPrice rhs)
        {
			EnsurePeerCreated();
			int code = NativeMethods.mamaPrice_add(nativeHandle, rhs.nativeHandle);
			CheckResultCode(code);
			GC.KeepAlive(rhs);
        }

        /// <summary>
        /// Subtract a price object.  
        /// </summary>
        public void subtract (MamaPrice rhs)
        {
			EnsurePeerCreated();
			int code = NativeMethods.mamaPrice_subtract(nativeHandle, rhs.nativeHandle);
			CheckResultCode(code);
			GC.KeepAlive(rhs);
		}
        
        /// <summary>
        /// Check for equality between two price objects.
        /// </summary>
        public bool equal (MamaPrice rhs)
        {
			EnsurePeerCreated();
			bool result = NativeMethods.mamaPrice_equal(nativeHandle, rhs.nativeHandle) == 1;
			GC.KeepAlive(rhs);
			return result;
		}

		/// <summary>
		/// Synonym of equal
		/// </summary>
		public bool equals(MamaPrice rhs)
		{
			return equal(rhs);
		}

        /// <summary>
        /// Compare two price objects.  The return value is negative if lhs
        /// is earlier than rhs, positive if lhs is greater than rhs and zero
        /// if the two are equal.
        /// </summary>
        public double compare(MamaPrice rhs)
        {
			EnsurePeerCreated();
            double result = NativeMethods.mamaPrice_compare(nativeHandle, rhs.nativeHandle);
			GC.KeepAlive(rhs);
			return result;
        }

        /// <summary>
        /// Set the price value only (no hint information).
        /// </summary>
        public void setValue (double val)
        {
			EnsurePeerCreated();
			int code = NativeMethods.mamaPrice_setValue(nativeHandle, val);
			CheckResultCode(code);
        }

        /// <summary>
        /// Set the price precision.
        /// </summary>
        public void setPrecision (mamaPricePrecision precision)
        {
			EnsurePeerCreated();
			int code = NativeMethods.mamaPrice_setPrecision(nativeHandle, (int)precision);
			CheckResultCode(code);
        }

        /// <summary>
        /// Set all of the price hints.
        /// </summary>
        public void setHints(byte hints)
        {
			EnsurePeerCreated();
			int code = NativeMethods.mamaPrice_setHints(nativeHandle, hints);
			CheckResultCode(code);
        }

        /// <summary>
        /// Set the price with hint information.
        /// </summary>
        public void setWithHints(double val,byte hints)
        {
			EnsurePeerCreated();
			int code = NativeMethods.mamaPrice_setWithHints(nativeHandle,val, hints);
			CheckResultCode(code);
        }

        /// <summary>
        /// Set the price from a string representation, preserving any
        /// detectable hints.
        /// </summary>
        public void setFromString(string str)
        {
			EnsurePeerCreated();
#if MAMA_WRAPPERS_CHECK_ARGUMENTS
			if (str == null)
			{
				throw new ArgumentNullException("str");
			}
#endif
			int code = NativeMethods.mamaPrice_setFromString(nativeHandle, str);
			CheckResultCode(code);
        }

        /// <summary>
        /// Get the price value only.
        /// </summary>
        public double getValue()
        {
			EnsurePeerCreated();
            double ret = 0;
			int code = NativeMethods.mamaPrice_getValue(nativeHandle, ref ret);
			CheckResultCode(code);

            return ret;

        }

        /// <summary>
        /// Get the price precision.
        /// </summary>
        public mamaPricePrecision getPrecision()
        {
			EnsurePeerCreated();
            int ret = 0;
			int code = NativeMethods.mamaPrice_getPrecision(nativeHandle, ref ret);
			CheckResultCode(code);

            return (mamaPricePrecision)ret;

        }

        /// <summary>
        /// Get the price hints.
        /// </summary>
        public byte getHints()
        {
			EnsurePeerCreated();
            byte ret = 0;
			int code = NativeMethods.mamaPrice_getHints(nativeHandle, ref ret);
			CheckResultCode(code);

            return ret;

        }

        /// <summary>
        /// Get the price with special, optional hints.
        /// </summary>
        public double getWithHints(ref int precision)
        {
			EnsurePeerCreated();
            double ret = 0;
			int code = NativeMethods.mamaPrice_getWithHints(nativeHandle, ref ret, ref precision);
			CheckResultCode(code);

            return ret;

        }
        /// <summary>
        /// Get the price as a string, formatted according to the hints provided.
        /// </summary>
        public string getAsString()
        {
			EnsurePeerCreated();
            StringBuilder val = new StringBuilder(255);
			int code = NativeMethods.mamaPrice_getAsString(nativeHandle,val,255);
			CheckResultCode(code);
            return val.ToString();
        }

		/// <summary>
		/// Return a string representation of the price.
		/// </summary>
		public override string ToString()
		{
			return getAsString ();
		}

        /// <summary>
        /// Negate the price value.  Hints and precisions are not affected.
        /// </summary>
        public void negate()
        {
			EnsurePeerCreated();
			int code = NativeMethods.mamaPrice_negate(nativeHandle);
			CheckResultCode(code);
        }

        /// <summary>
        /// Return whether the price has a value equivalent to zero.  It
        /// may not be exactly 0.0, but we check against +/- epsilon.
        /// </summary>
        public bool isZero()
        {
			EnsurePeerCreated();
            bool ret = false;
			int code = NativeMethods.mamaPrice_isZero(nativeHandle, ref ret);
			CheckResultCode(code);

            return ret;

        }

		// necessary methods for instances to be put in sorted and hashed containers

		public override bool Equals(object obj)
		{
			return (equals(obj as MamaPrice));
		}

		public int CompareTo(object rhs) // IComparable
		{
			double ret = compare(rhs as MamaPrice);
			if (ret < 0) return -1;
			else if (ret > 0) return 1;
			else return 0;
		}

		public override int GetHashCode()
		{
			return getValue().GetHashCode();
		}

        /// <summary>
        /// Return the appropriate precision code for a given number of
        /// decimal places.
        /// </summary>
        public static mamaPricePrecision decimals2Precision (int places)
        {
            return (mamaPricePrecision)NativeMethods.mamaPrice_decimals2Precision(places);
        }

        /// <summary>
        /// Return the appropriate precision code for a given fractional
        /// denominator.
        /// </summary>
        public static mamaPricePrecision denom2Precision (int denominator)
        {
            return (mamaPricePrecision)NativeMethods.mamaPrice_denom2Precision(denominator);
        }

        /// <summary>
        /// Return the number of decimal places for a given precision code.
        /// </summary>
        public static int precision2Decimals (int  precision)
        {
            return NativeMethods.mamaPrice_precision2Decimals(precision);
        }

        /// <summary>
        /// Return the fractional denominator for a given precision code.
        /// </summary>
        public static int precision2Denom (int  precision)
        {
            return NativeMethods.mamaPrice_precision2Denom(precision);
        }

		#region Implementation details

		private struct NativeMethods
		{
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_create(ref IntPtr nativeHandle);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_copy(IntPtr dest,
				IntPtr src);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_destroy(IntPtr  nativeHandle);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_clear(IntPtr  nativeHandle);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_add(IntPtr dest,
				IntPtr rhs);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_subtract(IntPtr dest,
				IntPtr rhs);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_equal(IntPtr lhs,
				IntPtr rhs);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern double mamaPrice_compare(IntPtr lhs,
				IntPtr rhs);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_setValue(IntPtr nativeHandle,
				double val);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_setPrecision (IntPtr nativeHandle,
				int precision);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_setHints(IntPtr nativeHandle,
				byte hints);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_setWithHints(IntPtr nativeHandle,
				double val,
				byte hints);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_setFromString(IntPtr nativeHandle,
				string str);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_getValue(IntPtr nativeHandle,
				ref double val);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_getPrecision(IntPtr nativeHandle,
				ref int precision);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_getHints(IntPtr nativeHandle,
				ref byte hints);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_getWithHints(IntPtr nativeHandle,
				ref double value,
				ref int precision);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_getAsString(IntPtr nativeHandle,
				StringBuilder  buf,
				int length);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_negate(IntPtr nativeHandle);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_isZero(IntPtr nativeHandle,
				ref bool result);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_decimals2Precision (int places);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_denom2Precision (int denominator);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_precision2Decimals (int  precision);
			[DllImport(Mama.DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mamaPrice_precision2Denom (int  precision);
		}

		#endregion // Implementation details

    }
}




