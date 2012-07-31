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

#include <mamda/MamdaNewsManager.h>
#include <mamda/MamdaNewsFields.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaMsgListener.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaQuery.h>
#include <mama/MamaReservedFields.h>
#include <mama/MamaTimerCallback.h>
#include <mama/MamaTimer.h>
#include <deque>
#include <string>
#include <assert.h>

using std::string;
using std::deque;

namespace Wombat
{

    class MamdaNewsManager::MamdaNewsManagerImpl
    {
    public:
        MamdaNewsManagerImpl (MamdaNewsManager&  manager);
        virtual ~MamdaNewsManagerImpl();

        void addBroadcastHeadlineSource (
            MamaQueue*                queue,
            MamaSource*               source,
            const char*               symbol,
            void*                     closure);

        void addBroadcastStorySource (
            MamaQueue*                queue,
            MamaSource*               source,
            const char*               symbol,
            void*                     closure);

        void clearBroadcastHeadlineSources ();

        void clearBroadcastStorySources ();

        void clearQuerySources ();
        
        void requestStory (
            const MamdaNewsHeadline&  headline,
            MamdaNewsStoryHandler*    handler,
            void*                     closure);

        void requestStory (
            MamaQueue*                queue,
            MamaSource*               source,
            MamdaNewsStoryId          storyId,
            MamdaNewsStoryHandler*    handler,
            void*                     closure);

        void requestStoryLater (
            const MamdaNewsHeadline&  headline,
            MamdaNewsStoryHandler*    handler,
            double                    seconds,
            void*                     closure);

        void requestStoryLater (
            const MamdaNewsStory&     story,
            MamdaNewsStoryHandler*    handler,
            double                    seconds,
            void*                     closure);

        void requestStoryLater (
            MamaQueue*                queue,
            MamaSource*               source,
            MamdaNewsStoryId          storyId,
            MamdaNewsStoryHandler*    handler,
            double                    seconds,
            void*                     closure);

        void executeQuery (
            MamaQueue*                queue,
            MamaSource*               source,
            MamdaQuery*               newsQuery,
            MamdaNewsQueryType        queryType,
            MamdaNewsQueryHandler*    handler,
            void*                     closure);
        
        void removeQuery (
            MamdaQuery*  newsQuery);
            
        void handleHeadline (
            MamdaSubscription*        subscription,
            const MamaMsg&            msg,
            MamdaNewsHeadline*        headline);

        void handleStory (
            MamdaSubscription*        subscription,
            const MamaMsg&            msg,
            MamdaNewsStoryHandler*    replyHandler);

        void onError (
            MamdaSubscription*        subscription,
            MamdaErrorSeverity        severity,
            MamdaErrorCode            code,
            const char*               errorStr);

        void onQuality (
            MamdaSubscription*        subscription,
            mamaQuality               quality);

        MamdaNewsManager&                  mManager;
        deque<MamdaSubscription*>          mHeadlineSubscs;
        deque<MamdaSubscription*>          mStorySubscs;
        deque<MamdaSubscription*>          mQuerySubscs;
        deque<MamdaNewsHeadlineHandler*>   mHeadlineHandlers;
        deque<MamdaNewsStoryHandler*>      mStoryHandlers;
        deque<MamdaQualityListener*>       mQualityHandlers;
        deque<MamdaErrorListener*>         mErrorHandlers;
        
        MamaPublisher*  mPublisher;
        string*         mXmlString;
        MamaMsg*        mMessage;
        int             timeoutValue;
        
        void removeSubscription (MamdaSubscription* subscription);
        
        MamaMsgField mTmpField;
    };


    class HeadlineMsgHandler : public MamdaMsgListener
                             , public MamdaErrorListener
                             , public MamdaQualityListener
    {
    public:
        HeadlineMsgHandler (MamdaNewsManager::MamdaNewsManagerImpl&  impl)
            : mImpl (impl) {}

        virtual ~HeadlineMsgHandler () {}

        void onMsg (
            MamdaSubscription*        subscription,
            const MamaMsg&            msg,
            short                     msgType)
        {
            try
            {
                mHeadline.clear();
                mImpl.handleHeadline (subscription, msg, &mHeadline);
                
                mHeadline.setSubscriptionInfo (subscription);

                deque<MamdaNewsHeadlineHandler*>::iterator end = mImpl.mHeadlineHandlers.end();
                deque<MamdaNewsHeadlineHandler*>::iterator iter;

                for (iter = mImpl.mHeadlineHandlers.begin(); iter != end; ++iter)
                {
                    MamdaNewsHeadlineHandler*  handler = *iter;
                    handler->onNewsHeadline (mImpl.mManager, 
                                             msg, 
                                             mHeadline,
                                             subscription->getClosure());
                }
            }
            catch (MamaStatus& e)
            {
                mama_log (MAMA_LOG_LEVEL_NORMAL,
                          "caught MamaStatus exception: %s", 
                          e.toString());
            }
        }

        void onError (
            MamdaSubscription*        subscription,
            MamdaErrorSeverity        severity,
            MamdaErrorCode            code,
            const char*               errorStr)
        { 
            mImpl.onError (subscription, 
                           severity, 
                           code, 
                           errorStr); 
        }

        void onQuality (
            MamdaSubscription*        subscription,
            mamaQuality               quality)
        { 
            mImpl.onQuality (subscription, quality); 
        }

    private:
        MamdaNewsManager::MamdaNewsManagerImpl&  mImpl;
        MamdaNewsHeadline mHeadline;
    };


