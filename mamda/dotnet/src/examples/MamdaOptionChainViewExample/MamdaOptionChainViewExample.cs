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
using System.Threading;
using Wombat.Containers;
using System.Collections;

namespace Wombat.Mamda.Examples
{
	/// <summary>
	/// An example of MAMDA option chain "view" processing.  A "view" on an
	/// option chain is the set of option contracts that fits into a
	/// sliding set of strike prices and expiration cycles.  The strike
	/// prices included in the view may change as the price of underlying
	/// security changes.
	///
	/// <p>This example suffers from a minor lack of configurability: while
	/// it does support subscriptions to multiple underlying symbols (with
	/// multiple -s options), it does not allow a different source to be
	/// specified for each option.  Of course, any real world example would
	/// deal with this properly.</p>
	/// </summary>
	public class MamdaOptionChainViewExample
	{
        private static ArrayList mamdaSubscriptions = new ArrayList();
        private static ArrayList optionSubscriptions = new ArrayList();
		public static void Main(string[] args)
		{
			MamaTransport        baseTransport    = null;
			MamaTransport        optionTransport  = null;
			MamaTransport        dictTransport    = null;
			MamaQueue            defaultQueue     = null;
			MamaDictionary       dictionary       = null;
			CommandLineProcessor options = new CommandLineProcessor (args);

			if (options.hasLogLevel())
			{
				logLevel_ = options.getLogLevel();
				Mama.enableLogging(logLevel_);
			}

			try
			{
				// Initialize MAMA
				myBridge = new MamaBridge(options.getMiddleware());
				Mama.open ();

				// Initialize transports.  We're assuming that options and
				// underlyings might be on different transports.  Note:
				// some companies might use multiple transports for
				// underlyings (e.g., CTA and NASDAQ), which case it woud
				// be necessary to create three transports here and be
				// sure to pass the correct transport later.
				baseTransport = new MamaTransport();
				baseTransport.create(options.getTransport(), myBridge);
				optionTransport = baseTransport;
				defaultQueue = Mama.getDefaultEventQueue(myBridge);

				// The dictionary transport might be different from the
				// other transports (although in this example we're just
				// using the same transport name as the underlying).
				dictTransport = baseTransport;

				// Fetch and initialize the data dictionary
				MamaSource dictionarySource = new MamaSource();
                dictionarySource.symbolNamespace = options.getDictSource();
				dictionarySource.transport = baseTransport;
				dictionary = buildDataDictionary(dictTransport, defaultQueue, dictionarySource);
				MamdaQuoteFields.setDictionary(dictionary, null);
				MamdaTradeFields.setDictionary(dictionary, null);
				MamdaFundamentalFields.setDictionary(dictionary, null);
				MamdaOptionFields.setDictionary(dictionary, null);

				// Create listeners for each chain.  Two subscriptions are
				// necessary.
				foreach (string symbol in options.getSymbolList())
				{
					// Create chain and listener objects.
					MamdaTradeListener aBaseTradeListener = new MamdaTradeListener();
					MamdaQuoteListener aBaseQuoteListener = new MamdaQuoteListener();
					MamdaOptionChain anOptionChain = new MamdaOptionChain (symbol);
					anOptionChain.setUnderlyingQuoteListener(aBaseQuoteListener);
					anOptionChain.setUnderlyingTradeListener(aBaseTradeListener);
					MamdaOptionChainListener  anOptionListener =
						new MamdaOptionChainListener(anOptionChain);

					// Set up the view (could be configurable, of course):
					MamdaOptionChainView anOptionView =
						new MamdaOptionChainView (anOptionChain);
					anOptionView.setNumberOfExpirations(2);
					anOptionView.setStrikeRangeNumber(4);
					anOptionView.setJitterMargin(1.0);

					// Create our handlers (the UnderlyingTicker and
					// OptionChainDisplay could be a single class).
					UnderlyingTicker  aBaseTicker =
						new UnderlyingTicker(anOptionChain, false);
					OptionChainDisplay  aDisplay =
						new OptionChainDisplay(anOptionView);

					// Create subscriptions for underlying and option chain:
					MamdaSubscription  aBaseSubscription = new MamdaSubscription();
					MamdaSubscription  anOptionSubscription =
						new MamdaSubscription();

					// Register for underlying quote and trade events.
					aBaseTradeListener.addHandler(aBaseTicker);
					aBaseQuoteListener.addHandler(aBaseTicker);
					aBaseSubscription.addMsgListener(aBaseTradeListener);
					aBaseSubscription.addMsgListener(aBaseQuoteListener);

					aBaseSubscription.create(
						baseTransport,
                        defaultQueue,
						options.getSource(),
                        symbol,
						null);
                    mamdaSubscriptions.Add(aBaseSubscription);
					// Register for underlying option events.
					anOptionListener.addHandler(aDisplay);
					anOptionListener.addHandler(anOptionView);

					// We set the timeout to 1 for this example because we
					// currently use the timeout feature to determine when
					// to say that we have received all of the initials.
					// There will be a separate time interval for this in
					// the future.
					anOptionSubscription.setTimeout(1);
					anOptionSubscription.addMsgListener(anOptionListener);
					anOptionSubscription.addStaleListener(aDisplay);
					anOptionSubscription.addErrorListener(aDisplay);
					anOptionSubscription.setType(mamaSubscriptionType.MAMA_SUBSC_TYPE_GROUP);
					anOptionSubscription.create(
						optionTransport,
                        defaultQueue,
                        options.getSource(),
						symbol,
						null);
                     optionSubscriptions.Add(anOptionSubscription);
				}

				//Start dispatching on the default event queue
				Mama.start(myBridge);
				GC.KeepAlive(dictionary);

				Console.WriteLine("Press ENTER or Ctrl-C to quit...");
				Console.ReadLine();
			}
			catch (Exception e)
			{
				Console.WriteLine(e.ToString());
				Environment.Exit(1);
			}
		}

