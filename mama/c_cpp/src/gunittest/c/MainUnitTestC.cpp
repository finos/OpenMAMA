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
#include <cstdlib>
#include <iostream>
#include <gtest/gtest.h>
#include <mama/mama.h>
#include <mama/status.h>
#include <string>

#ifndef WOMBAT_CONFIG_NO_NAMESPACES
 using std::string;
#endif

#if !defined(VERSIONSTR)
# define VERSIONSTR    "?.?.?"
# define DATESTR       "date"
# define BUILDSTR      ""
#endif

static string version     ("APPNAMESTR:  Version " VERSIONSTR
                           "  Date " DATESTR "  Build " BUILDSTR);


static const char*       gMiddleware  = "wmw";
static const char*       gPayload     = "wmsg";
static const char        gPayloadId   = 'w';
static const char*       gTransport   = "tport";


const char* getMiddleware (void)
{
    return gMiddleware;
}

const char* getPayload (void)
{
    return gPayload;
}

const char* getTransport (void)
{
    return gTransport;
}

const char getPayloadId (void)
{
    return gPayloadId;
}

static void parseCommandLine (int argc, char** argv)
{
    int i = 1;


    for (i = 1; i < argc;)
    {
        if (strcmp ("-m", argv[i]) == 0)
        {
            gMiddleware = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-p", argv[i]) == 0)
        {
            gPayload = argv[i+1];
            i += 2;
        }
        else if (strcmp ("-tport", argv[i]) == 0)
        {
            gTransport = argv[i+1];
            i += 2;
        }
        else
        {
           //unknown arg
            i++;
        }
    }
}


int main(int argc, char **argv)
{
    // create all tests, passing in command line options
    // command line options can be google test command line options
    // e.g. --gtest_filter, --gtest_repeat, etc
    // see google test framework docs for details
    ::testing::InitGoogleTest(&argc, argv);

    parseCommandLine(argc, argv);


    // run all tests
    return RUN_ALL_TESTS();

}

