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
#ifndef MAMA3257_H
#define MAMA3257_H

/* ************************************************************************* */
/* Includes */
/* ************************************************************************* */
#include "gtest/gtest.h"
#include "MainUnitTestCpp.h"
#include "mama/types.h"
#include "mama/MamaDateTime.h"

/* ************************************************************************* */
/* Test Fixture */
/* ************************************************************************* */

using namespace Wombat;

class MamaDateTimeTest : public ::testing::Test
{
	// Member variables
protected:
	
	// This C date time will be used for testing
	//mamaDateTime m_cDateTime;
    MamaDateTime* m_DateTime;

	/* Work around for problem in gtest where the this pointer can't be accessed
	 * from a test fixture.
	 */
	MamaDateTimeTest *m_this;

	// Member functions
protected:

	// Construction and Destruction
	MamaDateTimeTest(void);
	virtual ~MamaDateTimeTest(void);

	// Setup and teardown functions
	virtual void SetUp(void);
	virtual void TearDown(void);

};
typedef MamaDateTimeTest * PMamaDateTimeTest;

#endif
