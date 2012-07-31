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

#ifndef ParseCmdH
#define ParseCmdH

#include <mama/MamaSource.h>
#include <vector>

using std::vector;

using namespace Wombat;

class CommonCommandLineParser
{
public:
    CommonCommandLineParser (int          argc, 
                             const char*  argv[]);
    ~CommonCommandLineParser ();

    MamaSource*                 getSource        ();
    MamaSource*                 getDictSource    ();
    MamaSource*                 getOptionSource  ();
    const char*                 getSymbolMapFile ();
    const vector<const char*>&  getSymbolList    ();
    double                      getThrottleRate  ();
    int                         getNumThreads    ();
    double                      getTimeout       ();
    MamaLogLevel                getSubscLogLevel ();

    bool                        getLogReqResp    ();
    bool                        getUseWorldview  ();
    const char*                 getSymbology     ();
    int                         getChurnRate     ();
    double                      getTimerInterval ();
    const char*                 getLogFileName   ();
    const char*                 getMiddleware    ();
    int                         getPrecision     ();
    int                         getShutdownTime  ();
    bool                        showDeltas       ();
    mamaBridge                  getBridge        ();
    bool                        getSnapshot      ();

    bool                        getOptBool   (char         option);
    bool                        getOptBool   (const char*  option);
    int                         getOptInt    (char         option,
                                              int          defaultValue = 0);
    int                         getOptInt    (const char*  option, 
                                              int          defaultValue = 0);
    const char*                 getOptString (char         option);
    const char*                 getOptString (const char*  option);
    
    const char*                 getQueryArg1 ();
    const char*                 getQueryArg2 ();
    const char*                 getQueryArg3 ();
    const char*                 getQueryArg4 ();
    int                         getQuery     ();
    int                         getQueryType ();
    int                         getQueries   ();
    
private:
    struct CommonCommandLineParserImpl;
    CommonCommandLineParserImpl& mImpl;
};

typedef enum ExampleLogLevel
{
    EXAMPLE_LOG_LEVEL_QUIETEST,
    EXAMPLE_LOG_LEVEL_QUIETER,
    EXAMPLE_LOG_LEVEL_QUIET,
    EXAMPLE_LOG_LEVEL_NORMAL
} ExampleLogLevel;

extern ExampleLogLevel gExampleLogLevel;

#endif
