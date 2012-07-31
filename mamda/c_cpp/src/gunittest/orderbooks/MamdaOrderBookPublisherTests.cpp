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

#include <gtest/gtest.h>

#include <mamda/MamdaOrderBookFields.h>
#include <mama/mamacpp.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaOrderBookHandler.h>
#include <mamda/MamdaOrderBookListener.h>
#include <mamda/MamdaOrderBook.h>
#include <mamda/MamdaOrderBookPriceLevel.h>
#include <mamda/MamdaOrderBookEntry.h>
#include <mamda/MamdaOrderBookTypes.h>
#include <mama/MamaDictionary.h>

#include "common/CpuTestGenerator.h"
#include "common/MemoryTestGenerator.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <sstream>

using namespace Wombat;

template <class T>
inline std::string to_String (const T& t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

class BookTicker : public MamdaOrderBookHandler
{
public:
    BookTicker () {}
    virtual ~BookTicker () {}
    
    void callMamdaOnMsg(MamdaSubscription* sub, MamaMsg& msg)
    {
        try
        {
            vector<MamdaMsgListener*>& msgListeners = sub->getMsgListeners();
            unsigned long size = msgListeners.size();
            for (unsigned long i = 0; i < size; i++)
            {
                MamdaMsgListener* listener = msgListeners[i];
                listener->onMsg (sub, msg, (short)msg.getType());
            }  
        }
        catch (std::exception &e)
        {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << "Exception: Unknown" << std::endl;
        }
        return;
    }


    void onBookRecap (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookComplexDelta*  delta,
        const MamdaOrderBookRecap&         recap,
        const MamdaOrderBook&              book)
    {
    }
    
    void onBookDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookSimpleDelta&   delta,
        const MamdaOrderBook&              book)
    {
    }
    
   void onBookComplexDelta (
        MamdaSubscription*                 subscription,
        MamdaOrderBookListener&            listener,
        const MamaMsg*                     msg,
        const MamdaOrderBookComplexDelta&  delta,
        const MamdaOrderBook&              book)
    {
    }
    void onBookClear (
        MamdaSubscription*          subscription,
        MamdaOrderBookListener&     listener,
        const MamaMsg*              msg,
        const MamdaOrderBookClear&  clear,
        const MamdaOrderBook&       book)
    {
    }

    void onBookGap (
        MamdaSubscription*          subscription,
        MamdaOrderBookListener&     listener,
        const MamaMsg*              msg,
        const MamdaOrderBookGap&    event,
        const MamdaOrderBook&       book)
    {
    }
};

typedef struct publisherTestImpl
{
    MamaDictionary*             dictionary;
    MamdaOrderBook*             myBook;
    MamaMsg                     myPublishMsg;
    MamdaSubscription*          mySubscription;
    MamdaOrderBookListener*     myBookListener;
    BookTicker*                 myTicker;
    MamaDateTime                myBookTime;
};

void assertEqualEntry(MamdaOrderBookEntry* lhs, MamdaOrderBookEntry* rhs)
{
    EXPECT_STREQ (lhs->getId(), rhs->getId());
    EXPECT_EQ (lhs->getSize(), rhs->getSize());
    EXPECT_EQ (lhs->getAction(), rhs->getAction());
    EXPECT_EQ (lhs->getTime().getEpochTimeSeconds(), rhs->getTime().getEpochTimeSeconds());
}

void assertEqualLevel(MamdaOrderBookPriceLevel* lhs, MamdaOrderBookPriceLevel* rhs)
{
    EXPECT_EQ (lhs->getPrice(),rhs->getPrice());
    EXPECT_EQ (lhs->getSize(), rhs->getSize());
    EXPECT_EQ (lhs->getNumEntries(), rhs->getNumEntries());
    EXPECT_EQ (lhs->getNumEntriesTotal(), rhs->getNumEntriesTotal());
    EXPECT_EQ (lhs->getSide(), rhs->getSide());
    EXPECT_EQ (lhs->getTime().getEpochTimeSeconds(), rhs->getTime().getEpochTimeSeconds()) ;
    EXPECT_EQ (lhs->getOrderType(), rhs->getOrderType());
    
    MamdaOrderBookPriceLevel::iterator lhsEnd  = lhs->end();
    MamdaOrderBookPriceLevel::iterator lhsIter = lhs->begin();
    MamdaOrderBookPriceLevel::iterator rhsEnd  = rhs->end();
    MamdaOrderBookPriceLevel::iterator rhsIter = rhs->begin();
    
    MamdaOrderBookEntry* lhsEntry = NULL;
    MamdaOrderBookEntry* rhsEntry = NULL;
    for (; lhsIter != lhsEnd;)
    {       
        for (; rhsIter != rhsEnd;)
        {
            lhsEntry = *lhsIter;
            rhsEntry = *rhsIter;
            assertEqualEntry(lhsEntry, rhsEntry);
            //lhsEntry->assertEqual(*rhsEntry);
            ++rhsIter;
            ++lhsIter;
        }
    }   
    
}

void assertEqual(MamdaOrderBook* lhs, MamdaOrderBook* rhs)
{
//     EXPECT_STREQ (lhs.getSymbol(), rhs.getSymbol());
// 
//     EXPECT_STREQ (lhs.getPartId(), lhs.getPartId());

    if (lhs->getNumAskLevels() != 0 || rhs->getNumAskLevels() != 0)
    {
        MamdaOrderBook::constAskIterator lhsEndAsk  = lhs->askEnd();
        MamdaOrderBook::askIterator lhsIterAsk = lhs->askBegin();
        MamdaOrderBook::constAskIterator rhsEndAsk  = rhs->askEnd();
        MamdaOrderBook::askIterator rhsIterAsk = rhs->askBegin();
        MamdaOrderBookPriceLevel* lhsLevel = NULL;
        MamdaOrderBookPriceLevel* rhsLevel = NULL;
        
        for (; lhsIterAsk != lhsEndAsk;)
        {        
            for (; rhsIterAsk != rhsEndAsk;)
            {
                lhsLevel = *lhsIterAsk;
                rhsLevel = *rhsIterAsk;
                assertEqualLevel(lhsLevel, rhsLevel);
                //lhsLevel->assertEqual(*rhsLevel);
                ++lhsIterAsk;
                ++rhsIterAsk;
            }
        }
    }

    if (lhs->getNumBidLevels() != 0 || rhs->getNumBidLevels() != 0)
    {
        MamdaOrderBook::constBidIterator lhsEndBid  = lhs->bidEnd();
        MamdaOrderBook::bidIterator lhsIterBid = lhs->bidBegin();
        MamdaOrderBook::constBidIterator rhsEndBid  = rhs->bidEnd();
        MamdaOrderBook::bidIterator rhsIterBid = rhs->bidBegin();
        MamdaOrderBookPriceLevel* lhsLevel = NULL;
        MamdaOrderBookPriceLevel* rhsLevel = NULL;
        
        for (; lhsIterBid != lhsEndBid; )
        {        
            for (; rhsIterBid != rhsEndBid; )
            {
                rhsLevel = *rhsIterBid;
                lhsLevel= *lhsIterBid;
                assertEqualLevel(lhsLevel, rhsLevel);
                ++lhsIterBid;
                ++rhsIterBid;
            }
        }
    }
}

void myCreateAndAddEntry (publisherTestImpl* myImpl, double price,
                        string id, double size, int status, 
                        MamdaOrderBookEntry::Action entryAction,
                        MamdaOrderBookPriceLevel::Side side)
{
    MamdaOrderBookEntry* entry = new MamdaOrderBookEntry();
    entry->setId(id.c_str());
    entry->setSize(size);
    entry->setAction(entryAction);
    entry->setStatus(status);
    myImpl->myBookTime.setToNow();
    entry->setTime(myImpl->myBookTime); 
    myImpl->myBook->addEntry(entry, price, side, myImpl->myBookTime, NULL);
}

