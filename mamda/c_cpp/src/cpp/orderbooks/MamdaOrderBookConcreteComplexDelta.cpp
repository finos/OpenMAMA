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

#include <mamda/MamdaOrderBookConcreteComplexDelta.h>
#include <mamda/MamdaConcreteBasicEvent.h>
#include <mamda/MamdaFieldState.h>

namespace Wombat
{

    struct MamdaOrderBookConcreteComplexDelta::OrderBookConcreteComplexDeltaImpl
                                          : public MamdaConcreteBasicEvent
    {
    };

    MamdaOrderBookConcreteComplexDelta::MamdaOrderBookConcreteComplexDelta ()
        : mImpl (*new OrderBookConcreteComplexDeltaImpl)
    {
    }

    MamdaOrderBookConcreteComplexDelta::~MamdaOrderBookConcreteComplexDelta ()
    {
        delete &mImpl;
    }

    const char* MamdaOrderBookConcreteComplexDelta::getSymbol() const
    {
        return mImpl.getSymbol();
    }

    const char* MamdaOrderBookConcreteComplexDelta::getPartId() const
    {
        return mImpl.getPartId();
    }

    mama_seqnum_t  MamdaOrderBookConcreteComplexDelta::getEventSeqNum() const
    {
        return mImpl.getEventSeqNum();
    }

    const MamaDateTime& MamdaOrderBookConcreteComplexDelta::getEventTime() const
    {
        return mImpl.getEventTime();
    }

    const MamaDateTime& MamdaOrderBookConcreteComplexDelta::getSrcTime() const
    {
        return mImpl.getSrcTime();
    }

    const MamaDateTime& MamdaOrderBookConcreteComplexDelta::getActivityTime() const
    {
        return mImpl.getActivityTime();
    }

    const MamaDateTime& MamdaOrderBookConcreteComplexDelta::getLineTime() const
    {
        return mImpl.getLineTime();
    }

    const MamaDateTime& MamdaOrderBookConcreteComplexDelta::getSendTime() const
    {
        return mImpl.getSendTime();
    }

    const MamaMsgQual& MamdaOrderBookConcreteComplexDelta::getMsgQual() const
    {
        return mImpl.getMsgQual();
    }

    /*  FieldState Accessors        */
    MamdaFieldState MamdaOrderBookConcreteComplexDelta::getSymbolFieldState() const
    {
        return mImpl.getSymbolFieldState();
    }

    MamdaFieldState MamdaOrderBookConcreteComplexDelta::getPartIdFieldState() const
    {
        return mImpl.getPartIdFieldState();
    }

    MamdaFieldState MamdaOrderBookConcreteComplexDelta::getSrcTimeFieldState() const
    {
        return mImpl.getSrcTimeFieldState();
    }

    MamdaFieldState MamdaOrderBookConcreteComplexDelta::getActivityTimeFieldState() const
    {
        return mImpl.getActivityTimeFieldState();
    }

    MamdaFieldState MamdaOrderBookConcreteComplexDelta::getLineTimeFieldState() const
    {
        return mImpl.getLineTimeFieldState();
    }

    MamdaFieldState MamdaOrderBookConcreteComplexDelta::getSendTimeFieldState() const
    {
        return mImpl.getSendTimeFieldState();
    }

    MamdaFieldState MamdaOrderBookConcreteComplexDelta::getMsgQualFieldState() const
    {
        return mImpl.getMsgQualFieldState();
    }

    MamdaFieldState MamdaOrderBookConcreteComplexDelta::getEventSeqNumFieldState() const
    {
        return mImpl.getEventSeqNumFieldState();
    }

    MamdaFieldState MamdaOrderBookConcreteComplexDelta::getEventTimeFieldState() const
    {
        return mImpl.getEventTimeFieldState();
    }



    void MamdaOrderBookConcreteComplexDelta::setSymbol (const char* value)
    {
        mImpl.setSymbol (value);
    }

    void MamdaOrderBookConcreteComplexDelta::setPartId (const char* value)
    {
        mImpl.setPartId (value);
    }

    void MamdaOrderBookConcreteComplexDelta::setEventSeqNum (mama_seqnum_t  value)
    {
        mImpl.setEventSeqNum (value);
    }

    void MamdaOrderBookConcreteComplexDelta::setEventTime (const MamaDateTime& value)
    {
        mImpl.setEventTime (value);
    }

    void MamdaOrderBookConcreteComplexDelta::setSrcTime (const MamaDateTime& value)
    {
        mImpl.setSrcTime (value);
    }

    void MamdaOrderBookConcreteComplexDelta::setActivityTime (const MamaDateTime& value)
    {
        mImpl.setActivityTime (value);
    }

    void MamdaOrderBookConcreteComplexDelta::setLineTime (const MamaDateTime& value)
    {
        mImpl.setLineTime (value);
    }

    void MamdaOrderBookConcreteComplexDelta::setSendTime (const MamaDateTime& value)
    {
        mImpl.setSendTime (value);
    }

    void MamdaOrderBookConcreteComplexDelta::setMsgQual (const MamaMsgQual& value)
    {
        mImpl.setMsgQual (value);
    }

}
