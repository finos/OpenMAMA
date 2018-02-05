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

#ifndef MamdaNewsStoryHandlerH
#define MamdaNewsStoryHandlerH

#include <mamda/MamdaOptionalConfig.h>
#include <mama/mamacpp.h>

namespace Wombat
{

    class MamdaNewsStory;
    class MamdaNewsManager;
    class MamdaSubscription;


    /**
     * MamdaNewsHandler is an interface for applications that want to
     * have an easy way to handle news stories.  The interface defines
     * callback methods for different types of news story events.  
     *
     * Instances of MamdaNewsStory do not survive beyond the onNewsStory()
     * callback and should be copied if they need to be preserved beyond
     * that callback.
     */

    class MAMDAOPTExpDLL MamdaNewsStoryHandler
    {
    public:
        /**
         * Method invoked when a news story response is received.
         *
         * @param manager  The MamdaNewsManager that is managing this news service.
         * @param msg      The MamaMsg that triggered this invocation.
         * @param story    The requested news story.
         * @param closure  The closure argument to the story request.
         */
        virtual void onNewsStory (
            MamdaNewsManager&         manager,
            const MamaMsg&            msg,
            const MamdaNewsStory&     story,
            void*                     closure) = 0;
            
        virtual ~MamdaNewsStoryHandler() {};

    };

} // namespace

#endif // MamdaNewsStoryHandlerH
