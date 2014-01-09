/*
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
// GTest & unit test Headers
#include <gtest/gtest.h>
#include "MainUnitTestC.h"

// STL Headers`
#include <map>
#include <string>

// MAMA Headers
#include "mama/mama.h"
#include "mama/msgstatus.h"

class MsgStatusTestsC : public ::testing::Test
{
protected:
   MsgStatusTestsC();
   virtual ~MsgStatusTestsC();

   virtual void SetUp(void);

   virtual void TearDown(void);

   typedef std::pair<mamaMsgStatus, std::string> MsgStatusPair;
   typedef std::map<mamaMsgStatus, std::string>  MsgStatusMapType;

   MsgStatusMapType testData; 
   mamaMsg            mMsg;
   mamaPayloadBridge  mPayloadBridge;
private:   
   void CreateTestData();

};

MsgStatusTestsC::MsgStatusTestsC() : testing::Test() 
{
   CreateTestData();
}


MsgStatusTestsC::~MsgStatusTestsC() {}

void
MsgStatusTestsC::SetUp()
{
    mama_loadPayloadBridge (&mPayloadBridge, getPayload());
    mamaMsg_create (&mMsg);
}

void
MsgStatusTestsC::TearDown()
{
	mamaMsg_destroy(mMsg);
}

void
MsgStatusTestsC::CreateTestData()
{
    testData.insert(MsgStatusPair(MAMA_MSG_STATUS_OK, "OK"));
    testData.insert(MsgStatusPair(MAMA_MSG_STATUS_LINE_DOWN, "LINE_DOWN"));
    testData.insert(MsgStatusPair(MAMA_MSG_STATUS_NO_SUBSCRIBERS, "NO_SUBSCRIBERS"));
    testData.insert(MsgStatusPair(MAMA_MSG_STATUS_BAD_SYMBOL, "BAD_SYMBOL"));
    testData.insert(MsgStatusPair(MAMA_MSG_STATUS_EXPIRED, "EXPIRED"));
    testData.insert(MsgStatusPair(MAMA_MSG_STATUS_TIMEOUT, "TIMEOUT"));
    testData.insert(MsgStatusPair(MAMA_MSG_STATUS_MISC, "MISC"));
    testData.insert(MsgStatusPair(MAMA_MSG_STATUS_STALE, "STALE"));
    testData.insert(MsgStatusPair(MAMA_MSG_STATUS_TIBRV_STATUS, "PLATFORM_SPECIFIC_STATUS"));
    testData.insert(MsgStatusPair(MAMA_MSG_STATUS_PLATFORM_STATUS, "PLATFORM_SPECIFIC_STATUS"));
    testData.insert(MsgStatusPair(MAMA_MSG_STATUS_NOT_ENTITLED, "NOT_ENTITLED"));
    testData.insert(MsgStatusPair(MAMA_MSG_STATUS_NOT_FOUND, "NOT_FOUND"));
    testData.insert(MsgStatusPair(MAMA_MSG_STATUS_POSSIBLY_STALE, "POSSIBLY_STALE"));
    testData.insert(MsgStatusPair(MAMA_MSG_STATUS_NOT_PERMISSIONED, "NOT_PERMISSIONED"));
    testData.insert(MsgStatusPair(MAMA_MSG_STATUS_TOPIC_CHANGE, "TOPIC_CHANGE"));
    testData.insert(MsgStatusPair(MAMA_MSG_STATUS_BANDWIDTH_EXCEEDED, "BANDWIDTH_EXCEEDED"));
    testData.insert(MsgStatusPair(MAMA_MSG_STATUS_DUPLICATE, "DUPLICATE"));
    testData.insert(MsgStatusPair(MAMA_MSG_STATUS_UNKNOWN, "UNKNOWN"));
}


TEST_F (MsgStatusTestsC, testStringForStatus)
{
    MsgStatusMapType::iterator itr;
    bool bPassed = true;

    for(itr = testData.begin(); itr != testData.end(); ++itr) {
    
       mamaMsgStatus status = (*itr).first;
       std::string szExpected = (*itr).second;

       std::string szActual = mamaMsgStatus_stringForStatus(status); 

       EXPECT_STREQ(szActual.c_str(), szExpected.c_str());

       if (szActual != szExpected) bPassed = false;
    }

    ASSERT_EQ( bPassed, true );
}

TEST_F (MsgStatusTestsC, testStatusForMsg)
{

    mamaMsgStatus expected = MAMA_MSG_STATUS_OK;
    mamaMsgStatus result = mamaMsgStatus_statusForMsg(mMsg);

    ASSERT_EQ( result, expected );
}

TEST_F (MsgStatusTestsC, testStringForMsg)
{
    std::string szExpected = "OK"; 
    
    std::string szResult = mamaMsgStatus_stringForMsg(mMsg);

    ASSERT_EQ( szResult, szExpected );
}
