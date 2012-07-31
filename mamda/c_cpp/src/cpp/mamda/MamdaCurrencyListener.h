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

#ifndef MamdaCurrencyListenerH
#define MamdaCurrencyListenerH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaMsgListener.h>
#include <mamda/MamdaCurrencyRecap.h>
#include <mamda/MamdaCurrencyUpdate.h>

namespace Wombat
{

    class MamdaCurrencyHandler;

    /**
     * MamdaCurrencyListener is a class that specializes in handling
     * currency data, Developers provide their own implementation of the
     * MamdaCurrencyHandler interface and will be delivered
     * notifications for updates in the currency data.  An obvious
     * application for this MAMDA class is any kind of currency analysis
     * application.
     *
     * MamdaCurrencyListener should initialize the MamdaCurrencyFields class 
     * prior to receiving the first message by calling
     * MamdaCurrencyFields::setDictionary() with a valid dictionary object
     * which contains Currency related fields.
     */

    class MAMDAExpDLL MamdaCurrencyListener 
        : public MamdaMsgListener
        , public MamdaCurrencyRecap
        , public MamdaCurrencyUpdate
    {
    public:
        MamdaCurrencyListener ();
        virtual ~MamdaCurrencyListener ();

        void addHandler (MamdaCurrencyHandler* handler);

        // Inherited from  MamdaBasicRecap
        const char*          getSymbol()        const;
        const char*          getPartId()        const;
        const MamaDateTime&  getSrcTime()       const;
        const MamaDateTime&  getActivityTime()  const;
        const MamaDateTime&  getLineTime()      const;
        const MamaDateTime&  getSendTime()      const;
        mama_seqnum_t        getEventSeqNum()   const;
        const MamaDateTime&  getEventTime()     const;
        const MamaMsgQual&   getMsgQual()       const;
        bool                 isInitialised()    const;
        
        MamdaFieldState     getSymbolFieldState()       const;
        MamdaFieldState     getPartIdFieldState()       const;
        MamdaFieldState     getSrcTimeFieldState()      const;
        MamdaFieldState     getActivityTimeFieldState() const;
        MamdaFieldState     getLineTimeFieldState()     const;
        MamdaFieldState     getSendTimeFieldState()     const;
        MamdaFieldState     getEventSeqNumFieldState()  const;
        MamdaFieldState     getEventTimeFieldState()    const;
        MamdaFieldState     getMsgQualFieldState()      const;

        // Inherited from MamdaCurrency
        const MamaPrice&    getBidPrice()       const;
        const MamaPrice&    getAskPrice()       const;

        MamdaFieldState     getBidPriceFieldState()  const;
        MamdaFieldState     getAskPriceFieldState()   const;

        /**
         * Implementation of MamdaListener interface.\n
         * \throw <MamaStatus> {MAMDA default implementation.}
         */
        virtual void onMsg (MamdaSubscription*  subscription,
                            const MamaMsg&      msg,
                            short               msgType);

        struct MamdaCurrencyListenerImpl;
    private:
        MamdaCurrencyListenerImpl& mImpl;

    };


} // namespace

#endif // MamdaCurrencyListenerH





