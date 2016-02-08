/* $Id: timertest.cpp,v 1.1.2.1.2.4 2012/08/24 16:12:00 clintonmcdowell Exp $
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
#include "wombat/wConfig.h"
#include "mama/types.h"
#include "mama/timer.h"
#include "mama/queue.h"
#include "mama/transport.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>


class MamaTransportTestC : public ::testing::Test
{	
protected:
    MamaTransportTestC();          
    virtual ~MamaTransportTestC(); 

    virtual void SetUp();        
    virtual void TearDown();    
public:
    MamaTransportTestC *m_this;
    mamaBridge mBridge;
    
};

MamaTransportTestC::MamaTransportTestC()
{
}

MamaTransportTestC::~MamaTransportTestC()
{
}

void MamaTransportTestC::SetUp(void)
{	
    m_this   = this;

    mama_loadBridge (&mBridge, getMiddleware());
    
}

void MamaTransportTestC::TearDown(void)
{
    m_this = NULL;
}

void MAMACALLTYPE tportCB (mamaTransport      tport, 
              mamaTransportEvent event, 
              short              cause, 
              const void*        platformInfo, 
              void*              closure)
{
}

/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */


/*  Description: Allocate memory for a transport, create then destroy it.    
 *
 *  Expected Result: MAMA_STATUS_OK.
 */
TEST_F (MamaTransportTestC, CreateDestroy)
{
    const char* tportName = getTransport();
    mamaTransport tport     = NULL;

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_allocate (&tport));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_create (tport, tportName, mBridge));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_destroy (tport));
}

/*  Description: Create a transport and set it's associated callback.
 *
 *  Expected Result: MAMA_STATUS_OK.
 */
TEST_F (MamaTransportTestC, Callback)
{
    const char* tportName = getTransport();
    mamaTransport tport     = NULL;

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_allocate (&tport));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_create (tport, tportName, mBridge));
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_setTransportCallback (tport, tportCB, m_this));
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_destroy (tport));
}

/*  Description: Create a transport and get it's name.
 *
 *  Expected Result: MAMA_STATUS_OK, testName=tportName=getTransport()
 */
TEST_F (MamaTransportTestC, getName)
{
    const char* tportName = getTransport();
    mamaTransport tport     = NULL;
    const char*   testName  = NULL;

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_allocate (&tport));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_create (tport, tportName, mBridge));
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_getName (tport, &testName));
    
    ASSERT_STREQ (tportName, testName);

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_destroy (tport));
}

/*  Description: Get the middleware associated with a transport.
 *
 *  Expected Result: MAMA_STATUS_OK, mamaTransport_getMiddleware() 
 *                   returns the correct midleware.
 */
TEST_F (MamaTransportTestC, getMiddleware)
{
    const char* tportName = getTransport();
    mamaTransport tport     = NULL;

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_allocate (&tport));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_create (tport, tportName, mBridge));
    
    ASSERT_STREQ (getMiddleware(),
                  mamaTransport_getMiddleware (tport));
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_destroy (tport));
}

/*  Description: Set the description for a newly created transport
 *               then get it again.
 *
 *  Expected Result: MAMA_STATUS_OK. desc=testDesc="description"
 */
TEST_F (MamaTransportTestC, Description)
{
    const char* tportName = getTransport();
    mamaTransport tport     = NULL;
    const char*   desc      = "description";
    const char*   testDesc  = NULL;

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_allocate (&tport));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_create (tport, tportName, mBridge));
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_setDescription (tport, desc));
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_getDescription (tport, &testDesc));

    ASSERT_STREQ (desc, testDesc);
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_destroy (tport));
}

/*  Description:  Set and get the quality invoked for all subscriptions 
 *                using a transport.
 *
 *  Expected Result: MAMA_STATUS_OK, invoke=testInvoke=1
 */
TEST_F (MamaTransportTestC, InvokeQuality)
{
    const char* tportName = getTransport();
    mamaTransport tport      = NULL;
    int           invoke     = 1;
    int           testInvoke = NULL;
   
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_allocate (&tport));
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_create (tport, tportName, mBridge));
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_setInvokeQualityForAllSubscs (tport, invoke));
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_getInvokeQualityForAllSubscs (tport, &testInvoke));

    ASSERT_EQ (invoke, testInvoke);
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_destroy (tport));
}

/*  Description: Get the quality of a newly created transport.  
 *
 *  Expected Result: MAMA_STATUS_OK.
 */
TEST_F (MamaTransportTestC, GetQuality)
{
    const char* tportName = getTransport();
    mamaTransport tport      = NULL;
    mamaQuality   quality    = MAMA_QUALITY_UNKNOWN;
   
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_allocate (&tport));
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_create (tport, tportName, mBridge));
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_getQuality (tport, &quality));
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_destroy (tport));
}

