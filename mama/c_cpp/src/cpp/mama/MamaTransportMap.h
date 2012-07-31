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

#ifndef MAMA_TRANSPORT_MAP_CPP_H
#define MAMA_TRANSPORT_MAP_CPP_H

#include <mama/config.h>
#include <mama/mama.h>

namespace Wombat
{

    class MamaTransport;

    class MAMACPPExpDLL MamaTransportMap
    {
    public:
        /**
         * Find the transport by name.  If no transport by the given name
         * has been requested before, a new instance of a MamaTransport is
         * created using the bridge specified.  This method will create 
         * exactly one MamaTransport object instance for each name passed.
         */
        static MamaTransport*  findOrCreate (const char*  transportName, 
                                             const        mamaBridge bridge);

        /**
         * Find the transport by name.  If no transport by the given name
         * exists, the default transport is returned.
         */
        static MamaTransport*  find (const char*  transportName,
                                     const        mamaBridge bridge);
    };

}

#endif
