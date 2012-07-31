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

public class MamdaBookPublisherTests extends TestCase
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
            String menu = new String(" 1 testFindOrCreateLevel\n" + " 2 testAddEntry\n" + " 3 testAddEntryAtLevel\n" + " 4 testAddLevel\n" +
                                     " 5 testFindOrCreateLevel\n" + " 6 testFindOrCreateEntry\n" + " 7 testUpdateEntryAtLevel\n" +
                                     " 8 testUpdateEntry\n" + " 9 testUpdateLevel\n" + "10 testOneLevelTwoEntries\n" +
                                     "11 testFlattenEntryInfo\n" + "12 testFlattenTwoEntriesOneLevel\n" + "13 testFlattenOneEntry\n" +
                                     "14 testRemoveEntryFromLevel\n" + "15 testRemoveEntryByIdFromLevel\n" + "16 testDeleteEntryFromLevel\n" +
                                     "17 testpopulateDeltaEmptyState\n" + "18 testpopulateRecapEmptyState\n" + "19 testMultipleLevelsAndEntries\n" + 
                                     "20 testUpdateMultipleEntries\n" + "21 testDisablePublishing\n" + "22 testUpdateMultipleLevels\n" + 
                                     "23 testPopulateRecapSimpleCpuTest\n" + "24 testPopulateRecapComplexCpu\n" + "25 testPopulateDeltaSimpleCpuTest\n" +
                                     "26 testPopulateDeltaComplexCpuTest\n" + "27 testPopulateLargeDeltaCpuTest\n" + "28 testPopulateDeltaGrowOnceCpuTest\n" +
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
            case 1:                   
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testFindOrCreateLevel(); }
                        }
                    );   
                    
                    return suite;   
            case 2:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testAddEntry(); }
                        }
                    );   
                    return suite; 
            case 3:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testAddEntryAtLevel(); }
                        }
                    );
                    return suite; 
            case 4:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testAddLevel(); }
                        }
                    );
                    return suite;        
            case 5:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testFindOrCreateLevel(); }
                        }
                    );
                    return suite;   
            case 6:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testFindOrCreateEntry(); }
                        }
                    );
                    return suite;     
            case 7:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testUpdateEntryAtLevel(); }
                        }
                    );
                    return suite;     
            case 8:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testUpdateEntry(); }
                        }
                    );
                    return suite;     
            case 9:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testUpdateLevel(); }
                        }
                    );
                    return suite;     
            case 10:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testOneLevelTwoEntries(); }
                        }
                    );
                    return suite;
            case 11:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testFlattenEntryInfo(); }
                        }
                    );
                    return suite;          
            case 12:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testFlattenTwoEntriesOneLevel(); }
                        }
                    );
                    return suite;     
            case 13:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testFlattenOneEntry(); }
                        }
                    );
                    return suite;     
            case 14:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testRemoveEntryFromLevel(); }
                        }
                    );
                    return suite;     
            case 15:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testRemoveEntryByIdFromLevel(); }
                        }
                    );
                    return suite;     
            case 16:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testDeleteEntryFromLevel(); }
                        }
                    );
                    return suite;   
            case 17:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testpopulateDeltaEmptyState(); }
                        }
                    );
                    return suite;     
            case 18:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testpopulateRecapEmptyState(); }
                        }
                    );
                    return suite;  
            case 19:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testMultipleLevelsAndEntries(); }
                        }
                    );
                    return suite; 
            case 20:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testUpdateMultipleEntries(); }
                        }
                    );
                    return suite;  
            case 21:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testDisablePublishing(); }
                        }
                    );
                    return suite;
            case 22:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testUpdateMultipleLevels(); }
                        }
                    );
                    return suite;  
            case 23:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testPopulateRecapSimpleCpuTest(); }
                        }
                    );
                    return suite;  
            case 24:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testPopulateRecapComplexCpu(); }
                        }
                    );
                    return suite;  
            case 25:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testPopulateDeltaSimpleCpuTest(); }
                        }
                    );
                    return suite;  
            case 26:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testPopulateDeltaComplexCpuTest(); }
                        }
                    );
                    return suite;  
            case 27:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testPopulateLargeDeltaCpuTest(); }
                        }
                    );
                    return suite;  
            
            case 28:  
                    suite.addTest(
                        new MamdaBookPublisherTests() 
                        {
                            protected void runTest() { testPopulateDeltaGrowOnceCpuTest(); }
                        }
                    );
                    return suite;  
            default:
                    return  new TestSuite(MamdaBookPublisherTests.class);            
        }   
    }

    public void testDisablePublishing()
    {          
       String testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t\t                  **** testDisablePublishing failed ****                         "+
               "\n This test calls populateRecap and populateDelta when generateDeltaMsgs = false.      "+
               "\n It checks no fields are returned from the populated msg when calling populateRecap(),"+
               "\n i.e. msg not populated, and that false is returned when calling populateDelta().     "+
               "\n ********************************************************************************** \n" ); 

        myImpl.myBook.generateDeltaMsgs(true);
        myImpl.myBook.populateRecap(myImpl.myPublishMsg);

        if(finest)
        {
            System.out.print("\n Ran testDisablePublishing \n");
            System.out.print("\n After populateRecap: \n");
            dumpinfo();
        }

        myImpl.myPublishMsg.clear();
        myImpl.myBook.populateDelta(myImpl.myPublishMsg);

        if(finest)
        {
            System.out.print("\n After populateDelta: \n");
            dumpinfo();
        }

        assertEquals(testdescription,0,(myImpl.myPublishMsg.getNumFields()));
        myImpl.myPublishMsg.clear();
        boolean populated = myImpl.myBook.populateDelta(myImpl.myPublishMsg);
        assertTrue(testdescription,!populated);       
    }

    public void testAddEntry()
    {
        String testdescription = new String(
                    "\n **********************************************************************************   " +
                    "\n \t\t                    **** testAddEntry failed ****                                " +
                    "\n This test adds an entry to the book using findOrCreateLevel() and level->addEntry(). " +
                    "\n It then calls populateRecap() and passes the message to an OrderBookListener. The    " +
                    "\n test then compares the level and entry info of the listener book against the         " +
                    "\n original book, specifically the entryId, NumEntries, NumLevels, NumBidLevels,        " +
                    "\n NumAskLevels, level size, level price and level side.                                " +
                    "\n ********************************************************************************** \n");

        myCreateAndAddEntry (myImpl, 100.0, "jmg", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);

        myImpl.myBook.populateRecap(myImpl.myPublishMsg);     

        if(finest)
        {
            System.out.print("\n Ran testAddEntry \n");
            System.out.print("\n After populateRecap: \n");
            dumpinfo();
        }

        myImpl.myTicker.callMamdaOnMsg(myImpl.mySubscription, myImpl.myPublishMsg);  
   
        if(finest)
        {
            System.out.print("\n After callMamdaOnMsg: \n");
            dumpinfo();
        }

        ownBkLevel = myImpl.myBook.findOrCreateLevel(100.0, 
                        MamdaOrderBookPriceLevel.SIDE_BID);
                        
        listBkLevel = myImpl.myBookListener.getOrderBook().findOrCreateLevel(100.0, 
                        MamdaOrderBookPriceLevel.SIDE_BID);  
        
        assertEquals (testdescription,listBkLevel.findOrCreateEntry("jmg").getId(),
                    (ownBkLevel.findOrCreateEntry("jmg")).getId());
        assertEquals(testdescription,listBkLevel.getNumEntries(),
                ownBkLevel.getNumEntries(),0);
        assertEquals(testdescription,listBkLevel.getNumEntriesTotal(),
                  ownBkLevel.getNumEntriesTotal());
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getTotalNumLevels(),
                myImpl.myBook.getTotalNumLevels());
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getNumBidLevels(),
                myImpl.myBook.getNumBidLevels()); 
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getNumAskLevels(),
                myImpl.myBook.getNumAskLevels()); 
        assertEquals(testdescription,listBkLevel.getSize(),
                     ownBkLevel.getSize(),0);
        assertEquals(testdescription,listBkLevel.getPrice().getValue(),
                ownBkLevel.getPrice().getValue(),0);
        assertEquals(testdescription,listBkLevel.getSide(),
                ownBkLevel.getSide());

    }

    public void testAddEntryAtLevel()
    {
        String testdescription = new String(
                "\n **********************************************************************************  "+
               "\n \t\t               **** testAddEntryAtLevel failed ****                              "+
               "\n This test creates a new level using book.findOrCreateLevel, a new entry, then calls  "+
               "\n level.addEntry(). It then calls populateRecap() and passes the message to an         "+ 
               "\n OrderBookListener. The test then compares the level and entry info of the listener   "+
               "\n book against the original book, specifically the entryId, NumEntries, NumLevels,     "+
               "\n NumBidLevels, NumAskLevels, level size, level price and level side.                  "+
               "\n ********************************************************************************** \n");

        myCreateAndAddEntryAtLevel (myImpl, 100.0, "jmg", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);

        myImpl.myBook.populateRecap(myImpl.myPublishMsg);

        if(finest)
        {
            System.out.print("\n Ran testAddEntryAtLevel \n");
            System.out.print("\n After populateRecap: \n");
            dumpinfo();
        }

        myImpl.myTicker.callMamdaOnMsg(myImpl.mySubscription, myImpl.myPublishMsg);  

        if(finest)
        {
            System.out.print("\n After callMamdaOnMsg: \n");
            dumpinfo();
        }

        ownBkLevel = myImpl.myBook.findOrCreateLevel(100.0, 
                        MamdaOrderBookPriceLevel.SIDE_BID);
                        
        listBkLevel = myImpl.myBookListener.getOrderBook().findOrCreateLevel(100.0, 
                        MamdaOrderBookPriceLevel.SIDE_BID);
        
        assertEquals (testdescription,listBkLevel.findOrCreateEntry("jmg").getId(),
                    (ownBkLevel.findOrCreateEntry("jmg")).getId());
        assertEquals(testdescription,listBkLevel.getNumEntries(),
                ownBkLevel.getNumEntries(),0);
        assertEquals(testdescription,listBkLevel.getNumEntriesTotal(),
                ownBkLevel.getNumEntriesTotal());
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getTotalNumLevels(),
                myImpl.myBook.getTotalNumLevels());
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getNumBidLevels(),
                myImpl.myBook.getNumBidLevels()); 
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getNumAskLevels(),
                myImpl.myBook.getNumAskLevels()); 
        assertEquals(testdescription,listBkLevel.getSize(),
               ownBkLevel.getSize(),0);
        assertEquals(testdescription,listBkLevel.getPrice().getValue(),
                  ownBkLevel.getPrice().getValue(),0);
        assertEquals(testdescription,listBkLevel.getSide(),
                ownBkLevel.getSide());
    }

    public void testAddLevel()
    {
        String testdescription = new String(
               "\n **********************************************************************************   "+
            "\n \t\t                          **** testAddLevel failed ****                             "+
               "\n This test creates a new level and the adds it to the book using book.addLevel()      "+
               "\n It then calls populateRecap() and passes the message to an OrderBookListener.        "+ 
               "\n The test then compares the level and entry info of the listener book against the     "+
               "\n original book, specifically the NumEntries, NumLevels, NumBidLevels, NumAskLevels,   "+
               "\n level size, level price and level side.                                              "+
               "\n ********************************************************************************** \n" );

        MamdaOrderBookPriceLevel level1= new MamdaOrderBookPriceLevel (); 
        myAddLevel (myImpl, 100.0, 1000, 1000, 
                MamdaOrderBookPriceLevel.ACTION_ADD,
                MamdaOrderBookPriceLevel.SIDE_BID,
                level1);
        myImpl.myBook.populateRecap(myImpl.myPublishMsg);

        if(finest)
        {
            System.out.print("\n Ran testAddLevel \n");
            System.out.print("\n After populateRecap: \n");
            dumpinfo();
        }

        myImpl.myTicker.callMamdaOnMsg(myImpl.mySubscription, myImpl.myPublishMsg);  

        if(finest)
        {
            System.out.print("\n After callMamdaOnMsg: \n");
            dumpinfo();
        }

        ownBkLevel = myImpl.myBook.findOrCreateLevel(100.0, 
                        MamdaOrderBookPriceLevel.SIDE_BID);
                        
        listBkLevel = myImpl.myBookListener.getOrderBook().findOrCreateLevel(100.0, 
                        MamdaOrderBookPriceLevel.SIDE_BID);
        
        assertEquals(testdescription,listBkLevel.getNumEntries(),
                ownBkLevel.getNumEntries(),0);
        assertEquals(testdescription,listBkLevel.getNumEntriesTotal(),
                ownBkLevel.getNumEntriesTotal());
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getTotalNumLevels(),
                myImpl.myBook.getTotalNumLevels());
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getNumBidLevels(),
                myImpl.myBook.getNumBidLevels()); 
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getNumAskLevels(),
                myImpl.myBook.getNumAskLevels()); 
        assertEquals(testdescription,listBkLevel.getSize(),
               ownBkLevel.getSize(),0);
        assertEquals(testdescription,listBkLevel.getPrice().getValue(),
               ownBkLevel.getPrice().getValue(),0);
        assertEquals(testdescription,listBkLevel.getSide(),
                ownBkLevel.getSide());
    }

    public void testFindOrCreateLevel()
    {    
        String testdescription = new String(
                "\n **********************************************************************************  "+
               "\n \t\t                  **** testFindOrCreateLevel failed ****                         "+
               "\n This test creates a new level and adds it to the book using book.findOrCreateLevel() "+
               "\n It then calls populateRecap() and passes the message to an OrderBookListener.        "+ 
               "\n The test then compares the level and entry info of the listener book against the     "+
               "\n original book, specifically the NumEntries, NumLevels, NumBidLevels, NumAskLevels,   "+
               "\n level size, level price and level side.                                              "+
               "\n ********************************************************************************** \n");     

        MamdaOrderBookPriceLevel level = null;
        
        level = myFindOrCreateLevel (myImpl, 100.0, 1000, 1000,
                                    MamdaOrderBookPriceLevel.ACTION_ADD, 
                                    MamdaOrderBookPriceLevel.SIDE_BID);

        myImpl.myBook.populateRecap(myImpl.myPublishMsg);
 
        if(finest)
        {
            System.out.print("\n Ran testFindOrCreateLevel \n");
            System.out.print("\n After populateRecap: \n");
            dumpinfo();
        }

        myImpl.myTicker.callMamdaOnMsg(myImpl.mySubscription, myImpl.myPublishMsg);  

        if(finest)
        {
            System.out.print("\n After callMamdaOnMsg: \n");
            dumpinfo();
        }

        ownBkLevel = myImpl.myBook.findOrCreateLevel(100.0, 
                        MamdaOrderBookPriceLevel.SIDE_BID);
                        
        listBkLevel = myImpl.myBookListener.getOrderBook().findOrCreateLevel(100.0, 
                        MamdaOrderBookPriceLevel.SIDE_BID);        

        assertEquals(testdescription,listBkLevel.getNumEntries(),
                ownBkLevel.getNumEntries(),0);
        assertEquals(testdescription,listBkLevel.getNumEntriesTotal(),
               ownBkLevel.getNumEntriesTotal());
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getTotalNumLevels(),
                myImpl.myBook.getTotalNumLevels());
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getNumBidLevels(),
                myImpl.myBook.getNumBidLevels()); 
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getNumAskLevels(),
               myImpl.myBook.getNumAskLevels()); 
        assertEquals(testdescription,listBkLevel.getSize(),
                ownBkLevel.getSize(),0);
        assertEquals(testdescription,listBkLevel.getPrice().getValue(),
                ownBkLevel.getPrice().getValue(),0);
        assertEquals(testdescription,listBkLevel.getSide(),
                ownBkLevel.getSide());
    }

    public void testFindOrCreateEntry()
    {
        String testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t\t                   **** testFindOrCreateEntry failed ****                        "+
               "\n This test creates a new level, findOrCreateLevel(), and new entry findOrCreateEntry()"+
               "\n It then calls populateRecap() and passes the message to an OrderBookListener.        "+ 
               "\n The test then compares the level and entry info of the listener book against the     "+
               "\n original book, specifically the NumEntries, NumLevels, NumBidLevels, NumAskLevels,   "+
               "\n level size, level price and level side.                                              "+
               "\n ********************************************************************************** \n");        

        MamdaOrderBookPriceLevel level = null;    

        level = myFindOrCreateEntry (myImpl, 100.0, 1000, 1000,1.0,
                                    MamdaOrderBookEntry.ACTION_ADD, 
                                    MamdaOrderBookPriceLevel.SIDE_BID);

        myImpl.myBook.populateRecap(myImpl.myPublishMsg);

        if(finest)
        {
            System.out.print("\n Ran testFindOrCreateEntry \n");
            System.out.print("\n After populateRecap: \n");
            dumpinfo();
        }

        myImpl.myTicker.callMamdaOnMsg(myImpl.mySubscription, myImpl.myPublishMsg);  

        if(finest)
        {
            System.out.print("\n After callMamdaOnMsg: \n");
            dumpinfo();
        }

        ownBkLevel = myImpl.myBook.findOrCreateLevel(100.0, 
                        MamdaOrderBookPriceLevel.SIDE_BID);
                        
        listBkLevel = myImpl.myBookListener.getOrderBook().findOrCreateLevel(100.0, 
                        MamdaOrderBookPriceLevel.SIDE_BID); 
        
        assertEquals(testdescription,listBkLevel.getNumEntries(),
                ownBkLevel.getNumEntries(),0);
        assertEquals(testdescription,listBkLevel.getNumEntriesTotal(),
                ownBkLevel.getNumEntriesTotal());
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getTotalNumLevels(),
                myImpl.myBook.getTotalNumLevels());
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getNumBidLevels(),
                myImpl.myBook.getNumBidLevels()); 
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getNumAskLevels(),
                myImpl.myBook.getNumAskLevels()); 
        assertEquals(testdescription,listBkLevel.getSize(),
                ownBkLevel.getSize(),0);
        assertEquals(testdescription,listBkLevel.getPrice().getValue(),
                ownBkLevel.getPrice().getValue(),0);
        assertEquals(testdescription,listBkLevel.getSide(),
                ownBkLevel.getSide());
    }

    public void testUpdateEntryAtLevel()
    {
        String testdescription = new String(
                "\n **********************************************************************************  "+
               "\n \t\t                 **** testUpdateEntryAtLevel failed ****                         "+
               "\n This test creates a new level, findOrCreateLevel(), and new entry findOrCreateEntry()"+
               "\n and then updates the created entry, using level.updateEntry(), a number of times.    "+
               "\n It then calls populateRecap() and passes the message to an OrderBookListener.        "+ 
               "\n The test then compares the level and entry info of the listener book against the     "+
               "\n original book, specifically the NumEntries, NumLevels, NumBidLevels, NumAskLevels,   "+
               "\n level size, level price and level side.                                              "+
               "\n ********************************************************************************** \n");

        myCreateAndAddEntry (myImpl, 100.0, "jmg", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
                                    
        myUpdateEntryAtLevel (myImpl, "jmg", 100.0, 1000.0, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntryAtLevel (myImpl, "jmg", 100.0, 2000.0, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntryAtLevel (myImpl, "jmg", 100.0, 3000.0, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntryAtLevel (myImpl, "jmg", 100.0, 4000.0, MamdaOrderBookPriceLevel.SIDE_BID);

        myImpl.myBook.populateRecap(myImpl.myPublishMsg);

        if(finest)
        {
            System.out.print("\n Ran testUpdateEntryAtLevel \n");
            System.out.print("\n After populateRecap: \n");
            dumpinfo();
        }

        myImpl.myTicker.callMamdaOnMsg(myImpl.mySubscription, myImpl.myPublishMsg);  

        if(finest)
        {
            System.out.print("\n After callMamdaOnMsg: \n");
            dumpinfo();
        }

        ownBkLevel = myImpl.myBook.findOrCreateLevel(100.0, 
                        MamdaOrderBookPriceLevel.SIDE_BID);
                        
        listBkLevel = myImpl.myBookListener.getOrderBook().findOrCreateLevel(100.0, 
                        MamdaOrderBookPriceLevel.SIDE_BID);
        
        assertEquals(testdescription,listBkLevel.getNumEntries(),
                ownBkLevel.getNumEntries(),0);
        assertEquals(testdescription,listBkLevel.getNumEntriesTotal(),
                ownBkLevel.getNumEntriesTotal());
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getTotalNumLevels(),
                myImpl.myBook.getTotalNumLevels());
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getNumBidLevels(),
                myImpl.myBook.getNumBidLevels()); 
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getNumAskLevels(),
                   myImpl.myBook.getNumAskLevels()); 
        assertEquals(testdescription,listBkLevel.getSize(),
                ownBkLevel.getSize(),0);
        assertEquals(testdescription,listBkLevel.getPrice().getValue(),
                ownBkLevel.getPrice().getValue(),0);
        assertEquals(testdescription,listBkLevel.getSide(),
                ownBkLevel.getSide());
    }

    public void testUpdateEntry()
    {
        String testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t\t                 **** testUpdateEntry failed ****                                "+
               "\n This test creates a new level, findOrCreateLevel(), and new entry findOrCreateEntry()"+
               "\n and then updates the created entry, using book.updateEntry(), a number of times.     "+
               "\n It then calls populateRecap() and passes the message to an OrderBookListener.        "+ 
               "\n The test then compares the level and entry info of the listener book against the     "+
               "\n original book, specifically the entryId, NumEntries, NumLevels, NumBidLevels,        "+
               "\n NumAskLevels, level size, level price and level side.                                "+
               "\n ********************************************************************************** \n");

        myCreateAndAddEntry (myImpl, 100.0, "jmg", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
                                    
        myUpdateEntry (myImpl, "jmg", 100.0, 1000.0, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntry (myImpl, "jmg", 100.0, 2000.0, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntry (myImpl, "jmg", 100.0, 3000.0, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntry (myImpl, "jmg", 100.0, 4000.0, MamdaOrderBookPriceLevel.SIDE_BID);

        myImpl.myBook.populateRecap(myImpl.myPublishMsg);

        if(finest)
        {
            System.out.print("\n Ran testUpdateEntry \n");
            System.out.print("\n After populateRecap: \n");
            dumpinfo();
        }

        myImpl.myTicker.callMamdaOnMsg(myImpl.mySubscription, myImpl.myPublishMsg);  

        if(finest)
        {
            System.out.print("\n After callMamdaOnMsg: \n");
            dumpinfo();
        }

        ownBkLevel = myImpl.myBook.findOrCreateLevel(100.0, 
                        MamdaOrderBookPriceLevel.SIDE_BID);
                        
        listBkLevel = myImpl.myBookListener.getOrderBook().findOrCreateLevel(100.0, 
                        MamdaOrderBookPriceLevel.SIDE_BID);
            
        assertEquals(testdescription,listBkLevel.getNumEntries(),
                ownBkLevel.getNumEntries(),0);
        assertEquals(testdescription,listBkLevel.getNumEntriesTotal(),
                ownBkLevel.getNumEntriesTotal());
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getTotalNumLevels(),
                myImpl.myBook.getTotalNumLevels());
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getNumBidLevels(),
                myImpl.myBook.getNumBidLevels()); 
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getNumAskLevels(),
               myImpl.myBook.getNumAskLevels()); 
        assertEquals(testdescription,listBkLevel.getSize(),
                ownBkLevel.getSize(),0);
        assertEquals(testdescription,listBkLevel.getPrice().getValue(),
                ownBkLevel.getPrice().getValue(),0);
        assertEquals(testdescription,listBkLevel.getSide(),
                ownBkLevel.getSide());
    }

    public void testUpdateLevel ()
    {
        String testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t\t                  **** testUpdateLevel failed ****                               "+
               "\n This test creates a new level, findOrCreateLevel(), and then updates the level, using"+
               "\n book.updateEntry().                                                                  "+
               "\n It then calls populateRecap() and passes the message to an OrderBookListener.        "+
               "\n The test then compares the level and entry info of the listener book against the     "+
               "\n original book, specifically the NumEntries, NumLevels, NumBidLevels, NumAskLevels,   "+
               "\n level size, level price and level side.                                              "+
               "\n ********************************************************************************** \n");

        MamdaOrderBookPriceLevel level = null;
        
        level = myFindOrCreateLevel (myImpl, 100.0, 1000, 1000,
                                    MamdaOrderBookPriceLevel.ACTION_ADD, 
                                    MamdaOrderBookPriceLevel.SIDE_BID);
            
        myUpdateLevel (myImpl, 100.0, 2000, 2000, 
                    MamdaOrderBookPriceLevel.ACTION_UPDATE,
                    MamdaOrderBookPriceLevel.SIDE_BID);

        myImpl.myBook.populateRecap(myImpl.myPublishMsg);

        if(finest)
        {
            System.out.print("\n Ran testUpdateLevel \n");
            System.out.print("\n After populateRecap: \n");
            dumpinfo();
        }

        myImpl.myTicker.callMamdaOnMsg(myImpl.mySubscription, myImpl.myPublishMsg);  

        if(finest)
        {
            System.out.print("\n After callMamdaOnMsg: \n");
            dumpinfo();
        }

        ownBkLevel = myImpl.myBook.findOrCreateLevel(100.0, 
                        MamdaOrderBookPriceLevel.SIDE_BID);
                        
        listBkLevel = myImpl.myBookListener.getOrderBook().findOrCreateLevel(100.0, 
                        MamdaOrderBookPriceLevel.SIDE_BID); 
        
        assertEquals(testdescription,listBkLevel.getNumEntries(),
                ownBkLevel.getNumEntries(),0);
        assertEquals(testdescription,listBkLevel.getNumEntriesTotal(),
                ownBkLevel.getNumEntriesTotal());
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getTotalNumLevels(),
                myImpl.myBook.getTotalNumLevels());
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getNumBidLevels(),
               myImpl.myBook.getNumBidLevels()); 
        assertEquals(testdescription,myImpl.myBookListener.getOrderBook().getNumAskLevels(),
                myImpl.myBook.getNumAskLevels()); 
        assertEquals(testdescription,listBkLevel.getSize(),
                ownBkLevel.getSize(),0);
        assertEquals(testdescription,listBkLevel.getPrice().getValue(),
                ownBkLevel.getPrice().getValue(),0);
        assertEquals(testdescription,listBkLevel.getSide(),
                 ownBkLevel.getSide());     
    }

    public void testOneLevelTwoEntries()
    {
        String testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t\t      **** testOneLevelTwoEntries failed ****                                    "+
               "\n This test creates a new level, addLevel(), and then creates and adds two new entries "+
               "\n using addEntry().                                                                    "+
               "\n It then calls populateRecap() and passes the message to an OrderBookListener.        "+ 
               "\n The test then uses assertEqual to check that the publishing book is the same as the  "+
               "\n listener book."+
               "\n ********************************************************************************** \n");

        MamdaOrderBookPriceLevel level1 = new MamdaOrderBookPriceLevel();

        myAddLevel (myImpl, 100.0, 0, 0, 
                    MamdaOrderBookPriceLevel.ACTION_ADD,
                    MamdaOrderBookPriceLevel.SIDE_BID,
                    level1);
                    
        myCreateAndAddEntry (myImpl, 100.0, "ent1", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
                        
        myCreateAndAddEntry (myImpl, 100.0, "ent2", 500, 82, 
                            MamdaOrderBookEntry.ACTION_ADD,
                            MamdaOrderBookPriceLevel.SIDE_BID);    

        populateRecapAndValidate(myImpl,testdescription);

        if(finest)
        {
            System.out.print("\n Ran testOneLevelTwoEntries \n");
            System.out.print("\n After populateRecapAndValidate: \n");
            dumpinfo();
        }
    }

    public void testFlattenEntryInfo()
    {
        String testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t\t      **** testFlattenEntryInfo failed ****                                      "+
               "\n This test checks whether entry info is correctly flattened into the price level      "+
               "\n It calls populateRecap() and passes the message to an OrderBookListener.             "+
               "\n The test then uses assertEqual to check that the publishing book is the same as the  "+
               "\n listener book. It also checks the recap contains a single priceLevel msg and no      "+
               "\n entryMsg  vector. It then adds two entries to a level, calls populateDelta() and does"+
               "\n the assert check again. The test then checks the returned msg for a PlEntry msg and  "+
               "\n fails if one exists, as this should be flattened into plMsg. It also checks for      "+
               "\n vector of price level msgs and verfies there should be two.                          "+
               "\n ********************************************************************************** \n"); 

        MamdaOrderBookPriceLevel level1 = new MamdaOrderBookPriceLevel();

        myCreateAndAddEntry (myImpl, 101.0, "ent", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);                
                  
        populateRecapAndValidate(myImpl,testdescription);

        if(finest)
        {
            System.out.print("\n Ran testFlattenEntryInfo \n");
            System.out.print("\n After populateRecapAndValidate: \n");
            dumpinfo();
        }

        MamaArrayMsg result = new MamaArrayMsg();       

        int numLevels = 0;              
        boolean hasPriceLevels = myImpl.myPublishMsg.tryArrayMsg(null, 699, result);  
        MamaMsg[] plVector = result.getValue(); 
        assertTrue(hasPriceLevels);
        numLevels =plVector.length;
        assertEquals(1, numLevels);
 
        int numEntries = 0;
        boolean hasEntryVector = myImpl.myPublishMsg.tryArrayMsg(null,681,result);       
        MamaMsg[] entryVector = result.getValue();     

        assertTrue(!hasEntryVector);   

        myCreateAndAddEntry (myImpl, 102.0, "ent1", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
                        
        myCreateAndAddEntry (myImpl, 103.0, "ent2", 500, 82, 
                            MamdaOrderBookEntry.ACTION_ADD,
                            MamdaOrderBookPriceLevel.SIDE_BID);        
        
        addMamaHeaderFields(myImpl,
                            myImpl.myPublishMsg,
                            MamaMsgType.TYPE_BOOK_UPDATE,  
                            MamaMsgStatus.STATUS_OK,  
                            (int)1);
                            
        populateDeltaAndValidate(myImpl,testdescription);

        if(finest)
        {
            System.out.print("\n Ran testFlattenEntryInfo \n");
            System.out.print("\n After populateDeltaAndValidate: \n");
            dumpinfo();
        }

        numLevels = 0;
        hasPriceLevels = false;
        hasPriceLevels = myImpl.myPublishMsg.tryArrayMsg(null,699,result);  
        plVector = result.getValue();
        assertTrue(hasPriceLevels);       
        numLevels = plVector.length;
        assertEquals(2,numLevels);

        numEntries = 0;
        hasEntryVector = false;
        hasEntryVector= myImpl.myPublishMsg.tryArrayMsg(null,681,result);
        assertTrue(!hasEntryVector);        
    }

    public void testFlattenTwoEntriesOneLevel()
    {
        String testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t\t      **** testFlattenTwoEntriesOneLevel failed ****                             "+
               "\n This test checks whether two entries are correctly flattened into one price level    "+
               "\n during populateDelta."+
               "\n It firstly calls populateRecap() and passes the message to an OrderBookListener.     "+ 
               "\n The test then uses assertEqual to check that the publishing book is the same as the  "+
               "\n listener book. It then adds two entries to a level, calls populateDelta() and does   "+
               "\n the assert check again. The test then checks the returned msg for a PlEntry msg and  "+
               "\n fails if one doesn't exist, also checking the numEntries in the plEntry Msg = 2.     "+
               "\n ********************************************************************************** \n");

        MamdaOrderBookPriceLevel level1 = new MamdaOrderBookPriceLevel();

        myCreateAndAddEntry (myImpl, 102.0, "ent ", 500, 82,  
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);     

        populateRecapAndValidate(myImpl,testdescription);    

        if(finest)
        {
            System.out.print("\n Ran testFlattenTwoEntriesOneLevel \n");
            System.out.print("\n After populateRecapAndValidate: \n");
            dumpinfo();
        }

        myCreateAndAddEntry (myImpl, 102.0, "ent1", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
                        
        myCreateAndAddEntry (myImpl, 102.0, "ent2 ", 500, 82, 
                            MamdaOrderBookEntry.ACTION_ADD,
                            MamdaOrderBookPriceLevel.SIDE_BID);
        addMamaHeaderFields(myImpl,
                            myImpl.myPublishMsg,
                            MamaMsgType.TYPE_BOOK_UPDATE,  
                            MamaMsgStatus.STATUS_OK, 
                            (int)1);

        populateDeltaAndValidate(myImpl,testdescription);

        if(finest)
        {
            System.out.print("\n After populateDeltaAndValidate: \n");
            dumpinfo();
        }
      
        MamaArrayMsg result = new MamaArrayMsg();
       
        int numEntries = 0;
        boolean hasEntryVector;  
        hasEntryVector = myImpl.myPublishMsg.tryArrayMsg(null,700,result);
        assertTrue(hasEntryVector); 
        
        MamaMsg[] msgVector = result.getValue();

        numEntries = msgVector.length;
        
        assertEquals(2, numEntries);
    }

    public void testFlattenOneEntry()
    {
        String testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t\t      **** testFlattenOneEntry failed ****                                       "+
               "\n This test checks whether entry and pricelevel info are correctly flattened into the  "+
               "\n top level msg duirng populateDelta.                                                  "+
               "\n It calls populateRecap() and passes the message to an OrderBookListener.             "+ 
               "\n The test then uses assertEqual to check that the publishing book is the same as the  "+
               "\n listener book. It then adds one entry to a level, calls populateDelta() and does     "+
               "\n the assert check again. The test then checks the returned msg for PlEntry and        "+
               "\n priceLevel messages and fails if either exist.                                       "+
               "\n ********************************************************************************** \n");

        MamdaOrderBookPriceLevel level1 = new MamdaOrderBookPriceLevel();

        myCreateAndAddEntry (myImpl, 101.0, "ent", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);

        populateRecapAndValidate(myImpl,testdescription);

        if(finest)
        {
            System.out.print("\n Ran testFlattenOneEntry \n");
            System.out.print("\n After populateRecapAndValidate: \n");
            dumpinfo();
        }
        
        myCreateAndAddEntry (myImpl, 102.0, "ent1", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);       

        addMamaHeaderFields(myImpl,
                            myImpl.myPublishMsg,                    
                            MamaMsgType.TYPE_BOOK_UPDATE,      
                            MamaMsgStatus.STATUS_OK,  
                            (int)1);
        populateDeltaAndValidate(myImpl,testdescription);

        if(finest)
        {
            System.out.print("\n After populateDeltaAndValidate: \n");
            dumpinfo();
        }
       
        boolean hasEntryVector;  
        hasEntryVector = myImpl.myPublishMsg.tryArrayMsg(null,700,null);        
        assertTrue(!hasEntryVector);             

        boolean hasPriceLevels = myImpl.myPublishMsg.tryArrayMsg(null,699,null);  
        assertTrue(!hasPriceLevels);  
    }

    public void testRemoveEntryFromLevel()
    {     
        String testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t\t      **** testRemoveEntryFromLevel failed ****                                  "+
               "\n This test checks checks publishing functions correctly when removing entries.        "+
               "\n It removes an entry from a level, level.removeEntry(), calls populateDelta(), passes "+
               "\n returned msg to listener does a book assert check.                                   "+
               "\n ********************************************************************************** \n");

        MamdaOrderBookPriceLevel level1 = new MamdaOrderBookPriceLevel();

        myAddLevel (myImpl, 100.0, 0, 0, 
                    MamdaOrderBookPriceLevel.ACTION_ADD,
                    MamdaOrderBookPriceLevel.SIDE_BID,
                    level1);
                    
        myCreateAndAddEntry (myImpl, 100.0, "ent1", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
                        
        myCreateAndAddEntry (myImpl, 100.0, "ent2", 500, 82, 
                            MamdaOrderBookEntry.ACTION_ADD,
                            MamdaOrderBookPriceLevel.SIDE_BID);

        populateRecapAndValidate(myImpl,testdescription);  

        if(finest)
        {
            System.out.print("\n Ran testRemoveEntryFromLevel \n");
            System.out.print("\n After populateRecapAndValidate: \n");
            dumpinfo();
        }
              
        myRemoveEntry(myImpl, 100.0, "ent2", 500, 82, 
                            MamdaOrderBookEntry.ACTION_DELETE,
                            MamdaOrderBookPriceLevel.SIDE_BID);   

        addMamaHeaderFields(myImpl,
                            myImpl.myPublishMsg,
                            MamaMsgType.TYPE_BOOK_UPDATE,  
                            MamaMsgStatus.STATUS_OK,  
                            (int)1);
        populateDeltaAndValidate(myImpl,testdescription);

        if(finest)
        {
            System.out.print("\n After populateDeltaAndValidate: \n");
            dumpinfo();
        }
    }

    public void testRemoveEntryByIdFromLevel()
    {
        String testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t\t      **** testRemoveEntryByIdFromLevel failed ****                              "+
               "\n This test checks checks publishing functions correctly when removing entries.        "+
               "\n It removes an entry from a level, level.removeEntryById(), calls populateDelta(),    "+
               "\n passes returned msg to listener does a book assert check.                            "+
               "\n ********************************************************************************** \n");

        MamdaOrderBookPriceLevel level1 = new MamdaOrderBookPriceLevel();  

        myAddLevel (myImpl, 100.0, 0, 0, 
                    MamdaOrderBookPriceLevel.ACTION_ADD,
                    MamdaOrderBookPriceLevel.SIDE_BID,
                    level1);
                    
        myCreateAndAddEntry (myImpl, 100.0, "ent1", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
                        
        myCreateAndAddEntry (myImpl, 100.0, "ent2", 500, 82, 
                            MamdaOrderBookEntry.ACTION_ADD,
                            MamdaOrderBookPriceLevel.SIDE_BID);        

        populateRecapAndValidate(myImpl,testdescription); 

        if(finest)
        {
            System.out.print("\n Ran testRemoveEntryByIdFromLevel \n");
            System.out.print("\n After populateRecapAndValidate: \n");
            dumpinfo();
        }
        
        myRemoveEntryById(myImpl, 100.0, "ent2", 500, 82, 
                            MamdaOrderBookEntry.ACTION_DELETE,
                            MamdaOrderBookPriceLevel.SIDE_BID);
        
        addMamaHeaderFields(myImpl,
                            myImpl.myPublishMsg,
                            MamaMsgType.TYPE_BOOK_UPDATE,  
                            MamaMsgStatus.STATUS_OK,   
                            (int)1);

        populateDeltaAndValidate(myImpl,testdescription);

        if(finest)
        {
            System.out.print("\n After populateDeltaAndValidate: \n");
            dumpinfo();
        }
    }

    public void testDeleteEntryFromLevel()
    {
        String testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t\t      **** testDeleteEntryFromLevel failed ****                                  "+
               "\n This test checks checks publishing functions correctly when deleting entries.        "+
               "\n It removes an entry from a level, level.deleteEntry(), calls populateDelta(),        "+
               "\n passes returned msg to listener does a book assert check.                            "+
               "\n ********************************************************************************** \n");

        MamdaOrderBookPriceLevel level1 = new MamdaOrderBookPriceLevel();     

        myAddLevel (myImpl, 100.0, 0, 0, 
                    MamdaOrderBookPriceLevel.ACTION_ADD,
                    MamdaOrderBookPriceLevel.SIDE_BID,
                    level1);             

        myCreateAndAddEntry (myImpl, 100.0, "ent1", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);       

        myCreateAndAddEntry (myImpl, 100.0, "ent2", 500, 82, 
                            MamdaOrderBookEntry.ACTION_ADD,
                            MamdaOrderBookPriceLevel.SIDE_BID);    

        populateRecapAndValidate(myImpl,testdescription); 

        if(finest)
        {
            System.out.print("\n Ran testDeleteEntryFromLevel \n");
            System.out.print("\n After populateRecapAndValidate: \n");
            dumpinfo();
        }

        myDeleteEntry(myImpl, 100.0, "ent2", 500, 82, 
                            MamdaOrderBookEntry.ACTION_DELETE,
                            MamdaOrderBookPriceLevel.SIDE_BID);    
        
        addMamaHeaderFields(myImpl,
                            myImpl.myPublishMsg,
                            MamaMsgType.TYPE_BOOK_UPDATE,  
                            MamaMsgStatus.STATUS_OK,    
                            (int)1); 

        populateDeltaAndValidate(myImpl,testdescription); 

        if(finest)
        {
            System.out.print("\n After populateDeltaAndValidate: \n");
            dumpinfo();
        }
    }

    public void testpopulateDeltaEmptyState()
    {
        String testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t\t      **** testpopulateDeltaEmptyState failed ****                               "+
               "\n This test checks checks publishing functions correctly delta list is empty.          "+
               "\n populateDelta is called when there's an empty delta list. Returned bool should be    "+
               "\n false                                                                                "+
               "\n ********************************************************************************** \n");

        MamdaOrderBookPriceLevel level1 = new MamdaOrderBookPriceLevel();     

        myAddLevel (myImpl, 100.0, 0, 0, 
                    MamdaOrderBookPriceLevel.ACTION_ADD,
                    MamdaOrderBookPriceLevel.SIDE_BID,
                    level1);             

        myCreateAndAddEntry (myImpl, 100.0, "ent1", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
                        
        myCreateAndAddEntry (myImpl, 100.0, "ent2", 500, 82, 
                            MamdaOrderBookEntry.ACTION_ADD,
                            MamdaOrderBookPriceLevel.SIDE_BID);
       
        populateRecapAndValidate(myImpl,testdescription);

        if(finest)
        {
            System.out.print("\n Ran testpopulateDeltaEmptyState \n");
            System.out.print("\n After populateRecapAndValidate: \n");
            dumpinfo();
        }

        addMamaHeaderFields(myImpl,
                            myImpl.myPublishMsg,
                            MamaMsgType.TYPE_BOOK_UPDATE,   
                            MamaMsgStatus.STATUS_OK,     
                            (int)1);        

        populateDeltaAndValidate(myImpl,testdescription);   

        if(finest)
        {
            System.out.print("\n After populateDeltaAndValidate: \n");
            dumpinfo();
        }
    }

    public void testpopulateRecapEmptyState()
    {
       String testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t\t      **** testpopulateRecapEmptyState failed ****                               "+
               "\n This test checks publishing functions correctly delta list is empty.                 "+
               "\n populateRecap is called when there's an empty delta list, and we check the populated "+
               "\n msg for the existence of entry and price levels msgs, failing if found               "+
               "\n ********************************************************************************** \n");

        populateRecapAndValidate(myImpl,null);

        if(finest)
        {
            System.out.print("\n Ran testpopulateRecapEmptyState \n");
            System.out.print("\n After populateRecapAndValidate: \n");
            dumpinfo();
        }

        MamaArrayMsg result = new MamaArrayMsg();   
        MamaMsg[] msgVector = null;

        int numLevels = 0;
        int numEntries = 0;
        boolean hasPriceLevels = myImpl.myPublishMsg.tryArrayMsg(null, 699, result);  
        msgVector = result.getValue();
        assertTrue(testdescription,!hasPriceLevels);
        boolean hasEntryVector = myImpl.myPublishMsg.tryArrayMsg(null, 700, result);  
        msgVector = result.getValue();
        assertTrue(testdescription,!hasEntryVector);
    }

     public void testMultipleLevelsAndEntries()
    {
        String testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t\t      **** testMultipleLevelsAndEntries failed ****                              "+
               "\n This test checks checks publishing functions correctly for multiple levels and       "+
               "\n entries. A number of entries are added to a book and populateRecap() is called.      "+
               "\n The populated msg is passed to a orderBookListener and we assert both books are equal"+
               "\n ********************************************************************************** \n");

        MamdaOrderBookPriceLevel level1 = new MamdaOrderBookPriceLevel();
                
        myCreateAndAddEntry (myImpl, 100.0, "ent1", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
                        
        myCreateAndAddEntry (myImpl, 100.0, "ent2", 500, 82, 
                            MamdaOrderBookEntry.ACTION_ADD,
                            MamdaOrderBookPriceLevel.SIDE_BID);
        
        MamdaOrderBookPriceLevel level2 = null;
        
        level2 = myFindOrCreateLevel (myImpl, 200.0, 2000, 2000,
                                    MamdaOrderBookPriceLevel.ACTION_ADD, 
                                    MamdaOrderBookPriceLevel.SIDE_BID);
            
        MamdaOrderBookPriceLevel level3 = null;
        
        level3 = myFindOrCreateLevel (myImpl, 300.0, 3000, 3000,
                                    MamdaOrderBookPriceLevel.ACTION_ADD, 
                                    MamdaOrderBookPriceLevel.SIDE_BID);
                                    
        MamdaOrderBookPriceLevel level4 = null;
        
        level4 = myFindOrCreateLevel (myImpl, 400.0, 4000, 4000,
                                    MamdaOrderBookPriceLevel.ACTION_ADD, 
                                    MamdaOrderBookPriceLevel.SIDE_BID);
        
        myImpl.myBook.populateRecap(myImpl.myPublishMsg);

        if(finest)
        {
            System.out.print("\n Ran testMultipleLevelsAndEntries \n");
            System.out.print("\n After populateRecap: \n");
            dumpinfo();
        }
      
        myImpl.myTicker.callMamdaOnMsg(myImpl.mySubscription, myImpl.myPublishMsg);  

        if(finest)
        {
            System.out.print("\n After callMamdaOnMsg: \n");
            dumpinfo();
        }
 
        assertEqual(myImpl.myBook, myImpl.myBookListener.getOrderBook(),testdescription);
        myImpl.myBook.clearDeltaList();

        populateRecapAndValidate(myImpl,testdescription);

        if(finest)
        {
            System.out.print("\n After populateRecapAndValidate: \n");
            dumpinfo();
        }
    }

    public void testUpdateMultipleLevels()
    {
        String testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t\t      **** testUpdateMultipleLevels failed ****                                  "+
               "\n This test checks checks publishing functions correctly for updating multiple levels  "+
               "\n A number of entries are added to a book and populateRecap() is called.      "+
               "\n The populated msg is passed to a orderBookListener and we assert both books are equal"+
               "\n We then update a no. of entries and levels and populateDelta() is called.      "+
               "\n The populated msg is passed to a orderBookListener and we assert both books are equal"+               
               "\n ********************************************************************************** \n");

        MamdaOrderBookPriceLevel level1 = new MamdaOrderBookPriceLevel();       

        myCreateAndAddEntry (myImpl, 100.0, "ent1", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);  
        myCreateAndAddEntry (myImpl, 100.0, "ent2", 500, 82, 
                            MamdaOrderBookEntry.ACTION_ADD,
                            MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntry (myImpl, "ent2", 100.0, 2000.0, MamdaOrderBookPriceLevel.SIDE_BID);   

        MamdaOrderBookPriceLevel level2;
        
        level2 = myFindOrCreateLevel (myImpl, 200.0, 2000, 2000,
                                    MamdaOrderBookPriceLevel.ACTION_ADD, 
                                    MamdaOrderBookPriceLevel.SIDE_BID);    
        MamdaOrderBookPriceLevel level3;
        
        level3 = myFindOrCreateLevel (myImpl, 300.0, 3000, 3000,
                                    MamdaOrderBookPriceLevel.ACTION_ADD, 
                                    MamdaOrderBookPriceLevel.SIDE_BID);
                           
        MamdaOrderBookPriceLevel level4;
        
        level4 = myFindOrCreateLevel (myImpl, 400.0, 4000, 4000,
                                    MamdaOrderBookPriceLevel.ACTION_ADD, 
                                    MamdaOrderBookPriceLevel.SIDE_BID);    

        populateRecapAndValidate(myImpl,testdescription);               

        if(finest)
        {
            System.out.print("\n Ran testUpdateMultipleLevels \n");
            System.out.print("\n After populateRecapAndValidate: \n");
            dumpinfo();
        }

        myUpdateEntry (myImpl, "ent2", 100.0, 2000.0, MamdaOrderBookPriceLevel.SIDE_BID);

        myUpdateLevel (myImpl, 200.0, 4000, 2000, 
                    MamdaOrderBookPriceLevel.ACTION_UPDATE,
                    MamdaOrderBookPriceLevel.SIDE_BID);  

        myUpdateLevel (myImpl, 300.0, 6000, 3000, 
                    MamdaOrderBookPriceLevel.ACTION_UPDATE,
                    MamdaOrderBookPriceLevel.SIDE_BID);
                    
        myUpdateLevel (myImpl, 400.0, 3000, -1000, 
                    MamdaOrderBookPriceLevel.ACTION_UPDATE,
                    MamdaOrderBookPriceLevel.SIDE_BID);
                    
        myUpdateLevel (myImpl, 200.0, 5000, 1000, 
                    MamdaOrderBookPriceLevel.ACTION_UPDATE,
                    MamdaOrderBookPriceLevel.SIDE_BID);
                    
        myUpdateLevel (myImpl, 300.0, 7000, 1000, 
                    MamdaOrderBookPriceLevel.ACTION_UPDATE,
                    MamdaOrderBookPriceLevel.SIDE_BID);
                    
        myUpdateLevel (myImpl, 400.0, 2000, -1000, 
                    MamdaOrderBookPriceLevel.ACTION_UPDATE,
                    MamdaOrderBookPriceLevel.SIDE_BID);

        myUpdateEntry (myImpl, "ent1", 100.0, 2000.0, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntry (myImpl, "ent2", 100.0, 3000.0, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntry (myImpl, "ent1", 100.0, 3000.0, MamdaOrderBookPriceLevel.SIDE_BID);   
        myUpdateEntry (myImpl, "ent2", 100.0, 2000.0, MamdaOrderBookPriceLevel.SIDE_BID);                    

        addMamaHeaderFields(myImpl,
                            myImpl.myPublishMsg,
                            MamaMsgType.TYPE_BOOK_UPDATE,   
                            MamaMsgStatus.STATUS_OK,  
                            (int)1);

        populateDeltaAndValidate(myImpl,testdescription);   

        if(finest)
        {
            System.out.print("\n After populateDeltaAndValidate: \n");
            dumpinfo();
        }
    }

    public void testUpdateMultipleEntries()
    {
        String testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t\t      **** testUpdateMultipleEntries failed ****                                 "+
               "\n This test checks checks publishing functions correctly for updating multiple levels  "+
               "\n A number of entries are added to a book and populateRecap() is called.      "+
               "\n The populated msg is passed to a orderBookListener and we assert both books are equal"+
               "\n We then update a no. of entries and levels and populateDelta() is called.      "+
               "\n The populated msg is passed to a orderBookListener and we assert both books are equal"+  
               "\n We then update a level and populateDelta() is called.      "+
               "\n The populated msg is passed to a orderBookListener and we assert both books are equal"+               
               "\n ********************************************************************************** \n");

        MamdaOrderBookPriceLevel level1 = new MamdaOrderBookPriceLevel();
                
        myCreateAndAddEntry (myImpl, 100.0, "ent1", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
                        
        myCreateAndAddEntry (myImpl, 100.0, "ent2", 500, 82, 
                            MamdaOrderBookEntry.ACTION_ADD,
                            MamdaOrderBookPriceLevel.SIDE_BID);
        
        MamdaOrderBookPriceLevel level2 = null;
        
        level2 = myFindOrCreateLevel (myImpl, 200.0, 2000, 2000,
                                    MamdaOrderBookPriceLevel.ACTION_ADD, 
                                    MamdaOrderBookPriceLevel.SIDE_BID);
            
        MamdaOrderBookPriceLevel level3 = null;
        
        level3 = myFindOrCreateLevel (myImpl, 300.0, 3000, 3000,
                                    MamdaOrderBookPriceLevel.ACTION_ADD, 
                                    MamdaOrderBookPriceLevel.SIDE_BID);
                                    
        MamdaOrderBookPriceLevel level4 = null;
        
        level4 = myFindOrCreateLevel (myImpl, 400.0, 4000, 4000,
                                    MamdaOrderBookPriceLevel.ACTION_ADD, 
                                    MamdaOrderBookPriceLevel.SIDE_BID);

        populateRecapAndValidate(myImpl,testdescription);
        
        if(finest)
        {
            System.out.print("\n Ran testUpdateMultipleEntries \n");
            System.out.print("\n After populateRecapAndValidate: \n");
            dumpinfo();
        }

        myUpdateEntry (myImpl, "ent1", 100.0, 1000.0, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntry (myImpl, "ent2", 100.0, 1000.0, MamdaOrderBookPriceLevel.SIDE_BID);

        myUpdateEntry (myImpl, "ent1", 100.0, 2000.0, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntry (myImpl, "ent2", 100.0, 2000.0, MamdaOrderBookPriceLevel.SIDE_BID);
        
        myUpdateLevel (myImpl, 300.0, 6000, 3000, 
                MamdaOrderBookPriceLevel.ACTION_UPDATE,
                MamdaOrderBookPriceLevel.SIDE_BID);
                    
        addMamaHeaderFields(myImpl,
                            myImpl.myPublishMsg,
                            MamaMsgType.TYPE_BOOK_UPDATE,   
                            MamaMsgStatus.STATUS_OK,                             
                            (int)1);

        populateDeltaAndValidate(myImpl,testdescription);

        if(finest)
        {
            System.out.print("\n After populateDeltaAndValidate: \n");
            dumpinfo();
        }

        myUpdateLevel (myImpl, 300.0, 7000, 1000, 
            MamdaOrderBookPriceLevel.ACTION_UPDATE,
            MamdaOrderBookPriceLevel.SIDE_BID);
                    
        addMamaHeaderFields(myImpl,
                            myImpl.myPublishMsg,
                            MamaMsgType.TYPE_BOOK_UPDATE,   
                            MamaMsgStatus.STATUS_OK,     
                            (int)2);   

        populateDeltaAndValidate(myImpl,testdescription);

        if(finest)
        {
            System.out.print("\n After populateDeltaAndValidate: \n");
            dumpinfo();
        }
    }

    public void testPopulateRecapSimpleCpuTest()
    {       
        String testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t\t      **** testPopulateRecapSimpleCpuTest failed ****                            "+
               "\n This test checks the CPU time for calling populateRecap() populating a one level, one"+
               "\n entry msg. This test CPU for populateMsg(simpleBook)                                 "+           
               "\n ********************************************************************************** \n");  

        myCreateAndAddEntry (myImpl, 100.0, "jmg", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);

        long t0 = System.nanoTime();
        myImpl.myBook.populateRecap(myImpl.myPublishMsg);
        myImpl.myPublishMsg.clear();
        long t1 = System.nanoTime();

        long Delta = (t1-t0)/1000;

        assertTrue(testdescription,Delta<900);

    }

    public void testPopulateRecapComplexCpu()
    {
        String testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t\t      **** testPopulateRecapComplexCpu failed ****                               "+
               "\n This test checks the CPU time for calling populateRecap() populating a one level, two"+
               "\n entry msg. This test CPU for populateMsg(complexBook)                                "+           
               "\n ********************************************************************************** \n");


        myCreateAndAddEntry (myImpl, 100.0, "jmg", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
        myCreateAndAddEntry (myImpl, 100.0, "jmg2", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
        long t0 = System.nanoTime();
        myImpl.myBook.populateRecap(myImpl.myPublishMsg);
        myImpl.myPublishMsg.clear();
        long t1 = System.nanoTime();

        long Delta = (t1-t0)/1000;

        assertTrue(testdescription,Delta<900);
    }    

    public void testPopulateDeltaSimpleCpuTest()
    {
        String testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t\t      **** testPopulateDeltaSimpleCpuTest failed ****                            "+
               "\n This test checks the CPU time for calling populateDelta() populating a one level, two"+
               "\n entry msg. This test CPU for populateMsg(simpleDelta)                                "+          
               "\n ********************************************************************************** \n");


        myImpl.myPublishMsg.clear();
        myCreateAndAddEntry (myImpl, 100.0, "jmg", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
        myImpl.myBook.clearDeltaList();
        long t0 = System.nanoTime();
        myUpdateEntry (myImpl, "jmg", 100.0, 500, MamdaOrderBookPriceLevel.SIDE_BID);
        myImpl.myBook.populateDelta(myImpl.myPublishMsg);
        myImpl.myPublishMsg.clear();
        long t1 = System.nanoTime();
        
        long Delta = (t1-t0)/1000;

        assertTrue(testdescription, Delta<600);
    }

    public void testPopulateDeltaComplexCpuTest()
    {
        String testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t\t      **** testPopulateDeltaComplexCpuTest failed ****                           "+
               "\n This test checks the CPU time for calling populateDelta() populating a one level, two"+
               "\n entry msg. This test CPU for populateMsg(complexDelta)                               "+          
               "\n ********************************************************************************** \n");

        myImpl.myPublishMsg.clear();
        myCreateAndAddEntry (myImpl, 100.0, "jmg", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
        myCreateAndAddEntry (myImpl, 100.0, "jmg2", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
        myImpl.myBook.clearDeltaList();
        long t0 = System.nanoTime();
        myUpdateEntry (myImpl, "jmg", 100.0, 500, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntry (myImpl, "jmg2", 100.0, 500, MamdaOrderBookPriceLevel.SIDE_BID);
        myImpl.myBook.populateDelta(myImpl.myPublishMsg);
        myImpl.myPublishMsg.clear();
        long t1 = System.nanoTime();

        long Delta = (t1-t0)/1000; 

        assertTrue(testdescription,Delta < 800);        
    }


    public void testPopulateLargeDeltaCpuTest()
    {
        String testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t\t      **** testPopulateLargeDeltaCpuTest failed ****                             "+
               "\n This test checks the CPU time for calling populateDelta() populating a one level, two"+
               "\n entry msg. This test CPU for populateMsg(complexDelta) populating very large msg     "+           
               "\n ********************************************************************************** \n");  

        myImpl.myPublishMsg.clear();
        myCreateAndAddEntry (myImpl, 100.0, "jmg", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
        myCreateAndAddEntry (myImpl, 100.0, "jmg2", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
        myCreateAndAddEntry (myImpl, 101.0, "jmg", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
        myCreateAndAddEntry (myImpl, 101.0, "jmg2", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
        myCreateAndAddEntry (myImpl, 102.0, "jmg", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
        myCreateAndAddEntry (myImpl, 102.0, "jmg2", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);                      
        myCreateAndAddEntry (myImpl, 103.0, "jmg", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
        myCreateAndAddEntry (myImpl, 103.0, "jmg2", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
        myCreateAndAddEntry (myImpl, 104.0, "jmg", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
        myCreateAndAddEntry (myImpl, 104.0, "jmg2", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);                      
        myImpl.myBook.clearDeltaList();
        long t0 = System.nanoTime();
        myUpdateEntry (myImpl, "jmg", 100.0, 500, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntry (myImpl, "jmg2", 100.0, 500, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntry (myImpl, "jmg", 101.0, 500, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntry (myImpl, "jmg2", 101.0, 500, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntry (myImpl, "jmg", 102.0, 500, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntry (myImpl, "jmg2", 102.0, 500, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntry (myImpl, "jmg", 103.0, 500, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntry (myImpl, "jmg2", 103.0, 500, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntry (myImpl, "jmg", 104.0, 500, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntry (myImpl, "jmg2", 104.0, 500, MamdaOrderBookPriceLevel.SIDE_BID);
        myImpl.myBook.populateDelta(myImpl.myPublishMsg);
        myImpl.myPublishMsg.clear();
        long t1 = System.nanoTime();

        long Delta = (t1-t0)/1000;

        assertTrue(testdescription, Delta<1000);
    }

    //Test grows vector msg holder at beginning and sucessive updates never need larger vector
    //Should be similar to typical use case 
    public void testPopulateDeltaGrowOnceCpuTest()
    {
        String testdescription = new String(
               "\n **********************************************************************************   "+
               "\n \t\t      **** testPopulateDeltaGrowOnceCpuTest failed ****                          "+
               "\n This test checks the CPU time for calling populateDelta(). The Test grows vector msg "+
               "\n holder at beginning and sucessive updates never need larger vector                   "+
               "\n This CPU test should be similar to typical use case                                  "+
               "\n ********************************************************************************** \n");

        myImpl.myPublishMsg.clear();
        myCreateAndAddEntry (myImpl, 100.0, "jmg", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
        myCreateAndAddEntry (myImpl, 100.0, "jmg2", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
        myCreateAndAddEntry (myImpl, 101.0, "jmg", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
        myCreateAndAddEntry (myImpl, 101.0, "jmg2", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
        myCreateAndAddEntry (myImpl, 102.0, "jmg", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
        myCreateAndAddEntry (myImpl, 102.0, "jmg2", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);                      
        myCreateAndAddEntry (myImpl, 103.0, "jmg", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
        myCreateAndAddEntry (myImpl, 103.0, "jmg2", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
        myCreateAndAddEntry (myImpl, 104.0, "jmg", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);
        myCreateAndAddEntry (myImpl, 104.0, "jmg2", 500, 82, 
                        MamdaOrderBookEntry.ACTION_ADD,
                        MamdaOrderBookPriceLevel.SIDE_BID);                      
        myImpl.myBook.clearDeltaList();
        long t0 = System.nanoTime();
        myUpdateEntry (myImpl, "jmg", 100.0, 500, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntry (myImpl, "jmg2", 100.0, 500, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntry (myImpl, "jmg", 101.0, 500, MamdaOrderBookPriceLevel.SIDE_BID);
        myUpdateEntry (myImpl, "jmg2", 101.0, 500, MamdaOrderBookPriceLevel.SIDE_BID);
        myImpl.myBook.populateDelta(myImpl.myPublishMsg);
        myImpl.myPublishMsg.clear();
        long t1 = System.nanoTime();

        long Delta = (t1-t0)/1000;

        assertTrue(testdescription,Delta<800);
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

    public void myCreateAndAddEntry (publisherTestImpl myImpl, double price,
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

    }

    public void myCreateAndAddEntryAtLevel (publisherTestImpl myImpl, double price,
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
