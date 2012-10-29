/* $Id: ComboTicker.cs,v 1.2.32.4 2012/08/24 16:12:13 clintonmcdowell Exp $
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
using System.Threading;
using Wombat;

namespace Wombat.Mamda.Examples
{
	/// <summary>
	/// Is a MamdaTradeHandler and a MamdaQuoteHandler.
	/// 
	/// Can be used to handler callbacks from both a TradeListener and a
	/// QuoteListener.
	/// </summary>
	class ComboTicker :
		MamdaTradeHandler,
		MamdaQuoteHandler,
		MamdaStaleListener,
		MamdaErrorListener
	{
		public void onTradeRecap (
			MamdaSubscription   sub,
			MamdaTradeListener  listener,
			MamaMsg             msg,
			MamdaTradeRecap     recap)
		{
			Console.WriteLine("Trade Recap (" + msg.getString
							(MamdaCommonFields.ISSUE_SYMBOL)+ "): ");
		}

		public void onTradeReport (
			MamdaSubscription   sub,
			MamdaTradeListener  listener,
			MamaMsg             msg,
			MamdaTradeReport    trade,
			MamdaTradeRecap     recap)
		{
			Console.WriteLine ("Trade ("  + msg.getString
											(MamdaCommonFields.ISSUE_SYMBOL) +
								":"        + recap.getTradeCount()    +
								"):  "     + trade.getTradeVolume()   +
								" @ "      + trade.getTradePrice()    +
								" (seq#: " + trade.getEventSeqNum()   +
								"; time: " + trade.getEventTime()     +
								"; qual: " + trade.getTradeQual()     +
								"; acttime: " + trade.getActivityTime() + ")");
		}

		public void onTradeGap (
			MamdaSubscription   sub,
			MamdaTradeListener  listener,
			MamaMsg             msg,
			MamdaTradeGap       gapEvent,
			MamdaTradeRecap     recap)
		{
			Console.WriteLine("Trade gap  (" +  msg.getString
							(MamdaCommonFields.ISSUE_SYMBOL) +
							":"+   gapEvent.getBeginGapSeqNum() +
							"-" + gapEvent.getEndGapSeqNum() + ")");
		}

		public void onTradeCancelOrError (
			MamdaSubscription        sub,
			MamdaTradeListener       listener,
			MamaMsg                  msg,
			MamdaTradeCancelOrError  cancelOrError,
			MamdaTradeRecap          recap)
		{
			Console.WriteLine("Trade error/cancel (" + sub.getSymbol() + "): ");
		}

		public void onTradeCorrection (
			MamdaSubscription        sub,
			MamdaTradeListener       listener,
			MamaMsg                  msg,
			MamdaTradeCorrection     correction,
			MamdaTradeRecap          recap)
		{
			Console.WriteLine("Trade correction (" + sub.getSymbol() + "): ");
		}

		public void onTradeClosing (
			MamdaSubscription        sub,
			MamdaTradeListener       listener,
			MamaMsg                  msg,
			MamdaTradeClosing        closingEvent,
			MamdaTradeRecap          recap)
		{
			Console.WriteLine("Trade Closing (" + sub.getSymbol() + "): ");
		}

		public void onQuoteRecap (
			MamdaSubscription   sub,
			MamdaQuoteListener  listener,
			MamaMsg             msg,
			MamdaQuoteRecap     recap)
		{
			Console.WriteLine ("Quote Recap (" + msg.getString
							(MamdaCommonFields.ISSUE_SYMBOL)+ "): ");
		}

		public void onQuoteUpdate (
			MamdaSubscription   sub,
			MamdaQuoteListener  listener,
			MamaMsg             msg,
			MamdaQuoteUpdate    update,
			MamdaQuoteRecap     recap)
		{
			Console.WriteLine ("Quote ("  + msg.getString
								(MamdaCommonFields.ISSUE_SYMBOL)   +
								":"        + recap.getQuoteCount()  +
								"):  "     + update.getBidPrice()    +
								" "        + update.getBidSize()     +
								" X "      + update.getAskSize()     +
								" "        + update.getAskPrice()    +
								" (seq#: " + update.getEventSeqNum() +
								"; time: " + update.getEventTime()   +
								"; qual: " + update.getQuoteQual()   + ")");
		}

		public void onQuoteGap (
			MamdaSubscription   sub,
			MamdaQuoteListener  listener,
			MamaMsg             msg,
			MamdaQuoteGap       gapEvent,
			MamdaQuoteRecap     recap)
		{
			Console.WriteLine("Quote gap (" + gapEvent.getBeginGapSeqNum() +
							"-" + gapEvent.getEndGapSeqNum() + ")");
		}

		public void onQuoteClosing (
			MamdaSubscription   sub,
			MamdaQuoteListener  listener,
			MamaMsg             msg,
			MamdaQuoteClosing   closingEvent,
			MamdaQuoteRecap     recap)
		{
			Console.WriteLine("Quote Closing (" + sub.getSymbol() + "): ");
		}

		public void onStale (
			MamdaSubscription   subscription,
			mamaQuality         quality)
		{
			Console.WriteLine("Stale (" + subscription.getSymbol() + "): ");
		}

		public void onError (
			MamdaSubscription   subscription,
			MamdaErrorSeverity  severity,
			MamdaErrorCode      errorCode,
			string              errorStr)
		{
			Console.WriteLine("Error (" + subscription.getSymbol() + "): ");
		}
	}
}