		private class OptionChainDisplay :
			MamdaOptionChainHandler,
			MamdaStaleListener,
			MamdaErrorListener
		{
			MamdaOptionChainView  myView  = null;
			bool myGotRecap = false;  // Updated when we get the first recap.

			public OptionChainDisplay(MamdaOptionChainView view)
			{
				myView  = view;
			}

			public void onOptionChainRecap(
				MamdaSubscription         subscription,
				MamdaOptionChainListener  listener,
				MamaMsg                   msg,
				MamdaOptionChain          chain)
			{
				myGotRecap = true;
				printView();
			}

			public void onOptionSeriesUpdate(
				MamdaSubscription         subscription,
				MamdaOptionChainListener  listener,
				MamaMsg                   msg,
				MamdaOptionSeriesUpdate   update,
				MamdaOptionChain          chain)
			{
				printView();
			}

			public void onOptionChainGap(
				MamdaSubscription         subscription,
				MamdaOptionChainListener  listener,
				MamaMsg                   msg,
				MamdaOptionChain          chain)
			{
				printView ();
			}

			public void onOptionContractCreate(
				MamdaSubscription         subscription,
				MamdaOptionChainListener  listener,
				MamaMsg                   msg,
				MamdaOptionContract       contract,
				MamdaOptionChain          chain)
			{

				// (Optional: create a trade/quote handler for the
				// individual option contract.)
				OptionContractTicker aTicker =
					new OptionContractTicker(contract, myView, this);
				contract.addQuoteHandler (aTicker);
				contract.addTradeHandler (aTicker);
			}

			public void onStale(
				MamdaSubscription   subscription,
				mamaQuality         quality)
			{
				Console.WriteLine("Stale (" + subscription.getSymbol() + "): ");
			}

			public void onError(
				MamdaSubscription   subscription,
				MamdaErrorSeverity  severity,
				MamdaErrorCode      errorCode,
				string              errorStr)
			{
				Console.WriteLine("Error (" + subscription.getSymbol() + "): " + errorStr);
			}

			public void printView()
			{
				if (!myGotRecap)
					return;

				Console.WriteLine("Chain: " + myView.getSymbol());

				// Loop through the expiration date sets (within the view):
				Iterator expireIter = myView.expirationIterator();
				while (expireIter.hasNext())
				{
					MamdaOptionExpirationStrikes expireStrikes =
						(MamdaOptionExpirationStrikes) expireIter.next();
					printExpireStrikes (expireStrikes);
				}

				Console.WriteLine("\nDone with chain\n");
			}

			public void printExpireStrikes(
				MamdaOptionExpirationStrikes expireStrikes)
			{
				// Loop through the strike prices (within the view):
				Iterator strikeIter = expireStrikes.values().iterator();
				while (strikeIter.hasNext())
				{
					MamdaOptionStrikeSet strikeSet =
						(MamdaOptionStrikeSet)strikeIter.next();

					MamdaOptionContractSet callContracts = strikeSet.getCallSet();
					MamdaOptionContractSet putContracts  = strikeSet.getPutSet();
					String  expireDateStr = strikeSet.getExpireDateStr();
					double  strikePrice   = strikeSet.getStrikePrice();

					Console.WriteLine ();
					Console.WriteLine ("Strike: " + expireDateStr + " " +
						strikePrice);
					printContractSet (callContracts, "Call");
					printContractSet (putContracts,  "Put");
				}
			}

			public void printContractSet(
				MamdaOptionContractSet  contractSet,
				String                  putCallStr)
			{
				if (contractSet == null)
					return;
				MamdaOptionContract  bbo   = contractSet.getBboContract();
				MamdaOptionContract  wbbo  = contractSet.getWombatBboContract();
				Collection           exchs = contractSet.getExchangeContracts();
				Iterator             exchIter = exchs.iterator();

				Console.WriteLine("  " + putCallStr + " options:");
				printContract (bbo);
				printContract (wbbo);
				while (exchIter.hasNext())
				{
					MamdaOptionContract exch =(MamdaOptionContract)exchIter.next();
					printContract (exch);
				}
			}

			public void printContract(MamdaOptionContract contract)
			{
				if (!myGotRecap)
					return;
				if (contract == null)
					return;

				MamdaTradeRecap tradeRecap = contract.getTradeInfo();
				MamdaQuoteRecap quoteRecap = contract.getQuoteInfo();
				String symbol       = contract.getSymbol();
				String exchange     = contract.getExchange();
				String expireDate   = contract.getExpireDateStr();
				double strikePrice  = contract.getStrikePrice();
				double lastPrice    = tradeRecap.getLastPrice().getValue();
				long   accVolume    = tradeRecap.getAccVolume();
				double bidPrice     = quoteRecap.getBidPrice().getValue();
				double askPrice     = quoteRecap.getAskPrice().getValue();

				Console.WriteLine ("    " +
					expireDate + " "   + strikePrice + " ("    +
					symbol     + " "   + exchange    + ")  | " +
					lastPrice  + " | " + bidPrice    + " | "   +
					askPrice   + " | " + accVolume);
			}
		}

