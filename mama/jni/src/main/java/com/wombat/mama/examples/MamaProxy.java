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

package com.wombat.mama.examples;

import com.wombat.mama.Mama;
import com.wombat.mama.MamaReservedFields;
import com.wombat.mama.MamaBridge;
import com.wombat.mama.MamaDQPublisher;
import com.wombat.mama.MamaDQPublisherManager;
import com.wombat.mama.MamaDQPublisherManagerCallback;
import com.wombat.mama.MamaMsg;
import com.wombat.mama.MamaMsgStatus;
import com.wombat.mama.MamaMsgType;
import com.wombat.mama.MamaQuality;
import com.wombat.mama.MamaQueue;
import com.wombat.mama.MamaQueueEventCallback;
import com.wombat.mama.MamaSource;
import com.wombat.mama.MamaSubscription;
import com.wombat.mama.MamaSubscriptionCallback;
import com.wombat.mama.MamaTimer;
import com.wombat.mama.MamaTimerCallback;
import com.wombat.mama.MamaTransport;
import com.wombat.mama.MamaTransportListener;
import com.wombat.mama.MamaSubscriptionType;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.Logger;
import java.util.logging.Level;
import java.io.*;

public class MamaProxy
{
	private String pubmw = "wmw";
	private String submw = "wmw";

	private String pubTportName = "pub";
	private String subTportName = "sub";

	private MamaTransport pubTransport = null;
	private MamaTransport subTransport = null;

	private MamaBridge publisherBridge = null;
	private MamaBridge subscriberBridge = null;

	private MamaStartRunnable subRunnable = null;
	private MamaStartRunnable pubRunnable = null;

	private String subscriberNameSpace = "WOMBAT";
	private String publisherNameSpace = "MAMA_PROXY";

	private MamaSource subscriberSource = null;
	private MamaSource publisherSource = null;

	private boolean sync = false;

	private Map symbol2subscription = new HashMap(32);

	private MamaDQPublisherManager publisherManager = null;

	private MamaDQPublisherManagerCallback pubManagerCallback = null;

    private static Level      myLogLevel = null;
    private static String     filename            = null;
    
    
	public MamaProxy()
	{
	}

	private void initializeMama()
	{
		subscriberBridge = Mama.loadBridge(submw);
		publisherBridge = Mama.loadBridge(pubmw);
		Mama.open();

		subTransport = new MamaTransport();
		subTransport.create(subTportName, subscriberBridge);
		subTransport.addTransportListener(new TransportCallback());

		pubTransport = new MamaTransport();
		pubTransport.create(pubTportName, publisherBridge);
		pubTransport.addTransportListener(new TransportCallback());

		subscriberSource = new MamaSource();
		subscriberSource.setTransport (subTransport);
		subscriberSource.setSymbolNamespace (subscriberNameSpace);
	}

	private void createPublisher()
	{
		MamaQueue defaultPubQueue = Mama.getDefaultQueue(publisherBridge);

		pubManagerCallback = new SubscriptionHandler(this);
		publisherManager = new MamaDQPublisherManager();

		publisherManager.create(pubTransport, defaultPubQueue,
			pubManagerCallback, publisherNameSpace, "_MD");

		if (sync)
		{
			MamaTimer syncTimer = new MamaTimer();
			syncTimer.create(defaultPubQueue, new SyncCallback(publisherManager), 15, null);
		}
	}

	private void subscribeToSymbols()
	{
		MamaQueue subQueue = Mama.getDefaultQueue(subscriberBridge);

		for (Iterator iter = symbol2subscription.keySet().iterator(); iter.hasNext(); )
		{
			String symbol = (String) iter.next();
			MamaSubscription subscription = new MamaSubscription();
			CacheCallback callback = new CacheCallback(this,subQueue);

			subscription.createSubscription(callback, subQueue, subscriberSource,
				symbol, callback);

			symbol2subscription.put(symbol, subscription);
		}
	}

	private void start()
	{
		
		subRunnable = new MamaStartRunnable(subscriberBridge);
        new Thread(subRunnable).start();
        System.out.println("Starting  subscriber bridge");
          
        if (!submw.equals(pubmw))
        {
            System.out.println("Starting  publisher bridge");
            pubRunnable = new MamaStartRunnable(publisherBridge);
            
            new Thread(pubRunnable).start();
        }
	}

