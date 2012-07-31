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

#include <mamda/MamdaSymbolSourceFile.h>
#include <mamda/MamdaSymbolSourceHandler.h>
#include <mama/mamacpp.h>
#include "MamdaUtils.h"
#include <string>

using std::string;

namespace Wombat
{

    class MamdaSymbolSourceFile::MamdaSymbolSourceFileImpl 
        : public MamaSymbolSourceCallback
    {
    public:
        MamdaSymbolSourceFileImpl (MamdaSymbolSourceFile& listener, 
                                   const char* fileName);

        virtual ~MamdaSymbolSourceFileImpl() {}

        void onSymbol (MamaSymbolSource* symbolSource,
                       const char*       symbol,
                       void*             closure);

        string                             mFileName;
        MamdaSymbolSourceFile&             mCppSymbolSourceFile;
        MamaSymbolSourceFile*              mSymbolSourceFile;
        string                             mSourcedSymbol;
        vector<MamdaSymbolSourceHandler*>  mSymbolSourceHandlers;
    };

    MamdaSymbolSourceFile::MamdaSymbolSourceFile (const char* fileName)
        : mImpl (*new MamdaSymbolSourceFileImpl(*this, fileName))
    {
    }

    MamdaSymbolSourceFile::~MamdaSymbolSourceFile ()
    {
        delete &mImpl;
    }

    void MamdaSymbolSourceFile::create()
    {
        mImpl.mSymbolSourceFile->create (mImpl.mFileName.c_str(), (void*) this);
    }

    void MamdaSymbolSourceFile::addHandler (MamdaSymbolSourceHandler* handler)
    {
        mImpl.mSymbolSourceHandlers.push_back (handler);
    }

    const char* MamdaSymbolSourceFile::getSourcedSymbol() const
    {
        return mImpl.mSourcedSymbol.c_str();
    }

    MamdaSymbolSourceFile::MamdaSymbolSourceFileImpl::MamdaSymbolSourceFileImpl (
        MamdaSymbolSourceFile&  source,
        const char*             fileName)
        : mFileName            (fileName)
        , mCppSymbolSourceFile (source)
        , mSourcedSymbol       ("")
    {
        mSymbolSourceFile = new MamaSymbolSourceFile();
        mSymbolSourceFile->addHandler (this);

    }

    void MamdaSymbolSourceFile::MamdaSymbolSourceFileImpl::onSymbol (
        MamaSymbolSource*  symbolSource,
        const char*        symbol,
        void*              closure)
    {
        mSourcedSymbol = symbol;
        for (unsigned int i=0; i < mSymbolSourceHandlers.size(); i++)
        {
            mSymbolSourceHandlers[i]->onSymbol (mCppSymbolSourceFile);
        }
    }

} // namespace
