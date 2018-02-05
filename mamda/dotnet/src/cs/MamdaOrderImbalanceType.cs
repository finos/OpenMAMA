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
	/// </summary>
	public class MamdaOrderImbalanceType
	{
		private int    mValue;
		private string mDescription;

		/**MARKET_IMBALANCE_BUY*/
		public const int MARKET_IMBALANCE_BUY_VALUE = 16;
		public static readonly MamdaOrderImbalanceType  MARKET_IMBALANCE_BUY =
			new MamdaOrderImbalanceType(valueToString(MARKET_IMBALANCE_BUY_VALUE), MARKET_IMBALANCE_BUY_VALUE);

		/**MARKET_IMBALANCE_SELL*/
		public const int MARKET_IMBALANCE_SELL_VALUE =17;
		public static readonly MamdaOrderImbalanceType  MARKET_IMBALANCE_SELL =
			new MamdaOrderImbalanceType(valueToString(MARKET_IMBALANCE_SELL_VALUE), MARKET_IMBALANCE_SELL_VALUE);

		/**NO_MARKET_IMBALANCE*/
		public const int NO_MARKET_IMBALANCE_VALUE = 18;
		public static readonly MamdaOrderImbalanceType  NO_MARKET_IMBALANCE =
			new MamdaOrderImbalanceType(valueToString(NO_MARKET_IMBALANCE_VALUE), NO_MARKET_IMBALANCE_VALUE);

		/**MOC_IMBALANCE_BUY*/
		public const int MOC_IMBALANCE_BUY_VALUE = 19;
		public static readonly MamdaOrderImbalanceType  MOC_IMBALANCE_BUY =
			new MamdaOrderImbalanceType(valueToString(MOC_IMBALANCE_BUY_VALUE), MOC_IMBALANCE_BUY_VALUE);

		/**MOC_IMBALANCE_SELL*/
		public const int MOC_IMBALANCE_SELL_VALUE = 20;
		public static readonly MamdaOrderImbalanceType  MOC_IMBALANCE_SELL =
			new MamdaOrderImbalanceType(valueToString(MOC_IMBALANCE_SELL_VALUE), MOC_IMBALANCE_SELL_VALUE);

		/**NO_MOC_IMBALANCE*/
		public const int NO_MOC_IMBALANCE_VALUE = 21;
		public static readonly MamdaOrderImbalanceType  NO_MOC_IMBALANCE =
			new MamdaOrderImbalanceType(valueToString(NO_MOC_IMBALANCE_VALUE), NO_MOC_IMBALANCE_VALUE);

		/**ORDER_IMB*/
		public const int ORDER_IMB_VALUE = 22;
		public static readonly MamdaOrderImbalanceType ORDER_IMB =
			new MamdaOrderImbalanceType(valueToString(ORDER_IMB_VALUE), ORDER_IMB_VALUE);

		/**ORDER_INF*/
		public const int ORDER_INF_VALUE = 23;
		public static readonly MamdaOrderImbalanceType ORDER_INF =
			new MamdaOrderImbalanceType(valueToString(ORDER_INF_VALUE), ORDER_INF_VALUE);

		/**ORDER_IMBALANCE_BUY_VALUE*/
		public const int ORDER_IMBALANCE_BUY_VALUE = 24;
		public static readonly MamdaOrderImbalanceType  ORDER_IMBALANCE_BUY =
			new MamdaOrderImbalanceType(valueToString(ORDER_IMBALANCE_BUY_VALUE), ORDER_IMBALANCE_BUY_VALUE);

		/**ORDER_IMBALANCE_SELL_VALUE*/
		public const int ORDER_IMBALANCE_SELL_VALUE = 25;
		public static readonly MamdaOrderImbalanceType ORDER_IMBALANCE_SELL =
			new MamdaOrderImbalanceType(valueToString(ORDER_IMBALANCE_SELL_VALUE), ORDER_IMBALANCE_SELL_VALUE);

		/**NO_ORDER_IMBALANCE*/
		public const int NO_ORDER_IMBALANCE_VALUE = 26;
		public static readonly MamdaOrderImbalanceType  NO_ORDER_IMBALANCE =
			new MamdaOrderImbalanceType(valueToString(NO_ORDER_IMBALANCE_VALUE), NO_ORDER_IMBALANCE_VALUE);

		/**UKNOWN*/
		public const int UNKNOWN = -99;
		public static readonly MamdaOrderImbalanceType TYPE_UNKNOWN =
			new MamdaOrderImbalanceType(valueToString(UNKNOWN), UNKNOWN);

		private  MamdaOrderImbalanceType (String type, int value)
		{
			mValue       = value;
			mDescription = type;
		}

		/// <summary>
		/// Returns the integer value for the type.
		/// This value can be used in switch statements against the public
		/// XXX_VALUE static members of the class.
		/// </summary>
		/// <returns>The integer type.</returns>
		public int getValue()
		{
			return mValue;
		}

		/// <summary>
		/// Returns the string name for the enumerated type.
		/// </summary>
		/// <returns>Name for the type.</returns>
		public string toString()
		{
			return mDescription;
		}

		public override string ToString()
		{
			return toString();
		}

		public override int GetHashCode()
		{
			return mValue.GetHashCode();
		}

		/// <summary>
		/// Return an instance of a MamdaOrderImbalanceType corresponding to the specified
		/// integer value.
		/// Returns null if the integer value is not recognised.
		/// </summary>
		/// <param name="value">Int value for a MamdaOrderImbalanceType</param>
		/// <returns>Instance of a MamdaOrderImbalanceType if a mapping exists.</returns>
		public static MamdaOrderImbalanceType enumObjectForValue(int value)
		{
			switch (value)
			{
				case MARKET_IMBALANCE_BUY_VALUE:
					return MARKET_IMBALANCE_BUY;
				case MARKET_IMBALANCE_SELL_VALUE:
					return MARKET_IMBALANCE_SELL;
				case NO_MARKET_IMBALANCE_VALUE:
					return NO_MARKET_IMBALANCE;
				case MOC_IMBALANCE_BUY_VALUE:
					return MOC_IMBALANCE_BUY;
				case MOC_IMBALANCE_SELL_VALUE:
					return MOC_IMBALANCE_SELL;
				case NO_MOC_IMBALANCE_VALUE:
					return NO_MOC_IMBALANCE;
				case ORDER_IMB_VALUE:
					return ORDER_IMB;
				case ORDER_INF_VALUE:
					return ORDER_INF;
				case ORDER_IMBALANCE_BUY_VALUE:
					return ORDER_IMBALANCE_BUY;
				case ORDER_IMBALANCE_SELL_VALUE:
					return ORDER_IMBALANCE_SELL;
				case NO_ORDER_IMBALANCE_VALUE:
					return NO_ORDER_IMBALANCE;
				default:
					return null;
			}
		}

		/// <summary>
		/// Utility method for mapping type integer values to corresponding string
		/// values.
		///
		/// Returns "UNKNOWN" if the int type value is not recognised.
		/// </summary>
		/// <param name="value">The int value for a MamdaOrderImbalanceType</param>
		/// <returns>The string name value of the specified MamdaOrderImbalanceType
		/// value.</returns>
		public static String valueToString(int value)
		{
			switch (value)
			{
				case MARKET_IMBALANCE_BUY_VALUE:
					return "MktImbBuy";
				case MARKET_IMBALANCE_SELL_VALUE:
					return "MktImbSell";
				case NO_MARKET_IMBALANCE_VALUE:
					return "NoMktImb";
				case MOC_IMBALANCE_BUY_VALUE:
					return "MocImbBuy";
				case MOC_IMBALANCE_SELL_VALUE:
					return "MocImbSell";
				case NO_MOC_IMBALANCE_VALUE:
					return "NoMocImb";
				case ORDER_IMB_VALUE:
					return "OrderImb";
				case ORDER_INF_VALUE:
					return "OrderInf";
				case ORDER_IMBALANCE_BUY_VALUE:
					return "OrderImbBuy";
				case ORDER_IMBALANCE_SELL_VALUE:
					return "OrderImbSell";
				case NO_ORDER_IMBALANCE_VALUE:
					return "OrderImbNone";
				default:
					return "UNKNOWN";
			}

		}

		public static int stringToValue(String type)
		{
			if (type == MARKET_IMBALANCE_BUY.toString())
				return MARKET_IMBALANCE_BUY_VALUE;
			if (type == MARKET_IMBALANCE_SELL.toString())
				return MARKET_IMBALANCE_SELL_VALUE;
			if (type == NO_MARKET_IMBALANCE.toString())
				return NO_MARKET_IMBALANCE_VALUE;
			if (type == MOC_IMBALANCE_BUY.toString())
				return MOC_IMBALANCE_BUY_VALUE;
			if (type == MOC_IMBALANCE_SELL.toString())
				return MOC_IMBALANCE_SELL_VALUE;
			if (type == NO_MOC_IMBALANCE.toString())
				return NO_MOC_IMBALANCE_VALUE;
			if (type == ORDER_IMB.toString())
				return ORDER_IMB_VALUE;
			if (type == ORDER_INF.toString())
				return ORDER_INF_VALUE;
			if (type == ORDER_IMBALANCE_BUY.ToString())
				return ORDER_IMBALANCE_BUY_VALUE;
			if (type == ORDER_IMBALANCE_SELL.ToString())
				return ORDER_IMBALANCE_SELL_VALUE;
			if (type == NO_ORDER_IMBALANCE.ToString())
				return NO_ORDER_IMBALANCE_VALUE;
			if (type == MARKET_IMBALANCE_BUY_VALUE.ToString())
				return MARKET_IMBALANCE_BUY_VALUE;
			if (type == MARKET_IMBALANCE_SELL_VALUE.ToString())
				return MARKET_IMBALANCE_SELL_VALUE;
			if (type == NO_MARKET_IMBALANCE_VALUE.ToString())
				return NO_MARKET_IMBALANCE_VALUE;
			if (type == MOC_IMBALANCE_BUY_VALUE.ToString())
				return MOC_IMBALANCE_BUY_VALUE;
			if (type == MOC_IMBALANCE_SELL_VALUE.ToString())
				return MOC_IMBALANCE_SELL_VALUE;
			if (type == NO_MOC_IMBALANCE_VALUE.ToString())
				return NO_MOC_IMBALANCE_VALUE;
			if (type == ORDER_IMB_VALUE.ToString())
				return ORDER_IMB_VALUE;
			if (type == ORDER_INF_VALUE.ToString())
				return ORDER_INF_VALUE;
			if (type == ORDER_IMBALANCE_BUY_VALUE.ToString())
				return ORDER_IMBALANCE_BUY_VALUE;
			if (type == ORDER_IMBALANCE_SELL_VALUE.ToString())
				return ORDER_IMBALANCE_SELL_VALUE;
			if (type == NO_ORDER_IMBALANCE_VALUE.ToString())
				return NO_ORDER_IMBALANCE_VALUE;
			return UNKNOWN;
		}

		public static bool isMamdaOrderImbalanceType(int value)
		{
			bool imbalanceType = false;
			switch (value)
			{
				case MARKET_IMBALANCE_BUY_VALUE:
				case MARKET_IMBALANCE_SELL_VALUE:
				case MOC_IMBALANCE_BUY_VALUE:
				case MOC_IMBALANCE_SELL_VALUE:
				case ORDER_IMB_VALUE:
				case ORDER_INF_VALUE:
				case ORDER_IMBALANCE_BUY_VALUE:
				case ORDER_IMBALANCE_SELL_VALUE:
				case NO_MARKET_IMBALANCE_VALUE:
				case NO_MOC_IMBALANCE_VALUE:
				case NO_ORDER_IMBALANCE_VALUE:
					imbalanceType = true;
					break;
				default:
					imbalanceType = false;
					break;
			}
			return imbalanceType;
		}

		public static bool isMamdaImbalanceOrder(int value)
		{
			bool imbalanceOrder = false;
			switch (value)
			{
				case MARKET_IMBALANCE_BUY_VALUE:
				case MARKET_IMBALANCE_SELL_VALUE:
				case MOC_IMBALANCE_BUY_VALUE:
				case MOC_IMBALANCE_SELL_VALUE:
				case ORDER_IMB_VALUE:
				case ORDER_INF_VALUE:
				case ORDER_IMBALANCE_BUY_VALUE:
				case ORDER_IMBALANCE_SELL_VALUE:
					imbalanceOrder = true;
					break;
				case NO_MARKET_IMBALANCE_VALUE:
				case NO_MOC_IMBALANCE_VALUE:
				case NO_ORDER_IMBALANCE_VALUE:
					imbalanceOrder = false;
					break;
				default:
					imbalanceOrder = false;
					break;
			}
			return imbalanceOrder;
		}

		/// <summary>
		/// Compare the two types for equality.
		/// Returns true if the integer value of both types is equal. Otherwise
		/// returns false.
		/// </summary>
		/// <param name="imbDataType">The object to check equality against.</param>
		/// <returns>Whether the two objects are equal.</returns>
		public bool equals(MamdaOrderImbalanceType imbDataType)
		{
			return mValue == imbDataType.mValue;
		}

		public override bool Equals(object obj)
		{
			return (obj is MamdaOrderImbalanceType) && equals((MamdaOrderImbalanceType)obj);
		}

		public static bool operator==(MamdaOrderImbalanceType left, MamdaOrderImbalanceType right)
		{
			return Object.ReferenceEquals(left, right) || (left != null && left.equals(right));
		}

		public static bool operator!=(MamdaOrderImbalanceType left, MamdaOrderImbalanceType right)
		{
			return !(left == right);
		}

	}
}
