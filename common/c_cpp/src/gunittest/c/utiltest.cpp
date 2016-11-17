/* $Id: queuetest.cpp,v 1.1.2.1 2012/11/21 10:03:55 matthewmulhern Exp $
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
#include <wombat/fileutils.h>
#include <wombat/environment.h>
#include <stdio.h>


class CommonUtilTestC : public ::testing::Test
{	
protected:
    CommonUtilTestC();
    virtual ~CommonUtilTestC();

    virtual void SetUp();        
    virtual void TearDown ();    
public:
};

CommonUtilTestC::CommonUtilTestC()
{
}

CommonUtilTestC::~CommonUtilTestC()
{
}

void CommonUtilTestC::SetUp(void)
{	
}

void CommonUtilTestC::TearDown(void)
{
}

/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */

TEST_F (CommonUtilTestC, findFileInPath)
{
    char filePath[1024];

    /* This should be successful in test environment */
    EXPECT_EQ(1,
        fileUtils_findFileInPathList (filePath,
                                      sizeof(filePath),
                                      "mama.properties",
                                      environment_getVariable("WOMBAT_PATH"),
                                      NULL));

}

TEST_F(CommonUtilTestC, findFileInPathNotExist)
{
    char filePath[1024];

    /* This should be successful in test environment */
    EXPECT_EQ(0,
        fileUtils_findFileInPathList(filePath,
            sizeof(filePath),
            "mama.invisible_man",
            environment_getVariable("WOMBAT_PATH"),
            NULL));
}
