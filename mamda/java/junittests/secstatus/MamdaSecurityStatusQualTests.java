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

public class MamdaSecurityStatusQualTests extends TestCase
{ 
    public MamdaSubscription            mSubscription           = null;
    public MamdaSecurityStatusListener  mSecurityStatusListener = null;
    public SecStatusTickerCB            ticker                  = null;
    public MamaMsg                      newMessage              = null;
    public String                       testdescription         = null;


    private static class SecStatusTickerCB implements MamdaSecurityStatusHandler
    {

        public String myTestStr  = null;
        public short  myTestEnum = 0;

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

        public void onSecurityStatusRecap (
        MamdaSubscription            subscription,
        MamdaSecurityStatusListener  listener,
        MamaMsg                      msg,
        MamdaSecurityStatusRecap     recap)
        {
        }

        public void onSecurityStatusUpdate (
                MamdaSubscription            subscription,
                MamdaSecurityStatusListener  listener,
                MamaMsg                      msg,
                MamdaSecurityStatusUpdate    event,
                MamdaSecurityStatusRecap     recap)
        {
            myTestStr = recap.getSecurityStatusQualifierStr();
            System.out.print("myTestStr ="+myTestStr);
            myTestEnum = recap.getSecurityStatusQualifierEnum();

        }
    }

