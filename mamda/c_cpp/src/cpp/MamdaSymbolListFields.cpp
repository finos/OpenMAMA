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

#include <mamda/MamdaSymbolListFields.h>
#include <mamda/MamdaCommonFields.h>
#include <mama/mamacpp.h>


namespace Wombat
{

    static uint16_t  theMaxFid     = 0;
    static bool      initialised   = false;

    const MamaFieldDescriptor*  MamdaSymbolListFields::SRC_TIME           = NULL;
    const MamaFieldDescriptor*  MamdaSymbolListFields::ACTIVITY_TIME      = NULL;
    const MamaFieldDescriptor*  MamdaSymbolListFields::PUB_ID             = NULL;
    const MamaFieldDescriptor*  MamdaSymbolListFields::EVENT_SEQ_NUM      = NULL;
    const MamaFieldDescriptor*  MamdaSymbolListFields::EVENT_TIME         = NULL;
    const MamaFieldDescriptor*  MamdaSymbolListFields::SYMBOL_LIST        = NULL;


    void MamdaSymbolListFields::setDictionary (const MamaDictionary&  dictionary)
    {
        // Already initialised
        if (initialised)
        {
            return;
        }

        // Set the Common Fields
        if(!MamdaCommonFields::isSet())
        {
            MamdaCommonFields::setDictionary (dictionary);
        }

        SRC_TIME             = dictionary.getFieldByName ("wSrcTime");
        ACTIVITY_TIME        = dictionary.getFieldByName ("wActivityTime");
        PUB_ID               = dictionary.getFieldByName ("wPubId");
        EVENT_SEQ_NUM        = dictionary.getFieldByName ("wSeqNum");
        EVENT_TIME           = dictionary.getFieldByName ("wEventTime");
        SYMBOL_LIST          = dictionary.getFieldByName ("MdSymbolList");

        theMaxFid   = dictionary.getMaxFid();
        initialised = true;
    }

    void MamdaSymbolListFields::reset ()
    {
        initialised          = false;

        // Set the Common Fields
        if (MamdaCommonFields::isSet())
        {
            MamdaCommonFields::reset ();
        }

        theMaxFid            = 0;
        SRC_TIME             = NULL;
        ACTIVITY_TIME        = NULL;
        PUB_ID               = NULL;
        EVENT_SEQ_NUM        = NULL;
        EVENT_TIME           = NULL;
        SYMBOL_LIST          = NULL;
    }

    bool MamdaSymbolListFields::isSet ()
    {
        return initialised;
    }

    uint16_t  MamdaSymbolListFields::getMaxFid()
    {
        return theMaxFid;
    }

}
