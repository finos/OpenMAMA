/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2012 NYSE Technologies, Inc.
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

package com.wombat.mamda.examples;

import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.Iterator;
import java.util.ArrayList;
import java.math.BigDecimal;
import java.util.Vector;
import java.util.Random;

import com.wombat.mamda.orderbook.*;
import com.wombat.mamda.*;
import com.wombat.mama.*;

public class MamdaListenerBookPublisher
{
    private static Logger mLogger =   
        Logger.getLogger( "com.wombat.mamda.examples" );

    public static class BookTicker implements MamdaOrderBookHandler
    {
        private Vector msgListeners = new Vector();
        private MamaMsgType ob      = new MamaMsgType();


        public void callMamdaOnMsg(MamdaSubscription sub, MamaMsg msg)
        {   
            try
            {   
                msgListeners = sub.getMsgListeners();
                int size = msgListeners.size();
                for (int i = 0; i < size; i++)           
                {      
                     MamdaMsgListener listener = (MamdaMsgListener)msgListeners.get(i);
                     listener.onMsg (sub, msg, (short)ob.typeForMsg(msg));
                }   
            }                
            catch (Exception e)
            {
                e.printStackTrace ();
                System.exit (1);
            }
        }

         public void onBookDelta (MamdaSubscription         sub,
                                  MamdaOrderBookListener    listener,
                                  MamaMsg                   msg,
                                  MamdaOrderBookSimpleDelta delta,
                                  MamdaOrderBook            book)
        {    
            System.out.println ("\n Book Delta \n");
            book.dump();  
        }

        public void onBookComplexDelta (MamdaSubscription          subscription,
                                        MamdaOrderBookListener     listener,
                                        MamaMsg                    msg,
                                        MamdaOrderBookComplexDelta delta,
                                        MamdaOrderBook             book)
        {  
            System.out.println ("\n Book ComplexDelta \n");
            book.dump();  
        }

        public void onBookClear (MamdaSubscription      subscription,
                                 MamdaOrderBookListener listener,
                                 MamaMsg                msg,
                                 MamdaOrderBookClear    event,
                                 MamdaOrderBook         book)
        {
            System.out.println("\n Book Clear \n");
            book.dump();  
        }

        public void onBookRecap (MamdaSubscription          sub,
                                 MamdaOrderBookListener     listener, 
                                 MamaMsg                    msg,
                                 MamdaOrderBookComplexDelta delta,
                                 MamdaOrderBookRecap        event,
                                 MamdaOrderBook             book)
        {   
            System.out.println("\n Book Recap \n");
            book.dump();      
        }

        public void onBookGap (MamdaSubscription      sub,
                               MamdaOrderBookListener listener,
                               MamaMsg                msg,
                               MamdaOrderBookGap      event,
                               MamdaOrderBook         fullBook)
        { 
            System.out.println("\n Book Gap \n");
            fullBook.dump();  
        }
    }

    private static class MamdaBookPublisher   implements MamaTimerCallback
    {
        MamaDQPublisherManager     mPubManager          = null;
        SubscriptionHandler        managerCallback      = null;
        ArrayList                  mSubscriptionList    = new ArrayList();
        ArrayList                  mSymbolList          = new ArrayList();
        MamdaOrderBook             mBook                = null;
        MamaDateTime               mBookTime            = null;
        MamaMsg                    mPublishMsg          = null;
        MamaDQPublisher            mPublisher           = null; 
        MamaTimer                  mTimer               = null;
        boolean                    mProcessEntries      = false;
        
        MamaBridge                 mBridge              = null;
        String                     mMiddleware          = null;
        String                     mPubTport            = null;
        String                     mSubTport            = null;
        MamaTransport              mSubTransport        = null;
        MamaTransport              mPubTransport        = null;
        String                     mPubSourceName       = null;
        String                     mSubSourceName       = null;
        MamaSource                 mPubSource           = null;
        MamaSource                 mSubSource           = null;


        MamdaBookPublisher (){}; 
        
