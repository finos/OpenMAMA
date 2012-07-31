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

#ifndef MamdaNewsMetaDataH
#define MamdaNewsMetaDataH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaBasicEvent.h>
#include <mamda/MamdaNewsTypes.h>
#include <mama/types.h>


namespace Wombat
{

    /**
     * MamdaNewsMetaData represents information about many types of
     * attributes associated with the news headline.
     *
     * Note that some information is inherited from the MamdaBasicEvent,
     * including the source time (the time the data source sent the
     * headline) and the event time (the time the article was actually
     * published).  Depending upon the richness of information provided by
     * a particular data source, the source time and event time may be the
     * same.
     */

    class MAMDAOPTExpDLL MamdaNewsMetaData : public MamdaBasicEvent
    {
    public:

        /**
         * @return The text of the headline for the story.
         */
        virtual const char*  getHeadlineText() const = 0;

        /**
         * @return The data source ID of the news story.
         */
        virtual const char*  getNewsSourceId() const = 0;

        /**
         * @return The original data source ID of the news story (e.g., if
         * the story was provided by a news aggregator).
         */
        virtual const char*  getNewsOrigSourceId() const = 0;

        /**
         * @return The ANSI language ID of the news story.
         */
        virtual const char*  getLanguageId() const = 0;

        /**
         * Get an array of native meta-data codes associated with this
         * news story.
         */
        virtual void getNativeCodes (const char**&  codes,
                                     mama_size_t&   numCodes) const = 0;

        /**
         * Get an array of native feed symbol codes associated with this
         * news story.
         */
        virtual void getNativeRelatedSymbols (const char**&  symbols,
                                              mama_size_t&   numSymbols) const = 0;

        /**
         * Get an array of normalized industry codes associated with this
         * news story.
         */
        virtual void getIndustries (const char**&  industries,
                                    mama_size_t&   numIndustries) const = 0;

        /**
         * Get an array of normalized market sector codes associated with this
         * news story.
         */
        virtual void getMarketSectors (const char**&  marketSectors,
                                       mama_size_t&   numMarketSectors) const = 0;

        /**
         * Get an array of normalized region codes associated with this
         * news story.
         */
        virtual void getRegions (const char**&  regions,
                                 mama_size_t&   numRegions) const = 0;

        /**
         * Get an array of ISO country codes associated with this
         * news story.
         */
        virtual void getCountries (const char**&  countries,
                                   mama_size_t&   numCountries) const = 0;

        /**
         * Get an array of normalized topic (or "subject") codes
         * associated with this news story.
         */
        virtual void getTopics (const char**&  topics,
                                mama_size_t&   numTopics) const = 0;

        /**
         * Get an array of normalized product codes associated with this
         * news story.
         */
        virtual void getProducts (const char**&  products,
                                  mama_size_t&   numProducts) const = 0;

        /**
         * Get an array of normalized miscellaneous codes associated with
         * this news story.  Miscellaneous codes are those not categorized
         * as industry, market sector, region, country or product codes.
         */
        virtual void getMiscCodes (const char**&  miscCodes,
                                   mama_size_t&   numMiscCodes) const = 0;

        /**
         * Get an array of normalized symbol codes associated
         * with this news story.
         */
        virtual void getRelatedSymbols (const char**&  symbols,
                                        mama_size_t&   numSymbols) const = 0;

        /**
         * @return Whether the feed provider has designated this story as
         * with normal priority or "hot" (important) priority.
         */
        virtual MamdaNewsPriority getNewsPriority() const = 0;

        /**
         * @return The revision number.  Note: the value returned is zero
         * if the data source does not provide revision numbers.
         */
        virtual mama_u16_t  getNewsStoryRevNumber() const = 0;

        /**
         * @return The original publish time of the news story.
         */
        virtual const MamaDateTime&  getNewsOrigStoryTime() const = 0;
        
        virtual ~MamdaNewsMetaData() {};
    };

} // namespace

#endif // MamdaNewsMetaDataH
