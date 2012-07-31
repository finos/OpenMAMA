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

#ifndef MamdaUnitTestNamespacesH
#define MamdaUnitTestNamespacesH


#include <mamda/MamdaFieldState.h>
#include <mama/msgtype.h>

typedef struct ControlFields
{
    const char* mySymbol; 
    double      myPrice;
    double      mySize;
    char        myAction;
    char        mySide;
    float       myNumEntries;
    const char* myTime; 

    const char* mySymbolEntryLevel;
    long        myActNumEntriesEntryLevel;
    char        myEntryActionEntryLevel;
    const char* myEntryIdEntryLevel;
    long        myEntrySizeEntryLevel;
    
    const char* myGapSymbol;
    uint        myBeginGapSeqNum;
    uint        myEndGapSeqNum;
    
    const char* myBeginBook;
    
    const char* myClear;
    
    float       myPriceLevelNumLevels;
    bool        myHasMarkerOrders;
    const char* mySymbolEnd;
} ControlFields;

typedef struct TestFields
{
    //TestFields for onBookAtomicLevelRecap.
    const char* mySymbolLevelRecap;
    double      myPriceLevelRecap;
    double      mySizeLevelRecap;
    char        myActionLevelRecap;
    char        mySideLevelRecap;
    float       myNumEntriesLevelRecap;
    const char* myTimeLevelRecap;
    
    //TestFields for onBookAtomicLevelEntryRecap.
    const char* mySymbolEntryLevelRecap;
    long        myActNumEntriesEntryLevelRecap;
    char        myEntryActionEntryLevelRecap;
    const char* myEntryIdEntryLevelRecap;
    long        myEntrySizeEntryLevelRecap;

    //TestFields for onBookAtomicLevelDelta.
    const char* mySymbolLevelDelta;
    double      myPriceLevelDelta;
    double      mySizeLevelDelta;
    char        myActionLevelDelta;
    char        mySideLevelDelta;
    float       myNumEntriesLevelDelta;
    const char* myTimeLevelDelta;
    
    //TestFields for onBookAtomicLevelEntryDelta.
    const char* mySymbolEntryLevelDelta;
    long        myActNumEntriesEntryLevelDelta;
    char        myEntryActionEntryLevelDelta;
    const char* myEntryIdEntryLevelDelta;
    long        myEntrySizeEntryLevelDelta;
    
    //TestFields for onBookAtomicGap.
    uint        myCallBackBeginGapSeqNum;
    uint        myCallBackEndGapSeqNum;
    const char* mySymbolGap;
    
    //TestField for onBookAtomicBegin.
    const char* myCallBackBeginBook;
    
    //TestField for onBookAtomicClear.
    const char* myCallBackClear;
    
    //TestFields for onBookAtomicEndBook.
    float       myPriceLevelNumLevelsEndBook;
    bool        myHasMarkerOrdersEndBook;
    const char* mySymbolEndBook;
    
} TestFields;

