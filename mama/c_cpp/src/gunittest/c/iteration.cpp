/* $Id: iteration.cpp,v 1.1.2.1 2011/09/12 09:41:00 ianbell Exp $
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

static const char* bridges[]={"wmw","avis",""};

class MamaMsgIterateTestC : public ::testing::Test
{




protected:
	MamaMsgIterateTestC () {}
    virtual ~MamaMsgIterateTestC () {}

    virtual void SetUp()
    {
        mamaBridge mMamaBridge;
    	int 		i=0;
    	while (bridges[i]!="")
    	{
    			mama_loadBridge (&mMamaBridge, bridges[i]);
    			i++;
    	};
    	mama_open();
    }

    virtual void TearDown()
    {
    	mama_close();
    }


};

TEST_F (MamaMsgIterateTestC, Callback)
{
    mamaMsg msg         = NULL;
    mama_status status  = MAMA_STATUS_OK;
    const char* testString      = "test";


    status = mamaMsg_create (&msg);
    ASSERT_EQ(status, MAMA_STATUS_OK);
    ASSERT_TRUE(msg != NULL);

    status = mamaMsg_addString (msg, NULL, 101, testString);
    ASSERT_EQ(status, MAMA_STATUS_OK);







    mamaMsg_destroy (msg);
}


TEST_F (MamaMsgIterateTestC, User)
{
    mamaMsg msg         = NULL;
    mama_status status  = MAMA_STATUS_OK;
    const char* testString      = "test";


    status = mamaMsg_create (&msg);
    ASSERT_EQ(status, MAMA_STATUS_OK);
    ASSERT_TRUE(msg != NULL);

    status = mamaMsg_addString (msg, NULL, 101, testString);
    ASSERT_EQ(status, MAMA_STATUS_OK);







    mamaMsg_destroy (msg);
}


