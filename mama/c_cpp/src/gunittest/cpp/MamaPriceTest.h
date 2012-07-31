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
#ifndef MAMA3373_H
#define MAMA3373_H

/* ************************************************************************* */
/* Includes */
/* ************************************************************************* */
#include "gtest/gtest.h"
#include "MainUnitTestCpp.h"
#include "mama/MamaPrice.h"

/* ************************************************************************* */
/* Test Fixture */
/* ************************************************************************* */

using namespace Wombat;

class MamaPriceTest : public ::testing::Test
{
	// Member variables
protected:

	// The price under test
	MamaPrice* m_price;

	// Member functions
protected:

	// Construction and Destruction
	MamaPriceTest(void);
	virtual ~MamaPriceTest(void);

	// Setup and teardown functions
	virtual void SetUp(void);
	virtual void TearDown(void);	
};

#endif
