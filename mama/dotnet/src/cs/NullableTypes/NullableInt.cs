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
	public struct NullableInt : IComparable
	{
        /// <summary>
        /// Constructor
        /// </summary>
        public NullableInt(int value)
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
        public int Value
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
        public int GetValueOrDefault()
		{
			return mValue;
		}

        /// <summary>
        /// Get value, or default if value is not defined
        /// </summary>
        public int GetValueOrDefault(int defaultValue)
		{
			return mHasValue ? mValue : defaultValue;
		}

        /// <summary>
        /// Test for equality
        /// </summary>
        /// <param name="other">Object to compare with</param>
        /// <returns>TRUE if other object is equal to this one, FALSE otherwise</returns>
        public override bool Equals(object other)
		{
			return CompareTo(other) == 0;
		}

        /// <summary>
        /// Compare another object
        /// </summary>
        /// <param name="obj">Object to compare with</param>
        /// <returns>
        /// If both objects are the same, returns zero. If this object 
        /// has a value but the other object does not, returns +1. If this 
        /// object does not have a value, but the other object does have a 
        /// value, returns -1. If both objects have a value, returns an 
        /// indication of their relative values
        ///</returns>
        public int CompareTo(object obj)
		{
			NullableInt other = (NullableInt)obj;
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
        /// Conversion from int
        /// </summary>
        public static implicit operator NullableInt(int value)
		{
			return new NullableInt(value);
		}

        /// <summary>
        /// Conversion to int
        /// </summary>
        public static explicit operator int(NullableInt value)
		{
			return value.Value;
		}

		private bool mHasValue;
		private int mValue;
	}
}
