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

#include <mamda/MamdaPubStatusFields.h>
#include <mama/mamacpp.h>

namespace Wombat
{

    static uint16_t theMaxFid = 0;
    static bool initialised   = false;

    const MamaFieldDescriptor*  MamdaPubStatusFields::SYMBOL                = NULL;
    const MamaFieldDescriptor*  MamdaPubStatusFields::ACTIVITY_TIME         = NULL;
    const MamaFieldDescriptor*  MamdaPubStatusFields::ACTIVITY_DATE         = NULL;
    const MamaFieldDescriptor*  MamdaPubStatusFields::PUB_ID                = NULL;
    const MamaFieldDescriptor*  MamdaPubStatusFields::PUB_CLASS             = NULL;
    const MamaFieldDescriptor*  MamdaPubStatusFields::PUB_HOST_NAME         = NULL;
    const MamaFieldDescriptor*  MamdaPubStatusFields::STATUS_MSG_TYPE       = NULL;
    const MamaFieldDescriptor*  MamdaPubStatusFields::PUB_FT_MODE           = NULL;
    const MamaFieldDescriptor*  MamdaPubStatusFields::PUB_MH_MODE           = NULL;
    const MamaFieldDescriptor*  MamdaPubStatusFields::LINE_STATUS           = NULL;
    const MamaFieldDescriptor*  MamdaPubStatusFields::STATUS_SEQ_NUM        = NULL;
    const MamaFieldDescriptor*  MamdaPubStatusFields::MSG_GAP_BEGIN         = NULL;
    const MamaFieldDescriptor*  MamdaPubStatusFields::MSG_GAP_END           = NULL;
    const MamaFieldDescriptor*  MamdaPubStatusFields::PUB_CACHE_SIZE        = NULL;
    const MamaFieldDescriptor*  MamdaPubStatusFields::FREE_TEXT             = NULL;


    void MamdaPubStatusFields::setDictionary (const MamaDictionary&  dictionary)
    {
        // already initialised
        if (initialised)
        {
            return;
        }

        SYMBOL                   = dictionary.getFieldByName ("wSymbol");
        ACTIVITY_TIME            = dictionary.getFieldByName ("wActivityTime");
        ACTIVITY_DATE            = dictionary.getFieldByName ("wActivityDate");
        PUB_ID                   = dictionary.getFieldByName ("wPubId");
        PUB_CLASS                = dictionary.getFieldByName ("wPubClass");
        PUB_HOST_NAME            = dictionary.getFieldByName ("wPubHostName");
        STATUS_MSG_TYPE          = dictionary.getFieldByName ("wStatusMsgType");
        PUB_FT_MODE              = dictionary.getFieldByName ("wPubFtMode");
        PUB_MH_MODE              = dictionary.getFieldByName ("wPubMhMode");
        LINE_STATUS              = dictionary.getFieldByName ("wLineStatus");
        STATUS_SEQ_NUM           = dictionary.getFieldByName ("wSeqNum");
        MSG_GAP_BEGIN            = dictionary.getFieldByName ("wMsgGapBegin");
        MSG_GAP_END              = dictionary.getFieldByName ("wMsgGapEnd");
        PUB_CACHE_SIZE           = dictionary.getFieldByName ("wPubCacheSize");
        FREE_TEXT                = dictionary.getFieldByName ("wText");

        theMaxFid = dictionary.getMaxFid();
        initialised = true;
    }

    void MamdaPubStatusFields::reset ()
    {
        initialised              = false;
        theMaxFid                = 0;
        SYMBOL                   = NULL;
        ACTIVITY_TIME            = NULL;
        ACTIVITY_DATE            = NULL;
        PUB_ID                   = NULL;
        PUB_CLASS                = NULL;
        PUB_HOST_NAME            = NULL;
        STATUS_MSG_TYPE          = NULL;
        PUB_FT_MODE              = NULL;
        PUB_MH_MODE              = NULL;
        LINE_STATUS              = NULL;
        STATUS_SEQ_NUM           = NULL;
        MSG_GAP_BEGIN            = NULL;
        MSG_GAP_END              = NULL;
        PUB_CACHE_SIZE           = NULL;
        FREE_TEXT                = NULL;
    }

    bool MamdaPubStatusFields::isSet ()
    {
        return initialised;
    }

    uint16_t  MamdaPubStatusFields::getMaxFid()
    {
        return theMaxFid;
    }

}
