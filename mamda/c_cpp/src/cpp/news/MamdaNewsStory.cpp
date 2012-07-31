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

#include <mamda/MamdaNewsStory.h>
#include "MamdaNewsUtils.h"
#include <vector>
#include <string>

using std::string;
using std::vector;

namespace Wombat
{

    struct MamdaNewsStory::MamdaNewsStoryImpl 
    {
    public:
        MamdaNewsStoryImpl  (MamdaNewsStory&  story);
        ~MamdaNewsStoryImpl ();

        void setStory (
            const char*                 storyText,
            const char*                 storyId,
            uint16_t                    storyRevNum,
            Status                      storyStatus,
            const MamaDateTime&         storyTime,
            const MamaDateTime&         storyOrigTime,
            const MamdaNewsHeadlineId*  headlines,
            mama_size_t                 numHeadlines);

        void copy (const MamdaNewsStoryImpl&  rhs);

        MamdaNewsStory&         mStory;
        string                  mStoryText;
        string                  mStoryId;
        uint16_t                mStoryRevNumber;
        MamdaNewsStory::Status  mStoryStatus;
        MamaDateTime            mStoryTime;
        MamaDateTime            mStoryOrigTime;
        MamdaNewsHeadlineId*    mStoryHeadlineIds;
        mama_size_t             mStoryNumHeadlineIds;
        MamaQueue*              mSubscQueue;
        MamaSource*             mSubscSource;
    };


    MamdaNewsStory::MamdaNewsStory ()
        : mImpl (*new MamdaNewsStoryImpl(*this))
    {
    }

    MamdaNewsStory::MamdaNewsStory (const MamdaNewsStory&  copy)
        : mImpl (*new MamdaNewsStoryImpl(*this))
    {
        operator=(copy);
    }

    MamdaNewsStory::~MamdaNewsStory ()
    {
        delete &mImpl;
    }

    MamdaNewsStory& MamdaNewsStory::operator= (const MamdaNewsStory& rhs)
    {
        if (&rhs != this)
        {
            mImpl.copy (rhs.mImpl);
        }
        return *this;
    }

    void MamdaNewsStory::setStory (
        const char*                 storyText,
        const char*                 storyId,
        uint16_t                    storyRevNum,
        Status                      storyStatus,
        const MamaDateTime&         storyTime,
        const MamaDateTime&         storyOrigTime,
        const MamdaNewsHeadlineId*  headlines,
        mama_size_t                 numHeadlines)
    {
        mImpl.setStory (storyText, 
                        storyId, 
                        storyRevNum, 
                        storyStatus,
                        storyTime, 
                        storyOrigTime, 
                        headlines, 
                        numHeadlines);
    }

    void MamdaNewsStory::setNewsStoryText (const char* storyText)
    {
        mImpl.mStoryText = storyText;
    }

    const char* MamdaNewsStory::getNewsStoryText() const
    {
        return mImpl.mStoryText.c_str();
    }

    void MamdaNewsStory::setNewsStoryId (MamdaNewsStoryId storyId)
    {
        mImpl.mStoryId = storyId;
    }

    MamdaNewsStoryId MamdaNewsStory::getNewsStoryId() const
    {
        return mImpl.mStoryId.c_str();
    }

    void MamdaNewsStory::setNewsStoryRevNumber (uint16_t storyRevNum)
    {
        mImpl.mStoryRevNumber = storyRevNum;
    }

    uint16_t MamdaNewsStory::getNewsStoryRevNumber() const
    {
        return mImpl.mStoryRevNumber;
    }

    void MamdaNewsStory::setStatus (Status  status)
    {
        mImpl.mStoryStatus = status;
    }

    const MamaDateTime& MamdaNewsStory::getStoryTime() const
    {
        return mImpl.mStoryTime;
    }

    const MamaDateTime& MamdaNewsStory::getOrigStoryTime() const
    {
        return mImpl.mStoryOrigTime;
    }

    void MamdaNewsStory::getHeadlines(
        const MamdaNewsHeadlineId*&  headlines,
        mama_size_t&                 numHeadlines) const
    {
        headlines    = mImpl.mStoryHeadlineIds;
        numHeadlines = mImpl.mStoryNumHeadlineIds;
    }