    class StoryMsgHandler : public MamdaMsgListener
                          , public MamdaErrorListener
                          , public MamdaQualityListener
    {
    public:
        StoryMsgHandler (
            MamdaNewsManager::MamdaNewsManagerImpl&  impl,
            MamdaNewsStoryHandler*                   handler)
            : mImpl   (impl) , 
            mHandler  (handler) 
        {}

        virtual ~StoryMsgHandler () {}

        void onMsg (
            MamdaSubscription*        subscription,
            const MamaMsg&            msg,
            short                     msgType)
        {
            try
            {
                mImpl.handleStory (subscription, msg, mHandler);
            }
            catch (MamaStatus& e)
            {
                mama_log (MAMA_LOG_LEVEL_NORMAL,
                          "caught MamaStatus exception: %s", 
                          e.toString());
            }
        }

        void onError (
            MamdaSubscription*        subscription,
            MamdaErrorSeverity        severity,
            MamdaErrorCode            code,
            const char*               errorStr)
        { 
            mImpl.onError (subscription, 
                           severity, 
                           code, 
                           errorStr); 
        }

        void onQuality (
            MamdaSubscription*        subscription,
            mamaQuality               quality)
        { 
            mImpl.onQuality (subscription, quality); 
        }

    private:
        MamdaNewsManager::MamdaNewsManagerImpl&  mImpl;
        MamdaNewsStoryHandler*                   mHandler;
    };

    #define QUERY_TYPE_FLD_FID		    4001
    #define QUERY_TYPE_FLD_NAME	        "NewsQueryType"
    #define QUERY_FLD_FID   			4002		
    #define QUERY_FLD_NAME  		    "NewsQuery"
    #define QUERY_TOPIC_FLD_FID  	    4003
    #define QUERY_TOPIC_FLD_NAME 	    "NewsQueryTopic"
    #define QUERY_ERROR_FLD_FID  	    4004
    #define QUERY_ERROR_FLD_NAME 	    "NewsQueryError"
    #define QUERY_HEADLINE_FLD_FID  	4005
    #define QUERY_HEADLINE_FLD_NAME 	"NewsQueryHeadline"


    class QueryMsgHandler : public MamdaMsgListener
                          , public MamdaErrorListener
                          , public MamdaQualityListener
                          , public MamaInboxCallback
                          , public MamaTimerCallback
    {
    public:
        QueryMsgHandler (
            MamdaNewsManager::MamdaNewsManagerImpl&  impl,
            MamdaNewsQueryHandler*                   handler, 
            MamaSource*                              aSource, 
            MamaQueue*                               aQueue, 
            MamdaQuery*                              aQuery, 
            MamdaNewsQueryType                       queryType,
            MamaTimer                                *timeOut, 
            void*                                    closure)
            : mImpl      (impl), 
              mHandler   (handler),
              errorText  (NULL),
              mQueue     (aQueue),
              mSource    (aSource),
              mQuery     (aQuery),
              mTimeout   (timeOut),
              mQueryType (queryType),
              mClosure   (closure)
        {}
                        
        virtual ~QueryMsgHandler () {}

        
        /*   REALTIME SUIBSCRIPTION CALLBACKS */
        void onMsg (
            MamdaSubscription*        subscription,
            const MamaMsg&            msg,
            short                     msgType)
        {
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "got sub onMsg");
                          
            mHeadline.clear();
            mImpl.handleHeadline (subscription, msg, &mHeadline);
            
            mHeadline.setSubscriptionInfo (subscription);

            mHandler->onNewsQueryHeadline (mImpl.mManager, 
                                           msg, 
                                           mHeadline, 
                                           *mQuery,
                                           mClosure);
        }

