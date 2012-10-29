/* $Id: MamdaOptionContractSet.cs,v 1.2.38.5 2012/08/24 16:12:12 clintonmcdowell Exp $
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
using Wombat.Containers;

namespace Wombat
{
	/// <summary>
	/// A class that represents a set of option contracts at a given strike
	/// price.
	/// </summary>
	public class MamdaOptionContractSet
	{
		public MamdaOptionContractSet()
		{
			mExchangeContracts = new TreeMap();
		}

		/// <summary>
		/// Set the contract for the best bid and offer.
		/// </summary>
		/// <param name="contract"></param>
		public void setBboContract(MamdaOptionContract contract)
		{
			mBboContract = contract;
		}

		/// <summary>
		/// Set the contract for the best bid and offer, as calculated
		/// by Wombat.
		/// </summary>
		/// <param name="contract"></param>
		public void setWombatBboContract(MamdaOptionContract contract)
		{
			mWombatmBboContract = contract;
		}

		/// <summary>
		/// Set the contract for the particular exchange.
		/// </summary>
		/// <param name="exchange"></param>
		/// <param name="contract"></param>
		public void setExchangeContract(
			string               exchange,
			MamdaOptionContract  contract)
		{
			mExchangeContracts.put(exchange, contract);
		}
		
		/// <summary>
		/// Return the contract for the best bid and offer.
		/// </summary>
		/// <returns></returns>
		public MamdaOptionContract getBboContract()
		{
			return mBboContract;
		}

		/// <summary>
		/// Return the contract for the best bid and offer, as calculated
		/// by Wombat.
		/// </summary>
		/// <returns></returns>
		public MamdaOptionContract getWombatBboContract()
		{
			return mWombatmBboContract;
		}

		/// <summary>
		/// Return the contract for the particular exchange.
		/// </summary>
		/// <param name="exchange"></param>
		/// <returns></returns>
		public MamdaOptionContract getExchangeContract(string exchange)
		{
			return (MamdaOptionContract)mExchangeContracts.valueOf(exchange);
		}

		/// <summary>
		/// Return the set of individual regional exchange option
		/// contracts.  The type of object in the set is a instance of
		/// MamdaOptionContract.
		/// </summary>
		/// <returns></returns>
		public Collection getExchangeContracts()
		{
			return mExchangeContracts.values();
		}

		/// <summary>
		/// Return the set of individual regional exchange identifiers
		/// The type of object in the set is a instance of String.
		/// </summary>
		/// <returns></returns>
		public Set getExchanges()
		{
			return mExchangeContracts.keySet();
		}
	
		private MamdaOptionContract  mBboContract       = null;
		private MamdaOptionContract  mWombatmBboContract = null;
		private TreeMap              mExchangeContracts = null;
	}
}
