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

#include <mamda/MamdaNewsHeadline.h>
#include <mamda/MamdaSubscription.h>
#include "MamdaNewsUtils.h"
#include <vector>
#include <string>

using std::string;
using std::vector;

namespace Wombat
{

    struct MamdaNewsHeadline::MamdaNewsHeadlineImpl 
    {
    public:
        MamdaNewsHeadlineImpl  (MamdaNewsHeadline&  headline);
        ~MamdaNewsHeadlineImpl ();

        void copy  (const MamdaNewsHeadlineImpl&  rhs);
        void clear ();
        
        MamdaNewsHeadline&           mHeadline;

        // Members for MamdaNewsMetaData:
        string                       mText;
        string                       mId;
        string                       mStoryId;
        string                       mOrigStoryId;
        string                       mSourceId;
        string                       mOrigSourceId;
        string                       mLanguageId;
        bool                         mHasStory;
        MamdaNewsPriority            mPriority;
        mama_u16_t                   mStoryRevNumber;
        const char**                 mNativeCodes;
        const char**                 mNativeSymbols;
        const char**                 mIndustries;
        const char**                 mMarketSectors;
        const char**                 mRegions;
        const char**                 mCountries;
        const char**                 mProducts;
        const char**                 mTopics;
        const char**                 mMiscCodes;
        const char**                 mSymbols;
        mama_size_t                  mNumNativeCodes;
        mama_size_t                  mNumNativeSymbols;
        mama_size_t                  mNumIndustries;
        mama_size_t                  mNumMarketSectors;
        mama_size_t                  mNumRegions;
        mama_size_t                  mNumCountries;
        mama_size_t                  mNumProducts;
        mama_size_t                  mNumTopics;
        mama_size_t                  mNumMiscCodes;
        mama_size_t                  mNumSymbols;

        // Members for MamdaBasicEvent:
        string                       mSymbol;          MamdaFieldState     mSymbolFieldState;  
        MamaDateTime                 mSrcTime;         MamdaFieldState     mSrcTimeFieldState; 
        MamaDateTime                 mActTime;         MamdaFieldState     mActTimeFieldState;
        MamaDateTime                 mLineTime;        MamdaFieldState     mLineTimeFieldState;
        MamaDateTime                 mSendTime;        MamdaFieldState     mSendTimeFieldState;
        MamaMsgQual                  mMsgQual;         MamdaFieldState     mMsgQualFieldState; 
        MamaDateTime                 mOrigTime;        MamdaFieldState     mOrigTimeFieldState;
        MamaDateTime                 mEventTime;       MamdaFieldState     mEventTimeFieldState;  
        mama_seqnum_t                mEventSeqNum;     MamdaFieldState     mEventSeqNumFieldState;
                                                       MamdaFieldState     mPartIdFieldState;

        MamdaSubscription*           mSubscInfo;
        MamaQueue*                   mSubscQueue;
        MamaSource*                  mSubscSource;
    };


    MamdaNewsHeadline::MamdaNewsHeadline ()
        : mImpl (*new MamdaNewsHeadlineImpl(*this))
    {
    }

    MamdaNewsHeadline::MamdaNewsHeadline (const MamdaNewsHeadline&  copy)
        : mImpl (*new MamdaNewsHeadlineImpl(*this))
    {
        operator=(copy);
    }

    MamdaNewsHeadline::~MamdaNewsHeadline ()
    {
        delete &mImpl;
    }

    MamdaNewsHeadline& MamdaNewsHeadline::operator= (const MamdaNewsHeadline& rhs)
    {
        if (&rhs != this)
        {
            mImpl.copy (rhs.mImpl);
        }
        return *this;
    }

    const char* MamdaNewsHeadline::getHeadlineText() const
    {
        return mImpl.mText.c_str();
    }

    MamdaNewsHeadlineId MamdaNewsHeadline::getHeadlineId() const
    {
        return mImpl.mId.c_str();
    }

    MamdaNewsStoryId MamdaNewsHeadline::getNewsStoryId() const
    {
        return mImpl.mStoryId.c_str();
    }

    MamdaNewsStoryId MamdaNewsHeadline::getNewsOrigStoryId() const
    {
        return mImpl.mOrigStoryId.c_str();
    }

    bool MamdaNewsHeadline::getNewsHasStory() const
    {
        return mImpl.mHasStory;
    }

    const char* MamdaNewsHeadline::getNewsSourceId() const
    {
        return mImpl.mSourceId.c_str();
    }

    const char* MamdaNewsHeadline::getNewsOrigSourceId() const
    {
        return mImpl.mOrigSourceId.c_str();
    }

