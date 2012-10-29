/* $Id: MamdaOrderImbalanceSide.cs,v 1.3.32.5 2012/08/24 16:12:11 clintonmcdowell Exp $
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
	/// </summary>
	public class MamdaOrderImbalanceSide
	{
		/*The imbalance side could be either the buy or sell side
		  i.e. bid/ask. 0 and 1 representation will be used for
		  bid/ask respectively.
		*/
		private int mValue;
		private string mDescription;

		/**BID_SIDE_IMBALANCE*/
		public const int BID_SIDE_VALUE = 0;
		public static readonly MamdaOrderImbalanceSide BID_SIDE =
			new MamdaOrderImbalanceSide(valueToString(BID_SIDE_VALUE), BID_SIDE_VALUE);
    
		/**ASK_SIDE_IMBALANCE*/
		public const int ASK_SIDE_VALUE = 1; 
		public static readonly MamdaOrderImbalanceSide ASK_SIDE =
			new MamdaOrderImbalanceSide(valueToString(ASK_SIDE_VALUE), ASK_SIDE_VALUE);

		/**NO_IMBALANCE*/
		public const int NO_IMBALANCE_VALUE = 2;
		public static readonly MamdaOrderImbalanceSide NO_IMBALANCE_SIDE =
			new MamdaOrderImbalanceSide(valueToString(NO_IMBALANCE_VALUE), NO_IMBALANCE_VALUE);

		/// <summary>
		/// Returns the string name for the enumerated type.
		/// </summary>
		/// <returns>Name for the type.</returns>
		public override string ToString()
		{
			return mDescription;
		}

		/// <summary>
		/// Returns the string name for the enumerated type.
		/// </summary>
		/// <returns>Name for the type.</returns>
		public string toString()
		{
			return ToString();
		}

		/// <summary>
		/// Returns the integer value for the type.
		/// This value can be used in switch statements against the public
		/// XXX_VALUE const members of the class.
		/// </summary>
		/// <returns>The integer type.</returns>
		public int getValue()
		{
			return mValue;
		}

		/// <summary>
		/// Compare the two types for equality.
		/// </summary>
		/// <param name="imbalanceSide">The object to check equality against.</param>
		/// <returns>Returns true if the integer value of both types is equal. Otherwise
		/// returns false.</returns>
		public bool equals(MamdaOrderImbalanceSide imbalanceSide)
		{
			return mValue == imbalanceSide.mValue;
		}

		public override bool Equals(object obj)
		{
			return (obj is MamdaOrderImbalanceSide) && equals((MamdaOrderImbalanceSide)obj);
		}

		public override int GetHashCode()
		{
			return mValue.GetHashCode();
		}

		public static bool operator==(MamdaOrderImbalanceSide left, MamdaOrderImbalanceSide right)
		{
			return Object.ReferenceEquals(left, right) || (left != null && left.equals(right));
		}

		public static bool operator!=(MamdaOrderImbalanceSide left, MamdaOrderImbalanceSide right)
		{
			return !(left == right);
		}

		private MamdaOrderImbalanceSide(string desc, int value)
		{
			mDescription = desc;
			mValue       = value;
		}

		/// <summary>
		/// Utility method for mapping type integer values to corresponding string
		/// values.
		/// 
		/// Returns "UNKNOWN" is the int type value is not recognised.
		/// </summary>
		/// <param name="value">The int value for a MamdaOrderImbalanceSide</param>
		/// <returns>The string name value of the specified MamdaOrderImbalanceSide
		/// integer value</returns>
		public static String valueToString (int value)
		{
			switch (value)
			{
				case ASK_SIDE_VALUE:
					return "ASK_SIDE";
				case BID_SIDE_VALUE:
					return "BID_SIDE";
				case NO_IMBALANCE_VALUE:
					return "NO_IMBALANCE";
				default:
					return "UNKNOWN";
			}
		}

		/// <summary>
		/// Return an instance of a MamdaOrderImbalanceSide  corresponding to
		/// the specified integer value.
		/// Returns null if the integer value is not recognised.
		/// </summary>
		/// <param name="value">Int value for a MamdaOrderImbalanceSide</param>
		/// <returns>Instance of a MamdaOrderImbalanceSide if a mapping exists.</returns>
		public static MamdaOrderImbalanceSide enumObjectForValue(int value)
		{
			switch (value)
			{
				case ASK_SIDE_VALUE:
					return ASK_SIDE;
				case BID_SIDE_VALUE:
					return BID_SIDE;
				case NO_IMBALANCE_VALUE:
					return NO_IMBALANCE_SIDE;
				default:
					return null;
			}
		}
        
	}
}
