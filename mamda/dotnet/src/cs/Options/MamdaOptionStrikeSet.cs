/* $Id: MamdaOptionStrikeSet.cs,v 1.2.38.5 2012/08/24 16:12:12 clintonmcdowell Exp $
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
	/// A class that represents the call and put contract sets at a given
	/// strike price.
	/// </summary>
	public class MamdaOptionStrikeSet
	{
		public MamdaOptionStrikeSet(
			DateTime	expireDate,
			double		strikePrice)
		{
			mExpireDate  = expireDate.Date; // cut the time
			mStrikePrice = strikePrice;
		}

		/// <summary>
		/// Return a set of call option contracts at a given strike price.
		/// </summary>
		/// <returns>Set of call contracts at the given strike price.</returns>
		public MamdaOptionContractSet getCallSet()
		{
			return mCallSet;
		}

		/// <summary>
		/// Return a set of put option contracts at a given strike price.
		/// </summary>
		/// <returns>MamdaOptionContractSet Set of put contracts at the
		/// given strike price.</returns>
		public MamdaOptionContractSet getPutSet()
		{
			return mPutSet;
		}

		/// <summary>
		/// Return the expiration date for the contracts at the given strike price.
		/// </summary>
		/// <returns>The expiration date.</returns>
		public DateTime getExpireDate ()
		{
			return mExpireDate;
		}

		/// <summary>
		/// Return the expiration date as a string for the contracts at the
		/// given strike price.
		/// </summary>
		/// <returns>The expiration date.</returns>
		public string getExpireDateStr()
		{
			return mDateFormat.Format(mExpireDate);
		}

		/// <summary>
		/// Return the strike price.
		/// </summary>
		/// <returns>The strike price.</returns>
		public double getStrikePrice ()
		{
			return mStrikePrice;
		}

		private readonly MamdaOptionContractSet mCallSet = new MamdaOptionContractSet();
		private readonly MamdaOptionContractSet mPutSet = new MamdaOptionContractSet();
		private readonly SimpleDateFormat mDateFormat = new SimpleDateFormat("MMMyy");
		private readonly DateTime mExpireDate;
		private readonly double mStrikePrice;
	}
}
