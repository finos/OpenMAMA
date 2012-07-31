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

#include <mamda/MamdaOptionFields.h>
#include <mama/mamacpp.h>

namespace Wombat
{

    static bool initialised = false;

    const MamaFieldDescriptor*  MamdaOptionFields::CONTRACT_SYMBOL        = NULL;
    const MamaFieldDescriptor*  MamdaOptionFields::UNDERLYING_SYMBOL      = NULL;
    const MamaFieldDescriptor*  MamdaOptionFields::EXPIRATION_DATE        = NULL;
    const MamaFieldDescriptor*  MamdaOptionFields::STRIKE_PRICE           = NULL;
    const MamaFieldDescriptor*  MamdaOptionFields::PUT_CALL               = NULL;
    const MamaFieldDescriptor*  MamdaOptionFields::OPEN_INTEREST          = NULL;
    const MamaFieldDescriptor*  MamdaOptionFields::EXERCISE_STYLE         = NULL;

    void MamdaOptionFields::setDictionary (const MamaDictionary&  dictionary)
    {
        // already initialised
        if (initialised)
        {
            return;
        }

        CONTRACT_SYMBOL       = dictionary.getFieldByName ("wIssueSymbol");
        UNDERLYING_SYMBOL     = dictionary.getFieldByName ("wUnderlyingSymbol");
        EXPIRATION_DATE       = dictionary.getFieldByName ("wExpirationDate");
        STRIKE_PRICE          = dictionary.getFieldByName ("wStrikePrice");
        PUT_CALL              = dictionary.getFieldByName ("wPutCall");
        OPEN_INTEREST         = dictionary.getFieldByName ("wOpenInterest");
        EXERCISE_STYLE        = dictionary.getFieldByName ("wExerciseStyle");
        
        initialised = true;
    }

    void MamdaOptionFields::reset ()
    {
        initialised           = false;
        CONTRACT_SYMBOL       = NULL;
        UNDERLYING_SYMBOL     = NULL;
        EXPIRATION_DATE       = NULL;
        STRIKE_PRICE          = NULL;
        PUT_CALL              = NULL;
        OPEN_INTEREST         = NULL;
        EXERCISE_STYLE        = NULL;
    }

    bool MamdaOptionFields::isSet ()
    {
        return initialised;
    }

} // namespace Wombat
