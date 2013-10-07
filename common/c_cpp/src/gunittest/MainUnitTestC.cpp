/* $Id: MainUnitTestC.cpp,v 1.2.22.2.10.5 2012/09/16 06:45:14 ianbell Exp $
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

/*  Description : This main test ody will create and run the test fixtures
 *                defined in other source files.
 */

#include <cstdlib>
#include <iostream>
#include <gtest/gtest.h>
#include <string>


static void parseCommandLine (int argc, char** argv)
{
    int i = 1;


    for (i = 1; i < argc;)
    {
        {
           // unknown arg 
            i++;
        }
    }
}

int main(int argc, char **argv)
{
    /* create all tests, passing in command line options
     * command line options can be google test command line options
     * e.g. --gtest_filter, --gtest_repeat, etc
     * see google test framework docs for details
     */
    ::testing::InitGoogleTest(&argc, argv);

    parseCommandLine(argc, argv);


    /* run all tests */
    return RUN_ALL_TESTS();

}

