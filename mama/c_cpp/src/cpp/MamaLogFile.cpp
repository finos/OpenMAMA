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

#include "mama/mamacpp.h"
#include "mamacppinternal.h"
#include <string.h>

namespace Wombat
{
    /******************************************************************************
     * MamaLogFile implementation.
     */

    void MamaLogFile::setMaxSize (unsigned long newMax)
    {
        mamaTry (mama_setLogSize (newMax));
    }

    void MamaLogFile::setNumBackups (unsigned int newNum)
    {
        mamaTry (mama_setNumLogFiles (newNum));

        if (0 == newNum)
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                    "MamaLogFile::setNumBackups(newNum = %d)\n "
                    "\t- Only a single log file will be maintained\n"
                    "\t- Log file will be over written when max size exceeded\n",
                    newNum);
        }
        else
        {
            mama_setLogFilePolicy(LOGFILE_ROLL);
        }
    }

    void MamaLogFile::setAppendMode (bool append)
    {
        if(append)
        {
            mamaTry (mama_setAppendToLogFile (1));
        }
        else
        {
            mamaTry (mama_setAppendToLogFile (0));
        }
    }

    void MamaLogFile::enableLogging (const char* file, MamaLogLevel  level)
    {
        if (file && (strlen(file) > 0))
        {
            mamaTry (mama_logToFile (file, level));
        }
        else
        {
            mamaTry (mama_enableLogging (NULL, level));
        }
    }

    void MamaLogFile::disableLogging (void)
    {
        mamaTry (mama_disableLogging ());
    }

    bool MamaLogFile::loggingToFile (void)
    {
        if (0 == mama_loggingToFile ())
        {
            return false;
        }
        else
        {
            return true;
        }
    }

} // namespace Wombat
