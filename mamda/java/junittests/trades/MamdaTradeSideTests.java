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

package junittests.Trades;

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

public class MamdaTradeSideTests extends TestCase
{ 
    public  MamdaSubscription            mSubscription           = null;
    public  MamdaTradeListener           mTradeListener          = null;
    public  MamaBridge                   bridge                  = null;
    public  MamaDictionary               mDictionary             = null; 
    public  MamaMsg                      mMsg                    = null; 
    private Vector                       msgListeners            = new Vector();
    private MamaMsgType                  ob                      = new MamaMsgType();
  
    protected void setUp()
    {   
        try
        {
            bridge = Mama.loadBridge("wmw");
            Mama.open();
/*                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);*/
            mDictionary = new MamaDictionary();
            mDictionary.create();            
            mDictionary._populateFromFile("dictionary.txt");

            MamdaCommonFields.setDictionary (mDictionary,null);
            MamdaTradeFields.reset();
            MamdaTradeFields.setDictionary (mDictionary,null);
            
            mSubscription = new MamdaSubscription();
            mTradeListener = new MamdaTradeListener();
           
            mSubscription.addMsgListener(mTradeListener);

            mMsg = new MamaMsg();   
           
        }
        catch (Exception e)
        {
            e.printStackTrace ();
            System.exit (1);
        }
    } 

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

    public static Test suite() {      

//         //Run single tests.
//         TestSuite suite= new TestSuite();
//         suite.addTest(
//             new MamdaTradeSideTests() 
//             {
//                  protected void runTest() { testMamdaTradeSide(); }
//             }
//            );   
//         
//         return suite; 

//         Run all tests.
        return  new TestSuite(MamdaTradeSideTests.class);
    }

    public void testMamdaTradeSide()
    {
        MamdaTradeSide tradeside = new MamdaTradeSide();
        assertEquals(0,tradeside.getState());
        tradeside.setState((short)1);
        assertEquals(1,tradeside.getState());  
        assertEquals("Buy",MamdaTradeSide.toString(tradeside.getState()));
      
     }  

    public void testNoAggressorSideOrTradeSide()
    {
        addMamaHeaderFields(mMsg,
                            MamaMsgType.TYPE_TRADE,
                            MamaMsgStatus.STATUS_OK,
                            (int)1);
        
        callMamdaOnMsg(mSubscription,mMsg);
        assertEquals("",mTradeListener.getSide());
        assertEquals(MamdaFieldState.NOT_INITIALISED,mTradeListener.getSideFieldState());   
    }

    public void testAggressorSide()
    {
         addMamaHeaderFields(mMsg,
                            MamaMsgType.TYPE_TRADE,
                            MamaMsgStatus.STATUS_OK,
                            (int)1);
        mMsg.addChar("wAggressorSide",4486,'1');
        callMamdaOnMsg(mSubscription,mMsg);       
        assertEquals("1",mTradeListener.getSide());
        assertEquals(MamdaFieldState.MODIFIED,mTradeListener.getSideFieldState());   
    }

    public void testTradeSideString()
    {
         addMamaHeaderFields(mMsg,
                            MamaMsgType.TYPE_TRADE,
                            MamaMsgStatus.STATUS_OK,
                            (int)1);
        mMsg.addString("wTradeSide",1161,"1");
        callMamdaOnMsg(mSubscription,mMsg);       
        assertEquals("Buy",mTradeListener.getSide());
        assertEquals(MamdaFieldState.MODIFIED,mTradeListener.getSideFieldState());   
    }

    public void testTradeSideInt()
    {
         addMamaHeaderFields(mMsg,
                            MamaMsgType.TYPE_TRADE,
                            MamaMsgStatus.STATUS_OK,
                            (int)1);
        mMsg.addU32("wTradeSide",1161,1);
        callMamdaOnMsg(mSubscription,mMsg);       
        assertEquals("Buy",mTradeListener.getSide());
        assertEquals(MamdaFieldState.MODIFIED,mTradeListener.getSideFieldState());   
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
        msg.addU8  (null, 1,     msgType);
        msg.addU8  (null, 2,   msgStatus);
        msg.addU32(null, 10, seqNum);             
        msg.addU64 (null, 20,  (short)  1);         
   
    }
}    
