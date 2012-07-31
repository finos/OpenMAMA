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
package junittests.orderImbalances;

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

public class MamdaOrderImbalanceCallBackTests extends TestCase
{ 

    public MamdaSubscription            mSubscription           = null;
    public MamdaOrderImbalanceListener  mOrderImbalanceListener = null;
    public MamaBridge                   bridge                  = null;
    public MamaDictionary               mDictionary             = null;
    public OrderImbalanceTickerCB       ticker                  = null;      
    public MamaMsg                      newMessage              = null;

    private class OrderImbalanceTickerCB implements  MamdaOrderImbalanceHandler,
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

        public void onOrderImbalance ( 
            MamdaSubscription            subscription,
            MamdaOrderImbalanceListener  listener,
            MamaMsg                      msg,
            MamdaOrderImbalanceRecap     imbalance,
            MamdaOrderImbalanceUpdate    update)
        {         
        }

        public void onNoOrderImbalance (
            MamdaSubscription            subscription,
            MamdaOrderImbalanceListener  listener,
            MamaMsg                      msg,
            MamdaOrderImbalanceRecap     imbalance,
            MamdaOrderImbalanceUpdate    update)
        {       
        }

        public void onOrderImbalanceRecap (
            MamdaSubscription            subscription,
            MamdaOrderImbalanceListener  listener,
            MamaMsg                      msg,
            MamdaOrderImbalanceRecap     imbalance)
        {                 
        }

        public void onError(
            MamdaSubscription   subscription,
            short               severity,
            short               errorCode,
            String              errorStr)
        {
        }

        public void onStale (
            MamdaSubscription  subscription,
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
/*                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);*/
            mDictionary = new MamaDictionary();
            mDictionary.create();            
            mDictionary._populateFromFile("dictionary.txt");

            MamdaCommonFields.setDictionary (mDictionary,null);
            MamdaOrderImbalanceFields.reset();
            MamdaOrderImbalanceFields.setDictionary (mDictionary,null);
            
            mSubscription = new MamdaSubscription();
            mOrderImbalanceListener = new MamdaOrderImbalanceListener();
           
            mSubscription.addMsgListener(mOrderImbalanceListener);

            ticker = new OrderImbalanceTickerCB();      
            mOrderImbalanceListener.addHandler (ticker);   

            newMessage = new MamaMsg();        

            addMamaHeaderFields(newMessage);



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
        return  new TestSuite(MamdaOrderImbalanceCallBackTests.class);
    }

    /*Each Test has one Field Set and the SecurityStatus is set to OrderImbNone. 
      these should trigger the noOrderImbalance CallBack. */
    public void testBuyVolume_onNoOrderImbal()
    { 
        newMessage.addI64("wBuyVolume", 239, 1000);
        newMessage.addString("wSecStatusQual",1020,"OrderImbNone");

        ticker.callMamdaOnMsg(mSubscription, newMessage);     
        
        assertEquals(1000,mOrderImbalanceListener.getBuyVolume());
    }

    public void testSellVolume_onNoOrdImbal_Test()
    {   
        newMessage.addI64("wSellVolume", 453, 2000);
        newMessage.addString("wSecStatusQual",1020,"OrderImbNone");

        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals(2000,mOrderImbalanceListener.getSellVolume());
    }

    public void testHighIndication_onNoOrdImbal_Test()
    {  
        MamaPrice temp = new MamaPrice();

        temp.setValue(2000);

        newMessage.addPrice("wHighIndicationPrice",276,temp);
        newMessage.addString("wSecStatusQual",1020,"OrderImbNone");

        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals(2000,mOrderImbalanceListener.getHighIndicationPrice().getValue(),0);
    }

    public void testLowIndication_onNoOrdImbal_Test()
    {      
        MamaPrice temp = new MamaPrice();

        temp.setValue(3000);

        newMessage.addPrice("wLowIndicationPrice",325,temp);
        newMessage.addString("wSecStatusQual",1020,"OrderImbNone");

        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals(3000,mOrderImbalanceListener.getLowIndicationPrice().getValue(),0);
    }

