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

#ifndef MAMA_SYMBOL_MAP_FILE_CPP_H__
#define MAMA_SYMBOL_MAP_FILE_CPP_H__

#include "mama/mamacpp.h"

namespace Wombat 
{
    /**
     * MamaSymbolMapFile is a concrete implementation of a symbology map.
     * 
     * It can load a filename and expects the contents of that file to
     * contain two columns of data, with the columns separated by white
     * space.  A matching symbol of the left column is mapped to the
     * symbol in the right column.  If the symbol does not match anything
     * in the file (or the file cannot be found), then the original symbol
     * is used (no mapping).
     */

    class MAMACPPExpDLL MamaSymbolMapFile : public MamaSymbolMap
    {
    public:
        MamaSymbolMapFile          ();

        virtual ~MamaSymbolMapFile ();

        virtual mama_status load   (const char*   mapFileName);

        virtual void addMap        (const char*   fromSymbol,
                                    const char*   toSymbol);

        virtual bool map           (char*         result,
                                    const char*   symbol,
                                    size_t        maxLen) const;

        virtual bool revMap        (char*         result,
                                    const char*   symbol,
                                    size_t        maxLen) const;
    private:
        struct                  MamaSymbolMapFileImpl;
        MamaSymbolMapFileImpl&  mImpl;
    };

} // namespace Wombat
#endif // MAMA_SYMBOL_MAP_FILE_CPP_H__
