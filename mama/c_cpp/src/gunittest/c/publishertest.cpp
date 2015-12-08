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
#include "mama/io.h"
#include <publisherimpl.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>


class MamaPublisherTestC : public ::testing::Test
{
protected:
    MamaPublisherTestC();          
    virtual ~MamaPublisherTestC(); 

    virtual void SetUp();        
    virtual void TearDown ();    
public:
    MamaPublisherTestC *m_this;
    mamaBridge          mBridge;
    
};

MamaPublisherTestC::MamaPublisherTestC()
{
}

MamaPublisherTestC::~MamaPublisherTestC()
{
}

void MamaPublisherTestC::SetUp(void)
{
    m_this = this;

    mama_loadBridge (&mBridge, getMiddleware());
    
}

void MamaPublisherTestC::TearDown(void)
{
    m_this = NULL;
}


/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/*  Description: Create a mamaPublisher then destroy it.  
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaPublisherTestC, CreateDestroy)
{
    mamaPublisher    publisher = NULL;
    mamaTransport    tport     = NULL;
    const char*      symbol    = "SYM";
    const char*      root      = "ROOT";
    const char*      source    = "SRC";
   
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_allocate (&tport));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_create (tport, getTransport(), mBridge));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPublisher_create (&publisher, tport, source, NULL, NULL));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPublisher_destroy (publisher));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_destroy (tport));
}

/*  Description: Create a mamaPublisher and get its transport
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaPublisherTestC, GetTransport)
{
    mamaPublisher    publisher = NULL;
    mamaTransport    tport     = NULL;
    const char*      symbol    = "SYM";
    const char*      root      = "ROOT";
    const char*      source    = "SRC";
   
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_allocate (&tport));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_create (tport, getTransport(), mBridge));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPublisher_create (&publisher, tport, source, NULL, NULL));

    ASSERT_EQ(tport, mamaPublisherImpl_getTransportImpl (publisher));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPublisher_destroy (publisher));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_destroy (tport));
}

/*  Description: Create a mamaPublisher and mamaMsg, send the msg using 
 *               mamaPublisher then destroy both.
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaPublisherTestC, DISABLED_Send)
{
    mamaPublisher    publisher = NULL;
    mamaTransport    tport     = NULL;
    const char*      symbol    = "SYM";
    const char*      root      = "ROOT";
    const char*      source    = "SRC";
    mamaMsg          msg       = NULL;

    ASSERT_EQ (MAMA_STATUS_OK, mama_open());

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaMsg_create (&msg));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaMsg_addString (msg, symbol, 101, source));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_allocate (&tport));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_create (tport, getTransport(), mBridge));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPublisher_create (&publisher, tport, source, NULL, NULL));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPublisher_send (publisher, msg));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaPublisher_destroy (publisher));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_destroy (tport));

    ASSERT_EQ (MAMA_STATUS_OK, mama_close());
}

