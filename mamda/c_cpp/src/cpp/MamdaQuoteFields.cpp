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

#include <mamda/MamdaQuoteFields.h>
#include <mamda/MamdaCommonFields.h>
#include <mama/mamacpp.h>

namespace Wombat
{

    static uint16_t theMaxFid = 0;
    static bool initialised   = false;

    const MamaFieldDescriptor*  MamdaQuoteFields::BID_PRICE            = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::BID_SIZE             = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::BID_DEPTH            = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::BID_PART_ID          = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::BID_CLOSE_PRICE      = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::BID_CLOSE_DATE       = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::BID_PREV_CLOSE_PRICE = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::BID_PREV_CLOSE_DATE  = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::BID_HIGH             = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::BID_LOW              = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::ASK_PRICE            = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::ASK_SIZE             = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::ASK_DEPTH            = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::ASK_PART_ID          = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::ASK_CLOSE_PRICE      = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::ASK_CLOSE_DATE       = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::ASK_PREV_CLOSE_PRICE = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::ASK_PREV_CLOSE_DATE  = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::ASK_HIGH             = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::ASK_LOW              = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::QUOTE_SEQ_NUM        = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::QUOTE_TIME           = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::QUOTE_DATE           = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::QUOTE_QUAL           = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::QUOTE_QUAL_NATIVE    = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::QUOTE_COUNT          = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::MID_PRICE            = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::SHORT_SALE_BID_TICK  = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::BID_TICK             = NULL;

    const MamaFieldDescriptor*  MamdaQuoteFields::ASK_TIME             = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::BID_TIME             = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::ASK_INDICATOR        = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::BID_INDICATOR        = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::ASK_UPDATE_COUNT     = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::BID_UPDATE_COUNT     = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::ASK_YIELD            = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::BID_YIELD            = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::BID_SIZES_LIST       = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::ASK_SIZES_LIST       = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::BOOK_CONTRIBUTORS    = NULL;
    const MamaFieldDescriptor*  MamdaQuoteFields::SHORT_SALE_CIRCUIT_BREAKER = NULL;


    void MamdaQuoteFields::setDictionary (const MamaDictionary&  dictionary)
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

        BID_PRICE            = dictionary.getFieldByName ("wBidPrice");
        BID_SIZE             = dictionary.getFieldByName ("wBidSize");
        BID_DEPTH            = dictionary.getFieldByName ("wBidDepth");
        BID_PART_ID          = dictionary.getFieldByName ("wBidPartId");
        BID_CLOSE_PRICE      = dictionary.getFieldByName ("wBidClose");
        BID_CLOSE_DATE       = dictionary.getFieldByName ("wBidCloseDate");
        BID_PREV_CLOSE_PRICE = dictionary.getFieldByName ("wPrevBidClose");
        BID_PREV_CLOSE_DATE  = dictionary.getFieldByName ("wPrevBidCloseDate");
        BID_HIGH             = dictionary.getFieldByName ("wBidHigh");
        BID_LOW              = dictionary.getFieldByName ("wBidLow");
        ASK_PRICE            = dictionary.getFieldByName ("wAskPrice");
        ASK_SIZE             = dictionary.getFieldByName ("wAskSize");
        ASK_DEPTH            = dictionary.getFieldByName ("wAskDepth");
        ASK_PART_ID          = dictionary.getFieldByName ("wAskPartId");
        ASK_CLOSE_PRICE      = dictionary.getFieldByName ("wAskClose");
        ASK_CLOSE_DATE       = dictionary.getFieldByName ("wAskCloseDate");
        ASK_PREV_CLOSE_PRICE = dictionary.getFieldByName ("wPrevAskClose");
        ASK_PREV_CLOSE_DATE  = dictionary.getFieldByName ("wPrevAskCloseDate");
        ASK_HIGH             = dictionary.getFieldByName ("wAskHigh");
        ASK_LOW              = dictionary.getFieldByName ("wAskLow");
        QUOTE_SEQ_NUM        = dictionary.getFieldByName ("wQuoteSeqNum");
        QUOTE_TIME           = dictionary.getFieldByName ("wQuoteTime");
        QUOTE_DATE           = dictionary.getFieldByName ("wQuoteDate");
        QUOTE_QUAL           = dictionary.getFieldByName ("wQuoteQualifier");
        QUOTE_QUAL_NATIVE    = dictionary.getFieldByName ("wCondition");
        QUOTE_COUNT          = dictionary.getFieldByName ("wQuoteCount");
        MID_PRICE            = dictionary.getFieldByName ("wMidPrice");
        SHORT_SALE_BID_TICK  = dictionary.getFieldByName ("wShortSaleBidTick");
        BID_TICK             = dictionary.getFieldByName ("wBidTick");
        
