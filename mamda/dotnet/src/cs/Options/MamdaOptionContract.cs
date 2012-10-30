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
	/// A class that represents a single option contract.  Instances of
	/// this object are typically created by the MamdaOptionChainListener.
	/// Applications may attach a custom object to each instance of
	/// MamdaOptionContract.
	/// </summary>
	/// <remarks>
	/// Note: User applications can be notified of creation of
	/// MamdaOptionContract instances via the
	/// MamdaOptionChainListener.onOptionContractCreate() method.
	/// </remarks>
	public class MamdaOptionContract
	{
		public enum PutOrCall : byte
		{
			Unknown = (byte)'Z',
			Call	= (byte)'C',
			Put		= (byte)'P'
		}
		
        public enum ExerciseStyle : byte
        {
            Unknown  = (byte)'Z', 
            American = (byte)'A',
            European = (byte)'E',
            Capped   = (byte)'C'
        }      
         
		private MamdaOptionContract()
		{
			// Hide the default constructor.
		}

		/// <summary>
		/// Constructor from expiration date, strike price, and put/call
		/// indicator.
		/// </summary>
		/// <param name="symbol"></param>
		/// <param name="exchange"></param>
		/// <param name="expireDate"></param>
		/// <param name="strikePrice"></param>
		/// <param name="putCall"></param>
		public MamdaOptionContract(
			string		symbol,
			string		exchange,
			DateTime	expireDate,
			double		strikePrice,
			PutOrCall	putCall)
		{
			mSymbol        = symbol;
			mExchange      = exchange;
			mExpireDate    = expireDate;
			mStrikePrice   = strikePrice;
			mPutCall       = putCall;
		}

		/// <summary>
		/// Set the open interest size.
		/// </summary>
		/// <param name="openInterest"></param>
		public void setOpenInterest(long openInterest)
		{
			mOpenInterest = openInterest;
		}

		/// <summary>
		/// Set the exercise style.
		/// </summary>
		/// <param name="exerciseStyle"></param>
		public void setExerciseStyle(ExerciseStyle exerciseStyle)
		{
			mExerciseStyle = exerciseStyle;
		}

		/// <summary>
		/// Return the OPRA contract symbol.
		/// </summary>
		/// <returns></returns>
		public string getSymbol()
		{
			return mSymbol;
		}

		/// <summary>
		/// Return the exchange.
		/// </summary>
		/// <returns></returns>
		public string getExchange()
		{
			return mExchange;
		}

		/// <summary>
		/// Return the expiration date.
		/// </summary>
		/// <returns></returns>
		public DateTime getExpireDate()
		{
			return mExpireDate;
		}

		/// <summary>
		/// Return the expiration date as a commonly formatted string (MMyy).
		/// </summary>
		/// <returns></returns>
		public string getExpireDateStr()
		{
			return mExpireFormat.Format(mExpireDate);
		}

		/// <summary>
		/// Return the strike price.
		/// </summary>
		/// <returns></returns>
		public double getStrikePrice()
		{
			return mStrikePrice;
		}

		/// <summary>
		/// Return the put/call indicator.
		/// </summary>
		/// <returns></returns>
		public PutOrCall getPutCall()
		{
			return mPutCall;
		}

		/// <summary>
		/// Return the open interest.
		/// </summary>
		/// <returns></returns>
		public long getOpenInterest()
		{
			return mOpenInterest;
		}

		/// <summary>
		/// Return the exercise style.
		/// </summary>
		/// <returns></returns>
		public ExerciseStyle getExerciseStyle()
		{
			return mExerciseStyle;
		}

		/// <summary>
		/// Add a MamdaTradeHandler for handling trade updates to this option contract.
		/// </summary>
		/// <param name="handler"></param>
		public void addTradeHandler(MamdaTradeHandler handler)
		{
			mTradeListener.addHandler(handler);
		}

		/// <summary>
		/// Add a MamdaQuoteHandler for handling quote updates to this
		/// option contract.
		/// </summary>
		/// <param name="handler"></param>
		public void addQuoteHandler(MamdaQuoteHandler handler)
		{
			mQuoteListener.addHandler(handler);
		}

		/// <summary>
		/// Add a MamdaFundamentalHandler for handling fundamental updates
		/// to this option contract.
		/// </summary>
		/// <param name="handler"></param>
		public void addFundamentalHandler(MamdaFundamentalHandler handler)
		{
			mFundamentalListeners.addHandler(handler);
		}

		/// <summary>
		/// Add a custom object to this option contract.  Such an object
		/// might contain customer per-contract data.
		/// </summary>
		/// <param name="obj"></param>
		public void setCustomObject(object obj)
		{
			mCustomObject = obj;
		}

		/// <summary>
		/// Return the current trade fields.
		/// </summary>
		/// <returns></returns>
		public MamdaTradeRecap getTradeInfo()
		{
			return mTradeListener;
		}

		/// <summary>
		/// Return the current quote fields.
		/// </summary>
		/// <returns></returns>
		public MamdaQuoteRecap getQuoteInfo()
		{
			return mQuoteListener;
		}

		/// <summary>
		/// Return the current fundamental fields.
		/// </summary>
		/// <returns></returns>
		public MamdaFundamentals getFundamentalsInfo()
		{
			return mFundamentalListeners;
		}

		/// <summary>
		/// Return the custom object.
		/// </summary>
		/// <returns></returns>
		public object getCustomObject()
		{
			return mCustomObject;
		}

		/// <summary>
		/// Return the trade listener.
		/// </summary>
		/// <returns></returns>
		public MamdaTradeListener getTradeListener()
		{
			return mTradeListener;
		}

		/// <summary>
		/// Return the current quote listener.
		/// </summary>
		/// <returns></returns>
		public MamdaQuoteListener getQuoteListener()
		{
			return mQuoteListener;
		}

		/// <summary>
		/// Return the current fundamental listener.
		/// </summary>
		/// <returns></returns>
		public MamdaFundamentalListener getFundamentalListener()
		{
			return mFundamentalListeners;
		}

		/// <summary>
		/// Set whether this contract is in the "view" within the option
		/// chain.  
		/// <see cref="MamdaOptionChain"/>
		/// </summary>
		/// <param name="inView"></param>
		public void setInView(bool inView)
		{
			mInView = inView;
		}

		/// <summary>
		/// Return whether this contract is in the "view" within the option
		/// chain.
		///	<see cref="MamdaOptionChain"/>
		/// </summary>
		/// <returns></returns>
		public bool getInView()
		{
			return mInView;
		}
		
		private string              mSymbol;
		private string              mExchange;
		private DateTime            mExpireDate    = DateTime.MinValue;
		private double              mStrikePrice   = 0.0;
		private PutOrCall           mPutCall       = PutOrCall.Unknown;
		private long                mOpenInterest;
		private ExerciseStyle       mExerciseStyle;
		private bool                mInScope;
		private MamdaTradeListener  mTradeListener = new MamdaTradeListener();
		private MamdaQuoteListener  mQuoteListener = new MamdaQuoteListener();
		private MamdaFundamentalListener mFundamentalListeners = new MamdaFundamentalListener();
		private object              mCustomObject;
		private bool                mInView;
		private SimpleDateFormat    mExpireFormat = new SimpleDateFormat("MMMyy");
	}
}
