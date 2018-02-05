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

#ifndef MAMA_INBOX_CPP_H__
#define MAMA_INBOX_CPP_H__

#include <mama/mamacpp.h>

namespace Wombat 
{

    /**
     * Used for sending messages requesting a direct reply.
     */
    class MAMACPPExpDLL MamaInbox
    {
        /* Private Member Variables. */
        mamaInbox           mInbox;
        MamaInboxCallback * mCallback;
        void *              mClosure;    

        /* Private Functions. */    
        static void MAMACALLTYPE onInboxDestroy (mamaInbox   inbox, 
                                                 void        *closure);

        static void MAMACALLTYPE onInboxError   (mama_status status, 
                                                 void        *closure);

        static void MAMACALLTYPE onInboxMsg     (mamaMsg     msg, 
                                                 void        *closure);    

    public:
        virtual ~MamaInbox (void);

        MamaInbox (void);

        /**
         * Create an inbox.
         *
         * @param transport The transport for sending requests and receiving
         *                  replies.
         * @param queue The queue.
         * @param callback  The callback for receiving replies and errors.
         * @param closure The caller supplied closure.
         */
        virtual void  create     (MamaTransport*       transport,
                                  MamaQueue*           queue,
                                  MamaInboxCallback*   callback,
                                  void*                closure = NULL);

        virtual void  destroy    (void);
        virtual void* getClosure (void) const;

        // Access to C types for implementation of related classes.
        mamaInbox        getCValue    ();
        const mamaInbox  getCValue    () const;
    };

} // namespace Wombat
#endif // MAMA_INBOX_CPP_H__