        public void onTimer (MamaTimer timer)
        {
            // need to clear publishMsg after every update
            String symbol;
            for (Iterator iterator = mSymbolList.iterator();
                    iterator.hasNext();
                    )
            {
                symbol = (String)iterator.next();
                mPublishMsg.clear();
                if (mBook.populateDelta(mPublishMsg))
                {
                    publishMessage(null, symbol);
                }       
                else 
                    System.out.println("\n Not publishing from Empty State \n");
            }
        }

        public void subscribeToSymbols (boolean processEntries)
        {
            for (Iterator iterator = mSymbolList.iterator();
                    iterator.hasNext();
                    )
            {   
                String symbol = (String)iterator.next();
                mBook = new MamdaOrderBook();
                MamdaSubscription      aSubscription = new MamdaSubscription();
                MamdaOrderBookListener aBookListener = new MamdaOrderBookListener(mBook);
                aSubscription.addMsgListener    (aBookListener);
                aBookListener.setProcessEntries (processEntries);

                BookTicker aTicker = new BookTicker();
                aBookListener.addHandler (aTicker);

                mBook.generateDeltaMsgs(true);
                
                aSubscription.setType (MamaSubscriptionType.BOOK);

                if(mSubscriptionList.isEmpty())
                    System.out.println("\n mSubscriptionList is empty \n");
    
                mSubscriptionList.add(aSubscription);

                if(!mSubscriptionList.isEmpty())
                    System.out.println("\n mSubscriptionList is not \n");

                aSubscription.create (mSubTransport, 
                                      Mama.getDefaultQueue(mBridge),
                                      "SOURCE"/*mSubSource.getId()*/, 
                                      symbol, 
                                      null);
            }
        }

        public void  createPublisher ()
        {

            mPublishMsg     = new MamaMsg();
            managerCallback = new SubscriptionHandler (this);         
            mPubManager     = new MamaDQPublisherManager();
            
            mPubManager.create (mPubTransport, 
                                Mama.getDefaultQueue(mBridge),
                                managerCallback,
                                mPubSourceName);
        }

        public void  createTimer()
        {
            // create timer for editing book and publishing changes every 5s
            mTimer = new MamaTimer();
            mTimer.create (Mama.getDefaultQueue(mBridge), this, 2.0);
        }

        public void  removeSubscription (MamdaSubscription subscribed)
        {
            mSubscriptionList.remove (subscribed);
        }

        public void  publishMessage (MamaMsg  request, String symbol)
        {           

            if (mPublisher == null)
            {    
                MamdaSubscription mamdaSub =
                    (MamdaSubscription)(mSubscriptionList.iterator()).next();
                
                if (mamdaSub != null)
                {
                    MamaSubscription mSub = mamdaSub.getMamaSubscription();
                    mPublisher = mPubManager.createPublisher(symbol, managerCallback);   
                }
            }        
            try
            {
                if (request != null)
                {
                    mPublisher.sendReply (request, mPublishMsg);
                }
                else
                {
                    mPublisher.send (mPublishMsg);
                }
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
        }

        public MamdaSubscription getMamdaSubscription (String symbol)
        {
            for (Iterator iterator = mSubscriptionList.iterator();
                                        iterator.hasNext();
                                        )
            {
                if  (((MamdaSubscription)iterator.next()).getSymbol() == symbol)
                {
                    return  ((MamdaSubscription)iterator.next());
                }
            }                        
            return null; 
        }

        public void  start ()
        {
            Mama.start(mBridge);
        } 

        public void  initializeMama ()
        {

            if (mMiddleware != null) 
                mMiddleware = "wmw";
            mBridge = Mama.loadBridge (mMiddleware);

            Mama.open ();

            mSubTransport = new MamaTransport();
            mSubTransport.create (mSubTport, mBridge);
            mSubTransport.addTransportListener (new TransportCallback ());

            mPubTransport = new MamaTransport();
            mPubTransport.create (mPubTport, mBridge);
            mPubTransport.addTransportListener (new TransportCallback ());
        }

        public void createSources()
        {       
            mSubSource = new MamaSource ("default", 
                                         mSubTransport, 
                                         mSubSourceName);
                                         
            mSubSource.getTransport().setOutboundThrottle (MamaThrottleInstance.DEFAULT_THROTTLE,       
                                        (long)500) ;                             

        }
        
        public MamaDictionary buildDataDictionary (MamaTransport transport, 
                                                   String        dictSource)                                                     
                                                   throws        InterruptedException
        {
            final boolean gotDict[] = {false};
            MamaDictionaryCallback dictionaryCallback =
            new MamaDictionaryCallback ()
            {
                public void onTimeout ()
                {
                    System.err.println
                    ("Timed out waiting for dictionary");
                    System.exit (1);
                }

                public void onError (final String s)
                {
                    System.err.println ("Error getting dictionary: " + s);
                    System.exit(1);
                }

                public synchronized void onComplete ()
                {
                    gotDict[0] = true;
                    Mama.stop (mBridge);
                    notifyAll ();
                }
            };

            synchronized (dictionaryCallback)
            {
                MamaSubscription subscription = new MamaSubscription ();
                mSubSource.setTransport       (transport);
                mSubSource.setSymbolNamespace (dictSource);

                MamaDictionary   dictionary   = subscription.
                    createDictionarySubscription (
                        dictionaryCallback,
                        Mama.getDefaultQueue (mBridge),
                        mSubSource);

                Mama.start (mBridge);
                if (!gotDict[0]) dictionaryCallback.wait (30000);
                if (!gotDict[0])
                {
                    System.err.println ("Timed out waiting for dictionary.");
                    System.exit (0);
                }
                return dictionary;
            }
        }

        public MamaBridge getBridge()
        {
            return mBridge;
        }
        
        public void create(MamaMsg msg)
        {
            msg = new MamaMsg();
        }

        public MamaTransport getTransport()
        {
            return mSubTransport;
        }

        public void onDestroy (MamaTimer timer)
        {
        }

    };

