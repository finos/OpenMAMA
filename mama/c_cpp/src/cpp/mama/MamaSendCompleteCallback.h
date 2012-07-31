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

#ifndef MAMA_SEND_COMPLETE_CB_H__
#define MAMA_SEND_COMPLETE_CB_H__

namespace Wombat 
{

    class MamaPublisher;
    class MamaMsg;
    class MamaStatus;

    /**
     * Callback interface for use with the MamaPublisher.sendWithThrottle() and
     * MamaPublisher.sendFromInboxWithThrottle() methods. The
     * <code>onSendComplete()</code> method is invoked once a message being sent
     * on the throttle is no longer required by the API.
     *
     * Messages sent on the throttle queue are no longer destroyed by the API. It
     * is the responsibility of the application developer to manage the lifecycle
     * of any messages sent on the throttle.
     */
    class MAMACPPExpDLL MamaSendCompleteCallback
    {
    public:
        virtual ~MamaSendCompleteCallback () 
        {
        };

        /**
         * Called whenever the API has sent a message on the throttle queue.
         * 
         * @param publisher The publisher object used to send the message.
         * @param msg       The MamaMsg which has been sent from the throttle
         *                  queue.
         * @param status    Whether the message was successfully sent from the
         *                  throttle. A value of MAMA_STATUS_OK indicates that
         *                  the send was successful.
         * @param closure   User supplied context data.
         */
        virtual void onSendComplete (
            MamaPublisher&  publisher,
            MamaMsg*        msg,
            MamaStatus&     status,
            void*           closure) = 0;
    };

} // namespace Wombat
#endif //MAMA_SEND_COMPLETE_CB
