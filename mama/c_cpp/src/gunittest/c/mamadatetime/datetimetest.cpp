/* $Id: datetimetest.cpp,v 1.1.2.1.2.4 2012/10/09 13:00:08 ianbell Exp $
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

#include "gtest/gtest.h"
#include "mama/mama.h"
#include "mama/timezone.h"
#include "mama/status.h"
#include "MainUnitTestC.h"
#include "mama/datetime.h"
#include <iostream>

class MamaDateTimeTestC : public ::testing::Test
{
protected:

    MamaDateTimeTestC(void);
    virtual ~MamaDateTimeTestC(void);

    virtual void SetUp(void);
    virtual void TearDown(void);
};

MamaDateTimeTestC::MamaDateTimeTestC(void)
{
}

MamaDateTimeTestC::~MamaDateTimeTestC(void)
{
}

void
MamaDateTimeTestC::SetUp(void)
{
}

void
MamaDateTimeTestC::TearDown(void)
{
}

// Test Create function will valid and NULL parameters
TEST_F (MamaDateTimeTestC, TestCreate)
{
    mamaDateTime t = NULL;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

// Test Destroy function will valid and NULL parameters
TEST_F (MamaDateTimeTestC, TestDestroy)
{
    mamaDateTime t, nullTime = NULL; 

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_destroy(nullTime) );
}

// Test Clear function will valid and NULL parameters
TEST_F (MamaDateTimeTestC, TestClear)
{
    mamaDateTime t, nullTime = NULL; 

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_clear(t) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_clear(nullTime) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

// Test ClearDate function will valid and NULL parameters
TEST_F (MamaDateTimeTestC, TestClearDate)
{
    mamaDateTime t, nullTime = NULL; 

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_clearDate(t) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_clearDate(nullTime) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

// Test ClearTime function will valid and NULL parameters
TEST_F (MamaDateTimeTestC, TestClearTime)
{
    mamaDateTime t, nullTime = NULL; 

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_clearTime(t) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_clearTime(nullTime) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

// Test Copy function will valid and NULL parameters
TEST_F (MamaDateTimeTestC, TestCopy)
{
    mamaDateTime t,u, nullTime = NULL; 

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&u) );

    // Test will NULL parameters
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_copy(t, nullTime) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_copy(nullTime, t) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_copy(nullTime, nullTime) );

    // Test wil valid parameters
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_copy(t,u) );
    EXPECT_EQ ( 1, mamaDateTime_equal(t,u) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(u) );
}


// Test Empty function will valid and NULL parameters
TEST_F (MamaDateTimeTestC, TestEmpty)
{
    mamaDateTime t, nullTime = NULL; 

    // Test with NULL paremeters
    EXPECT_EQ ( 1, mamaDateTime_empty(nullTime) );

    // Test with Valid parematers
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( 1, mamaDateTime_empty(t) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

// Test Equal function will valid and NULL parameters
TEST_F (MamaDateTimeTestC, TestEqual)
{
    mamaDateTime t, u, nullTime = NULL; 

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&u) );

    // Test with NULL paremeters
    EXPECT_EQ ( 1, mamaDateTime_equal(nullTime, nullTime) );
    EXPECT_EQ ( 0, mamaDateTime_equal(t, nullTime) );
    EXPECT_EQ ( 0, mamaDateTime_equal(nullTime, t) );
    EXPECT_EQ ( 1, mamaDateTime_equal(t, t) );

    // Test with Valid Parameters
    EXPECT_EQ (MAMA_STATUS_OK, mamaDateTime_setToNow(u) );
    ASSERT_NE ( 1, mamaDateTime_equal(t,u) );
    ASSERT_NE ( 1, mamaDateTime_equal(u,t) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(u) );
}


// Test SetEpochTime with valid and invalid input parameters
TEST_F (MamaDateTimeTestC, TestSetEpochTime)
{
    mamaDateTime t, nullTime = NULL;
    mama_u32_t secs    = 43200,  negSecs    = secs * -1,
               micSecs = 500000, negMicSecs = micSecs * -1,
               secsPerDay = (24 * 60 * 60),
               secsPer2Days = secsPerDay * 2;
    mamaDateTimePrecision precision = MAMA_DATE_TIME_PREC_MICROSECONDS;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );

    // Test with invalid arguments
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setEpochTime(nullTime, secs, micSecs, precision) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setEpochTime(nullTime, negSecs, micSecs, precision) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setEpochTime(nullTime, secs, negMicSecs, precision) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setEpochTime(nullTime, negSecs, negMicSecs, precision) );

    // Test with valid arguments
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTime(t, negSecs, negMicSecs, precision) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTime(t, negSecs, micSecs, precision) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTime(t, secs, negMicSecs, precision) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTime(t, secs, micSecs, precision) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTime(t, secsPerDay, micSecs, precision) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTime(t, secsPer2Days, micSecs, precision) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

// Test Compare method iwth valid and invalid input parameters
TEST_F (MamaDateTimeTestC, TestCompare)
{
    mamaDateTime t, u, nullTime = NULL; 

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&u) );

    EXPECT_EQ ( 0, mamaDateTime_compare(nullTime, nullTime) );
    EXPECT_EQ ( 0, mamaDateTime_compare(t, nullTime) );
    EXPECT_EQ ( 0, mamaDateTime_compare(nullTime, t) );

    EXPECT_EQ (MAMA_STATUS_OK, mamaDateTime_setToNow(u) );
    // t is less than u

    EXPECT_EQ ( -1, mamaDateTime_compare(t,u) );
    ASSERT_NE ( 1, mamaDateTime_compare(t,u) );
    EXPECT_EQ ( 0, mamaDateTime_compare(t, t) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(u) );
}

// Test Set Epoch Time F64 with valid and invalid parameters
TEST_F (MamaDateTimeTestC, TestSetEpochTimeF64)
{
    mamaDateTime t, nullTime = NULL;
    mama_f64_t secs    = 43200,  negSecs    = secs * -1,
               secsPerDay = (24 * 60 * 60),
               secsPer2Days = secsPerDay * 2;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );

    // Test with invalid arguments
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setEpochTimeF64(nullTime, secs) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setEpochTimeF64(nullTime, negSecs) );

    // Test with valid input arguments
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTimeF64(t, secs) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTimeF64(t, negSecs) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTimeF64(t, secsPerDay) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTimeF64(t, secsPer2Days) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

// Test Set Epoch Time Millisecond with valid and invalid parameters
TEST_F (MamaDateTimeTestC, TestSetEpochTimeMilliseconds)
{
    mamaDateTime t, nullTime = NULL;
    mama_u64_t mSecs      = 43200 * 1000,  negMSecs = mSecs * -1,
               mSecsPerDay = (24 * 60 * 60) * 1000,
               mSecsPer2Days = mSecsPerDay * 2;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );

    // Test with invalid arguments
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setEpochTimeMilliseconds(nullTime, mSecs) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setEpochTimeMilliseconds(nullTime, negMSecs) );

    // Test with valid input arguments
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTimeMilliseconds(t, mSecs) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTimeMilliseconds(t, negMSecs) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTimeMilliseconds(t, mSecsPerDay) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTimeMilliseconds(t, mSecsPer2Days) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

// Test Set Epoch Time Microseconds with valid and invalid parameters
TEST_F (MamaDateTimeTestC, TestSetEpochTimeMicroseconds)
{
    mamaDateTime t, nullTime = NULL;
    mama_u64_t m1 = 43200, m2 = 1000, m3 = 1000,
               mSecs       = m1 * m2 * m3,  negMSecs = mSecs * -1,
               mSecsPerDay = (24 * 60 * 60) * m2 * m3,
               mSecsPer2Days = mSecsPerDay * 2;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );

    // Test with invalid arguments
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setEpochTimeMicroseconds(nullTime, mSecs) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setEpochTimeMilliseconds(nullTime, negMSecs) );

    // Test with valid input arguments
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTimeMicroseconds(t, mSecs) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTimeMicroseconds(t, negMSecs) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTimeMicroseconds(t, mSecsPerDay) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTimeMicroseconds(t, mSecsPer2Days) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

// Test Set With Hints with valid and invalid parameters
TEST_F (MamaDateTimeTestC, TestSetWithHints)
{
    mamaDateTime t, nullTime = NULL;
    mama_u64_t  secs = 43200, negSecs = secs * -1,
               micSecs = 500 * 1000, negMicSecs = micSecs * -1,
               secsPerDay = (24 * 60 * 60),
               secsPer2Days = secsPerDay * 2;
    mamaDateTimePrecision precision = MAMA_DATE_TIME_PREC_MICROSECONDS;
    mamaDateTimeHints hints =  MAMA_DATE_TIME_HAS_DATE;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );

    // Test with invalid arguments
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setWithHints(nullTime, secs, micSecs, precision, hints) );

    // Test with valid input arguments
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setWithHints(t, secs, micSecs, precision, hints) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setWithHints(t, secs, negMicSecs, precision, hints) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setWithHints(t, negSecs, micSecs, precision, hints) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setWithHints(t, negSecs, negMicSecs, precision, hints) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setWithHints(t, secsPerDay, micSecs, precision, hints) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setWithHints(t, secsPer2Days, negMicSecs, precision, hints) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

// Test Set Precsiosn with valid and invalid parameters
TEST_F (MamaDateTimeTestC, TestSetPrecision)
{
    mamaDateTime t, nullTime = NULL;
    mamaDateTimePrecision precision = MAMA_DATE_TIME_PREC_MICROSECONDS;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );

    // Test with invalid arguments
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setPrecision(nullTime, precision) );

    // Test with valid arguments
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setPrecision(t, precision) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

// Test Set To Now with valid and invalid parameters
TEST_F (MamaDateTimeTestC, TestSetToNow)
{
    mamaDateTime t, nullTime = NULL;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );

    // Test with invalid arguments
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setToNow(nullTime) );

    // Test with valid arguments
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToNow(t) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}


TEST_F (MamaDateTimeTestC, TestSetToMidnightToday)
{
    mamaDateTime t, nullTime = NULL;
    mamaTimeZone z, nullTimeZone = NULL;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );

    // Test with invalid arguments
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setToMidnightToday(nullTime, nullTimeZone) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setToMidnightToday(nullTime, z) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToMidnightToday(t, z) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToMidnightToday(t, nullTimeZone) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestSetWithPrecisionAndTzTest)
{
    mamaDateTime t, nullTime = NULL;
    mamaTimeZone tz  = mamaTimeZone_utc(), nullTz = NULL;
    mamaDateTimePrecision precision = MAMA_DATE_TIME_PREC_MICROSECONDS;
    // These are all valid values
    mama_u32_t year = 2013, month = 1, day = 7, hour = 9, minute = 21, second = 12, microsecond = 500;

   // These are valid values which will get converted by the underlying implementation.
    mama_u32_t invyear = -2013, invmonth = 13, invday = 47, invhour = 29, invminute = 121, invsecond = 712, invmicrosecond = -500;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );

    // Test with invalid arguments
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setWithPrecisionAndTz(nullTime, year, month, day, hour, minute, 
                                                                            second, microsecond, precision, nullTz) );

    // Test with valid argumetns
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setWithPrecisionAndTz(t, invyear, invmonth, invday, invhour, invminute, 
                                                                            invsecond, invmicrosecond, precision, tz) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setWithPrecisionAndTz(t, year, month, day, hour, minute, 
                                                                   second, microsecond, precision, tz) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestSetTimeTest)
{
    mamaDateTime t, nullTime = NULL;
    // These are all valid values
    mama_u32_t hour = 9, minute = 21, second = 12, microsecond = 500;

   // Larger values, which are converted by the implemenation.
    mama_u32_t invhour = 29, invminute = 121, invsecond = 712, invmicrosecond = -500;

   
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    // Test with invalid arguments
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setTime(nullTime, hour, minute, second, microsecond) );

    // Test with valid arguments
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setTime(t, hour, minute, second, invmicrosecond) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setTime(t, hour, minute, invsecond, microsecond) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setTime(t, hour, invminute, second, microsecond) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setTime(t, invhour, minute, second, microsecond) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setTime(t, hour, minute, second, microsecond) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}


TEST_F (MamaDateTimeTestC, TestSetTimeWithPrecisionTzTest)
{
    mamaDateTime t, nullTime = NULL;
    mamaTimeZone tz  = mamaTimeZone_utc(), nullTz = NULL;
    mamaDateTimePrecision precision = MAMA_DATE_TIME_PREC_MICROSECONDS;
    // These are all valid values
    mama_u32_t hour = 9, minute = 21, second = 12, microsecond = 500;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    // Test with invalid arguments
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setTimeWithPrecisionAndTz(nullTime, hour, minute, second, microsecond, precision, tz) );

    // Test with valid arguments
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setTimeWithPrecisionAndTz(t, hour, minute, second, microsecond, precision, nullTz) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setTimeWithPrecisionAndTz(t, hour, minute, second, microsecond, precision, tz) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestSetDate)
{
    mamaDateTime t, nullTime = NULL;
    mama_u32_t year = 2013,         month = 6,     day = 18,
               invYear = year * -1, invMonth = 15, invDay = 35;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );

    // Test with invalid arguments
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setDate(nullTime, year, month, day) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setDate(t, year, month, invDay) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setDate(t, year, invMonth, day) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setDate(t, invYear, month, day) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setDate(t, year, month, day) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestCopyTime)
{
    mamaDateTime t1, t2, nullTime = NULL;
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t1) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t2) );

    mama_u32_t hour = 10, minute = 6, second = 18, microsecond = 123;

    // Test with valid parameters
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_copyTime(nullTime,nullTime) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_copyTime(t1,nullTime) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_copyTime(nullTime,t2) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToNow(t1) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToNow(t2) );
    
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setTime(t2, hour, minute, second, microsecond) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_copyTime(t1,t2) );
    EXPECT_EQ ( 0, mamaDateTime_equal(t1,t2) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t1) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t2) ); 
}

TEST_F (MamaDateTimeTestC, TestCopyDate)
{
    mamaDateTime t1, t2, nullTime = NULL;
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t1) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t2) );
    mama_u32_t year = 2009, month = 6, day = 18;

    // Test with valid parameters
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_copyDate(nullTime,nullTime) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_copyDate(t1,nullTime) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_copyDate(nullTime,t2) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToNow(t1) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToNow(t2) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setDate(t2, year, month, day) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_copyDate(t1,t2) );
    EXPECT_EQ ( 1, mamaDateTime_equal(t1,t2) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t1) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t2) ); 
}

TEST_F (MamaDateTimeTestC, TestHasTime)
{
    mamaDateTime t, nullTime = NULL;
    mama_bool_t b;
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_hasTime(nullTime, &b) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToNow(t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_hasTime(t, &b) );
    EXPECT_TRUE ( b );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) ); 
}

TEST_F (MamaDateTimeTestC, TestHasDate)
{
    mamaDateTime t, nullTime = NULL;
    mama_bool_t b;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_hasDate(nullTime, &b) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToNow(t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_hasDate(t, &b) );
    EXPECT_TRUE ( b );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) ); 
}

TEST_F (MamaDateTimeTestC, TestAddSeconds)
{
    mamaDateTime t, nullTime = NULL;
    mama_f64_t hour = 3600, minute = 60, second = 1, lessThanOneMin = 20, greaterThanOneMin = 520;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToNow(t) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_addSeconds(nullTime, hour) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_addSeconds(t, hour) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_addSeconds(t, minute) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_addSeconds(t, second) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_addSeconds(t, lessThanOneMin) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_addSeconds(t, greaterThanOneMin) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestAddWholeSeconds)
{
    mamaDateTime t, nullTime = NULL;
    mama_i32_t hour = 3600, minute = 60, second = 1, lessThanOneMin = 20, greaterThanOneMin = 520;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToNow(t) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_addWholeSeconds(nullTime, hour) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_addWholeSeconds(t, hour) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_addWholeSeconds(t, minute) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_addWholeSeconds(t, second) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_addWholeSeconds(t, lessThanOneMin) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_addWholeSeconds(t, greaterThanOneMin) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestAddMicroSeconds)
{
    mamaDateTime t, nullTime = NULL;
    mama_i64_t mic = 1, milli = mic * 1000, sec = milli * 1000, minute = sec * 60, hour = minute * 60, day = hour * 24;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToNow(t) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_addMicroseconds(nullTime, mic) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_addMicroseconds(t, mic) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_addMicroseconds(t, milli) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_addMicroseconds(t, sec) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_addMicroseconds(t, minute) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_addMicroseconds(t, hour) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_addMicroseconds(t, day) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestSetFromString)
{
    mamaDateTime t, nullTime = NULL;

    // Format a string using today's date and a time, 
    // 4th July
    std::string szGood = "2013-07-04 10:03:21.123";
    // 31st June
    std::string szInvalid = "2013-06-31 10:03:21.123";
    // Garbage
    std::string szBad = "13th Aquarius 429 BC";
    // NULL String
    std::string szNull;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setFromString(nullTime, szGood.c_str()) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, szInvalid.c_str()) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, szBad.c_str()) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, szNull.c_str()) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, szGood.c_str()) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestSetFromStringWithTz)
{
    mamaDateTime t, nullTime = NULL;

    mamaTimeZone tz  = mamaTimeZone_utc(), nullTz = NULL;;

    // Format a string using today's date and a time, 
    // 4th July
    std::string szGood = "2013-07-04 10:03:21.123";
    // 31st June
    std::string szInvalid = "2013-06-31 10:03:21.123";
    // Garbage
    std::string szBad = "13th Aquarius 429 BC";
    // NULL String
    std::string szNull;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setFromStringWithTz(nullTime, szGood.c_str(), tz) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromStringWithTz(t, szInvalid.c_str(), nullTz) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromStringWithTz(t, szBad.c_str(), nullTz) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromStringWithTz(t, szNull.c_str(), nullTz) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromStringWithTz(t, szGood.c_str(), nullTz) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromStringWithTz(t, szGood.c_str(), tz) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestSetFromStringBuffer)
{
    mamaDateTime t, nullTime = NULL;
    mama_size_t  bigBuff = 100, smallBuff = 1;

    // Format a string using today's date and a time, 
    // 4th July
    std::string szGood = "2013-07-04 10:03:21.123";
    // 31st June
    std::string szInvalid = "2013-06-31 10:03:21.123";
    // Garbage
    std::string szBad = "13th Aquarius 429 BC";
    // NULL String
    std::string szNull;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setFromStringBuffer(nullTime, szGood.c_str(), bigBuff) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromStringBuffer(t, szInvalid.c_str(), bigBuff) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromStringBuffer(t, szBad.c_str(), bigBuff) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromStringBuffer(t, szNull.c_str(), bigBuff) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromStringBuffer(t, szGood.c_str(), bigBuff) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromStringBuffer(t, szGood.c_str(), smallBuff) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestSetFromStringBufferWIthTz)
{
    mamaDateTime t, nullTime = NULL;
    mama_size_t  bigBuff = 100, smallBuff = 1;
    mamaTimeZone tz  = mamaTimeZone_utc(), nullTz = NULL;;

    // Format a string using today's date and a time, 
    // 4th July
    std::string szGood = "2013-07-04 10:03:21.123";
    // 31st June
    std::string szInvalid = "2013-06-31 10:03:21.123";
    // Garbage
    std::string szBad = "13th Aquarius 429 BC";
    // NULL String
    std::string szNull;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_setFromStringBufferWithTz(nullTime, szGood.c_str(), bigBuff, nullTz) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromStringBufferWithTz(t, szInvalid.c_str(), bigBuff, nullTz) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromStringBufferWithTz(t, szBad.c_str(), bigBuff, nullTz) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromStringBufferWithTz(t, szNull.c_str(), bigBuff, nullTz) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromStringBufferWithTz(t, szGood.c_str(), bigBuff, nullTz) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromStringBufferWithTz(t, szGood.c_str(), smallBuff, nullTz) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromStringBufferWithTz(t, szGood.c_str(), bigBuff, tz) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetEpochTime)
{
    mamaDateTime t, nullTime = NULL;

    mama_u32_t secs    = 43200,  expectedSecs,
               micSecs = 500000, expectedMicSecs;

    mamaDateTimePrecision precision = MAMA_DATE_TIME_PREC_MICROSECONDS,
                          expectedPrecision;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToNow(t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTime(t, secs, micSecs, precision) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getEpochTime(nullTime, &expectedSecs, &expectedMicSecs, &expectedPrecision) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getEpochTime(t, &expectedSecs, &expectedMicSecs, &expectedPrecision) );
    EXPECT_EQ ( secs, expectedSecs );
    EXPECT_EQ ( micSecs, expectedMicSecs );
    EXPECT_EQ ( precision, expectedPrecision );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetEpochTimeWithTz)
{
    mamaDateTime t, nullTime = NULL;

    mama_u32_t secs    = 43200,  expectedSecs,
               micSecs = 500000, expectedMicSecs;

    mamaDateTimePrecision precision = MAMA_DATE_TIME_PREC_MICROSECONDS,
                          expectedPrecision;

    mamaTimeZone tz  = mamaTimeZone_utc(), nullTz = NULL;;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToNow(t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTime(t, secs, micSecs, precision) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getEpochTimeWithTz(nullTime, &expectedSecs, &expectedMicSecs, &expectedPrecision, tz) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getEpochTimeWithTz(t, &expectedSecs, &expectedMicSecs, &expectedPrecision, tz) );
    EXPECT_EQ ( secs, expectedSecs );
    EXPECT_EQ ( micSecs, expectedMicSecs );
    EXPECT_EQ ( precision, expectedPrecision );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetEpochTimeMicroseconds)
{
    mamaDateTime t, nullTime = NULL;

    mama_u32_t secs32    = 43219,  micSecs32 = 500000;
    mama_u64_t secs64    = secs32, micSecs64 = micSecs32,
               expectedMicSecs = (secs64 * 1000000) + micSecs64, actualMicSecs;

    mamaDateTimePrecision precision = MAMA_DATE_TIME_PREC_MICROSECONDS;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToNow(t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTime(t, secs32, micSecs32, precision) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getEpochTimeMicroseconds(nullTime, &actualMicSecs) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getEpochTimeMicroseconds(t, &actualMicSecs) );
    EXPECT_EQ ( actualMicSecs, expectedMicSecs );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetEpochTimeMicrosecondsWithTz)
{
    mamaDateTime t, nullTime = NULL;

    mamaTimeZone tz  = mamaTimeZone_utc();

    mama_u32_t secs32    = 43219,  milSecs32 = 123, micSecs32 = (milSecs32 * 1000) + 456;
    mama_u64_t secs64    = secs32, micSecs64 = micSecs32,
               expectedMicSecs = (secs64 * 1000000) + micSecs64, actualMicSecs;

    mamaDateTimePrecision precision = MAMA_DATE_TIME_PREC_MICROSECONDS;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToNow(t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTime(t, secs32, micSecs32, precision) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getEpochTimeMicrosecondsWithTz(nullTime, &actualMicSecs, tz) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getEpochTimeMicrosecondsWithTz(t, &actualMicSecs, tz) );
    EXPECT_EQ ( actualMicSecs, expectedMicSecs );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetEpochTimeMilliseconds)
{
    mamaDateTime t, nullTime = NULL;

    mama_u32_t secs32    = 43219,  milSecs32 = 123, micSecs32 = (milSecs32 * 1000) + 456;
    mama_u64_t secs64    = secs32, milSecs64 = milSecs32,
               expectedMilSecs = (secs64 * 1000) + milSecs64, actualMilSecs;

    mamaDateTimePrecision precision = MAMA_DATE_TIME_PREC_MICROSECONDS;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToNow(t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTime(t, secs32, micSecs32, precision) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getEpochTimeMilliseconds(nullTime, &actualMilSecs) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getEpochTimeMilliseconds(t, &actualMilSecs) );
    EXPECT_EQ ( actualMilSecs, expectedMilSecs );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetEpochTimeMillisecondsWithTz)
{
    mamaDateTime t, nullTime = NULL;

    mamaTimeZone tz  = mamaTimeZone_utc();

    mama_u32_t secs32    = 43219,  milSecs32 = 123, micSecs32 = (milSecs32 * 1000) + 456;
    mama_u64_t secs64    = secs32, milSecs64 = milSecs32,
               expectedMilSecs = (secs64 * 1000) + milSecs64, actualMilSecs;

    mamaDateTimePrecision precision = MAMA_DATE_TIME_PREC_MICROSECONDS;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToNow(t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTime(t, secs32, micSecs32, precision) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getEpochTimeMillisecondsWithTz(nullTime, &actualMilSecs, tz) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getEpochTimeMillisecondsWithTz(t, &actualMilSecs, tz) );
    EXPECT_EQ ( actualMilSecs, expectedMilSecs );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetEpochTimeSeconds)
{
    mamaDateTime t, nullTime = NULL;

    mama_u32_t secs32    = 43219,  milSecs32 = 123, micSecs32 = (milSecs32 * 1000) + 456;
    mama_f64_t expectedSecs, actualSecs;

    /* The returned value will also take into account the microseconds value, so
     * this should be factored into the expected value calculation.
     */
    expectedSecs = secs32 + (mama_f64_t)(micSecs32 / 1000000.0);

    mamaDateTimePrecision precision = MAMA_DATE_TIME_PREC_MICROSECONDS;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToNow(t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTime(t, secs32, micSecs32, precision) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getEpochTimeSeconds(nullTime, &actualSecs) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getEpochTimeSeconds(t, &actualSecs) );
    EXPECT_EQ ( actualSecs, expectedSecs );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetEpochTimeSecondsWithCheck)
{
    mamaDateTime t, nullTime = NULL;

    mama_u32_t secs32    = 432190,  milSecs32 = 123, micSecs32 = (milSecs32 * 1000) + 456;
    mama_f64_t expectedSecs, actualSecs;

    /* In this case, we pass in a value for the seconds which is greater than
     * those in single day. This ensures that the check for getting the time
     * doesn't try and add the current date, making our expected calculation
     * more straightforward.
     */
    expectedSecs = secs32 + (mama_f64_t)(micSecs32 / 1000000.0);

    mamaDateTimePrecision precision = MAMA_DATE_TIME_PREC_MICROSECONDS;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToNow(t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTime(t, secs32, micSecs32, precision) );

    EXPECT_EQ ( MAMA_STATUS_NULL_ARG, mamaDateTime_getEpochTimeSecondsWithCheck(nullTime, &actualSecs) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getEpochTimeSecondsWithCheck(t, &actualSecs) );
    EXPECT_EQ ( actualSecs, expectedSecs );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetEpochTimeSecondsWithTz)
{
    mamaDateTime t, nullTime = NULL;

    mamaTimeZone tz  = mamaTimeZone_utc();

    mama_u32_t secs32    = 43219,  milSecs32 = 123, micSecs32 = (milSecs32 * 1000) + 456;
    mama_f64_t expectedSecs, actualSecs;

    /* Since we're using the UTC timezone, we don't need to take into account
     * the offset at this point, though the test could be improved to consider
     * it.
     */
    expectedSecs = secs32 + (mama_f64_t)(micSecs32 / 1000000.0);

    mamaDateTimePrecision precision = MAMA_DATE_TIME_PREC_MICROSECONDS;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setToNow(t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setEpochTime(t, secs32, micSecs32, precision) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getEpochTimeSecondsWithTz(nullTime, &actualSecs, tz) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getEpochTimeSecondsWithTz(t, &actualSecs, tz) );
    EXPECT_EQ ( actualSecs, expectedSecs );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetWithHints)
{
    mamaDateTime t, nullTime = NULL;

    mamaDateTimePrecision actualPrecision = MAMA_DATE_TIME_PREC_MICROSECONDS, expectedPrecision;

    mama_u32_t actualSecs      = 43219,  expectedSecs,
               actualMilliSecs = 123,    expectedMilliSecs,
               actualMicroSecs = (actualMilliSecs * 1000) + 456, expectedMicroSecs;

    mamaDateTimeHints actualHints = MAMA_DATE_TIME_HAS_DATE, expectedHints;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );

    // Test with valid input arguments
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setWithHints(t, actualSecs, actualMicroSecs, actualPrecision, actualHints) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getWithHints(nullTime, &expectedSecs, &expectedMicroSecs, &expectedPrecision, &expectedHints) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getWithHints(t, &expectedSecs, &expectedMicroSecs, &expectedPrecision, &expectedHints) );
    EXPECT_EQ ( actualSecs, expectedSecs );
    EXPECT_EQ ( actualMicroSecs, expectedMicroSecs );
    EXPECT_EQ ( actualPrecision, expectedPrecision );
    EXPECT_EQ ( actualHints, expectedHints );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetStructTimeVal)
{
    struct timeval tVal;

    mamaDateTime t, nullTime = NULL;

    mamaDateTimePrecision precision = MAMA_DATE_TIME_PREC_MICROSECONDS;

    mama_u32_t secs      = 43219, mSecs = 123, uSecs = (mSecs * 1000) + 456;

    mamaDateTimeHints hints =  MAMA_DATE_TIME_HAS_DATE;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setWithHints(t, secs, uSecs, precision, hints) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getStructTimeVal(nullTime, NULL) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getStructTimeVal(nullTime, &tVal) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getStructTimeVal(t, NULL) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getStructTimeVal(t, &tVal) );
    EXPECT_EQ ( tVal.tv_sec, secs ); 
    EXPECT_EQ ( tVal.tv_usec, uSecs ); 

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetStructTimeValWithTz)
{
    struct timeval tVal;

    mamaDateTime t, nullTime = NULL;

    mamaTimeZone tz  = mamaTimeZone_utc();

    mamaDateTimePrecision precision = MAMA_DATE_TIME_PREC_MICROSECONDS;

    mama_u32_t secs      = 43219, mSecs = 123, uSecs = (mSecs * 1000) + 456;

    mamaDateTimeHints hints =  MAMA_DATE_TIME_HAS_DATE;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setWithHints(t, secs, uSecs, precision, hints) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getStructTimeValWithTz(nullTime, NULL, tz) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getStructTimeValWithTz(nullTime, &tVal, tz) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getStructTimeValWithTz(t, NULL, tz) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getStructTimeValWithTz(t, &tVal, tz) );
    EXPECT_EQ ( tVal.tv_sec, secs ); 
    EXPECT_EQ ( tVal.tv_usec, uSecs ); 

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetStructTm)
{
    struct tm tM;

    mamaDateTime t, nullTime = NULL;

    int sec = 21, min = 3, hour = 10, mday = 4, mon = 6, year = 113, wday = 4, yday = 184;

    std::string szTime = "2013-07-04 10:03:21.123";

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, szTime.c_str()) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getStructTm(nullTime, NULL) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getStructTm(nullTime, &tM) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getStructTm(t, NULL) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getStructTm(t, &tM) );
    EXPECT_EQ ( sec, tM.tm_sec );
    EXPECT_EQ ( min, tM.tm_min );
    EXPECT_EQ ( hour, tM.tm_hour );
    EXPECT_EQ ( mday, tM.tm_mday );
    EXPECT_EQ ( mon, tM.tm_mon );
    EXPECT_EQ ( year, tM.tm_year );
    EXPECT_EQ ( wday, tM.tm_wday );
    EXPECT_EQ ( yday, tM.tm_yday );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetStructTmWithTz)
{
    struct tm tM;

    mamaDateTime t, nullTime = NULL;

    mamaTimeZone tz  = mamaTimeZone_utc();

    int sec = 21, min = 3, hour = 10, mday = 4, mon = 6, year = 113, wday = 4, yday = 184;

    std::string szTime = "2013-07-04 10:03:21.123";

    mamaDateTimePrecision precision = MAMA_DATE_TIME_PREC_MICROSECONDS;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, szTime.c_str()) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getStructTmWithTz(nullTime, NULL, tz) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getStructTmWithTz(nullTime, &tM, tz) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getStructTmWithTz(t, NULL, tz) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getStructTmWithTz(t, &tM, tz) );
    EXPECT_EQ ( sec, tM.tm_sec );
    EXPECT_EQ ( min, tM.tm_min );
    EXPECT_EQ ( hour, tM.tm_hour );
    EXPECT_EQ ( mday, tM.tm_mday );
    EXPECT_EQ ( mon, tM.tm_mon );
    EXPECT_EQ ( year, tM.tm_year );
    EXPECT_EQ ( wday, tM.tm_wday );
    EXPECT_EQ ( yday, tM.tm_yday );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetAsString)
{
    mamaDateTime t, nullTime = NULL;

    std::string szActualTime = "2013-07-04 10:03:21.123";
    char szBuff[50];

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, szActualTime.c_str()) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getAsString(nullTime, szBuff, 50) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getAsString(t, szBuff, 50) );
    EXPECT_STREQ ( szActualTime.c_str(), szBuff );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetTimeAsString)
{
    mamaDateTime t, nullTime = NULL;

    std::string szActualTime = "2013-07-04 10:03:21.123",
                szExpectedTime = "10:03:21.123";
    char szBuff[50];

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, szActualTime.c_str()) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getTimeAsString(nullTime, szBuff, 50) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getTimeAsString(t, szBuff, 50) );
    EXPECT_STREQ ( szExpectedTime.c_str(), szBuff );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetDateAsString)
{
    mamaDateTime t, nullTime = NULL;

    std::string szActualTime = "2013-07-04 10:03:21.123",
                szExpectedDate = "2013-07-04";
    char szBuff[50];

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, szActualTime.c_str()) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getDateAsString(nullTime, szBuff, 50) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getDateAsString(t, szBuff, 50) );
    EXPECT_STREQ ( szExpectedDate.c_str(), szBuff );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetYear)
{
    mamaDateTime t, nullTime = NULL;

    std::string szActualTime = "2013-07-04 10:03:21.123";

    mama_u32_t act, expected = 2013;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, szActualTime.c_str()) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getYear(nullTime, &act) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getYear(t, &act) );
    EXPECT_EQ ( act, expected );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetMonth)
{
    mamaDateTime t, nullTime = NULL;

    std::string szActualTime = "2013-07-04 10:03:21.123";

    mama_u32_t act, expected = 7;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, szActualTime.c_str()) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getMonth(nullTime, &act) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getMonth(t, &act) );
    EXPECT_EQ ( act, expected );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetDay)
{
    mamaDateTime t, nullTime = NULL;

    std::string szActualTime = "2013-07-04 10:03:21.123";

    mama_u32_t act, expected = 4;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, szActualTime.c_str()) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getDay(nullTime, &act) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getDay(t, &act) );
    EXPECT_EQ ( act, expected );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetHour)
{
    mamaDateTime t, nullTime = NULL;

    std::string szActualTime = "2013-07-04 10:03:21.123";

    mama_u32_t act, expected = 10;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, szActualTime.c_str()) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getHour(nullTime, &act) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getHour(t, &act) );
    EXPECT_EQ ( act, expected );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetMinute)
{
    mamaDateTime t, nullTime = NULL;

    std::string szActualTime = "2013-07-04 10:03:21.123";

    mama_u32_t act, expected = 3;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, szActualTime.c_str()) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getMinute(nullTime, &act) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getMinute(t, &act) );
    EXPECT_EQ ( act, expected );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetSecond)
{
    mamaDateTime t, nullTime = NULL;

    std::string szActualTime = "2013-07-04 10:03:21.123";

    mama_u32_t act, expected = 21;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, szActualTime.c_str()) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getSecond(nullTime, &act) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getSecond(t, &act) );
    EXPECT_EQ ( act, expected );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetMicrosecond)
{
    mamaDateTime t, nullTime = NULL;

    std::string szActualTime = "2013-07-04 10:03:21.123";

    mama_u32_t act, expected = 123000;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, szActualTime.c_str()) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getMicrosecond(nullTime, &act) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getMicrosecond(t, &act) );
    EXPECT_EQ ( act, expected );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestGetDayOfWeek)
{
    mamaDateTime t, nullTime = NULL;

    std::string szActualTime = "2013-07-04 10:03:21.123";

    mamaDayOfWeek act, expected = Thursday;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t, szActualTime.c_str()) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_getDayOfWeek(nullTime, &act) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_getDayOfWeek(t, &act) );
    EXPECT_EQ ( act, expected );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t) );
}

