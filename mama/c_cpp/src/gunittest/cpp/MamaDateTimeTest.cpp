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

/*
 *	This file contains a test harness for MAMA3306. It will test that the
 *  mama_startBackground function will work correctly. The main thread will
 *  write log entries to the console until a number of messages have been
 *  received on the main subscription.
 */

/* ************************************************************************* */
/* Includes */
/* ************************************************************************* */
#include "MamaDateTimeTest.h"
#include <mama/mamacpp.h>

/* ************************************************************************* */
/* Construction and Destruction */
/* ************************************************************************* */

MamaDateTimeTest::MamaDateTimeTest(void)
{
	// Initialise member variables
	m_DateTime = NULL;
}

MamaDateTimeTest::~MamaDateTimeTest(void)
{
}

/* ************************************************************************* */
/* Setup and Teardown */
/* ************************************************************************* */

void MamaDateTimeTest::SetUp(void)
{
	// Create the date time
	//ASSERT_EQ(mamaDateTime_create(&m_DateTime), MAMA_STATUS_OK);
    Mama::loadBridge(getMiddleware());
    Mama::open();

    m_DateTime = new MamaDateTime();
}

void MamaDateTimeTest::TearDown(void)
{
	// Destroy the date time
	//ASSERT_EQ(mamaDateTime_destroy(m_DateTime), MAMA_STATUS_OK);
    Mama::close();
    delete m_DateTime;
}

/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

TEST_F(MamaDateTimeTest, NullArguments)
{
	// Call with a NULL date time
	mama_f64_t seconds = 0;
	ASSERT_EQ(mamaDateTime_getEpochTimeSecondsWithCheck(NULL, &seconds), MAMA_STATUS_NULL_ARG);

	// NULL seconds
	//ASSERT_EQ(mamaDateTime_getEpochTimeSecondsWithCheck(*m_DateTime, NULL), MAMA_STATUS_NULL_ARG);

	// NULL for both
	ASSERT_EQ(mamaDateTime_getEpochTimeSecondsWithCheck(NULL, NULL), MAMA_STATUS_NULL_ARG);
}

TEST_F(MamaDateTimeTest, CompareDates)
{
	// Get todays date in a date time
	MamaDateTime *today = NULL;
    try
    {
        today = new MamaDateTime();
    }
    catch (...)
    {
        ASSERT_TRUE(today != NULL);
    }
	//ASSERT_EQ(mamaDateTime_create(&today), MAMA_STATUS_OK);
    today->setToNow();
    //ASSERT_EQ(mamaDateTime_setToNow(today), MAMA_STATUS_OK);

	// Get the string representation of the data
	char stringDate[100] = "";
	//ASSERT_EQ(mamaDateTime_getAsFormattedString(today, stringDate, 100, "%Y-%m-%d"), MAMA_STATUS_OK);
    today->getAsFormattedString(stringDate, 100, "%Y-%m-%d");

	// Destroy the date
	//ASSERT_EQ(mamaDateTime_destroy(today), MAMA_STATUS_OK);
    delete today;

	// Format a string using today's date and a time, this should be as "2010-07-04 10:00:00.000"
	const char *time = "10:00:00.000000";
	char completeDateTime[100] = "";
	sprintf(completeDateTime, "%s %s", stringDate, time);

	// Set the date from this string
	//ASSERT_EQ(mamaDateTime_setFromString(m_cDateTime, completeDateTime), MAMA_STATUS_OK);
    delete m_DateTime;
    m_DateTime = new MamaDateTime(completeDateTime);    

	// Get the number of seconds
	mama_f64_t completeDateSeconds = 0;
	//ASSERT_EQ(mamaDateTime_getEpochTimeSecondsWithCheck(m_cDateTime, &completeDateSeconds), MAMA_STATUS_OK);
    completeDateSeconds = m_DateTime->getEpochTimeSecondsWithCheck();

	// Set the date using just the time string
	//ASSERT_EQ(mamaDateTime_clear(m_cDateTime), MAMA_STATUS_OK);
    m_DateTime->clear();
	//ASSERT_EQ(mamaDateTime_setFromString(m_cDateTime, time), MAMA_STATUS_OK);
    m_DateTime->setFromString(time);

	// Get the number of seconds from this
	mama_f64_t timeSeconds = 0;
	//ASSERT_EQ(mamaDateTime_getEpochTimeSecondsWithCheck(m_cDateTime, &timeSeconds), MAMA_STATUS_OK);
    timeSeconds = m_DateTime->getEpochTimeSecondsWithCheck();
    
	// These must be the same
	ASSERT_EQ(completeDateSeconds, timeSeconds);
}

