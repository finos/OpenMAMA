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

#include <mamda/MamdaOrderImbalanceFields.h>
#include <mamda/MamdaCommonFields.h>
#include <mama/mamacpp.h>

#include <iostream>

using namespace Wombat;

static uint16_t theMaxFid = 0; 
static bool initialised   = false;

const MamaFieldDescriptor* MamdaOrderImbalanceFields::HIGH_INDICATION_PRICE = NULL;
const MamaFieldDescriptor* MamdaOrderImbalanceFields::LOW_INDICATION_PRICE  = NULL; 
const MamaFieldDescriptor* MamdaOrderImbalanceFields::INDICATION_PRICE      = NULL; 
const MamaFieldDescriptor* MamdaOrderImbalanceFields::BUY_VOLUME            = NULL;
const MamaFieldDescriptor* MamdaOrderImbalanceFields::SELL_VOLUME           = NULL;
const MamaFieldDescriptor* MamdaOrderImbalanceFields::MATCH_VOLUME          = NULL;
const MamaFieldDescriptor* MamdaOrderImbalanceFields::SECURITY_STATUS_QUAL  = NULL;
const MamaFieldDescriptor* MamdaOrderImbalanceFields::INSIDE_MATCH_PRICE    = NULL;
const MamaFieldDescriptor* MamdaOrderImbalanceFields::FAR_CLEARING_PRICE    = NULL;
const MamaFieldDescriptor* MamdaOrderImbalanceFields::NEAR_CLEARING_PRICE   = NULL; 
const MamaFieldDescriptor* MamdaOrderImbalanceFields::NO_CLEARING_PRICE     = NULL; 
const MamaFieldDescriptor* MamdaOrderImbalanceFields::PRICE_VAR_IND         = NULL;
const MamaFieldDescriptor* MamdaOrderImbalanceFields::CROSS_TYPE            = NULL;
const MamaFieldDescriptor* MamdaOrderImbalanceFields::SRC_TIME              = NULL;
const MamaFieldDescriptor* MamdaOrderImbalanceFields::ACTIVITY_TIME         = NULL;
    
const MamaFieldDescriptor* MamdaOrderImbalanceFields::MSG_TYPE              = NULL;
const MamaFieldDescriptor* MamdaOrderImbalanceFields::ISSUE_SYMBOL          = NULL;
const MamaFieldDescriptor* MamdaOrderImbalanceFields::PART_ID               = NULL;
const MamaFieldDescriptor* MamdaOrderImbalanceFields::SEQ_NUM               = NULL;
const MamaFieldDescriptor* MamdaOrderImbalanceFields::SECURITY_STATUS_ORIG  = NULL;
const MamaFieldDescriptor* MamdaOrderImbalanceFields::SECURITY_STATUS_TIME  = NULL;
const MamaFieldDescriptor* MamdaOrderImbalanceFields::AUCTION_TIME          = NULL;
const MamaFieldDescriptor* MamdaOrderImbalanceFields::LINE_TIME             = NULL;
const MamaFieldDescriptor* MamdaOrderImbalanceFields::SEND_TIME             = NULL;

void MamdaOrderImbalanceFields::setDictionary (const MamaDictionary&  dictionary)
{
    // already initialised
    if (initialised)
    {
        return;
    }

    //set the Common Fields
    if (!MamdaCommonFields::isSet())
    {
        MamdaCommonFields::setDictionary (dictionary);
    }

    HIGH_INDICATION_PRICE = dictionary.getFieldByName ("wHighIndicationPrice");
    LOW_INDICATION_PRICE  = dictionary.getFieldByName ("wLowIndicationPrice");
    INDICATION_PRICE      = dictionary.getFieldByName ("wIndicationPrice");
    BUY_VOLUME            = dictionary.getFieldByName ("wBuyVolume");
    SELL_VOLUME           = dictionary.getFieldByName ("wSellVolume");
    MATCH_VOLUME          = dictionary.getFieldByName ("wMatchVolume");
    SECURITY_STATUS_QUAL  = dictionary.getFieldByName ("wSecStatusQual");
    INSIDE_MATCH_PRICE    = dictionary.getFieldByName ("wInsideMatchPrice");
    FAR_CLEARING_PRICE    = dictionary.getFieldByName ("wFarClearingPrice");
    NEAR_CLEARING_PRICE   = dictionary.getFieldByName ("wNearClearingPrice");
    NO_CLEARING_PRICE     = dictionary.getFieldByName ("wNoClearingPrice");
    PRICE_VAR_IND         = dictionary.getFieldByName ("wPriceVarInd");
    CROSS_TYPE            = dictionary.getFieldByName ("wCrossType");
    SRC_TIME              = dictionary.getFieldByName ("wSrcTime");
    ACTIVITY_TIME         = dictionary.getFieldByName ("wActivityTime");
    
    MSG_TYPE              = dictionary.getFieldByName ("wMsgType");
    ISSUE_SYMBOL          = dictionary.getFieldByName ("wIssueSymbol");
    PART_ID               = dictionary.getFieldByName ("wPartId");
    SEQ_NUM               = dictionary.getFieldByName ("wSeqNum");
    SECURITY_STATUS_ORIG  = dictionary.getFieldByName ("wSecurityStatusOrig");
    SECURITY_STATUS_TIME  = dictionary.getFieldByName ("wSecurityStatusTime");
    AUCTION_TIME          = dictionary.getFieldByName ("wAuctionTime");
    LINE_TIME             = dictionary.getFieldByName ("wLineTime");
    SEND_TIME             = dictionary.getFieldByName ("wSendTime");

    theMaxFid = dictionary.getMaxFid();
    initialised = true;
}

void MamdaOrderImbalanceFields::reset ()
{
    initialised           = false;

    //reset the Common Fields
    if (MamdaCommonFields::isSet())
    {
        MamdaCommonFields::reset ();
    }

    theMaxFid             = 0;
    HIGH_INDICATION_PRICE = NULL;
    LOW_INDICATION_PRICE  = NULL;
    INDICATION_PRICE      = NULL;
    BUY_VOLUME            = NULL;
    SELL_VOLUME           = NULL;
    MATCH_VOLUME          = NULL;
    SECURITY_STATUS_QUAL  = NULL;
    INSIDE_MATCH_PRICE    = NULL;
    FAR_CLEARING_PRICE    = NULL;
    NEAR_CLEARING_PRICE   = NULL;
    NO_CLEARING_PRICE     = NULL;
    PRICE_VAR_IND         = NULL;
    CROSS_TYPE            = NULL;
    SRC_TIME              = NULL;
    ACTIVITY_TIME         = NULL;

    MSG_TYPE              = NULL;
    ISSUE_SYMBOL          = NULL;
    PART_ID               = NULL;
    SEQ_NUM               = NULL;
    SECURITY_STATUS_ORIG  = NULL;
    SECURITY_STATUS_TIME  = NULL;
    AUCTION_TIME          = NULL;
    LINE_TIME             = NULL;
    SEND_TIME             = NULL;
}

bool MamdaOrderImbalanceFields::isSet ()
{
    return initialised;
}

uint16_t   MamdaOrderImbalanceFields::getMaxFid()
{
    return theMaxFid;
}
