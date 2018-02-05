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

#include "mama/MamaSymbolListFile.h"
#include "mama/MamaTransportMap.h"
#include <stdio.h>
#include <string.h>

namespace Wombat
{

    static const int  MAX_FILE_NAME_LEN = 512;

    struct MamaSymbolListFile::MamaSymbolListFileImpl : public MamaTimerCallback
    {
        MamaSymbolListFileImpl () 
        { 
            myFileName[0] = '\0'; 
        }

        void onTimer (MamaTimer* timer);

        char        myFileName [MAX_FILE_NAME_LEN];
        MamaTimer   myCheckTimer;
        mamaBridge  myBridge;
    };

    MamaSymbolListFile::MamaSymbolListFile ()
        : myPimpl (new MamaSymbolListFileImpl)
    {
    }

    MamaSymbolListFile::~MamaSymbolListFile ()
    {
        delete myPimpl;
    }

    void MamaSymbolListFile::setFileName (const char*  fileName)
    {
        if (fileName)
        {
            strcpy (myPimpl->myFileName, fileName);
        }
        else
        {
            myPimpl->myFileName[0] = '\0';
        }
    }

    void MamaSymbolListFile::setBridge (const mamaBridge bridge)
    {
        myPimpl->myBridge = bridge;    
    }

    void MamaSymbolListFile::readFile ()
    {
        if (!myPimpl->myFileName[0])
        {
            return;
        }

        FILE* f = fopen (myPimpl->myFileName, "r");
        if (f)
        {
            char symbol     [MAMA_MAX_SYMBOL_LEN];
            char source     [MAMA_MAX_SOURCE_LEN];
            char transport  [MAMA_MAX_TRANSPORT_LEN];
            char line       [1024];

            while (fgets (line, 1024, f))
            {
                symbol    [0] = '\0';
                source    [0] = '\0';
                transport [0] = '\0';

                sscanf (line, "%s %s %s", symbol, source, transport);

                MamaSymbolListMember* member = new MamaSymbolListMember (this);

                member->setSymbol    (symbol);
                member->setSource    (source);
                member->setTransport (MamaTransportMap::findOrCreate (transport, 
                                                                      myPimpl->myBridge));
                addMember (member);
            }
        }
    }

    class SymbolWriter : public MamaSymbolListIteratorHandler
    {
    public:
        SymbolWriter (FILE* f) 
            : myStream(f) 
        {
        }

        virtual ~SymbolWriter () 
        {
        }

        void onMember (MamaSymbolList&        symbolList,
                       MamaSymbolListMember&  member,
                       void*                  closure)
        {
            const char*     symbol    = member.getSymbol();
            const char*     source    = member.getSource();
            MamaTransport*  transport = member.getTransport();

            if (symbol)
            {
                 fprintf (myStream, "%s %s %s\n",
                          symbol, 
                          source, 
                          transport ? transport->getName() : "NULL");
            }
        }

    private:
        FILE* myStream;
    };

    void MamaSymbolListFile::writeFile ()
    {
        if (!myPimpl->myFileName[0])
        {
            return;
        }

        FILE* f = fopen (myPimpl->myFileName, "w");

        if (f)
        {
            SymbolWriter writer (f);
            iterate             (writer);
        }

        fclose (f);
    }

    void MamaSymbolListFile::setFileMonitor (double      checkSeconds,
                                             MamaQueue*  queue)
    {
        myPimpl->myCheckTimer.destroy ();
        myPimpl->myCheckTimer.create  (queue, myPimpl, checkSeconds);
    }

    void MamaSymbolListFile::MamaSymbolListFileImpl::onTimer (MamaTimer*  timer)
    {
        int  toBeCompleted = 0;
        toBeCompleted = 1;
    }


} // namespace Wombat