typedef struct TradeControlCache
{
    //Wombat::MamdaFieldStates have been changed to uint.
    const char*          mySymbol;                  uint  mySymbolFieldState;  
    const char*          myPartId;                  uint  myPartIdFieldState;  
    const char*          mySrcTime;                 uint  mySrcTimeFieldState; 
    const char*          myActTime;                 uint  myActTimeFieldState; 
    const char*          myLineTime;                uint  myLineTimeFieldState;
    //Wombat::MamaDateTime         mySendTime;        uint  mySendTimeFieldState;
    const char*          myPubId;                   uint  myPubIdFieldState;   
    bool                 myIsIrregular;             uint  myIsIrregularFieldState; 
    //bool                 mWasIrregular;             uint  mWasIrregularFieldState;
    double               myLastPrice;               uint  myLastPriceFieldState; 
    mama_quantity_t      myLastVolume;              uint  myLastVolumeFieldState;
    const char*          myLastPartId;              uint  myLastPartIdFieldState;
    const char*          myLastTime;                uint  myLastTimeFieldState;   
    const char*          myTradeDate;               uint  myTradeDateFieldState;  
    //MamaPrice            myIrregPrice;              uint  myIrregPriceFieldState; 
    mama_quantity_t      myIrregVolume;             uint  myIrregVolumeFieldState; 
    const char*          myIrregPartId;             uint  myIrregPartIdFieldState; 
    const char*          myIrregTime;               uint  myIrregTimeFieldState;   
    mama_quantity_t      myAccVolume;               uint  myAccVolumeFieldState;   
    mama_quantity_t      myOffExAccVolume;          uint  myOffExAccVolumeFieldState;
    mama_quantity_t      myOnExAccVolume;           uint  myOnExAccVolumeFieldState; 
    uint                 myTradeDirection;          uint  myTradeDirectionFieldState;
    double               myNetChange;               uint  myNetChangeFieldState;     
    double               myPctChange;               uint  myPctChangeFieldState;     
    double               myOpenPrice;               uint  myOpenPriceFieldState;     
    double               myHighPrice;               uint  myHighPriceFieldState;     
    double               myLowPrice;                uint  myLowPriceFieldState;      
    double               myClosePrice;              uint  myClosePriceFieldState;    
    double               myPrevClosePrice;          uint  myPrevClosePriceFieldState;
    const char*          myPrevCloseDate;           uint  myPrevCloseDateFieldState; 
    double               myAdjPrevClose;            uint  myAdjPrevCloseFieldState;  
    mama_u32_t           myTradeCount;              uint  myTradeCountFieldState;    
    mama_quantity_t      myBlockVolume;             uint  myBlockVolumeFieldState;   
    mama_u32_t           myBlockCount;              uint  myBlockCountFieldState;
    double               myVwap;                    uint  myVwapFieldState;      
    double               myOffExVwap;               uint  myOffExVwapFieldState; 
    double               myOnExVwap;                uint  myOnExVwapFieldState;  
    double               myTotalValue;              uint  myTotalValueFieldState;
    double               myOffExTotalValue;         uint  myOffExTotalValueFieldState;
    double               myOnExTotalValue;          uint  myOnExTotalValueFieldState; 
    double               myStdDev;                  uint  myStdDevFieldState;  
    mama_u64_t           myOrderId;                 uint  myOrderIdFieldState; 
    //bool                  myLastGenericMsgWasTrade;
    //bool                  myGotTradeTime; 
    //bool                  myGotTradePrice;
    //bool                  myGotTradeSize; 
    //bool                  myGotTradeCount;
    double               myStdDevSum;               uint  myStdDevSumFieldState;      
    double               myStdDevSumSquares;        uint  myStdDevSumSquaresFieldState;
    const char*          myTradeUnits;              uint  myTradeUnitsFieldState;  
    mama_seqnum_t        myLastSeqNum;              uint  myLastSeqNumFieldState;  
    mama_seqnum_t        myHighSeqNum;              uint  myHighSeqNumFieldState;  
    mama_seqnum_t        myLowSeqNum;               uint  myLowSeqNumFieldState;   
    mama_seqnum_t        myTotalVolumeSeqNum;       uint  myTotalVolumeSeqNumFieldState; 
    const char*          myCurrencyCode;            uint  myCurrencyCodeFieldState; 
    const char*          myUniqueId;                uint  myUniqueIdFieldState;     
    const char*          myTradeAction;             uint  myTradeActionFieldState;  
    //bool                 myIsSnapshot;
    double               mySettlePrice;             uint  mySettlePriceFieldState;  
    const char*          mySettleDate;              uint  mySettleDateFieldState;   
    double               myOffExchangeTradePrice;   uint  myOffExchangeTradePriceFieldState; 
    double               myOnExchangeTradePrice;    uint  myOnExchangeTradePriceFieldState;  

    // The following fields are used for caching the last reported
    // trade, which might have been out of order.  The reason for
    // cahcing these values is to allow a configuration that passes
    // the minimum amount of data  (unchanged fields not sent).
    mama_seqnum_t         myEventSeqNum;            uint  myEventSeqNumFieldState;
    const char*           myEventTime;              uint  myEventTimeFieldState;  
    //double                myTradePrice;              uint  myTradePriceFieldState; 
    //mama_quantity_t       myTradeVolume;             uint  myTradeVolumeFieldState;
    //const char*                myTradePartId;             uint  myTradePartIdFieldState;
    const char*           myTradeQualStr;           uint  myTradeQualStrFieldState; 
    const char*           myTradeQualNativeStr;     uint  myTradeQualNativeStrFieldState;
    mama_u32_t            mySellersSaleDays;        uint  mySellersSaleDaysFieldState; 
    char                 myStopStockInd;            uint  myStopStockIndFieldState;
    //mama_u32_t          myTmpTradeCount;             uint  myTmpTradeCountFieldState;
    uint                  myTradeExecVenue;         uint  myTradeExecVenueFieldState;

    // Additional fields for cancels/error/corrections:
    //bool                  myIsCancel;                  //bool  myIsCancelFieldState; 
    mama_seqnum_t         myOrigSeqNum;             uint  myOrigSeqNumFieldState;
    double                myOrigPrice;              uint  myOrigPriceFieldState; 
    mama_quantity_t       myOrigVolume;             uint  myOrigVolumeFieldState;
    const char*           myOrigPartId;             uint  myOrigPartIdFieldState;
    const char*           myOrigQualStr;            uint  myOrigQualStrFieldState;
    const char*           myOrigQualNativeStr;      uint  myOrigQualNativeStrFieldState;
    mama_u32_t            myOrigSellersSaleDays;    uint  myOrigSellersSaleDaysFieldState;
    char                 myOrigStopStockInd;        uint  myOrigStopStockIndFieldState;
    double                myCorrPrice;              uint  myCorrPriceFieldState; 
    mama_quantity_t       myCorrVolume;             uint  myCorrVolumeFieldState; 
    const char*           myCorrPartId;             uint  myCorrPartIdFieldState; 
    const char*           myCorrQualStr;            uint  myCorrQualStrFieldState;
    const char*           myCorrQualNativeStr;      uint  myCorrQualNativeStrFieldState;
    mama_u32_t            myCorrSellersSaleDays;    uint  myCorrSellersSaleDaysFieldState;
    char                 myCorrStopStockInd;        uint  myCorrStopStockIndFieldState;
    //Wombat::MamaDateTime          myCancelTime;                //Wombat::MamdaFieldState  myCancelTimeFieldState;

    // Additional fields for gaps:
    //mama_seqnum_t         myGapBegin;                  Wombat::MamdaFieldState  myGapBeginFieldState;
    //mama_seqnum_t         myGapEnd;                    Wombat::MamdaFieldState  myGapEndFieldState;

    // Additional fields for closing summaries:
    //bool                  myIsIndicative;            
} TradeControlCache;

