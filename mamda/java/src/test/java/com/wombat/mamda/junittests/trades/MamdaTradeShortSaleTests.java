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
package com.wombat.mamda.junittests.trades;

import java.util.Vector;
import java.util.Iterator;
import java.util.Date;
import java.util.logging.Logger;
import java.util.logging.Level;
import java.io.*;

import com.wombat.mamda.junittests.Main;
import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;
import org.junit.runner.Description;

import com.wombat.mamda.*;

import com.wombat.mama.*;

public class MamdaTradeShortSaleTests extends TestCase
{ 
    private MamdaSubscription            mSubscription           = null;
    private MamdaTradeListener           mTradeListener          = null;
    private MamaBridge                   bridge                  = null;
    private MamaDictionary               mDictionary             = null;
    private TradeTickerCB                ticker                  = null;      
    private MamaMsg                      mMsg                    = null;        
   
    private class TradeTickerCB implements  MamdaTradeHandler,
                                            MamdaErrorListener,
                                            MamdaStaleListener
    { 

        private char      myShortSaleCircuitBreaker;  
        private char      myOrigShortSaleCircuitBreaker;   
        private char      myCorrShortSaleCircuitBreaker;       

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
                    listener.onMsg (sub, msg,(short)ob.typeForMsg(msg));
                }   
            }                
            catch (Exception e)
            {
                e.printStackTrace ();
                System.exit (1);
            }
        }

        public void onTradeRecap (
            final MamdaSubscription   sub,
            final MamdaTradeListener  listener,
            final MamaMsg             msg,
            final MamdaTradeRecap     recap)
        {         
        }

        public void onTradeReport (
            final MamdaSubscription   sub,
            final MamdaTradeListener  listener,
            final MamaMsg             msg,
            final MamdaTradeReport    trade,
            final MamdaTradeRecap     recap)
        {  
          myShortSaleCircuitBreaker = trade.getShortSaleCircuitBreaker();
          System.out.println("myShortSaleCircuitBreaker: " + myShortSaleCircuitBreaker);
        }

        public void onTradeGap (
            final MamdaSubscription   sub,
            final MamdaTradeListener  listener,
            final MamaMsg             msg,
            final MamdaTradeGap       event,
            final MamdaTradeRecap     recap)
        {        
        }

        public void onTradeCancelOrError (
            final MamdaSubscription        sub,
            final MamdaTradeListener       listener,
            final MamaMsg                  msg,
            final MamdaTradeCancelOrError  event,
            final MamdaTradeRecap          recap)
        {
          myOrigShortSaleCircuitBreaker = event.getOrigShortSaleCircuitBreaker();
          System.out.println("myOrigShortSaleCircuitBreaker: " + myOrigShortSaleCircuitBreaker);
        }

        public void onTradeCorrection (
            final MamdaSubscription        sub,
            final MamdaTradeListener       listener,
            final MamaMsg                  msg,
            final MamdaTradeCorrection     event,
            final MamdaTradeRecap          recap)
        {
          myCorrShortSaleCircuitBreaker = event.getCorrShortSaleCircuitBreaker();
          System.out.println("myCorrShortSaleCircuitBreaker: " + myCorrShortSaleCircuitBreaker);
        }

        public void onTradeClosing (
            final MamdaSubscription        sub,
            final MamdaTradeListener       listener,
            final MamaMsg                  msg,
            final MamdaTradeClosing        event,
            final MamdaTradeRecap          recap)
        {           
        }

        public void onError(
                  MamdaSubscription   subscription,
                  short               severity,
                  short               errorCode,
                  String              errorStr)
        {
        }

        public void onStale (MamdaSubscription  subscription,
                  short              quality)
        {
        }
    };
 
    protected void setUp()
    {   
        try
        {
            bridge = Mama.loadBridge(Main.getMiddleware());
            Mama.open();
            //mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);
            mDictionary = new MamaDictionary();
            mDictionary.create();            
            mDictionary._populateFromFile(Main.getDictionaryFile());

            MamdaCommonFields.setDictionary (mDictionary,null);
            MamdaTradeFields.reset();
            MamdaTradeFields.setDictionary (mDictionary,null);  
            mSubscription = new MamdaSubscription();
            mTradeListener = new MamdaTradeListener();
        
            mSubscription.addMsgListener(mTradeListener);

            ticker = new TradeTickerCB();      
            mTradeListener.addHandler (ticker);   
        }
        catch (Exception e)
        {
            e.printStackTrace ();
            System.exit (1);
        }
    }
  
    public void testShortSaleCircuitBreaker()
    {           
        mMsg = new MamaMsg();   
        addMamaHeaderFields(mMsg,
                            MamaMsgType.TYPE_TRADE,
                            MamaMsgStatus.STATUS_OK,
                            0);
        mMsg.addChar("wShortSaleCircuitBreaker",5248,'C');
        ticker.callMamdaOnMsg(mSubscription,mMsg);
        assertEquals('C',ticker.myShortSaleCircuitBreaker);        
    }

    public void testOrigShortSaleCircuitBreaker()
    {           
        mMsg = new MamaMsg();   
        addMamaHeaderFields(mMsg,
                            MamaMsgType.TYPE_CANCEL,
                            MamaMsgStatus.STATUS_OK,
                            0);
        mMsg.addChar("wOrigShortSaleCircuitBreaker",5249,'C');
        ticker.callMamdaOnMsg(mSubscription,mMsg);
        assertEquals('C',ticker.myOrigShortSaleCircuitBreaker);        
    }
    

    public void testCorrShortSaleCircuitBreaker()
    {           
        mMsg = new MamaMsg();   
        addMamaHeaderFields(mMsg,
                            MamaMsgType.TYPE_CORRECTION,
                            MamaMsgStatus.STATUS_OK,
                            0);
        mMsg.addChar("wCorrShortSaleCircuitBreaker",5250,'C');
        ticker.callMamdaOnMsg(mSubscription,mMsg);
        assertEquals('C',ticker.myCorrShortSaleCircuitBreaker);        
    }

    public void addMamaHeaderFields (                           
                            MamaMsg           msg,
                            short             msgType,
                            short             msgStatus,
                            int               seqNum)
    {  
        msg.addU8  (null, 1, msgType);
        msg.addU8  (null, 2, msgStatus);
        msg.addU32(null, 10, seqNum);             
        msg.addU64 (null, 20,(short)1); 
    }    
}
