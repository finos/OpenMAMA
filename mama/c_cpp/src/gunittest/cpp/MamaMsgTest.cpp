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
#include "mama/types.h"
#include "mama/MamaReservedFields.h"
#include "../c/msgimpl.h"
#include <cstdlib>

using namespace Wombat;

void MamaMsgTestCPP::SetUp(void)
{
    m_bridge = Mama::loadBridge(getMiddleware());

    Mama::open();
}

void MamaMsgTestCPP::TearDown(void)
{
    Mama::close();
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

TEST_F (MamaMsgTestCPP, TempCopyOwnerTest)
{
    // getTempCopy: when the message can be modified, getTempCopy returns
    // the message itself, so any changes will be applied to the Msg as well.

    MamaMsg* orig = new MamaMsg;
    MamaMsg* copy1 = NULL;
    MamaMsg* copy2 = NULL;
    const char* testString      = "test";

    orig->create ();
    orig->addString (NULL, 101, testString);

    copy1 = orig->getTempCopy ();
    ASSERT_TRUE(copy1);

    ASSERT_STREQ(orig->toString (), copy1->toString ());

    /* Try a second copy */
    copy2 = orig->getTempCopy ();
    ASSERT_TRUE(copy2);

    /* String compare second copy */
    ASSERT_STREQ(orig->toString (), copy2->toString ());

    ASSERT_EQ(1, orig->getNumFields());

    copy1->clear();
    ASSERT_EQ(0, orig->getNumFields());

    /* Clean up here */
    delete orig;
}

TEST_F (MamaMsgTestCPP, TempCopyNotOwnerTest)
{
    // getTempCopy: when the message cannot be modified, getTempCopy returns
    // a copy of the message and always the same copy.

    const void*       buffer          = NULL;
    size_t            bufferLength    = 0;
    MamaMsg*          msg             = new MamaMsg;
    MamaMsg* orig = new MamaMsg;
    MamaMsg* copy1 = NULL;
    MamaMsg* copy2 = NULL;
    const char* testString      = "test";
    short owner;

    msg->create();
    msg->addString (NULL, 101, testString);
    msg->getByteBuffer (buffer, bufferLength);

    orig->createFromBuffer (buffer, bufferLength);
    mamaMsgImpl_setMessageOwner (orig->getUnderlyingMsg(), 0);
    mamaMsgImpl_getMessageOwner (orig->getUnderlyingMsg(), &owner);
    ASSERT_FALSE(owner);

    copy1 = orig->getTempCopy ();
    ASSERT_TRUE(copy1);
    ASSERT_TRUE(orig != copy1);

    ASSERT_STREQ(testString, copy1->getString(NULL, 101));
    copy1->addString(NULL, 102, "copy1");
    ASSERT_THROW(orig->getString(NULL, 102), MamaStatus);

    /* Try a second copy */
    copy2 = orig->getTempCopy ();
    ASSERT_TRUE(copy2);
    ASSERT_EQ(copy1, copy2);
    ASSERT_STREQ(copy1->getString(NULL, 102), copy2->getString(NULL, 102));

    mamaMsgImpl_setMessageOwner (orig->getUnderlyingMsg(), 1);
    mamaMsgImpl_getMessageOwner (orig->getUnderlyingMsg(), &owner);
    /* Clean up here */
    delete orig;
    delete msg;
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

TEST_F (MamaMsgTestCPP, IteratorTest)
{
    MamaMsg msg;
    msg.create();

    msg.addChar("char", 1, 'a');
    msg.addBoolean("boolean", 2, true);
    msg.addI32("i32", 3, 123);
    msg.addString("string", 4, "hello");

    MamaMsgIterator it;
    msg.begin(it);

    int counter = 0;
    while (*it != NULL)
    {
        mama_fid_t fid = it->getFid();
        ASSERT_TRUE(0 < fid && fid < 5);
        ++it;
        ++counter;
    }
    ASSERT_EQ(4, counter);
}
