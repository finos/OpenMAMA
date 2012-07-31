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

#ifndef MAMA_SYMBOL_SOURCE_CALLBACK_CPP_H__
#define MAMA_SYMBOL_SOURCE_CALLBACK_CPP_H__

#include "mama/mamacpp.h"

namespace Wombat 
{
    class MamaSymbolSource;

    /**
     * The MamaSymbolSourceCallback interface. The single callback function 
     * "onSymbol" gets invoked when a symbol arrives at a symbol source.
     */
    class MAMACPPExpDLL MamaSymbolSourceCallback
    {
    public:
        virtual ~MamaSymbolSourceCallback () {};

        /**
         * Method invoked when a symbol arrives at a symbol source.
         *
         * @param symbolSource The symbolSource.
         * @param symbol       The newly sourced symbol
         * @param closure      The closure associated with the symbol source.
         */
        virtual void onSymbol (MamaSymbolSource* symbolSource,
                               const char*       symbol,
                               void*             closure) = 0;
    };

} // namespace Wombat
#endif // MAMA_SYMBOL_SOURCE_CALLBACK_CPP_H__