    private static class TransportCallback implements MamaTransportListener
    {
        public void onDisconnect(short cause, final Object platformInfo)
        {
            mLogger.fine("TRANSPORT DISCONNECTED");
        }

        public void onReconnect(short cause, final Object platformInfo)
        {
            mLogger.fine("TRANSPORT RECONNECTED");
        }

        public void onQuality(short cause, Object platformInfo)
        {
           mLogger.fine("QUALITY - " + cause);
        }

        public void onConnect (short cause, final Object platformInfo)
        {
            mLogger.fine("TRANSPORT CONNECTED");
        }

        public void onAccept (short cause, final Object platformInfo)
        {
            mLogger.fine("TRANSPORT ACCEPTED");
        }

        public void onAcceptReconnect (short cause, final Object platformInfo)
        {
            mLogger.fine("TRANSPORT RECONNECTED");
        }

        public void onPublisherDisconnect (short cause, final Object platformInfo)
        {
            mLogger.fine("PUBLISHER DISCONNECTED");
        }

        public void onNamingServiceConnect (short cause, final Object platformInfo)
        {
            mLogger.fine("NSD CONNECTED");
        }

        public void onNamingServiceDisconnect (short cause, final Object platformInfo)
        {
            mLogger.fine("NSD DISCONNECTED");
        }

    } 


    private static class SubscriptionHandler extends MamaDQPublisherManagerCallback
    {
        SubscriptionHandler(MamdaBookPublisher bookPublisher) 
        {
            mBookPublisher = bookPublisher;
        }

        public void onCreate(MamaDQPublisherManager publisherManager)
        {
             System.out.println ("Created publisher subscription.\n");
             System.out.flush ();
        }

        public void onNewRequest(MamaDQPublisherManager publisherManager,
            String symbol, short subType, short msgType, MamaMsg msg)
        {

            System.out.println("\n IN ON NEWREQUEST \n");

            MamaMsg   request = null;

            MamdaSubscription mamdaSub = (MamdaSubscription) (mBookPublisher.mSubscriptionList).iterator().next();

            System.out.println("\n line 445 \n");
            
            if(mamdaSub == null)
                System.exit(1);

            if (mamdaSub != null)
            {
                MamaSubscription mSub = mamdaSub.getMamaSubscription();
                MamaDQPublisher  mPub = publisherManager.createPublisher (symbol, mBookPublisher.managerCallback);   

                System.out.println("Received New request: \n" + symbol);

                switch (msgType)
                {
                    case 0:
                    case 1:
                        mBookPublisher.publishMessage(request, symbol);
                        break;
                    default:
                        mBookPublisher.publishMessage(null, symbol);
                        break;
                
                }
            }
            else
            System.out.println("Received request for unknown symbol: \n" +  symbol);

            System.out.flush();          
        }

