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

import com.wombat.mamda.locks.*;
import com.wombat.mamda.orderbook.*;
import com.wombat.mamda.*;

import com.wombat.mama.*;

public class MamdaBookPublisher extends MamaDQPublisherManagerCallback implements MamaTimerCallback
{
    private static Logger mLogger =   
    Logger.getLogger( "com.wombat.mamda.examples" );

    private static Level                   mLogLevel            = Level.INFO;
    private static MamaSource              mMamaSource          = null;
    private MamaDQPublisherManager         mPublisherManager    = null;
    private MamaTransport                  mPubTransport        = null;
    private static MamaBridge              mBridge              = null;
    private MamdaOrderBook                 mBook                = null;
    private MamaDateTime                   mBookTime            = null;
    public  MamaMsg                        mPublishMsg          = null;
    private MamaDQPublisher                mPublisher           = null; 
    private MamaTimer                      mTimer               = null;
    private boolean                        mProcessEntries      = false;
    private final MamdaLock                mBookLock            = new MamdaLock(); 
    private MamaQueueGroup                 mQueueGroup          = null;
    private MamaDQPublisherManagerCallback pubManagerCallback   = null;
    private boolean                        mPublishing          = false;
    private ArrayList                      mSymbolList          = new ArrayList();
 
    public MamdaBookPublisher(){};      
        
    /*  ******************************************************************* */
    /*  Functionality required for artificially created book orders used to */
    /*  update the book object and generate deltas for publishing           */
    /* ******************************************************************** */
    public char bidSide = MamdaOrderBookPriceLevel.SIDE_BID;
    public char askSide = MamdaOrderBookPriceLevel.SIDE_ASK;

    public static final char PLADD     = MamdaOrderBookPriceLevel.ACTION_ADD;
    public static final char PLUPDATE  = MamdaOrderBookPriceLevel.ACTION_UPDATE;
    public static final char PLDELETE  = MamdaOrderBookPriceLevel.ACTION_DELETE;
    public static final char PLUNKNOWN = MamdaOrderBookPriceLevel.ACTION_UNKNOWN;

    public static final char ENTADD    = MamdaOrderBookEntry.ACTION_ADD;
    public static final char ENTUPDATE = MamdaOrderBookEntry.ACTION_UPDATE;
    public static final char ENTDELETE = MamdaOrderBookEntry.ACTION_DELETE;  

    //  The class "order" represents all pl and entry info of a typical book order  
    class order
    {   
        char        mSide;
        char        mPlAction;
        double      mPrice;
        double      mVolume;
        String      mEntId;
        char        mEntAction;
        long        mEntSize;
        double      mSizeChange;
        double      mNumEntries;
        
        public order( 
            char    Side,    char   PlAction,   double Price,
            double  Volume,  String EntId,      char   EntAction,
            long    EntSize, double SizeChange, double NumEntries)
        {
            mSide       = Side ;
            mPlAction   = PlAction;
            mPrice      = Price;
            mVolume     = Volume;
            mEntId      = EntId;
            mEntAction  = EntAction;
            mEntSize    = EntSize;
            mSizeChange = SizeChange;
            mNumEntries = NumEntries;
        }      
    }

    // Create a number of orderMamdaBookPublishers
    order order0 = new order(bidSide, PLADD,    100, 1000, "bid1", ENTADD,    1000, 1000, 1);               
    order order1 = new order(bidSide, PLADD,    101, 2000, "bid1", ENTADD,    2000, 2000, 1);
    order order2 = new order(bidSide, PLUPDATE, 100, 2000, "bid2", ENTADD,    1000, 1000, 2);                
    order order3 = new order(askSide, PLADD,    110, 2000, "ask1", ENTADD,    2000, 2000, 1);    
    order order4 = new order(askSide, PLADD,    109, 3000, "ask1", ENTADD,    3000, 3000, 1);
    order order5 = new order(bidSide, PLUPDATE, 101, 4000, "bid2", ENTADD,    3000, 3000, 2);
    order order6 = new order(bidSide, PLUPDATE, 101, 2000, "bid1", ENTDELETE, 0,   -2000, 1);    
    order order7 = new order(askSide, PLADD,    104, 4000, "ask1", ENTADD,    4000, 4000, 1);
    order order8 = new order(bidSide, PLUPDATE, 100, 1500, "bid3", ENTADD,    500,  500,  3);
    order order9 = new order(askSide, PLDELETE, 110, 0,    "ask1", ENTDELETE, 0,   -2000, 0); 

