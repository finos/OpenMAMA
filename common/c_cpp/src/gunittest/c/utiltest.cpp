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
#include <wombat/strutils.h>
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

    const char* wombatPath = environment_getVariable("WOMBAT_PATH");
    ASSERT_NE (nullptr, wombatPath);

    /* This should be successful in test environment */
    EXPECT_EQ(1,
        fileUtils_findFileInPathList (filePath,
                                      sizeof(filePath),
                                      "mama.properties",
                                      wombatPath,
                                      ":;"));

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
            ":;"));
}

TEST_F(CommonUtilTestC, getVerInfoFromString)
{
    versionInfo v;
    EXPECT_EQ(1, strToVersionInfo("1.2.3extra", &v));
    EXPECT_EQ(1, v.mMajor);
    EXPECT_EQ(2, v.mMinor);
    EXPECT_EQ(3, v.mRelease);
    EXPECT_STREQ("extra", v.mExtra);
}

TEST_F(CommonUtilTestC, getVersionInfoFromInvalidString)
{
    versionInfo v;
    EXPECT_EQ(0, strToVersionInfo("stringwithnonumbers", &v));
}

TEST_F(CommonUtilTestC, getVersionInfoFromNullString)
{
    versionInfo v;
    EXPECT_EQ(0, strToVersionInfo(NULL, &v));
}

TEST_F(CommonUtilTestC, getVersionInfoFromNullVersion)
{
    EXPECT_EQ(0, strToVersionInfo("1.2.3", NULL));
}

TEST_F(CommonUtilTestC, getVersionInfoFromPrefixedString)
{
    versionInfo v;
    EXPECT_EQ(1, strToVersionInfo("somestuff1.2.3extra", &v));
    EXPECT_EQ(1, v.mMajor);
    EXPECT_EQ(2, v.mMinor);
    EXPECT_EQ(3, v.mRelease);
    EXPECT_STREQ("extra", v.mExtra);

}

TEST_F(CommonUtilTestC, getVersionInfoFromSuffixedString)
{
    versionInfo v;
    EXPECT_EQ(1, strToVersionInfo("1.2.3extra but. not this", &v));
    EXPECT_EQ(1, v.mMajor);
    EXPECT_EQ(2, v.mMinor);
    EXPECT_EQ(3, v.mRelease);
    EXPECT_STREQ("extra but", v.mExtra);
}

