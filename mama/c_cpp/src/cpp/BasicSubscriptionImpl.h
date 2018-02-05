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

#include "mama/mamacpp.h"
#include "mamacppinternal.h"
#include "mamainternal.h"
#include "../c/mama/subscription.h"

extern "C"
{
    #include "destroyHandle.h"
}


namespace Wombat
{
class BasicSubscriptionImpl 
{
public:
    BasicSubscriptionImpl (void);

    virtual ~BasicSubscriptionImpl (void);
    
    void create (
        MamaBasicSubscription*          cppSubscription,
        MamaTransport*             transport,
        MamaQueue*                      queue,
        MamaBasicSubscriptionCallback*  callback,
        const char*                     topic,
        void*                           closure);
   

    virtual void destroy ();
    virtual void destroyEx();
    virtual bool isActive () const;

    virtual const char* getTopic (void) const 
    {
        const char* topic  = NULL;
        mamaTry(mamaSubscription_getSubscSymbol(
                    mSubscription,&topic));
        return topic; 
    }

    virtual MamaTransport*       getTransport (void) const;
    virtual MamaBasicSubscriptionCallback* getBasicCallback  (void) const
    {
        return mCallback;
    }

    virtual MamaQueue*   getQueue        (void) const;
    virtual void*        getClosure      (void) const;
    virtual void         setClosure      (void*  closure);
    virtual void         setDebugLevel   (MamaLogLevel  level);
    virtual MamaLogLevel getDebugLevel   () const;
    virtual bool         checkDebugLevel (MamaLogLevel  level) const;

    MamaBasicSubscription* getSubscription (void) const {return mCppSubscription;}
       
    MamaBasicSubscription*          mCppSubscription;
    mamaSubscription                mSubscription;
    MamaBasicSubscriptionCallback*  mCallback;
    MamaTransport*                  mTransport;
    MamaQueue*                      mQueue;
    void*                           mClosure;
        
    /* This message is used during the subscription callback to avoid having
     * to create a new MamaMsg object each time it is invoked.
     */
	MamaMsg                         mReuseableMsg;


protected:

    /* This flag is held for convenience and will be set if the callback
     * passed to the create function is an extended callback supporting
     * the destroy function.
     */
    bool mIsExCallback;

    /* The destroy handle will ensure that there is no access violation if the on
     * destroy callback is invoked after the C++ class has been deleted.
     */
    pDestroyHandle mDestroyHandle;

    // Protected functions
    virtual void cleanup(void);
    static void MAMACALLTYPE onSubscriptionDestroy(mamaSubscription subscription, void *closure);

private:

    /* Private functions. */    
    static void MAMACALLTYPE onSubscriptionCreate(mamaSubscription subsc, void *closure);    
    static void MAMACALLTYPE onSubscriptionError(mamaSubscription subsc, mama_status status, void *platformError, const char *subject, void *closure);
    static void MAMACALLTYPE onSubscriptionMessage(mamaSubscription subsc, mamaMsg msg, void *closure, void *itemClosure);
};

} // namespace Wombat 
