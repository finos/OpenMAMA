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

#ifndef MamdaOrderBookConcreteSimpleDeltaH
#define MamdaOrderBookConcreteSimpleDeltaH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaOrderBookSimpleDelta.h>

namespace Wombat
{

    /**
     * MamdaOrderBookConcreteSimpleDelta is a class that saves information
     * about a simple order book delta.  A simple delta is one that
     * affects a single order book entry.
     */

    class MAMDAOPTExpDLL MamdaOrderBookConcreteSimpleDelta : public MamdaOrderBookSimpleDelta
    {
    public:
        MamdaOrderBookConcreteSimpleDelta ();
        virtual ~MamdaOrderBookConcreteSimpleDelta ();

        /*
         * The following methods are inherited from MamdaBasicEvent.
         */
        virtual const char*          getSymbol()       const;
        virtual const char*          getPartId()       const;
        virtual mama_seqnum_t        getEventSeqNum()  const;
        virtual const MamaDateTime&  getEventTime()    const;
        virtual const MamaDateTime&  getSrcTime()      const;
        virtual const MamaDateTime&  getActivityTime() const;
        virtual const MamaDateTime&  getLineTime()     const;
        virtual const MamaDateTime&  getSendTime()     const;
        virtual const MamaMsgQual&   getMsgQual()      const;
        
        virtual MamdaFieldState     getSymbolFieldState()       const;
        virtual MamdaFieldState     getPartIdFieldState()       const;
        virtual MamdaFieldState     getEventSeqNumFieldState()  const;
        virtual MamdaFieldState     getEventTimeFieldState()    const;
        virtual MamdaFieldState     getSrcTimeFieldState()      const;
        virtual MamdaFieldState     getActivityTimeFieldState() const;
        virtual MamdaFieldState     getLineTimeFieldState()     const;
        virtual MamdaFieldState     getSendTimeFieldState()     const;
        virtual MamdaFieldState     getMsgQualFieldState()      const;

        /*
         * The following methods set the data for the MamdaBasicEvent.
         */
        virtual void setSymbol               (const char*          value);
        virtual void setPartId               (const char*          value);
        virtual void setEventSeqNum          (mama_seqnum_t        value);
        virtual void setEventTime            (const MamaDateTime&  value);
        virtual void setSrcTime              (const MamaDateTime&  value);
        virtual void setActivityTime         (const MamaDateTime&  value);
        virtual void setLineTime             (const MamaDateTime&  value);
        virtual void setSendTime             (const MamaDateTime&  value);
        virtual void setMsgQual              (const MamaMsgQual&   value);

    private:
        struct OrderBookConcreteSimpleDeltaImpl;
        OrderBookConcreteSimpleDeltaImpl& mImpl;

        MamdaOrderBookConcreteSimpleDelta& operator= (
            const MamdaOrderBookConcreteSimpleDelta&  copy);
        MamdaOrderBookConcreteSimpleDelta (
            const MamdaOrderBookConcreteSimpleDelta&  copy);
    };

} // namespace

#endif // MamdaOrderBookConcreteSimpleDeltaH
