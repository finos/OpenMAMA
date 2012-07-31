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

#include <mamda/MamdaOrderBookConcreteSimpleDelta.h>
#include <mamda/MamdaConcreteBasicEvent.h>
#include <mamda/MamdaFieldState.h>


namespace Wombat
{

    struct MamdaOrderBookConcreteSimpleDelta::OrderBookConcreteSimpleDeltaImpl
                                         : public MamdaConcreteBasicEvent
    {
    };

    MamdaOrderBookConcreteSimpleDelta::MamdaOrderBookConcreteSimpleDelta ()
        : mImpl (*new OrderBookConcreteSimpleDeltaImpl)
    {
    }

    MamdaOrderBookConcreteSimpleDelta::~MamdaOrderBookConcreteSimpleDelta ()
    {
        delete &mImpl;
    }

    const char* MamdaOrderBookConcreteSimpleDelta::getSymbol() const
    {
        return mImpl.getSymbol();
    }

    const char* MamdaOrderBookConcreteSimpleDelta::getPartId() const
    {
        return mImpl.getPartId();
    }

    mama_seqnum_t  MamdaOrderBookConcreteSimpleDelta::getEventSeqNum() const
    {
        return mImpl.getEventSeqNum();
    }

    const MamaDateTime& MamdaOrderBookConcreteSimpleDelta::getEventTime() const
    {
        return mImpl.getEventTime();
    }

    const MamaDateTime& MamdaOrderBookConcreteSimpleDelta::getSrcTime() const
    {
        return mImpl.getSrcTime();
    }

    const MamaDateTime& MamdaOrderBookConcreteSimpleDelta::getActivityTime() const
    {
        return mImpl.getActivityTime();
    }

    const MamaDateTime& MamdaOrderBookConcreteSimpleDelta::getLineTime() const
    {
        return mImpl.getLineTime();
    }

    const MamaDateTime& MamdaOrderBookConcreteSimpleDelta::getSendTime() const
    {
        return mImpl.getSendTime();
    }

    const MamaMsgQual& MamdaOrderBookConcreteSimpleDelta::getMsgQual() const
    {
        return mImpl.getMsgQual();
    }

    /*      FieldAccessors      */
    MamdaFieldState MamdaOrderBookConcreteSimpleDelta::getSymbolFieldState() const
    {
        return mImpl.getSymbolFieldState ();
    }

    MamdaFieldState MamdaOrderBookConcreteSimpleDelta::getPartIdFieldState() const
    {
        return mImpl.getPartIdFieldState ();
    }

    MamdaFieldState  MamdaOrderBookConcreteSimpleDelta::getEventSeqNumFieldState() const
    {
        return mImpl.getEventSeqNumFieldState ();
    }

    MamdaFieldState MamdaOrderBookConcreteSimpleDelta::getEventTimeFieldState() const
    {
        return mImpl.getEventTimeFieldState ();
    }

    MamdaFieldState MamdaOrderBookConcreteSimpleDelta::getSrcTimeFieldState() const
    {
        return mImpl.getSrcTimeFieldState ();
    }

    MamdaFieldState MamdaOrderBookConcreteSimpleDelta::getActivityTimeFieldState() const
    {
        return mImpl.getActivityTimeFieldState ();
    }

    MamdaFieldState MamdaOrderBookConcreteSimpleDelta::getLineTimeFieldState() const
    {
        return mImpl.getLineTimeFieldState ();
    }

    MamdaFieldState MamdaOrderBookConcreteSimpleDelta::getSendTimeFieldState() const
    {
        return mImpl.getSendTimeFieldState ();
    }

    MamdaFieldState MamdaOrderBookConcreteSimpleDelta::getMsgQualFieldState() const
    {
        return mImpl.getMsgQualFieldState ();
    }

    /*  End FieldAccessors  */

    void MamdaOrderBookConcreteSimpleDelta::setSymbol (const char* value)
    {
        mImpl.setSymbol(value);
    }

    void MamdaOrderBookConcreteSimpleDelta::setPartId (const char* value)
    {
        mImpl.setPartId(value);
    }

    void MamdaOrderBookConcreteSimpleDelta::setEventSeqNum (mama_seqnum_t  value)
    {
        mImpl.setEventSeqNum(value);
    }

    void MamdaOrderBookConcreteSimpleDelta::setEventTime (const MamaDateTime& value)
    {
        mImpl.setEventTime(value);
    }

    void MamdaOrderBookConcreteSimpleDelta::setSrcTime (const MamaDateTime& value)
    {
        mImpl.setSrcTime(value);
    }

    void MamdaOrderBookConcreteSimpleDelta::setActivityTime (const MamaDateTime& value)
    {
        mImpl.setActivityTime(value);
    }

    void MamdaOrderBookConcreteSimpleDelta::setLineTime (const MamaDateTime& value)
    {
        mImpl.setLineTime(value);
    }

    void MamdaOrderBookConcreteSimpleDelta::setSendTime (const MamaDateTime& value)
    {
        mImpl.setSendTime(value);
    }

    void MamdaOrderBookConcreteSimpleDelta::setMsgQual (const MamaMsgQual& value)
    {
        mImpl.setMsgQual(value);
    }

}
