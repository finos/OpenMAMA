/* $Id: //commsof_depot/platform/products/6.0/trunk/mama/c_cpp/src/gunittest/c/mamadatetime/datetimerangetest.cpp#1 $
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

/*  Description: These tests will check the creation and manipulation of
 *               the mamaDateTime format.
 */

// GTest Header
#include <gtest/gtest.h>

// MAMA Headers
#include "mama/mama.h"
#include "mama/timezone.h"
#include "mama/status.h"
#include "mama/datetime.h"

// Unit Test Headers
//#include "MainUnitTestC.h"


// STL Headers
#include <iostream>
#include <vector>
#include <map>

class DateTimeRangeTests : public ::testing::Test
{
protected:

    DateTimeRangeTests();
    virtual ~DateTimeRangeTests();

    virtual void SetUp(void);
    virtual void TearDown(void);

protected:
   typedef struct TestDataItemStruct
   {
      mama_u32_t year;
      mama_u32_t month;
      mama_u32_t day;
      mama_u32_t hour;
      mama_u32_t min;
      mama_u32_t secs;
      mama_u32_t micsecs;
      mamaDayOfWeek dow;
   } TestDataItem;

   typedef std::pair<mamaDateTime, TestDataItem> TestDataPair;
   typedef std::map<mamaDateTime, TestDataItem> TestDataMap;
   TestDataMap m_TestData;

   typedef std::vector<std::string> DateTimeStringColl;
   DateTimeStringColl m_TestDateTimeStrings;

   void ClearAndCopyTest(const mamaDateTime& t);
   void DayOfWeekTest(const mamaDateTime& t, const mamaDayOfWeek& dow);
   void EqualAndCompareTest(const mamaDateTime& t);
   void HasDateAndTimeTest(const mamaDateTime& t);
   void GettersTest(const mamaDateTime& t, const TestDataItem& item);

private:
   void MakeTestData();
   void DestroyTestData();
   void MakeDateTimeStrings();
   void MakeTestPairs();
};

DateTimeRangeTests::DateTimeRangeTests() : ::testing::Test()
{
   MakeTestData();
}

DateTimeRangeTests::~DateTimeRangeTests()
{
}

void
DateTimeRangeTests::MakeDateTimeStrings()
{
   m_TestDateTimeStrings.push_back("1970-07-04 10:03:21.123");
   m_TestDateTimeStrings.push_back("2013-01-01 10:03:21.123");
   m_TestDateTimeStrings.push_back("2013-07-04 00:00:00.000");
   m_TestDateTimeStrings.push_back("2013-07-04 10:03:21.789");
   m_TestDateTimeStrings.push_back("2013-12-31 10:03:21.123");
   m_TestDateTimeStrings.push_back("2013-07-04 23:03:21.123");
   m_TestDateTimeStrings.push_back("1900-07-04 10:03:21.123");
   m_TestDateTimeStrings.push_back("2050-07-04 10:03:21.123");
   m_TestDateTimeStrings.push_back("2012-07-04 10:03:21.123");
   m_TestDateTimeStrings.push_back("2011-07-04 10:03:21.123");
}

void
DateTimeRangeTests::MakeTestPairs()
{
   mamaDateTime t;
   TestDataItem data;

   ASSERT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
   ASSERT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, "2013-06-17 10:03:21.123") );
   data.year = 2013; data.month = 6; data.day = 17; data.hour = 10; data.min = 3; data.secs = 21; data.micsecs = 123000; 
   data.dow = Monday;
   m_TestData.insert(TestDataPair(t, data));

   ASSERT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
   ASSERT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, "2013-06-18 00:00:00.0") );
   data.year = 2013; data.month = 6; data.day = 18; data.hour = 0; data.min = 0; data.secs = 0; data.micsecs = 0; 
   data.dow = Tuesday;
   m_TestData.insert(TestDataPair(t, data));

   ASSERT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
   ASSERT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, "2013-06-19 00:00:00.0") );
   data.year = 2013; data.month = 6; data.day = 19; data.hour = 0; data.min = 0; data.secs = 0; data.micsecs = 0; 
   data.dow = Wednesday;
   m_TestData.insert(TestDataPair(t, data));

   ASSERT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
   ASSERT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, "2013-06-20 00:00:00.0") );
   data.year = 2013; data.month = 6; data.day = 20; data.hour = 0; data.min = 0; data.secs = 0; data.micsecs = 0; 
   data.dow = Thursday;
   m_TestData.insert(TestDataPair(t, data));

   ASSERT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
   ASSERT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, "2013-06-21 00:00:00.0") );
   data.year = 2013; data.month = 6; data.day = 21; data.hour = 0; data.min = 0; data.secs = 0; data.micsecs = 0; 
   data.dow = Friday;
   m_TestData.insert(TestDataPair(t, data));

   ASSERT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
   ASSERT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, "2013-06-22 00:00:00.0") );
   data.year = 2013; data.month = 6; data.day = 22; data.hour = 0; data.min = 0; data.secs = 0; data.micsecs = 0; 
   data.dow = Saturday;
   m_TestData.insert(TestDataPair(t, data));

   ASSERT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
   ASSERT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, "2013-06-23 00:00:00.0") );
   data.year = 2013; data.month = 6; data.day = 23; data.hour = 0; data.min = 0; data.secs = 0; data.micsecs = 0; 
   data.dow = Sunday;
   m_TestData.insert(TestDataPair(t, data));

}