typedef struct TradeTestCache
{
    // The following fields are used for caching the offical last
    // price and related fields.  These fields can be used by
    // applications for reference and will be passed for recaps.
    const char*           myTempSymbol;                    uint  myTempSymbolFieldState;  
    const char*           myTempPartId;                    uint  myTempPartIdFieldState;  
    const char*           myTempSrcTime;                   uint  myTempSrcTimeFieldState; 
    const char*           myTempActTime;                   uint  myTempActTimeFieldState; 
    const char*           myTempLineTime;                  uint  myTempLineTimeFieldState;
    //const char*           myTempSendTime;                  uint  myTempSendTimeFieldState;
    const char*           myTempPubId;                     uint  myTempPubIdFieldState;   
    bool                  myTempIsIrregular;               uint  myTempIsIrregularFieldState; 
    //bool                  myTempWasIrregular;              uint  myTempWasIrregularFieldState;
    double                myTempLastPrice;                 uint  myTempLastPriceFieldState; 
    mama_quantity_t       myTempLastVolume;                uint  myTempLastVolumeFieldState;
    const char*           myTempLastPartId;                uint  myTempLastPartIdFieldState;
    const char*           myTempLastTime;                  uint  myTempLastTimeFieldState;   
    const char*           myTempTradeDate;                 uint  myTempTradeDateFieldState;  
    //double                myTempIrregPrice;                uint  myTempIrregPriceFieldState; 
    mama_quantity_t       myTempIrregVolume;               uint  myTempIrregVolumeFieldState; 
    const char*           myTempIrregPartId;               uint  myTempIrregPartIdFieldState; 
    const char*           myTempIrregTime;                 uint  myTempIrregTimeFieldState;   
    mama_quantity_t       myTempAccVolume;                 uint  myTempAccVolumeFieldState;   
    mama_quantity_t       myTempOffExAccVolume;            uint  myTempOffExAccVolumeFieldState;
    mama_quantity_t       myTempOnExAccVolume;             uint  myTempOnExAccVolumeFieldState; 
    uint                  myTempTradeDirection;            uint  myTempTradeDirectionFieldState;
    double                myTempNetChange;                 uint  myTempNetChangeFieldState;     
    double                myTempPctChange;                 uint  myTempPctChangeFieldState;     
    double                myTempOpenPrice;                 uint  myTempOpenPriceFieldState;     
    double                myTempHighPrice;                 uint  myTempHighPriceFieldState;     
    double                myTempLowPrice;                  uint  myTempLowPriceFieldState;      
    double                myTempClosePrice;                uint  myTempClosePriceFieldState;    
    double                myTempPrevClosePrice;            uint  myTempPrevClosePriceFieldState;
    const char*           myTempPrevCloseDate;             uint  myTempPrevCloseDateFieldState; 
    double                myTempAdjPrevClose;              uint  myTempAdjPrevCloseFieldState;  
    mama_u32_t            myTempTradeCount;                uint  myTempTradeCountFieldState;    
    mama_quantity_t       myTempBlockVolume;               uint  myTempBlockVolumeFieldState;   
    mama_u32_t            myTempBlockCount;                uint  myTempBlockCountFieldState;
    double                myTempVwap;                      uint  myTempVwapFieldState;      
    double                myTempOffExVwap;                 uint  myTempOffExVwapFieldState; 
    double                myTempOnExVwap;                  uint  myTempOnExVwapFieldState;  
    double                myTempTotalValue;                uint  myTempTotalValueFieldState;
    double                myTempOffExTotalValue;           uint  myTempOffExTotalValueFieldState;
    double                myTempOnExTotalValue;            uint  myTempOnExTotalValueFieldState; 
    double                myTempStdDev;                    uint  myTempStdDevFieldState;  
    mama_u64_t            myTempOrderId;                   uint  myTempOrderIdFieldState; 
    //bool                myTempLastGenericMsgWasTrade;
    //bool                myTempGotTradeTime; 
    //bool                myTempGotTradePrice;
    //bool                myTempGotTradeSize; 
    //bool                myTempGotTradeCount;
    double                myTempStdDevSum;                 uint  myTempStdDevSumFieldState;      
    double                myTempStdDevSumSquares;          uint  myTempStdDevSumSquaresFieldState;
    const char*           myTempTradeUnits;                uint  myTempTradeUnitsFieldState;  
    mama_seqnum_t         myTempLastSeqNum;                uint  myTempLastSeqNumFieldState;  
    mama_seqnum_t         myTempHighSeqNum;                uint  myTempHighSeqNumFieldState;  
    mama_seqnum_t         myTempLowSeqNum;                 uint  myTempLowSeqNumFieldState;   
    mama_seqnum_t         myTempTotalVolumeSeqNum;         uint  myTempTotalVolumeSeqNumFieldState; 
    const char*           myTempCurrencyCode;              uint  myTempCurrencyCodeFieldState; 
    const char*           myTempUniqueId;                  uint  myTempUniqueIdFieldState;     
    const char*           myTempTradeAction;               uint  myTempTradeActionFieldState;  
    //bool                myTempIsSnapshot;
    double                myTempSettlePrice;               uint  myTempSettlePriceFieldState;  
    const char*           myTempSettleDate;                uint  myTempSettleDateFieldState;   
    double                myTempOffExchangeTradePrice;     uint  myTempOffExchangeTradePriceFieldState; 
    double                myTempOnExchangeTradePrice;      uint  myTempOnExchangeTradePriceFieldState;  

    // The following fields are used for caching the last reported
    // trade, which might have been out of order.  The reason for
    // cahcing these values is to allow a configuration that passes
    // the minimum amount of data  (unchanged fields not sent).
    mama_seqnum_t         myTempEventSeqNum;               uint  myTempEventSeqNumFieldState;
    const char*           myTempEventTime;                 uint  myTempEventTimeFieldState;  
    //double              myTempTradePrice;                uint  myTempTradePriceFieldState; 
    //mama_quantity_t     myTempTradeVolume;               uint  myTempTradeVolumeFieldState;
    //const char*         myTempTradePartId;               uint  myTempTradePartIdFieldState;
    const char*           myTempTradeQualStr;              uint  myTempTradeQualStrFieldState; 
    const char*           myTempTradeQualNativeStr;        uint  myTempTradeQualNativeStrFieldState;
    mama_u32_t            myTempSellersSaleDays;           uint  myTempSellersSaleDaysFieldState; 
    char                 myTempStopStockInd;               uint  myTempStopStockIndFieldState;
    //mama_u32_t          myTempTmpTradeCount;             uint  myTempTmpTradeCountFieldState;
    uint                  myTempTradeExecVenue;            uint  myTempTradeExecVenueFieldState;

    // Additional fields for cancels/error/corrections:
    //bool                  myTempIsCancel;                  //uint  myTempIsCancelFieldState; 
    mama_seqnum_t         myTempOrigSeqNum;                uint  myTempOrigSeqNumFieldState;
    double                myTempOrigPrice;                 uint  myTempOrigPriceFieldState; 
    mama_quantity_t       myTempOrigVolume;                uint  myTempOrigVolumeFieldState;
    const char*           myTempOrigPartId;                uint  myTempOrigPartIdFieldState;
    const char*           myTempOrigQualStr;               uint  myTempOrigQualStrFieldState;
    const char*           myTempOrigQualNativeStr;         uint  myTempOrigQualNativeStrFieldState;
    mama_u32_t            myTempOrigSellersSaleDays;       uint  myTempOrigSellersSaleDaysFieldState;
    char                 myTempOrigStopStockInd;           uint  myTempOrigStopStockIndFieldState; 
    double                myTempCorrPrice;                 uint  myTempCorrPriceFieldState; 
    mama_quantity_t       myTempCorrVolume;                uint  myTempCorrVolumeFieldState; 
    const char*           myTempCorrPartId;                uint  myTempCorrPartIdFieldState; 
    const char*           myTempCorrQualStr;               uint  myTempCorrQualStrFieldState;
    const char*           myTempCorrQualNativeStr;         uint  myTempCorrQualNativeStrFieldState;
    mama_u32_t            myTempCorrSellersSaleDays;       uint  myTempCorrSellersSaleDaysFieldState;
    char                 myTempCorrStopStockInd;           uint  myTempCorrStopStockIndFieldState;
    //const char*         myTempCancelTime;                //uint  myTempCancelTimeFieldState;

    // Additional fields for gaps:
    //mama_seqnum_t         myTempGapBegin;                  uint  myTempGapBeginFieldState;
    //mama_seqnum_t         myTempGapEnd;                    uint  myTempGapEndFieldState;

    // Additional fields for closing summaries:
    //bool                  myTempIsIndicative;
} TradeTestCache;