TEST_F(MamaDateTimeTest, SetDateTimezoneCheck)
{
    // Test for Windows bug where setting date used localtime
    // This was in port.c where the TZ env var was set to "" rather than "UTC"
    MamaDateTime dt1;
    dt1.set(2015, 8, 23, 0, 0, 0, 0, MAMA_DATE_TIME_PREC_UNKNOWN, NULL);

    MamaDateTime dt2;
    dt2.setDate(2015, 8, 23);

    ASSERT_EQ(dt1.getEpochTimeMicroseconds(), dt2.getEpochTimeMicroseconds());
}

TEST_F(MamaDateTimeTest, GetFromStructTimeVal)
{
    struct timeval          tVal;
    MamaDateTime            dt1;
    mamaDateTimePrecision   precision   = MAMA_DATE_TIME_PREC_MICROSECONDS;
    mamaDateTimeHints       hints       =  MAMA_DATE_TIME_HAS_DATE;

    mama_u32_t secs      = 43219, mSecs = 123, uSecs = (mSecs * 1000) + 456;

    dt1.setWithHints(secs, uSecs, precision, hints);

    dt1.getAsStructTimeVal(tVal);

    ASSERT_EQ ( tVal.tv_sec, secs );
    ASSERT_EQ ( tVal.tv_usec, uSecs );
}

TEST_F(MamaDateTimeTest, SetFromStructTimeVal)
{
    struct timeval  tVal;
    MamaDateTime    dt1;
    const char*     timeStr     = "2017-01-17 17:31:47.123000";
    char            stringBuffer[50];

    /* The following timeval represents the time - "2017-01-17 17:31:47.123000" */
    tVal.tv_sec = 1484674307;
    tVal.tv_usec = 123000;

    dt1.set(tVal);
    dt1.getAsString(stringBuffer, 50);

    ASSERT_STREQ (timeStr, stringBuffer);
}

TEST_F (MamaDateTimeTest, TestGetEpochTimeExtGranularFunctions)
{
    /* The following test represents the time - "3001-01-01 10:03:21" */
	MamaDateTime          dt1;
    int64_t               inSecs       = 32535252201;
    uint32_t              inNSecs      = 123456789;
    mamaDateTimeHints     inHints      = MAMA_DATE_TIME_HAS_DATE;
    mamaDateTimePrecision inPrecision  = MAMA_DATE_TIME_PREC_NANOSECONDS;
    int64_t               outSecs      = 0;
    uint32_t              outNSecs     = 0;
    mamaDateTimeHints     outHints     = 0;
    mamaDateTimePrecision outPrecision = MAMA_DATE_TIME_PREC_UNKNOWN;

    dt1.setEpochTimeExt(inSecs, inNSecs);
    dt1.setHints(inHints);
    dt1.setPrecision(inPrecision);

    outSecs = dt1.getEpochTimeExtSeconds();
    outNSecs = dt1.getEpochTimeExtNanoseconds();
    outHints = dt1.getHints();
    outPrecision = dt1.getPrecision();

    EXPECT_EQ ( inSecs, outSecs );
    EXPECT_EQ ( inNSecs, outNSecs );
    EXPECT_EQ ( inHints, outHints );
    EXPECT_EQ ( inPrecision, outPrecision );

}