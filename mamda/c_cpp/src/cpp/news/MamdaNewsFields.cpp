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

#include <mamda/MamdaNewsFields.h>
#include <mamda/MamdaCommonFields.h>
#include <mama/mamacpp.h>

namespace Wombat
{

    static uint16_t  theMaxFid   = 0;
    static bool      initialised = false;

    const MamaFieldDescriptor*  MamdaNewsFields::HEADLINE_TEXT   = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::HEADLINE_ID     = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::STORY_TEXT      = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::STORY_ID        = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::ORIG_STORY_ID   = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::SOURCE_ID       = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::ORIG_SOURCE_ID  = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::LANGUAGE_ID     = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::NATIVE_CODES    = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::NATIVE_SYMBOLS  = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::INDUSTRIES      = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::MARKET_SECTORS  = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::REGIONS         = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::COUNTRIES       = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::PRODUCTS        = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::TOPICS          = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::MISC_CODES      = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::SYMBOLS         = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::PRIORITY        = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::TIME            = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::REVISION_NUM    = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::STORY_STATUS    = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::STORY_TIME      = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::ORIG_STORY_TIME = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::STORY_HEADLINES = NULL;
    const MamaFieldDescriptor*  MamdaNewsFields::HAS_STORY       = NULL;


    void MamdaNewsFields::setDictionary (const MamaDictionary&  dictionary)
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

        HEADLINE_TEXT       = dictionary.getFieldByName ("wNewsHeadline");
        HEADLINE_ID         = dictionary.getFieldByName ("wNewsHeadlineId");
        STORY_TEXT          = dictionary.getFieldByName ("wNewsStory");
        STORY_ID            = dictionary.getFieldByName ("wNewsStoryId");
        ORIG_STORY_ID       = dictionary.getFieldByName ("wNewsOrigStoryId");
        SOURCE_ID           = dictionary.getFieldByName ("wNewsSourceId");
        ORIG_SOURCE_ID      = dictionary.getFieldByName ("wNewsOrigSourceId");
        LANGUAGE_ID         = dictionary.getFieldByName ("wNewsLangId");
        NATIVE_CODES        = dictionary.getFieldByName ("wNewsNativeCodes");
        NATIVE_SYMBOLS      = dictionary.getFieldByName ("wNewsNativeSymbols");
        INDUSTRIES          = dictionary.getFieldByName ("wNewsIndustries");
        MARKET_SECTORS      = dictionary.getFieldByName ("wNewsMarketSectors");
        REGIONS             = dictionary.getFieldByName ("wNewsRegions");
        COUNTRIES           = dictionary.getFieldByName ("wNewsCountries");
        PRODUCTS            = dictionary.getFieldByName ("wNewsProducts");
        TOPICS              = dictionary.getFieldByName ("wNewsTopics");
        MISC_CODES          = dictionary.getFieldByName ("wNewsMiscCodes");
        SYMBOLS             = dictionary.getFieldByName ("wNewsSymbols");
        PRIORITY            = dictionary.getFieldByName ("wNewsPriority");
        TIME                = dictionary.getFieldByName ("wNewsTime");
        REVISION_NUM        = dictionary.getFieldByName ("wNewsRevNum");
        STORY_STATUS        = dictionary.getFieldByName ("wNewsStoryStatus");
        STORY_TIME          = dictionary.getFieldByName ("wNewsStoryTime");
        ORIG_STORY_TIME     = dictionary.getFieldByName ("wNewsOrigStoryTime");
        STORY_HEADLINES     = dictionary.getFieldByName ("wNewsStoryHeadlineIds");
        HAS_STORY           = dictionary.getFieldByName ("wNewsHasStory");

        theMaxFid = dictionary.getMaxFid();
        initialised = true;
    }

    void MamdaNewsFields::reset ()
    {
        initialised         = false;

        // Set the Common Fields
        if (MamdaCommonFields::isSet())
        {
            MamdaCommonFields::reset ();
        }

        theMaxFid           = 0;
        HEADLINE_TEXT       = NULL;
        HEADLINE_ID         = NULL;
        STORY_TEXT          = NULL;
        STORY_ID            = NULL;
        ORIG_STORY_ID       = NULL;
        SOURCE_ID           = NULL;
        ORIG_SOURCE_ID      = NULL;
        LANGUAGE_ID         = NULL;
        NATIVE_CODES        = NULL;
        NATIVE_SYMBOLS      = NULL;
        INDUSTRIES          = NULL;
        MARKET_SECTORS      = NULL;
        REGIONS             = NULL;
        COUNTRIES           = NULL;
        PRODUCTS            = NULL;
        TOPICS              = NULL;
        MISC_CODES          = NULL;
        SYMBOLS             = NULL;
        PRIORITY            = NULL;
        TIME                = NULL;
        REVISION_NUM        = NULL;
        STORY_STATUS        = NULL;
        STORY_TIME          = NULL;
        ORIG_STORY_TIME     = NULL;
        STORY_HEADLINES     = NULL;
        HAS_STORY           = NULL;
    }

    bool MamdaNewsFields::isSet ()
    {
        return initialised;
    }

    uint16_t  MamdaNewsFields::getMaxFid()
    {
        return theMaxFid;
    }

}