	private void shutdownMama()
	{
		Mama.stop(subscriberBridge);

		do
		{
			try
			{
				Thread.sleep(1000);
			}
			catch (InterruptedException e)
			{
				Thread.interrupted();
			}
		}
		while(!subRunnable.hasCompleted());

		if (!submw.equals(pubmw))
        	{
			Mama.stop(publisherBridge);
			do
			{
				try
				{
					Thread.sleep(1000);
				}
				catch (InterruptedException e)
				{
					Thread.interrupted();
				}
			}
			while(!pubRunnable.hasCompleted());
		}

		// iterate through the subscriptions, destroy
		for (Iterator iter = symbol2subscription.values().iterator(); iter.hasNext(); )
		{
			MamaSubscription subs = (MamaSubscription) iter.next();
			subs.destroy();
		}

		symbol2subscription.clear();

		subTransport.destroy();
		pubTransport.destroy();

		Mama.close();

	}

	private void removeSubscription(MamaSubscription subscription)
	{
		subscription.deactivate(); 
		subscription.destroy();
		symbol2subscription.remove(subscription.getSymbol());
	}

	private MamaSubscription getSubscription(String symbol)
	{
		return (MamaSubscription) symbol2subscription.get(symbol);
	}



	private void parseCommandLine(final String[] args) throws IOException
	{
		for (int i = 0; i < args.length;)
		{
			if (args[i].equals("-SS"))
			{
				subscriberNameSpace = args[i + 1];
				i += 2;
			}
            else if (args[i].equals("-SP"))
			{
				publisherNameSpace = args[i + 1];
				i += 2;
			}
			else if (args[i].equals("-s"))
			{
				symbol2subscription.put(args[i + 1], null);
				i += 2;
			}
            else if (args[i].equals ("-f"))
            {
                filename = args[i + 1];
                i += 2;
            }
			else if (args[i].equals("-tports"))
			{
				subTportName = args[i + 1];
				i += 2;
			}
			else if (args[i].equals("-tportp"))
			{
				pubTportName = args[i + 1];
				i += 2;
			}
			else if (args[i].equals("-v"))
			{
				myLogLevel = myLogLevel == null
						? Level.FINE : myLogLevel == Level.FINE
						? Level.FINER : Level.FINEST;

				Mama.enableLogging(myLogLevel);
				i++;
			}
			else if (args[i].equals("-ms"))
			{
				submw = args[i + 1];
				i += 2;
			}
            else if (args[i].equals("-mp"))
			{
				pubmw = args[i + 1];
				i += 2;
			}
			else if (args[i].equals("-sync"))
			{
				sync = true;
				i++;
			}
            else
            {
                System.out.println("Unknown Arg: " + args[i]);
                System.exit (1);  
            }
		}
        
        
        if (filename != null)
        {
            InputStream input;
            input = new FileInputStream (filename);
             final BufferedReader reader =
            new BufferedReader (new InputStreamReader (input));

            String symbol;
            while (null != (symbol = reader.readLine()))
            {
                if (!symbol.equals(""))
                {
                    symbol2subscription.put(symbol, null);
                }
            }
        }
	}

