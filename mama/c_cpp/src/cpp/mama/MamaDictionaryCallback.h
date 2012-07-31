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

#ifndef MAMA_DICTIONARY_CALLBACK_CPP_H__
#define MAMA_DICTIONARY_CALLBACK_CPP_H__

#include "mama/mamacpp.h"

namespace Wombat 
{
/**
 * The <code>WombatDictionaryCallback</code> receives notification
 * regarding the population of the data dictionary. Clients implement
 * the interface and pass it to
 * <code>Mama.createDictionarySubscription</code>
 *
 * @see MamaDictionary
 * @see Mama#createDictionarySubscription
 *
 */
class MAMACPPExpDLL MamaDictionaryCallback
{
public:
    virtual ~MamaDictionaryCallback (void) {}

    /**
     * Called when a timeout occurs.
     */
    virtual void onTimeout (void) {}

    /**
     * Invoked when an error occurs.
     * @param message The error message.
     */
    virtual void onError (const char* message) {}

    /**
     * Invoked when dictionary creation is complete.
     */
    virtual void onComplete (void) {}
};

} // namespace Wombat
#endif // MAMA_DICTIONARY_CALLBACK_CPP_H__
