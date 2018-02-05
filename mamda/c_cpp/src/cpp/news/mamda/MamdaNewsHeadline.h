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

#ifndef MamdaNewsHeadlineH
#define MamdaNewsHeadlineH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaNewsMetaData.h>
#include <mama/mamacpp.h>

namespace Wombat
{

    class MamdaSubscription;
    class MamdaSource;
    class MamaQueue;

    /**
     * MamdaNewsHeadline represents a news headline and includes
     * information about many types of meta-data attributes associated
     * with the headline.
     *
     * Note on story IDs: all headlines have a story ID, even though no
     * story may exist for the headline.  The story ID is generated in
     * order to be able to index together all revisions of a
     * headline/story over time.
     */

    class MAMDAOPTExpDLL MamdaNewsHeadline : public MamdaNewsMetaData
    {
    public:
        /**
         * Default constructor.
         */
        MamdaNewsHeadline ();

        /**
         * Copy constructor.
         */
        MamdaNewsHeadline (const MamdaNewsHeadline&  copy);

        /**
         * Destructor.
         */
        virtual ~MamdaNewsHeadline ();

        /**
         * Assignment operator.
         */
        virtual MamdaNewsHeadline& operator= (const MamdaNewsHeadline&  rhs);

        /**
         * @return The text of the headline for the story.
         */
        virtual const char*  getHeadlineText() const;

        /**
         * @return The headline ID for this headline.
         */
        virtual MamdaNewsHeadlineId  getHeadlineId() const;

        /**
         * @return The NYSE Technologies ID of the news story.
         */
        virtual MamdaNewsStoryId  getNewsStoryId() const;

        /**
         * @return The data source story ID for this news story.
         */
        virtual MamdaNewsStoryId  getNewsOrigStoryId() const;

        /**
         * @return Whether the headline has an associated story at this time.
         */
        virtual bool getNewsHasStory() const;

        /**
         * @return The data source of the news story.
         */
        virtual const char*  getNewsSourceId() const;

        /**
         * @return The original data source of the news story (e.g., if
         * the story was provided by a news aggregator).
         */
        virtual const char*  getNewsOrigSourceId() const;

        /**
         * @return The ANSI language ID of the news story.
         */
        virtual const char*  getLanguageId() const;

        /**
         * Get an array of native meta-data codes associated with this
         * news story.
         */
        virtual void getNativeCodes (const char**&  codes,
                                     mama_size_t&   numCodes) const;

        /**
         * Get an array of native feed symbol codes associated with this
         * news story.
         */
        virtual void getNativeRelatedSymbols (const char**&  symbols,
                                              mama_size_t&   numSymbols) const;

        /**
         * Get an array of normalized industry codes associated with this
         * news story.
         */
        virtual void getIndustries (const char**&  industries,
                                    mama_size_t&   numIndustries) const;

        /**
         * Get an array of normalized market sector codes associated with this
         * news story.
         */
        virtual void getMarketSectors (const char**&  marketSectors,
                                       mama_size_t&   numMarketSectors) const;

        /**
         * Get an array of normalized region codes associated with this
         * news story.
         */
        virtual void getRegions (const char**&  regions,
                                 mama_size_t&   numRegions) const;

        /**
         * Get an array of ISO country codes associated with this
         * news story.
         */
        virtual void getCountries (const char**&  countries,
                                   mama_size_t&   numCountries) const;

        /**
         * Get an array of normalized product codes associated with this
         * news story.
         */
        virtual void getProducts (const char**&  products,
                                  mama_size_t&   numProducts) const;

        /**
         * Get an array of normalized topic (or "subject") codes
         * associated with this news story.
         */
        virtual void getTopics (const char**&  topics,
                                mama_size_t&   numTopics) const;

        /**
         * Get an array of normalized miscellaneous codes associated with
         * this news story.  Miscellaneous codes are those not categorized
         * as industry, market sector, region, country or product codes.
         */
        virtual void getMiscCodes (const char**&  miscCodes,
                                   mama_size_t&   numMiscCodes) const;

        /**
         * Get an array of normalized symbol codes associated
         * with this news story.
         */
        virtual void getRelatedSymbols (const char**&  symbols,
                                        mama_size_t&   numSymbols) const;

