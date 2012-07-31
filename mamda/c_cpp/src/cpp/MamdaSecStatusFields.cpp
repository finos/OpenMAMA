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

#include <mamda/MamdaSecStatusFields.h>
#include <mamda/MamdaCommonFields.h>
#include <mama/mamacpp.h>

#include <iostream>

namespace Wombat
{

    static uint16_t theMaxFid = 0; 
    static bool initialised   = false;

    const MamaFieldDescriptor*  MamdaSecStatusFields::PUB_CLASS             = NULL;
    const MamaFieldDescriptor*  MamdaSecStatusFields::REASON                = NULL;
    const MamaFieldDescriptor*  MamdaSecStatusFields::SECURITY_ACTION       = NULL;
    const MamaFieldDescriptor*  MamdaSecStatusFields::SECURITY_TYPE         = NULL;
    const MamaFieldDescriptor*  MamdaSecStatusFields::SECURITY_STATUS       = NULL;
    const MamaFieldDescriptor*  MamdaSecStatusFields::SECURITY_STATUS_NATIVE= NULL;
    const MamaFieldDescriptor*  MamdaSecStatusFields::SECURITY_STATUS_QUAL  = NULL;
    const MamaFieldDescriptor*  MamdaSecStatusFields::SECURITY_STATUS_TIME  = NULL;
    const MamaFieldDescriptor*  MamdaSecStatusFields::EVENT_SEQ_NUM         = NULL;
    const MamaFieldDescriptor*  MamdaSecStatusFields::FREE_TEXT             = NULL;
    const MamaFieldDescriptor*  MamdaSecStatusFields::SHORT_SALE_CIRCUIT_BREAKER        = NULL;

    void MamdaSecStatusFields::setDictionary (const MamaDictionary&  dictionary)
    {
        // Already initialised
        if (initialised)
        {
            return;
        }

        // Set the Common Fields
        if (!MamdaCommonFields::isSet())
        {
            MamdaCommonFields::setDictionary (dictionary);
        }

        PUB_CLASS              = dictionary.getFieldByName ("wPubClass");
        REASON                 = dictionary.getFieldByName ("wReason");
        SECURITY_ACTION        = dictionary.getFieldByName ("wSecurityAction");
        SECURITY_TYPE          = dictionary.getFieldByName ("wSecurityType");
        SECURITY_STATUS        = dictionary.getFieldByName ("wSecurityStatus");
        SECURITY_STATUS_QUAL   = dictionary.getFieldByName ("wSecStatusQual");
        SECURITY_STATUS_NATIVE = dictionary.getFieldByName ("wSecurityStatusOrig");
        SECURITY_STATUS_TIME   = dictionary.getFieldByName ("wSecurityStatusTime");
        EVENT_SEQ_NUM          = dictionary.getFieldByName ("wSeqNum");
        FREE_TEXT              = dictionary.getFieldByName ("wText");
        SHORT_SALE_CIRCUIT_BREAKER = dictionary.getFieldByName("wShortSaleCircuitBreaker");

        theMaxFid = dictionary.getMaxFid();
        initialised = true;
    }

    void MamdaSecStatusFields::reset ()
    {
        initialised            = false;

        //reset the Common Fields
        if (MamdaCommonFields::isSet())
        {
            MamdaCommonFields::reset ();
        }

        theMaxFid              = 0;
        PUB_CLASS              = NULL;
        REASON                 = NULL;
        SECURITY_ACTION        = NULL;
        SECURITY_TYPE          = NULL;
        SECURITY_STATUS        = NULL;
        SECURITY_STATUS_QUAL   = NULL;
        SECURITY_STATUS_NATIVE = NULL;
        SECURITY_STATUS_TIME   = NULL;
        EVENT_SEQ_NUM          = NULL;
        FREE_TEXT              = NULL;
        SHORT_SALE_CIRCUIT_BREAKER       = NULL;
    }

    bool MamdaSecStatusFields::isSet ()
    {
        return initialised;
    }

    uint16_t  MamdaSecStatusFields::getMaxFid()
    {
        return theMaxFid;
    }

}
