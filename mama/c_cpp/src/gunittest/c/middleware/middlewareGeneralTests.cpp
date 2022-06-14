/* $Id: middlewareGeneralTests.cpp,v 1.1.2.7 2013/02/01 17:26:58 matthewmulhern Exp $
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
#include "mama/mama.h"
#include "MainUnitTestC.h"
#include <iostream>
#include "bridge.h"
#include "mama/types.h"

using std::cout;
using std::endl;


class MiddlewareGeneralTests : public ::testing::Test
{
protected:
    MiddlewareGeneralTests(void);
    virtual ~MiddlewareGeneralTests(void);

    virtual void SetUp(void);
    virtual void TearDown(void);

    mamaBridge mBridge;
};

MiddlewareGeneralTests::MiddlewareGeneralTests(void)
{
}

MiddlewareGeneralTests::~MiddlewareGeneralTests(void)
{
}

void MiddlewareGeneralTests::SetUp(void)
{
    mama_loadBridge (&mBridge,getMiddleware());
    mama_open ();
}

void MiddlewareGeneralTests::TearDown(void)
{
    mama_close ();
}



/*===================================================================
 =                      Used in mama.c                              =
 ====================================================================*/

TEST_F (MiddlewareGeneralTests, startInvalid)
{
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, mBridge->bridgeStart(NULL));
}

TEST_F (MiddlewareGeneralTests, stopInvalid)
{
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, mBridge->bridgeStop(NULL));
}

TEST_F (MiddlewareGeneralTests, getVersion)
{
    const char* res = "0.0";

    res = mBridge->bridgeGetVersion();

	ASSERT_TRUE (res != NULL);
}

TEST_F (MiddlewareGeneralTests, getName)
{
    const char* res = "name";

    res = mBridge->bridgeGetName();

	ASSERT_TRUE (res != NULL);
}

TEST_F (MiddlewareGeneralTests, getDefaultPayloadId)
{
    char** names = {NULL};
    char*  id    = NULL;
    
    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge-> bridgeGetDefaultPayloadId(&names,&id));
}

TEST_F (MiddlewareGeneralTests, getDefaultPayloadIdInvalidName)
{
    char* id = NULL;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge-> bridgeGetDefaultPayloadId(NULL,&id));
}

TEST_F (MiddlewareGeneralTests, getDefaultPayloadIdInvalidId)
{
    char ** names;
    
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge-> bridgeGetDefaultPayloadId(&names,NULL));
}

