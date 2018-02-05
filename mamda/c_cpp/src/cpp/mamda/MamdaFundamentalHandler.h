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

#ifndef MamdaFundamentalHandlerH
#define MamdaFundamentalHandlerH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaFundamentalListener.h>
#include <mama/mamacpp.h>

namespace Wombat
{

    /**
     * MamdaFundamentalHandler is an interface for applications that want to
     * have an easy way to access fundamental equity pricing/analysis attributes, 
     * indicators and ratios.  The interface defines a single callback methods for 
     * for receiving updates on fundamental data.
     */

    class MAMDAExpDLL MamdaFundamentalHandler
    {
    public:
        /**
         * Method invoked when one or more of the Fundamental fields have
         * been updated by one of the following market data events:
         * - Initial image.
         * - Recap update (e.g., after server fault tolerant event or data
         *   quality event.)
         * - Generic update..
         *
         * @param subscription  The subscription which received the update.
         * @param listener      The listener which invoked this callback.
         * @param msg           The MamaMsg that triggered this invocation.
         * @param fundas        Access to the fundamental update details.
         */
        virtual void onFundamentals (
            MamdaSubscription*        subscription,
            MamdaFundamentalListener& listener,
            const MamaMsg&            msg,
            const MamdaFundamentals&  fundas) = 0;
            
        virtual ~MamdaFundamentalHandler () {};
    };

} // namespace

#endif // MamdaFundamentalHandlerH
