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
using Wombat;

namespace Wombat.Mamda.Examples
{
	/// <summary>
	/// UnderlyingTicker is a class that we can use to handle updates
	/// to the underlying security.  This example just prints
	/// individual trades and quotes.
	/// </summary>
    class UnderlyingTicker :
		MamdaTradeHandler,
        MamdaQuoteHandler
    {
        public UnderlyingTicker(
			MamdaOptionChain chain,
			bool printStrikes)
        {
            myChain = chain;
			myPrintStrikes = printStrikes;
        }

        public void onTradeRecap(
            MamdaSubscription   sub,
            MamdaTradeListener  listener,
            MamaMsg             msg,
            MamdaTradeRecap     recap)
        {
        }

        public void onTradeReport(
            MamdaSubscription   sub,
            MamdaTradeListener  listener,
            MamaMsg             msg,
            MamdaTradeReport    trade,
            MamdaTradeRecap     recap)
        {
            Console.WriteLine("Underlying trade: " + trade.getTradeVolume () +
                               " @ " + trade.getTradePrice ());
        }

        public void onTradeGap(
            MamdaSubscription   sub,
            MamdaTradeListener  listener,
            MamaMsg             msg,
            MamdaTradeGap       gap,
            MamdaTradeRecap     recap)
        {
        }

        public void onTradeCancelOrError(
            MamdaSubscription        sub,
            MamdaTradeListener       listener,
            MamaMsg                  msg,
            MamdaTradeCancelOrError  cancelOrError,
            MamdaTradeRecap          recap)
        {
        }

        public void onTradeCorrection(
            MamdaSubscription        sub,
            MamdaTradeListener       listener,
            MamaMsg                  msg,
            MamdaTradeCorrection     tradeCorrection,
            MamdaTradeRecap          recap)
        {
        }

        public void onTradeClosing(
            MamdaSubscription        sub,
            MamdaTradeListener       listener,
            MamaMsg                  msg,
            MamdaTradeClosing        closingEvent,
            MamdaTradeRecap          recap)
        {
        }

        public void onQuoteRecap(
            MamdaSubscription   sub,
            MamdaQuoteListener  listener,
            MamaMsg             msg,
            MamdaQuoteRecap     recap)
        {
        }

        public void onQuoteUpdate(
            MamdaSubscription   sub,
            MamdaQuoteListener  listener,
            MamaMsg             msg,
            MamdaQuoteUpdate    quote,
            MamdaQuoteRecap     recap)
        {
            Console.WriteLine(
                "Underlying quote: " +
                quote.getBidSize () + "x" + quote.getBidPrice () + "   " +
                quote.getAskPrice () + "x" + quote.getAskSize () + "   " +
                "mid=" + recap.getQuoteMidPrice ());

			if (myPrintStrikes)
			{
				double lowStrike  = -1.0;
				double highStrike = -1.0;

				Console.WriteLine(
					"  strikes within 15%: " + lowStrike + " " + highStrike);
			}
        }

        public void onQuoteGap(
            MamdaSubscription   sub,
            MamdaQuoteListener  listener,
            MamaMsg             msg,
            MamdaQuoteGap       gap,
            MamdaQuoteRecap     recap)
        {
        }

        public void onQuoteClosing(
            MamdaSubscription   sub,
            MamdaQuoteListener  listener,
            MamaMsg             msg,
            MamdaQuoteClosing   closingEvent,
            MamdaQuoteRecap     recap)
        {
        }

		private MamdaOptionChain myChain = null;
		private bool             myPrintStrikes = false;
	}
}
