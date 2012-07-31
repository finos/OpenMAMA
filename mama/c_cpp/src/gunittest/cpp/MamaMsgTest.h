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
#ifndef MAMAMSGTEST_H
#define MAMAMSGTEST_H

/* ************************************************************************* */
/* Includes */
/* ************************************************************************* */
#include "gtest/gtest.h"
#include "mama/mamacpp.h"
#include "MainUnitTestCpp.h"
#include "mama/MamaMsg.h"

/* ************************************************************************* */
/* Test */
/* ************************************************************************* */
using namespace Wombat;

class MamaMsgTestCPP : public ::testing::Test
{
protected:

	//MamaMsg m_msg;
    mamaBridge m_bridge;
    

protected:

	MamaMsgTestCPP(void) {};
	virtual ~MamaMsgTestCPP(void) {};

	virtual void SetUp(void);
	virtual void TearDown(void);	
};

#endif
