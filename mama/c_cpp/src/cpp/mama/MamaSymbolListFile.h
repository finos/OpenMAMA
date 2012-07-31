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

#ifndef MAMA_SYMBOL_LIST_FILE_CPP_H__
#define MAMA_SYMBOL_LIST_FILE_CPP_H__

#include <mama/MamaSymbolList.h>

namespace Wombat 
{

    /**
     * MamaSymbolListFile is a file based symbol list with the ability to
     * detect external changes to the file.  
     * 
     * The file's structure is as follows:
     *   - One symbol per line.
     *   - Whitespace-separated columns for the symbol, the source and the 
     *     MAMA transport name.
     *   - The source and transport names are optional; if the transport
     *     name is present then so must be the source name.
     *
     * For example:
     * IBM   CTA    default
     * DIS   CTA    default
     * MSFT  NASDAQ default
     */

    class MAMACPPExpDLL MamaSymbolListFile : public MamaSymbolList
    {
    public:
        MamaSymbolListFile();
        ~MamaSymbolListFile();

        /**
         * Set the file name.
         */
        void setFileName (const char*  fileName);

        /**
         * Set the bridge to use for creating transports
         */
        void setBridge (const mamaBridge bridge);
        
        /**
         * Read the contents of the file into the symbol list.
         */
        void readFile ();

        /**
         * Write the symbol list to the file.
         */
        void writeFile ();

        /**
         * Set a timer to check whether the file has changed its contents.
         * If the contents have changed, various add/remove membership
         * events may be applied to the MamaSymbolList.
         */
        void setFileMonitor (double      checkSeconds,
                             MamaQueue*  queue);

    private:
        struct MamaSymbolListFileImpl;
        MamaSymbolListFileImpl* myPimpl;
    };

} // namespace Wombat

#endif