void myCreateAndAddEntryAtLevel (publisherTestImpl* myImpl, double price,
                        string id, double size, int status, 
                        MamdaOrderBookEntry::Action entryAction,
                        MamdaOrderBookPriceLevel::Side side)
{
    MamdaOrderBookEntry* entry = new MamdaOrderBookEntry();
    entry->setId(id.c_str());
    entry->setSize(size);
    entry->setAction(entryAction);
    entry->setStatus(status);
    myImpl->myBookTime.setToNow();
    entry->setTime(myImpl->myBookTime); 

    MamdaOrderBookPriceLevel* level = NULL;
    level = myImpl->myBook->findOrCreateLevel(price, side);
    if (level)
    {
        level->addEntry(entry);
    }
    else printf("\n myCreateAndAddEntryAtLevel(); level = NULL \n");
}

void myDeleteEntry (publisherTestImpl* myImpl, double price,
                        string id, double size, int status, 
                        MamdaOrderBookEntry::Action entryAction,
                        MamdaOrderBookPriceLevel::Side side)
{
    MamdaOrderBookPriceLevel* level = NULL;
    level = myImpl->myBook->findOrCreateLevel(price, side);
    MamdaOrderBookEntry* entry = level->findOrCreateEntry(id.c_str());
    myImpl->myBook->deleteEntry(entry, myImpl->myBookTime, NULL);
}

void myRemoveEntry (publisherTestImpl* myImpl, double price,
                        string id, double size, int status, 
                        MamdaOrderBookEntry::Action entryAction,
                        MamdaOrderBookPriceLevel::Side side)
{
    MamdaOrderBookPriceLevel* level = NULL;
    level = myImpl->myBook->findOrCreateLevel(price, side);
    if (level)
    {
        MamdaOrderBookEntry* entry = level->findOrCreateEntry(id.c_str());
        level->removeEntry(entry);
    }
}

void myRemoveEntryById (publisherTestImpl* myImpl, double price,
                        string id, double size, int status, 
                        MamdaOrderBookEntry::Action entryAction,
                        MamdaOrderBookPriceLevel::Side side)
{
    MamdaOrderBookPriceLevel* level = NULL;
    level = myImpl->myBook->findOrCreateLevel(price, side);
    if (level)
    {
        MamdaOrderBookEntry* entry = level->findOrCreateEntry(id.c_str());
        level->removeEntryById(*entry);
    }
}

void myAddLevel (publisherTestImpl* myImpl, double price, 
               double size, double sizeChange,
               MamdaOrderBookPriceLevel::Action action, 
               MamdaOrderBookPriceLevel::Side side, 
               MamdaOrderBookPriceLevel* level)
{
    level->setPrice(price);
    level->setSize(size);
    level->setSizeChange(sizeChange);
    level->setAction(action);
    level->setSide(side); 

    myImpl->myBookTime.setToNow();
    level->setTime(myImpl->myBookTime);
    //add to book
    myImpl->myBook->addLevel(*level);
    
}

MamdaOrderBookPriceLevel* 
    myFindOrCreateLevel (publisherTestImpl* myImpl, double price, 
                       double size, double sizeChange,
                       MamdaOrderBookPriceLevel::Action action, 
                       MamdaOrderBookPriceLevel::Side side)
{
    MamdaOrderBookPriceLevel* level = NULL;
    level = myImpl->myBook->findOrCreateLevel(price, side);
    level->setPrice(price);
    level->setSize(size);
    level->setSizeChange(sizeChange);
    level->setAction(action);
    level->setSide(side); 

    myImpl->myBookTime.setToNow();
    level->setTime(myImpl->myBookTime);
    return level;
}

MamdaOrderBookPriceLevel* 
    myFindOrCreateEntry (publisherTestImpl* myImpl, double price, 
                       double size, double sizeChange,
                       MamdaOrderBookEntry::Action action, 
                       MamdaOrderBookPriceLevel::Side side)
{
    MamdaOrderBookPriceLevel* level = NULL;
    MamdaOrderBookEntry* entry = NULL;

    level = myImpl->myBook->findOrCreateLevel(price, side);
    level->setSizeChange(sizeChange);
    level->setPrice(price);
    level->setSize(size);

    myImpl->myBookTime.setToNow();
    level->setTime(myImpl->myBookTime);
    level->setAction(MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD);
    level->setSide(side); 
    
    entry = level->findOrCreateEntry("jmg");
    entry->setSize(size);
    entry->setAction(action);
    entry->setTime(myImpl->myBookTime);


    return level;
}

void myUpdateEntry (publisherTestImpl* myImpl, string id, double price,
                   double size, MamdaOrderBookPriceLevel::Side side)
{
    MamdaOrderBookPriceLevel* level = NULL;
    level= myImpl->myBook->findOrCreateLevel(price, side); 
    MamdaOrderBookEntry* entry = NULL;
    entry = level->findOrCreateEntry(id.c_str());
    myImpl->myBookTime.setToNow();
    myImpl->myBook->updateEntry(entry, size, myImpl->myBookTime, NULL);
}

void myUpdateEntryAtLevel (publisherTestImpl* myImpl, string id, double price,
                   double size, MamdaOrderBookPriceLevel::Side side)
{
    MamdaOrderBookPriceLevel* level = NULL;
    level= myImpl->myBook->findOrCreateLevel(price, side); 
    MamdaOrderBookEntry* entry = NULL;
    entry = level->findOrCreateEntry(id.c_str());
    myImpl->myBookTime.setToNow();
    entry->setSize(size);
    entry->setTime(myImpl->myBookTime);
        
    level->updateEntry(*entry);
}

void myUpdateLevel (publisherTestImpl* myImpl, double price, 
                    double size, double sizeChange,
                    MamdaOrderBookPriceLevel::Action action, 
                    MamdaOrderBookPriceLevel::Side side)
{
    MamdaOrderBookPriceLevel* level = NULL;
    level = myImpl->myBook->findOrCreateLevel(price, side);
    level->setPrice(price);
    level->setSize(size);
    level->setSizeChange(sizeChange);
    level->setAction(action);
    level->setSide(side); 

    myImpl->myBookTime.setToNow();
    level->setTime(myImpl->myBookTime);
    myImpl->myBook->updateLevel(*level);
}

void populateRecapAndValidate (publisherTestImpl* myImpl)
{
    
    myImpl->myBook->populateRecap(myImpl->myPublishMsg);
    myImpl->myTicker->callMamdaOnMsg(myImpl->mySubscription, myImpl->myPublishMsg); 
    //printf("\n Own Full Book \n");
    //myImpl->myBook->dump(std::cout);      
    //printf("\n Listener Full Book \n");
    //myImpl->myBookListener->getOrderBook()->dump(std::cout); 
    assertEqual(myImpl->myBook, myImpl->myBookListener->getOrderBook());
    myImpl->myBook->clearDeltaList();
}

void populateDeltaAndValidate( publisherTestImpl* myImpl)
{
    bool publish = false;
    publish = myImpl->myBook->populateDelta(myImpl->myPublishMsg);
    //printf("\n populateDelta() Msg = %s \n", myImpl->myPublishMsg.toString());
    if (publish) 
        myImpl->myTicker->callMamdaOnMsg (myImpl->mySubscription, myImpl->myPublishMsg); 
    //printf("\n Own Delta Book \n");
    //myImpl->myBook->dump(std::cout);      
    //printf("\n Listener Delta Book \n");
    //myImpl->myBookListener->getOrderBook()->dump(std::cout);
    assertEqual(myImpl->myBook, myImpl->myBookListener->getOrderBook());
}

