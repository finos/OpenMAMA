/* $Id: msgtests.cpp,v 1.1.2.4 2011/09/16 16:20:11 ianbell Exp $
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

#include "mama/mama.h"
#include "mama/msg.h"
#include <gtest/gtest.h>
#include <cstdlib>

#include "MainUnitTestC.h"

class MamaMsgTestC : public ::testing::Test
{


protected:
    MamaMsgTestC () {}
    virtual ~MamaMsgTestC () {}

    virtual void SetUp()
    {
        mamaBridge mMamaBridge;

        mama_loadBridge (&mMamaBridge, getMiddleware());


    	mama_open();
    }

    virtual void TearDown()
    {
    	mama_close();
    }


};


TEST_F (MamaMsgTestC, CreateTest)
{
    mamaMsg msg         = NULL;
    mama_status status  = MAMA_STATUS_OK;
    status = mamaMsg_create (&msg);

    ASSERT_EQ(status, MAMA_STATUS_OK);
    ASSERT_TRUE(msg != NULL);

    mamaMsg_destroy (msg);
}

TEST_F (MamaMsgTestC, CopyTest)
{
    mamaMsg orig = NULL;
    mamaMsg copy = NULL;
    const char* testString      = "test";
    mama_status status          = MAMA_STATUS_OK;

    mamaMsg_create (&orig);
    mamaMsg_create (&copy);

    status = mamaMsg_addString (orig, "101", 101, testString);
    ASSERT_EQ(status, MAMA_STATUS_OK);

    status = mamaMsg_copy (orig, &copy);
    ASSERT_EQ(status, MAMA_STATUS_OK);

    ASSERT_STREQ(mamaMsg_toString (orig), mamaMsg_toString (copy));

    /* Try a second copy */
    status = mamaMsg_copy (orig, &copy);
    ASSERT_EQ(status, MAMA_STATUS_OK);

    /* String compare second copy */
    ASSERT_STREQ(mamaMsg_toString (orig), mamaMsg_toString (copy));

    /* Clean up here */
    mamaMsg_destroy (orig);
    mamaMsg_destroy (copy);
}

TEST_F (MamaMsgTestC, CreateFromBufferTest)
{
    const void* buffer          = NULL;
    mama_size_t bufferLength    = 0;
    mama_status status          = MAMA_STATUS_OK;
    mamaMsg     msg             = NULL;
    mamaMsg     createMsg       = NULL;
    const char* testString      = "test";
	mamaPrice   tempPrice		= NULL;
	mamaDateTime   tempDate		= NULL;


	mamaDateTime_create(&tempDate);
	mamaPrice_create(&tempPrice);
	mamaPrice_setValue(tempPrice, 89.99);
    mamaDateTime_setEpochTimeF64(tempDate, 56.88);

    mamaMsg_create (&msg);
    mamaMsg_addString (msg, "name0", 100, testString);
    mamaMsg_addString (msg, "name1", 101, "");
    mamaMsg_addBool (msg, "name2", 102, true);
    mamaMsg_addChar (msg, "name3", 103, 'A');
    mamaMsg_addI8 (msg, "name4", 104, 8);
    mamaMsg_addU8 (msg, "name5", 105, 18);
    mamaMsg_addI16 (msg, "name6", 106, 160);
    mamaMsg_addU16 (msg, "name7", 107, 1160);
    mamaMsg_addI32 (msg, "name8", 108, 320);
    mamaMsg_addU32 (msg, "name9", 109, 13200);
    mamaMsg_addI64 (msg, "name10", 110, 6400000);
    mamaMsg_addU64 (msg, "name11", 111, 16400000);
    mamaMsg_addF32 (msg, "name12", 112, 32.45678);
    mamaMsg_addF64 (msg, "name13", 113, 64.3333231);
    mamaMsg_addPrice (msg, "name14", 114, tempPrice);
    mamaMsg_addDateTime (msg, "name15", 115, tempDate);

    status = mamaMsg_getByteBuffer (msg, &buffer, &bufferLength);
    ASSERT_EQ(status, MAMA_STATUS_OK);

    status = mamaMsg_createFromByteBuffer (&createMsg, buffer, bufferLength);
    ASSERT_EQ(status, MAMA_STATUS_OK);
    ASSERT_TRUE(msg != NULL);
    ASSERT_STREQ(mamaMsg_toString(msg), mamaMsg_toString(createMsg));

    mamaMsg_destroy (msg);
    mamaMsg_destroy (createMsg);
}