	private class SubscriptionHandler extends
			MamaDQPublisherManagerCallback
	{
		private final MamaProxy proxy;

		public SubscriptionHandler(MamaProxy proxy)
		{
			this.proxy = proxy;
		}

		public void onCreate(MamaDQPublisherManager publisherManager)
		{
			System.out.println("Created publisher subscription");
		}

		public void onNewRequest(MamaDQPublisherManager publisherManager,
			String symbol, short subType, short msgType, MamaMsg msg)
		{
			MamaSubscription subscription = proxy.getSubscription(symbol);
			if (subscription != null)
			{
				CacheCallback cb = (CacheCallback) subscription.getClosure();
				MamaDQPublisher publisher = publisherManager.createPublisher(symbol, (Object)cb);
	
				System.out.println("Received New Request: " + symbol);
	
	
				cb.setPublisher(publisher);
				
	            switch (msgType)
	            {
	                case 0:
	                case 1:
	                    MamaMsg request = msg.detach();
	                    cb.sendRecap(request);
	                    break;
	                
	                default:
	                    cb.sendRecap();
	                    break;
	            }
			}
			else if (subType==MamaSubscriptionType.SYMBOL_LIST_NORMAL.getValue() || subType==MamaSubscriptionType.SYMBOL_LIST.getValue())
			{
				MamaDQPublisher myPub = publisherManager.createPublisher(symbol, null);
				int numSymbols = 0;
				String symbols = "";
				MamaMsg aMsg = new MamaMsg();
			
				for (Iterator iter = symbol2subscription.keySet().iterator(); iter.hasNext(); )
				{
					String aSymbol = (String) iter.next();
					if (numSymbols > 0)
						symbols+=",";
					numSymbols++;
					symbols+=aSymbol;
					if (numSymbols>49)
					{
						aMsg.addString(null, MamaReservedFields.SymbolList.getId(), symbols);
						aMsg.addU8 (null,MamaReservedFields.MsgType.getId(), MamaMsgType.TYPE_INITIAL);
						myPub.sendReply(msg, aMsg);
						aMsg.clear();
						numSymbols=0;
						symbols="";
					}
				}

				if (numSymbols>0)
				{
					aMsg.addString(null, MamaReservedFields.SymbolList.getId(), symbols);
					aMsg.addU8 (null,MamaReservedFields.MsgType.getId(), MamaMsgType.TYPE_INITIAL);
					myPub.sendReply(msg, aMsg);
					aMsg.clear();
				}
				aMsg.destroy();
			}
			else
				System.out.println("Received Request for unknown symbol: " + symbol);
		}

		public void onRequest(MamaDQPublisherManager publisherManager,
			MamaDQPublisherManager.MamaPublishTopic mamaPublishTopic, short subType,
				short msgType, MamaMsg msg)
		{
			if (subType==MamaSubscriptionType.SYMBOL_LIST.getValue() || subType==MamaSubscriptionType.SYMBOL_LIST_NORMAL.getValue())
			{
				int numSymbols = 0;
				String symbols = "";
				MamaMsg aMsg = new MamaMsg();
			
				for (Iterator iter = symbol2subscription.keySet().iterator(); iter.hasNext(); )
				{
					String aSymbol = (String) iter.next();
					if (numSymbols > 0)
						symbols+=",";
					numSymbols++;
					symbols+=aSymbol;
					if (numSymbols>49)
					{
						aMsg.addString(null, MamaReservedFields.SymbolList.getId(), symbols);
						aMsg.addU8 (null,MamaReservedFields.MsgType.getId(), MamaMsgType.TYPE_INITIAL);
						mamaPublishTopic.getPublisher().sendReply(msg, aMsg);
						aMsg.clear();
						numSymbols=0;
						symbols="";
					}
				}

				if (numSymbols>0)
				{
					aMsg.addString(null, MamaReservedFields.SymbolList.getId(), symbols);
					aMsg.addU8 (null,MamaReservedFields.MsgType.getId(), MamaMsgType.TYPE_INITIAL);
					mamaPublishTopic.getPublisher().sendReply(msg, aMsg);
					aMsg.clear();
				}
				aMsg.destroy();
			}
			else
			{
				System.out.println("Received request: " + mamaPublishTopic.getSymbol());
				((CacheCallback)mamaPublishTopic.getCache()).sendRecap();
			}
		}

		public void onRefresh(MamaDQPublisherManager publisherManager,
			MamaDQPublisherManager.MamaPublishTopic mamaPublishTopic, short subType,
				short msgType, MamaMsg msg)
		{
			System.out.println("Received Refresh: " + mamaPublishTopic.getSymbol());
		}

		public void onError(MamaDQPublisherManager publisherManager, int status,
			String errorTxt, Object platformInfo)
		{
			System.out.println("Error creating publisher: " + errorTxt);
		}
	}

	private static class SyncCallback implements MamaTimerCallback
	{
		private final MamaDQPublisherManager pubman;

		public SyncCallback(MamaDQPublisherManager publisherManager)
		{
			this.pubman = publisherManager;	
		}

		public void onTimer(MamaTimer timer)
		{
			timer.destroy();
			pubman.sendSyncRequest(50,5,30);
			System.out.println("Sent Sync Request");
		}

        public void onDestroy (MamaTimer timer)
        {
            System.out.println ("Timer destroyed");
        }
	}

