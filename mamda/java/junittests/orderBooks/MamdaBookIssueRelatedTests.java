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

package junittests.orderBooks;

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

import com.wombat.mamda.orderbook.*;
import com.wombat.mamda.*;

import com.wombat.mama.*;
import junittests.orderBooks.*;

public class MamdaBookIssueRelatedTests extends TestCase
{         
    publisherTestImpl myImpl = new publisherTestImpl();
    MamdaOrderBookPriceLevel ownBkLevel = null;
    MamdaOrderBookPriceLevel listBkLevel = null;
    static boolean finest = false;

    protected void setUp()
    {   
        //Initialise Mama
        MamaBridge bridge;
        bridge = Mama.loadBridge("wmw");
        Mama.open();  
        
//         finest = true;

        if(finest)
        {
            Mama.enableLogging (Level.FINEST); 
        }

        myImpl.dictionary = new MamaDictionary();
        myImpl.dictionary.create();
        myImpl.dictionary._populateFromFile("dictionary.txt");
        MamdaCommonFields.reset();
        MamdaCommonFields.setDictionary(myImpl.dictionary,null);
        MamdaOrderBookFields.reset();
        MamdaOrderBookFields.setDictionary (myImpl.dictionary,null);   
     
        myImpl.mySubscription = new MamdaSubscription();
        myImpl.myBookListener = new MamdaOrderBookListener();
        myImpl.mySubscription.addMsgListener (myImpl.myBookListener);  
        myImpl.myBookListener.setProcessEntries (true);
        myImpl.mySubscription.setType( MamaSubscriptionType.BOOK);
        myImpl.myBook = new MamdaOrderBook();
        myImpl.myBook.generateDeltaMsgs(true);
        myImpl.myPublishMsg = new MamaMsg();       

        addMamaHeaderFields(myImpl,
                            myImpl.myPublishMsg,
                            MamaMsgType.TYPE_BOOK_INITIAL, 
                            MamaMsgStatus.STATUS_OK, 
                            (int)0);
        myImpl.myBook.setSymbol("TEST");
        myImpl.myBook.setPartId("NYS");
        myImpl.myTicker = new BookTicker();
        myImpl.myBookTime = new MamaDateTime();
        myImpl.myBookTime.setToNow();        
    }

