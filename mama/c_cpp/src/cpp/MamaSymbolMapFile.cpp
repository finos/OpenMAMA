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

#include <stdio.h>
#include <string.h>
#include <mama/mamacpp.h>
#include <mama/symbolmapfile.h>

using namespace Wombat;


/**
 * Wrapper for C implementation
 */

struct MamaSymbolMapFile::MamaSymbolMapFileImpl
{
    MamaSymbolMapFileImpl () 
        : mFileMapC (NULL) 
    {}

    mamaSymbolMapFile mFileMapC;
};


MamaSymbolMapFile::MamaSymbolMapFile ()
    : mImpl (*new MamaSymbolMapFileImpl)
{
    mamaSymbolMapFile_create (&mImpl.mFileMapC);
}

MamaSymbolMapFile::~MamaSymbolMapFile ()
{
    mamaSymbolMapFile_destroy (mImpl.mFileMapC);
    delete &mImpl;
}

mama_status MamaSymbolMapFile::load (const char*  mapFileName)
{
    return mamaSymbolMapFile_load (mImpl.mFileMapC, 
                                   mapFileName);
}

void MamaSymbolMapFile::addMap (const char*  fromSymbol,
                                const char*  toSymbol)
{
     mamaSymbolMapFile_addMap (mImpl.mFileMapC, 
                               fromSymbol, 
                               toSymbol);
}

bool MamaSymbolMapFile::map (char*        result,
                             const char*  symbol,
                             size_t       maxLen) const
{
    int rslt = mamaSymbolMapFile_map (mImpl.mFileMapC, 
                                      result, 
                                      symbol, 
                                      maxLen);

    return (rslt == 1 ? true : false);
}

bool MamaSymbolMapFile::revMap (char*        result,
                                const char*  symbol,
                                size_t       maxLen) const
{
    int rslt = mamaSymbolMapFile_revMap (mImpl.mFileMapC, 
                                         result, 
                                         symbol, 
                                         maxLen);
    return (rslt == 1 ? true : false);
}
