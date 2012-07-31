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

#ifndef SUBSCRIPTIONIMPL_CPP_H__
#define SUBSCRIPTIONIMPL_CPP_H__

#include "mama/mamacpp.h"
#include "mamacppinternal.h"
#include "BasicSubscriptionImpl.h"

namespace Wombat
{
class SubscriptionImpl : public BasicSubscriptionImpl
{
public:
    void setup (
        MamaSubscription*           cppSubscription,
        MamaTransport*              transport,
        MamaQueue*                  queue,
        MamaSubscriptionCallback*   callback,
        const char*                 source,
        const char*                 symbol,
        void*                       closure);

    void setup (
        MamaSubscription*           cppSubscription,
        MamaQueue*                  queue,
        MamaSubscriptionCallback*   callback,
        MamaSource*                 source,
        const char*                 symbol,

        void*                       closure);

    void activate ();
    void deactivate ();
    
    SubscriptionImpl (void) 
       : mItemClosure(NULL)
       , mCppSubscription(NULL)
       , mCallback(NULL)
       , mSourceDeriv(NULL)       
       {}
   
    virtual ~SubscriptionImpl (void);

    virtual mamaSubscriptionState getState(void);
    virtual const char*   getSymbol     (void) const;
    virtual void          setSymbol     (const char* symbol);
    virtual const char*   getSubscSource (void) const;

    virtual void   setTimeout (double timeout);
    virtual double getTimeout (void);

    virtual void setRetries (int retries);
    virtual int  getRetries (void);

    void setRequiresInitial  (bool val)
    {
        mamaTry(mamaSubscription_setRequiresInitial(
                    mSubscription, val==true?1:0));
    }
    
    bool getRequiresInitial (void)     
    { 
        int  requiresInitial = 0;
        mamaTry(mamaSubscription_getRequiresInitial(
                    mSubscription,&requiresInitial));
        return requiresInitial==1?true:false;
    }

    bool getReceivedInitial (void)
    {
        int  receivedInitial = 0;
        mamaTry(mamaSubscription_getReceivedInitial(
                    mSubscription,&receivedInitial));
        return receivedInitial==1?true:false;
    }

    virtual void setRecoverGaps (bool recover);
    virtual bool getRecoverGaps (void        ) const;

    void setServiceLevel (mamaServiceLevel svcLevel, long svcLevelOpt) 
    {
        mamaTry(mamaSubscription_setServiceLevel(
                    mSubscription,svcLevel,svcLevelOpt));
    }
    
    mamaServiceLevel getServiceLevel (void)
    {
        mamaServiceLevel svcLevel;
        mamaTry(mamaSubscription_getServiceLevel(
                    mSubscription,&svcLevel));
        return svcLevel;
    }
    
    long getServiceLevelOpt (void)
    {
        long svcLevelOpt = 0;
        mamaTry(mamaSubscription_getServiceLevelOpt(
                   mSubscription,&svcLevelOpt));
        return svcLevelOpt;
    }

    MamaSubscription* getSubscription(void) const 
    {
        return mCppSubscription;
    }
    
    void setSubscriptionType (mamaSubscriptionType type) 
    {
        mamaTry(mamaSubscription_setSubscriptionType(
                    mSubscription,type));
    }

    mamaSubscriptionType getSubscriptionType (void)
    {
        mamaSubscriptionType type;
        mamaTry (mamaSubscription_getSubscriptionType(
                    mSubscription,&type));
        return type;
    }

    void    setAppDataType (uint8_t dataType)
    {
        mamaTry (mamaSubscription_setAppDataType(
                    mSubscription, dataType));
    }
    
    uint8_t  getAppDataType (void) const
    {
        uint8_t dataType = 0U;
        mamaTry (mamaSubscription_getAppDataType(
                    mSubscription,&dataType));
        return dataType;
    }
    
    void setItemClosure (void*            closure)
    {
        mamaTry (mamaSubscription_setItemClosure(
                    mSubscription, closure));
    }
    
    void* getItemClosure (void)
    {
        return mItemClosure;
    }
    
    void setPreInitialCacheSize (int cacheSize)
    {
        mamaTry (mamaSubscription_setPreIntitialCacheSize (mSubscription,cacheSize));
    }
    
    int getPreInitialCacheSize (void)
    {
        int result = 0;
        mamaTry (mamaSubscription_getPreIntitialCacheSize(mSubscription,&result));
        return result;
    }

    virtual void setGroupSizeHint (int groupSizeHint);
    
    void setMsgQualifierFilter( 
        bool ignoreDefinitelyDuplicate,
        bool ignorePossiblyDuplicate,
        bool ignoreDefinitelyDelyaed,
        bool ignorePossiblyDelayed,
        bool ignoreOutOfSequence)
    {
        mamaTry (mamaSubscription_setMsgQualifierFilter (mSubscription,                                                     
														ignoreDefinitelyDuplicate,
                                                        ignorePossiblyDuplicate,
                                                        ignoreDefinitelyDelyaed,
                                                        ignorePossiblyDelayed,
                                                        ignoreOutOfSequence));
    }
    
    void getMsgQualifierFilter( 
        bool ignoreDefinitelyDuplicate,
        bool ignorePossiblyDuplicate,
        bool ignoreDefinitelyDelyaed,
        bool ignorePossiblyDelayed,
        bool ignoreOutOfSequence) const
    {
        int iDDup = 0;
        int iPDup = 0;
        int iDDel = 0;
        int iPDel = 0;
        int iSeq  = 0;

        mamaTry (mamaSubscription_getMsgQualifierFilter (mSubscription,
                                                        &iDDup,
                                                        &iPDup,
                                                        &iDDel,
                                                        &iPDel,
                                                        &iSeq));

        ignoreDefinitelyDuplicate = iDDup != 0;
        ignorePossiblyDuplicate   = iPDup != 0;  
        ignoreDefinitelyDelyaed   = iDDel != 0;
        ignorePossiblyDelayed     = iPDel != 0;
        ignoreOutOfSequence       = iSeq  != 0;
    }

    MamaSubscriptionCallback* getCallback (void) const {return mCallback;}
    
    void*                      mItemClosure;
    MamaSubscription*          mCppSubscription; 
    MamaSubscriptionCallback*  mCallback;
    MamaSourceDerivative*      mSourceDeriv;    
    	
protected:

    /* Protected functions. */
    virtual void cleanup(void);

private:

    /* Private functions. */    
    static void MAMACALLTYPE subscriptionImplCreateCb (mamaSubscription subsc, void *closure);
    static void MAMACALLTYPE subscriptionImplQualityCb (mamaSubscription subsc, mamaQuality quality, const char* symbol, short cause, const void* platformInfo, void* closure);
    static void MAMACALLTYPE subscriptionImplErrorCb (mamaSubscription subsc, mama_status status, void *platformError, const char *subject, void *closure);
    static void MAMACALLTYPE subscriptionImplMsgCb (mamaSubscription subsc, mamaMsg msg, void *closure, void *itemClosure);
    static void MAMACALLTYPE subscriptionImplGapCb (mamaSubscription subscription, void* closure);
    static void MAMACALLTYPE subscriptionImplRecapRequestCb (mamaSubscription subscription, void* closure);    
};
}

#endif

