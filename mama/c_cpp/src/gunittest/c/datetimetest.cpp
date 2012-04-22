/* Text editors: this is -*-C-*- code. */
/*----------------------------------------------------------------------------
 *
 * $Id$
 *
 * Copyright (c) 1999-2007 Wombat Financial Software Inc., of Incline
 * Village, NV.  All rights reserved.
 * 
 * This software and documentation constitute an unpublished work and
 * contain valuable trade secrets and proprietary information belonging
 * to Wombat.  None of this material may be copied, duplicated or
 * disclosed without the express written permission of Wombat.  
 * 
 * WOMBAT EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES CONCERNING THIS
 * SOFTWARE AND DOCUMENTATION, INCLUDING ANY WARRANTIES OF
 * MERCHANTABILITY AND/OR FITNESS FOR ANY PARTICULAR PURPOSE, AND
 * WARRANTIES OF PERFORMANCE, AND ANY WARRANTY THAT MIGHT OTHERWISE ARISE
 * FROM COURSE OF DEALING OR USAGE OF TRADE. NO WARRANTY IS EITHER
 * EXPRESS OR IMPLIED WITH RESPECT TO THE USE OF THE SOFTWARE OR
 * DOCUMENTATION.  
 * 
 * Under no circumstances shall Wombat be liable for incidental, special,
 * indirect, direct or consequential damages or loss of profits,
 * interruption of business, or related expenses which may arise from use
 * of software or documentation, including but not limited to those
 * resulting from defects in software and/or documentation, or loss or
 * inaccuracy of data of any kind.
 *
 *--------------------------------------------------------------------------*/

#include "gtest/gtest.h"
#include "mama/mama.h"
#include "mama/status.h"
#include "MainUnitTestC.h"
#include "mama/types.h"
#include "mama/datetime.h"

class MamaDateTimeTestC : public ::testing::Test
{
protected:
    // This C date time will be used for testing
    mamaDateTime m_DateTime;

    /* Work around for problem in gtest where the this pointer can't be accessed
     * from a test fixture.
     */
    MamaDateTimeTestC *m_this;

    MamaDateTimeTestC(void);
    virtual ~MamaDateTimeTestC(void);

    virtual void SetUp(void);
    virtual void TearDown(void);

};

MamaDateTimeTestC::MamaDateTimeTestC(void)
{
	m_DateTime = NULL;
}

MamaDateTimeTestC::~MamaDateTimeTestC(void)
{
}

void MamaDateTimeTestC::SetUp(void)
{
	ASSERT_EQ(mamaDateTime_create(&m_DateTime), MAMA_STATUS_OK);
}

void MamaDateTimeTestC::TearDown(void)
{
	ASSERT_EQ(mamaDateTime_destroy(m_DateTime), MAMA_STATUS_OK);
}

/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

TEST_F(MamaDateTimeTestC, NullArguments)
{
	// Call with a NULL date time
	mama_f64_t seconds = 0;
	ASSERT_EQ(mamaDateTime_getEpochTimeSecondsWithCheck(NULL, &seconds), MAMA_STATUS_NULL_ARG);

	// NULL seconds
	ASSERT_EQ(mamaDateTime_getEpochTimeSecondsWithCheck(m_DateTime, NULL), MAMA_STATUS_NULL_ARG);

	// NULL for both
	ASSERT_EQ(mamaDateTime_getEpochTimeSecondsWithCheck(NULL, NULL), MAMA_STATUS_NULL_ARG);
}

TEST_F(MamaDateTimeTestC, CompareDates)
{
	// Get todays date in a date time
	mamaDateTime today = NULL;
	
    ASSERT_EQ(mamaDateTime_create(&today), MAMA_STATUS_OK);
    ASSERT_EQ(mamaDateTime_setToNow(today), MAMA_STATUS_OK);

	// Get the string representation of the data
	char stringDate[100] = "";
	ASSERT_EQ(mamaDateTime_getAsFormattedString(today, stringDate, 100, "%Y-%m-%d"), MAMA_STATUS_OK);

	// Destroy the date
	ASSERT_EQ(mamaDateTime_destroy(today), MAMA_STATUS_OK);

	// Format a string using today's date and a time, this should be as "2010-07-04 10:00:00.000"
	const char *time = "10:00:00.000000";
	char completeDateTime[100] = "";
    mamaDateTime m_cDateTime;
	sprintf(completeDateTime, "%s %s", stringDate, time);

	// Set the date from this string
	ASSERT_EQ(mamaDateTime_setFromString(m_cDateTime, completeDateTime), MAMA_STATUS_OK);

	// Get the number of seconds
	mama_f64_t completeDateSeconds = 0;
	ASSERT_EQ(mamaDateTime_getEpochTimeSecondsWithCheck(m_cDateTime, &completeDateSeconds), MAMA_STATUS_OK);

	// Set the date using just the time string
	ASSERT_EQ(mamaDateTime_clear(m_cDateTime), MAMA_STATUS_OK);
	
    ASSERT_EQ(mamaDateTime_setFromString(m_cDateTime, time), MAMA_STATUS_OK);

	// Get the number of seconds from this
	mama_f64_t timeSeconds = 0;
	ASSERT_EQ(mamaDateTime_getEpochTimeSecondsWithCheck(m_cDateTime, &timeSeconds), MAMA_STATUS_OK);
    
	// These must be the same
	ASSERT_EQ(completeDateSeconds, timeSeconds);
}

