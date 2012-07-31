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

#ifndef MAMA_SYMBOL_MAP_CPP_H__
#define MAMA_SYMBOL_MAP_CPP_H__

#include "mama/mamacpp.h"

namespace Wombat 
{
    /**
     * The MamaSymbolMap class provides the ability for MAMA to do
     * client side symbology mapping.  Subclasses of this class can
     * provide custom symbology mapping.  A MamaSymbolMap can be
     * assigned to each MamaTransport.
     */

    class MAMACPPExpDLL MamaSymbolMap
    {
    public:
        virtual ~MamaSymbolMap (void) {}

        /**
         * Map a symbol.  
         * The result is the feed side symbol used to actually subscribe 
         * to in the infrastructure.
         * The return value indicates whether or not a symbology mapping 
         * existed for the given symbol (True = yes, False = No). 
         */
        virtual bool map (char*        result,
                          const char*  symbol,
                          size_t       maxLen) const = 0;
        
        /**
         * Map a feed side symbol. 
         * This is reverse of the natural mapping schema and the result in 
         * this case is the corresponding client side symbol.
         * The return value indicates whether or not a symbology mapping 
         * existed for the given symbol (True = yes, False = No). 
         */
        virtual bool revMap  (char*        result,
                              const char*  symbol,
                              size_t       maxLen) const = 0;
    };

} // namespace Wombat
#endif // MAMA_SYMBOL_MAP_CPP_H__