    const char* MamdaNewsHeadline::getLanguageId() const
    {
        return mImpl.mLanguageId.c_str();
    }

    void MamdaNewsHeadline::getNativeCodes (const char**&  codes,
                                            mama_size_t&   size) const
    {
        codes = mImpl.mNativeCodes;
        size  = mImpl.mNumNativeCodes;
    }

    void MamdaNewsHeadline::getNativeRelatedSymbols (const char**&  symbols,
                                                     mama_size_t&   size) const
    {
        symbols = mImpl.mNativeSymbols;
        size    = mImpl.mNumNativeSymbols;
    }

    void MamdaNewsHeadline::getIndustries (const char**&  industries,
                                           mama_size_t&   size) const
    {
        industries = mImpl.mIndustries;
        size       = mImpl.mNumIndustries;
    }

    void MamdaNewsHeadline::getMarketSectors (const char**&  marketSectors,
                                              mama_size_t&   size) const
    {
        marketSectors = mImpl.mMarketSectors;
        size          = mImpl.mNumMarketSectors;
    }

    void MamdaNewsHeadline::getRegions (const char**&  regions,
                                        mama_size_t&   size) const
    {
        regions = mImpl.mRegions;
        size    = mImpl.mNumRegions;
    }

    void MamdaNewsHeadline::getCountries (const char**&  countries,
                                          mama_size_t&   size) const
    {
        countries = mImpl.mCountries;
        size = mImpl.mNumCountries;
    }

    void MamdaNewsHeadline::getProducts (const char**&  products,
                                         mama_size_t&   size) const
    {
        products = mImpl.mProducts;
        size     = mImpl.mNumProducts;
    }

    void MamdaNewsHeadline::getTopics (const char**&  topics,
                                       mama_size_t&   size) const
    {
        topics = mImpl.mTopics;
        size   = mImpl.mNumTopics;
    }

    void MamdaNewsHeadline::getMiscCodes (const char**&  miscCodes,
                                          mama_size_t&   size) const
    {
        miscCodes = mImpl.mMiscCodes;
        size      = mImpl.mNumMiscCodes;
    }

    void MamdaNewsHeadline::getRelatedSymbols (const char**&  symbols,
                                               mama_size_t&   size) const
    {
        symbols = mImpl.mSymbols;
        size    = mImpl.mNumSymbols;
    }

    MamdaNewsPriority MamdaNewsHeadline::getNewsPriority() const
    {
        return mImpl.mPriority;
    }

    mama_u16_t MamdaNewsHeadline::getNewsStoryRevNumber() const
    {
        return mImpl.mStoryRevNumber;
    }

    const char* MamdaNewsHeadline::getSymbol() const
    {
        return mImpl.mSymbol.c_str();
    }

    const char* MamdaNewsHeadline::getPartId() const
    {
        return "";
    }

    const MamaDateTime& MamdaNewsHeadline::getSrcTime() const
    {
        return mImpl.mSrcTime;
    }

    const MamaDateTime& MamdaNewsHeadline::getActivityTime() const
    {
        return mImpl.mActTime;
    }

    const MamaDateTime& MamdaNewsHeadline::getLineTime() const
    {
        return mImpl.mLineTime;
    }

    const MamaDateTime& MamdaNewsHeadline::getSendTime() const
    {
        return mImpl.mSendTime;
    }

    const MamaMsgQual& MamdaNewsHeadline::getMsgQual() const
    {
        return mImpl.mMsgQual;
    }

    mama_seqnum_t MamdaNewsHeadline::getEventSeqNum() const
    {
        return mImpl.mEventSeqNum;
    }

    const MamaDateTime& MamdaNewsHeadline::getEventTime() const
    {
        return mImpl.mEventTime;
    }

    const MamaDateTime& MamdaNewsHeadline::getNewsOrigStoryTime() const
    {
        return mImpl.mOrigTime;
    }

    /*  FieldState Accessors        */

    MamdaFieldState MamdaNewsHeadline::getSymbolFieldState() const
    {
        return mImpl.mSymbolFieldState;
    }

    MamdaFieldState MamdaNewsHeadline::getPartIdFieldState() const
    {
        return mImpl.mPartIdFieldState;
    }

    MamdaFieldState MamdaNewsHeadline::getSrcTimeFieldState() const
    {
        return mImpl.mSrcTimeFieldState;
    }

    MamdaFieldState MamdaNewsHeadline::getActivityTimeFieldState() const
    {
        return mImpl.mActTimeFieldState;
    }

    MamdaFieldState MamdaNewsHeadline::getLineTimeFieldState() const
    {
        return mImpl.mLineTimeFieldState;
    }