    protected void setUp()
    {   
        try
        {
            MamaBridge bridge;
            bridge = Mama.loadBridge("wmw");
            Mama.open();
/*                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);*/

            MamaDictionary mDictionary = new MamaDictionary();
            mDictionary.create();   
            mDictionary._populateFromFile("dictionary.txt");

            MamdaCommonFields.setDictionary (mDictionary,null);
            MamdaSecurityStatusFields.reset();
            MamdaSecurityStatusFields.setDictionary (mDictionary,null);   
         
            mSubscription = new MamdaSubscription();           
            mSecurityStatusListener = new MamdaSecurityStatusListener();   
            mSubscription.addMsgListener(mSecurityStatusListener);

            ticker = new SecStatusTickerCB();      
            mSecurityStatusListener.addHandler (ticker);   

            newMessage = new MamaMsg();  
            addMamaHeaderFields(newMessage);

            testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t              ****     MamdaSecurityStatusQualTests   failed         ****          "+
               "\n This test passes in the wSecStatusQual/wSecurityStatusOrig field to the              "+
               "\n SecurityStatusListener and checks that the field  and fieldstates are                "+
               "\n correctly cached and updated.                                                        "+             
               "\n ********************************************************************************** \n" ); 
        }
        catch (Exception e)
        {
            e.printStackTrace ();
            System.exit (1);
        }
    }

    public static Test suite() {      

// //         //Run single tests.
//         TestSuite suite= new TestSuite();
//         suite.addTest(
//             new MamdaSecurityStatusQualTests() 
//             {
//                  protected void runTest() { test_NoneEnum();}
//             }
//            );   
//         
//         return suite; 

//         Run all tests.
        return  new TestSuite(MamdaSecurityStatusQualTests.class);
    }

    public void testSecStatusQualStr()
    {
        MamaMsg mMsg = new MamaMsg();
        addMamaHeaderFields (mMsg,
                             MamaMsgType.TYPE_SEC_STATUS,
                             MamaMsgStatus.STATUS_OK,
                             (int)1);
        mMsg.addString (null, 1020, "normal");

        ticker.callMamdaOnMsg (mSubscription, mMsg);

        String tmpstr = "normal";
     
        assertTrue (testdescription, tmpstr.compareTo(mSecurityStatusListener.getSecurityStatusQualifierStr()) == 0);
       
        assertTrue (testdescription, MamdaFieldState.MODIFIED == mSecurityStatusListener.getSecurityStatusQualifierStrFieldState());      

        mMsg = null;
    }

    public void testSecStatusOrigStr()
    {
        MamaMsg mMsg = new MamaMsg();
        addMamaHeaderFields (mMsg,
                             MamaMsgType.TYPE_SEC_STATUS,
                             MamaMsgStatus.STATUS_OK,
                             (int)1);
        mMsg.addString (null, 598, "orig-normal");

        ticker.callMamdaOnMsg (mSubscription, mMsg);

        String tmpstr = "orig-normal";   

        assertTrue (testdescription, tmpstr.compareTo(mSecurityStatusListener.getSecurityStatusOrigStr()) == 0);
    
        assertTrue (testdescription, MamdaFieldState.MODIFIED == mSecurityStatusListener.getSecurityStatusOrigStrFieldState());        
    } 

    public void test_NoneStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"None");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("None",ticker.myTestStr);
        assertEquals(0,ticker.myTestEnum);

        ticker.myTestStr  = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OpeningStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Opening");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Opening",ticker.myTestStr);
        assertEquals(1,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ExcusedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Excused");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Excused",ticker.myTestStr);
        assertEquals(7,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_WithdrawnStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Withdrawn");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Withdrawn",ticker.myTestStr);
        assertEquals(8,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_SuspendedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Suspended");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Suspended",ticker.myTestStr);
        assertEquals(9,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ResumeStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Resume");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Resume",ticker.myTestStr);
        assertEquals(11,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_QuoteResumeStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"QuoteResume");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("QuoteResume",ticker.myTestStr);
        assertEquals(12,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_TradeResumeStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"TradeResume");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("TradeResume",ticker.myTestStr);
        assertEquals(13,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    } 
 
    public void test_ResumeTimeStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"ResumeTime");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("ResumeTime",ticker.myTestStr);
        assertEquals(14,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_MktImbBuyStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"MktImbBuy");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("MktImbBuy",ticker.myTestStr);
        assertEquals(16,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_MktImbSellStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"MktImbSell");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("MktImbSell",ticker.myTestStr);
        assertEquals(17,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_NoMktImbStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"NoMktImb");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("NoMktImb",ticker.myTestStr);
        assertEquals(18,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_MocImbBuyStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"MocImbBuy");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("MocImbBuy",ticker.myTestStr);
        assertEquals(19,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_MocImbSellStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"MocImbSell");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("MocImbSell",ticker.myTestStr);
        assertEquals(20,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;

    } 
 
    public void test_NoMocImbStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"NoMocImb");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("NoMocImb",ticker.myTestStr);
        assertEquals(21,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OrderImbStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrderImb");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OrderImb",ticker.myTestStr);
        assertEquals(22,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OrderInfStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrderInf");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OrderInf",ticker.myTestStr);
        assertEquals(23,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OrderImbBuyStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrderImbBuy");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OrderImbBuy",ticker.myTestStr);
        assertEquals(24,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OrderImbSellStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrderImbSell");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OrderImbSell",ticker.myTestStr);
        assertEquals(25,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OrderImbNoneStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrderImbNone");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OrderImbNone",ticker.myTestStr);
        assertEquals(26,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_LoaImbBuyStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"LoaImbBuy");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("LoaImbBuy",ticker.myTestStr);
        assertEquals(27,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_LoaImbSellStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"LoaImbSell");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("LoaImbSell",ticker.myTestStr);
        assertEquals(28,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    } 
 
    public void test_NoLoaImbStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"NoLoaImb");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("NoLoaImb",ticker.myTestStr);
        assertEquals(29,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    } 

    public void test_OrdersElimatedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrdersElimated");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OrdersElimated",ticker.myTestStr);
        assertEquals(30,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_RangeIndStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"RangeInd");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("RangeInd",ticker.myTestStr);
        assertEquals(31,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ItsPreOpenStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"ItsPreOpen");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("ItsPreOpen",ticker.myTestStr);
        assertEquals(32,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ReservedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Reserved");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Reserved",ticker.myTestStr);
        assertEquals(33,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_FrozenStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Frozen");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Frozen",ticker.myTestStr);
        assertEquals(34,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_PreOpenStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"PreOpen");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("PreOpen",ticker.myTestStr);
        assertEquals(35,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ThoImbBuyStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"ThoImbBuy");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("ThoImbBuy",ticker.myTestStr);
        assertEquals(36,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;

    }  

    public void test_ThoImbSellStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"ThoImbSell");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("ThoImbSell",ticker.myTestStr);
        assertEquals(37,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_NoThoImbStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"NoThoImb");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("NoThoImb",ticker.myTestStr);
        assertEquals(38,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    } 
 
    public void test_AddInfoStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"AddInfo");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("AddInfo",ticker.myTestStr);
        assertEquals(41,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_IpoImbBuyStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"IpoImbBuy");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("IpoImbBuy",ticker.myTestStr);
        assertEquals(42,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_IpoImbSellStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"IpoImbSell");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("IpoImbSell",ticker.myTestStr);
        assertEquals(43,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_IpoNoImbStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"IpoNoImb");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("IpoNoImb",ticker.myTestStr);
        assertEquals(44,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }     

    public void test_OpenDelayStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OpenDelay");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OpenDelay",ticker.myTestStr);
        assertEquals(51,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_NoOpenNoResumeStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"NoOpenNoResume");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("NoOpenNoResume",ticker.myTestStr);
        assertEquals(52,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_PriceIndStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"PriceInd");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("PriceInd",ticker.myTestStr);
        assertEquals(53,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_EquipmentStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Equipment");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Equipment",ticker.myTestStr);
        assertEquals(54,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_FilingsStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Filings");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Filings",ticker.myTestStr);
        assertEquals(55,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_NewsStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"News");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("News",ticker.myTestStr);
        assertEquals(56,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_NewsDissemStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"NewsDissem");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("NewsDissem",ticker.myTestStr);
        assertEquals(57,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ListingStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Listing");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Listing",ticker.myTestStr);
        assertEquals(58,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OperationStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Operation");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Operation",ticker.myTestStr);
        assertEquals(59,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_InfoStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Info");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Info",ticker.myTestStr);
        assertEquals(60,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_SECStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"SEC");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("SEC",ticker.myTestStr);
        assertEquals(61,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_TimesStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Times");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Times",ticker.myTestStr);
        assertEquals(62,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  


   public void test_OtherStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Other");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Other",ticker.myTestStr);
        assertEquals(63,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_RelatedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Related");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Related",ticker.myTestStr);
        assertEquals(64,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

   public void test_IPOStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"IPO");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("IPO",ticker.myTestStr);
        assertEquals(65,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_EmcImbBuyStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"EmcImbBuy");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("EmcImbBuy",ticker.myTestStr);
        assertEquals(46,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_EmcImbSellStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"EmcImbSell");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("EmcImbSell",ticker.myTestStr);
        assertEquals(47,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_EmcImbNoneStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"EmcImbNone");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("EmcImbNone",ticker.myTestStr);
        assertEquals(48,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_PreCrossStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Pre-Cross");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Pre-Cross",ticker.myTestStr);
        assertEquals(66,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_CrossStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Cross");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Cross",ticker.myTestStr);
        assertEquals(67,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ReleasedForQuotationStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Released For Quotation (IPO)");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Released For Quotation (IPO)",ticker.myTestStr);
        assertEquals(68,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_IPOWindowExtensionStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"IPO Window Extension");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("IPO Window Extension",ticker.myTestStr);
        assertEquals(69,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }

    public void test_PreClosingStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"PreClosing");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("PreClosing",ticker.myTestStr);
        assertEquals(70,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_AuctionExtensionStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Auction Extension");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Auction Extension",ticker.myTestStr);
        assertEquals(71,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_VolatilityAuctionStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Volatility Auction");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Volatility Auction",ticker.myTestStr);
        assertEquals(72,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_SecurityAuthorizedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"SecurityAuthorized");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("SecurityAuthorized",ticker.myTestStr);
        assertEquals(73,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_SecurityForbiddenStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"SecurityForbidden");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("SecurityForbidden",ticker.myTestStr);
        assertEquals(74,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_FastMarketStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"FastMarket");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("FastMarket",ticker.myTestStr);
        assertEquals(75,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_SlowMarketStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"SlowMarket");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("SlowMarket",ticker.myTestStr);
        assertEquals(76,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_SubPennyTradingStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"SubPennyTrading");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("SubPennyTrading",ticker.myTestStr);
        assertEquals(77,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OrderInputStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrderInput");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OrderInput",ticker.myTestStr);
        assertEquals(78,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_PreOrderMatchingStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"PreOrderMatching");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("PreOrderMatching",ticker.myTestStr);
        assertEquals(79,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OrderMatchingStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrderMatching");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OrderMatching",ticker.myTestStr);
        assertEquals(80,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_BlockingStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Blocking");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Blocking",ticker.myTestStr);
        assertEquals(81,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OrderCancelStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrderCancel");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OrderCancel",ticker.myTestStr);
        assertEquals(82,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_FixedPriceStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"FixedPrice");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("FixedPrice",ticker.myTestStr);
        assertEquals(83,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_SalesInputStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"SalesInput");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("SalesInput",ticker.myTestStr);
        assertEquals(84,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ExchangeInterventionStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"ExchangeIntervention");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("ExchangeIntervention",ticker.myTestStr);
        assertEquals(85,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_PreAuctionStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"PreAuction");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("PreAuction",ticker.myTestStr);
        assertEquals(86,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_AdjustStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Adjust");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Adjust",ticker.myTestStr);
        assertEquals(87,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_AdjustOnStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"AdjustOn");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("AdjustOn",ticker.myTestStr);
        assertEquals(88,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_LateTradingStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"LateTrading");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("LateTrading",ticker.myTestStr);
        assertEquals(89,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_EnquireStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Enquire");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Enquire",ticker.myTestStr);
        assertEquals(90,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_PreNightTradingStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"PreNightTrading");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("PreNightTrading",ticker.myTestStr);
        assertEquals(91,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OpenNightTradingStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OpenNightTrading");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OpenNightTrading",ticker.myTestStr);
        assertEquals(92,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_SubHolidayStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"SubHoliday");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("SubHoliday",ticker.myTestStr);
        assertEquals(93,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_BidOnlyStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"BidOnly");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("BidOnly",ticker.myTestStr);
        assertEquals(94,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_AskOnlyStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"AskOnly");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("AskOnly",ticker.myTestStr);
        assertEquals(95,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OpeningDelayCommonStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OpeningDelayCommon");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OpeningDelayCommon",ticker.myTestStr);
        assertEquals(101,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ResumeCommonStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"ResumeCommon");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("ResumeCommon",ticker.myTestStr);
        assertEquals(102,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_NoOpenNoResumeCommonStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"NoOpenNoResumeCommon");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("NoOpenNoResumeCommon",ticker.myTestStr);
        assertEquals(103,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_NewsDisseminationRelatedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"NewsDisseminationRelated");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("NewsDisseminationRelated",ticker.myTestStr);
        assertEquals(104,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OrderInfluxRelatedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrderInfluxRelated");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OrderInfluxRelated",ticker.myTestStr);
        assertEquals(105,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OrderImbRelatedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrderImbRelated");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OrderImbRelated",ticker.myTestStr);
        assertEquals(106,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_InfoRequestedRelatedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"InfoRequestedRelated");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("InfoRequestedRelated",ticker.myTestStr);
        assertEquals(107,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_NewsPendingRelatedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"NewsPendingRelated");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("NewsPendingRelated",ticker.myTestStr);
        assertEquals(108,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_EquipChangeOverRelatedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"EquipChangeOverRelated");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("EquipChangeOverRelated",ticker.myTestStr);
        assertEquals(109,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_SubPennyTradingRelatedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"SubPennyTradingRelated");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("SubPennyTradingRelated",ticker.myTestStr);
        assertEquals(110,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ImbPreOpenBuyStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"ImbPreOpenBuy");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("ImbPreOpenBuy",ticker.myTestStr);
        assertEquals(112,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ImbPreOpenSellStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"ImbPreOpenSell");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("ImbPreOpenSell",ticker.myTestStr);
        assertEquals(113,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ImbPreCloseBuyStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"ImbPreCloseBuy");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("ImbPreCloseBuy",ticker.myTestStr);
        assertEquals(114,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ImbPreCloseSellStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"ImbPreCloseSell");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("ImbPreCloseSell",ticker.myTestStr);
        assertEquals(115,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_VolatilityPauseStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"VolatilityPause");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("VolatilityPause",ticker.myTestStr);
        assertEquals(116,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_VolatilityPauseQuoteResumeStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"VolatilityPauseQuoteResume");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("VolatilityPauseQuoteResume",ticker.myTestStr);
        assertEquals(117,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_UnknownStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Unknown");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Unknown",ticker.myTestStr);
        assertEquals(99,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

/*

    public void test_NoneEnum()
    {  
        newMessage.addI8("wSecStatusQual",1020,(byte)4);
        ticker.callMamdaOnMsg(mSubscription, newMessage);    
        assertEquals("None",ticker.myTestStr);
        assertEquals(0,ticker.myTestEnum);

        ticker.myTestStr  = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OpeningStr()
    {  
        newMessage.addI8("wSecStatusQual",1020,"Opening");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Opening",ticker.myTestStr);
        assertEquals(1,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ExcusedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Excused");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Excused",ticker.myTestStr);
        assertEquals(7,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_WithdrawnStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Withdrawn");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Withdrawn",ticker.myTestStr);
        assertEquals(8,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_SuspendedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Suspended");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Suspended",ticker.myTestStr);
        assertEquals(9,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ResumeStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Resume");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Resume",ticker.myTestStr);
        assertEquals(11,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_QuoteResumeStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"QuoteResume");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("QuoteResume",ticker.myTestStr);
        assertEquals(12,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_TradeResumeStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"TradeResume");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("TradeResume",ticker.myTestStr);
        assertEquals(13,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    } 
 
    public void test_ResumeTimeStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"ResumeTime");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("ResumeTime",ticker.myTestStr);
        assertEquals(14,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_MktImbBuyStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"MktImbBuy");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("MktImbBuy",ticker.myTestStr);
        assertEquals(16,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_MktImbSellStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"MktImbSell");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("MktImbSell",ticker.myTestStr);
        assertEquals(17,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_NoMktImbStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"NoMktImb");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("NoMktImb",ticker.myTestStr);
        assertEquals(18,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_MocImbBuyStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"MocImbBuy");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("MocImbBuy",ticker.myTestStr);
        assertEquals(19,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_MocImbSellStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"MocImbSell");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("MocImbSell",ticker.myTestStr);
        assertEquals(20,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;

    } 
 
    public void test_NoMocImbStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"NoMocImb");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("NoMocImb",ticker.myTestStr);
        assertEquals(21,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OrderImbStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrderImb");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OrderImb",ticker.myTestStr);
        assertEquals(22,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OrderInfStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrderInf");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OrderInf",ticker.myTestStr);
        assertEquals(23,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OrderImbBuyStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrderImbBuy");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OrderImbBuy",ticker.myTestStr);
        assertEquals(24,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OrderImbSellStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrderImbSell");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OrderImbSell",ticker.myTestStr);
        assertEquals(25,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OrderImbNoneStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrderImbNone");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OrderImbNone",ticker.myTestStr);
        assertEquals(26,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_LoaImbBuyStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"LoaImbBuy");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("LoaImbBuy",ticker.myTestStr);
        assertEquals(27,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_LoaImbSellStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"LoaImbSell");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("LoaImbSell",ticker.myTestStr);
        assertEquals(28,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    } 
 
    public void test_NoLoaImbStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"NoLoaImb");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("NoLoaImb",ticker.myTestStr);
        assertEquals(29,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    } 

    public void test_OrdersElimatedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrdersElimated");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OrdersElimated",ticker.myTestStr);
        assertEquals(30,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_RangeIndStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"RangeInd");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("RangeInd",ticker.myTestStr);
        assertEquals(31,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ItsPreOpenStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"ItsPreOpen");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("ItsPreOpen",ticker.myTestStr);
        assertEquals(32,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ReservedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Reserved");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Reserved",ticker.myTestStr);
        assertEquals(33,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_FrozenStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Frozen");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Frozen",ticker.myTestStr);
        assertEquals(34,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_PreOpenStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"PreOpen");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("PreOpen",ticker.myTestStr);
        assertEquals(35,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ThoImbBuyStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"ThoImbBuy");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("ThoImbBuy",ticker.myTestStr);
        assertEquals(36,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;

    }  

    public void test_ThoImbSellStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"ThoImbSell");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("ThoImbSell",ticker.myTestStr);
        assertEquals(37,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_NoThoImbStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"NoThoImb");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("NoThoImb",ticker.myTestStr);
        assertEquals(38,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    } 
 
    public void test_AddInfoStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"AddInfo");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("AddInfo",ticker.myTestStr);
        assertEquals(41,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_IpoImbBuyStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"IpoImbBuy");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("IpoImbBuy",ticker.myTestStr);
        assertEquals(42,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_IpoImbSellStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"IpoImbSell");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("IpoImbSell",ticker.myTestStr);
        assertEquals(43,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_IpoNoImbStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"IpoNoImb");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("IpoNoImb",ticker.myTestStr);
        assertEquals(44,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }     

    public void test_OpenDelayStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OpenDelay");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OpenDelay",ticker.myTestStr);
        assertEquals(51,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_NoOpenNoResumeStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"NoOpenNoResume");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("NoOpenNoResume",ticker.myTestStr);
        assertEquals(52,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_PriceIndStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"PriceInd");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("PriceInd",ticker.myTestStr);
        assertEquals(53,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_EquipmentStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Equipment");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Equipment",ticker.myTestStr);
        assertEquals(54,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_FilingsStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Filings");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Filings",ticker.myTestStr);
        assertEquals(55,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_NewsStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"News");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("News",ticker.myTestStr);
        assertEquals(56,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_NewsDissemStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"NewsDissem");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("NewsDissem",ticker.myTestStr);
        assertEquals(57,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ListingStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Listing");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Listing",ticker.myTestStr);
        assertEquals(58,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OperationStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Operation");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Operation",ticker.myTestStr);
        assertEquals(59,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_InfoStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Info");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Info",ticker.myTestStr);
        assertEquals(60,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_SECStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"SEC");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("SEC",ticker.myTestStr);
        assertEquals(61,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_TimesStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Times");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Times",ticker.myTestStr);
        assertEquals(62,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  


   public void test_OtherStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Other");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Other",ticker.myTestStr);
        assertEquals(63,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_RelatedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Related");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Related",ticker.myTestStr);
        assertEquals(64,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

   public void test_IPOStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"IPO");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("IPO",ticker.myTestStr);
        assertEquals(65,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_EmcImbBuyStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"EmcImbBuy");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("EmcImbBuy",ticker.myTestStr);
        assertEquals(46,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_EmcImbSellStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"EmcImbSell");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("EmcImbSell",ticker.myTestStr);
        assertEquals(47,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_EmcImbNoneStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"EmcImbNone");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("EmcImbNone",ticker.myTestStr);
        assertEquals(48,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_PreCrossStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Pre-Cross");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Pre-Cross",ticker.myTestStr);
        assertEquals(66,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_CrossStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Cross");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Cross",ticker.myTestStr);
        assertEquals(67,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ReleasedForQuotationStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Released For Quotation (IPO)");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Released For Quotation (IPO)",ticker.myTestStr);
        assertEquals(68,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_IPOWindowExtensionStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"IPO Window Extension");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("IPO Window Extension",ticker.myTestStr);
        assertEquals(69,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }

    public void test_PreClosingStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"PreClosing");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("PreClosing",ticker.myTestStr);
        assertEquals(70,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_AuctionExtensionStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Auction Extension");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Auction Extension",ticker.myTestStr);
        assertEquals(71,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_VolatilityAuctionStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Volatility Auction");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Volatility Auction",ticker.myTestStr);
        assertEquals(72,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_SecurityAuthorizedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"SecurityAuthorized");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("SecurityAuthorized",ticker.myTestStr);
        assertEquals(73,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_SecurityForbiddenStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"SecurityForbidden");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("SecurityForbidden",ticker.myTestStr);
        assertEquals(74,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_FastMarketStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"FastMarket");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("FastMarket",ticker.myTestStr);
        assertEquals(75,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_SlowMarketStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"SlowMarket");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("SlowMarket",ticker.myTestStr);
        assertEquals(76,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_SubPennyTradingStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"SubPennyTrading");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("SubPennyTrading",ticker.myTestStr);
        assertEquals(77,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OrderInputStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrderInput");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OrderInput",ticker.myTestStr);
        assertEquals(78,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_PreOrderMatchingStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"PreOrderMatching");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("PreOrderMatching",ticker.myTestStr);
        assertEquals(79,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OrderMatchingStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrderMatching");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OrderMatching",ticker.myTestStr);
        assertEquals(80,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_BlockingStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Blocking");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Blocking",ticker.myTestStr);
        assertEquals(81,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OrderCancelStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrderCancel");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OrderCancel",ticker.myTestStr);
        assertEquals(82,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_FixedPriceStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"FixedPrice");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("FixedPrice",ticker.myTestStr);
        assertEquals(83,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_SalesInputStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"SalesInput");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("SalesInput",ticker.myTestStr);
        assertEquals(84,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ExchangeInterventionStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"ExchangeIntervention");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("ExchangeIntervention",ticker.myTestStr);
        assertEquals(85,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_PreAuctionStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"PreAuction");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("PreAuction",ticker.myTestStr);
        assertEquals(86,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_AdjustStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Adjust");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Adjust",ticker.myTestStr);
        assertEquals(87,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_AdjustOnStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"AdjustOn");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("AdjustOn",ticker.myTestStr);
        assertEquals(88,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_LateTradingStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"LateTrading");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("LateTrading",ticker.myTestStr);
        assertEquals(89,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_EnquireStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Enquire");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Enquire",ticker.myTestStr);
        assertEquals(90,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_PreNightTradingStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"PreNightTrading");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("PreNightTrading",ticker.myTestStr);
        assertEquals(91,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OpenNightTradingStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OpenNightTrading");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OpenNightTrading",ticker.myTestStr);
        assertEquals(92,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_SubHolidayStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"SubHoliday");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("SubHoliday",ticker.myTestStr);
        assertEquals(93,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_BidOnlyStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"BidOnly");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("BidOnly",ticker.myTestStr);
        assertEquals(94,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_AskOnlyStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"AskOnly");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("AskOnly",ticker.myTestStr);
        assertEquals(95,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OpeningDelayCommonStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OpeningDelayCommon");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OpeningDelayCommon",ticker.myTestStr);
        assertEquals(101,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ResumeCommonStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"ResumeCommon");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("ResumeCommon",ticker.myTestStr);
        assertEquals(102,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_NoOpenNoResumeCommonStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"NoOpenNoResumeCommon");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("NoOpenNoResumeCommon",ticker.myTestStr);
        assertEquals(103,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_NewsDisseminationRelatedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"NewsDisseminationRelated");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("NewsDisseminationRelated",ticker.myTestStr);
        assertEquals(104,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OrderInfluxRelatedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrderInfluxRelated");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OrderInfluxRelated",ticker.myTestStr);
        assertEquals(105,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_OrderImbRelatedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"OrderImbRelated");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("OrderImbRelated",ticker.myTestStr);
        assertEquals(106,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_InfoRequestedRelatedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"InfoRequestedRelated");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("InfoRequestedRelated",ticker.myTestStr);
        assertEquals(107,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_NewsPendingRelatedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"NewsPendingRelated");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("NewsPendingRelated",ticker.myTestStr);
        assertEquals(108,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_EquipChangeOverRelatedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"EquipChangeOverRelated");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("EquipChangeOverRelated",ticker.myTestStr);
        assertEquals(109,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_SubPennyTradingRelatedStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"SubPennyTradingRelated");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("SubPennyTradingRelated",ticker.myTestStr);
        assertEquals(110,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ImbPreOpenBuyStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"ImbPreOpenBuy");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("ImbPreOpenBuy",ticker.myTestStr);
        assertEquals(112,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ImbPreOpenSellStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"ImbPreOpenSell");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("ImbPreOpenSell",ticker.myTestStr);
        assertEquals(113,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ImbPreCloseBuyStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"ImbPreCloseBuy");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("ImbPreCloseBuy",ticker.myTestStr);
        assertEquals(114,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_ImbPreCloseSellStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"ImbPreCloseSell");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("ImbPreCloseSell",ticker.myTestStr);
        assertEquals(115,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_VolatilityPauseStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"VolatilityPause");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("VolatilityPause",ticker.myTestStr);
        assertEquals(116,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_VolatilityPauseQuoteResumeStr()
    {  
        newMessage.addI8("wSecStatusQual",1020,"VolatilityPauseQuoteResume");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("VolatilityPauseQuoteResume",ticker.myTestStr);
        assertEquals(117,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

    public void test_UnknownStr()
    {  
        newMessage.addString("wSecStatusQual",1020,"Unknown");
        ticker.callMamdaOnMsg(mSubscription, newMessage);    

        assertEquals("Unknown",ticker.myTestStr);
        assertEquals(99,ticker.myTestEnum);

        ticker.myTestStr = null;
        ticker.myTestEnum = 0;
    }  

*/





















    public static void main (String[] args) 
    {  
        junit.textui.TestRunner.run(suite());
    }  
    
    void addMamaHeaderFields(MamaMsg msg)
    {
        msg.addU8     ("MdMsgType",      1,   (short)0);  
        msg.addU8     ("MdMsgStatus",    2,   (short) 0); 
        msg.addU32    ("MdSeqNum",       10,  42868);    
        msg.addU8     ("MamaAppMsgType", 18,  (short)15);
        msg.addU64    ("MamaSenderId",   20,  68471603);
        msg.addString ("wIssueSymbol",   305, "SPY");     
        msg.addString ("wSrcTime",       465, "14:40:33.760");                  
    }

    public void addMamaHeaderFields (                           
                            MamaMsg           msg,
                            short             msgType,
                            short             msgStatus,
                            int               seqNum)
    {  
        msg.addU8  (null, 1,  msgType);
        msg.addU8  (null, 2,  msgStatus);
        msg.addU32 (null, 10, seqNum);             
        msg.addU64 (null, 20, (short)1); 
    } 
    
}