    //Initialise an array of these orders
    order[] orderArray = {order0, order1, order2, order3, order4, order5, order6,
                          order7, order8, order9};
    int mOrderCount = 0;

    /* ******************************************************************************** */
    /* Book publishing functionality. This section contains necessary functionality to  */
    /* edit a book and publish the deltaMsg() generated                                 */

    public void onTimer (MamaTimer timer)
    {   
        if(!mPublishing)return;
        //  On every timer update we take the next order from the orderArray and processEntries

        // clear book when at end of orderArray
        // To avoid editing the book while publishing using multiple threads, we need to have a 
        // lock during the book editing functionality and also when sending initial/recap data.
        // We use the MamdaLock class for this functionality and have two functions called
        // acquireLock() and releaseLock() that implement this functionality. 
        mBookLock.acquireWriteLock();   
        
        boolean publish = false;    

        if (10 == mOrderCount)
        {             
            mBook.clear();
            mOrderCount = 0;
            mPublishMsg.updateU8 (null, 1, MamaMsgType.TYPE_BOOK_CLEAR);
            publish = true;
        }
        else
        {             
            // process multiple orders, alike a complex update
            processOrder();
            processOrder();    

            // get changes to the book and publish  
            publish = mBook.populateDelta(mPublishMsg);          
        }
        if (publish) publishMessage(null);   
        mBookLock.releaseWriteLock();
    }

    /* ******************************************************************************** */
    /*                                                                                  */
    /* processOrder() processes the next order in the orderArray,then loops back to 1st */
    /* when all order are processed, clearing the book at this point. This generates    */
    /* changes to the book for publishing. Any real world application would replace     */ 
    /* this functionality with it's own appropriate book handling functionality         */
    /*                                                                                  */
    /* ******************************************************************************** */

    public void processOrder () 
    { 
        MamdaOrderBookPriceLevel level     = null;
        MamdaOrderBookEntry      entry     = null;
        order                    thisOrder = orderArray[mOrderCount];
        mBookTime.setToNow();   

        if (mProcessEntries)
        {                     
            switch (thisOrder.mEntAction)
            {               
                case ENTDELETE:
                {           
                    level = mBook.getLevelAtPrice (thisOrder.mPrice, thisOrder.mSide);
                    
                    if(level != null)
                        entry = level.findOrCreateEntry (thisOrder.mEntId);
                    if (entry != null)      
                        mBook.deleteEntry (entry, mBookTime, null);    
                    break;
                }
                case ENTADD:
                {                  
                    mBook.addEntry (thisOrder.mEntId, thisOrder.mEntSize,
                                    thisOrder.mPrice, thisOrder.mSide,
                                    mBookTime, null,null);    
                    break;
                }
                case ENTUPDATE:
                {   
                    entry = level.findOrCreateEntry (thisOrder.mEntId);
                    mBook.updateEntry (entry, thisOrder.mEntSize,
                                        mBookTime, null);
                    break;
                }
            }
        }
        else
        {                       
            level = mBook.getLevelAtPrice(thisOrder.mPrice, thisOrder.mSide);

            if (level != null)
            {
                level.setSizeChange (thisOrder.mSizeChange);
                level.setPrice      (thisOrder.mPrice);
                level.setSize       (thisOrder.mVolume);
                level.setNumEntries (thisOrder.mNumEntries);    
                level.setTime       (mBookTime);
                level.setAction     (thisOrder.mPlAction);
            }
            else
            {
                level = new MamdaOrderBookPriceLevel();
                level.setSide       (thisOrder.mSide);
                level.setSizeChange (thisOrder.mSizeChange);
                level.setPrice      (thisOrder.mPrice);
                level.setSize       (thisOrder.mVolume);
                level.setNumEntries (thisOrder.mNumEntries);    
                level.setTime       (mBookTime);
                level.setAction     (thisOrder.mPlAction);    
            }
            
            switch (thisOrder.mPlAction)
            {
                case PLDELETE:
                    mBook.deleteLevel(level);
                    break;
                case PLADD:
                    mBook.addLevel(level);
                    break;
                case PLUPDATE:
                    mBook.updateLevel(level);
                    break;
            }
        }
        mOrderCount++;
    }     