void addMamaHeaderFields (publisherTestImpl* myImpl,
                         MamaMsg&            msg,
                         mamaMsgType         msgType,
                         mamaMsgStatus_      msgStatus,
                         mamaMdMsgType       appMsgType,
                         mama_seqnum_t       seqNum)
{
    myImpl->myPublishMsg.clear();                      

    msg.addU8  (NULL, MamaFieldMsgType.mFid,     msgType);
    msg.addU8  (NULL, MamaFieldMsgStatus.mFid,   msgStatus);
    msg.addU32(MamdaCommonFields::MSG_SEQ_NUM, seqNum);          
    //msg.addU8  (NULL, MamaFieldAppMsgType.mFid,  appMsgType);
    msg.addU64 (NULL, MamaFieldSenderId.mFid,    1);    
}

   
class MamdaBookPublisherTest : public ::testing::Test
{
protected:
    MamdaBookPublisherTest () {}
    virtual ~MamdaBookPublisherTest () {}
  
    virtual void SetUp()
    {
        myImpl = (publisherTestImpl*) calloc(1, sizeof (publisherTestImpl));
        //mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);
        
        myImpl->dictionary = new MamaDictionary;
        myImpl->dictionary->populateFromFile("dictionary");
        MamdaCommonFields::reset();
        MamdaCommonFields::setDictionary (*(myImpl->dictionary));
        MamdaOrderBookFields::reset();
        MamdaOrderBookFields::setDictionary (*(myImpl->dictionary));   
      
        myImpl->mySubscription = new MamdaSubscription;
        myImpl->myBookListener = new MamdaOrderBookListener;
        myImpl->mySubscription->addMsgListener (myImpl->myBookListener);  
        myImpl->myBookListener->setProcessEntries (true);
        myImpl->mySubscription->setType(MAMA_SUBSC_TYPE_BOOK);
        myImpl->mySubscription->setMdDataType(MAMA_MD_DATA_TYPE_ORDER_BOOK);
        myImpl->myBook = new MamdaOrderBook();
        myImpl->myBook->generateDeltaMsgs(true);
        myImpl->myPublishMsg.create();
    
        addMamaHeaderFields(myImpl,
                            myImpl->myPublishMsg,
                            MAMA_MSG_TYPE_BOOK_INITIAL, 
                            MAMA_MSG_STATUS_OK, 
                            MAMA_MD_MSG_TYPE_BOOK_INITIAL,
                            (mama_seqnum_t)0);
        myImpl->myBook->setSymbol("TEST");
        myImpl->myBook->setPartId("NYS");
        myImpl->myTicker = new BookTicker;
        
    }
  
    virtual void TearDown()
    {
        delete myImpl->dictionary;
        myImpl->dictionary = NULL;
        
        delete myImpl->myBook;
        myImpl->myBook = NULL;
        
        //delete myPublishMsg;
        delete myImpl->mySubscription;
        myImpl->mySubscription = NULL;
        
        delete myImpl->myBookListener;
        myImpl->myBookListener = NULL;

        delete myImpl->myTicker;
        myImpl->myTicker = NULL;

        myImpl->myPublishMsg.clear();
        if (myImpl)
        {
            free (myImpl);
        }
                
    }
    publisherTestImpl* myImpl;
    MamdaOrderBookPriceLevel* ownBkLevel;
    MamdaOrderBookPriceLevel* listBkLevel;
};


TEST_F (MamdaBookPublisherTest, DisablePublishingTest)
{                        
    myImpl->myPublishMsg.clear();
    myImpl->myBook->generateDeltaMsgs(false);
    myImpl->myBook->populateRecap(myImpl->myPublishMsg);
    EXPECT_EQ(0, (myImpl->myPublishMsg.getNumFields()));
    myImpl->myPublishMsg.clear();
    bool populated = myImpl->myBook->populateDelta(myImpl->myPublishMsg);
      
    if (populated) ADD_FAILURE();
    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test calls populateRecap and populateDelta when generateDeltaMsgs = false.      "
               "\n It checks no fields are returned from the populated msg when calling populateRecap(),"
               "\n i.e. msg not populated, and that false is returned when calling populateDelta().     "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}

TEST_F (MamdaBookPublisherTest, AddEntryTest)
{
    myCreateAndAddEntry (myImpl, 100.0, "jmg", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);


    myImpl->myBook->populateRecap(myImpl->myPublishMsg);
    myImpl->myTicker->callMamdaOnMsg(myImpl->mySubscription, myImpl->myPublishMsg);  

    ownBkLevel = myImpl->myBook->findOrCreateLevel(100.0, 
                    MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                    
    listBkLevel = myImpl->myBookListener->getOrderBook()->findOrCreateLevel(100.0, 
                    MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    
    EXPECT_STREQ (listBkLevel->findOrCreateEntry("jmg")->getId(),
                 (ownBkLevel->findOrCreateEntry("jmg"))->getId());

    EXPECT_EQ(listBkLevel->getNumEntries(),
              ownBkLevel->getNumEntries());
    EXPECT_EQ(listBkLevel->getNumEntriesTotal(),
              ownBkLevel->getNumEntriesTotal());
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getTotalNumLevels(),
              myImpl->myBook->getTotalNumLevels());
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getNumBidLevels(),
              myImpl->myBook->getNumBidLevels()); 
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getNumAskLevels(),
              myImpl->myBook->getNumAskLevels()); 
    EXPECT_EQ(listBkLevel->getSize(),
              ownBkLevel->getSize());
    EXPECT_EQ(listBkLevel->getPrice(),
              ownBkLevel->getPrice());
    EXPECT_EQ(listBkLevel->getSide(),
              ownBkLevel->getSide());

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test adds an entry to the book using findOrCreateLevel() and level->addEntry(). "
               "\n It then calls populateRecap() and passes the message to an OrderBookListener. The    "
               "\n test then compares the level and entry info of the listener book against the         "
               "\n original book, specifically the entryId, NumEntries, NumLevels, NumBidLevels,        "
               "\n NumAskLevels, level size, level price and level side.                                "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}

TEST_F (MamdaBookPublisherTest, AddEntryAtLevelTest)
{
    myCreateAndAddEntryAtLevel (myImpl, 100.0, "jmg", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);

    myImpl->myBook->populateRecap(myImpl->myPublishMsg);
    myImpl->myTicker->callMamdaOnMsg(myImpl->mySubscription, myImpl->myPublishMsg);  

    ownBkLevel = myImpl->myBook->findOrCreateLevel(100.0, 
                    MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                    
    listBkLevel = myImpl->myBookListener->getOrderBook()->findOrCreateLevel(100.0, 
                    MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    
    EXPECT_STREQ (listBkLevel->findOrCreateEntry("jmg")->getId(),
                 (ownBkLevel->findOrCreateEntry("jmg"))->getId());
    EXPECT_EQ(listBkLevel->getNumEntries(),
              ownBkLevel->getNumEntries());
    EXPECT_EQ(listBkLevel->getNumEntriesTotal(),
              ownBkLevel->getNumEntriesTotal());
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getTotalNumLevels(),
              myImpl->myBook->getTotalNumLevels());
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getNumBidLevels(),
              myImpl->myBook->getNumBidLevels()); 
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getNumAskLevels(),
              myImpl->myBook->getNumAskLevels()); 
    EXPECT_EQ(listBkLevel->getSize(),
              ownBkLevel->getSize());
    EXPECT_EQ(listBkLevel->getPrice(),
              ownBkLevel->getPrice());
    EXPECT_EQ(listBkLevel->getSide(),
              ownBkLevel->getSide());

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a new level using book.findOrCreateLevel, a new entry, then calls  "
               "\n level.addEntry(). It then calls populateRecap() and passes the message to an         " 
               "\n OrderBookListener. The test then compares the level and entry info of the listener   "
               "\n book against the original book, specifically the entryId, NumEntries, NumLevels,     "
               "\n NumBidLevels, NumAskLevels, level size, level price and level side.                  "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}