    MamdaNewsStory::Status MamdaNewsStory::getStatus () const
    {
        return mImpl.mStoryStatus;
    }

    MamdaNewsStory::Status MamdaNewsStory::toStatus (const char*  str)
    {
        Status result = UNKNOWN;
        if (str)
        {
            if (strcmp(str,"NoStory") == 0)
                result = NO_STORY;
            else if (strcmp(str,"Full") == 0)
                result = FULL_STORY;
            else if (strcmp(str,"Fetching") == 0)
                result = FETCHING_STORY;
            else if (strcmp(str,"Delayed") == 0)
                result = DELAYED_STORY;
            else if (strcmp(str,"NotFound") == 0)
                result = NOT_FOUND;
            else if (strcmp(str,"Deleted") == 0)
                result = DELETED;
            else if (strcmp(str,"Expired") == 0)
                result = EXPIRED;
            else if (strcmp(str,"Unknown") == 0)
                result = UNKNOWN;
        }
        return result;
    }

    const char* MamdaNewsStory::toString (Status status)
    {
        switch (status)
        {
            case NO_STORY       : return "NoStory";
            case FULL_STORY     : return "Full";
            case FETCHING_STORY : return "Fetching";
            case DELAYED_STORY  : return "Delayed";
            case NOT_FOUND      : return "NotFound";
            case DELETED        : return "Deleted";
            case EXPIRED        : return "Expired";
            case UNKNOWN        : return "Unknown";
        }
        return "Unknown";
    }

    void MamdaNewsStory::setSubscInfo (
        MamaQueue*      queue,
        MamaSource*     source)
    {
        mImpl.mSubscQueue     = queue;
        mImpl.mSubscSource    = source;
    }

    MamaSource* MamdaNewsStory::getSource() const
    {
        return mImpl.mSubscSource;
    }

    MamaQueue* MamdaNewsStory::getQueue() const
    {
        return mImpl.mSubscQueue;
    }

    MamdaNewsStory::MamdaNewsStoryImpl::MamdaNewsStoryImpl (
        MamdaNewsStory&  story)
        : mStory                (story)
        , mStoryRevNumber       (0)
        , mStoryHeadlineIds     (NULL)
        , mStoryNumHeadlineIds  (0)
    {
    }

    MamdaNewsStory::MamdaNewsStoryImpl::~MamdaNewsStoryImpl()
    {
        destroyStringArray (mStoryHeadlineIds, mStoryNumHeadlineIds);
    }

    void MamdaNewsStory::MamdaNewsStoryImpl::setStory (
        const char*                 storyText,
        const char*                 storyId,
        uint16_t                    storyRevNum,
        MamdaNewsStory::Status      storyStatus,
        const MamaDateTime&         storyTime,
        const MamaDateTime&         storyOrigTime,
        const MamdaNewsHeadlineId*  headlines,
        mama_size_t                 numHeadlines)
    {
        mStoryText      = storyText;
        mStoryId        = storyId;
        mStoryRevNumber = storyRevNum;
        mStoryStatus    = storyStatus;
        mStoryTime      = storyTime;
        mStoryOrigTime  = storyOrigTime;

        copyStringArray (mStoryHeadlineIds, 
                         mStoryNumHeadlineIds,
                         (const char**)headlines, 
                         numHeadlines);
    }

    void MamdaNewsStory::MamdaNewsStoryImpl::copy (
        const MamdaNewsStoryImpl&  rhs)
    {
        mStoryText      = rhs.mStoryText;
        mStoryId        = rhs.mStoryId;
        mStoryRevNumber = rhs.mStoryRevNumber;
        mStoryStatus    = rhs.mStoryStatus;
        mStoryTime      = rhs.mStoryTime;
        mStoryOrigTime  = rhs.mStoryOrigTime;

        copyStringArray (mStoryHeadlineIds, 
                         mStoryNumHeadlineIds,
                         rhs.mStoryHeadlineIds, 
                         rhs.mStoryNumHeadlineIds);

        mSubscSource    = rhs.mSubscSource;
        mSubscQueue     = rhs.mSubscQueue;
    }

} // namespace