TEST_F (MamaDateTimeTestC, TestDiffSeconds)
{
    mamaDateTime t1, t2, nullTime = NULL;

    std::string szTime1 = "2013-07-04 10:03:21.123",
                szTime2 = "2012-07-04 10:03:21.123";

    mama_f64_t act, expected =  60 * 60 * 24 * 365;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t1) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t2) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t1, szTime1.c_str()) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t2, szTime2.c_str()) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_diffSeconds(nullTime, nullTime, NULL) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_diffSeconds(t1, nullTime, NULL) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_diffSeconds(t1, t2, NULL) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_diffSeconds(t1, t2, &act) );
    EXPECT_EQ ( act, expected );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t1) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t2) );
}

TEST_F (MamaDateTimeTestC, TestDiffSecondsSameDay)
{
    mamaDateTime t1, t2, nullTime = NULL;

    std::string szTime1 = "2013-07-04 10:03:21.123",
                szTime2 = "2013-07-04 12:03:21.123";

    mama_f64_t act, expected =  60 * 60 * 2;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t1) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t2) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t1, szTime1.c_str()) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t2, szTime2.c_str()) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_diffSecondsSameDay(nullTime, nullTime, NULL) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_diffSecondsSameDay(t1, nullTime, NULL) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_diffSecondsSameDay(t1, t2, NULL) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_diffSecondsSameDay(t2, t1, &act) );
    EXPECT_EQ ( act, expected );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t1) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t2) );
}

