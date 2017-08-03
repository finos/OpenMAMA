/* $Id:
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
#include "wombat/strutils.h"
#include "stdlib.h"

class CommonStrutilsTestC : public ::testing::Test
{
protected:
    CommonStrutilsTestC();
    virtual ~CommonStrutilsTestC();

    virtual void SetUp();
    virtual void TearDown ();
public:
};

CommonStrutilsTestC::CommonStrutilsTestC()
{
}

CommonStrutilsTestC::~CommonStrutilsTestC()
{
}

void CommonStrutilsTestC::SetUp(void)
{
}

void CommonStrutilsTestC::TearDown(void)
{
}

/* ************************************************************************* */
/* Test Functions */
/* ************************************************************************* */
TEST_F (CommonStrutilsTestC, replaceEnvironmentVariableValidBrackets)
{
    const char* envVar      = "$(TMPDIR)";
    const char* baseDir     = "/tmp";
    const char* logfileName = "/test/logfile.log";
    char*       outputStr   = NULL;
    char        inputString[30];
    char        expectedStr[30];

    setenv ("TMPDIR", baseDir, 1);

    inputString[0] = '\0';
    strncat (inputString, envVar, 9);
    strncat (inputString, logfileName, 17);

    outputStr = strReplaceEnvironmentVariable (inputString);

    expectedStr[0] = '\0';
    strncat (expectedStr, baseDir, 4);
    strncat (expectedStr, logfileName, 17);

    ASSERT_STREQ (outputStr, expectedStr);

    free (outputStr);
}

TEST_F (CommonStrutilsTestC, replaceEnvironmentVariableValidBracketsAlternative)
{
    const char* envVar      = "${TMPDIR}";
    const char* baseDir     = "/tmp";
    const char* logfileName = "/test/logfile.log";
    char*       outputStr   = NULL;
    char        inputString[30];
    char        expectedStr[30];

    setenv ("TMPDIR", baseDir, 1);

    inputString[0] = '\0';
    strncat (inputString, envVar, 9);
    strncat (inputString, logfileName, 17);

    outputStr = strReplaceEnvironmentVariable (inputString);

    expectedStr[0] = '\0';
    strncat (expectedStr, baseDir, 4);
    strncat (expectedStr, logfileName, 17);

    ASSERT_STREQ (outputStr, expectedStr);

    free (outputStr);
}

TEST_F (CommonStrutilsTestC, replaceEnvironmentVariableValidMixedBrackets)
{
    const char* envVar      = "$(TMPDIR}";
    const char* baseDir     = "/tmp";
    const char* logfileName = "/test/logfile.log";
    char*       outputStr   = NULL;
    char        inputString[30];

    setenv ("TMPDIR", baseDir, 1);

    inputString[0] = '\0';
    strncat (inputString, envVar, 9);
    strncat (inputString, logfileName, 17);

    outputStr = strReplaceEnvironmentVariable (inputString);

    ASSERT_STREQ (outputStr, NULL);

    free (outputStr);
}

TEST_F (CommonStrutilsTestC, replaceEnvironmentVariableValidMixedBracketsReverse)
{
    const char* envVar      = "${TMPDIR)";
    const char* baseDir     = "/tmp";
    const char* logfileName = "/test/logfile.log";
    char*       outputStr   = NULL;
    char        inputString[30];

    setenv ("TMPDIR", baseDir, 1);

    inputString[0] = '\0';
    strncat (inputString, envVar, 9);
    strncat (inputString, logfileName, 17);

    outputStr = strReplaceEnvironmentVariable (inputString);

    ASSERT_STREQ (outputStr, NULL);

    free (outputStr);
}

TEST_F (CommonStrutilsTestC, replaceEnvironmentVariableNoEnvVar)
{
    const char* logfileName = "/test/logfile.log";
    char*       outputStr   = NULL;
    char        inputString[20];

    inputString[0] = '\0';
    strncat (inputString, logfileName, 17);

    outputStr = strReplaceEnvironmentVariable (inputString);

    ASSERT_STREQ (outputStr, NULL);

    free (outputStr);
}

TEST_F (CommonStrutilsTestC, replaceEnvironmentVariableLargeLogfileStr)
{
    const char* envVar      = "$(TMPDIR)";
    const char* baseDir     = "/tmp";
    const char* logfileName = "/test/llllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllogfile.log";
    char*       outputStr   = NULL;
    char        inputString[810];
    char        expectedStr[810];

    setenv ("TMPDIR", baseDir, 1);

    inputString[0] = '\0';
    strncat (inputString, envVar, 9);
    strncat (inputString, logfileName, 795);

    outputStr = strReplaceEnvironmentVariable (inputString);

    expectedStr[0] = '\0';
    strncat (expectedStr, baseDir, 4);
    strncat (expectedStr, logfileName, 795);

    ASSERT_STREQ (outputStr, expectedStr);

    free (outputStr);
}

TEST_F (CommonStrutilsTestC, replaceEnvironmentVariableLargePathStr)
{
    const char* envVar      = "$(TMPDIR)";
    const char* baseDir     = "/tmp";
    const char* logfileName = "/this/is/a/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/really/long/path/to/a/logfile.log";
    char*       outputStr   = NULL;
    char        inputString[540];
    char        expectedStr[540];

    setenv ("TMPDIR", baseDir, 1);

    inputString[0] = '\0';
    strncat (inputString, envVar, 9);
    strncat (inputString, logfileName,515);

    outputStr = strReplaceEnvironmentVariable (inputString);

    expectedStr[0] = '\0';
    strncat (expectedStr, baseDir, 4);
    strncat (expectedStr, logfileName, 515);

    ASSERT_STREQ (outputStr, expectedStr);

    free (outputStr);
}

TEST_F (CommonStrutilsTestC, replaceMultipleEnvironmentVariableValid)
{
    const char* envVar      = "$(TMPDIR)$(TMPLOGFILE)";
    const char* baseDir     = "/tmp";
    const char* logfileName = "/test/logfile.log";
    char*       outputStr   = NULL;
    char        expectedStr[30];

    setenv ("TMPDIR", baseDir, 1);
    setenv ("TMPLOGFILE", logfileName, 1);

    outputStr = strReplaceEnvironmentVariable (envVar);

    expectedStr[0] = '\0';
    strncat (expectedStr, baseDir, 4);
    strncat (expectedStr, logfileName, 17);

    ASSERT_STREQ (outputStr, expectedStr);

    free (outputStr);
}

TEST_F (CommonStrutilsTestC, replaceMultipleEmbeddedEnvironmentVariableValid)
{
    const char* envVar      = "$(TMPDIR)$(TMPLOGFILE)";
    const char* baseDirEx   = "/tmp$(TMPSECONDDIR)";
    const char* baseDir     = "/tmp";
    const char* secondDir   = "/for";
    const char* logfileName = "/test/logfile.log";
    char*       outputStr   = NULL;
    char        expectedStr[30];

    setenv ("TMPDIR", baseDirEx, 1);
    setenv ("TMPSECONDDIR", secondDir, 1);
    setenv ("TMPLOGFILE", logfileName, 1);

    outputStr = strReplaceEnvironmentVariable (envVar);

    expectedStr[0] = '\0';
    strncat (expectedStr, baseDir, 4);
    strncat (expectedStr, secondDir, 4);
    strncat (expectedStr, logfileName, 17);

    ASSERT_STREQ (outputStr, expectedStr);

    free (outputStr);
}