    MamdaFieldState MamdaNewsHeadline::getSendTimeFieldState() const
    {
        return mImpl.mSendTimeFieldState;
    }

    MamdaFieldState MamdaNewsHeadline::getMsgQualFieldState() const
    {
        return mImpl.mMsgQualFieldState;
    }

    MamdaFieldState MamdaNewsHeadline::getEventSeqNumFieldState() const
    {
        return mImpl.mEventSeqNumFieldState;
    }

    MamdaFieldState MamdaNewsHeadline::getEventTimeFieldState() const
    {
        return mImpl.mEventTimeFieldState;
    }

    /*  End FieldState Accessors    */


    void MamdaNewsHeadline::setHeadlineText (const char*  headlineText)
    {
        if (headlineText)
            mImpl.mText = headlineText;
        else
            mImpl.mText.clear();

    }

    void MamdaNewsHeadline::setHeadlineId (const char*  headlineId)
    {
        if (headlineId)
            mImpl.mId = headlineId;
        else
            mImpl.mId.clear();
    }

    void MamdaNewsHeadline::setStoryId (const char*  storyId)
    {
        if (storyId)
            mImpl.mStoryId = storyId;
        else
            mImpl.mStoryId.clear();
    }

    void MamdaNewsHeadline::setOrigStoryId (const char*  storyId)
    {
        if (storyId)
            mImpl.mOrigStoryId = storyId;
        else
            mImpl.mOrigStoryId.clear();

    }

    void MamdaNewsHeadline::setSourceId (const char*  sourceId)
    {
        if (sourceId)
            mImpl.mSourceId = sourceId;
        else
            mImpl.mSourceId.clear();
    }

    void MamdaNewsHeadline::setOrigSourceId (const char*  origSourceId)
    {
        if (origSourceId)
            mImpl.mOrigSourceId = origSourceId;
        else
            mImpl.mOrigSourceId.clear();

    }

    void MamdaNewsHeadline::setHasStory (bool  hasStory)
    {
        mImpl.mHasStory = hasStory;
    }

    void MamdaNewsHeadline::setLanguageId (const char*  langId)
    {
        if (langId)
            mImpl.mLanguageId = langId;
        else
            mImpl.mLanguageId.clear();
    }

    void MamdaNewsHeadline::setNativeCodes (
        const char**  codes,
        mama_size_t   numCodes)
    {
        copyStringArray (mImpl.mNativeCodes,
                         mImpl.mNumNativeCodes,
                         codes, 
                         numCodes);
    }

    void MamdaNewsHeadline::setNativeRelatedSymbols (
        const char**  symbols,
        mama_size_t   numSymbols)
    {
        copyStringArray (mImpl.mNativeSymbols,
                         mImpl.mNumNativeSymbols,
                         symbols, 
                         numSymbols);
    }

    void MamdaNewsHeadline::setIndustries (
        const char**  industries,
        mama_size_t   numIndustries)
    {
        copyStringArray (mImpl.mIndustries, 
                         mImpl.mNumIndustries,
                         industries, 
                         numIndustries);
    }

    void MamdaNewsHeadline::setMarketSectors (
        const char**  marketSectors,
        mama_size_t   numMarketSectors)
    {
        copyStringArray (mImpl.mMarketSectors, 
                         mImpl.mNumMarketSectors,
                         marketSectors, 
                         numMarketSectors);
    }

    void MamdaNewsHeadline::setRegions (
        const char**  regions,
        mama_size_t   numRegions)
    {
        copyStringArray (mImpl.mRegions, 
                         mImpl.mNumRegions,
                         regions, 
                         numRegions);
    }

    void MamdaNewsHeadline::setCountries (
        const char**  countries,
        mama_size_t   numCountries)
    {
        copyStringArray (mImpl.mCountries, 
                         mImpl.mNumCountries,
                         countries, 
                         numCountries);
    }

    void MamdaNewsHeadline::setProducts (
        const char**  products,
        mama_size_t   numProducts)
    {
        copyStringArray (mImpl.mProducts, 
                         mImpl.mNumProducts,
                         products, 
                         numProducts);
    }

    void MamdaNewsHeadline::setTopics (
        const char**  topics,
        mama_size_t   size)
    {
        copyStringArray (mImpl.mTopics, 
                         mImpl.mNumTopics,
                         topics,
                         size);
    }

    void MamdaNewsHeadline::setMiscCodes (
        const char**  miscCodes,
        mama_size_t   numMiscCodes)
    {
        copyStringArray (mImpl.mMiscCodes, 
                         mImpl.mNumMiscCodes,
                         miscCodes, 
                         numMiscCodes);
    }

