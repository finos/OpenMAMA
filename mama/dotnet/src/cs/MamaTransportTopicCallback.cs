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

using System;

namespace Wombat
{
    using System;

    /// <summary>
	/// Callbacks at the topic level associated with MamaTransports
    /// </summary>
    /// <remarks>
    /// These are topic-specific callbacks from the transport.
    /// They are for sub/unsub, and for publisher events.
    /// </remarks>    
    public interface MamaTransportTopicCallback
    {
        /// <summary>
        /// See interface remarks for details
        /// </summary>
        void onTopicSubscribe (MamaTransport transport,
                               string topic,
                               Object platformInfo);

        /// <summary>
        /// See interface remarks for details
        /// </summary>
        void onTopicUnsubscribe(MamaTransport transport,
                                string topic,
                                Object platformInfo);

        /// <summary>
        /// See interface remarks for details
        /// </summary>
        void onTopicPublishError(MamaTransport transport,
                                 string topic,
                                 Object platformInfo);

        /// <summary>
        /// See interface remarks for details
        /// </summary>
        void onTopicPublishErrorNotEntitled(MamaTransport transport,
                                            string topic,
                                            Object platformInfo);

        /// <summary>
        /// See interface remarks for details
        /// </summary>
        void onTopicPublishErrorBadSymbol(MamaTransport transport,
                                           string topic,
                                           Object platformInfo);
    }
}