        void onError (
            MamdaSubscription*        subscription,
            MamdaErrorSeverity        severity,
            MamdaErrorCode            code,
            const char*               errorStr)
        { 
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "got sub onError");   
        }

        void onQuality (
            MamdaSubscription*        subscription,
            mamaQuality               quality)
        {
            mama_log (MAMA_LOG_LEVEL_FINE,
                      "got sub onQuality");
        }

        /*  QUERY CALLBACKS */
        void onMsg (
            MamaInbox*         inbox,
            MamaMsg&     msg)
        {
            size_t      resultLen = 0;
            MamaStatus  mStatus   = MAMA_STATUS_SYSTEM_ERROR;

            mama_log (MAMA_LOG_LEVEL_FINE,
                      "got inbox reply");
            
            mTimeout->destroy();
            delete mTimeout;

            if (msg.tryString (QUERY_ERROR_FLD_NAME,
                               QUERY_ERROR_FLD_FID,
                               errorText))
            {
                mama_log (MAMA_LOG_LEVEL_FINE,
                          "got news error");
                
                mHandler->onNewsQueryError (mImpl.mManager, 
                                            &msg, 
                                            errorText, 
                                            mStatus, 
                                            *mQuery, 
                                            mClosure);
            }
            
            if (msg.tryString (QUERY_TOPIC_FLD_NAME,
                               QUERY_TOPIC_FLD_FID,
                               newTopic))
            {
                mama_log (MAMA_LOG_LEVEL_FINE,
                          "got news topic");
                
                MamdaSubscription * aSubscription = new MamdaSubscription;

                aSubscription->addMsgListener     (this);
                aSubscription->addErrorListener   (this);
                aSubscription->addQualityListener (this);
                aSubscription->setRequireInitial  (false);
                aSubscription->create (mQueue, 
                                       mSource, 
                                       newTopic, 
                                       this);
                
                mImpl.mQuerySubscs.push_back (aSubscription);
                
                mQuery->setSubscriptionInfo (aSubscription);
            }
            
            if (msg.tryVectorMsg (QUERY_HEADLINE_FLD_NAME, 
                                  QUERY_HEADLINE_FLD_FID,
                                  mMsgArray,
                                  resultLen))
            {
                mama_log (MAMA_LOG_LEVEL_FINE,
                          "got news headlines");
                          
                // Pass to handler                 
                for (size_t i=0; i<resultLen; i++)
                {
                    mHeadline.clear();
                    mImpl.handleHeadline (NULL, *mMsgArray[i], &mHeadline);
                    
                    mHeadline.setSubscInfo (mQueue, mSource);
                    
                    mHandler->onNewsQueryHeadline (mImpl.mManager, 
                                                   *mMsgArray[i], 
                                                   mHeadline, 
                                                   *mQuery,
                                                   mClosure);
                }
            }  

            if (mQueryType == QUERY_TYPE_HISTORICAL)
            {
                mHandler->onNewsQueryComplete (mImpl.mManager, 
                                               *mQuery, 
                                               mClosure);
            }
            
            inbox->destroy();  
            
            delete inbox;
            
            if (mQueryType == QUERY_TYPE_HISTORICAL)
                delete this;
        }
            
        void onError (
            MamaInbox*         inbox,
            const MamaStatus&  status)
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                      "got inbox error");

            mHandler->onNewsQueryError (mImpl.mManager, 
                                        NULL, 
                                        "Query Send Fail", 
                                        status, 
                                        *mQuery, 
                                        mClosure);  
                
            mTimeout->destroy();
            inbox->destroy();

            delete inbox;
            delete mTimeout;
            delete this;
        }

        void onTimer (MamaTimer* timer)
        {
            MamaStatus  mStatus = MAMA_STATUS_TIMEOUT;
            MamaInbox *ainbox = (MamaInbox*)timer->getClosure();

            mHandler->onNewsQueryError (mImpl.mManager, 
                                        NULL, 
                                        "Query Timeout", 
                                        mStatus, 
                                        *mQuery, 
                                        mClosure);

            timer->destroy();
            ainbox->destroy();
            
            delete ainbox;
            delete timer;
            delete this;
        }
            
    private:
        MamdaNewsManager::MamdaNewsManagerImpl&  mImpl;
        MamdaNewsQueryHandler*                   mHandler;
        const MamaMsg                            **mMsgArray;
        const char*                              errorText;
        const char*                              newTopic;
        MamdaNewsHeadline mHeadline;
        MamaQueue*                               mQueue; 
        MamaSource*                              mSource;
        MamdaQuery*                              mQuery;
        MamaTimer*                               mTimeout;
        MamdaNewsQueryType                       mQueryType;
        void*                                    mClosure;
    };

    MamdaNewsManager::MamdaNewsManager ()
        : mImpl (*new MamdaNewsManagerImpl(*this))
    {
    }

    MamdaNewsManager::~MamdaNewsManager ()
    {
        delete &mImpl;
    }

    void MamdaNewsManager::addBroadcastHeadlineSource (
        MamaQueue*      queue,
        MamaSource*     source,
        const char*     symbol,
        void*           closure)
    {
        mImpl.addBroadcastHeadlineSource (queue, 
                                          source, 
                                          symbol, 
                                          closure);
    }

    void MamdaNewsManager::addBroadcastStorySource (
        MamaQueue*      queue,
        MamaSource*     source,
        const char*     symbol,
        void*           closure)
    {
        mImpl.addBroadcastStorySource (queue, 
                                       source, 
                                       symbol, 
                                       closure);
    }

    void MamdaNewsManager::clearBroadcastHeadlineSources ()
    {
        mImpl.clearBroadcastHeadlineSources ();
    }

    void MamdaNewsManager::clearBroadcastStorySources ()
    {
        mImpl.clearBroadcastStorySources ();
    }

    void MamdaNewsManager::addBroadcastHeadlineHandler (
        MamdaNewsHeadlineHandler*  handler)
    {
        mImpl.mHeadlineHandlers.push_back (handler);
    }

    void MamdaNewsManager::addBroadcastStoryHandler (
        MamdaNewsStoryHandler*  handler)
    {
        mImpl.mStoryHandlers.push_back (handler);
    }

    void MamdaNewsManager::addQualityHandler (
        MamdaQualityListener*  handler)
    {
        mImpl.mQualityHandlers.push_back (handler);
    }

    void MamdaNewsManager::addErrorHandler (
        MamdaErrorListener*  handler)
    {
        mImpl.mErrorHandlers.push_back (handler);
    }

    void MamdaNewsManager::requestStory (
        const MamdaNewsHeadline&  headline,
        MamdaNewsStoryHandler*    handler,
        void*                     closure)
    {
        mImpl.requestStory (headline, 
                            handler, 
                            closure);
    }

    void MamdaNewsManager::requestStory (
        MamaQueue*                queue,
        MamaSource*               source,
        MamdaNewsStoryId          storyId,
        MamdaNewsStoryHandler*    handler,
        void*                     closure)
    {
        mImpl.requestStory (queue, 
                            source, 
                            storyId, 
                            handler, 
                            closure);
    }

    void MamdaNewsManager::requestStoryLater (
        const MamdaNewsHeadline&  headline,
        MamdaNewsStoryHandler*    handler,
        double                    seconds,
        void*                     closure)
    {
        mImpl.requestStoryLater (headline, 
                                 handler, 
                                 seconds, 
                                 closure);
    }

    void MamdaNewsManager::requestStoryLater (
        const MamdaNewsStory&     story,
        MamdaNewsStoryHandler*    handler,
        double                    seconds,
        void*                     closure)
    {
        mImpl.requestStoryLater (story, 
                                 handler, 
                                 seconds, 
                                 closure);
    }

    void MamdaNewsManager::requestStoryLater (
        MamaQueue*                queue,
        MamaSource*               source,
        MamdaNewsStoryId          storyId,
        MamdaNewsStoryHandler*    handler,
        double                    seconds,
        void*                     closure)
    {
        mImpl.requestStoryLater (queue, 
                                 source, 
                                 storyId, 
                                 handler, 
                                 seconds, 
                                 closure);
    }

    void MamdaNewsManager::executeQuery (
        MamaQueue*                queue,
        MamaSource*               source,
        MamdaQuery*               newsQuery,
        MamdaNewsQueryType        queryType,
        MamdaNewsQueryHandler*    handler,
        void*                     closure)
    {
        mImpl.executeQuery (queue, 
                            source, 
                            newsQuery, 
                            queryType, 
                            handler, 
                            closure);
    }

    void MamdaNewsManager::removeQuery (
        MamdaQuery*  newsQuery)
    {
        mImpl.removeQuery (newsQuery);
    }

    void MamdaNewsManager::clearQuerySources ()
    {
        mImpl.clearQuerySources ();
    }

    MamdaNewsManager::MamdaNewsManagerImpl::MamdaNewsManagerImpl (
        MamdaNewsManager&  manager)
        : mManager (manager),
	    mPublisher (NULL),
	    mXmlString (NULL),
	    mMessage   (NULL)
    {
        const char * timeoutString = Mama::getProperty ("mama.properties.server.timeout");
        if( timeoutString == NULL )
        {
           timeoutString = "10";
        }
        timeoutValue = atoi (timeoutString);
    }

    MamdaNewsManager::MamdaNewsManagerImpl::~MamdaNewsManagerImpl()
    {
        clearBroadcastHeadlineSources ();
        clearBroadcastStorySources ();
    }

    void MamdaNewsManager::MamdaNewsManagerImpl::requestStory (
        const MamdaNewsHeadline&  headline,
        MamdaNewsStoryHandler*    handler,
        void*                     closure)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaNewsManager: got request for story ID: %s",
                  headline.getNewsStoryId());

        requestStory (headline.getQueue(),
                      headline.getSource(),
                      headline.getNewsStoryId(),
                      handler,
                      closure);
    }

    void MamdaNewsManager::MamdaNewsManagerImpl::requestStory (
        MamaQueue*                queue,
        MamaSource*               source,
        MamdaNewsStoryId          storyId,
        MamdaNewsStoryHandler*    handler,
        void*                     closure)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaNewsManager: got request for story ID: %s",
                  storyId);

        MamdaSubscription*  storySubsc    = new MamdaSubscription;
        StoryMsgHandler* aStoryMsgHandler = new StoryMsgHandler (*this, handler);

        storySubsc->addMsgListener     (aStoryMsgHandler);
        storySubsc->addErrorListener   (aStoryMsgHandler);
        storySubsc->addQualityListener (aStoryMsgHandler);
        storySubsc->setMdDataType      (MAMA_MD_DATA_TYPE_NEWS_STORY);

        storySubsc->create (queue, 
                            source, 
                            storyId, 
                            closure);
    }

    void MamdaNewsManager::MamdaNewsManagerImpl::executeQuery (
        MamaQueue*                queue,
        MamaSource*               source,
        MamdaQuery*               newsQuery,
        MamdaNewsQueryType        queryType,
        MamdaNewsQueryHandler*    handler,
        void*                     closure)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaNewsManager: got query");
        
        if (!mPublisher)
        {
           mPublisher = new MamaPublisher;
           mPublisher->create (source->getTransport(), 
                               "TOPIC_NEWS_QUERY", 
                               NULL, 
                               NULL);
                  
           mMessage = new MamaMsg;
           mMessage->create ();
        }
            
         mama_log (MAMA_LOG_LEVEL_FINE,
                   "MamdaNewsManager: query depth = %d", 
                   newsQuery->getDepth());
         
        char *queryString;

        if (newsQuery->getQuery(queryString))
        {    
           mMessage->clear();

           mMessage->addU8 (MamaReservedFields::MsgType,     MAMA_MSG_TYPE_NEWS_QUERY);
           mMessage->addU8 (MamaReservedFields::AppDataType, MAMA_MD_DATA_TYPE_NEWS_QUERY);
         
           mMessage->addI32 (QUERY_TYPE_FLD_NAME, 
                             QUERY_TYPE_FLD_FID, 
                             queryType);

           mMessage->addString (QUERY_FLD_NAME, 
                                QUERY_FLD_FID, 
                                queryString);
            
            MamaTimer *mTimeout = new MamaTimer;
            MamaInbox *aInbox   = new MamaInbox;

            QueryMsgHandler* aQueryHandler = new QueryMsgHandler (*this, 
                                                                  handler, 
                                                                  source, 
                                                                  queue, 
                                                                  newsQuery, 
                                                                  queryType, 
                                                                  mTimeout,
                                                                  closure);
        
            aInbox->create (source->getTransport(), 
                            queue, 
                            aQueryHandler, 
                            NULL);

            mPublisher->sendFromInbox (aInbox, mMessage);
              
            mTimeout->create (queue, 
                              aQueryHandler, 
                              timeoutValue, 
                              aInbox);
        
            delete queryString;
        }
    }

    void MamdaNewsManager::MamdaNewsManagerImpl::removeQuery (MamdaQuery*  newsQuery)
    {
        MamdaSubscription *aSub = newsQuery->getSubscriptionInfo();
        
        if (aSub)
        {
            removeSubscription (aSub);
            newsQuery->setSubscriptionInfo (NULL);
        }
    }


    class DelayedStoryRequest : public MamaTimerCallback
    {
    public:
        DelayedStoryRequest (
            MamdaNewsManager::MamdaNewsManagerImpl&  impl,
            MamaQueue*                               queue,
            MamaSource*                              source,
            MamdaNewsStoryId                         storyId,
            MamdaNewsStoryHandler*                   handler,
            void*                                    closure)
            : mImpl      (impl)
            , mQueue     (queue)
            , mSource    (source)
            , mStoryId   (storyId)
            , mHandler   (handler)
            , mClosure   (closure)
        {}

        virtual ~DelayedStoryRequest() {}

        virtual void onTimer (MamaTimer*  timer)
        {
            mImpl.requestStory (mQueue, 
                                mSource,
                                mStoryId.c_str(), 
                                mHandler, 
                                mClosure);

            delete timer;
            delete this;
        }

    private:
        MamdaNewsManager::MamdaNewsManagerImpl&  mImpl;
        MamaQueue*                               mQueue;
        MamaSource*                              mSource;
        string                                   mStoryId;
        MamdaNewsStoryHandler*                   mHandler;
        void*                                    mClosure;    
    };

    void MamdaNewsManager::MamdaNewsManagerImpl::requestStoryLater (
        const MamdaNewsHeadline&  headline,
        MamdaNewsStoryHandler*    handler,
        double                    seconds,
        void*                     closure)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaNewsManager: got delayed request for story ID: %s",
                  headline.getNewsStoryId());

        requestStoryLater (headline.getQueue(),
                           headline.getSource(),
                           headline.getNewsStoryId(),
                           handler,
                           seconds,
                           closure);
    }

    void MamdaNewsManager::MamdaNewsManagerImpl::requestStoryLater (
        const MamdaNewsStory&     story,
        MamdaNewsStoryHandler*    handler,
        double                    seconds,
        void*                     closure)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaNewsManager: got delayed request for story ID: %s",
                  story.getNewsStoryId());

        requestStoryLater (story.getQueue(),
                           story.getSource(),
                           story.getNewsStoryId(),
                           handler,
                           seconds,
                           closure);
    }

    void MamdaNewsManager::MamdaNewsManagerImpl::requestStoryLater (
        MamaQueue*                queue,
        MamaSource*               source,
        MamdaNewsStoryId          storyId,
        MamdaNewsStoryHandler*    handler,
        double                    seconds,
        void*                     closure)
    {
        mama_log (MAMA_LOG_LEVEL_FINE,
                  "MamdaNewsManager: got delayed request for story ID: %s",
                  storyId);

        MamaTimer*  timer = new MamaTimer;
        timer->create (queue,
                       new DelayedStoryRequest (*this, 
                                                queue, 
                                                source,
                                                storyId, 
                                                handler, 
                                                closure),
                       seconds);
    }

    void MamdaNewsManager::MamdaNewsManagerImpl::addBroadcastHeadlineSource (
        MamaQueue*      queue,
        MamaSource*     source,
        const char*     symbol,
        void*           closure)
    {
        MamdaSubscription* subscription         = new MamdaSubscription;
        HeadlineMsgHandler* aHeadlineMsgHandler = new HeadlineMsgHandler(*this);

        subscription->addMsgListener     (aHeadlineMsgHandler);
        subscription->addErrorListener   (aHeadlineMsgHandler);
        subscription->addQualityListener (aHeadlineMsgHandler);

        subscription->setRequireInitial (false);

        subscription->create (queue, 
                              source, 
                              symbol, 
                              closure);

        mHeadlineSubscs.push_back (subscription);
    }

    void MamdaNewsManager::MamdaNewsManagerImpl::addBroadcastStorySource (
        MamaQueue*      queue,
        MamaSource*     source,
        const char*     symbol,
        void*           closure)
    {
        MamdaSubscription* subscription   = new MamdaSubscription;
        StoryMsgHandler* aStoryMsgHandler = new StoryMsgHandler (*this, NULL);

        subscription->addMsgListener     (aStoryMsgHandler);
        subscription->addErrorListener   (aStoryMsgHandler);
        subscription->addQualityListener (aStoryMsgHandler);

        subscription->setRequireInitial (false);

        subscription->create (queue, 
                              source, 
                              symbol, 
                              closure);

        mStorySubscs.push_back (subscription);
    }

    void MamdaNewsManager::MamdaNewsManagerImpl::clearBroadcastHeadlineSources ()
    {
        for (deque<MamdaSubscription*>::iterator i = mHeadlineSubscs.begin();
             i != mHeadlineSubscs.end();
             ++i)
        {
            removeSubscription(*i);
        }
        mHeadlineSubscs.clear();
    }

    void MamdaNewsManager::MamdaNewsManagerImpl::clearBroadcastStorySources ()
    {
        for (deque<MamdaSubscription*>::iterator i = mStorySubscs.begin();
             i != mStorySubscs.end();
             ++i)
        {
            removeSubscription(*i);
        }
        mStorySubscs.clear();
    }

    void MamdaNewsManager::MamdaNewsManagerImpl::clearQuerySources ()
    {
        for (deque<MamdaSubscription*>::iterator i = mQuerySubscs.begin();
             i != mQuerySubscs.end();
             ++i)
        {       
            removeSubscription(*i);
        }
        mQuerySubscs.clear();
    }

    void MamdaNewsManager::MamdaNewsManagerImpl::removeSubscription (
        MamdaSubscription* subscription)
    {
        vector<MamdaMsgListener*> mMsgListeners = subscription->getMsgListeners();
        delete subscription;
            
        for (vector<MamdaMsgListener*>::iterator j = mMsgListeners.begin();
         j != mMsgListeners.end(); ++j)
        {
            MamdaMsgListener* listener = *j;
            delete (listener);
        }
    }
            
    void MamdaNewsManager::MamdaNewsManagerImpl::handleHeadline (
        MamdaSubscription*  subscription,
        const MamaMsg&      msg,
        MamdaNewsHeadline*  headline)
    {
        /*
         * NOTE: fields which are enums can be pubished as integers if feedhandler
         * uses mama-publish-enums-as-ints.  It may also be possible for a feed to
         * publish the numerical value as a string. All enumerated fields must be handled
         * by getting the value based on the field type.
         */

        const char*   headlineText      = NULL;
        const char*   headlineId        = "";
        const char*   storyId           = "";
        const char*   sourceId          = "";
        const char*   origSourceId      = "";
        const char*   langId            = "";
        const char**  stringArray       = NULL;
        mama_size_t   stringArraySize   = 0;
        mama_u16_t    revNum            = 0;
        mama_seqnum_t eventSeqNum       = 0;
        bool          hasStory          = false;
        MamaDateTime  timestamp;
        MamdaNewsPriority priority      = MAMDA_NEWS_PRIORITY_NORMAL;
        const char*   priorityStr       = "";

        mama_log (MAMA_LOG_LEVEL_FINE, "MamdaNewsManager: received headline");

        if (!msg.tryString (MamdaNewsFields::HEADLINE_TEXT, headlineText))
        {
            mama_log (MAMA_LOG_LEVEL_NORMAL,
                      "MamdaNewsManager: could not unpack headline text!");
            return;
        }

        headline->setHeadlineText (headlineText);

        if (msg.tryString       (MamdaNewsFields::HEADLINE_ID,    headlineId))
            headline->setHeadlineId (headlineId);

        if (msg.tryString       (MamdaNewsFields::STORY_ID,       storyId))
            headline->setStoryId (storyId);

        if (msg.tryString       (MamdaNewsFields::ORIG_STORY_ID,  storyId))
            headline->setOrigStoryId (storyId);

        if (msg.tryString       (MamdaNewsFields::SOURCE_ID,      sourceId))
            headline->setSourceId (sourceId);

        if (msg.tryString       (MamdaNewsFields::ORIG_SOURCE_ID, origSourceId))
            headline->setOrigSourceId (origSourceId);

        if (msg.tryString       (MamdaNewsFields::LANGUAGE_ID,    langId))
            headline->setLanguageId (langId);

        if (msg.tryBoolean      (MamdaNewsFields::HAS_STORY,      hasStory))
            headline->setHasStory (hasStory);

        if (msg.tryVectorString (MamdaNewsFields::NATIVE_CODES,
                                 stringArray, stringArraySize))
            headline->setNativeCodes (stringArray, stringArraySize);

        if (msg.tryVectorString (MamdaNewsFields::NATIVE_SYMBOLS,
                                 stringArray, stringArraySize))
            headline->setNativeRelatedSymbols (stringArray, stringArraySize);

        if (msg.tryVectorString (MamdaNewsFields::INDUSTRIES,
                                 stringArray, stringArraySize))
            headline->setIndustries (stringArray, stringArraySize);

        if (msg.tryVectorString (MamdaNewsFields::MARKET_SECTORS,
                                 stringArray, stringArraySize))
            headline->setMarketSectors (stringArray, stringArraySize);

        if (msg.tryVectorString (MamdaNewsFields::REGIONS,
                                 stringArray, stringArraySize))
            headline->setRegions (stringArray, stringArraySize);

        if (msg.tryVectorString (MamdaNewsFields::COUNTRIES,
                                 stringArray, stringArraySize))
            headline->setCountries (stringArray, stringArraySize);

        if (msg.tryVectorString (MamdaNewsFields::PRODUCTS,
                                 stringArray, stringArraySize))
            headline->setProducts (stringArray, stringArraySize);

        if (msg.tryVectorString (MamdaNewsFields::TOPICS,
                                 stringArray, stringArraySize))
            headline->setTopics (stringArray, stringArraySize);

        if (msg.tryVectorString (MamdaNewsFields::MISC_CODES,
                                 stringArray, stringArraySize))
            headline->setMiscCodes (stringArray, stringArraySize);

        if (msg.tryVectorString (MamdaNewsFields::SYMBOLS,
                                 stringArray, stringArraySize))
            headline->setRelatedSymbols (stringArray, stringArraySize);

        if(msg.tryField(MamdaNewsFields::PRIORITY, &mTmpField))
        {
            switch(mTmpField.getType())
            {
                case MAMA_FIELD_TYPE_I8:
                case MAMA_FIELD_TYPE_U8:
                case MAMA_FIELD_TYPE_I16:
                case MAMA_FIELD_TYPE_U16:
                case MAMA_FIELD_TYPE_I32:
                case MAMA_FIELD_TYPE_U32:
                    priority = (MamdaNewsPriority)mTmpField.getU32();
                    headline->setPriority (priority);
                    break;

                case MAMA_FIELD_TYPE_STRING:
                    priorityStr = mTmpField.getString();

                    if ((strcmp(priorityStr,"0") == 0) || (strcmp(priorityStr,"") == 0))
                    {
                        headline->setPriority (MAMDA_NEWS_PRIORITY_NONE);
                    }
                    else if ((strcmp(priorityStr,"9") == 0) || (strcmp(priorityStr,"Hot") == 0))
                    {
                        headline->setPriority (MAMDA_NEWS_PRIORITY_HOT);
                    }
                    else
                    {
                        headline->setPriority (MAMDA_NEWS_PRIORITY_NORMAL);
                        mama_log (MAMA_LOG_LEVEL_FINE,
                                  "Unhandled value %s for wNewsPriority.", 
                                  priorityStr);
                    }
                    break;

                default:
                    mama_log (MAMA_LOG_LEVEL_FINE,
                              "Unhandled type %d for wNewsPriority.  "
                              "Expected string or integer.",
                              mTmpField.getType());
                    break;
            }
        }

        if (msg.tryU16          (MamdaNewsFields::REVISION_NUM,    revNum))
            headline->setStoryRevNumber (revNum);

        if (msg.tryDateTime     (MamdaNewsFields::TIME,            timestamp))
            headline->setEventTime (timestamp);

        if (msg.tryDateTime     (MamdaNewsFields::ORIG_STORY_TIME, timestamp))
            headline->setOrigStoryTime (timestamp);

        if (msg.tryU32          (MamdaCommonFields::SEQ_NUM,       eventSeqNum))
            headline->setEventSeqNum (eventSeqNum);

        if (msg.tryDateTime     (MamdaCommonFields::SRC_TIME,      timestamp))
            headline->setSrcTime (timestamp);

        if (msg.tryDateTime     (MamdaCommonFields::ACTIVITY_TIME, timestamp))
            headline->setActivityTime (timestamp);

        if (msg.tryDateTime     (MamdaCommonFields::LINE_TIME,     timestamp))
            headline->setLineTime (timestamp);

        mama_log (MAMA_LOG_LEVEL_FINE, "MamdaNewsManager: finished headline");
    }

    void MamdaNewsManager::MamdaNewsManagerImpl::handleStory (
        MamdaSubscription*      subscription,
        const MamaMsg&          msg,
        MamdaNewsStoryHandler*  replyHandler)
    {
        /*
         * NOTE: fields which are enums can be pubished as integers if feedhandler
         * uses mama-publish-enums-as-ints.  It may also be possible for a feed to
         * publish the numerical value as a string. All enumerated fields must be handled
         * by getting the value based on the field type.
         */

        const char*   storyText         = "";
        const char*   storyId           = "";
        mama_u16_t    revNum            = 0;
        MamaDateTime  storyTime;
        MamaDateTime  storyOrigTime;
        const char**  headlines         = NULL;
        mama_size_t   numHeadlines      = 0;
        const char* statusStr           = "";
        MamdaNewsStory::Status status   = MamdaNewsStory::UNKNOWN;

        mama_log (MAMA_LOG_LEVEL_FINE, "MamdaNewsManager: received news story");

        msg.tryString       (MamdaNewsFields::STORY_TEXT,      storyText);
        msg.tryString       (MamdaNewsFields::STORY_ID,        storyId);
        msg.tryU16          (MamdaNewsFields::REVISION_NUM,    revNum);

        if(msg.tryField(MamdaNewsFields::STORY_STATUS, &mTmpField))
        {
            switch(mTmpField.getType())
            {
                case MAMA_FIELD_TYPE_I8:
                case MAMA_FIELD_TYPE_U8:
                case MAMA_FIELD_TYPE_I16:
                case MAMA_FIELD_TYPE_U16:
                case MAMA_FIELD_TYPE_I32:
                case MAMA_FIELD_TYPE_U32:
                    status = (MamdaNewsStory::Status)mTmpField.getU32();
                    break;

                case MAMA_FIELD_TYPE_STRING:
                    statusStr = mTmpField.getString();

                    if ((strcmp(statusStr,"0") == 0) || (strcmp(statusStr,"NoStory") == 0))
                    {
                        status = MamdaNewsStory::NO_STORY;
                    }
                    else if ((strcmp(statusStr,"1") == 0) || (strcmp(statusStr,"Full") == 0))
                    {
                        status = MamdaNewsStory::FULL_STORY;
                    }
                    else if ((strcmp(statusStr,"2") == 0) || (strcmp(statusStr,"Fetching") == 0))
                    {
                        status = MamdaNewsStory::FETCHING_STORY;
                    }
                    else if ((strcmp(statusStr,"3") == 0) || (strcmp(statusStr,"Delayed") == 0))
                    {
                        status = MamdaNewsStory::DELAYED_STORY;
                    }
                    else if ((strcmp(statusStr,"4") == 0) || (strcmp(statusStr,"NotFound") == 0))
                    {
                        status = MamdaNewsStory::NOT_FOUND;
                    }
                    else if ((strcmp(statusStr,"5") == 0) || (strcmp(statusStr,"Deleted") == 0))
                    {
                        status = MamdaNewsStory::DELETED;
                    }
                    else if ((strcmp(statusStr,"6") == 0) || (strcmp(statusStr,"Expired") == 0))
                    {
                        status = MamdaNewsStory::EXPIRED;
                    }
                    else if ((strcmp(statusStr,"99") == 0) || (strcmp(statusStr,"Unknown") == 0))
                    {
                        status = MamdaNewsStory::UNKNOWN;
                    }
                    else
                    {
                        status = MamdaNewsStory::UNKNOWN;
                        mama_log (MAMA_LOG_LEVEL_FINE,
                                  "Unhandled value %s for wNewsStoryStatus.",
                                  statusStr);
                    }
                    break;

                default:
                    mama_log (MAMA_LOG_LEVEL_FINE,
                              "Unhandled type %d for wNewsStoryStatus.  "
                              "Expected string or integer.", 
                              mTmpField.getType());
                    break;
            }
        }

        msg.tryDateTime     (MamdaNewsFields::STORY_TIME,      storyTime);
        msg.tryDateTime     (MamdaNewsFields::ORIG_STORY_TIME, storyOrigTime);
        msg.tryVectorString (MamdaNewsFields::STORY_HEADLINES, headlines, numHeadlines);

        MamdaNewsStory  story;

        story.setStory (storyText, 
                        storyId, 
                        revNum,
                        (MamdaNewsStory::Status)status,
                        storyTime, 
                        storyOrigTime, 
                        headlines, 
                        numHeadlines);

        story.setSubscInfo (subscription->getQueue(), subscription->getSource());

        if (replyHandler)
        {
            replyHandler->onNewsStory (mManager, 
                                       msg, 
                                       story,
                                       subscription->getClosure());
        }
        else
        {
            deque<MamdaNewsStoryHandler*>::iterator end = mStoryHandlers.end();
            deque<MamdaNewsStoryHandler*>::iterator iter;

            for (iter = mStoryHandlers.begin(); iter != end; ++iter)
            {
                MamdaNewsStoryHandler*  handler = *iter;
                handler->onNewsStory (mManager, 
                                      msg, 
                                      story,
                                      subscription->getClosure());
            }
        }

        switch (status)
        {
            case MamdaNewsStory::NO_STORY:
            case MamdaNewsStory::FULL_STORY:
            case MamdaNewsStory::DELAYED_STORY:
            case MamdaNewsStory::NOT_FOUND:
            case MamdaNewsStory::DELETED:
            case MamdaNewsStory::EXPIRED:
            case MamdaNewsStory::UNKNOWN:
                break;
            case MamdaNewsStory::FETCHING_STORY:
                break;
        }

        if (replyHandler)
        {
            // This was a request for a single story, so we can delete the
            // subscription.
            delete subscription;
        }

        mama_log (MAMA_LOG_LEVEL_FINE, "MamdaNewsManager: finished news story");
    }

    void MamdaNewsManager::MamdaNewsManagerImpl::onQuality (
        MamdaSubscription*   subscription,
        mamaQuality          quality)
    {
        deque<MamdaQualityListener*>::iterator end = mQualityHandlers.end();
        deque<MamdaQualityListener*>::iterator iter;

        for (iter = mQualityHandlers.begin(); iter != end; ++iter)
        {
            MamdaQualityListener*  handler = *iter;

            handler->onQuality (subscription, quality);
        }
    }

    void MamdaNewsManager::MamdaNewsManagerImpl::onError (
        MamdaSubscription*   subscription,
        MamdaErrorSeverity   severity,
        MamdaErrorCode       code,
        const char*          errorStr)
    {
        deque<MamdaErrorListener*>::iterator end = mErrorHandlers.end();
        deque<MamdaErrorListener*>::iterator iter;
        for (iter = mErrorHandlers.begin(); iter != end; ++iter)
        {
            MamdaErrorListener*  handler = *iter;

            handler->onError (subscription, 
                              severity, 
                              code, 
                              errorStr);
        }
    }

} // namespace
