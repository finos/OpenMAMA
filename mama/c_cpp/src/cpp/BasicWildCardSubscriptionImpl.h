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

#ifndef BASIC_WC_SUBSCRIPTIONIMPL_CPP_H__
#define BASIC_WC_SUBSCRIPTIONIMPL_CPP_H__

#include "mama/mamacpp.h"
#include "mamacppinternal.h"
#include "BasicSubscriptionImpl.h"

namespace Wombat
{
class BasicWildCardSubscriptionImpl : public BasicSubscriptionImpl
{
public:
    
    BasicWildCardSubscriptionImpl (void) 
       : mTopicClosure(NULL)
       , mCppSubscription(NULL)
       , mCallback(NULL)
       {}
   
    void create (MamaBasicWildCardSubscription*           subscription,
                 MamaTransport*                           transport,
                 MamaQueue*                               queue,
                 MamaBasicWildCardSubscriptionCallback*   callback,
                 const char*                              source,
                 const char*                              symbol,
                 void*                                    closure = NULL);

    virtual ~BasicWildCardSubscriptionImpl (void);

    virtual const char*   getSymbol     (void) const;
    virtual const char*   getSource (void) const;

    MamaBasicWildCardSubscription* getSubscription(void) const 
    {
        return mCppSubscription;
    }
    
    void setTopicClosure (void*            closure)
    {
        mamaTry (mamaSubscription_setItemClosure(
                    mSubscription, closure));
    }
    
    void muteCurrentTopic (void)
    {
        mamaTry (mamaSubscription_muteCurrentTopic(
                    mSubscription));
    }
    
    void* getTopicClosure (void)
    {
        return mTopicClosure;
    }
    
    void*                                   mTopicClosure;
    MamaBasicWildCardSubscription*          mCppSubscription; 
    MamaBasicWildCardSubscriptionCallback*  mCallback;    

protected:

    // Protected functions
    virtual void cleanup(void);

private:

    // Private functions
    static void MAMACALLTYPE subscriptionWcImplCreateCb (mamaSubscription subsc, void *closure);
    static void MAMACALLTYPE subscriptionWcImplErrorCb (mamaSubscription subsc, mama_status status, void *platformError, const char *subject, void *closure);
    static void MAMACALLTYPE subscriptionWcImplMsgCb (mamaSubscription subsc, mamaMsg msg, const char *topic, void *closure, void *topicClosure);
};
}

#endif

