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

#include "mama/MamaFt.h"
#include "mamacppinternal.h"
#include <assert.h>
namespace Wombat
{

    MamaFtMember::MamaFtMember ()
        : mCValue    (NULL)
        , mCallback  (NULL)
        , mClosure   (NULL)
    {
        mamaTry (mamaFtMember_create (&mCValue));
    }

    MamaFtMember::~MamaFtMember ()
    {
        destroy ();
    }

    extern "C"
    {
        static void MAMACALLTYPE mamaFtCallbackWrapper (
                                           mamaFtMember  ftMember,
                                           const char*   groupName,
                                           mamaFtState   state,
                                           void*         closure)
        {
            MamaFtMember* cppMember = (MamaFtMember*)closure;
            assert (cppMember != NULL);
            MamaFtMemberCallback*  callback = cppMember->getCallback ();
            if (callback)
            {
                callback->onFtStateChange (cppMember, groupName, state);
            }
        }
    }

    void MamaFtMember::destroy ()
    {
        if (mCValue != NULL)
        {
            mamaTry (mamaFtMember_destroy (mCValue));
            mCValue = NULL;
        }
    }

    void MamaFtMember::activate ()
    {
        mamaTry (mamaFtMember_activate (mCValue));
    }

    void MamaFtMember::deactivate ()
    {
        mamaTry (mamaFtMember_deactivate (mCValue));
    }

    bool MamaFtMember::isActive () const
    {
        int result = 0;
        mamaTry (mamaFtMember_isActive (mCValue, &result));
        return (bool)result;
    }

    const char* MamaFtMember::getGroupName () const
    {
        const char* result = NULL;
        mamaTry (mamaFtMember_getGroupName (mCValue, &result));
        return result;
    }

    mama_u32_t MamaFtMember::getWeight () const
    {
        mama_u32_t result = 0;
        mamaTry (mamaFtMember_getWeight (mCValue, &result));
        return result;
    }

    mama_f64_t MamaFtMember::getHeartbeatInterval () const
    {
        mama_f64_t result = 0.0;
        mamaTry (mamaFtMember_getHeartbeatInterval (mCValue, &result));
        return result;
    }

    mama_f64_t MamaFtMember::getTimeoutInterval () const
    {
        mama_f64_t result = 0.0;
        mamaTry (mamaFtMember_getTimeoutInterval (mCValue, &result));
        return result;
    }

    MamaFtMemberCallback* MamaFtMember::getCallback () const
    {
        return mCallback;
    }

    void* MamaFtMember::getClosure () const
    {
        return mClosure;
    }

    void MamaFtMember::setWeight (mama_u32_t  weight)
    {
        mamaTry (mamaFtMember_setWeight (mCValue, weight));
    }

    void MamaFtMember::setInstanceId (const char*  id)
    {
        mamaTry (mamaFtMember_setInstanceId (mCValue, id));
    }

    void MamaMulticastFtMember::setup (MamaQueue*             queue,
                                       MamaFtMemberCallback*  handler,
                                       MamaTransport*         transport,
                                       const char*            groupName,
                                       mama_u32_t             weight,
                                       mama_f64_t             heartbeatInterval,
                                       mama_f64_t             timeoutInterval,
                                       void*                  closure)
    {
        if (mCValue == NULL)
        {
            throw MamaStatus (MAMA_STATUS_INVALID_ARG);
        }

        mClosure  = closure;
        mCallback = handler;

        mamaQueue cQueue = queue ? queue->getCValue () : NULL;

        mamaTry (mamaFtMember_setup (mCValue,
				                     MAMA_FT_TYPE_MULTICAST,
                                     cQueue, 
                                     mamaFtCallbackWrapper,
                                     transport ? transport->getCValue () : NULL,
                                     groupName,
                                     weight,
                                     heartbeatInterval,
                                     timeoutInterval,
                                     (void*)this));
    }

    void MamaBridgeFtMember::setup (MamaQueue*             queue,
                                    MamaFtMemberCallback*  handler,
                                    MamaTransport*         transport,
                                    const char*            groupName,
                                    mama_u32_t             weight,
                                    mama_f64_t             heartbeatInterval,
                                    mama_f64_t             timeoutInterval,
                                    void*                  closure)
    {
        if (mCValue == NULL)
        {
            throw MamaStatus (MAMA_STATUS_INVALID_ARG);
        }

        mClosure  = closure;
        mCallback = handler;

        mamaQueue cQueue = queue ? queue->getCValue () : NULL;

        mamaTry (mamaFtMember_setup (mCValue,
				                     MAMA_FT_TYPE_BRIDGE,
                                     cQueue, 
                                     mamaFtCallbackWrapper,
                                     transport->getCValue (),
                                     groupName,
                                     weight,
                                     heartbeatInterval,
                                     timeoutInterval,
                                     (void*)this));
    }

}