typedef struct TradeControlFields
{
    const char*      mySymbol;      
    mama_u32_t       myTradeCount;
    mama_quantity_t  myTradeVolume;
    const char*      myTradePrice;
    mama_seqnum_t    myTradeEventSeqNum;
    const char*      myTradeEventTime;
    const char*      myTradeActivityTime;
    const char*      myTradePartId;
    const char*      myTradeQual;
    const char*      myTradeQualNative;
    bool             myTradeIsIrregular;     
    const char*      myTradeMsgQualStr;
    uint             myTradeMsgQual;
    const char*      myTradeMsgQualPossDupStr;
    uint             myTradeMsgQualPossDup;
    mama_quantity_t  myAccVolume;
} TradeControlFields;

typedef struct TradeTestFields
{        
    const char*      myTempSymbol;   
    mama_u32_t       myTempTradeCount;
    mama_quantity_t  myTempTradeVolume;
    const char*      myTempTradePrice;
    mama_seqnum_t    myTempTradeEventSeqNum;
    const char*      myTempTradeEventTime;
    const char*      myTempTradeActivityTime;
    const char*      myTempTradePartId;
    const char*      myTempTradeQual;
    const char*      myTempTradeQualNative;
    bool             myTempTradeIsIrregular;   
    const char*      myTempTradeMsgQualStr;
    uint             myTempTradeMsgQual;        
    mama_quantity_t  myTempAccVolume;
} TradeTestFields;

