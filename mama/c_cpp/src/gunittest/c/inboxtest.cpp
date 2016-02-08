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
#include <cstring>
#include <cstdio>
#include <cstdlib>


class MamaInboxTestC : public ::testing::Test
{	
protected:
    MamaInboxTestC();          
    virtual ~MamaInboxTestC(); 

    virtual void SetUp();        
    virtual void TearDown ();    
public:
    MamaInboxTestC *m_this;
    mamaBridge mBridge;
    
};

MamaInboxTestC::MamaInboxTestC()
{
}

MamaInboxTestC::~MamaInboxTestC()
{
}

void MamaInboxTestC::SetUp(void)
{	
    m_this = this;

    mama_loadBridge (&mBridge, getMiddleware());
}

void MamaInboxTestC::TearDown(void)
{
    m_this = NULL;
}

void MAMACALLTYPE onInboxMsg (mamaMsg msg, void *closure)
{
}

void MAMACALLTYPE onInboxError (mama_status status, void *closure)
{
}

void MAMACALLTYPE onInboxDestroy (mamaInbox inbox, void *closure)
{
}

/* ************************************************************************* */
/* Test Functions                                                            */
/* ************************************************************************* */

/*  Description: Creates and destroys a mamaInbox  
 *
 *  Expected Result: MAMA_STATUS_OK
 */

TEST_F (MamaInboxTestC, CreateDestroy)
{
    mamaInbox     inbox     = NULL;
    mamaTransport tport     = NULL;
	const char* tportName   = getTransport();
    mamaQueue     queue     = NULL;

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_allocate (&tport));
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaTransport_create (tport, tportName, mBridge));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_create (&queue, mBridge));
 
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaInbox_create2 (&inbox, tport, queue, 
                                  onInboxMsg, onInboxError, 
                                  onInboxDestroy, m_this));
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaInbox_destroy (inbox));

    ASSERT_EQ (MAMA_STATUS_OK, mamaQueue_destroy (queue));
    ASSERT_EQ (MAMA_STATUS_OK, mamaTransport_destroy (tport));
}