    public void testIndication_onNoOrdImbal_Test()
    {
        MamaPrice temp = new MamaPrice();

        temp.setValue(4000);

        newMessage.addPrice("wIndicationPrice",978,temp);
        newMessage.addString("wSecStatusQual",1020,"OrderImbNone");
        

        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals(4000,mOrderImbalanceListener.getImbalancePrice().getValue(),0);
    }

    public void testMatchVolume_onNoOrdImbal_Test()
    {            
        newMessage.addI64("wMatchVolume",979,4000);
        newMessage.addString("wSecStatusQual",1020,"OrderImbNone");        

        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals(4000,mOrderImbalanceListener.getMatchVolume(),0);
    }

    public void testSecurityStatus_onNoOrdImbal_Test()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrderImbNone");    

        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals("OrderImbNone",mOrderImbalanceListener.getImbalanceState());
    }

    public void testInsideMatchPrice_onNoOrdImbal_Test()
    {  
        MamaPrice temp = new MamaPrice();
        temp.setValue(5000);

        newMessage.addPrice("wInsideMatchPrice",1023,temp); 
        newMessage.addString("wSecStatusQual",1020,"OrderImbNone");

        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals(5000,mOrderImbalanceListener.getMatchPrice().getValue(),0);
    }

    public void testFarClearingPrice_onNoOrdImbal_Test()
    {  
        MamaPrice temp = new MamaPrice();
        temp.setValue(6000);

        newMessage.addPrice("wFarClearingPrice",1024,temp);
        newMessage.addString("wSecStatusQual",1020,"OrderImbNone");

        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals(6000,mOrderImbalanceListener.getFarClearingPrice().getValue(),0);
    }

    public void testNearClearingPrice_onNoOrdImbal_Test()
    {   
        MamaPrice temp = new MamaPrice();
        temp.setValue(6000);

        newMessage.addPrice("wNearClearingPrice",1025,temp);
        newMessage.addString("wSecStatusQual",1020,"OrderImbNone");

        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals(6000,mOrderImbalanceListener.getNearClearingPrice().getValue(),0);
    }

    public void testNoClearingPrice_onNoOrdImbal_Test()
    {   
        newMessage.addChar("wNoClearingPrice",1026,'c');
        newMessage.addString("wSecStatusQual",1020,"OrderImbNone");
        
        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals('c',mOrderImbalanceListener.getNoClearingPrice());
    }

    public void testPriceVarInd_onNoOrdImbal_Test()
    {  
        newMessage.addChar("wPriceVarInd",1027,'a');
        newMessage.addString("wSecStatusQual",1020,"OrderImbNone");
        
        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals('a',mOrderImbalanceListener.getPriceVarInd());
    }

    public void testCrossType_onNoOrdImbal_Test()
    {  
        newMessage.addChar("wCrossType",1170,'b');
        newMessage.addString("wSecStatusQual",1020,"OrderImbNone");
        
        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals('b',mOrderImbalanceListener.getCrossType());
    }
    
    /*Each Test has one Field set and the SecurityStatus field is set to OrderImbBuy.
      these messages should trigger the onOrderImbalance CallBack.*/

  public void testBuyVolume_OnOrderImbal_Test()
    {   
        newMessage.addI64("wBuyVolume", 239, 1000);

        ticker.callMamdaOnMsg(mSubscription, newMessage);     
        newMessage.addString("wSecStatusQual",1020,"OrderImbBuy");

        assertEquals(1000,mOrderImbalanceListener.getBuyVolume());
    }

    public void testSellVolume_OnOrderImbal_Test()
    {  
        newMessage.addI64("wSellVolume", 453, 2000);
        newMessage.addString("wSecStatusQual",1020,"OrderImbBuy");

        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals(2000,mOrderImbalanceListener.getSellVolume());
    }

    public void testHighIndication_OnOrderImbal_Test()
    {  
        MamaPrice temp = new MamaPrice();

        temp.setValue(2000);

        newMessage.addPrice("wHighIndicationPrice",276,temp);
        newMessage.addString("wSecStatusQual",1020,"OrderImbBuy");

        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals(2000,mOrderImbalanceListener.getHighIndicationPrice().getValue(),0);
    }

    public void testLowIndication_OnOrderImbal_Test()
    { 
        MamaPrice temp = new MamaPrice();

        temp.setValue(3000);

        newMessage.addPrice("wLowIndicationPrice",325,temp);
        newMessage.addString("wSecStatusQual",1020,"OrderImbBuy");

        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals(3000,mOrderImbalanceListener.getHighIndicationPrice().getValue(),0);
    }

    public void testIndication_OnOrderImbal_Test()
    { 
        MamaPrice temp = new MamaPrice();

        temp.setValue(4000);

        newMessage.addPrice("wIndicationPrice",978,temp);
        newMessage.addString("wSecStatusQual",1020,"OrderImbBuy");
        

        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals(4000,mOrderImbalanceListener.getImbalancePrice().getValue(),0);
    }

    public void testMatchVolume_OnOrderImbal_Test()
    {
        newMessage.addI64("wMatchVolume",979,4000);
        newMessage.addString("wSecStatusQual",1020,"OrderImbBuy");        

        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals(4000,mOrderImbalanceListener.getMatchVolume(),0);
    }

    public void testSecurityStatus_OnOrdImbal_Test()
    {
        newMessage.addString("wSecStatusQual",1020,"OrderImbBuy");    

        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals("OrderImbBuy",mOrderImbalanceListener.getImbalanceState());
    }

    public void testInsideMatchPrice_OnOrderImbal_Test()
    { 
        MamaPrice temp = new MamaPrice();
        temp.setValue(5000);

        newMessage.addPrice("wInsideMatchPrice",1023,temp); 
        newMessage.addString("wSecStatusQual",1020,"OrderImbBuy");

        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals(5000,mOrderImbalanceListener.getMatchPrice().getValue(),0);
    }

    public void testFarClearingPrice_OnOrderImbal_Test()
    { 
        MamaPrice temp = new MamaPrice();
        temp.setValue(6000);

        newMessage.addPrice("wFarClearingPrice",1024,temp);
        newMessage.addString("wSecStatusQual",1020,"OrderImbBuy");

        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals(6000,mOrderImbalanceListener.getFarClearingPrice().getValue(),0);
    }

    public void testNearClearingPrice_OnOrderImbal_Test()
    { 
        MamaPrice temp = new MamaPrice();
        temp.setValue(6000);

        newMessage.addPrice("wNearClearingPrice",1025,temp);
        newMessage.addString("wSecStatusQual",1020,"OrderImbBuy");

        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals(6000,mOrderImbalanceListener.getNearClearingPrice().getValue(),0);
    }

    public void testNoClearingPrice_OnOrderImbal_Test()
    {  
        newMessage.addChar("wNoClearingPrice",1026,'c');
        newMessage.addString("wSecStatusQual",1020,"OrderImbBuy");
        
        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals('c',mOrderImbalanceListener.getNoClearingPrice());
    }

    public void testPriceVarInd_OnOrderImbal_Test()
    {  
        newMessage.addChar("wPriceVarInd",1027,'a');
        newMessage.addString("wSecStatusQual",1020,"OrderImbBuy");
        
        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals('a',mOrderImbalanceListener.getPriceVarInd());
    }

    public void testCrossType_OnOrderImbal_Test()
    {
        newMessage.addChar("wCrossType",1170,'b');
        newMessage.addString("wSecStatusQual",1020,"OrderImbBuy");
        
        ticker.callMamdaOnMsg(mSubscription, newMessage);     

        assertEquals('b',mOrderImbalanceListener.getCrossType());
    }

    public static void main (String[] args) 
    {  
        junit.textui.TestRunner.run(suite());
    }  
    
    void addMamaHeaderFields(MamaMsg msg)
    {
        msg.addU8("MdMsgType", 1,(short)0);  
        msg.addU8("MdMsgStatus",2,(short) 0); 
        msg.addU32("MdSeqNum", 10, 42868);    
        msg.addU8("MamaAppMsgType", 18, (short)15);
        msg.addU64("MamaSenderId", 20, 68471603);
        msg.addString("wIssueSymbol", 305, "IBM");     
        msg.addString("wSrcTime", 465," 14:40:33.760");                  
    }      
    
}
