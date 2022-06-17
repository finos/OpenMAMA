/*
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
#include "MainUnitTestC.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "wombat/port.h"
#include "wombat/timers.h"
#include <sys/types.h>

class CommonPortTestC : public ::testing::Test
{
protected:
	CommonPortTestC();
	virtual ~CommonPortTestC();

	virtual void SetUp();
	virtual void TearDown();
public:
	CommonPortTestC *m_this;
};

CommonPortTestC::CommonPortTestC()
{
	m_this = NULL;
}

CommonPortTestC::~CommonPortTestC()
{
}

void CommonPortTestC::SetUp(void)
{
}

void CommonPortTestC::TearDown(void)
{
}

/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/*  Description: Creates a timerHeap, starts dispatching then destroys the
*               heap
*
*  Expected Result: Functions return zero
*/
TEST_F(CommonPortTestC, strptimeTest)
{
	struct tm result;
	memset(&result, 0, sizeof(struct tm));

	strptime("2001-11-12 18:31:01", "%Y-%m-%d %H:%M:%S", &result);

	// Leaving arithmetic to make it clear the numbers strptime uses
	EXPECT_EQ(2001 - 1900, result.tm_year);
	EXPECT_EQ(11 - 1, result.tm_mon);
	EXPECT_EQ(12, result.tm_mday);
	EXPECT_EQ(18, result.tm_hour);
	EXPECT_EQ(31, result.tm_min);
	EXPECT_EQ(01, result.tm_sec);
}
