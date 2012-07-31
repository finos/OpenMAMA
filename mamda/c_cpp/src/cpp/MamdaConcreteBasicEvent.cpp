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

#include <mamda/MamdaConcreteBasicEvent.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaFieldState.h>
#include <mama/MamaDateTime.h>
#include <mama/MamaMsgQual.h>
#include <string>

using std::string;

namespace Wombat
{

    struct MamdaConcreteBasicEvent::ConcreteBasicEventImpl
    {
        ConcreteBasicEventImpl ();
        void clear ();

        string          mSymbol;
        string          mPartId;
        mama_seqnum_t   mEventSeqNum; 
        MamaDateTime    mEventTime;
        MamaDateTime    mSrcTime;  
        MamaDateTime    mActivityTime;
        MamaDateTime    mLineTime;
        MamaDateTime    mSendTime;
        MamaMsgQual     mMsgQual; 
    };

    MamdaConcreteBasicEvent::MamdaConcreteBasicEvent ()
        : mImpl (*new ConcreteBasicEventImpl)
    {
    }

    MamdaConcreteBasicEvent::~MamdaConcreteBasicEvent ()
    {
        delete &mImpl;
    }

    const char*  MamdaConcreteBasicEvent::getSymbol () const
    {
        return mImpl.mSymbol.c_str();
    }

    const char*  MamdaConcreteBasicEvent::getPartId () const
    {
        return mImpl.mPartId.c_str();
    }

    mama_seqnum_t  MamdaConcreteBasicEvent::getEventSeqNum () const
    {
        return mImpl.mEventSeqNum;
    }

    const MamaDateTime&  MamdaConcreteBasicEvent::getEventTime () const
    {
        return mImpl.mEventTime;
    }

    const MamaDateTime&  MamdaConcreteBasicEvent::getSrcTime () const
    {
        return mImpl.mSrcTime;
    }

    const MamaDateTime&  MamdaConcreteBasicEvent::getActivityTime () const
    {
        return mImpl.mActivityTime;
    }

    const MamaDateTime&  MamdaConcreteBasicEvent::getLineTime () const
    {
        return mImpl.mLineTime;
    }

    const MamaDateTime&  MamdaConcreteBasicEvent::getSendTime () const
    {
        return mImpl.mSendTime;
    }

    const MamaMsgQual&  MamdaConcreteBasicEvent::getMsgQual () const
    {
        return mImpl.mMsgQual;
    }

    /*      FieldAccessors      */
    MamdaFieldState  MamdaConcreteBasicEvent::getSymbolFieldState () const
    {
        return MODIFIED;
    }

    MamdaFieldState  MamdaConcreteBasicEvent::getPartIdFieldState () const
    {
        return MODIFIED;
    }

    MamdaFieldState  MamdaConcreteBasicEvent::getEventSeqNumFieldState () const
    {
        return MODIFIED;
    }

    MamdaFieldState  MamdaConcreteBasicEvent::getEventTimeFieldState () const
    {
        return MODIFIED;
    }

    MamdaFieldState  MamdaConcreteBasicEvent::getSrcTimeFieldState () const
    {
        return MODIFIED;
    }

    MamdaFieldState  MamdaConcreteBasicEvent::getActivityTimeFieldState () const
    {
        return MODIFIED;
    }

    MamdaFieldState  MamdaConcreteBasicEvent::getLineTimeFieldState () const
    {
        return MODIFIED;
    }

    MamdaFieldState  MamdaConcreteBasicEvent::getSendTimeFieldState () const
    {
        return MODIFIED;
    }

    MamdaFieldState  MamdaConcreteBasicEvent::getMsgQualFieldState () const
    {
        return MODIFIED;
    }

    /*  End FieldAccessors  */


    void MamdaConcreteBasicEvent::setSymbol (const char*  value)
    {
        mImpl.mSymbol.assign (value);
    }

    void MamdaConcreteBasicEvent::setPartId (const char*  value)
    {
        mImpl.mPartId.assign (value);
    }

    void MamdaConcreteBasicEvent::setEventSeqNum (mama_seqnum_t  value)
    {
        mImpl.mEventSeqNum = value;
    }

    void MamdaConcreteBasicEvent::setEventTime (const MamaDateTime&  value)
    {
        mImpl.mEventTime = value;
    }

    void MamdaConcreteBasicEvent::setSrcTime (const MamaDateTime&  value)
    {
        mImpl.mSrcTime = value;
    }

    void MamdaConcreteBasicEvent::setActivityTime (const MamaDateTime&  value)
    {
        mImpl.mActivityTime = value;
    }

    void MamdaConcreteBasicEvent::setLineTime (const MamaDateTime&  value)
    {
        mImpl.mLineTime = value;
    }

    void MamdaConcreteBasicEvent::setSendTime (const MamaDateTime&  value)
    {
        mImpl.mSendTime = value;
    }

    void MamdaConcreteBasicEvent::setMsgQual (const MamaMsgQual&  value)
    {
        mImpl.mMsgQual = value;
    }

    MamdaConcreteBasicEvent::ConcreteBasicEventImpl::ConcreteBasicEventImpl()
    {
        clear();
    }

    void MamdaConcreteBasicEvent::ConcreteBasicEventImpl::clear ()
    {
        mSymbol.clear      ();
        mPartId.clear      ();
        mEventSeqNum  = 0;
        mEventTime.clear   ();
        mSrcTime.clear     ();
        mActivityTime.clear();
        mLineTime.clear    ();
        mSendTime.clear    ();
    }   
    
} // namespace