TEST_F (MamdaBookPublisherTest, AddLevelTest)
{
    MamdaOrderBookPriceLevel* level1= new MamdaOrderBookPriceLevel (); 
    
    myAddLevel (myImpl, 100.0, 1000, 1000, 
              MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD,
              MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID,
              level1);

    myImpl->myBook->populateRecap(myImpl->myPublishMsg);
    myImpl->myTicker->callMamdaOnMsg(myImpl->mySubscription, myImpl->myPublishMsg);  

    ownBkLevel = myImpl->myBook->findOrCreateLevel(100.0, 
                    MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                    
    listBkLevel = myImpl->myBookListener->getOrderBook()->findOrCreateLevel(100.0, 
                    MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    
    EXPECT_EQ(listBkLevel->getNumEntries(),
              ownBkLevel->getNumEntries());
    EXPECT_EQ(listBkLevel->getNumEntriesTotal(),
              ownBkLevel->getNumEntriesTotal());
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getTotalNumLevels(),
              myImpl->myBook->getTotalNumLevels());
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getNumBidLevels(),
              myImpl->myBook->getNumBidLevels()); 
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getNumAskLevels(),
              myImpl->myBook->getNumAskLevels()); 
    EXPECT_EQ(listBkLevel->getSize(),
              ownBkLevel->getSize());
    EXPECT_EQ(listBkLevel->getPrice(),
              ownBkLevel->getPrice());
    EXPECT_EQ(listBkLevel->getSide(),
              ownBkLevel->getSide());

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a new level and the adds it to the book using book.addLevel()      "
               "\n It then calls populateRecap() and passes the message to an OrderBookListener.        " 
               "\n The test then compares the level and entry info of the listener book against the     "
               "\n original book, specifically the NumEntries, NumLevels, NumBidLevels, NumAskLevels,   "
               "\n level size, level price and level side.                                              "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }              
}
               
TEST_F (MamdaBookPublisherTest, FindOrCreateLevelTest)
{
    MamdaOrderBookPriceLevel* level;
    
    level = myFindOrCreateLevel (myImpl, 100.0, 1000, 1000,
                                MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD, 
                                MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);

    myImpl->myBook->populateRecap(myImpl->myPublishMsg);
    myImpl->myTicker->callMamdaOnMsg(myImpl->mySubscription, myImpl->myPublishMsg);  

    ownBkLevel = myImpl->myBook->findOrCreateLevel(100.0, 
                    MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                    
    listBkLevel = myImpl->myBookListener->getOrderBook()->findOrCreateLevel(100.0, 
                    MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    
    EXPECT_EQ(listBkLevel->getNumEntries(),
              ownBkLevel->getNumEntries());
    EXPECT_EQ(listBkLevel->getNumEntriesTotal(),
              ownBkLevel->getNumEntriesTotal());
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getTotalNumLevels(),
              myImpl->myBook->getTotalNumLevels());
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getNumBidLevels(),
              myImpl->myBook->getNumBidLevels()); 
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getNumAskLevels(),
              myImpl->myBook->getNumAskLevels()); 
    EXPECT_EQ(listBkLevel->getSize(),
              ownBkLevel->getSize());
    EXPECT_EQ(listBkLevel->getPrice(),
              ownBkLevel->getPrice());
    EXPECT_EQ(listBkLevel->getSide(),
              ownBkLevel->getSide());

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a new level and adds it to the book using book.findOrCreateLevel() "
               "\n It then calls populateRecap() and passes the message to an OrderBookListener.        " 
               "\n The test then compares the level and entry info of the listener book against the     "
               "\n original book, specifically the NumEntries, NumLevels, NumBidLevels, NumAskLevels,   "
               "\n level size, level price and level side.                                              "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }  
}

TEST_F (MamdaBookPublisherTest, FindOrCreateEntryTest)
{
    MamdaOrderBookPriceLevel* level;
    
    level = myFindOrCreateEntry (myImpl, 100.0, 1000, 1000,
                                MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD, 
                                MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    
    myImpl->myBook->populateRecap(myImpl->myPublishMsg);
    myImpl->myTicker->callMamdaOnMsg(myImpl->mySubscription, myImpl->myPublishMsg);  

    ownBkLevel = myImpl->myBook->findOrCreateLevel(100.0, 
                    MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                    
    listBkLevel = myImpl->myBookListener->getOrderBook()->findOrCreateLevel(100.0, 
                    MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    
    
    EXPECT_EQ(listBkLevel->getNumEntries(),
              ownBkLevel->getNumEntries());
    EXPECT_EQ(listBkLevel->getNumEntriesTotal(),
              ownBkLevel->getNumEntriesTotal());
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getTotalNumLevels(),
              myImpl->myBook->getTotalNumLevels());
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getNumBidLevels(),
              myImpl->myBook->getNumBidLevels()); 
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getNumAskLevels(),
              myImpl->myBook->getNumAskLevels()); 
    EXPECT_EQ(listBkLevel->getSize(),
              ownBkLevel->getSize());
    EXPECT_EQ(listBkLevel->getPrice(),
              ownBkLevel->getPrice());
    EXPECT_EQ(listBkLevel->getSide(),
              ownBkLevel->getSide());

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a new level, findOrCreateLevel(), and new entry findOrCreateEntry()"
               "\n It then calls populateRecap() and passes the message to an OrderBookListener.        " 
               "\n The test then compares the level and entry info of the listener book against the     "
               "\n original book, specifically the NumEntries, NumLevels, NumBidLevels, NumAskLevels,   "
               "\n level size, level price and level side.                                              "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}

