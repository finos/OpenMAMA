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

package com.wombat.mama;

/**
 * Subscription request type of a MAMA subscription.  Some types of
 * subscriptions require special handling inside MAMA or by the
 * publisher.
 */

public final class MamaSubscriptionType
{
    private String myTypeName;
    private short  myTypeValue;

    private MamaSubscriptionType (short value, String name)
    {
        myTypeName  = name;
        myTypeValue = value;
    }

    public short getValue ()
    {
        return myTypeValue;
    }

    public String toString ()
    {
        return myTypeName;
    }
    
    public static final  MamaSubscriptionType NORMAL =
    new MamaSubscriptionType ((short)0, "NORMAL");
    public static final MamaSubscriptionType GROUP = 
    new MamaSubscriptionType ((short)1,"GROUP");
    public static final MamaSubscriptionType BOOK = 
    new MamaSubscriptionType ((short)2,"BOOK");
    public static final MamaSubscriptionType BASIC = 
    new MamaSubscriptionType ((short)3,"BASIC");
    public static final MamaSubscriptionType DICTIONARY =
    new MamaSubscriptionType ((short)4, "DICTIONARY");
    
    public static final MamaSubscriptionType SYMBOL_LIST = 
    new MamaSubscriptionType ((short)5,"SYMBOL_LIST");
    public static final MamaSubscriptionType SYMBOL_LIST_NORMAL = 
    new MamaSubscriptionType ((short)6,"NORMAL_SYMBOL_LIST");
    public static final MamaSubscriptionType SYMBOL_LIST_GROUP =
    new MamaSubscriptionType ((short)7,"GROUP_SYMBOL_LIST");
    public static final MamaSubscriptionType SYMBOL_LIST_BOOK =
    new MamaSubscriptionType ((short)8,"BOOK_SYMBOL_LIST");
}
