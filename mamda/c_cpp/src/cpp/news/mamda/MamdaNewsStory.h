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

#ifndef MamdaNewsStoryH
#define MamdaNewsStoryH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaNewsMetaData.h>
#include <mama/mamacpp.h>

namespace Wombat
{

    /**
     * MamdaNewsStory represents a complete text of a news story.
     */

    class MAMDAOPTExpDLL MamdaNewsStory
    {
    public:
        /**
         * The Status enumeration provides information about the status of
         * the news story and is provided in responses to news story
         * requests.
         *
         * NO_STORY indicates that there is currently no story for the
         * headline.  This may occur for feeds that provide "alert"
         * headlines, either as the only headline or as a precursor to a
         * full story.
         *
         * FULL_STORY indicates that the complete story text is being
         * provided in the current callback.
         *
         * FETCHING_STORY indicates that the story is currently being
         * fetched by the publisher.  This is a temporary status.  An
         * additional callback will automatically be invoked when the full
         * story is available.
         *
         * DELAYED_STORY indicates that the story is not currently
         * available but is expected at some time in the future.  No
         * additional callback will be automatically invoked.
         *
         * NOT_FOUND indicates that the publisher does not currently have
         * a story for this headline and cannot determine whether a story
         * will arrive for the headline.  No additional callback will be
         * automatically invoked.
         *
         * UNKNOWN indicates an unknown condition (should not happen).
         */
        enum Status
        {
            NO_STORY       = 0,
            FULL_STORY     = 1,
            FETCHING_STORY = 2,
            DELAYED_STORY  = 3,
            NOT_FOUND      = 4,
            DELETED        = 5,
            EXPIRED        = 6,
            UNKNOWN        = 99
        };

        /**
         * Default constructor.
         */
        MamdaNewsStory ();

        /**
         * Copy constructor.
         */
        MamdaNewsStory (const MamdaNewsStory&  copy);

        /**
         * Destructor.
         */
        virtual ~MamdaNewsStory ();

        /**
         * Assignment operator.
         */
        virtual MamdaNewsStory& operator= (const MamdaNewsStory&  rhs);

        /**
         * Set all of the news story attributes.
         */
        virtual void setStory (
            const char*                 storyText,
            const char*                 storyId,
            uint16_t                    storyRevNum,
            Status                      storyStatus,
            const MamaDateTime&         storyTime,
            const MamaDateTime&         storyOrigTime,
            const MamdaNewsHeadlineId*  headlines,
            mama_size_t                 numHeadlines);

        /**
         * Set the news story text
         */
        virtual void setNewsStoryText (const char* storyText);

        /**
         * @return The text of the story.
         */
        virtual const char*  getNewsStoryText() const;

        /**
         * Set the news story id
         */
        virtual void setNewsStoryId (MamdaNewsStoryId storyId);

        /**
         * @return The unique (for the data source) story ID for this news
         * story.
         */
        virtual MamdaNewsStoryId  getNewsStoryId() const;

        /**
         * Set the story revision number
         */
        virtual void setNewsStoryRevNumber (uint16_t storyRevNum);

        /**
         * @return The revision number.  Note: the value returned is zero
         * if the data source does not provide revision numbers.
         */
        virtual uint16_t  getNewsStoryRevNumber() const;

        /**
         * Set the story status.
         */
        virtual void setStatus (Status status);

        /**
         * @return The story status.
         */
        virtual Status getStatus () const;

        /**
         * @return The latest story update time (e.g. time of correction).
         */
        virtual const MamaDateTime& getStoryTime () const;

        /**
         * @return The original story publish time.
         */
        virtual const MamaDateTime& getOrigStoryTime () const;

        /**
         * @return All of the headline IDs associated with this news story.
         */
        virtual void getHeadlines (const MamdaNewsHeadlineId*& headlines,
                                   mama_size_t&                numHeadlines) const;

        /**
         * Convert a string representation of a Status to the enumerated
         * representation.
         */
        static Status toStatus (const char*  strVal);

        /**
         * Convert an enumerated representation of a Status to a string
         * representation.
         */
        static const char* toString (Status  status);

    public:
        // Implementation subscription information:
        void setSubscInfo (MamaQueue*      queue,
                           MamaSource*     source);

        MamaQueue*      getQueue     () const;
        MamaSource*     getSource    () const;

    private:
        struct MamdaNewsStoryImpl;
        MamdaNewsStoryImpl& mImpl;
    };

} // namespace

#endif // MamdaNewsStoryH
