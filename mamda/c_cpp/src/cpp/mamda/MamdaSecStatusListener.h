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

#ifndef MamdaSecStatusListenerH
#define MamdaSecStatusListenerH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaMsgListener.h> 
#include <mamda/MamdaSecStatus.h>
#include <mamda/MamdaSecStatusRecap.h>
#include <mamda/MamdaSecurityStatus.h>
#include <mamda/MamdaSecurityStatusQual.h>

namespace Wombat
{

    class MamdaSecStatusHandler;

    /**
     * MamdaSecurityStatusListener is a class that specializes in handling
     * security status updates.  Developers provide their own
     * implementation of the MamdaSecurityStatusHandler interface and will
     * be delivered notifications for security status updates.
     *
     * Note: The MamdaSecurityStatusListener class caches some field
     * values.  Among other reasons, caching of these fields makes it
     * possible to provide complete certain callbacks, even when the
     * publisher (e.g., feed handler) is only publishing deltas
     * containing modified fields.
     *
     * MamdaSecStatusListener should initialize the MamdaSecStatusFields class 
     * prior to receiving the first message by calling
     * MamdaSecStatusFields::setDictionary() with a valid dictionary object
     * which contains SecStatus related fields.
     */
    class MAMDAExpDLL MamdaSecStatusListener 
        : public MamdaMsgListener
        , public MamdaSecStatus
        , public MamdaSecStatusRecap
    {
    public:

        /**
         * Create a specialized security status listener.
         */
        MamdaSecStatusListener ();
        virtual ~MamdaSecStatusListener ();

        /**
         * Add a specialized security status handler.
         */
        void        addHandler (MamdaSecStatusHandler* handler);

        // Inherited from MamdaBasicEvent
        const char*             getSymbol                       () const;
        const char*             getPartId                       () const;
        const MamaDateTime&     getSrcTime                      () const;
        const MamaDateTime&     getActivityTime                 () const;
        const MamaDateTime&     getLineTime                     () const;
        const MamaDateTime&     getSendTime                     () const;
        const MamaMsgQual&      getMsgQual                      () const;
        const MamaDateTime&     getEventTime                    () const;
        mama_seqnum_t           getEventSeqNum                  () const;
        
        MamdaFieldState         getSymbolFieldState             () const;
        MamdaFieldState         getPartIdFieldState             () const;
        MamdaFieldState         getSrcTimeFieldState            () const;
        MamdaFieldState         getActivityTimeFieldState       () const;
        MamdaFieldState         getLineTimeFieldState           () const;
        MamdaFieldState         getSendTimeFieldState           () const;
        MamdaFieldState         getMsgQualFieldState            () const;
        MamdaFieldState         getEventTimeFieldState          () const;
        MamdaFieldState         getEventSeqNumFieldState        () const;


        const char*             getIssueSymbol                  () const;
        const char*             getReason                       () const;
        char                    getSecurityAction               () const;
        char                    getShortSaleCircuitBreaker      () const;
        const char*             getSecurityType                 () const;
        const char*             getSecurityStatus               () const;
        const char*             getSecurityStatusQual           () const; 
        const char*             getSecurityStatusNative         () const; 
        const char*             getSecurityStatusStr            () const;
        const char*             getSecurityStatusQualStr        () const;
        MamdaSecurityStatus     getSecurityStatusEnum           () const; 
        MamdaSecurityStatusQual getSecurityStatusQualifierEnum  () const; 
        const char*             getFreeText                     () const;
        
        MamdaFieldState         getIssueSymbolFieldState                 () const;
        MamdaFieldState         getReasonFieldState                      () const;
        MamdaFieldState         getSecurityActionFieldState              () const;
        MamdaFieldState         getShortSaleCircuitBreakerFieldState     () const;
        MamdaFieldState         getSecurityTypeFieldState                () const;
        MamdaFieldState         getSecurityStatusFieldState              () const;
        MamdaFieldState         getSecurityStatusQualFieldState          () const; 
        MamdaFieldState         getSecurityStatusNativeFieldState        () const; 
        MamdaFieldState         getSecurityStatusStrFieldState           () const;
        MamdaFieldState         getSecurityStatusQualStrFieldState       () const;
        MamdaFieldState         getSecurityStatusEnumFieldState          () const; 
        MamdaFieldState         getSecurityStatusQualifierEnumFieldState () const; 
        MamdaFieldState         getFreeTextFieldState                    () const;

        /**
         * Implementation of MamdaListener interface.
         */
        virtual void onMsg (MamdaSubscription*  subscription,
                            const MamaMsg&      msg,
                            short               msgType);

        struct MamdaSecStatusListenerImpl;

    private:
        MamdaSecStatusListenerImpl& mImpl;
    };

}

#endif // MamdaSecStatusListenerH