typedef struct QuoteTestCache
{
    // The following fields are used for caching the offical quote and
    // related fields.  These fields can be used by applications for
    // reference and will be passed for recaps.
    const char*     myTempSymbol;                            uint   myTempSymbolFieldState;   
    const char*     myTempPartId;                            uint   myTempPartIdFieldState;   
    const char*     myTempSrcTime;                           uint   myTempSrcTimeFieldState;  
    const char*     myTempActTime;                           uint   myTempActTimeFieldState;  
    const char*     myTempLineTime;                          uint   myTempLineTimeFieldState; 
    const char*     myTempSendTime;                          uint   myTempSendTimeFieldState; 
    const char*     myTempPubId;                             uint   myTempPubIdFieldState;
    double          myTempBidPrice;                          uint   myTempBidPriceFieldState; 
    mama_quantity_t myTempBidSize;                           uint   myTempBidSizeFieldState;
    mama_quantity_t myTempBidDepth;                          uint   myTempBidDepthFieldState;
    const char*     myTempBidPartId;                         uint   myTempBidPartIdFieldState;
    double          myTempBidClosePrice;                     uint   myTempBidClosePriceFieldState; 
    const char*     myTempBidCloseDate;                      uint   myTempBidCloseDateFieldState;

    double          myTempBidPrevClosePrice;                 uint   myTempBidPrevClosePriceFieldState;
    const char*     myTempBidPrevCloseDate;                  uint   myTempBidPrevCloseDateFieldState; 
    double          myTempBidHigh;                           uint   myTempBidHighFieldState;          
    double          myTempBidLow;                            uint   myTempBidLowFieldState;           
    double          myTempAskPrice;                          uint   myTempAskPriceFieldState;         
    mama_quantity_t myTempAskSize;                           uint   myTempAskSizeFieldState;          
    mama_quantity_t myTempAskDepth;                          uint   myTempAskDepthFieldState;         
    const char*     myTempAskPartId;                         uint   myTempAskPartIdFieldState;        
    double          myTempAskClosePrice;                     uint   myTempAskClosePriceFieldState;     
    const char*     myTempAskCloseDate;                      uint   myTempAskCloseDateFieldState;     
    double          myTempAskPrevClosePrice;                 uint   myTempAskPrevClosePriceFieldState;
    const char*     myTempAskPrevCloseDate;                  uint   myTempAskPrevCloseDateFieldState; 
    double          myTempAskHigh;                           uint   myTempAskHighFieldState;          
    double          myTempAskLow;                            uint   myTempAskLowFieldState;           
    //double          myTempMidPrice;                          uint   myTempMidPriceFieldState;         
    mama_seqnum_t   myTempEventSeqNum;                       uint   myTempEventSeqNumFieldState;      
    const char*     myTempEventTime;                         uint   myTempEventTimeFieldState;        
    const char*     myTempQuoteDate;                         uint   myTempQuoteDateFieldState;        
    const char*     myTempQuoteQualStr;                      uint   myTempQuoteQualStrFieldState;     
    const char*     myTempQuoteQualNative;                   uint   myTempQuoteQualNativeFieldState;  

    const char*     myTempAskTime;                           uint   myTempAskTimeFieldState;          
    const char*     myTempBidTime;                           uint   myTempBidTimeFieldState;          
    const char*     myTempAskIndicator;                      uint   myTempAskIndicatorFieldState;     
    const char*     myTempBidIndicator;                      uint   myTempBidIndicatorFieldState;     
    //mama_u32_t      myTempAskUpdateCount;                    uint   myTempAskUpdateCountFieldState;   
    //mama_u32_t      myTempBidUpdateCount;                    uint   myTempBidUpdateCountFieldState;   
    double          myTempAskYield;                          uint   myTempAskYieldFieldState;         
    double          myTempBidYield;                          uint   myTempBidYieldFieldState;         

    mama_u32_t      myTempQuoteCount;                        uint   myTempQuoteCountFieldState;
    //mama_seqnum_t   myTempGapBegin;                          uint   myTempGapBeginFieldState;         
    //mama_seqnum_t   myTempGapEnd;                            uint   myTempGapEndFieldState;
    const char*     myTempBidSizesList;                      uint   myTempBidSizesListFieldState;
    const char*     myTempAskSizesList;                      uint   myTempAskSizesListFieldState;

    char            myTempShortSaleBidTick;                  uint   myTempShortSaleBidTickFieldState;
} QuoteTestCache;

