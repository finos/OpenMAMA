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

#ifndef MamdaSymbolSourceHandlerH
#define MamdaSymbolSourceHandlerH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaSymbolSourceEvent.h>

namespace Wombat
{

    /**
     * MamdaSymbolSourceHandler is an interface for applications that want to
     * have an easy way to handle newly sourced symbol events. The interface 
     * defines the callback method for the new symbol event: onSymbol
     */
    class MAMDAExpDLL MamdaSymbolSourceHandler
    {
    public:

        /**
         * Method invoked when a new symbol has been sourced.
         *
         * @param event  The MamdaSymbolSourceEvent object.
         */
        virtual void onSymbol(
            const MamdaSymbolSourceEvent&  event) = 0;
            
        virtual ~MamdaSymbolSourceHandler() {};
    };

} // namespace

#endif // MamdaSymbolSourceHandlerH