    public MamdaOrderBook getBook()
    {   
        return mBook;
    }

    public MamaMsg getPublishMsg()
    {
        mPublishMsg.clear();
        return mPublishMsg;
    }

    public void publishMessage (MamaMsg  request)
    {
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

            //clear publish message for later use
            mPublishMsg.clear();
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

    public void createMsg()
    {
        mPublishMsg = new MamaMsg();
    }

    public void createBook (final String symbol, final String partId) 
    {        
        mBook = new MamdaOrderBook();

        // This turns on the generation of deltas at the order book
        mBook.generateDeltaMsgs (true);
        if (symbol != null) mBook.setSymbol(symbol); 
        if (partId != null) mBook.setPartId(partId); 
        mBookTime = new MamaDateTime();
        mBookTime.setToNow();
        mBook.setBookTime (mBookTime);
        mSymbolList.add   (symbol);  
    }

    public void createPublisherTransport(MamaBridge bridge)
    {
        mPubTransport = new MamaTransport();
        mPubTransport.create ("pub", bridge);
        mPubTransport.addTransportListener(new TransportCallback ());
    }  

    public void createPublisherManager (final String pubSource, MamaBridge bridge)
    {
        mPublisherManager = new MamaDQPublisherManager();                 
        //create pub
        mPublisherManager.create (mPubTransport, mQueueGroup.getNextQueue(),
                                  this, pubSource);
    }

    public void createTimer (final String symbol,MamaBridge bridge)
    {
        //create timer, on next queue, for editing book and publishing changes every 1s
        mTimer = new MamaTimer();
        mTimer.create (mQueueGroup.getNextQueue(), this, 1);
    }

    public void setProcessEntries (boolean process)
    {
        mProcessEntries = process;
    }

    public void setQueueGroup (MamaQueueGroup queues)
    {
        mQueueGroup = queues;
    }

    public boolean publishingSymbol (String symbol) 
    {   
        for (Iterator iterator = mSymbolList.iterator ();
                 iterator.hasNext ();
                )
        {
            final String msymbol = (String) iterator.next();                
            if  (msymbol.equals(symbol))
            {
                return true;
            }     
        }
        return false;
    }
    
    public MamaQueueGroup getQueueGroup()
    {
        return mQueueGroup;
    }

    public void onNewRequest (
        MamaDQPublisherManager  publisherManager,
        final String            symbol,
        short                   subType,
        short                   msgType,
        MamaMsg                 msg)
    {     
         MamaMsg  request = null;
        
        if (publishingSymbol(symbol))  
        {
            mPublisher  = mPublisherManager.createPublisher (symbol, this);
            mPublishing = true;

            mLogger.fine ("Received New request: " + symbol);   

            boolean publish = false;
            switch (msgType)
            {
                case 0:
                case 1:
                    mBookLock.acquireWriteLock();   
                    getBook().populateRecap(getPublishMsg());
                    publishMessage(msg);
                    mBookLock.releaseWriteLock();          
                    break;           
                default:
                    mBookLock.acquireWriteLock();
                    getBook().populateRecap(getPublishMsg());
                    publishMessage(msg);
                    mBookLock.releaseWriteLock();          
                    break;
            }
        }
        else
        {
            mLogger.fine ("Received request for unknown symbol: " + symbol);  
        }       
    }

    public void onRequest (
        MamaDQPublisherManager                  publisherManager,
        MamaDQPublisherManager.MamaPublishTopic mamaPublishTopic,
        short                                   subType,
        short                                   msgType,
        MamaMsg                                 msg)
    {               
            MamaMsg  request = null;
            String   symbol  = mamaPublishTopic.getSymbol();
            boolean  publish = false;

            mLogger.fine ("Received request:" + symbol);  
            
            switch (msgType)
            {      
                case 0:
                case 1:                
                    // publish current delta list first 
                    mBookLock.acquireWriteLock();
                    publish = getBook().populateDelta(getPublishMsg());
                    if (publish) publishMessage(null);
                    publish = false;
                    getBook().populateRecap(getPublishMsg());
                    publishMessage(msg);
                    mBookLock.releaseWriteLock();          
                    break; 
            }    
    }

    public void onRefresh (
        MamaDQPublisherManager                  publisherManager,
        MamaDQPublisherManager.MamaPublishTopic mamaPublishTopic,
        short                                   subType,
        short                                   msgType,
        MamaMsg                                 msg)
    {
    }

    public void onError (
        MamaDQPublisherManager  publisher,
        final short             status,
        final String            errortxt,
        MamaMsg                 msg)
    {   
        if (msg != null) 
        {
            mLogger.fine ("Unhandled Msg:"+ status +msg.toString()+
                          errortxt + "\n");       
        }
        else
        {
            mLogger.fine ("Unhandled Msg: " + status
                          + errortxt + "\n");         
        }
    }      

    public void onCreate (MamaDQPublisherManager  publisher)
    {           
        mLogger.fine("Created publisher subscription.\n");
    }   

    public void onDestroy (MamaTimer timer)
    {
    }

    private static MamaDictionary buildDataDictionary (MamaTransport transport, 
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

            mMamaSource.setTransport (transport);
            mMamaSource.setSymbolNamespace (dictSource);
            MamaDictionary   dictionary   = subscription.
            createDictionarySubscription (
                dictionaryCallback,
                Mama.getDefaultQueue (mBridge),
                mMamaSource);

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
    
    private static class TransportCallback implements MamaTransportListener
    {
        public void onConnect (short cause, final Object platformInfo)
        {
            mLogger.fine ("TRANSPORT CONNECTED");
        }

        public void onDisconnect (short cause, final Object platformInfo)
        {
            mLogger.fine ("TRANSPORT DISCONNECTED");
        }

        public void onReconnect (short cause, final Object platformInfo)
        {
            mLogger.fine ("TRANSPORT RECONNECTED");
        }

        public void onAccept (short cause, final Object platformInfo)
        {
            mLogger.fine ("TRANSPORT ACCEPTED");
        }

        public void onAcceptReconnect (short cause, final Object platformInfo)
        {
            mLogger.fine ("TRANSPORT RECONNECT ACCEPTED");
        }

        public void onPublisherDisconnect (short cause, final Object platformInfo)
        {
            mLogger.fine ("PUBLISHER DISCONNECTED");
        }

        public void onNamingServiceConnect (short cause, final Object platformInfo)
        {
            mLogger.fine ("NSD CONNECTED");
        }

        public void onNamingServiceDisconnect (short cause, final Object platformInfo)
        {
            mLogger.fine ("NSD DISCONNECTED");
        }

        public void onQuality (short cause, Object platformInfo)
        {
           mLogger.fine ("QUALITY - " + cause);           
        }
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

            mLogLevel  = options.getLogLevel();   

            if (mLogLevel != null)
            {
                mLogger.setLevel (mLogLevel);
                Mama.enableLogging (mLogLevel);
            }

            // Initialise the MAMA API
            mBridge = options.getBridge();
            Mama.open ();

            // Get the Data dictionary.....
            MamaTransport transport = new MamaTransport();
            transport.create(options.getTransport(), mBridge);
            mMamaSource = new MamaSource();

            mDictTransportName = options.getDictTransport();            

            if (mDictTransportName != null)
            {
                mDictTransport = new MamaTransport ();
                mDictTransport.create (mDictTransportName, mBridge);
            }
            else 
            {
                mDictTransport = transport;
            }    

            dictionary = buildDataDictionary (mDictTransport,options.getDictSource());   
            if (dictionary != null)
            {              
                MamdaCommonFields.setDictionary    (dictionary,null); 
                MamdaOrderBookFields.setDictionary (dictionary, null);             
            }   

            for (Iterator iterator = options.getSymbolList().iterator();
                            iterator.hasNext();)   
            {
                final String symbol       = (String)iterator.next();   
                String       partId       = options.getPartId();      
                String       pubSource    = options.getPubSource();
    
                MamaQueueGroup   queues = new MamaQueueGroup (options.getNumThreads(),mBridge);
                mBookPublisher.setQueueGroup (queues);          

                mBookPublisher.setProcessEntries (options.getPrintEntries());

                mBookPublisher.createPublisherTransport (mBridge);

                mBookPublisher.createPublisherManager (pubSource, mBridge);  
                                
                // create publisher and also set up MamaTimer to process order and publish changes
                mBookPublisher.createTimer (symbol, mBridge);    

                // create a mamaMsg      
                mBookPublisher.createMsg();
                
                // set up new Book and enable order book publishing
                mBookPublisher.createBook (symbol, partId);    
            }
    
            Mama.start (mBridge);
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


  
    




   