typedef struct QuoteControlCache
{
    
    // The following fields are used for caching the offical quote and
    // related fields.  These fields can be used by applications for
    // reference and will be passed for recaps.
    const char*     mySymbol;              uint   mySymbolFieldState;   
    //const char*     myPartId;              uint   myPartIdFieldState;   
    const char*     mySrcTime;             uint   mySrcTimeFieldState;  
    const char*     myActTime;             uint   myActTimeFieldState;  
    const char*     myLineTime;            uint   myLineTimeFieldState; 
    //const char*     mySendTime;            uint   mySendTimeFieldState; 
    const char*     myPubId;               uint   myPubIdFieldState;
    double          myBidPrice;            uint   myBidPriceFieldState; 
    mama_quantity_t myBidSize;             uint   myBidSizeFieldState;
    mama_quantity_t myBidDepth;            uint   myBidDepthFieldState;
    const char*     myBidPartId;           uint   myBidPartIdFieldState;
    double          myBidClosePrice;       uint   myBidClosePriceFieldState; 
    const char*     myBidCloseDate;        uint   myBidCloseDateFieldState;
    double          myBidPrevClosePrice;   uint   myBidPrevClosePriceFieldState;
    const char*     myBidPrevCloseDate;    uint   myBidPrevCloseDateFieldState; 
    double          myBidHigh;             uint   myBidHighFieldState;          
    double          myBidLow;              uint   myBidLowFieldState;           
    double          myAskPrice;            uint   myAskPriceFieldState;         
    mama_quantity_t myAskSize;             uint   myAskSizeFieldState;          
    mama_quantity_t myAskDepth;            uint   myAskDepthFieldState;         
    const char*     myAskPartId;           uint   myAskPartIdFieldState;        
    double          myAskClosePrice;       uint   myAskClosePriceFieldState;     
    const char*     myAskCloseDate;        uint   myAskCloseDateFieldState;     
    double          myAskPrevClosePrice;   uint   myAskPrevClosePriceFieldState;
    const char*     myAskPrevCloseDate;    uint   myAskPrevCloseDateFieldState; 
    double          myAskHigh;             uint   myAskHighFieldState;          
    double          myAskLow;              uint   myAskLowFieldState;           
    //double          myMidPrice;            uint   myMidPriceFieldState;
    mama_seqnum_t   myEventSeqNum;         uint   myEventSeqNumFieldState;      
    const char*     myEventTime;           uint   myEventTimeFieldState;        
    const char*     myQuoteDate;           uint   myQuoteDateFieldState;        
    const char*     myQuoteQualStr;        uint   myQuoteQualStrFieldState;     
    const char*     myQuoteQualNative;     uint   myQuoteQualNativeFieldState;  

    const char*     myAskTime;             uint   myAskTimeFieldState;          
    const char*     myBidTime;             uint   myBidTimeFieldState;          
    const char*     myAskIndicator;        uint   myAskIndicatorFieldState;     
    const char*     myBidIndicator;        uint   myBidIndicatorFieldState;     
    //mama_u32_t      myAskUpdateCount;      uint   myAskUpdateCountFieldState;   
    //mama_u32_t      myBidUpdateCount;      uint   myBidUpdateCountFieldState;   
    double          myAskYield;            uint   myAskYieldFieldState;         
    double          myBidYield;            uint   myBidYieldFieldState;         

    //mama_u32_t      myQuoteCount;          uint   myQuoteCountFieldState;
    //mama_seqnum_t   myGapBegin;            uint   myGapBeginFieldState;         
    //mama_seqnum_t   myGapEnd;              uint   myGapEndFieldState;           
    const char*     myBidSizesList;        uint   myBidSizesListFieldState;
    const char*     myAskSizesList;        uint   myAskSizesListFieldState;
    char            myShortSaleBidTick;    uint   myShortSaleBidTickFieldState;//doesnt getupdated in quotelistener.
} QuoteControlCache;

