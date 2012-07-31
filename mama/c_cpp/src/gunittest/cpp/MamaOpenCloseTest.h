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
#ifndef MAMA3998_H
#define MAMA3998_H

/* ************************************************************************* */
/* Includes */
/* ************************************************************************* */
#include "gtest/gtest.h"
#include "mama/mamacpp.h"
#include "mama/MamaStatus.h"
#include "MainUnitTestCpp.h"

/* ************************************************************************* */
/* Namespaces */
/* ************************************************************************* */
using namespace Wombat;

/* ************************************************************************* */
/* Test Fixture */
/* ************************************************************************* */

class MamaOpenCloseTest : public ::testing::Test
{
	// Member variables
protected:
 
	/* Work around for problem in gtest where the this pointer can't be accessed
	 * from a test fixture.
	 */
	MamaOpenCloseTest *m_this;

	// Member functions
protected:

	// Construction and Destruction
	MamaOpenCloseTest(void);
	virtual ~MamaOpenCloseTest(void);

	// Setup and teardown functions
	virtual void SetUp(void);
	virtual void TearDown(void);

    // Static functions for C

    class StartBackgroundCallback : public MamaStartCallback
    {
        void onStartComplete(MamaStatus status);
    };

};
//typedef MamaOpenCloseTest * PMamaOpenCloseTest;

#endif
