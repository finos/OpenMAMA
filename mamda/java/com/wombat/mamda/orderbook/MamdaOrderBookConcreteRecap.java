/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2012 NYSE Technologies, Inc.
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

package com.wombat.mamda.orderbook;

import com.wombat.mamda.MamdaConcreteBasicEvent;

import com.wombat.mama.*;
import com.wombat.mamda.*;

/**
 * MamdaOrderBookConcreteRecap is a class that saves information
 * about a full order book image.
 */

public class MamdaOrderBookConcreteRecap
    extends MamdaConcreteBasicEvent
    implements MamdaOrderBookRecap
{
    private MamdaOrderBook    mBook;
    private MamaDateTime      mSendTime;
    private MamaDateTime      mLineTime;
    private String            mPartId;
    private String            mSymbol;

    private MamdaFieldState   mSendTimeFieldState = new MamdaFieldState();
    private MamdaFieldState   mLineTimeFieldState = new MamdaFieldState();
    private MamdaFieldState   mPartIdFieldState   = new MamdaFieldState();
    private MamdaFieldState   mSymbolFieldState   = new MamdaFieldState();
    
    public MamdaOrderBook getOrderBook ()
    {
        return mBook;
    }

    public void setBook (MamdaOrderBook book)
    {
        mBook = book;
    }

    public MamaDateTime getSendTime ()
    {
        return mSendTime;
    }

    public short getSendTimeFieldState ()
    {
        return mSendTimeFieldState.getState();
    }

    public void setSendTime (MamaDateTime sendTime)
    {
        mSendTime = sendTime;
    }

    public MamaDateTime getLineTime ()
    {
        return mLineTime;
    }

    public short getLineTimeFieldState ()
    {
        return mLineTimeFieldState.getState();
    }

    public void setLineTime (MamaDateTime lineTime)
    {
        mLineTime = lineTime;
    }

    public String getPartId ()
    {
        return mPartId;
    }

    public short getPartIdFieldState ()
    {
        return mPartIdFieldState.getState();
    }

    public void setPartId (String partId)
    {
        mPartId = partId;
    }

    public String getSymbol ()
    {
        return mSymbol;
    }

    public short getSymbolFieldState ()
    {
        return mSymbolFieldState.getState();
    }

    public void setSymbol (String symbol)
    {
        mSymbol = symbol;
    }
}