        public void onRequest (MamaDQPublisherManager                  publisherManager,
                               MamaDQPublisherManager.MamaPublishTopic mamaPublishTopic, 
                               short                                   subType, 
                               short                                   msgType, 
                               MamaMsg                                 msg)
        {
            MamaMsg  request = null;
            System.out.println ("Received request: \n" + mamaPublishTopic.getSymbol());
            String symbol = mamaPublishTopic.getSymbol();

            switch (msgType)
            {      
                case 0:
                case 1:
                {
                    mBookPublisher.mPublishMsg.clear();
                    mBookPublisher.mBook.populateRecap(mBookPublisher.mPublishMsg);
                    mBookPublisher.publishMessage(request, symbol);
                    break;
                }
            }
            System.out.flush ();
        }

        public void onRefresh (MamaDQPublisherManager                  publisherManager,
                               MamaDQPublisherManager.MamaPublishTopic mamaPublishTopic, 
                               short                                   subType, 
                               short                                   msgType, 
                               MamaMsg                                 msg)
        {        
            System.out.println("Received Refresh: \n" +  mamaPublishTopic.getSymbol());
            System.out.flush();
        }

        public void onError (MamaDQPublisherManager publisherManager, 
                             int                    status,
                             String                 errorTxt, 
                             MamaMsg                msg)
        {
            if (msg != null) 
                System.out.println ("Unhandled Msg:\n" + 
                    status + msg.toString () + errorTxt);
            else
                System.out.println ("Unhandled Msg: \n" + 
                    status + errorTxt);
            System.out.flush ();
        }

        private MamdaBookPublisher mBookPublisher = null;

    }

    public static void main (final String[] args)   
    {
        try
        {
            MamdaBookPublisher     mBookPublisher     = new MamdaBookPublisher();           
            CommandLineProcessor   options            = new CommandLineProcessor (args);
            MamaTransport          mDictTransport     = null;
            MamaDictionary         dictionary         = null;
            String                 mDictTransportName = null;

            Mama.enableLogging (Level.FINEST);    

            mBookPublisher.mSymbolList    = options.getSymbolList ();
            mBookPublisher.mSubSourceName = "SOURCE";           //options.getOptString("SS");
            mBookPublisher.mPubSourceName = "BOOKPUBLISHER";    //options.getOptString("SP");
            mBookPublisher.mSubTport      = "sub";              //options.getOptString("tports");
            mBookPublisher.mPubTport      = "pub";              //options.getOptString("tportp");
            mBookPublisher.mMiddleware    = "wmw";              //options.getOptString("m");
            boolean  processEntries       = true;               /*cmdLine.getOptBool ('e');   */


            System.out.println("\n processEntries = \n" + processEntries);
        
            mBookPublisher.initializeMama();
            mBookPublisher.createSources();

            mDictTransportName = options.getDictTransport();            

            if (mDictTransportName != null)
            {
                mDictTransport = new MamaTransport ();
                mDictTransport.create (mDictTransportName, mBookPublisher.getBridge());
            }
            else 
            {
                mDictTransport = mBookPublisher.getTransport();
            }  

            dictionary = mBookPublisher.buildDataDictionary(mDictTransport,options.getDictSource());          
            if(dictionary != null)
            {
                MamdaQuoteFields.setDictionary     (dictionary, null);          
                MamdaTradeFields.setDictionary     (dictionary, null);          
                MamdaCommonFields.setDictionary    (dictionary,null); 
                MamdaOrderBookFields.setDictionary (dictionary, null);             
            }   

            mBookPublisher.createPublisher();
            mBookPublisher.subscribeToSymbols(processEntries);          

            //create publisher and also set up MamaTimer to process order and publish changes
            mBookPublisher.createTimer();    

            //set up new Book and enable order book publishing               
            mBookPublisher.start(); 
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
    }  
}