void
DateTimeRangeTests::MakeTestData()
{
   MakeDateTimeStrings();
   MakeTestPairs();
}

void
DateTimeRangeTests::DestroyTestData()
{
   for (TestDataMap::iterator itr = m_TestData.begin(); itr != m_TestData.end(); ++itr)
   {
      mamaDateTime t = (*itr).first;   

      ASSERT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
   }
}

void
DateTimeRangeTests::SetUp(void)
{
}

void
DateTimeRangeTests::TearDown(void)
{
}


void 
DateTimeRangeTests::ClearAndCopyTest(const mamaDateTime& t)
{
   mamaDateTime t1, t2;
   mama_bool_t b;

   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t1) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t2) );

   // t is not equal to t1 or t2
   EXPECT_NE ( 1, mamaDateTime_equal(t,t1) );
   EXPECT_NE ( 1, mamaDateTime_equal(t,t2) );

   // Copy t into t1 and t2
   // Check they are equal
   // clear t1
   // check they are not equal
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_copy(t1,t) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_copy(t2,t) );
   EXPECT_EQ ( 1, mamaDateTime_equal(t1,t2) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_clear(t2) );
   EXPECT_NE ( 1, mamaDateTime_equal(t1,t2) );

   // Test Copy Time
   // Change the time of t1
   // copy the time from t1 into t2
   // Check that the time components of t2 are as expected
   // Clear t2
   // Check that the time compoents of t2 do not equal the original values.
   mama_u32_t expHour = 10, expMinute = 6, expSecond = 18, expMicrosecond = 123,
              actHour,      actMinute,     actSecond,      actMicrosecond;

   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setTime(t1, expHour, expMinute, expSecond, expMicrosecond) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_copyTime(t2,t1) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getHour(t2, &actHour) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getMinute(t2, &actMinute) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getSecond(t2, &actSecond) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getMicrosecond(t2, &actMicrosecond) );
   EXPECT_EQ ( actHour, expHour);
   EXPECT_EQ ( actMinute, expMinute);
   EXPECT_EQ ( actSecond, expSecond);
   EXPECT_EQ ( actMicrosecond, expMicrosecond);
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_hasTime(t2, &b) );  
   EXPECT_TRUE ( b );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_clearTime(t2) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_hasTime(t2, &b) );  
   EXPECT_FALSE ( b );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getHour(t2, &actHour) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getMinute(t2, &actMinute) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getSecond(t2, &actSecond) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getMicrosecond(t2, &actMicrosecond) );
   EXPECT_NE ( actHour, expHour);
   EXPECT_NE ( actMinute, expMinute);
   EXPECT_NE ( actSecond, expSecond);
   EXPECT_NE ( actMicrosecond, expMicrosecond);

   // Test Copy Date
   // Change the date of t1
   // copy the date from t1 into t2
   // Check that the time components of t2 are as expected
   // Clear t2
   // Check that the Date compoents of t2 do not equal the original values.
   mama_u32_t expYear = 2009, expMonth = 6, expDay = 18,
              actYear,        actMonth,     actDay;

   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setDate(t1, expYear, expMonth, expDay) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_copyDate(t2,t1) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getYear(t2, &actYear) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getMonth(t2, &actMonth) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getDay(t2, &actDay) );
   EXPECT_EQ ( actYear, expYear);
   EXPECT_EQ ( actMonth, expMonth);
   EXPECT_EQ ( actDay, expDay);
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_hasDate(t2, &b) );  
   EXPECT_TRUE ( b );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_clearDate(t2) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_hasDate(t2, &b) );  
   EXPECT_FALSE ( b );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getYear(t2, &actYear) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getMonth(t2, &actMonth) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getDay(t2, &actDay) );
   EXPECT_NE ( actYear, expYear);
   EXPECT_NE ( actMonth, expMonth);
   EXPECT_NE ( actDay, expDay);

   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t1) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t2) );
}

