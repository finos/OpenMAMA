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
package junittests.trades;

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

public class MamdaTradeIdTests extends TestCase
{ 
    private MamdaSubscription            mSubscription           = null;
    private MamdaTradeListener           mTradeListener          = null;
    private MamaBridge                   bridge                  = null;
    private MamaDictionary               mDictionary             = null;
    private TradeTickerCB                ticker                  = null;      
    private MamaMsg                      mMsg                    = null;        
    private String                       myTradeId               = null;
    private String                       myOrigTradeId           = null;
    private String                       myCorrTradeId           = null;

    private class TradeTickerCB implements  MamdaTradeHandler,
                                            MamdaErrorListener,
                                            MamdaStaleListener
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
            myTradeId = listener.getTradeId();      
            System.out.print("msg: " + msg.toString() + "\n");
            System.out.print("myTradeId: " + myTradeId + "\n");
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
            myOrigTradeId = event.getOrigTradeId();
            System.out.print("msg: " + msg.toString());
            System.out.print("myOrigTradeId: " + myOrigTradeId + "\n");
        }

        public void onTradeCorrection (
            final MamdaSubscription        sub,
            final MamdaTradeListener       listener,
            final MamaMsg                  msg,
            final MamdaTradeCorrection     event,
            final MamdaTradeRecap          recap)
        {
            myCorrTradeId = event.getCorrTradeId();    
            System.out.print("msg: " + msg.toString());
            System.out.print("myCorrTradeId: " + myCorrTradeId + "\n");
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
            bridge = Mama.loadBridge("wmw");
            Mama.open();
            //mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);
            mDictionary = new MamaDictionary();
            mDictionary.create();            
            mDictionary._populateFromFile("dictionary");

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

    public static Test suite() {      

//         //Run single tests.
//         TestSuite suite= new TestSuite();
//         suite.addTest(
//             new MamdaOrderImbalanceCallBackTests() 
//             {
//                  protected void runTest() { Mamda1424(); }
//             }
//            );   
//         
//         return suite; 

//         Run all tests.
        return  new TestSuite(MamdaTradeIdTests.class);
    }
  
    public void testTradeId()
    {           
        mMsg = new MamaMsg();   
        addMamaHeaderFields(mMsg,
                            MamaMsgType.TYPE_TRADE,
                            MamaMsgStatus.STATUS_OK,
                            0);
        mMsg.addString("wTradeId",477,"A");
        ticker.callMamdaOnMsg(mSubscription,mMsg);
        assertEquals("A",myTradeId);        
    }

    public void testOrigTradeId()
    {
        mMsg = new MamaMsg();   
        addMamaHeaderFields(mMsg,
                            MamaMsgType.TYPE_CANCEL,
                            MamaMsgStatus.STATUS_OK,
                            0);
        mMsg.addString("wOrigTradeId",1096,"B");
        ticker.callMamdaOnMsg(mSubscription,mMsg);
        assertEquals("B",myOrigTradeId);
    }

    public void testCorrTradeId()
    {
        mMsg = new MamaMsg();   
        addMamaHeaderFields(mMsg,
                            MamaMsgType.TYPE_CORRECTION,
                            MamaMsgStatus.STATUS_OK,
                            0);
        mMsg.addString("wCorrTradeId",4060,"C");
        ticker.callMamdaOnMsg(mSubscription,mMsg);
        assertEquals("C",myCorrTradeId);
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