		/// <summary>
		/// OptionContractTicker is a class that we can use to handle
		/// updates to individual option contracts.  This example just
		/// prints individual trades and quotes.  Instances of this class
		/// are associated with actual option contracts in the
		/// OptionChainDisplay.onOptionSeriesUpdate method, above.  The
		/// trade and quote information of the underlying is accessible via
		/// the chain argument.
		/// </summary>
		private class OptionContractTicker :
			MamdaTradeHandler,
			MamdaQuoteHandler
		{
			MamdaOptionContract   myContract = null;
			MamdaOptionChainView  myView     = null;
			OptionChainDisplay    myDisplay  = null;

			public OptionContractTicker(
				MamdaOptionContract   contract,
				MamdaOptionChainView  view,
				OptionChainDisplay    display)
			{
				myContract            = contract;
				myView                = view;
				myDisplay             = display;
			}

			public void onTradeRecap(
				MamdaSubscription   sub,
				MamdaTradeListener  listener,
				MamaMsg             msg,
				MamdaTradeRecap     recap)
			{
				// Refresh the "display" of this contract.
				if (myView.isVisible(myContract))
					myDisplay.printContract (myContract);
			}

			public void onTradeReport(
				MamdaSubscription   sub,
				MamdaTradeListener  listener,
				MamaMsg             msg,
				MamdaTradeReport    trade,
				MamdaTradeRecap     recap)
			{
				// Do something with a specific trade report.
				if (myView.isVisible(myContract))
					myDisplay.printContract (myContract);
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
				MamdaTradeClosing        tradeClosing,
				MamdaTradeRecap          recap)
			{
			}

			public void onQuoteRecap(
				MamdaSubscription   sub,
				MamdaQuoteListener  listener,
				MamaMsg             msg,
				MamdaQuoteRecap     recap)
			{
				// Refresh the "display" of this contract.
				if (myView.isVisible(myContract))
					myDisplay.printContract (myContract);
			}

			public void onQuoteUpdate(
				MamdaSubscription   sub,
				MamdaQuoteListener  listener,
				MamaMsg             msg,
				MamdaQuoteUpdate    update,
				MamdaQuoteRecap     recap)
			{
				// Do something with a specific quote.
				if (myView.isVisible(myContract))
					myDisplay.printContract (myContract);
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
				MamdaQuoteClosing   quoteClosing,
				MamdaQuoteRecap     recap)
			{
			}
		}

		private static MamaDictionary buildDataDictionary(
			MamaTransport transport,
			MamaQueue defaultQueue,
			MamaSource dictionarySource)
		{
			bool[] gotDict = new bool[] { false };
			MamaDictionaryCallback dictionaryCallback = new DictionaryCallback(gotDict);
			lock (dictionaryCallback)
			{
				MamaSubscription subscription = new MamaSubscription ();

				MamaDictionary dictionary = new MamaDictionary();
				dictionary.create(
					defaultQueue,
					dictionaryCallback,
					dictionarySource,
					3,
					10);

				Mama.start(myBridge);
				if (!gotDict[0])
				{
					if (!Monitor.TryEnter(dictionaryCallback, 30000))
					{
						Console.Error.WriteLine("Timed out waiting for dictionary.");
						Environment.Exit(0);
					}
					Monitor.Exit(dictionaryCallback);
				}
				return dictionary;
			}
		}

		private class DictionaryCallback : MamaDictionaryCallback
		{
			public DictionaryCallback(bool[] gotDict)
			{
				gotDict_ = gotDict;
			}

			public void onTimeout(MamaDictionary dictionary)
			{
				Console.Error.WriteLine("Timed out waiting for dictionary");
				Environment.Exit(1);
			}

			public void onError(MamaDictionary dictionary, string message)
			{
				Console.Error.WriteLine("Error getting dictionary: {0}", message);
				Environment.Exit(1);
			}

			public void onComplete(MamaDictionary dictionary)
			{
				lock (this)
				{
					gotDict_[0] = true;
					Mama.stop(myBridge);
					Monitor.PulseAll(this);
				}
			}

			private bool[] gotDict_;
		}

		private static MamaLogLevel	logLevel_ = MamaLogLevel.MAMA_LOG_LEVEL_NORMAL;
		private static object       guard_ = new object();
		private static MamaBridge	myBridge = null;
	}
}