    void MamdaNewsHeadline::setRelatedSymbols (
        const char**  symbols,
        mama_size_t   numSymbols)
    {
        copyStringArray (mImpl.mSymbols, 
                         mImpl.mNumSymbols,
                         symbols, 
                         numSymbols);
    }

    void MamdaNewsHeadline::setPriority (MamdaNewsPriority  priority)
    {
        mImpl.mPriority = priority;
    }

    void MamdaNewsHeadline::setStoryRevNumber (mama_u16_t   storyRevNum)
    {
        mImpl.mStoryRevNumber = storyRevNum;
    }

    void MamdaNewsHeadline::setEventSeqNum (mama_seqnum_t   seqNum)
    {
        mImpl.mEventSeqNum = seqNum;
    }

    void MamdaNewsHeadline::setEventTime (const MamaDateTime&  eventTime)
    {
        mImpl.mEventTime = eventTime;
    }

    void MamdaNewsHeadline::setOrigStoryTime (const MamaDateTime&  storyTime)
    {
        mImpl.mOrigTime = storyTime;
    }

    void MamdaNewsHeadline::setSrcTime (const MamaDateTime&  srcTime)
    {
        mImpl.mSrcTime = srcTime;
    }

    void MamdaNewsHeadline::setActivityTime (const MamaDateTime&  actTime)
    {
        mImpl.mActTime = actTime;
    }

    void MamdaNewsHeadline::setLineTime (const MamaDateTime&  lineTime)
    {
        mImpl.mLineTime = lineTime;
    }

    void MamdaNewsHeadline::setSendTime (const MamaDateTime&  sendTime)
    {
        mImpl.mSendTime = sendTime;
    }

    void MamdaNewsHeadline::setMsgQual (const MamaMsgQual& msgQualifier)
    {
        mImpl.mMsgQual = msgQualifier;
    }

    void MamdaNewsHeadline::setSubscriptionInfo (
        MamdaSubscription* subscInfo)
    {
        mImpl.mSubscInfo = subscInfo;
        mImpl.mSubscQueue     = subscInfo->getQueue();
        mImpl.mSubscSource    = subscInfo->getSource() ;
    }

    void MamdaNewsHeadline::setSubscInfo (MamaQueue*   queue,
                                          MamaSource*  source)
    {
        mImpl.mSubscQueue     = queue;
        mImpl.mSubscSource    = source;
    }

    MamaSource* MamdaNewsHeadline::getSource() const
    {
        return mImpl.mSubscSource;
    }

    MamaQueue* MamdaNewsHeadline::getQueue() const
    {
        return mImpl.mSubscQueue;
    }

    void MamdaNewsHeadline::clear()
    {
        mImpl.clear();
    }


    MamdaNewsHeadline::MamdaNewsHeadlineImpl::MamdaNewsHeadlineImpl (
        MamdaNewsHeadline&  headline)
        : mHeadline         (headline)
        , mHasStory         (false)
        , mPriority         (MAMDA_NEWS_PRIORITY_NONE)
        , mStoryRevNumber   (0)
        , mNativeCodes      (NULL)
        , mNativeSymbols    (NULL)
        , mIndustries       (NULL)
        , mMarketSectors    (NULL)
        , mRegions          (NULL)
        , mCountries        (NULL)
        , mProducts         (NULL)
        , mTopics           (NULL)
        , mMiscCodes        (NULL)
        , mSymbols          (NULL)
        , mNumNativeCodes   (0)
        , mNumNativeSymbols (0)
        , mNumIndustries    (0)
        , mNumMarketSectors (0)
        , mNumRegions       (0)
        , mNumCountries     (0)
        , mNumProducts      (0)
        , mNumTopics        (0)
        , mNumMiscCodes     (0)
        , mNumSymbols       (0)
        , mMsgQual          (0)
        , mEventSeqNum      (0)
        , mSubscInfo        (NULL)
    {
    }

    MamdaNewsHeadline::MamdaNewsHeadlineImpl::~MamdaNewsHeadlineImpl ()
    {
        destroyStringArray (mNativeCodes,    mNumNativeCodes);
        destroyStringArray (mNativeSymbols,  mNumNativeSymbols);
        destroyStringArray (mIndustries,     mNumIndustries);
        destroyStringArray (mMarketSectors,  mNumMarketSectors);
        destroyStringArray (mRegions,        mNumRegions);
        destroyStringArray (mCountries,      mNumCountries);
        destroyStringArray (mProducts,       mNumProducts);
        destroyStringArray (mTopics,         mNumTopics);
        destroyStringArray (mMiscCodes,      mNumMiscCodes);
        destroyStringArray (mSymbols,        mNumSymbols);
    }

