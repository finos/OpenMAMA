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

#ifndef MAMA_INBOX_CALLBACK_IMPL_CPP_H__
#define MAMA_INBOX_CALLBACK_IMPL_CPP_H__

#include "mama/mamacpp.h"

namespace Wombat 
{
    /**
     * The MamaInboxCallback gets invoked when a message arrives in an inbox or
     * when inbox related errors arise.
     */
    class MAMACPPExpDLL MamaInboxCallback
    {
    public:
        virtual ~MamaInboxCallback (void) 
        {
        };

        /**
	     * This method is invoked when an inbox has been completely destroyed,
	     * the client can have confidence that no further events will be placed 
	     * on the queue for this inbox.
	     * 
	     * @param[in] inbox The MamaInbox.
         * @param[in] closure The closure passed to the create function.
	     */
        virtual void onDestroy (MamaInbox   *inbox, 
                                void        *closure)
        {
        };

        virtual void onMsg   (MamaInbox*         inbox,
                              MamaMsg&           msg) = 0;

        virtual void onError (MamaInbox*         inbox,
                              const MamaStatus&  status) = 0;
    };


} // namespace Wombat
#endif // MAMA_INBOX_CALLBACK_IMPL_CPP_H__
