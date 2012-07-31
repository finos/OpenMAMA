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

#ifndef MamdaFundamentalsH
#define MamdaFundamentalsH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaBasicRecap.h>
#include <mamda/MamdaFieldState.h>

namespace Wombat
{

    /**
     * MamdaFundamentals is an interface that provides access to the 
     * fundamental equity pricing/analysis attributes, indicators and ratios.  
     */

    class MAMDAExpDLL MamdaFundamentals : public MamdaBasicRecap
    {
    public:
        virtual const char*       getCorporateActionType()            const = 0;
        virtual double            getDividendPrice()                  const = 0;
        virtual const char*       getDividendFrequency()              const = 0;
        virtual const char*       getDividendExDate()                 const = 0;
        virtual const char*       getDividendPayDate()                const = 0;  
        virtual const char*       getDividendRecordDate()             const = 0;  
        virtual const char*       getDividendCurrency()               const = 0;  
        virtual long              getSharesOut()                      const = 0;
        virtual long              getSharesFloat()                    const = 0;
        virtual long              getSharesAuthorized()               const = 0;
        virtual double            getEarningsPerShare()               const = 0;
        virtual double            getVolatility()                     const = 0;
        virtual double            getPriceEarningsRatio()             const = 0;
        virtual double            getYield()                          const = 0;
        virtual const char*       getMarketSegmentNative()            const = 0;
        virtual const char*       getMarketSectorNative()             const = 0;
        virtual const char*       getMarketSegment()                  const = 0;
        virtual const char*       getMarketSector()                   const = 0;
        virtual double            getHistoricalVolatility()           const = 0;  
        virtual double            getRiskFreeRate()                   const = 0;

        virtual MamdaFieldState   getCorporateActionTypeFieldState()  const = 0;
        virtual MamdaFieldState   getDividendPriceFieldState()        const = 0;
        virtual MamdaFieldState   getDividendFrequencyFieldState()    const = 0;
        virtual MamdaFieldState   getDividendExDateFieldState()       const = 0;
        virtual MamdaFieldState   getDividendPayDateFieldState()      const = 0;  
        virtual MamdaFieldState   getDividendRecordDateFieldState()   const = 0;  
        virtual MamdaFieldState   getDividendCurrencyFieldState()     const = 0;  
        virtual MamdaFieldState   getSharesOutFieldState()            const = 0;
        virtual MamdaFieldState   getSharesFloatFieldState()          const = 0;
        virtual MamdaFieldState   getSharesAuthorizedFieldState()     const = 0;
        virtual MamdaFieldState   getEarningsPerShareFieldState()     const = 0;
        virtual MamdaFieldState   getVolatilityFieldState()           const = 0;
        virtual MamdaFieldState   getPriceEarningsRatioFieldState()   const = 0;
        virtual MamdaFieldState   getYieldFieldState()                const = 0;
        virtual MamdaFieldState   getMarketSegmentNativeFieldState()  const = 0;
        virtual MamdaFieldState   getMarketSectorNativeFieldState()   const = 0;
        virtual MamdaFieldState   getMarketSegmentFieldState()        const = 0;
        virtual MamdaFieldState   getMarketSectorFieldState()         const = 0;
        virtual MamdaFieldState   getHistoricalVolatilityFieldState() const = 0;  
        virtual MamdaFieldState   getRiskFreeRateFieldState()         const = 0;

        virtual ~MamdaFundamentals() {};
    };

} // namespace

#endif // MamdaFundamentalsH