        ASK_TIME             = dictionary.getFieldByName ("wAskTime");
        BID_TIME             = dictionary.getFieldByName ("wBidTime");
        ASK_INDICATOR        = dictionary.getFieldByName ("wAskIndicator");
        BID_INDICATOR        = dictionary.getFieldByName ("wBidIndicator");
        ASK_UPDATE_COUNT     = dictionary.getFieldByName ("wAskUpdateCount");
        BID_UPDATE_COUNT     = dictionary.getFieldByName ("wBidUpdateCount");
        ASK_YIELD            = dictionary.getFieldByName ("wAskYield");
        BID_YIELD            = dictionary.getFieldByName ("wBidYield");
        BID_SIZES_LIST       = dictionary.getFieldByName ("wBidSizesList");
        ASK_SIZES_LIST       = dictionary.getFieldByName ("wAskSizesList");
        BOOK_CONTRIBUTORS    = dictionary.getFieldByName ("wBookContributors");
        SHORT_SALE_CIRCUIT_BREAKER = dictionary.getFieldByName("wShortSaleCircuitBreaker");

        theMaxFid = dictionary.getMaxFid();
        initialised = true;
    }

    void MamdaQuoteFields::reset ()
    {
        initialised          = false;

        //set the Common Fields
        if (MamdaCommonFields::isSet())
        {
            MamdaCommonFields::reset ();
        }

        theMaxFid            = 0;
        BID_PRICE            = NULL;
        BID_SIZE             = NULL;
        BID_DEPTH            = NULL;
        BID_PART_ID          = NULL;
        BID_CLOSE_PRICE      = NULL;
        BID_CLOSE_DATE       = NULL;
        BID_PREV_CLOSE_PRICE = NULL;
        BID_PREV_CLOSE_DATE  = NULL;
        BID_HIGH             = NULL;
        BID_LOW              = NULL;
        ASK_PRICE            = NULL;
        ASK_SIZE             = NULL;
        ASK_DEPTH            = NULL;
        ASK_PART_ID          = NULL;
        ASK_CLOSE_PRICE      = NULL;
        ASK_CLOSE_DATE       = NULL;
        ASK_PREV_CLOSE_PRICE = NULL;
        ASK_PREV_CLOSE_DATE  = NULL;
        ASK_HIGH             = NULL;
        ASK_LOW              = NULL;
        QUOTE_SEQ_NUM        = NULL;
        QUOTE_TIME           = NULL;
        QUOTE_DATE           = NULL;
        QUOTE_QUAL           = NULL;
        QUOTE_QUAL_NATIVE    = NULL;
        QUOTE_COUNT          = NULL;
        MID_PRICE            = NULL;
        SHORT_SALE_BID_TICK  = NULL;
        BID_TICK             = NULL;

        ASK_TIME             = NULL;
        BID_TIME             = NULL;
        ASK_INDICATOR        = NULL;
        BID_INDICATOR        = NULL;
        ASK_UPDATE_COUNT     = NULL;
        BID_UPDATE_COUNT     = NULL;
        ASK_YIELD            = NULL;
        BID_YIELD            = NULL;
        BID_SIZES_LIST       = NULL;
        ASK_SIZES_LIST       = NULL;
        BOOK_CONTRIBUTORS    = NULL;
        SHORT_SALE_CIRCUIT_BREAKER = NULL;
    }

    bool MamdaQuoteFields::isSet ()
    {
        return initialised;
    }

    uint16_t  MamdaQuoteFields::getMaxFid()
    {
        return theMaxFid;
    }

}