TEST_F (MamdaBookPublisherTest, UpdateEntryAtLevelTest)
{
    myCreateAndAddEntry (myImpl, 100.0, "jmg", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                                
    myUpdateEntryAtLevel (myImpl, "jmg", 100.0, 1000.0, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntryAtLevel (myImpl, "jmg", 100.0, 2000.0, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntryAtLevel (myImpl, "jmg", 100.0, 3000.0, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntryAtLevel (myImpl, "jmg", 100.0, 4000.0, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);

    myImpl->myBook->populateRecap(myImpl->myPublishMsg);
    myImpl->myTicker->callMamdaOnMsg(myImpl->mySubscription, myImpl->myPublishMsg);  

    ownBkLevel = myImpl->myBook->findOrCreateLevel(100.0, 
                    MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                    
    listBkLevel = myImpl->myBookListener->getOrderBook()->findOrCreateLevel(100.0, 
                    MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    
    EXPECT_STREQ (listBkLevel->findOrCreateEntry("jmg")->getId(),
                 (ownBkLevel->findOrCreateEntry("jmg"))->getId());
    EXPECT_EQ(listBkLevel->getNumEntries(),
              ownBkLevel->getNumEntries());
    EXPECT_EQ(listBkLevel->getNumEntriesTotal(),
              ownBkLevel->getNumEntriesTotal());
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getTotalNumLevels(),
              myImpl->myBook->getTotalNumLevels());
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getNumBidLevels(),
              myImpl->myBook->getNumBidLevels()); 
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getNumAskLevels(),
              myImpl->myBook->getNumAskLevels()); 
    EXPECT_EQ(listBkLevel->getSize(),
              ownBkLevel->getSize());
    EXPECT_EQ(listBkLevel->getPrice(),
              ownBkLevel->getPrice());
    EXPECT_EQ(listBkLevel->getSide(),
              ownBkLevel->getSide());
              
    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a new level, findOrCreateLevel(), and new entry findOrCreateEntry()"
               "\n and then updates the created entry, using level.updateEntry(), a number of times.    "
               "\n It then calls populateRecap() and passes the message to an OrderBookListener.        " 
               "\n The test then compares the level and entry info of the listener book against the     "
               "\n original book, specifically the NumEntries, NumLevels, NumBidLevels, NumAskLevels,   "
               "\n level size, level price and level side.                                              "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}


TEST_F (MamdaBookPublisherTest, UpdateEntryTest)
{
    myCreateAndAddEntry (myImpl, 100.0, "jmg", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                                
    myUpdateEntry (myImpl, "jmg", 100.0, 1000.0, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntry (myImpl, "jmg", 100.0, 2000.0, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntry (myImpl, "jmg", 100.0, 3000.0, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntry (myImpl, "jmg", 100.0, 4000.0, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);

    myImpl->myBook->populateRecap(myImpl->myPublishMsg);
    myImpl->myTicker->callMamdaOnMsg(myImpl->mySubscription, myImpl->myPublishMsg);  

    ownBkLevel = myImpl->myBook->findOrCreateLevel(100.0, 
                    MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                    
    listBkLevel = myImpl->myBookListener->getOrderBook()->findOrCreateLevel(100.0, 
                    MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
        
    EXPECT_STREQ (listBkLevel->findOrCreateEntry("jmg")->getId(),
                 (ownBkLevel->findOrCreateEntry("jmg"))->getId());
    EXPECT_EQ(listBkLevel->getNumEntries(),
              ownBkLevel->getNumEntries());
    EXPECT_EQ(listBkLevel->getNumEntriesTotal(),
              ownBkLevel->getNumEntriesTotal());
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getTotalNumLevels(),
              myImpl->myBook->getTotalNumLevels());
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getNumBidLevels(),
              myImpl->myBook->getNumBidLevels()); 
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getNumAskLevels(),
              myImpl->myBook->getNumAskLevels()); 
    EXPECT_EQ(listBkLevel->getSize(),
              ownBkLevel->getSize());
    EXPECT_EQ(listBkLevel->getPrice(),
              ownBkLevel->getPrice());
    EXPECT_EQ(listBkLevel->getSide(),
              ownBkLevel->getSide());

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a new level, findOrCreateLevel(), and new entry findOrCreateEntry()"
               "\n and then updates the created entry, using book.updateEntry(), a number of times.     "
               "\n It then calls populateRecap() and passes the message to an OrderBookListener.        " 
               "\n The test then compares the level and entry info of the listener book against the     "
               "\n original book, specifically the entryId, NumEntries, NumLevels, NumBidLevels,        "
               "\n NumAskLevels, level size, level price and level side.                                "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}

TEST_F (MamdaBookPublisherTest, UpdateLevelTest)
{
    MamdaOrderBookPriceLevel* level;
    
    level = myFindOrCreateLevel (myImpl, 100.0, 1000, 1000,
                                MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD, 
                                MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
        
    myUpdateLevel (myImpl, 100.0, 2000, 2000, 
                   MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE,
                   MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);

    myImpl->myBook->populateRecap(myImpl->myPublishMsg);
    myImpl->myTicker->callMamdaOnMsg(myImpl->mySubscription, myImpl->myPublishMsg);  

    ownBkLevel = myImpl->myBook->findOrCreateLevel(100.0, 
                    MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                    
    listBkLevel = myImpl->myBookListener->getOrderBook()->findOrCreateLevel(100.0, 
                    MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
       
    EXPECT_EQ(listBkLevel->getNumEntries(),
              ownBkLevel->getNumEntries());
    EXPECT_EQ(listBkLevel->getNumEntriesTotal(),
              ownBkLevel->getNumEntriesTotal());
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getTotalNumLevels(),
              myImpl->myBook->getTotalNumLevels());
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getNumBidLevels(),
              myImpl->myBook->getNumBidLevels()); 
    EXPECT_EQ(myImpl->myBookListener->getOrderBook()->getNumAskLevels(),
              myImpl->myBook->getNumAskLevels()); 
    EXPECT_EQ(listBkLevel->getSize(),
              ownBkLevel->getSize());
    EXPECT_EQ(listBkLevel->getPrice(),
              ownBkLevel->getPrice());
    EXPECT_EQ(listBkLevel->getSide(),
              ownBkLevel->getSide());
    
    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a new level, findOrCreateLevel(), and then updates the level, using"
               "\n book.updateEntry().                                                                  "
               "\n It then calls populateRecap() and passes the message to an OrderBookListener.        " 
               "\n The test then compares the level and entry info of the listener book against the     "
               "\n original book, specifically the NumEntries, NumLevels, NumBidLevels, NumAskLevels,   "
               "\n level size, level price and level side.                                              "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}

TEST_F (MamdaBookPublisherTest, OneLevelTwoEntriesTest)
{
    MamdaOrderBookPriceLevel* level1 = new MamdaOrderBookPriceLevel();

    myAddLevel (myImpl, 100.0, 0, 0, 
                MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD,
                MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID,
                level1);
                
    myCreateAndAddEntry (myImpl, 100.0, "ent1", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                      
    myCreateAndAddEntry (myImpl, 100.0, "ent2", 500, 82, 
                        MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                        MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);

    populateRecapAndValidate(myImpl);
    
    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test creates a new level, addLevel(), and then creates and adds two new entries "
               "\n using addEntry().                                                                    "
               "\n It then calls populateRecap() and passes the message to an OrderBookListener.        " 
               "\n The test then uses assertEqual to check that the publishing book is the same as the  "
               "\n listener book."
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}

TEST_F (MamdaBookPublisherTest, FlattenEntryInfo)
{
    MamdaOrderBookPriceLevel* level1 = new MamdaOrderBookPriceLevel();

    myCreateAndAddEntry (myImpl, 101.0, "ent", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                                   
    populateRecapAndValidate(myImpl);
    
    const MamaMsg** plVector = NULL;
    size_t numLevels = 0;
    bool hasPriceLevels = myImpl->myPublishMsg.tryVectorMsg(MamdaOrderBookFields::PRICE_LEVELS, plVector, numLevels);  
    if(!hasPriceLevels) ADD_FAILURE();
    EXPECT_EQ(1,numLevels);
    
    const MamaMsg** entryVector = NULL;
    size_t numEntries = 0;
    bool hasEntryVector = myImpl->myPublishMsg.tryVectorMsg(MamdaOrderBookFields::PL_ENTRIES, entryVector, numEntries);
    if (hasEntryVector) ADD_FAILURE();
    
    myCreateAndAddEntry (myImpl, 102.0, "ent1", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                      
    myCreateAndAddEntry (myImpl, 103.0, "ent2", 500, 82, 
                        MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                        MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    
    addMamaHeaderFields(myImpl,
                        myImpl->myPublishMsg,
                        MAMA_MSG_TYPE_BOOK_UPDATE, 
                        MAMA_MSG_STATUS_OK, 
                        MAMA_MD_MSG_TYPE_BOOK_UPDATE,
                        (mama_seqnum_t)1);
                        
    populateDeltaAndValidate(myImpl);

    numLevels = 0;
    hasPriceLevels = false;
    hasPriceLevels = myImpl->myPublishMsg.tryVectorMsg(MamdaOrderBookFields::PRICE_LEVELS, plVector, numLevels);  
    if(!hasPriceLevels) ADD_FAILURE();
    EXPECT_EQ(2,numLevels);
    
    numEntries = 0;
    hasEntryVector = false;
    hasEntryVector= myImpl->myPublishMsg.tryVectorMsg(MamdaOrderBookFields::PL_ENTRIES, entryVector, numEntries);
    if (hasEntryVector) ADD_FAILURE();

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test checks whether entry info is correctly flattened into the price level      "
               "\n It calls populateRecap() and passes the message to an OrderBookListener.             " 
               "\n The test then uses assertEqual to check that the publishing book is the same as the  "
               "\n listener book. It also checks the recap contains a single priceLevel msg and no      "
               "\n entryMsg  vector. It then adds two entries to a level, calls populateDelta() and does"
               "\n the assert check again. The test then checks the returned msg for a PlEntry msg and  "
               "\n fails if one exists, as this should be flattened into plMsg. It also checks for      "
               "\n vector of price level msgs and verfies there should be two.                          "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}

TEST_F (MamdaBookPublisherTest, FlattenTwoEntriesOneLevel)
{
    MamdaOrderBookPriceLevel* level1 = new MamdaOrderBookPriceLevel();

    myCreateAndAddEntry (myImpl, 102.0, "ent", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                                   
    populateRecapAndValidate(myImpl);
    
    myCreateAndAddEntry (myImpl, 102.0, "ent1", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                      
    myCreateAndAddEntry (myImpl, 102.0, "ent2", 500, 82, 
                        MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                        MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);

    addMamaHeaderFields(myImpl,
                        myImpl->myPublishMsg,
                        MAMA_MSG_TYPE_BOOK_UPDATE, 
                        MAMA_MSG_STATUS_OK, 
                        MAMA_MD_MSG_TYPE_BOOK_UPDATE,
                        (mama_seqnum_t)1);
    
    populateDeltaAndValidate(myImpl);
             
    const MamaMsg** msgVector = NULL;
    size_t numEntries = 0;
    bool hasEntryVector;  
    hasEntryVector = myImpl->myPublishMsg.tryVectorMsg(MamdaOrderBookFields::PL_ENTRIES, msgVector, numEntries);
        
    if (!hasEntryVector) 
    {
        ADD_FAILURE();
    }
    EXPECT_EQ (2, numEntries);

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test checks whether two entries are correctly flattened into one price level    "
               "\n during populateDelta."
               "\n It firstly calls populateRecap() and passes the message to an OrderBookListener.     " 
               "\n The test then uses assertEqual to check that the publishing book is the same as the  "
               "\n listener book. It then adds two entries to a level, calls populateDelta() and does   "
               "\n the assert check again. The test then checks the returned msg for a PlEntry msg and  "
               "\n fails if one doesn't exist, also checking the numEntries in the plEntry Msg = 2.     "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}
TEST_F (MamdaBookPublisherTest, FlattenOneEntry)
{
    MamdaOrderBookPriceLevel* level1 = new MamdaOrderBookPriceLevel();

    myCreateAndAddEntry (myImpl, 101.0, "ent", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                                   
    populateRecapAndValidate(myImpl);
    
    myCreateAndAddEntry (myImpl, 102.0, "ent1", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                         
    addMamaHeaderFields(myImpl,
                        myImpl->myPublishMsg,
                        MAMA_MSG_TYPE_BOOK_UPDATE, 
                        MAMA_MSG_STATUS_OK, 
                        MAMA_MD_MSG_TYPE_BOOK_UPDATE,
                        (mama_seqnum_t)1);
    populateDeltaAndValidate(myImpl);
    
    const MamaMsg** msgVector = NULL;
    size_t numEntries = 0;
    bool hasEntryVector;  
    hasEntryVector = myImpl->myPublishMsg.tryVectorMsg(MamdaOrderBookFields::PL_ENTRIES, msgVector, numEntries);
    if (hasEntryVector) 
    {
        ADD_FAILURE();
    }

    const MamaMsg** plVector = NULL;
    size_t numLevels = 0;
    bool hasPriceLevels = myImpl->myPublishMsg.tryVectorMsg(MamdaOrderBookFields::PRICE_LEVELS, plVector, numLevels);  
    if(hasPriceLevels) ADD_FAILURE();
    
    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test checks whether entry and pricelevel info are correctly flattened into the  "
               "\n top level msg duirng populateDelta.                                                  "
               "\n It calls populateRecap() and passes the message to an OrderBookListener.             " 
               "\n The test then uses assertEqual to check that the publishing book is the same as the  "
               "\n listener book. It then adds one entry to a level, calls populateDelta() and does     "
               "\n the assert check again. The test then checks the returned msg for PlEntry and        "
               "\n priceLevel messages and fails if either exist.                                         "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }

}

TEST_F (MamdaBookPublisherTest, RemoveEntryFromLevelTest)
{
    MamdaOrderBookPriceLevel* level1 = new MamdaOrderBookPriceLevel();

    myAddLevel (myImpl, 100.0, 0, 0, 
                MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD,
                MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID,
                level1);
                
    myCreateAndAddEntry (myImpl, 100.0, "ent1", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                      
    myCreateAndAddEntry (myImpl, 100.0, "ent2", 500, 82, 
                        MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                        MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    
    populateRecapAndValidate(myImpl);
    
    myRemoveEntry(myImpl, 100.0, "ent2", 500, 82, 
                        MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE,
                        MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    
    addMamaHeaderFields(myImpl,
                        myImpl->myPublishMsg,
                        MAMA_MSG_TYPE_BOOK_UPDATE, 
                        MAMA_MSG_STATUS_OK, 
                        MAMA_MD_MSG_TYPE_BOOK_UPDATE,
                        (mama_seqnum_t)1);
    populateDeltaAndValidate(myImpl);
    
    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test checks checks publishing functions correctly when removing entries.        "
               "\n It removes an entry from a level, level.removeEntry(), calls populateDelta(), passes "
               "\n returned msg to listener does a book assert check.                                   "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}

TEST_F (MamdaBookPublisherTest, RemoveEntryByIdFromLevelTest)
{
    MamdaOrderBookPriceLevel* level1 = new MamdaOrderBookPriceLevel();

    myAddLevel (myImpl, 100.0, 0, 0, 
                MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD,
                MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID,
                level1);
                
    myCreateAndAddEntry (myImpl, 100.0, "ent1", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                      
    myCreateAndAddEntry (myImpl, 100.0, "ent2", 500, 82, 
                        MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                        MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    
    populateRecapAndValidate(myImpl);
    
    myRemoveEntryById(myImpl, 100.0, "ent2", 500, 82, 
                        MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE,
                        MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    
    addMamaHeaderFields(myImpl,
                        myImpl->myPublishMsg,
                        MAMA_MSG_TYPE_BOOK_UPDATE, 
                        MAMA_MSG_STATUS_OK, 
                        MAMA_MD_MSG_TYPE_BOOK_UPDATE,
                        (mama_seqnum_t)1);
    populateDeltaAndValidate(myImpl);

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test checks checks publishing functions correctly when removing entries.        "
               "\n It removes an entry from a level, level.removeEntryById(), calls populateDelta(),    "
               "\n passes returned msg to listener does a book assert check.                            "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}

TEST_F (MamdaBookPublisherTest, DeleteEntryFromLevelTest)
{
    MamdaOrderBookPriceLevel* level1 = new MamdaOrderBookPriceLevel();

    myAddLevel (myImpl, 100.0, 0, 0, 
                MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD,
                MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID,
                level1);
                
    myCreateAndAddEntry (myImpl, 100.0, "ent1", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                      
    myCreateAndAddEntry (myImpl, 100.0, "ent2", 500, 82, 
                        MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                        MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    
    populateRecapAndValidate(myImpl);
    
    myDeleteEntry(myImpl, 100.0, "ent2", 500, 82, 
                        MamdaOrderBookEntry::MAMDA_BOOK_ACTION_DELETE,
                        MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    
    addMamaHeaderFields(myImpl,
                        myImpl->myPublishMsg,
                        MAMA_MSG_TYPE_BOOK_UPDATE, 
                        MAMA_MSG_STATUS_OK, 
                        MAMA_MD_MSG_TYPE_BOOK_UPDATE,
                        (mama_seqnum_t)1);
    populateDeltaAndValidate(myImpl);
    
    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test checks checks publishing functions correctly when deleting entries.        "
               "\n It removes an entry from a level, level.deleteEntry(), calls populateDelta(),        "
               "\n passes returned msg to listener does a book assert check.                            "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}

TEST_F (MamdaBookPublisherTest, populateDeltaEmptyStateTest)
{
    MamdaOrderBookPriceLevel* level1 = new MamdaOrderBookPriceLevel();

    myAddLevel (myImpl, 100.0, 0, 0, 
                MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD,
                MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID,
                level1);
                
    myCreateAndAddEntry (myImpl, 100.0, "ent1", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                      
    myCreateAndAddEntry (myImpl, 100.0, "ent2", 500, 82, 
                        MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                        MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    
    populateRecapAndValidate(myImpl);
    
    addMamaHeaderFields(myImpl,
                        myImpl->myPublishMsg,
                        MAMA_MSG_TYPE_BOOK_UPDATE, 
                        MAMA_MSG_STATUS_OK, 
                        MAMA_MD_MSG_TYPE_BOOK_UPDATE,
                        (mama_seqnum_t)1);
    
    bool publish = false;
    publish = myImpl->myBook->populateDelta(myImpl->myPublishMsg);
    if (publish) ADD_FAILURE();
    
    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test checks checks publishing functions correctly delta list is empty.          "
               "\n populateDelta is called when there's an empty delta list. Returned bool should be    "
               "\n false                                                                                "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}

TEST_F (MamdaBookPublisherTest, populateRecapEmptyStateTest)
{
    populateRecapAndValidate(myImpl);
    
    const MamaMsg** mVector = NULL;
    size_t numLevels = 0;
    size_t numEntries = 0;
    bool hasPriceLevels = myImpl->myPublishMsg.tryVectorMsg(MamdaOrderBookFields::PRICE_LEVELS, mVector, numLevels);  
    if(hasPriceLevels) ADD_FAILURE();
    bool hasEntryVector = myImpl->myPublishMsg.tryVectorMsg(MamdaOrderBookFields::PL_ENTRIES, mVector, numEntries);  
    if(hasEntryVector) ADD_FAILURE();

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test checks checks publishing functions correctly delta list is empty.          "
               "\n populateRecap is called when there's an empty delta list, and we check the populated "
               "\n msg for the existence of entry and price levels msgs, fialing if found               "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}


TEST_F (MamdaBookPublisherTest, MultipleLevelsAndEntriesTest)
{
    MamdaOrderBookPriceLevel* level1 = new MamdaOrderBookPriceLevel();
              
    myCreateAndAddEntry (myImpl, 100.0, "ent1", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                      
    myCreateAndAddEntry (myImpl, 100.0, "ent2", 500, 82, 
                        MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                        MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    
    MamdaOrderBookPriceLevel* level2;
    
    level2 = myFindOrCreateLevel (myImpl, 200.0, 2000, 2000,
                                 MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD, 
                                 MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
        
    MamdaOrderBookPriceLevel* level3;
    
    level3 = myFindOrCreateLevel (myImpl, 300.0, 3000, 3000,
                                 MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD, 
                                 MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                                
    MamdaOrderBookPriceLevel* level4;
    
    level4 = myFindOrCreateLevel (myImpl, 400.0, 4000, 4000,
                                 MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD, 
                                 MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);

    myImpl->myBook->populateRecap(myImpl->myPublishMsg);
    myImpl->myTicker->callMamdaOnMsg(myImpl->mySubscription, myImpl->myPublishMsg); 
    assertEqual(myImpl->myBook, myImpl->myBookListener->getOrderBook());
    myImpl->myBook->clearDeltaList();

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test checks checks publishing functions correctly for multiple levels and       "
               "\n entries. A number of entries are added to a book and populateRecap() is called.      "
               "\n The populated msg is passed to a orderBookListener and we assert both books are equal"
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}

TEST_F (MamdaBookPublisherTest, UpdateMultipleLevelsTest)
{
    MamdaOrderBookPriceLevel* level1 = new MamdaOrderBookPriceLevel();

              
    myCreateAndAddEntry (myImpl, 100.0, "ent1", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                      
    myCreateAndAddEntry (myImpl, 100.0, "ent2", 500, 82, 
                        MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                        MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    
    myUpdateEntry (myImpl, "ent2", 100.0, 2000.0, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);

    MamdaOrderBookPriceLevel* level2;
    
    level2 = myFindOrCreateLevel (myImpl, 200.0, 2000, 2000,
                                 MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD, 
                                 MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
        
    MamdaOrderBookPriceLevel* level3;
    
    level3 = myFindOrCreateLevel (myImpl, 300.0, 3000, 3000,
                                 MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD, 
                                 MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                                
    MamdaOrderBookPriceLevel* level4;
    
    level4 = myFindOrCreateLevel (myImpl, 400.0, 4000, 4000,
                                 MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD, 
                                 MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);

    populateRecapAndValidate(myImpl);
        
    myUpdateEntry (myImpl, "ent2", 100.0, 2000.0, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateLevel (myImpl, 200.0, 4000, 2000, 
                   MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE,
                   MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);

    myUpdateLevel (myImpl, 300.0, 6000, 3000, 
                   MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE,
                   MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                   
    myUpdateLevel (myImpl, 400.0, 3000, -1000, 
                   MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE,
                   MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                   
                       myUpdateLevel (myImpl, 200.0, 5000, 1000, 
                   MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE,
                   MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                   
    myUpdateLevel (myImpl, 300.0, 7000, 1000, 
                   MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE,
                   MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                   
    myUpdateLevel (myImpl, 400.0, 2000, -1000, 
                   MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE,
                   MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);

    myUpdateEntry (myImpl, "ent1", 100.0, 2000.0, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntry (myImpl, "ent2", 100.0, 3000.0, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntry (myImpl, "ent1", 100.0, 3000.0, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);                   

    myUpdateEntry (myImpl, "ent2", 100.0, 2000.0, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                   
    addMamaHeaderFields(myImpl,
                        myImpl->myPublishMsg,
                        MAMA_MSG_TYPE_BOOK_UPDATE, 
                        MAMA_MSG_STATUS_OK, 
                        MAMA_MD_MSG_TYPE_BOOK_UPDATE,
                        (mama_seqnum_t)1);
    populateDeltaAndValidate(myImpl);
    
    myUpdateLevel (myImpl, 400.0, 1000, -1000, 
                  MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE,
                  MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                  
    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test checks checks publishing functions correctly for updating multiple levels  "
               "\n A number of entries are added to a book and populateRecap() is called.      "
               "\n The populated msg is passed to a orderBookListener and we assert both books are equal"
               "\n We then update a no. of entries and levels and populateDelta() is called.      "
               "\n The populated msg is passed to a orderBookListener and we assert both books are equal"               
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}

TEST_F (MamdaBookPublisherTest, UpdateMultipleEntriesTest)
{
    MamdaOrderBookPriceLevel* level1 = new MamdaOrderBookPriceLevel();

              
    myCreateAndAddEntry (myImpl, 100.0, "ent1", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                      
    myCreateAndAddEntry (myImpl, 100.0, "ent2", 500, 82, 
                        MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                        MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    
    MamdaOrderBookPriceLevel* level2;
    
    level2 = myFindOrCreateLevel (myImpl, 200.0, 2000, 2000,
                                 MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD, 
                                 MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
        
    MamdaOrderBookPriceLevel* level3;
    
    level3 = myFindOrCreateLevel (myImpl, 300.0, 3000, 3000,
                                 MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD, 
                                 MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                                
    MamdaOrderBookPriceLevel* level4;
    
    level4 = myFindOrCreateLevel (myImpl, 400.0, 4000, 4000,
                                 MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD, 
                                 MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);

    populateRecapAndValidate(myImpl);
    
    myUpdateEntry (myImpl, "ent1", 100.0, 1000.0, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntry (myImpl, "ent2", 100.0, 1000.0, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);

    myUpdateEntry (myImpl, "ent1", 100.0, 2000.0, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntry (myImpl, "ent2", 100.0, 2000.0, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    
    myUpdateLevel (myImpl, 300.0, 6000, 3000, 
               MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE,
               MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                   
    addMamaHeaderFields(myImpl,
                        myImpl->myPublishMsg,
                        MAMA_MSG_TYPE_BOOK_UPDATE, 
                        MAMA_MSG_STATUS_OK, 
                        MAMA_MD_MSG_TYPE_BOOK_UPDATE,
                        (mama_seqnum_t)1);
    populateDeltaAndValidate(myImpl);
    
    myUpdateLevel (myImpl, 300.0, 7000, 1000, 
           MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE,
           MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
                   
    addMamaHeaderFields(myImpl,
                        myImpl->myPublishMsg,
                        MAMA_MSG_TYPE_BOOK_UPDATE, 
                        MAMA_MSG_STATUS_OK, 
                        MAMA_MD_MSG_TYPE_BOOK_UPDATE,
                        (mama_seqnum_t)2);
   populateDeltaAndValidate(myImpl);
    
    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test checks checks publishing functions correctly for updating multiple levels  "
               "\n A number of entries are added to a book and populateRecap() is called.      "
               "\n The populated msg is passed to a orderBookListener and we assert both books are equal"
               "\n We then update a no. of entries and levels and populateDelta() is called.      "
               "\n The populated msg is passed to a orderBookListener and we assert both books are equal"  
               "\n We then update a level and populateDelta() is called.      "
               "\n The populated msg is passed to a orderBookListener and we assert both books are equal"               
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}

TEST_F (MamdaBookPublisherTest, PopulateRecapSimpleCpuTest)
{
    myCreateAndAddEntry (myImpl, 100.0, "jmg", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    START_RECORDING_CPU (5000000);
    myImpl->myBook->populateRecap(myImpl->myPublishMsg);
    myImpl->myPublishMsg.clear();
    STOP_RECORDING_CPU ();

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test checks the CPU time for calling populateRecap() populating a one level, one"
               "\n entry msg. This test CPU for populateMsg(simpleBook)                                 "           
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}

TEST_F (MamdaBookPublisherTest, PopulateRecapComplexCpuTest)
{
    myCreateAndAddEntry (myImpl, 100.0, "jmg", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myCreateAndAddEntry (myImpl, 100.0, "jmg2", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    START_RECORDING_CPU (5000000);
    myImpl->myBook->populateRecap(myImpl->myPublishMsg);
    myImpl->myPublishMsg.clear();
    STOP_RECORDING_CPU ();
    
    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test checks the CPU time for calling populateRecap() populating a one level, two"
               "\n entry msg. This test CPU for populateMsg(complexBook)                               "           
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}


TEST_F (MamdaBookPublisherTest, PopulateDeltaSimpleCpuTest)
{
    myImpl->myPublishMsg.clear();
    myCreateAndAddEntry (myImpl, 100.0, "jmg", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myImpl->myBook->clearDeltaList();
    START_RECORDING_CPU (5000000);
    myUpdateEntry (myImpl, "jmg", 100.0, 500, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myImpl->myBook->populateDelta(myImpl->myPublishMsg);
    myImpl->myPublishMsg.clear();
    STOP_RECORDING_CPU ();
    
    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test checks the CPU time for calling populateDelta() populating a one level, two"
               "\n entry msg. This test CPU for populateMsg(simpleDelta)                                "           
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}

TEST_F (MamdaBookPublisherTest, PopulateDeltaComplexCpuTest)
{
    myImpl->myPublishMsg.clear();
    myCreateAndAddEntry (myImpl, 100.0, "jmg", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myCreateAndAddEntry (myImpl, 100.0, "jmg2", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myImpl->myBook->clearDeltaList();
    START_RECORDING_CPU (5000000);
    myUpdateEntry (myImpl, "jmg", 100.0, 500, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntry (myImpl, "jmg2", 100.0, 500, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myImpl->myBook->populateDelta(myImpl->myPublishMsg);
    myImpl->myPublishMsg.clear();
    STOP_RECORDING_CPU ();
    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test checks the CPU time for calling populateDelta() populating a one level, two"
               "\n entry msg. This test CPU for populateMsg(complexDelta)                               "           
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}

TEST_F (MamdaBookPublisherTest, PopulateLargeDeltaCpuTest)
{
    myImpl->myPublishMsg.clear();
    myCreateAndAddEntry (myImpl, 100.0, "jmg", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myCreateAndAddEntry (myImpl, 100.0, "jmg2", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myCreateAndAddEntry (myImpl, 101.0, "jmg", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myCreateAndAddEntry (myImpl, 101.0, "jmg2", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myCreateAndAddEntry (myImpl, 102.0, "jmg", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myCreateAndAddEntry (myImpl, 102.0, "jmg2", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);                      
    myCreateAndAddEntry (myImpl, 103.0, "jmg", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myCreateAndAddEntry (myImpl, 103.0, "jmg2", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myCreateAndAddEntry (myImpl, 104.0, "jmg", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myCreateAndAddEntry (myImpl, 104.0, "jmg2", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);                      
    myImpl->myBook->clearDeltaList();
    START_RECORDING_CPU (1000000);
    myUpdateEntry (myImpl, "jmg", 100.0, 500, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntry (myImpl, "jmg2", 100.0, 500, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntry (myImpl, "jmg", 101.0, 500, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntry (myImpl, "jmg2", 101.0, 500, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntry (myImpl, "jmg", 102.0, 500, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntry (myImpl, "jmg2", 102.0, 500, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntry (myImpl, "jmg", 103.0, 500, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntry (myImpl, "jmg2", 103.0, 500, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntry (myImpl, "jmg", 104.0, 500, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntry (myImpl, "jmg2", 104.0, 500, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myImpl->myBook->populateDelta(myImpl->myPublishMsg);
    myImpl->myPublishMsg.clear();
    STOP_RECORDING_CPU ();
//     myImpl->myBook->dump(std::cout);
    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test checks the CPU time for calling populateDelta() populating a one level, two"
               "\n entry msg. This test CPU for populateMsg(complexDelta) populating very large msg     "           
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}

//Test grows vector msg holder at beginning and sucessive updates never need larger vector
//Should be similar to typical use case 
TEST_F (MamdaBookPublisherTest, PopulateDeltaGrowOnceCpuTest)
{
    myImpl->myPublishMsg.clear();
    myCreateAndAddEntry (myImpl, 100.0, "jmg", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myCreateAndAddEntry (myImpl, 100.0, "jmg2", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myCreateAndAddEntry (myImpl, 101.0, "jmg", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myCreateAndAddEntry (myImpl, 101.0, "jmg2", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myCreateAndAddEntry (myImpl, 102.0, "jmg", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myCreateAndAddEntry (myImpl, 102.0, "jmg2", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);                      
    myCreateAndAddEntry (myImpl, 103.0, "jmg", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myCreateAndAddEntry (myImpl, 103.0, "jmg2", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myCreateAndAddEntry (myImpl, 104.0, "jmg", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myCreateAndAddEntry (myImpl, 104.0, "jmg2", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);                      
    myImpl->myBook->clearDeltaList();
    START_RECORDING_CPU (1000000);
    myUpdateEntry (myImpl, "jmg", 100.0, 500, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntry (myImpl, "jmg2", 100.0, 500, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntry (myImpl, "jmg", 101.0, 500, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myUpdateEntry (myImpl, "jmg2", 101.0, 500, MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);
    myImpl->myBook->populateDelta(myImpl->myPublishMsg);
    myImpl->myPublishMsg.clear();
    STOP_RECORDING_CPU ();
//     myImpl->myBook->dump(std::cout);

    const ::testing::TestInfo* const test_info =
        ::testing::UnitTest::GetInstance()->current_test_info();
    if (HasFailure())
    {
        printf("\n **********************************************************************************   "
               "\n \t\t      **** %s failed ****                                                        "
               "\n This test checks the CPU time for calling populateDelta(). The Test grows vector msg "
               "\n holder at beginning and sucessive updates never need larger vector                   "
               "\n This CPU test should be similar to typical use case                                  "
               "\n ********************************************************************************** \n",               
               test_info->name());
    }
}

TEST_F (MamdaBookPublisherTest, populateRecapMemTest)
{
    myCreateAndAddEntry (myImpl, 100.0, "jmg", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);

    START_RECORDING_MEMORY ();
    myImpl->myBook->populateRecap(myImpl->myPublishMsg);
    STOP_RECORDING_MEMORY ();


}

TEST_F (MamdaBookPublisherTest, populateDeltaMemTest)
{
    myCreateAndAddEntry (myImpl, 100.0, "jmg", 500, 82, 
                      MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD,
                      MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID);

    START_RECORDING_MEMORY ();
    myImpl->myBook->populateDelta(myImpl->myPublishMsg);
    STOP_RECORDING_MEMORY ();
}