        /**
         * @return Whether the feed provider has designated this story as
         * with normal priority or "hot" (important) priority.
         */
        virtual MamdaNewsPriority getNewsPriority() const;

        /**
         * @return The revision number.  Note: the value returned is zero
         * if the data source does not provide revision numbers.
         */
        virtual mama_u16_t  getNewsStoryRevNumber() const;

        /**
         * @return The original publish time of the news story.
         */
        virtual const MamaDateTime&  getNewsOrigStoryTime() const;

        // Inherited from MamdaBasicEvent:
        virtual const char*          getSymbol       () const;
        virtual const char*          getPartId       () const;
        virtual const MamaDateTime&  getSrcTime      () const;
        virtual const MamaDateTime&  getActivityTime () const;
        virtual const MamaDateTime&  getLineTime     () const;
        virtual const MamaDateTime&  getSendTime     () const;
        virtual const MamaMsgQual&   getMsgQual      () const;
        virtual const MamaDateTime&  getEventTime    () const;
        virtual mama_seqnum_t        getEventSeqNum  () const;
        
        MamdaFieldState     getSymbolFieldState               () const;
        MamdaFieldState     getPartIdFieldState               () const;
        MamdaFieldState     getSrcTimeFieldState              () const;
        MamdaFieldState     getActivityTimeFieldState         () const;
        MamdaFieldState     getLineTimeFieldState             () const;
        MamdaFieldState     getSendTimeFieldState             () const;
        MamdaFieldState     getMsgQualFieldState              () const;
        MamdaFieldState     getEventTimeFieldState            () const;
        MamdaFieldState     getEventSeqNumFieldState          () const;


        void setHeadlineText         (const char*          headlineText);
        void setHeadlineId           (const char*          headlineId);
        void setStoryId              (const char*          storyId);
        void setHasStory             (bool                 hasStory);
        void setSourceId             (const char*          source);
        void setOrigStoryId          (const char*          storyId);
        void setOrigSourceId         (const char*          origSourceId);
        void setLanguageId           (const char*          langId);
        void setNativeCodes          (const char**         codes,
                                      mama_size_t          numCodes);
        void setNativeRelatedSymbols (const char**         symbols,
                                      mama_size_t          numSymbols);
        void setIndustries           (const char**         industries,
                                      mama_size_t          numIndustries);
        void setMarketSectors        (const char**         marketSectors,
                                      mama_size_t          numMarketSectors);
        void setRegions              (const char**         regions,
                                      mama_size_t          numRegions);
        void setCountries            (const char**         countries,
                                      mama_size_t          numCountries);
        void setProducts             (const char**         products,
                                      mama_size_t          numProducts);
        void setTopics               (const char**         topics,
                                      mama_size_t          numTopics);
        void setMiscCodes            (const char**         codes,
                                      mama_size_t          numCodes);
        void setRelatedSymbols       (const char**         symbols,
                                      mama_size_t          numSymbols);
        void setPriority             (MamdaNewsPriority    priority);
        void setStoryRevNumber       (mama_u16_t           storyRevNum);
        void setEventSeqNum          (mama_seqnum_t        seqNum);
        void setEventTime            (const MamaDateTime&  eventTime);
        void setOrigStoryTime        (const MamaDateTime&  storyTime);
        void setSrcTime              (const MamaDateTime&  srcTime);
        void setActivityTime         (const MamaDateTime&  actTime);
        void setLineTime             (const MamaDateTime&  lineTime);
        void setSendTime             (const MamaDateTime&  sendTime);
        void setMsgQual              (const MamaMsgQual&   msgQualifier);
        void clear                   ();
        
    public:

        /**
         * Implementation: save the subscription info for the
         * headline (so we know where to request stories from).
         */
        void setSubscriptionInfo (MamdaSubscription*  subsc);

        void setSubscInfo (MamaQueue*   queue,
                           MamaSource*  source);
        
        /**
         * Implementation: get the subscription info for the
         * headline.
         */
        MamaQueue*      getQueue     () const;
        MamaSource*     getSource    () const;

    private:
        struct MamdaNewsHeadlineImpl;
        MamdaNewsHeadlineImpl& mImpl;
    };

} // namespace

#endif // MamdaNewsHeadlineH
