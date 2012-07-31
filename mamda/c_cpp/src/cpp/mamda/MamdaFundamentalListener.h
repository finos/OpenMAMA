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

#ifndef MamdaFundamentalListenerH
#define MamdaFundamentalListenerH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaMsgListener.h>
#include <mamda/MamdaFundamentals.h>

namespace Wombat
{

    class MamdaFundamentalHandler;

    /**
     * MamdaFundamentalListener is a class that specializes in handling
     * fundamental equity pricing/analysis attributes, indicators and
     * ratios.  Developers provide their own implementation of the
     * MamdaFundamentalHandler interface and will be delivered
     * notifications for updates in the fundamental data.  An obvious
     * application for this MAMDA class is any kind of pricing analysis
     * application.
     *
     * Note: The MamdaFundamentalListener class caches equity
     * pricing/analysis attributes, indicators and ratios. Among other
     * reasons, caching of these fields makes it possible to provide
     * complete fundamental callbacks, even when the publisher (e.g., feed
     * handler) is only publishing deltas containing modified fields.
     *
     * MamdaFundamentalListener should initialize the MamdaFundamentalFields class 
     * prior to receiving the first message by calling
     * MamdaFundamentalFields::setDictionary() with a valid dictionary object
     * which contains Fundamental related fields.
     */

    class MAMDAExpDLL MamdaFundamentalListener 
        : public MamdaMsgListener
        , public MamdaFundamentals
    {
    public:
        MamdaFundamentalListener ();
        virtual ~MamdaFundamentalListener ();

        void addHandler (MamdaFundamentalHandler* handler);

        // Inherited from  MamdaBasicRecap
        const char*          getSymbol()       const;
        const char*          getPartId()       const;
        const MamaDateTime&  getSrcTime()      const;
        const MamaDateTime&  getActivityTime() const;
        const MamaDateTime&  getLineTime()     const;
        const MamaDateTime&  getSendTime()     const;
        
        MamdaFieldState     getSymbolFieldState()       const;
        MamdaFieldState     getPartIdFieldState()       const;
        MamdaFieldState     getSrcTimeFieldState()      const;
        MamdaFieldState     getActivityTimeFieldState() const;
        MamdaFieldState     getLineTimeFieldState()     const;
        MamdaFieldState     getSendTimeFieldState()     const;


        // Inherited from MamdaFundamentals
        const char* getCorporateActionType()  const;
        double      getDividendPrice()        const;
        const char* getDividendFrequency()    const;
        const char* getDividendExDate()       const;
        const char* getDividendPayDate()      const;  
        const char* getDividendRecordDate()   const;  
        const char* getDividendCurrency()     const;  
        long        getSharesOut()            const;
        long        getSharesFloat()          const;
        long        getSharesAuthorized()     const;
        double      getEarningsPerShare()     const;
        double      getVolatility()           const;
        double      getPriceEarningsRatio()   const;
        double      getYield()                const;
        const char* getMarketSegmentNative()  const;
        const char* getMarketSectorNative()   const;
        const char* getMarketSegment()        const;
        const char* getMarketSector()         const;
        double      getHistoricalVolatility() const;  
        double      getRiskFreeRate()         const;

        MamdaFieldState     getCorporateActionTypeFieldState()  const;
        MamdaFieldState     getDividendPriceFieldState()        const;
        MamdaFieldState     getDividendFrequencyFieldState()    const;
        MamdaFieldState     getDividendExDateFieldState()       const;
        MamdaFieldState     getDividendPayDateFieldState()      const;  
        MamdaFieldState     getDividendRecordDateFieldState()   const;  
        MamdaFieldState     getDividendCurrencyFieldState()     const;  
        MamdaFieldState     getSharesOutFieldState()            const;
        MamdaFieldState     getSharesFloatFieldState()          const;
        MamdaFieldState     getSharesAuthorizedFieldState()     const;
        MamdaFieldState     getEarningsPerShareFieldState()     const;
        MamdaFieldState     getVolatilityFieldState()           const;
        MamdaFieldState     getPriceEarningsRatioFieldState()   const;
        MamdaFieldState     getYieldFieldState()                const;
        MamdaFieldState     getMarketSegmentNativeFieldState()  const;
        MamdaFieldState     getMarketSectorNativeFieldState()   const;
        MamdaFieldState     getMarketSegmentFieldState()        const;
        MamdaFieldState     getMarketSectorFieldState()         const;
        MamdaFieldState     getHistoricalVolatilityFieldState() const;  
        MamdaFieldState     getRiskFreeRateFieldState()         const;
        /**
         * Implementation of MamdaListener interface.\n
         * \throw <MamaStatus> {NYSE Technologies default implementation.}
         */
        virtual void onMsg (MamdaSubscription*  subscription,
                            const MamaMsg&      msg,
                            short               msgType);

        struct MamdaFundamentalListenerImpl;
    private:
        MamdaFundamentalListenerImpl& mImpl;

    };

} // namespace

#endif // MamdaFundamentalListenerH