TEST_F (MamaDateTimeTestC, TestDiffMicroseconds)
{
    mamaDateTime t1, t2, nullTime = NULL;

    std::string szTime1 = "2013-07-04 10:03:21.123",
                szTime2 = "2012-07-04 10:03:21.123";

    mama_i64_t mics = 1000000, dd = 365, hh = 24, mm = 60, ss = 60;
    mama_i64_t act, expected =  dd * hh * mm * ss * mics;

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t1) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_create(&t2) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t1, szTime1.c_str()) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_setFromString(t2, szTime2.c_str()) );

    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_diffMicroseconds(nullTime, nullTime, NULL) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_diffMicroseconds(t1, nullTime, NULL) );
    EXPECT_EQ ( MAMA_STATUS_INVALID_ARG, mamaDateTime_diffMicroseconds(t1, t2, NULL) );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_diffMicroseconds(t1, t2, &act) );
    EXPECT_EQ ( act, expected );

    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t1) );
    EXPECT_EQ ( MAMA_STATUS_OK, mamaDateTime_destroy(t2) );
}

/*  Description:     Get the current time since epoch by passing: 
 *                       1) NULL date-time & valid seconds
 *                       2) Valid date-time & NULL seconds
 *                       3) NULL date-time & NULL seconds
 *                   to mamaDateTime_getEpochTimeSecondsWithCheck();
 *
 *  Expected Result: MAMA_STATUS_NULL_ARG
 */

