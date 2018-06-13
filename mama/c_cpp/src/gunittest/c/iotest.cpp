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


class MamaIoTestC : public ::testing::Test
{	
protected:
    MamaIoTestC();          
    virtual ~MamaIoTestC(); 

    virtual void SetUp();        
    virtual void TearDown ();    
public:
    MamaIoTestC *m_this;
    mamaBridge mBridge;
    
};

MamaIoTestC::MamaIoTestC()
{
}

MamaIoTestC::~MamaIoTestC()
{
}

void MamaIoTestC::SetUp(void)
{	
    m_this = this;

    mama_loadBridge (&mBridge, getMiddleware());
    mama_open ();
}

void MamaIoTestC::TearDown(void)
{
    mama_close ();
    m_this = NULL;
}

void MAMACALLTYPE onIo (mamaIo io, mamaIoType ioType, void* closure)
{
}

/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/*  Description: Create a mamaIo, then destroy it.
 *
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaIoTestC, CreateDestroy)
{
    mamaIo      io         = NULL;
    mamaQueue   queue      = NULL;
    uint32_t    descriptor = 1;
    mamaIoType  ioType     = MAMA_IO_READ;

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_create (&queue, mBridge));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaIo_create (&io, queue, descriptor, onIo, ioType, m_this));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaIo_destroy (io));

    ASSERT_EQ (MAMA_STATUS_OK, mamaQueue_destroy (queue));
}

/*  Description:  Create a mamaIo, get it's descriptor then destory it.
 *
 *  Expected Results:MAMA_STATUS_OK, descriptor=testDescriptor 
 *                  
 */
TEST_F (MamaIoTestC, getDescriptor)
{
    mamaIo      io             = NULL;
    mamaQueue   queue          = NULL;
    uint32_t    descriptor     = 1;
    uint32_t    testDescriptor = 0;
    mamaIoType  ioType         = MAMA_IO_READ;

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaQueue_create (&queue, mBridge));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaIo_create (&io, queue, descriptor, onIo, ioType, m_this));

    ASSERT_EQ (MAMA_STATUS_OK,
               mamaIo_getDescriptor (io, &testDescriptor));
    
    ASSERT_EQ (MAMA_STATUS_OK,
               mamaIo_destroy (io));

    ASSERT_EQ (MAMA_STATUS_OK, mamaQueue_destroy (queue));

    ASSERT_EQ (descriptor, testDescriptor);
}