typedef struct QuoteControlFields
{
    const char*     mySymbol;
    mama_u32_t      myQuoteCount;
    const char*     myBidPriceStr;
    const char*     myBidClosePriceStr;
    mama_quantity_t myBidSize;
    mama_quantity_t myAskSize;
    const char*     myAskPriceStr;
    const char*     myAskClosePriceStr;
    mama_seqnum_t   myEventSeqNum;
    const char*     myEventTimeStr;
    const char*     myQuoteQualStr;
    const char*     myMsgQualStr;
    uint            myMsgQual;
    const char*     myMsgQualPossDupStr;
    uint            myMsgQualPossDup;       
    mama_seqnum_t   myGapBeginSeqNum;
    mama_seqnum_t   myEndGapSeqNum;       
} QuoteControlFields;

typedef struct QuoteTestFields
{
    const char*     myTempSymbol;
    mama_u32_t      myTempQuoteCount;
    const char*     myTempBidPriceStr;
    const char*     myTempBidClosePriceStr;
    mama_quantity_t myTempBidSize;
    mama_quantity_t myTempAskSize;
    const char*     myTempAskPriceStr;
    const char*     myTempAskClosePriceStr;
    mama_seqnum_t   myTempEventSeqNum;
    const char*     myTempEventTimeStr;
    const char*     myTempQuoteQualStr;
    const char*     myTempMsgQualStr;
    uint            myTempMsgQual;
    mama_seqnum_t   myTempBeginGapSeqNum;
    mama_seqnum_t   myTempEndGapSeqNum;
} QuoteTestFields;

typedef struct OrderImbalanceControlCache 
{
    // The following fields are used for caching the offical order imbalance
    // and related fields.  These fields can be used by applications for
    // reference and will be passed for recaps.
    double                myHighIndicationPrice;    Wombat::MamdaFieldState     myHighIndicationPriceFieldState;
    double                myLowIndicationPrice;     Wombat::MamdaFieldState     myLowIndicationPriceFieldState;
    double                myIndicationPrice;        Wombat::MamdaFieldState     myIndicationPriceFieldState;
    int64_t               myBuyVolume;              Wombat::MamdaFieldState     myBuyVolumeFieldState;
    int64_t               mySellVolume;             Wombat::MamdaFieldState     mySellVolumeFieldState;
    int64_t               myMatchVolume;            Wombat::MamdaFieldState     myMatchVolumeFieldState;
    const char*           mySecurityStatusQual;     Wombat::MamdaFieldState     mySecurityStatusQualFieldState;
    double                myInsideMatchPrice;       Wombat::MamdaFieldState     myInsideMatchPriceFieldState;
    double                myFarClearingPrice;       Wombat::MamdaFieldState     myFarClearingPriceFieldState;
    double                myNearClearingPrice;      Wombat::MamdaFieldState     myNearClearingPriceFieldState;
    char                  myNoClearingPrice;        Wombat::MamdaFieldState     myNoClearingPriceFieldState;
    char                  myPriceVarInd;            Wombat::MamdaFieldState     myPriceVarIndFieldState;
    char                  myCrossType;              Wombat::MamdaFieldState     myCrossTypeFieldState;
    Wombat::MamaDateTime  myEventTime;              Wombat::MamdaFieldState     myEventTimeFieldState;
    mama_seqnum_t         myEventSeqNum;            Wombat::MamdaFieldState     myEventSeqNumFieldState;
    Wombat::MamaDateTime  mySrcTime;                Wombat::MamdaFieldState     mySrcTimeFieldState;
    Wombat::MamaDateTime  myActTime;                Wombat::MamdaFieldState     myActTimeFieldState;
    mama_i32_t            myMsgType;                Wombat::MamdaFieldState     myMsgTypeFieldState;
    const char*           myIssueSymbol;            Wombat::MamdaFieldState     myIssueSymbolFieldState;
    const char*           myPartId;                 Wombat::MamdaFieldState     myPartIdFieldState;
    mama_seqnum_t         mySeqNum;                 Wombat::MamdaFieldState     mySeqNumFieldState;
    const char*           mySecurityStatusOrig;     Wombat::MamdaFieldState     mySecurityStatusOrigFieldState;
    Wombat::MamaDateTime  mySecurityStatusTime;     Wombat::MamdaFieldState     mySecurityStatusTimeFieldState;
    Wombat::MamaDateTime  myAuctionTime;            Wombat::MamdaFieldState     myAuctionTimeFieldState;
    Wombat::MamaDateTime  myLineTime;               Wombat::MamdaFieldState     myLineTimeFieldState;
    Wombat::MamaDateTime  mySendTime;               Wombat::MamdaFieldState     mySendTimeFieldState;
    const char*           mySymbol;                 Wombat::MamdaFieldState     mySymbolFieldState;  
    bool                  isOrderImbalance;
} OrderImbalanceCacheControlCache;
    
