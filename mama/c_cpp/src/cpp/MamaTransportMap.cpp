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

#include "mama/MamaTransportMap.h"
#include "mama/MamaTransport.h"
#include "mamacppinternal.h"
#include "wombat/wtable.h"
#include <stdio.h>

namespace Wombat
{

    static wtable_t        theTransportList    = 0;
    static MamaTransport*  theDefaultTransport = NULL;

    static wtable_t getList ()
    {
        if (!theTransportList)
        {
            theTransportList = wtable_create ("MamaTransportMap", 64);
        }
        return theTransportList;
    }

    static MamaTransport* getDefault (const mamaBridge bridge)
    {
        if (!theDefaultTransport)
        {
            theDefaultTransport = new MamaTransport;
            theDefaultTransport->create (NULL, bridge);
        }
        return theDefaultTransport;
    }

     /* TODO implement this in C and wrap it */
    MamaTransport* MamaTransportMap::find (const char* transportName,
                                           const mamaBridge bridge)
    {
        if (!transportName || !transportName[0])
        {
            return getDefault (bridge);
        }
        wtable_t thelist = getList ();
        return (MamaTransport*)wtable_lookup (thelist, transportName);
    }

    MamaTransport* MamaTransportMap::findOrCreate (const char* transportName, 
                                                   const mamaBridge bridge)
    {
        MamaTransport* tport = MamaTransportMap::find (transportName, bridge);
        if (!tport)
        {
            tport = new MamaTransport;
            tport->create (transportName, bridge);
            wtable_t thelist = getList ();
            wtable_insert (thelist, transportName, tport);
        }
        return tport;
    }

} // namespace Wombat
