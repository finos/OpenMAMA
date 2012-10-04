/* $Id:
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
 * Callback interface for MamaMsg.iterateFields().
 * An object implementing this interface is passed to the
 * <code>MamaMsg.iterateFields()</code> method. The <code>onField()</code>
 * method is invoked for each field in the MamaMsg object.
 */
public interface MamaMsgFieldIterator
{
    /**
    * Method invoked for each field in the underlying message.
    * Please note that the same single instance of the MamaMsgField is passed
    * to this method in all callbacks during iteration on a single message.
    * The state of the object is set for each invocation. This decision helps
    * to reduce unnecessary creation of many new, short lived, objects when parsing
    * messages. As such is is recommended that references to MamaMsgField
    * instances are not maintained after the completion of the
    * <code>onField</code> method.
    *
    * @param msg The MamaMsg on which <code>iterateFields()</code> was called
    * @param field The MamaMsgField object representing the underlying message field.
    * @param dict The MamaDictionary object if one was passed to <code>iterateFields()</code>
    * @param closure The closure object passed to the <code>MamaMsg.iterateFields()</code> method
    */
    void onField(MamaMsg msg, MamaMsgField field, MamaDictionary dict, Object closure);    
}
