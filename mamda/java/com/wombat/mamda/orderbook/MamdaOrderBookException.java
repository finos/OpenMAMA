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

/**
 * MamdaOrderBookException gets thrown when there is an inconsistency
 * in the order book.  This may be due to various reasons, including:
 * the feed sending inconsistent data, undetected missed data , 
 * the program manipulating  the order book independently of the 
 * MamdaOrderBookListener.
 */

public class MamdaOrderBookException extends RuntimeException
{
    MamdaOrderBookException (String message)
    {
        super (message);
    }
}
