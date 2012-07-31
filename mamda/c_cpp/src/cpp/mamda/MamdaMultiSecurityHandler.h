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

#ifndef MamdaMultiSecurityHandlerH
#define MamdaMultiSecurityHandlerH

#include <mamda/MamdaConfig.h>

namespace Wombat
{

    class MamdaSubscription;
    class MamdaMultiSecurityManager;

    /**
     * The MamdaMultiSecurityHandler class is an interface that allows
     * a developer to be notified dynamically when securities are added to
     * the list. Access to consolidated information (i.e., best bid and
     * offer and consolidated trade info) is also available.
     */
    class MAMDAExpDLL MamdaMultiSecurityHandler
    {
    public:
        
        /**
         * Method invoked when the trade and quote information for the
         * security has become available. Invoked as a result of
         * an initial image on subscription (assuming initial values
         * are enabled) or if an update is received for a security the
         * MamdaMultiSecurityManager is unaware of (typically for securities
         * which come online intra day and, for which, we have not received
         * an initial value).
         * This method is invoked for participant and consolidated
         * quote or trade information.
         *
         * isPrimary is not yet supported!
         */
        virtual void onSecurityCreate (
            MamdaSubscription*             subscription,
            MamdaMultiSecurityManager&     manager,
            const char*                    securitySymbol,
            bool                           isPrimary) = 0;

        virtual ~MamdaMultiSecurityHandler() {};
    };

} // namespace

#endif // MamdaMultiSecurityHandlerH
