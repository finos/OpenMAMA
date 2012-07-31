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
package junittests.quotes;

import java.util.Vector;
import java.util.Iterator;
import java.util.Date;
import java.util.logging.Logger;
import java.util.logging.Level;
import java.io.*;

import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;
import org.junit.runner.Description;

import com.wombat.mamda.*;

import com.wombat.mama.*;

public class MamdaQuoteTests extends TestCase
{ 
    private MamdaSubscription            mSubscription           = null;
    private MamdaQuoteListener           mQuoteListener          = null;
    private MamaBridge                   bridge                  = null;
    private MamaDictionary               mDictionary             = null;
    private QuoteTickerCB                ticker                  = null;      
    private MamaMsg                      mMsg                    = null;        
   
    private class QuoteTickerCB implements  MamdaQuoteHandler,
                                            MamdaErrorListener,
                                            MamdaStaleListener
    {  

        public char   myShortSaleCircuitBreaker;
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

          public void onQuoteRecap (
            final MamdaSubscription   sub,
            final MamdaQuoteListener  listener,
            final MamaMsg             msg,
            final MamdaQuoteRecap     recap)
        {
           
        }

        public void onQuoteUpdate (
            final MamdaSubscription   sub,
            final MamdaQuoteListener  listener,
            final MamaMsg             msg,
            final MamdaQuoteUpdate    event,
            final MamdaQuoteRecap     recap)
        {
            myShortSaleCircuitBreaker = event.getShortSaleCircuitBreaker();
       
        }

        public void onQuoteGap (
            final MamdaSubscription   sub,
            final MamdaQuoteListener  listener,
            final MamaMsg             msg,
            final MamdaQuoteGap       event,
            final MamdaQuoteRecap     recap)
        {
           
        }

        public void onQuoteClosing (
            final MamdaSubscription   sub,
            final MamdaQuoteListener  listener,
            final MamaMsg             msg,
            final MamdaQuoteClosing   event,
            final MamdaQuoteRecap     recap)
        {
      
        }

        public void onStale (
            MamdaSubscription   subscription,
            short               quality)
        {
           
        }

        public void onError (
            MamdaSubscription   subscription,
            short               severity,
            short               errorCode,
            String              errorStr)
        {
          
        }
    };
 
    protected void setUp()
    {   
        try
        {
            bridge = Mama.loadBridge("wmw");
            Mama.open();
            //mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);
            mDictionary = new MamaDictionary();
            mDictionary.create();            
            mDictionary._populateFromFile("dictionary.txt");

            MamdaCommonFields.setDictionary (mDictionary,null);
            MamdaQuoteFields.reset();
            MamdaQuoteFields.setDictionary (mDictionary,null);  
            mSubscription = new MamdaSubscription();
            mQuoteListener = new MamdaQuoteListener();
        
            mSubscription.addMsgListener(mQuoteListener);

            ticker = new QuoteTickerCB();      
            mQuoteListener.addHandler (ticker);   
        }
        catch (Exception e)
        {
            e.printStackTrace ();
            System.exit (1);
        }
    }

    public static Test suite() {      

//         //Run single tests.
//         TestSuite suite= new TestSuite();
//         suite.addTest(
//             new MamdaOrderImbalanceCallBackTests() 
//             {
//                  protected void runTest() { testShortSaleCircuitBreaker(); }
//             }
//            );   
//         
//         return suite; 

//         Run all tests.
        return  new TestSuite(MamdaQuoteTests.class);
    }
  
    public void testShortSaleCircuitBreaker()
    {           
        mMsg = new MamaMsg();   
        addMamaHeaderFields(mMsg,
                            MamaMsgType.TYPE_QUOTE,
                            MamaMsgStatus.STATUS_OK,
                            0);
        mMsg.addChar("wShortSaleCircuitBreaker",5248,'C');        
        ticker.callMamdaOnMsg(mSubscription,mMsg);
        assertEquals('C',ticker.myShortSaleCircuitBreaker);        
    }

    public static void main (String[] args) 
    {  
        junit.textui.TestRunner.run(suite());
    }      

    public void addMamaHeaderFields (                           
                            MamaMsg           msg,
                            short             msgType,
                            short             msgStatus,
                            int               seqNum)
    {  
        msg.addU8  (null, 1, msgType);
        msg.addU8  (null, 2, msgStatus);
        msg.addU32 (null, 10, seqNum);             
        msg.addU64 (null, 20,(short)1); 
    }    
}
