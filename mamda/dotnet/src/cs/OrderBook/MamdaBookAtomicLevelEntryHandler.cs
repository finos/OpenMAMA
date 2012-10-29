/* $Id: MamdaBookAtomicLevelEntryHandler.cs,v 1.2.32.5 2012/09/07 07:44:57 ianbell Exp $
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
    /**
     * MamdaBookAtomicLevelEntryHandler is an interface for applications that 
     * want to have an easy way to handle order book Price Level and Entry updates.
     * The interface defines callback methods for different types of 
     * orderBook-related events: order book recaps and updates.
     */
    public interface MamdaBookAtomicLevelEntryHandler
    {

		///<summary> Method invoked when a full refresh of the order book for the
        ///security is available.  The reason for the invocation may be
        ///any of the following:
        ///- Initial image.
        ///- Recap update (e.g., after server fault tolerant event or data
        ///quality event.)
        ///- After stale status removed.</summary>         
        ///<param name = "subscription"></param>//The MamdaSubscription handle.
        ///<param name = "listener"></param>	//The listener handling the recap. 
        ///<param name = "msg"></param>			//The MamaMsg that triggered this invocation.
        ///<param name = "levelEntry"></param>	//The Price Level Entry recap.
        void onBookAtomicLevelEntryRecap (
            MamdaSubscription          subscription,
            MamdaBookAtomicListener    listener,
            MamaMsg                    msg,
            MamdaBookAtomicLevelEntry  levelEntry);


		///<summary> Method invoked when an order book delta is reported.</summary>         
		///<param name = "subscription"></param>//The MamdaSubscription handle.
		///<param name = "listener"></param>	//The listener handling the recap. 
		///<param name = "msg"></param>			//The MamaMsg that triggered this invocation.
		///<param name = "levelEntry"></param>	//The Price Level Entry update.
        void onBookAtomicLevelEntryDelta (
            MamdaSubscription          subscription,
            MamdaBookAtomicListener    listener,
            MamaMsg                    msg,
            MamdaBookAtomicLevelEntry  levelEntry);

    }
}
