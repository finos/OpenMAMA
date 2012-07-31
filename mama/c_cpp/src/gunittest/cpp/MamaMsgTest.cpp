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
#include "MamaMsgTest.h"
#include <cstdlib>

using namespace Wombat;

void MamaMsgTestCPP::SetUp(void)
{
    
    m_bridge = Mama::loadBridge(getMiddleware());

    Mama::open();

}

void MamaMsgTestCPP::TearDown(void)
{
}

TEST_F (MamaMsgTestCPP, CreateTest)
{
    MamaMsg *msg = NULL;

    msg = new MamaMsg;
    msg->create();
    
    ASSERT_TRUE(&msg != NULL);

    delete msg;
}

TEST_F (MamaMsgTestCPP, CopyTest)
{  
    MamaMsg* orig = new MamaMsg;
    MamaMsg* copy = new MamaMsg;
    const char* testString      = "test";
    
    orig->create ();
    copy->create ();

    orig->addString (NULL, 101, testString);

    copy->copy (*orig);

    ASSERT_STREQ(orig->toString (), copy->toString ());

    /* Try a second copy */
    copy->copy (*orig);

    /* String compare second copy */
    ASSERT_STREQ(orig->toString (), copy->toString ());

    /* And copy constructor */
    MamaMsg* copy2 = new MamaMsg (*orig);

    ASSERT_STREQ(orig->toString(), copy2->toString());

    /* Clean up here */
    delete orig;
    delete copy;
    delete copy2;
}

TEST_F (MamaMsgTestCPP, CreateFromBufferTest)
{
    const void*       buffer          = NULL;
    size_t            bufferLength    = 0;
    MamaMsg*    msg                   = new MamaMsg;
    MamaMsg*          createMsg       = new MamaMsg; 
    const char*       testString      = "test";

    msg->create ();
    msg->addString (NULL, 100, testString);

    ((const MamaMsg*)msg)->getByteBuffer (buffer, bufferLength);

    createMsg->createFromBuffer (buffer, bufferLength);
    ASSERT_TRUE(msg != NULL);
    ASSERT_STREQ(msg->toString(), createMsg->toString());

    delete msg;
    delete createMsg;
}