typedef struct OrderImbalanceTestCache 
{
    // The following fields are used for caching the offical order imbalance
    // and related fields.  These fields can be used by applications for
    // reference and will be passed for recaps.
    double                myTestHighIndicationPrice;      Wombat::MamdaFieldState     myTestHighIndicationPriceFieldState;
    double                myTestLowIndicationPrice;       Wombat::MamdaFieldState     myTestLowIndicationPriceFieldState;
    double                myTestIndicationPrice;          Wombat::MamdaFieldState     myTestIndicationPriceFieldState;
    int64_t               myTestBuyVolume;                Wombat::MamdaFieldState     myTestBuyVolumeFieldState;
    int64_t               myTestSellVolume;               Wombat::MamdaFieldState     myTestSellVolumeFieldState;
    int64_t               myTestMatchVolume;              Wombat::MamdaFieldState     myTestMatchVolumeFieldState;
    const char*           myTestSecurityStatusQual;       Wombat::MamdaFieldState     myTestSecurityStatusQualFieldState;
    double                myTestInsideMatchPrice;         Wombat::MamdaFieldState     myTestInsideMatchPriceFieldState;
    double                myTestFarClearingPrice;         Wombat::MamdaFieldState     myTestFarClearingPriceFieldState;
    double                myTestNearClearingPrice;        Wombat::MamdaFieldState     myTestNearClearingPriceFieldState;
    char                  myTestNoClearingPrice;          Wombat::MamdaFieldState     myTestNoClearingPriceFieldState;
    char                  myTestPriceVarInd;              Wombat::MamdaFieldState     myTestPriceVarIndFieldState;
    char                  myTestCrossType;                Wombat::MamdaFieldState     myTestCrossTypeFieldState;
    Wombat::MamaDateTime  myTestEventTime;                Wombat::MamdaFieldState     myTestEventTimeFieldState;
    mama_seqnum_t         myTestEventSeqNum;              Wombat::MamdaFieldState     myTestEventSeqNumFieldState;
    Wombat::MamaDateTime  myTestSrcTime;                  Wombat::MamdaFieldState     myTestSrcTimeFieldState;
    Wombat::MamaDateTime  myTestActTime;                  Wombat::MamdaFieldState     myTestActTimeFieldState;
    mama_i32_t            myTestMsgType;                  Wombat::MamdaFieldState     myTestMsgTypeFieldState;
    const char*           myTestIssueSymbol;              Wombat::MamdaFieldState     myTestIssueSymbolFieldState;
    const char*           myTestPartId;                   Wombat::MamdaFieldState     myTestPartIdFieldState;
    mama_seqnum_t         myTestSeqNum;                   Wombat::MamdaFieldState     myTestSeqNumFieldState;
    const char*           myTestSecurityStatusOrig;       Wombat::MamdaFieldState     myTestSecurityStatusOrigFieldState;
    Wombat::MamaDateTime  myTestSecurityStatusTime;       Wombat::MamdaFieldState     myTestSecurityStatusTimeFieldState;
    Wombat::MamaDateTime  myTestAuctionTime;              Wombat::MamdaFieldState     myTestAuctionTimeFieldState;
    Wombat::MamaDateTime  myTestLineTime;                 Wombat::MamdaFieldState     myTestLineTimeFieldState;
    Wombat::MamaDateTime  myTestSendTime;                 Wombat::MamdaFieldState     myTestSendTimeFieldState;
    const char*           myTestSymbol;                   Wombat::MamdaFieldState     myTestSymbolFieldState;  
    bool                  isOrderImbalance;
} OrderImbalanceCacheTestCache;

#endif // MamdaUnitTestNamespacesH


