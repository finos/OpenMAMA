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

#ifndef MamdaQuoteListenerH
#define MamdaQuoteListenerH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaMsgListener.h>
#include <mamda/MamdaQuoteRecap.h>
#include <mamda/MamdaQuoteUpdate.h>
#include <mamda/MamdaQuoteGap.h>
#include <mamda/MamdaQuoteClosing.h>
#include <mamda/MamdaQuoteOutOfSequence.h>
#include <mamda/MamdaQuotePossiblyDuplicate.h>
#include <mamda/MamdaFieldState.h>

namespace Wombat
{

    class MamdaQuoteHandler;

    /**
     * MamdaQuoteListener is a class that specializes in handling quote
     * updates.  Developers provide their own implementation of the
     * MamdaQuoteHandler interface and will be delivered notifications for
     * quotes and quote closing prices.  An obvious application for this
     * MAMDA class is any kind of quote tick capture application.
     *
     * Note: The MamdaQuoteListener class caches quote-related field
     * values.  Among other reasons, caching of these fields makes it
     * possible to provide complete quote-related callbacks, even when the
     * publisher (e.g., feed handler) is only publishing deltas containing
     * modified fields.
     *
     * MamdaQuoteListener should initialize the MamdaQuoteFields class 
     * prior to receiving the first message by calling
     * MamdaQuoteFields::setDictionary() with a valid dictionary object
     * which contains Quote related fields.
     */
    class MAMDAExpDLL MamdaQuoteListener 
        : public MamdaMsgListener
        , public MamdaQuoteRecap
        , public MamdaQuoteUpdate
        , public MamdaQuoteGap
        , public MamdaQuoteClosing
        , public MamdaQuoteOutOfSequence
        , public MamdaQuotePossiblyDuplicate
    {
    public:
        MamdaQuoteListener ();
        virtual ~MamdaQuoteListener ();

        void        addHandler (MamdaQuoteHandler* handler);

        void        processPosDupAndOutOfSeqAsTransient (bool tf);
        void        resolvePossiblyDuplicate            (bool tf);
        void        usePosDupAndOutOfSeqHandlers        (bool tf);

        const char*          getSymbol                  () const;
        const char*          getPartId                  () const;
        const MamaDateTime&  getSrcTime                 () const;
        const MamaDateTime&  getActivityTime            () const;
        const MamaDateTime&  getLineTime                () const;
        const MamaDateTime&  getSendTime                () const;
        const MamaMsgQual&   getMsgQual                 () const;
        const char*          getPubId                   () const;
        const MamaPrice&     getBidPrice                () const;
        mama_quantity_t      getBidSize                 () const;
        mama_quantity_t      getBidDepth                () const;
        const char*          getBidPartId               () const;
        const MamaPrice&     getBidClosePrice           () const;
        const MamaDateTime&  getBidCloseDate            () const;
        const MamaPrice&     getBidPrevClosePrice       () const;
        const MamaDateTime&  getBidPrevCloseDate        () const;
        const MamaPrice&     getBidHigh                 () const;
        const MamaPrice&     getBidLow                  () const;
        const MamaPrice&     getAskPrice                () const;
        mama_quantity_t      getAskSize                 () const;
        mama_quantity_t      getAskDepth                () const;
        const char*          getAskPartId               () const;
        const MamaPrice&     getAskClosePrice           () const;
        const MamaDateTime&  getAskCloseDate            () const;
        const MamaPrice&     getAskPrevClosePrice       () const;
        const MamaDateTime&  getAskPrevCloseDate        () const;
        const MamaPrice&     getAskHigh                 () const;
        const MamaPrice&     getAskLow                  () const;
        const MamaPrice&     getQuoteMidPrice           () const;
        mama_u32_t           getQuoteCount              () const;
        const char*          getQuoteQualStr            () const;
        const char*          getQuoteQualNative         () const;
        mama_seqnum_t        getEventSeqNum             () const;
        const MamaDateTime&  getEventTime               () const;
        const MamaDateTime&  getQuoteDate               () const;
        mama_seqnum_t        getBeginGapSeqNum          () const;
        mama_seqnum_t        getEndGapSeqNum            () const;
        char                 getShortSaleBidTick        () const;
         
        const MamaDateTime&  getAskTime                 () const;
        const MamaDateTime&  getBidTime                 () const;
        const char*          getAskIndicator            () const;
        const char*          getBidIndicator            () const;
        mama_u32_t           getAskUpdateCount          () const;
        mama_u32_t           getBidUpdateCount          () const;
        double               getAskYield                () const;
        double               getBidYield                () const;
        const char*          getAskSizesList            () const;
        const char*          getBidSizesList            () const;
        char                 getShortSaleCircuitBreaker () const;

        /* IsModified Accessors */
        MamdaFieldState    getSymbolFieldState                  () const;
        MamdaFieldState    getBidPriceFieldState                () const;
        MamdaFieldState    getBidSizeFieldState                 () const;
        MamdaFieldState    getPartIdFieldState                  () const;
        MamdaFieldState    getSrcTimeFieldState                 () const;
        MamdaFieldState    getActivityTimeFieldState            () const;
        MamdaFieldState    getLineTimeFieldState                () const;
        MamdaFieldState    getSendTimeFieldState                () const;
        MamdaFieldState    getPubIdFieldState                   () const;
        MamdaFieldState    getMsgQualFieldState                 () const;
        MamdaFieldState    getPubIdModified                     () const;
        MamdaFieldState    getBidDepthFieldState                () const;
        MamdaFieldState    getBidPartIdFieldState               () const;
        MamdaFieldState    getBidClosePriceFieldState           () const;
        MamdaFieldState    getBidCloseDateFieldState            () const;
        MamdaFieldState    getBidPrevClosePriceFieldState       () const;
        MamdaFieldState    getBidPrevCloseDateFieldState        () const;
        MamdaFieldState    getBidHighFieldState                 () const;
        MamdaFieldState    getBidLowFieldState                  () const;
        MamdaFieldState    getAskPriceFieldState                () const;
        MamdaFieldState    getAskSizeFieldState                 () const;
        MamdaFieldState    getAskDepthFieldState                () const;
        MamdaFieldState    getAskPartIdFieldState               () const;
        MamdaFieldState    getAskClosePriceFieldState           () const;
        MamdaFieldState    getAskCloseDateFieldState            () const;
        MamdaFieldState    getAskPrevClosePriceFieldState       () const;
        MamdaFieldState    getAskPrevCloseDateFieldState        () const;
        MamdaFieldState    getAskHighFieldState                 () const;
        MamdaFieldState    getAskLowFieldState                  () const;
        MamdaFieldState    getQuoteMidPriceFieldState           () const;
        MamdaFieldState    getQuoteCountFieldState              () const;
        MamdaFieldState    getQuoteQualStrFieldState            () const;
        MamdaFieldState    getQuoteQualNativeFieldState         () const;
        MamdaFieldState    getEventSeqNumFieldState             () const;
        MamdaFieldState    getEventTimeFieldState               () const;
        MamdaFieldState    getQuoteDateFieldState               () const;
        MamdaFieldState    getBeginGapSeqNumFieldState          () const;
        MamdaFieldState    getEndGapSeqNumFieldState            () const;
        MamdaFieldState    getShortSaleBidTickFieldState        () const;
        
        MamdaFieldState    getAskTimeFieldState                 () const;
        MamdaFieldState    getBidTimeFieldState                 () const;
        MamdaFieldState    getAskIndicatorFieldState            () const;
        MamdaFieldState    getBidIndicatorFieldState            () const;
        MamdaFieldState    getAskUpdateCountFieldState          () const;
        MamdaFieldState    getBidUpdateCountFieldState          () const;
        MamdaFieldState    getAskYieldFieldState                () const;
        MamdaFieldState    getBidYieldFieldState                () const;
        MamdaFieldState    getAskSizesListFieldState            () const;
        MamdaFieldState    getBidSizesListFieldState            () const;
        MamdaFieldState    getShortSaleCircuitBreakerFieldState () const;
        
        /**
         * Implementation of MamdaListener interface.
         */
        virtual void onMsg         (MamdaSubscription*       subscription,
                                    const MamaMsg&           msg,
                                    short                    msgType);

        void         assertEqual   (MamdaQuoteListener*      listener);

        struct MamdaQuoteListenerImpl;

    private:
        MamdaQuoteListenerImpl& mImpl;
    };

}

#endif // MamdaQuoteListenerH
