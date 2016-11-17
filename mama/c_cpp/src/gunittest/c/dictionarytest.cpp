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


#include <gtest/gtest.h>
#include <wombat/fileutils.h>
#include "MainUnitTestC.h"
#include "wombat/wConfig.h"
#include "mama/mama.h"
#include "mama/status.h"
#include "mama/types.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>


class MamaDictionaryTestC : public ::testing::Test
{
protected:
    MamaDictionaryTestC();          
    virtual ~MamaDictionaryTestC(); 

    virtual void SetUp();        
    virtual void TearDown ();    
public:
    MamaDictionaryTestC *m_this;
    mamaBridge mBridge;
};

MamaDictionaryTestC::MamaDictionaryTestC()
{
}

MamaDictionaryTestC::~MamaDictionaryTestC()
{
}

void MamaDictionaryTestC::SetUp(void)
{
    mama_loadBridge (&mBridge, getMiddleware());
    mama_open (); 
}

void MamaDictionaryTestC::TearDown(void)
{
    mama_close ();
}

/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

/*  Description: Create a mamaDictionary from a file and write to a msg.
 *  The dictionary file includes a field type of UNKNOWN, which is logged with name and fid.
 *                   
 *  Expected Result: MAMA_STATUS_OK
 */
TEST_F (MamaDictionaryTestC, LoadFromFileAndWriteToMsg)
{
    mamaDictionary dictionary;
    mamaMsg msg;
    char buf[1024];
    size_t dictionarySize;
    size_t msgSize;
    
    ASSERT_EQ (MAMA_STATUS_OK, mamaDictionary_create(&dictionary));

    ASSERT_EQ (1, fileUtils_findFileInPathList(buf, sizeof(buf), "dictionary1.txt", getenv("WOMBAT_PATH"), NULL));

    ASSERT_EQ (MAMA_STATUS_OK, mamaDictionary_populateFromFile(dictionary, buf));

    ASSERT_EQ (MAMA_STATUS_OK, mamaDictionary_getDictionaryMessage(dictionary, &msg));

    ASSERT_EQ (MAMA_STATUS_OK, mamaDictionary_getSize(dictionary, &dictionarySize));

    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_getNumFields(msg, &msgSize));

    /* Expect one less in msg since there is an unknown in the dictionary */
    ASSERT_EQ (dictionarySize-1, msgSize);

    ASSERT_EQ (MAMA_STATUS_OK, mamaMsg_destroy(msg));

    ASSERT_EQ (MAMA_STATUS_OK, mamaDictionary_destroy(dictionary));
}

