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

#include <mama/MamaSymbolList.h>
#include <mama/MamaSymbolListMember.h>
#include <mama/mamacpp.h>
#include <mama/symbollistmember.h>
#include <mama/symbollist.h>
#include "mamacppinternal.h"

namespace Wombat 
{

    struct MamaSymbolListMember::MamaSymbolListMemberImpl
    {
        MamaSymbolListMemberImpl()
            : myCimpl       (NULL)
            , myTransport   (NULL)
            , myClosure     (NULL)
            , myDeleteTport (false) 
        {}

        mamaSymbolListMember  myCimpl;
        MamaTransport*        myTransport;
        void*                 myClosure;
        MamaSymbolList*       mySymbolList;
        bool                  myDeleteTport;
    };

    MamaSymbolListMember::MamaSymbolListMember (MamaSymbolList* symbolList)
        : myPimpl (new MamaSymbolListMemberImpl)
    {
        mamaSymbolListMember_allocate (symbolList->getCValue(), &myPimpl->myCimpl);

        // Note: the closure to the C implementation is the C++ object.
        mamaSymbolListMember_setClosure (myPimpl->myCimpl, this);
        myPimpl->mySymbolList = symbolList;
    }

    MamaSymbolListMember::MamaSymbolListMember(MamaSymbolList       *symbolList,
                                               mamaSymbolListMember member)
        : myPimpl (new MamaSymbolListMemberImpl)
    { 
        myPimpl->myCimpl = member;

        // Note: the closure to the C implementation is the C++ object.
        mamaSymbolListMember_setClosure (myPimpl->myCimpl, this);
        myPimpl->mySymbolList = symbolList;
    }

    MamaSymbolListMember::~MamaSymbolListMember()
    {
        // Cimpl has already been freed in C apis
        delete myPimpl;
    }

    const char* MamaSymbolListMember::getSymbol () const
    {
        const char* symbol = NULL;
        mamaTry (mamaSymbolListMember_getSymbol (myPimpl->myCimpl, &symbol));

        return symbol;
    }

    const char* MamaSymbolListMember::getSource () const
    {
        const char* source = NULL;
        mamaTry (mamaSymbolListMember_getSource (myPimpl->myCimpl, &source));

        return source;
    }

    MamaTransport* MamaSymbolListMember::getTransport () const
    {
        if (myPimpl->myTransport == NULL)
        {
            mamaTransport tport = NULL;
            mamaTry (mamaSymbolListMember_getTransport (myPimpl->myCimpl, &tport));

            if (tport)
            {    
                myPimpl->myDeleteTport = true;
                myPimpl->myTransport   = new MamaTransport (tport);
            }
        }

        return myPimpl->myTransport;
    }

    MamaSymbolList* MamaSymbolListMember::getSymbolList () const
    {
        return myPimpl->mySymbolList;
    }

    void* MamaSymbolListMember::getClosure() const
    {
        return myPimpl->myClosure;
    }

    void MamaSymbolListMember::setSymbol (const char*  symbol)
    {
        mamaSymbolListMember_setSymbol (myPimpl->myCimpl, symbol);
    }

    void MamaSymbolListMember::setSource (const char*  source)
    {
        mamaSymbolListMember_setSource (myPimpl->myCimpl, source);
    }

    void MamaSymbolListMember::setTransport (MamaTransport*  transport)
    {
        myPimpl->myTransport = transport;
        mamaSymbolListMember_setTransport (myPimpl->myCimpl, transport->getCValue());
    }

    void MamaSymbolListMember::setSymbolList (MamaSymbolList*  symbolList)
    {
        myPimpl->mySymbolList = symbolList;
    }

    void MamaSymbolListMember::setClosure (void*  closure)
    {
        myPimpl->myClosure = closure;
    }

    mamaSymbolListMember MamaSymbolListMember::getCimpl ()
    {
        return myPimpl->myCimpl;
    }


} // namespace Wombat
