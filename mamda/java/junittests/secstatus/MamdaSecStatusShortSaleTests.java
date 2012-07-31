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
package junittests.secstatus;

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

public class MamdaSecStatusShortSaleTests  extends TestCase
{ 
    public  MamdaSubscription            mSubscription           = null;
    public  MamdaSecurityStatusListener  mSecStatusListener      = null;
    public  MamaBridge                   bridge                  = null;
    public  MamaDictionary               mDictionary             = null; 
    public  String                       testdescription         = null;
    public  MamaMsg                      mMsg                    = null; 
    private Vector                       msgListeners            = new Vector();
    private MamaMsgType                  ob                      = new MamaMsgType();

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

  
    protected void setUp()
    {   
        try
        {
            bridge = Mama.loadBridge("wmw");
            Mama.open();
          /*mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);*/
            mDictionary = new MamaDictionary();
            mDictionary.create();            
            mDictionary._populateFromFile("dictionary.txt");

            MamdaCommonFields.setDictionary (mDictionary,null);
            MamdaSecurityStatusFields.reset();
            MamdaSecurityStatusFields.setDictionary (mDictionary,null);
            
            mSubscription = new MamdaSubscription();
            mSecStatusListener = new MamdaSecurityStatusListener();
           
            mSubscription.addMsgListener(mSecStatusListener);

            testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t              ****     MamdaSecStatusShortSaleTests failed         ****                             "+
               "\n This test passes in the ShortSaleCircuitBreaker field to the SecurityStatusListener  "+
               "\n and checks that the field  and fieldstates are correctly cached and updated.         "+             
               "\n ********************************************************************************** \n" );   
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
//                  protected void runTest() { testCrossType_onNoOrdImbal_Test(); }
//             }
//            );   
//         
//         return suite; 

//         Run all tests.
        return  new TestSuite(MamdaSecStatusShortSaleTests.class);
    }
  
    public void testNotInEffect()
    {
        mMsg = new MamaMsg();   
        addMamaHeaderFields(mMsg,
                            MamaMsgType.TYPE_SEC_STATUS,
                            MamaMsgStatus.STATUS_OK,
                            (int)1);

        callMamdaOnMsg(mSubscription,mMsg);
                                   
        assertEquals(testdescription,' ' , mSecStatusListener.getShortSaleCircuitBreaker());
        assertEquals(testdescription,MamdaFieldState.NOT_INITIALISED,mSecStatusListener.getShortSaleCircuitBreakerFieldState());
        mMsg = null;
    }

    public void testCached()
    {
        mMsg = new MamaMsg();   
        addMamaHeaderFields(mMsg,
                            MamaMsgType.TYPE_SEC_STATUS,
                            MamaMsgStatus.STATUS_OK,
                            (int)1); 

        mMsg.addChar("wShortSaleCircuitBreaker",5248,'A');     
        callMamdaOnMsg(mSubscription,mMsg);
                                   
        assertEquals(testdescription,'A',mSecStatusListener.getShortSaleCircuitBreaker());
        assertEquals(testdescription,MamdaFieldState.MODIFIED,mSecStatusListener.getShortSaleCircuitBreakerFieldState());
        mMsg = null;

    }

    public void testUpdated()
    {  
        mMsg = new MamaMsg();   
        addMamaHeaderFields(mMsg,
                            MamaMsgType.TYPE_INITIAL,
                            MamaMsgStatus.STATUS_OK,
                            (int)1); 
        mMsg.addChar("wShortSaleCircuitBreaker",5248,'C');     
        callMamdaOnMsg(mSubscription,mMsg);
                                   
        assertEquals(testdescription,'C',mSecStatusListener.getShortSaleCircuitBreaker());
        assertEquals(testdescription,MamdaFieldState.MODIFIED,mSecStatusListener.getShortSaleCircuitBreakerFieldState());

        mMsg = null;
        mMsg = new MamaMsg();   
        addMamaHeaderFields(mMsg,
                            MamaMsgType.TYPE_SEC_STATUS,
                            MamaMsgStatus.STATUS_OK,
                            (int)1); 
        callMamdaOnMsg(mSubscription,mMsg);      
        assertEquals(testdescription,MamdaFieldState.NOT_MODIFIED,mSecStatusListener.getShortSaleCircuitBreakerFieldState());

        mMsg = null;
        mMsg = new MamaMsg();
        addMamaHeaderFields(mMsg,
                            MamaMsgType.TYPE_SEC_STATUS,
                            MamaMsgStatus.STATUS_OK,
                            (int)1); 
        mMsg.addChar("wShortSaleCircuitBreaker",5248,'D');   
        callMamdaOnMsg(mSubscription,mMsg);      
        assertEquals(testdescription,'D',mSecStatusListener.getShortSaleCircuitBreaker());
        assertEquals(testdescription,MamdaFieldState.MODIFIED,mSecStatusListener.getShortSaleCircuitBreakerFieldState());
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

    public static void main (String[] args) 
    {  
        junit.textui.TestRunner.run(suite());
    }
}    
