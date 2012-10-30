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
/**
 * MamdaBookAtomicBookHandler is an interface for applications that need to
 * know when a MamdaBookAtomicListener finishes processing a single book
 * update.
 * 
 * This may be useful for applications that wish to destroy the subscription 
 * from a callback as the subscription can only be destroyed after the message
 * processing is complete. Furthermore, it allows applications to determine
 * when to clear the book when a recap arrives.
 */
    public interface MamdaBookAtomicBookHandler
    {
		///<summary> Method invoked before we start processing the first level in a message.
		///The book should be cleared when isRecap == true.</summary>
		///<param name = "subscription"></param>//The MamdaSubscription handle.
		///<param name = "listener"></param>	//The listener handling the recap. 
		///<param name = "isRecap"></param>		//Whether the first update was a recap.
        void onBookAtomicBeginBook (
            MamdaSubscription           subscription,
            MamdaBookAtomicListener     listener,
            bool                        isRecap );

		///<summary> Method invoked when we stop processing the last level in a message.
		///We invoke this method after the last entry for the level gets processed.
		///The subscription may be destroyed from this callback.</summary>
		///<param name = "subscription"></param>//The MamdaSubscription handle.
		///<param name = "listener"></param>	//The listener handling the recap. 
		///<param name = "isRecap"></param>		//Whether the first update was a recap.
        void onBookAtomicEndBook (
            MamdaSubscription           subscription,
            MamdaBookAtomicListener     listener);


		///<summary> Method invoked when an order book is cleared.</summary>
		///<param name = "subscription"></param>//The MamdaSubscription handle.
		///<param name = "listener"></param>	//The listener handling the recap. 
		///<param name = "msg"></param>			//The MamaMsg that triggered this invocation.
        void onBookAtomicClear (
            MamdaSubscription           subscription,
            MamdaBookAtomicListener     listener,
            MamaMsg                     msg);

		///<summary> Method invoked when a gap in orderBook reports is discovered.</summary>
		///<param name = "subscription"></param>//The MamdaSubscription handle.
		///<param name = "listener"></param>	//The listener handling the recap. 
		///<param name = "msg"></param>			//The MamaMsg that triggered this invocation.
		///<param name = "gapEvent"></param>	//The gap value object.
        void onBookAtomicGap (
            MamdaSubscription                subscription,
            MamdaBookAtomicListener          listener,
            MamaMsg                          msg,
            MamdaBookAtomicGap               gapEvent);
        
    }
}