TEST_F (MamaDateTimeTestC, NullArguments)
{
    mama_f64_t      seconds = 0;
    mamaDateTime    m_DateTime = NULL;
    
    /* Call with a NULL date time */
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mamaDateTime_getEpochTimeSecondsWithCheck (NULL, &seconds));

    /* NULL seconds */
    mamaDateTime_create (&m_DateTime);
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mamaDateTime_getEpochTimeSecondsWithCheck (m_DateTime, NULL));

    /* NULL for both */
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mamaDateTime_getEpochTimeSecondsWithCheck (NULL, NULL));
}

/*  Description:     Get the number of seconds since epoch from:
 *                       1) A string representation of a time
 *                       2) A string representation of a time and the current 
 *                          date
 *                   Compare these strings.
 *
 *  Expected Result: completeDateSeconds = timeSeconds 
 */
TEST_F (MamaDateTimeTestC, CompareDates)
{
    mamaDateTime today                 = NULL;
    char         stringDate[100]       = "";
    const char*  time                  = "10:00:00.000000";
    char         completeDateTime[100] = "";
    mamaDateTime m_cDateTime           = NULL;
    mama_f64_t   completeDateSeconds   = 0;
    mama_f64_t   timeSeconds           = 0;
    
    /* Get todays date in a date time */
    ASSERT_EQ (MAMA_STATUS_OK, 
               mamaDateTime_create (&today));

    ASSERT_EQ (MAMA_STATUS_OK, 
               mamaDateTime_setToNow (today));

    /* Get the string representation of the data */
    ASSERT_EQ (MAMA_STATUS_OK, 
               mamaDateTime_getAsFormattedString (today, stringDate, 100, "%Y-%m-%d"));

    /* Destroy the date */
    ASSERT_EQ (MAMA_STATUS_OK, 
               mamaDateTime_destroy (today));

    /* Format a string using today's date and a time, 
     * this should be as "2010-07-04 10:00:00.000" */
    mamaDateTime_create (&m_cDateTime);
    sprintf (completeDateTime, "%s %s", stringDate, time);

    /* Set the date from this string */
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaDateTime_setFromString (m_cDateTime, completeDateTime));

    /* Get the number of seconds */
    ASSERT_EQ (MAMA_STATUS_OK, 
               mamaDateTime_getEpochTimeSecondsWithCheck (m_cDateTime, &completeDateSeconds));

    /* Set the date using just the time string */
    ASSERT_EQ (MAMA_STATUS_OK, mamaDateTime_clear (m_cDateTime));
    ASSERT_EQ (MAMA_STATUS_OK, mamaDateTime_setFromString (m_cDateTime, time));

    /* Get the number of seconds from this */
    ASSERT_EQ (MAMA_STATUS_OK, mamaDateTime_getEpochTimeSecondsWithCheck (m_cDateTime, &timeSeconds));
    
    /* These must be the same */
    ASSERT_EQ (completeDateSeconds, timeSeconds);
}
