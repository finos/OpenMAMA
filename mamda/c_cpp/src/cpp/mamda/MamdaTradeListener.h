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

#ifndef MamdaTradeListenerH
#define MamdaTradeListenerH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaMsgListener.h>
#include <mamda/MamdaTradeRecap.h>
#include <mamda/MamdaTradeReport.h>
#include <mamda/MamdaTradeGap.h>
#include <mamda/MamdaTradeCancelOrError.h>
#include <mamda/MamdaTradeCorrection.h>
#include <mamda/MamdaTradeClosing.h>
#include <mamda/MamdaTradeDirection.h>
#include <mamda/MamdaTradeExecVenue.h>
#include <mamda/MamdaTradeOutOfSequence.h>
#include <mamda/MamdaTradePossiblyDuplicate.h>
#include <mamda/MamdaFieldState.h>
#include <mamda/MamdaTradeSide.h>

namespace Wombat
{

    class MamdaTradeHandler;

    /**
     * MamdaTradeListener is a class that specializes in handling trade
     * updates.  Developers provide their own implementation of the
     * MamdaTradeHandler interface and will be delivered notifications for
     * trades, trade cancels/error/corrections, and trade closing prices.
     * An obvious application for this MAMDA class is any kind of trade
     * tick capture application.
     *
     * Note: The MamdaTradeListener class caches trade-related field
     * values.  Among other reasons, caching of these fields makes it
     * possible to provide complete trade-related callbacks, even when the
     * publisher (e.g., feed handler) is only publishing deltas containing
     * modified fields.
     *
     * For details on the accessor methods for the cache data see the description
     * for the corresponding methods on the specific MamdaEvent derived classes.
     *
     * MamdaTradeListener should initialize the MamdaTradeFields class prior to
     * receiving the first message by calling
     * MamdaTradeFields::setDictionary() with a valid dictionary object
     * which contains Trade related fields.
     */
    class MAMDAExpDLL MamdaTradeListener 
        : public MamdaMsgListener
        , public MamdaTradeRecap
        , public MamdaTradeReport
        , public MamdaTradeGap
        , public MamdaTradeCancelOrError
        , public MamdaTradeCorrection
        , public MamdaTradeClosing
        , public MamdaTradeOutOfSequence
        , public MamdaTradePossiblyDuplicate
    {
    public:
        MamdaTradeListener ();
        virtual ~MamdaTradeListener ();

        void        addHandler                          (MamdaTradeHandler* handler);
        void        processPosDupAndOutOfSeqAsTransient (bool tf);
        void        resolvePossiblyDuplicate            (bool tf);
        void        usePosDupAndOutOfSeqHandlers        (bool tf);

        void        setCheckUpdatesForTrades            (bool check);
        
        const char*  getSide () const;   
        

        // Inherited from MamdaBasicRecap and MamdaBasicEvent
        const char*           getSymbol                     () const;
        const char*           getPartId                     () const;
        const MamaDateTime&   getSrcTime                    () const;
        const MamaDateTime&   getActivityTime               () const;
        const MamaDateTime&   getLineTime                   () const;
        const MamaDateTime&   getSendTime                   () const;
        const MamaMsgQual&    getMsgQual                    () const;
        const char*           getPubId                      () const;
        mama_seqnum_t         getEventSeqNum                () const;
        const MamaDateTime&   getEventTime                  () const;

        // Inherited from MamdaTradeRecap
        const MamaPrice&      getLastPrice                  () const;
        mama_quantity_t       getLastVolume                 () const;
        const char*           getLastPartId                 () const;
        const MamaDateTime&   getLastTime                   () const;
        const MamaPrice&      getIrregPrice                 () const;
        mama_quantity_t       getIrregVolume                () const;
        const char*           getIrregPartId                () const;
        const MamaDateTime&   getIrregTime                  () const;
        const MamaDateTime&   getTradeDate                  () const;
        mama_u32_t            getTradeCount                 () const;
        mama_quantity_t       getAccVolume                  () const;
        mama_quantity_t       getOffExAccVolume             () const;
        mama_quantity_t       getOnExAccVolume              () const;
        const MamaPrice&      getNetChange                  () const;
        double                getPctChange                  () const;
        MamdaTradeDirection   getTradeDirection             () const;
        const MamaPrice&      getOpenPrice                  () const;
        const MamaPrice&      getHighPrice                  () const;
        const MamaPrice&      getLowPrice                   () const;
        const MamaPrice&      getClosePrice                 () const;
        const MamaPrice&      getPrevClosePrice             () const;
        const MamaPrice&      getAdjPrevClosePrice          () const;
        const MamaDateTime&   getPrevCloseDate              () const;
        mama_u32_t            getBlockCount                 () const;
        mama_quantity_t       getBlockVolume                () const;
        double                getVwap                       () const;
        double                getOffExVwap                  () const;
        double                getOnExVwap                   () const;
        double                getTotalValue                 () const;
        double                getOffExTotalValue            () const;
        double                getOnExTotalValue             () const;
        double                getStdDev                     () const;
        double                getStdDevSum                  () const;
        double                getStdDevSumSquares           () const;
        const char*           getTradeUnits                 () const;
        mama_seqnum_t         getLastSeqNum                 () const;
        mama_seqnum_t         getHighSeqNum                 () const;
        mama_seqnum_t         getLowSeqNum                  () const;
        mama_seqnum_t         getTotalVolumeSeqNum          () const;
        const char*           getCurrencyCode               () const;
        const MamaPrice&      getSettlePrice                () const;
        const MamaDateTime&   getSettleDate                 () const;
        MamdaTradeExecVenue   getTradeExecVenue             () const;
        const MamaPrice&      getOffExchangeTradePrice      () const;
        const MamaPrice&      getOnExchangeTradePrice       () const;

        // Inherited from MamdaTradeReport
        const MamaPrice&      getTradePrice                 () const;
        mama_quantity_t       getTradeVolume                () const;
        const char*           getTradePartId                () const;
        const char*           getTradeQual                  () const;
        const char*           getTradeQualNative            () const;
        mama_u32_t            getTradeSellersSaleDays       () const;
        char                  getTradeStopStock             () const;
        bool                  getIsIrregular                () const;
        mama_u64_t            getOrderId                    () const;
        const char*           getUniqueId                   () const;
        const char*           getTradeId                    () const;
        const char*           getCorrTradeId                () const;
        const char*           getTradeAction                () const;    

        // Inherited from MamdaTradeGap
        mama_seqnum_t         getBeginGapSeqNum             () const;
        mama_seqnum_t         getEndGapSeqNum               () const;

        // Inherited from MamdaTradeCancelOrError
        bool                  getIsCancel                   () const;
        mama_seqnum_t         getOrigSeqNum                 () const;
        const MamaPrice&      getOrigPrice                  () const;
        mama_quantity_t       getOrigVolume                 () const;
        const char*           getOrigPartId                 () const;
        const char*           getOrigQual                   () const;
        const char*           getOrigQualNative             () const;
        mama_u32_t            getOrigSellersSaleDays        () const;
        char                  getOrigStopStock              () const;
        const char*           getOrigTradeId                () const;
        bool                  getGenericFlag                () const;
        char                  getShortSaleCircuitBreaker    () const;
        char                  getOrigShortSaleCircuitBreaker() const;
        char                  getCorrShortSaleCircuitBreaker() const;

        // Inherited from MamdaTradeCorrection
        const MamaPrice&      getCorrPrice                  () const;
        mama_quantity_t       getCorrVolume                 () const;
        const char*           getCorrPartId                 () const;
        const char*           getCorrQual                   () const;
        const char*           getCorrQualNative             () const;
        mama_u32_t            getCorrSellersSaleDays        () const;
        char                  getCorrStopStock              () const;

        // Inherited from MamdaTradeClosing
        bool                  getIsIndicative               () const;

        
        /*  FieldState Accessors    */
        MamdaFieldState    getSymbolFieldState                      () const;
        MamdaFieldState    getPartIdFieldState                      () const;
        MamdaFieldState    getSrcTimeFieldState                     () const;
        MamdaFieldState    getActivityTimeFieldState                () const;
        MamdaFieldState    getLineTimeFieldState                    () const;
        MamdaFieldState    getSendTimeFieldState                    () const;
        MamdaFieldState    getMsgQualFieldState                     () const;
        MamdaFieldState    getPubIdFieldState                       () const;
        MamdaFieldState    getEventSeqNumFieldState                 () const;
        MamdaFieldState    getEventTimeFieldState                   () const;
        MamdaFieldState    getLastPriceFieldState                   () const;
        MamdaFieldState    getLastVolumeFieldState                  () const;
        MamdaFieldState    getLastPartIdFieldState                  () const;
        MamdaFieldState    getLastTimeFieldState                    () const;
        MamdaFieldState    getIrregPriceFieldState                  () const;
        MamdaFieldState    getIrregVolumeFieldState                 () const;
        MamdaFieldState    getIrregPartIdFieldState                 () const;
        MamdaFieldState    getIrregTimeFieldState                   () const;
        MamdaFieldState    getTradeDateFieldState                   () const;
        MamdaFieldState    getSideFieldState                        () const;       
        MamdaFieldState    getTradeCounFieldState                   () const;
        MamdaFieldState    getAccVolumeFieldState                   () const;
        MamdaFieldState    getOffExAccVolumeFieldState              () const;
        MamdaFieldState    getOnExAccVolumeFieldState               () const;
        MamdaFieldState    getNetChangeFieldState                   () const;
        MamdaFieldState    getPctChangeFieldState                   () const;
        MamdaFieldState    getTradeDirectionFieldState              () const;
        MamdaFieldState    getOpenPriceFieldState                   () const;
        MamdaFieldState    getHighPriceFieldState                   () const;
        MamdaFieldState    getLowPriceFieldState                    () const;
        MamdaFieldState    getClosePriceFieldState                  () const;
        MamdaFieldState    getPrevClosePriceFieldState              () const;
        MamdaFieldState    getAdjPrevClosePriceFieldState           () const;
        MamdaFieldState    getPrevCloseDateFieldState               () const;
        MamdaFieldState    getBlockCountFieldState                  () const;
        MamdaFieldState    getBlockVolumeFieldState                 () const;
        MamdaFieldState    getVwapFieldState                        () const;
        MamdaFieldState    getOffExVwapFieldState                   () const;
        MamdaFieldState    getOnExVwapFieldState                    () const;
        MamdaFieldState    getTotalValueFieldState                  () const;
        MamdaFieldState    getOffExTotalValueFieldState             () const;
        MamdaFieldState    getOnExTotalValueFieldState              () const;
        MamdaFieldState    getStdDevFieldState                      () const;
        MamdaFieldState    getStdDevSumFieldState                   () const;
        MamdaFieldState    getStdDevSumSquaresFieldState            () const;
        MamdaFieldState    getTradeUnitsFieldState                  () const;
        MamdaFieldState    getLastSeqNumFieldState                  () const;
        MamdaFieldState    getHighSeqNumFieldState                  () const;
        MamdaFieldState    getLowSeqNumFieldState                   () const;
        MamdaFieldState    getTotalVolumeSeqNumFieldState           () const;
        MamdaFieldState    getCurrencyCodeFieldState                () const;
        MamdaFieldState    getSettlePriceFieldState                 () const;
        MamdaFieldState    getSettleDateFieldState                  () const;
        MamdaFieldState    getTradeExecVenuFieldState               () const;
        MamdaFieldState    getOffExchangeTradePriceFieldState       () const;
        MamdaFieldState    getOnExchangeTradePriceFieldState        () const;
        MamdaFieldState    getTradePriceFieldState                  () const;
        MamdaFieldState    getTradeVolumeFieldState                 () const;
        MamdaFieldState    getTradePartIdFieldState                 () const;
        MamdaFieldState    getTradeQualFieldState                   () const;
        MamdaFieldState    getTradeQualNativeFieldState             () const;
        MamdaFieldState    getTradeSellersSaleDaysFieldState        () const;
        MamdaFieldState    getTradeStopStockFieldState              () const;
        MamdaFieldState    getIsIrregularFieldState                 () const;
        MamdaFieldState    getOrderIdFieldState                     () const;
        MamdaFieldState    getUniqueIdFieldState                    () const;
        MamdaFieldState    getTradeIdFieldState                     () const;
        MamdaFieldState    getCorrTradeIdFieldState                 () const;
        MamdaFieldState    getTradeActionFieldState                 () const;
        MamdaFieldState    getTradeExecVenueFieldState              () const;
        MamdaFieldState    getBeginGapSeqNumFieldState              () const;
        MamdaFieldState    getEndGapSeqNumFieldState                () const;
        MamdaFieldState    getIsCancelFieldState                    () const;
        MamdaFieldState    getOrigSeqNumFieldState                  () const;
        MamdaFieldState    getOrigPriceFieldState                   () const;
        MamdaFieldState    getOrigVolumeFieldState                  () const;
        MamdaFieldState    getOrigPartIdFieldState                  () const;
        MamdaFieldState    getOrigQualFieldState                    () const;
        MamdaFieldState    getOrigQualNativeFieldState              () const;
        MamdaFieldState    getOrigSellersSaleDaysFieldState         () const;
        MamdaFieldState    getOrigStopStockFieldState               () const;
        MamdaFieldState    getCorrPriceFieldState                   () const;
        MamdaFieldState    getCorrVolumeFieldState                  () const;
        MamdaFieldState    getCorrPartIdFieldState                  () const;
        MamdaFieldState    getCorrQualFieldState                    () const;
        MamdaFieldState    getCorrQualNativeFieldState              () const;
        MamdaFieldState    getCorrSellersSaleDaysFieldState         () const;
        MamdaFieldState    getCorrStopStockFieldState               () const;
        MamdaFieldState    getIsIndicativeFieldState                () const;
        MamdaFieldState    getTradeCountFieldState                  () const;
        MamdaFieldState    getOrigTradeIdFieldState                 () const;
        MamdaFieldState    getGenericFlagFieldState                 () const;
        MamdaFieldState    getShortSaleCircuitBreakerFieldState     () const;
        MamdaFieldState    getOrigShortSaleCircuitBreakerFieldState () const;
        MamdaFieldState    getCorrShortSaleCircuitBreakerFieldState () const;

        /**
         * Implementation of MamdaListener interface.
         */
        virtual void onMsg (MamdaSubscription*  subscription,
                            const MamaMsg&      msg,
                            short               msgType);

        void assertEqual   (MamdaTradeListener* listener);
        void reset         (void);

        struct MamdaTradeListenerImpl;

    private:
        MamdaTradeListenerImpl& mImpl;
    };


} // namespace

#endif // MamdaTradeListenerH

