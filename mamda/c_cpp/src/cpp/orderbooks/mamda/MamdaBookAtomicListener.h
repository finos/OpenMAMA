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

#ifndef MamdaBookAtomicListenerH
#define MamdaBookAtomicListenerH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaMsgListener.h>
#include <mamda/MamdaBookAtomicLevel.h>
#include <mamda/MamdaBookAtomicLevelEntry.h>
#include <mamda/MamdaBookAtomicGap.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaFieldState.h>
#include <mamda/MamdaOrderBookTypes.h>

namespace Wombat
{

    class MamdaBookAtomicBookHandler;
    class MamdaBookAtomicLevelHandler;
    class MamdaBookAtomicLevelEntryHandler;

    /**
     * MamdaBookAtomicListener is a class that specializes in handling
     * order book updates. Unlike the MamdaOrderBookListener no actual 
     * order book is built or maintained. The sole purpose of this 
     * is to provide clients direct access to the orderbook updates
     * without the overhead of maintaining a book. Developers provide 
     * their own implementation of either or both the 
     * MamdaBookAtomicLevelHandler and the MamdaBookAtomicLevelEntryHandler
     * interfaces and will be delivered notifications for order book 
     * recaps and deltas. While the MamdaBookAtomicLevelHandler handles 
     * recaps and deltas at a Price Level granularity the 
     * MamdaBookAtomicLevelEntryHandler handles recaps and deltas at a 
     * Price Level Entry level (both level \& entry data).
     * Notifications for order book deltas include only the delta.
     * An obvious application for this MAMDA class is any kind of 
     * program trading application that needs to build its own order 
     * book or an application that needs to archive order book data.
     *
     * Note: The MamdaBookAtomicListener class caches only order book deltas.
     * If only an MamdaBookAtomicLevelHandler is add to this listener 
     * updates and deltas are only processed to Price Level granularity
     * and Entry Level data ignored saving on processing time. 
     * Among other reasons, caching of these fields makes it possible to
     * provide complete trade-related callbacks, even when the publisher
     * (e.g., feed handler) is only publishing deltas containing modified
     * fields.
     */
    class MAMDAOPTExpDLL MamdaBookAtomicListener : 
          public MamdaMsgListener
        , public MamdaBookAtomicLevel  
        , public MamdaBookAtomicLevelEntry  
        , public MamdaBookAtomicGap
    {
    public:
        MamdaBookAtomicListener ();
        virtual ~MamdaBookAtomicListener ();

        void addBookHandler (MamdaBookAtomicBookHandler* handler);
        void addLevelHandler (MamdaBookAtomicLevelHandler* handler);
        void addLevelEntryHandler (MamdaBookAtomicLevelEntryHandler* handler);

        // Inherited from MamdaBasicRecap and MamdaBasicEvent
        const char*          getSymbol         () const;
        const char*          getPartId         () const;
        const MamaDateTime&  getSrcTime        () const;
        const MamaDateTime&  getActivityTime   () const;
        const MamaDateTime&  getLineTime       () const;
        const MamaDateTime&  getSendTime       () const;
        const MamaMsgQual&   getMsgQual        () const;
        const MamaDateTime&  getEventTime      () const;
        mama_seqnum_t        getEventSeqNum    () const;

        MamdaFieldState      getSymbolFieldState()       const;
        MamdaFieldState      getPartIdFieldState()       const;
        MamdaFieldState      getEventSeqNumFieldState()  const;
        MamdaFieldState      getEventTimeFieldState()    const;
        MamdaFieldState      getSrcTimeFieldState()      const;
        MamdaFieldState      getActivityTimeFieldState() const;
        MamdaFieldState      getLineTimeFieldState()     const;
        MamdaFieldState      getSendTimeFieldState()     const;
        MamdaFieldState      getMsgQualFieldState()      const;
        
        // Inherited from MamdaBookAtomicLevel and MamdaBookAtomicLevelEntry
        mama_u32_t           getPriceLevelNumLevels      () const;
        mama_u32_t           getPriceLevelNum            () const;
        double               getPriceLevelPrice          () const;
        MamaPrice&           getPriceLevelMamaPrice      () const;
        mama_f64_t           getPriceLevelSize           () const;
        mama_i64_t           getPriceLevelSizeChange     () const;
        char                 getPriceLevelAction         () const;
        char                 getPriceLevelSide           () const;
        const MamaDateTime&  getPriceLevelTime           () const;
        mama_f32_t           getPriceLevelNumEntries     () const;
        mama_u32_t           getPriceLevelActNumEntries  () const;
        char                 getPriceLevelEntryAction    () const;
        char                 getPriceLevelEntryReason    () const;
        const char*          getPriceLevelEntryId        () const;
        mama_u64_t           getPriceLevelEntrySize      () const;  
        const MamaDateTime&  getPriceLevelEntryTime      () const;

        MamdaOrderBookTypes::OrderType getOrderType      () const;
        void setOrderType (MamdaOrderBookTypes::OrderType orderType) const;

        bool                 getHasMarketOrders          () const;
        void                 setProcessMarketOrders      (bool process) const;


        // Inherited from MamdaOrderBookGap
        mama_seqnum_t        getBeginGapSeqNum () const;
        mama_seqnum_t        getEndGapSeqNum   () const;

        // Implementation of MamdaListener interface.
        virtual void onMsg (MamdaSubscription*  subscription,
                            const MamaMsg&      msg,
                            short               msgType);

    private:
        struct MamdaBookAtomicListenerImpl;
        MamdaBookAtomicListenerImpl& mImpl;
    };

} // namespace

#endif // MamdaBookAtomicListenerH