    void MamdaNewsHeadline::MamdaNewsHeadlineImpl::copy (
        const MamdaNewsHeadlineImpl&  rhs)
    {
        mText           = rhs.mText;
        mId             = rhs.mId;
        mStoryId        = rhs.mStoryId;
        mOrigStoryId    = rhs.mOrigStoryId;
        mSourceId       = rhs.mSourceId;
        mOrigSourceId   = rhs.mOrigSourceId;
        mLanguageId     = rhs.mLanguageId;
        mHasStory       = rhs.mHasStory;

        copyStringArray (mNativeCodes,        mNumNativeCodes,
                         rhs.mNativeCodes,    rhs.mNumNativeCodes);
        copyStringArray (mNativeSymbols,      mNumNativeSymbols,
                         rhs.mNativeSymbols,  rhs.mNumNativeSymbols);
        copyStringArray (mIndustries,         mNumIndustries,
                         rhs.mIndustries,     rhs.mNumIndustries);
        copyStringArray (mMarketSectors,      mNumMarketSectors,
                         rhs.mMarketSectors,  rhs.mNumMarketSectors);
        copyStringArray (mRegions,            mNumRegions,
                         rhs.mRegions,        rhs.mNumRegions);
        copyStringArray (mCountries,          mNumCountries,
                         rhs.mCountries,      rhs.mNumCountries);
        copyStringArray (mProducts,           mNumProducts,
                         rhs.mProducts,       rhs.mNumProducts);
        copyStringArray (mTopics,             mNumTopics,
                         rhs.mTopics,         rhs.mNumTopics);
        copyStringArray (mMiscCodes,          mNumMiscCodes,
                         rhs.mMiscCodes,      rhs.mNumMiscCodes);
        copyStringArray (mSymbols,            mNumSymbols,
                         rhs.mSymbols,        rhs.mNumSymbols);

        mPriority       = rhs.mPriority;
        mStoryRevNumber = rhs.mStoryRevNumber;
        mSymbol         = rhs.mSymbol;
        mSrcTime        = rhs.mSrcTime;
        mActTime        = rhs.mActTime;
        mLineTime       = rhs.mLineTime;
        mSendTime       = rhs.mSendTime;
        mMsgQual        = rhs.mMsgQual;
        mEventSeqNum    = rhs.mEventSeqNum;
        mOrigTime       = rhs.mOrigTime;
        mEventTime      = rhs.mEventTime;
        mSubscInfo      = rhs.mSubscInfo;
        mSubscQueue     = rhs.mSubscQueue;
        mSubscSource    = rhs.mSubscSource;

    }

    void MamdaNewsHeadline::MamdaNewsHeadlineImpl::clear ()
    {
        mHasStory               = false;
        mPriority               = MAMDA_NEWS_PRIORITY_NONE;
        mNativeCodes            = NULL;
        mNativeSymbols          = NULL;
        mIndustries             = NULL;
        mMarketSectors          = NULL;
        mRegions                = NULL;
        mCountries              = NULL;
        mProducts               = NULL;
        mTopics                 = NULL;
        mMiscCodes              = NULL;
        mSymbols                = NULL;
        mSubscInfo              = NULL;
        mNumNativeCodes         = 0;
        mNumNativeSymbols       = 0;
        mNumIndustries          = 0;
        mNumMarketSectors       = 0;
        mNumRegions             = 0;
        mNumCountries           = 0;
        mNumProducts            = 0;
        mNumTopics              = 0;
        mNumMiscCodes           = 0;
        mNumSymbols             = 0;
        mMsgQual                = 0;
        mEventSeqNum            = 0;
        mStoryRevNumber         = 0;
        mText                   = "";
        mId                     = "";
        mStoryId                = "";
        mOrigStoryId            = "";
        mSourceId               = "";
        mOrigSourceId           = "";
        mLanguageId             = "";

        mSymbolFieldState       = NOT_INITIALISED;  
        mPartIdFieldState       = NOT_INITIALISED;   
        mSrcTimeFieldState      = NOT_INITIALISED;  
        mActTimeFieldState      = NOT_INITIALISED; 
        mLineTimeFieldState     = NOT_INITIALISED; 
        mSendTimeFieldState     = NOT_INITIALISED; 
        mMsgQualFieldState      = NOT_INITIALISED;    
        mEventTimeFieldState    = NOT_INITIALISED;  
        mEventSeqNumFieldState  = NOT_INITIALISED;
        
    }

} // namespace
