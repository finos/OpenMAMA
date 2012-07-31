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

#ifndef MamdaOrderImbalanceListenerH
#define MamdaOrderImbalanceListenerH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaMsgListener.h>
#include <mamda/MamdaOrderImbalanceUpdate.h>
#include <mamda/MamdaOrderImbalanceRecap.h>
#include <mamda/MamdaFieldState.h>

namespace Wombat
{

    class MamdaOrderImbalanceHandler;

    /**
     * A MamdaOrderImbalanceListener is class that specializes in handling
     * an imbalance order updates. An imbalance order occurs when too many 
     * orders of a particular type - either buy, sell or limit - for listed
     * securities and not enough of the other, matching orders are received by 
     * an exchange.
     * Developers provide their own implementation of the
     * MamdaOrderImbalanceHandler interface and will be delivered notifications 
     * for imbalance updates.  An obvious application for this
     * MAMDA class is any kind of tick capture application.
     *
     * MamdaOrderImbalanceListener should initialize the MamdaOrderImbalanceFields
     * class prior to receiving the first message by calling
     * MamdaOrderImbalanceFields::setDictionary() with a valid dictionary object
     * which contains OrderImbalance related fields.
    */
    class MAMDAExpDLL MamdaOrderImbalanceListener 
        : public MamdaOrderImbalanceUpdate
        , public MamdaMsgListener
        , public MamdaOrderImbalanceRecap
    {
    public:
        MamdaOrderImbalanceListener ();
        virtual ~MamdaOrderImbalanceListener();

        /**
         * Add a specialized order imbalance handler.
         */
        void               addHandler (MamdaOrderImbalanceHandler* handler);

        MamaPrice&         getHighIndicationPrice           () const;
        MamdaFieldState    getHighIndicationPriceFieldState () const;

        MamaPrice&         getLowIndicationPrice            () const;
        MamdaFieldState    getLowIndicationPriceFieldState  () const;

        MamaPrice&         getImbalancePrice                () const;
        MamdaFieldState    getImbalancePriceFieldState      () const;

        int64_t            getBuyVolume                     () const;
        MamdaFieldState    getBuyVolumeFieldState           () const;

        int64_t            getSellVolume                    () const;
        MamdaFieldState    getSellVolumeFieldState          () const;

        int64_t            getMatchVolume                   () const;
        MamdaFieldState    getMatchVolumeFieldState         () const;

        const char*        getImbalanceState                () const;
        MamdaFieldState    getImbalanceStateFieldState      () const;

        MamaPrice&         getMatchPrice                    () const;
        MamdaFieldState    getMatchPriceFieldState          () const;

        MamaPrice&         getFarClearingPrice              () const;
        MamdaFieldState    getFarClearingPriceFieldState    () const;

        MamaPrice&         getNearClearingPrice             () const;
        MamdaFieldState    getNearClearingPriceFieldState   () const;

        char               getNoClearingPrice               () const;
        MamdaFieldState    getNoClearingPriceFieldState     () const;

        char               getPriceVarInd                   () const;
        MamdaFieldState    getPriceVarIndFieldState         () const;

        char               getCrossType                     () const;
        MamdaFieldState    getCrossTypeFieldState           () const;

        MamaDateTime&      getEventTime                     () const;
        MamdaFieldState    getEventTimeFieldState           () const;

        mama_seqnum_t      getEventSeqNum                   () const;
        MamdaFieldState    getEventSeqNumFieldState         () const;

        MamaDateTime&      getSrcTime                       () const;
        MamdaFieldState    getSrcTimeFieldState             () const;

        MamaDateTime&      getActivityTime                  () const;
        MamdaFieldState    getActivityTimeFieldState        () const;

        mama_i32_t         getMsgType                       () const;
        MamdaFieldState    getMsgTypeFieldState             () const;

        const char*        getIssueSymbol                   () const;
        MamdaFieldState    getIssueSymbolFieldState         () const;

        const char*        getPartId                        () const;
        MamdaFieldState    getPartIdFieldState              () const;

        mama_seqnum_t      getSeqNum                        () const;
        MamdaFieldState    getSeqNumFieldState              () const;

        const char*        getSecurityStatusOrig            () const;
        MamdaFieldState    getSecurityStatusOrigFieldState  () const;

        MamaDateTime&      getSecurityStatusTime            () const;
        MamdaFieldState    getSecurityStatusTimeFieldState  () const;

        MamaDateTime&      getAuctionTime                   () const;
        MamdaFieldState    getAuctionTimeFieldState         () const;

        MamaDateTime&      getLineTime                      () const;
        MamdaFieldState    getLineTimeFieldState            () const;

        MamaDateTime&      getSendTime                      () const;
        MamdaFieldState    getSendTimeFieldState            () const;

        const char*        getSymbol                        () const;
        MamdaFieldState    getSymbolFieldState              () const;

        const MamaMsgQual& getMsgQual                       () const;
        MamdaFieldState    getMsgQualFieldState             () const;

        void   updateFieldStates();

        /**
         * Implementation of MamdaListener interface.
         */
        virtual void onMsg (MamdaSubscription* subscription,
                            const MamaMsg&     msg,
                            short              msgType);

        struct MamdaOrderImbalanceListenerImpl;
    private:
        MamdaOrderImbalanceListenerImpl& mImpl;
       

    };//MamdaOrderImbalanceListenerH

}
#endif
