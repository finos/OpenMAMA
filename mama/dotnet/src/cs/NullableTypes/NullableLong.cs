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

namespace Wombat
{
	/// <summary>
	/// Modelled after Nullable&lt;T&gt; in C# 2.0
	/// </summary>
	public struct NullableLong : IComparable
	{
	    /// <summary>
	    /// Constructor
	    /// </summary>
		public NullableLong(long value)
		{
			// can't just delegate to Value = value
			mValue = value;
			mHasValue = true;
		}

        /// <summary>
        /// Return TRUE if object has a value
        /// </summary>
		public bool HasValue
		{
			get
			{
				return mHasValue;
			}
		}

        /// <summary>
        /// Return the value
        /// </summary>
		public long Value
		{
			get
			{
				if (!mHasValue)
					throw new InvalidOperationException("The nullable instance has no value");
				return mValue;
			}
			set
			{
				mValue = value;
				mHasValue = true;
			}
		}

        /// <overloads>Get the value</overloads>
        /// <summary>
        /// Get the value
        /// </summary>
        public long GetValueOrDefault()
		{
			return mValue;
		}

        /// <summary>
        /// Get value, or default if value is not defined
        /// </summary>
        public long GetValueOrDefault(long defaultValue)
		{
			return mHasValue ? mValue : defaultValue;
		}

        /// <summary>
        /// Return TRUE if other object is equal to this one
        /// </summary>
		public override bool Equals(object other)
		{
			return CompareTo(other) == 0;
		}

        /// <summary>
        /// Compare another object. If both objects are the same, then return zero.
        /// If this object has a value but the other object does not, return +1
        /// If this object does not have a value, but the other object does have a value,
        /// return -1. If both objects have a value, the return value will be
        /// an indication of their relative values
        /// </summary>
        public int CompareTo(object obj)
		{
			NullableLong other = (NullableLong)obj;
			if (mHasValue)
			{
				if (other.mHasValue)
				{
					return mValue.CompareTo(other.mValue);
				}
				else
				{
					return 1;
				}
			}
			else if (other.mHasValue)
			{
				return -1;
			}
			else
			{
				return 0;
			}
		}

        /// <summary>
        /// Return the hashcode
        /// </summary>
		public override int GetHashCode()
		{
			return mHasValue ? mValue.GetHashCode() : 0;
		}

        /// <summary>
        /// Stringify the value
        /// </summary>
		public override string ToString()
		{
			return mHasValue ? mValue.ToString() : String.Empty;
		}

        /// <summary>
        /// For implicit type conversions
        /// </summary>
		public static implicit operator NullableLong(long value)
		{
			return new NullableLong(value);
		}

        /// <summary>
        /// For explicit type conversions
        /// </summary>
        public static explicit operator long(NullableLong value)
		{
			return value.Value;
		}

		private bool mHasValue;
		private long mValue;
	}
}
