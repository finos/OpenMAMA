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

#ifndef MamdaNewsManagerH
#define MamdaNewsManagerH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaErrorListener.h>
#include <mamda/MamdaQualityListener.h>
#include <mamda/MamdaNewsHeadline.h>
#include <mamda/MamdaNewsHeadlineHandler.h>
#include <mamda/MamdaNewsStory.h>
#include <mamda/MamdaNewsStoryHandler.h>
#include <mamda/MamdaNewsQueryHandler.h>
#include <mamda/MamdaNewsQuery.h>

namespace Wombat
{

    /**
     * MamdaNewsManager provides a class for managing access to streaming
     * news headlines, headline queries, individual story queries, etc.
     * Like the MAMDA "listener" classes, such as MamdaQuoteListener and
     * MamdaTradeListener, the MamdaNewsManager cache certain information
     * about the news service.  Unlike those MAMDA "listener" classes, the
     * MamdaNewsManager is also responsible for creating and managing its
     * own subscriptions.
     */

    class MAMDAOPTExpDLL MamdaNewsManager
    {
    public:
        MamdaNewsManager ();
        virtual ~MamdaNewsManager();

        /**
         * Add a broadcast headline source to the news manager.  Headline
         * updates will be passed to the handler(s) registered by
         * addBroadcastHeadlineHandler().
         */
        void    addBroadcastHeadlineSource (MamaQueue*      queue,
                                            MamaSource*     source,
                                            const char*     symbol,
                                            void*           closure);
        /**
         * Remove all broadcast headline sources from the news manager.
         */
        void    clearBroadcastHeadlineSources ();

        /**
         * Add a broadcast story source to the news manager.  Stories will
         * be passed to the handler(s) registered by
         * addBroadcastStoryHandler().
         */
        void    addBroadcastStorySource    (MamaQueue*      queue,
                                            MamaSource*     source,
                                            const char*     symbol,
                                            void*           closure);

        /**
         * Remove all broadcast story sources from the news manager.
         */
        void    clearBroadcastStorySources ();

        /**
         * Add a headline handler for broadcast headlines.
         */
        void    addBroadcastHeadlineHandler (MamdaNewsHeadlineHandler*  handler);

        /**
         * Add a story handler for broadcast stories.
         */
        void    addBroadcastStoryHandler (MamdaNewsStoryHandler*  handler);

        /**
         * Add a quality data handler.
         */
        void    addQualityHandler (MamdaQualityListener*  handler);

        /**
         * Add an error handler.
         */
        void    addErrorHandler (MamdaErrorListener*  handler);

        
        void clearQuerySources ();
        
        /**
         * Request a story (by headline), invoking the given story handler
         * when the story arrives (or not).
         */
        void executeQuery (
                    MamaQueue*                queue,
                    MamaSource*               source,
                    MamdaQuery*               newsQuery,
                    MamdaNewsQueryType        queryType,
                    MamdaNewsQueryHandler*    handler,
                    void*                     closure);
                              
        void removeQuery (MamdaQuery*             newsQuery);
        
        /**
         * Request a story (by headline), invoking the given story handler
         * when the story arrives (or not).
         */
        void    requestStory (const MamdaNewsHeadline&  headline,
                              MamdaNewsStoryHandler*    handler,
                              void*                     closure);

        /**
         * Request a story (by explicit headline ID, source and queue),
         * invoking the given story handler when the story arrives (or
         * not).  The source and queue are the same parameters that
         * correspond to the headline source (see addHeadlineSource()).
         */
        void    requestStory (MamaQueue*                queue,
                              MamaSource*               source,
                              MamdaNewsStoryId          storyId,
                              MamdaNewsStoryHandler*    handler,
                              void*                     closure);

        /**
         * Request a story (by headline) after some delay, invoking the
         * given story handler when the story arrives (or not).
         */
        void    requestStoryLater (const MamdaNewsHeadline&  headline,
                                   MamdaNewsStoryHandler*    handler,
                                   double                    secondsLater,
                                   void*                     closure);

        /**
         * Request a story (by existing or partial story) after some
         * delay, invoking the given story handler when the story arrives
         * (or not).
         */
        void    requestStoryLater (const MamdaNewsStory&     story,
                                   MamdaNewsStoryHandler*    handler,
                                   double                    secondsLater,
                                   void*                     closure);

        /**
         * Request a story (by headline) after some delay, invoking the
         * given story handler when the story arrives (or not).
         */
        void    requestStoryLater (MamaQueue*                queue,
                                   MamaSource*               source,
                                   MamdaNewsStoryId          storyId,
                                   MamdaNewsStoryHandler*    handler,
                                   double                    secondsLater,
                                   void*                     closure);

        struct MamdaNewsManagerImpl;
        MamdaNewsManagerImpl& mImpl;
    };

} // namespace

#endif // MamdaNewsManagerH