TEST_F (DateTimeRangeTests, testClearAndCopy)
{
   TestDataMap::iterator itr;

   for (itr = m_TestData.begin(); itr != m_TestData.end(); ++itr)
   {
      mamaDateTime t = (*itr).first;

      ClearAndCopyTest(t);
   }
}

void
DateTimeRangeTests::DayOfWeekTest(const mamaDateTime& t, const mamaDayOfWeek& dow)
{
   mamaDayOfWeek act;

   ASSERT_EQ ( MAMA_STATUS_OK, mamaDateTime_getDayOfWeek(t, &act) );
   ASSERT_EQ ( dow, act );
}

TEST_F (DateTimeRangeTests, testDayOfWeek)
{

   TestDataMap::iterator itr;

   for (itr = m_TestData.begin(); itr != m_TestData.end(); ++itr)
   {
      mamaDateTime t    = (*itr).first;   
      mamaDayOfWeek dow = (*itr).second.dow;   

      DayOfWeekTest(t, dow);
   }
}

void 
DateTimeRangeTests::EqualAndCompareTest(const mamaDateTime& t)
{

   mamaDateTime cpy;
   mama_u32_t year, month, day;

   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&cpy) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getYear(cpy, &year) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getMonth(cpy, &month) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getDay(cpy, &day) );

   // Copy t
   // Chek both date times are eaual
   // Change the value of Copy
   // Check they do not equal each other
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_copy(cpy,t) );
   EXPECT_EQ ( 1, mamaDateTime_equal(t, cpy) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToNow(cpy) );
   EXPECT_NE ( 1, mamaDateTime_equal(t, cpy) );


   // Copy t
   // Check that Compare is ZERO
   // Set cpy to be less that t
   // Check that compare = 1
   // Set cpy to be greater than t
   // Check that compare = -1
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_copy(cpy,t) );
   EXPECT_EQ ( 0, mamaDateTime_compare(t, cpy) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setDate(cpy, year - 1, month, day) );
   EXPECT_EQ ( 1, mamaDateTime_compare(t, cpy) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_copy(cpy,t) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setDate(cpy, year + 10, month, day) );
   EXPECT_EQ ( -1, mamaDateTime_compare(t, cpy) );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(cpy) );
   
}

TEST_F (DateTimeRangeTests, testEqualAndCompare)
{
   TestDataMap::iterator itr;

   for (itr = m_TestData.begin(); itr != m_TestData.end(); ++itr)
   {
      mamaDateTime t = (*itr).first;

      EqualAndCompareTest(t);
   }
}

void 
DateTimeRangeTests::HasDateAndTimeTest(const mamaDateTime& t)
{
   mamaDateTime cpy;
   mama_bool_t b;

   // Copy t
   // Set it to NOW
   // Test that HAs Date and Has Time are both true
   // Clear copy
   // Test that Has Date and Has TIme are both False.
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&cpy) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_copy(cpy,t) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToNow(cpy) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_hasTime(cpy, &b) );
   EXPECT_TRUE ( b );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_hasDate(cpy, &b) );
   EXPECT_TRUE ( b );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_clear(cpy) );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_hasTime(cpy, &b) );
   EXPECT_FALSE ( b );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_hasDate(cpy, &b) );
   EXPECT_FALSE ( b );
   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(cpy) );
}

TEST_F (DateTimeRangeTests, testHasDateAndTime)
{
   TestDataMap::iterator itr;

   for (itr = m_TestData.begin(); itr != m_TestData.end(); ++itr)
   {
      mamaDateTime t = (*itr).first;

      HasDateAndTimeTest(t);
   }
}

void 
DateTimeRangeTests::GettersTest(const mamaDateTime& t, const TestDataItem& item)
{
   mama_u32_t actual;

   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getYear(t, &actual) );
   EXPECT_EQ ( actual, item.year );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getMonth(t, &actual) );
   EXPECT_EQ ( actual, item.month );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getDay(t, &actual) );
   EXPECT_EQ ( actual, item.day );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getHour(t, &actual) );
   EXPECT_EQ ( actual, item.hour );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getMinute(t, &actual) );
   EXPECT_EQ ( actual, item.min );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getSecond(t, &actual) );
   EXPECT_EQ ( actual, item.secs );

   EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getMicrosecond(t, &actual) );
   EXPECT_EQ ( actual, item.micsecs );
}

TEST_F (DateTimeRangeTests, testGettersAndSetters)
{
   TestDataMap::iterator itr;

   for (itr = m_TestData.begin(); itr != m_TestData.end(); ++itr)
   {
      mamaDateTime t = (*itr).first;
      TestDataItem item = (*itr).second;

      GettersTest(t, item);
   }
}