	private static class CacheCallback implements MamaSubscriptionCallback,
			MamaQueueEventCallback
	{
		private final MamaProxy proxy;
		private final MamaQueue queue;
		private MamaMsg cachedMsg; 

		private MamaDQPublisher dqPublisher;

		private CacheCallback(MamaProxy proxy, MamaQueue queue)
		{
			this.proxy = proxy;
			this.queue = queue;
		}

		// MamaSubscriptionCallback methods

		public void onCreate(MamaSubscription subscription)
		{
			// empty
		}

		public void onError(MamaSubscription subscription, short wombatStatus,
								  int platformError, String subject, Exception e)
		{
			System.err.println("An error occurred creating subscription " + e.getMessage());
		}

		public void onMsg(MamaSubscription subscription, MamaMsg msg)
		{
			switch (MamaMsgType.typeForMsg(msg))
			{
				case MamaMsgType.TYPE_EXPIRE:
				case MamaMsgType.TYPE_DELETE:
					proxy.removeSubscription(subscription);
					subscription.destroy();
					if (dqPublisher != null)
						dqPublisher.send(msg);
					return;
				case MamaMsgType.TYPE_RECAP:
				case MamaMsgType.TYPE_INITIAL:
					cachedMsg.clear();
				default:
					break;
			}

			int status = MamaMsgStatus.statusForMsg(msg); 

			switch (status)
			{
				case MamaMsgStatus.STATUS_BAD_SYMBOL:
				case MamaMsgStatus.STATUS_TIMEOUT:
				case MamaMsgStatus.STATUS_EXPIRED:
					proxy.removeSubscription(subscription);
					subscription.destroy();
				default:
					break;
			}

			boolean detached = false;
			
			if (cachedMsg == null)
			{
				detached = true;
				cachedMsg = msg.detach();
			}
			else
				cachedMsg.apply(msg);

			if (dqPublisher != null)
			{
				dqPublisher.setStatus(status);
				// not the cached message then?
				if (detached)
					dqPublisher.send(cachedMsg);
				else
					dqPublisher.send(msg);
					
			}
		}

		public void onQuality(MamaSubscription subscription, short quality,
									 short cause, Object platformInfo)
		{
			System.out.println("Quality change: " + MamaQuality.toString(quality));
		}

		public void onRecapRequest(MamaSubscription subscription)
		{
			System.out.println("Recap request for subscription: " +
				subscription.getSubscSource() + "." + subscription.getSymbol());
		}

		public void onGap(MamaSubscription subscription)
		{
			System.out.println("Gap for subscription: " +
				subscription.getSubscSource() + "." + subscription.getSymbol());
		}

        public void onDestroy (MamaSubscription subscription)
        {
            System.out.println ("Subscription destroyed");
        }

		// MamaSubscriptionEventCallback methods

		public void onEvent(MamaQueue queue, Object closure)
		{
			if (dqPublisher != null)
			{
				try {
					cachedMsg.updateU8(null, MamaReservedFields.MsgType.getId(), MamaMsgType.TYPE_RECAP);
				} catch (Exception e)
				{
					cachedMsg.updateI16(null, MamaReservedFields.MsgType.getId(), MamaMsgType.TYPE_RECAP);
				}
					if (closure != null)
				{
					MamaMsg request = (MamaMsg)closure;
					dqPublisher.sendReply(request, cachedMsg);
					request.destroy(); 
				}
				else
					dqPublisher.send(cachedMsg);
			}
		}

		// -----------------------------------------------------------------------

		public void setPublisher(MamaDQPublisher dqPublisher)
		{
			this.dqPublisher = dqPublisher;
		}

		public void sendRecap()
		{
			queue.enqueueEvent(this, null);
		}

		public void sendRecap(MamaMsg msg)
		{
			queue.enqueueEvent(this, msg);
		}
	}

	private static class TransportCallback implements MamaTransportListener
	{
		public void onDisconnect(short cause, final Object platformInfo)
		{
			System.out.println("TRANSPORT DISCONNECTED");
		}

		public void onReconnect(short cause, final Object platformInfo)
		{
			 System.out.println("TRANSPORT RECONNECTED");
		}

		public void onQuality(short cause, Object platformInfo)
		{
			System.out.println("QUALITY - " + cause);			
		}

        public void onConnect(short cause, Object platformInfo)
        {
            System.out.println ("TRANSPORT CONNECTED!");
        }
        
        public void onPublisherDisconnect(short cause, final Object platformInfo)
        {
            System.out.println ("PUBLISHER DISCONNECTED!");
        }

        public void onAccept(short cause, final Object platformInfo)
        {
            System.out.println ("TRANSPORT ACCEPTED!");
        }

        public void onAcceptReconnect(short cause, final Object platformInfo)
        {
            System.out.println ("TRANSPORT RECONNECT ACCEPTED!");
        }

        public void onNamingServiceConnect(short cause, final Object platformInfo)
        {
            System.out.println ("NSD CONNECTED!");
        }

        public void onNamingServiceDisconnect(short cause, final Object platformInfo)
        {
           System.out.println ("NSD DISCONNECTED!");
        }
	}

	private static class MamaStartRunnable implements Runnable
	{
		private final MamaBridge bridge;
		private boolean completed = false;

		public MamaStartRunnable(MamaBridge bridge)
		{
			this.bridge = bridge;
		}

		public void run()
		{
			Mama.start(bridge);
			completed = true;
		}

		public boolean hasCompleted()
		{
			return completed;
		}
	}

	public static void main(String[] args)
	{
		final MamaProxy proxy = new MamaProxy();
        
                try
        {
            proxy.parseCommandLine(args);
            proxy.initializeMama();
            proxy.subscribeToSymbols();
            proxy.createPublisher();
            proxy.start();
        }
        catch (Exception e)
        {
            if (e.getCause() != null)
            {
                e.getCause ().printStackTrace ();
            }

            e.printStackTrace ();
            System.exit (1);
        }
        
		Runtime.getRuntime().addShutdownHook(new Thread()
		{
			public void run()
			{
				proxy.shutdownMama();
			}
		});
	}

}
