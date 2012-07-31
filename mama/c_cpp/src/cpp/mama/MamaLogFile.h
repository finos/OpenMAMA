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

#ifndef MAMA_LOF_FILE_CPP_H__
#define MAMA_LOF_FILE_CPP_H__

#include "mama/mamacpp.h"

namespace Wombat 
{

    /**
     * The <code>MamaLogFile</code> class provides a single interface for 
     * the configuration and control of <code>Mama</code> logging activity.
     * The <code>MamaLogFile</code> class expands upon the existing logging 
     * capabilities of <code>Mama</code> providing the ability to set log
     * file size and configure the number of log files maintained.
     *
     * @see MamaLogFile
     */
    class MAMACPPExpDLL MamaLogFile
    {
    public:

        /**
         * Set the Size of the log files.
         * Has no effect if log file is not successfully created via a 
         * call to MamaLogFile::enableLogging(...).
         *
         * @param newMax log file size.
         */
        static void setMaxSize (unsigned long newMax);

        /**
         * Set the number of log files to be maintained.
         * Has no effect if log file is not successfully created via a 
         * call to MamaLogFile::enableLogging(...).
         *
         * @param newNum number of log files to be maintained.
         */
        static void setNumBackups (unsigned int newNum);

        /**
         * Set the open method for the logfile.
         * 'true' will keep any existing data in the file
         * 'false' will overwrite any existing data in the file
         * @param append append mode (on or off)
         */
        static void setAppendMode (bool append);

        /**
         * Enable logging.
         * Will set log level and attempt to open a file of the named provided.
         * If a file could not be created or parameter file is NULL mama log 
         * output is re-directed to stderr.
         *
         * @param file log file name
         * @param level mama log level
         */
        static void enableLogging (const char* file, MamaLogLevel  level);

        /**
         * Disable logging.
         */
        static void disableLogging( void );

        /**
         * Return true if currently logging to file (via MamaLogFile).
         * Return false if not logging to file (via MamaLogFile).
         */
        static bool loggingToFile( void );

    private:
        /**
         * Utility class. No instances.
         */
        MamaLogFile( void ) {}
    };

} // namespace Wombat
#endif // MAMA_LOF_FILE_CPP_H__