    public static Test suite() 
    {
        BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
        String myLogging = null;
        int myTest =0;
        String str = null;

        try
        {
            System.out.println("MamaLogging: ");
            myLogging = br.readLine();
          
            if(myLogging != null)
            {
                if(myLogging.equals("yes"))
                {
                    finest = true;
                }                
            }
            if((myLogging != null) && (!myLogging.equals("yes")) && (!myLogging.equals("no")) )
            {
                System.out.print("To enable MamaLogging type yes\n");
            }
        } 
        catch (IOException ioe)
        {
            System.out.println("error trying to enableMamaLogging");
            System.exit(1);
        }           

       try
        {          
            String menu = new String("1 testRemoveAtomicEntryFromLevel\n" + "2 testRemoveAtomicEntryByIdFromLevel\n" +
                                     "0  AllTests\n");

            System.out.println("Select a test to run: \n" + menu + "run test(s) number: ");
            str = br.readLine();

            if( str != null)
            {
                myTest = Integer.parseInt(str);
            }       
        }
        catch (IOException ioe)
        {
            System.out.println("Selection not found\n");
            System.exit(1);
        } 
      
        TestSuite suite= new TestSuite();

        switch (myTest)
        {
            default:
                    return  new TestSuite(MamdaBookIssueRelatedTests.class);            
        }   
    }

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
                     listener.onMsg (sub, msg,(short)ob.typeForMsg(msg));
                }   
            }                
            catch (Exception e)
            {
                e.printStackTrace ();
                System.exit (1);
            }
        }

         public void onBookDelta (MamdaSubscription sub,
                                 MamdaOrderBookListener listener,
                                 MamaMsg msg,
                                 MamdaOrderBookSimpleDelta delta,
                                 MamdaOrderBook book)
        {           
        }

        public void onBookComplexDelta (MamdaSubscription subscription,
                                        MamdaOrderBookListener listener,
                                        MamaMsg msg,
                                        MamdaOrderBookComplexDelta delta,
                                        MamdaOrderBook book)
        {   
        }

        public void onBookClear (MamdaSubscription subscription,
                                 MamdaOrderBookListener listener,
                                 MamaMsg msg,
                                 MamdaOrderBookClear event,
                                 MamdaOrderBook      book)
        {           
        }

        public void onBookRecap (MamdaSubscription sub,
                                 MamdaOrderBookListener listener, MamaMsg msg,
                                 MamdaOrderBookComplexDelta delta,
                                 MamdaOrderBookRecap event,
                                 MamdaOrderBook book)
        {         
        }


        public void onBookGap (MamdaSubscription sub,
                               MamdaOrderBookListener listener,
                               MamaMsg msg,
                               MamdaOrderBookGap event,
                               MamdaOrderBook fullBook)
        { 
        }
    }

    private class publisherTestImpl
    {
        MamaDictionary              dictionary = null;
        MamdaOrderBook              myBook = null;
        MamaMsg                     myPublishMsg = null;
        MamdaSubscription           mySubscription = null;
        MamdaOrderBookListener      myBookListener = null;
        BookTicker                  myTicker = null;
        MamaDateTime                myBookTime = null;
    }

    public void assertEqualEntry(MamdaOrderBookEntry lhs, MamdaOrderBookEntry rhs,String testdescription)
    {
        assertEquals(testdescription,lhs.getId(), rhs.getId());
        assertEquals(testdescription,lhs.getSize(), rhs.getSize(),0);
        assertEquals(testdescription,lhs.getAction(), rhs.getAction());
        assertEquals(testdescription,lhs.getTime().getEpochTimeSeconds(), rhs.getTime().getEpochTimeSeconds(),0);
    }

    public void assertEqualLevel(MamdaOrderBookPriceLevel lhs, MamdaOrderBookPriceLevel rhs, String testdescription)
    {  
        assertEquals (testdescription,lhs.getPrice(),rhs.getPrice());
        assertEquals (testdescription,lhs.getSize(), rhs.getSize(),0);
        assertEquals (testdescription,lhs.getNumEntries(), rhs.getNumEntries(),0);
        assertEquals (testdescription,lhs.getNumEntriesTotal(), rhs.getNumEntriesTotal()); 
        assertEquals (testdescription,lhs.getSide(), rhs.getSide());
        assertEquals (testdescription,lhs.getTime().getEpochTimeSeconds(), rhs.getTime().getEpochTimeSeconds(),0) ;
        assertEquals (testdescription,lhs.getOrderType(), rhs.getOrderType());

        Iterator lhsIter = lhs.entryIterator ();
        Iterator rhsIter = rhs.entryIterator ();

        while(lhsIter.hasNext() && rhsIter.hasNext())
        {
            MamdaOrderBookEntry lhsEntry = (MamdaOrderBookEntry)lhsIter.next();    
            MamdaOrderBookEntry rhsEntry = (MamdaOrderBookEntry)rhsIter.next(); 
            assertEqualEntry(lhsEntry, rhsEntry,testdescription);    
        }      
    }
    
    public void assertEqual(MamdaOrderBook lhs, MamdaOrderBook rhs,String testdescription)
    {  
        Iterator lhsIter = lhs.askIterator();
        Iterator rhsIter = rhs.askIterator();

        while(lhsIter.hasNext())
        {
            MamdaOrderBookPriceLevel lhsLevel = (MamdaOrderBookPriceLevel)lhsIter.next();
            
            while(rhsIter.hasNext())
            {
                MamdaOrderBookPriceLevel rhsLevel = (MamdaOrderBookPriceLevel)rhsIter.next();
                assertEqualLevel(lhsLevel, rhsLevel,testdescription);
            }
        }

        Iterator bidlhsIter = lhs.bidIterator();
        Iterator bidrhsIter = rhs.bidIterator();   

        while(bidlhsIter.hasNext() && bidrhsIter.hasNext())
        {
            MamdaOrderBookPriceLevel bidlhsLevel = (MamdaOrderBookPriceLevel)bidlhsIter.next();          
            MamdaOrderBookPriceLevel bidrhsLevel = (MamdaOrderBookPriceLevel)bidrhsIter.next();   
            assertEqualLevel(bidlhsLevel, bidrhsLevel,testdescription);            
        }

    }

    public MamdaOrderBookEntry myCreateAndAddEntry (publisherTestImpl myImpl, double price,
                                                    String id, double size, int status, 
                                                    char entryAction,
                                                    char side)
    {
        MamdaOrderBookEntry entry = new MamdaOrderBookEntry();
        entry.setId(id);
        entry.setSize(size);
        entry.setAction(entryAction);
        entry.setStatus(status);
        myImpl.myBookTime.setToNow();
        entry.setTime(myImpl.myBookTime); 
        myImpl.myBook.addEntry(entry, price, side, myImpl.myBookTime, null);
        return entry;
    }

    public MamdaOrderBookEntry myCreateAndAddEntryAtLevel (publisherTestImpl myImpl, double price,
                                                           String id, double size, int status, 
                                                           char entryAction,
                                                           char side)
    {
        MamdaOrderBookEntry entry = new MamdaOrderBookEntry();
        entry.setId(id);
        entry.setSize(size);
        entry.setAction(entryAction);
        entry.setStatus(status);
        myImpl.myBookTime.setToNow();
        entry.setTime(myImpl.myBookTime); 

        MamdaOrderBookPriceLevel level = null;
        level = myImpl.myBook.findOrCreateLevel(price, side);
        if (level != null)
        {
            level.addEntry(entry);
        }
        else
        System.out.println("\n myCreateAndAddEntryAtLevel(); level = NULL \n");

        return entry;
    }

    public void myDeleteEntry (publisherTestImpl myImpl, double price,
                            String id, double size, int status, 
                            char entryAction,
                            char side)
    {
        MamdaOrderBookPriceLevel level = null;
        level = myImpl.myBook.findOrCreateLevel(price, side);
        MamdaOrderBookEntry entry = level.findOrCreateEntry(id);
    //     level.removeEntry(entry);
        myImpl.myBook.deleteEntry(entry, myImpl.myBookTime, null);
    }


    public void myRemoveEntry (publisherTestImpl myImpl, double price,
                            String id, double size, int status, 
                            char entryAction,
                            char side)
    {
        MamdaOrderBookPriceLevel level = null;
        level = myImpl.myBook.findOrCreateLevel(price, side);
        if (level != null)
        {
            MamdaOrderBookEntry entry = level.findOrCreateEntry(id);
            level.removeEntry(entry);

        }
        else
        System.out.println("\n myRemoveEntry(); level = NULL \n");
    //     myImpl.myBook.deleteEntry(entry, myImpl.myBookTime, NULL);
    }

    public void myRemoveEntry (publisherTestImpl myImpl,
                               double price,
                               char side,
                               MamdaBookAtomicLevelEntry entry)
    {
        MamdaOrderBookPriceLevel level = null;
        level = myImpl.myBook.findOrCreateLevel(price, side);
        if (level != null)
        {
            level.removeEntry(entry);
        }
        else
        {
            System.out.println("\n myRemoveEntry(); level = NULL \n");
        }
    }

    public void myRemoveEntryById (publisherTestImpl myImpl, double price,
                            String id, double size, int status, 
                            char entryAction,
                            char side)
    {
        MamdaOrderBookPriceLevel level = null;
        level = myImpl.myBook.findOrCreateLevel(price, side);
        if (level != null)
        {
            MamdaOrderBookEntry entry = level.findOrCreateEntry(id);
            level.removeEntryById(entry);
        }
        else 
        System.out.println("\n myRemoveEntry(); level = NULL \n");
    //     myImpl.myBook.deleteEntry(entry, myImpl.myBookTime, NULL);
    }

    public void myRemoveEntryById (publisherTestImpl myImpl,
                                   double price,
                                   char side,
                                   MamdaBookAtomicLevelEntry entry)
    {
        MamdaOrderBookPriceLevel level = null;
        level = myImpl.myBook.findOrCreateLevel(price, side);
        if (level != null)
        {
            level.removeEntryById (entry);
        }
        else
        {
            System.out.println("\n myRemoveEntry(); level = NULL \n");
        }
    }

    public void myAddLevel (publisherTestImpl myImpl, double price, 
                double size, double sizeChange,
                char action, 
                char side, 
                MamdaOrderBookPriceLevel level)
    {
        level.setPrice(price);
        level.setSize(size);
        level.setSizeChange(sizeChange);
        //     level.setNumEntries(numEntries);
        level.setAction(action);
        level.setSide(side); 

        myImpl.myBookTime.setToNow();
        level.setTime(myImpl.myBookTime);
        //add to book
        myImpl.myBook.addLevel(level);
    }
         
     public MamdaOrderBookPriceLevel myFindOrCreateLevel (
                                publisherTestImpl myImpl, double price, 
                                double size, double sizeChange,
                                char action, 
                                char side)
    {
        MamdaOrderBookPriceLevel level = null;
        level = myImpl.myBook.findOrCreateLevel(price, side);
        level.setPrice(price);
        level.setSize(size);
        level.setSizeChange(sizeChange);
/*        level.setNumEntries(numEntries);*/
        level.setAction(action);
        level.setSide(side); 

        myImpl.myBookTime.setToNow();
        level.setTime(myImpl.myBookTime);
        return level;
    }
         
     public MamdaOrderBookPriceLevel myFindOrCreateEntry (
                        publisherTestImpl myImpl, double price, 
                        double size, double sizeChange, double numentries,
                        char action, 
                        char side)
    {
        MamdaOrderBookPriceLevel level = null;
        MamdaOrderBookEntry entry = null;

        level = myImpl.myBook.findOrCreateLevel(price, side);
        level.setSizeChange(sizeChange);
        level.setPrice(price);
        level.setSize(size);

        myImpl.myBookTime.setToNow();
        level.setTime(myImpl.myBookTime);
        level.setAction(MamdaOrderBookPriceLevel.ACTION_ADD);
        level.setSide(side); 
        
        entry = level.findOrCreateEntry("jmg");
        entry.setSize(size);
        entry.setAction(action);
        entry.setTime(myImpl.myBookTime);

        return level;
    }

   public void myUpdateEntry (publisherTestImpl myImpl,
                              String id, double price,
                              double size,char side)
    {
        MamdaOrderBookPriceLevel level = null;
        level= myImpl.myBook.findOrCreateLevel(price, side); 
        MamdaOrderBookEntry entry = null;
        entry = level.findOrCreateEntry(id);
        myImpl.myBookTime.setToNow();
        myImpl.myBook.updateEntry(entry, size, myImpl.myBookTime, null);
    }

    public void myUpdateEntryAtLevel (publisherTestImpl myImpl, 
                                    String id, double price,
                                    double size, char side)
    {
        MamdaOrderBookPriceLevel level = null;
        level= myImpl.myBook.findOrCreateLevel(price, side); 
        MamdaOrderBookEntry entry = null;
        entry = level.findOrCreateEntry(id);
        myImpl.myBookTime.setToNow();
        entry.setSize(size);
        entry.setTime(myImpl.myBookTime);
            
        level.updateEntry(entry);
    }

    public void myUpdateLevel (publisherTestImpl myImpl, double price, 
                            double size, double sizeChange,
                            char action, 
                            char side)
    {
        MamdaOrderBookPriceLevel level = null;
        level = myImpl.myBook.findOrCreateLevel(price, side);
        level.setPrice(price);
        level.setSize(size);
        level.setSizeChange(sizeChange);
        level.setAction(action);
        level.setSide(side); 

        myImpl.myBookTime.setToNow();
        level.setTime(myImpl.myBookTime);
        myImpl.myBook.updateLevel(level);
    }

    public void populateRecapAndValidate (publisherTestImpl myImpl, String testdescription)
    {      
        myImpl.myBook.populateRecap(myImpl.myPublishMsg); 
        myImpl.myTicker.callMamdaOnMsg(myImpl.mySubscription, myImpl.myPublishMsg); 
        assertEqual(myImpl.myBook, myImpl.myBookListener.getOrderBook(),testdescription);
        myImpl.myBook.clearDeltaList();
    }

    public void populateDeltaAndValidate( publisherTestImpl myImpl, String testdescription)
    {
        boolean publish = false; 
        publish = myImpl.myBook.populateDelta(myImpl.myPublishMsg);  
        if (publish) myImpl.myTicker.callMamdaOnMsg(myImpl.mySubscription, myImpl.myPublishMsg); 
        assertEqual(myImpl.myBook , myImpl.myBookListener.getOrderBook(),testdescription);             
    }

    public void addMamaHeaderFields (
                            publisherTestImpl myImpl,
                            MamaMsg           msg,
                            short             msgType,
                            short             msgStatus,
                            int               seqNum)
    {
        myImpl.myPublishMsg.clear();  

        msg.addU8  (null, 1,     msgType);
        msg.addU8  (null, 2,   msgStatus);
        msg.addU32(null, 10, seqNum);             
        msg.addU64 (null, 20,  (short)  1);         
   
    }

    public void dumpinfo()
    {        
            System.out.print("\n **************** myPublishMsg ****************************\n");
            System.out.print("\n" + myImpl.myPublishMsg.toString() + "\n");
            System.out.print("\n **********************************************************\n");
            System.out.print("\n **************** Impl Book *******************************\n");
            myImpl.myBook.dump();
            System.out.print("\n **********************************************************\n");
            System.out.print("\n **************** Listener Book ***************************\n");
            myImpl.myBookListener.getOrderBook().dump();
            System.out.print("\n **********************************************************\n");      
    }

    public static void main (String[] args) 
    {  
        junit.textui.TestRunner.run(suite());
    }  

}
