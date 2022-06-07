/* $Id: middlewareIoTests.cpp,v 1.1.2.5 2012/12/13 11:18:44 matthewmulhern Exp $
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


class MiddlewareIoTests : public ::testing::Test
{
protected:
    MiddlewareIoTests(void);
    virtual ~MiddlewareIoTests(void);

    virtual void SetUp(void);
    virtual void TearDown(void);

    mamaBridge mBridge;
};

MiddlewareIoTests::MiddlewareIoTests(void)
{
}

MiddlewareIoTests::~MiddlewareIoTests(void)
{
}

void MiddlewareIoTests::SetUp(void)
{
    mama_loadBridge (&mBridge,getMiddleware());
    mama_open();
}

void MiddlewareIoTests::TearDown(void)
{
    mama_close();
}

void MAMACALLTYPE onMwIo(mamaIo io, mamaIoType ioType, void* closure)
{
}

/*===================================================================
 =               mamaIo bridge functions                             =
 ====================================================================*/
TEST_F (MiddlewareIoTests, create)
{
    ioBridge   io                = NULL;
    uint32_t   descriptor        = 0;
    mamaIoType ioType            = MAMA_IO_READ;
    mamaIo     parent            = NULL;
    void*      closure           = NULL;
    mamaQueue  queue             = NULL;

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaQueue_create(&queue, mBridge));

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaIo_create(&parent, queue, descriptor, onMwIo, ioType, closure));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaIoCreate(&io,queue, descriptor,
                                            onMwIo, ioType, parent, closure));
    ASSERT_EQ(MAMA_STATUS_OK, mBridge->bridgeMamaIoDestroy(io));

    ASSERT_EQ(MAMA_STATUS_OK, mamaIo_destroy(parent));
    ASSERT_EQ(MAMA_STATUS_OK, mamaQueue_destroy(queue));
}

TEST_F (MiddlewareIoTests, createInvalidResult)
{
    uint32_t   descriptor        =              0;
    mamaIoType ioType            =              MAMA_IO_READ;
    mamaIo     parent            = (mamaIo)     NOT_NULL;
    void*      closure           =              NOT_NULL;
    mamaQueue  queue             = (mamaQueue)  NOT_NULL;

    ASSERT_EQ(MAMA_STATUS_NULL_ARG,
              mBridge->bridgeMamaIoCreate(NULL, queue, descriptor,
                                            onMwIo, ioType, parent, closure));
}

/* THIS WAS COMMENTED OUT BECAUSE mamaIoType CANNOT BE CAST AS NULL.
TEST_F (MiddlewareIoTests, createInvalidType)
{
    ioBridge   io                = (ioBridge)   NOT_NULL;
    uint32_t   descriptor        =              0;
    mamaIo     parent            = (mamaIo)     NOT_NULL;
    void*      closure           =              NOT_NULL;
    mamaQueue  queue             = (mamaQueue)  NOT_NULL;

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaIoCreate(&io,queue, descriptor,
                                          onIo,(mamaIoType)* NULL, parent, closure));
}
*/

TEST_F (MiddlewareIoTests, getDescriptor)
{
    ioBridge   io                = NULL;
    uint32_t   descriptor        = 3;
    mamaIoType ioType            = MAMA_IO_READ;
    mamaIo     parent            = NULL;
    void*      closure           = NULL;
    mamaQueue  queue             = NULL;
    uint32_t   result            = 0;

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaQueue_create(&queue, mBridge));

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaIo_create(&parent, queue, descriptor, onMwIo, ioType, closure));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaIoCreate(&io,queue, descriptor,
                                            onMwIo, ioType, parent, closure));

    ASSERT_EQ (MAMA_STATUS_OK, 
               mBridge->bridgeMamaIoGetDescriptor(io,&result));

    ASSERT_EQ(MAMA_STATUS_OK, mBridge->bridgeMamaIoDestroy(io));

    ASSERT_EQ(MAMA_STATUS_OK, mamaIo_destroy(parent));

    ASSERT_EQ (descriptor, result);

    ASSERT_EQ(MAMA_STATUS_OK, mamaQueue_destroy(queue));
}

TEST_F (MiddlewareIoTests, getDescriptorInvalidIoBridge)
{
    uint32_t result = 0;
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaIoGetDescriptor(NULL,&result));
}

TEST_F (MiddlewareIoTests, getDescriptorInvalidResult)
{
    ioBridge io = (ioBridge) NOT_NULL;

    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaIoGetDescriptor(io,NULL));
}

TEST_F (MiddlewareIoTests, createDestroy)
{
    ioBridge   io                = NULL;
    uint32_t   descriptor        = 0;
    mamaIoType ioType            = MAMA_IO_READ;
    mamaIo     parent            = NULL;
    void*      closure           = NULL;
    mamaQueue  queue             = NULL;

    ASSERT_EQ(MAMA_STATUS_OK, mamaQueue_create(&queue, mBridge));

    ASSERT_EQ(MAMA_STATUS_OK,
              mamaIo_create(&parent, queue, descriptor, onMwIo, ioType, closure));

    ASSERT_EQ(MAMA_STATUS_OK,
              mBridge->bridgeMamaIoCreate(&io,queue, descriptor,
                                            onMwIo, ioType, parent, closure));

    ASSERT_EQ (MAMA_STATUS_OK, mBridge->bridgeMamaIoDestroy(io));

    ASSERT_EQ(MAMA_STATUS_OK, mamaIo_destroy(parent));

    ASSERT_EQ(MAMA_STATUS_OK, mamaQueue_destroy(queue));

}

TEST_F (MiddlewareIoTests, destroyInvalid)
{
    ASSERT_EQ (MAMA_STATUS_NULL_ARG, 
               mBridge->bridgeMamaIoDestroy(NULL));
}
