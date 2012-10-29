/* $Id: MamdaUncrossPriceInd.cs,v 1.1.4.5 2012/09/07 07:44:57 ianbell Exp $
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
	/// Security Status values
	/// </summary>
	public class MamdaUncrossPriceInd
	{
		/// <summary>
		/// An enumeration representing the status of a security
		/// such as whether or not it is halted or closed for trading.
		/// </summary>
		public enum mamdaUncrossPriceInd
		{
			UNCROSS_NONE                = 0,
			UNCROSS_INDICATIVE          = 1,
			UNCROSS_FIRM                = 2,
			UNCROSS_INSUFFICIENT_VOL    = 3
		}

		/// <summary>
		/// Only used internally
		/// </summary>
		private static string UNCROSS_NONE_STR              = "None";
		private static string UNCROSS_INDICATIVE_STR        = "I";
		private static string UNCROSS_FIRM_STR              = "F";
		private static string UNCROSS_INSUFFICIENT_VOL_STR  = "V";


		/// <summary>
		/// Convert a MamdaSecurityStatus to an appropriate, displayable string
		/// </summary>
		public static string toString (MamdaUncrossPriceInd.mamdaUncrossPriceInd priceInd)
		{
			switch (priceInd)
			{
				case  mamdaUncrossPriceInd.UNCROSS_INDICATIVE:			return UNCROSS_INDICATIVE_STR;
				case  mamdaUncrossPriceInd.UNCROSS_FIRM:		        return UNCROSS_FIRM_STR;
				case  mamdaUncrossPriceInd.UNCROSS_INSUFFICIENT_VOL:    return UNCROSS_INSUFFICIENT_VOL_STR;
				default:
					return "None";
			}
		}

		/// <summary>
		/// Convert a string representation of a security status to the enumeration.
		/// </summary>
		public static MamdaUncrossPriceInd.mamdaUncrossPriceInd mamdaAuctionPriceIndFromString (string priceInd)
		{
			if (priceInd == null)
			{
				return mamdaUncrossPriceInd.UNCROSS_NONE;
			}

			if (priceInd == UNCROSS_NONE_STR)
				return mamdaUncrossPriceInd.UNCROSS_NONE;
			if (priceInd == UNCROSS_INDICATIVE_STR)
				return mamdaUncrossPriceInd.UNCROSS_INDICATIVE;
            if (priceInd == UNCROSS_FIRM_STR)
				return mamdaUncrossPriceInd.UNCROSS_FIRM;
			if (priceInd == UNCROSS_INSUFFICIENT_VOL_STR)
				return mamdaUncrossPriceInd.UNCROSS_INSUFFICIENT_VOL;

			if (priceInd == "0")
				return mamdaUncrossPriceInd.UNCROSS_NONE;
			if (priceInd == "1")
				return mamdaUncrossPriceInd.UNCROSS_INDICATIVE;
			if (priceInd == "2")
				return mamdaUncrossPriceInd.UNCROSS_FIRM;
			if (priceInd == "3")
				return mamdaUncrossPriceInd.UNCROSS_INSUFFICIENT_VOL;


			return mamdaUncrossPriceInd.UNCROSS_NONE;
		}
	}
}
