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
    /* ************************************************************** */
    #region MamaTimerCallback Interface

    /// <summary>
    /// Prototype for callback invoked by timer.
    /// </summary>
    public interface MamaTimerCallback
    {
        /// <summary>
        /// Prototype for callback invoked by timer.
        /// </summary>
        void onTimer(MamaTimer mamaTimer, object closure);

        /// <summary>
        /// This method is invoked when a timer has been completely destroyed,
        /// the client can have confidence that no further events will be placed
        /// on the queue for this timer.
        /// </summary>
        /// <param name="mamaTimer">
        /// The mama timer.
        /// </param>
        /// <param name="closure">
        /// The closure passed to the create function.
        /// </param>
        void onDestroy(MamaTimer mamaTimer, object closure);
    }

    #endregion
}
