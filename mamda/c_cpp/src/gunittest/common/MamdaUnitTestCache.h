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
    unsigned int        myBeginGapSeqNum;
    unsigned int        myEndGapSeqNum;
    
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
    unsigned int        myCallBackBeginGapSeqNum;
    unsigned int        myCallBackEndGapSeqNum;
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
    //Wombat::MamdaFieldStates have been changed to unsigned int.
    const char*          mySymbol;                  unsigned int  mySymbolFieldState;  
    const char*          myPartId;                  unsigned int  myPartIdFieldState;  
    const char*          mySrcTime;                 unsigned int  mySrcTimeFieldState; 
    const char*          myActTime;                 unsigned int  myActTimeFieldState; 
    const char*          myLineTime;                unsigned int  myLineTimeFieldState;
    //Wombat::MamaDateTime         mySendTime;        unsigned int  mySendTimeFieldState;
    const char*          myPubId;                   unsigned int  myPubIdFieldState;   
    bool                 myIsIrregular;             unsigned int  myIsIrregularFieldState; 
    //bool                 mWasIrregular;             unsigned int  mWasIrregularFieldState;
    double               myLastPrice;               unsigned int  myLastPriceFieldState; 
    mama_quantity_t      myLastVolume;              unsigned int  myLastVolumeFieldState;
    const char*          myLastPartId;              unsigned int  myLastPartIdFieldState;
    const char*          myLastTime;                unsigned int  myLastTimeFieldState;   
    const char*          myTradeDate;               unsigned int  myTradeDateFieldState;  
    //MamaPrice            myIrregPrice;              unsigned int  myIrregPriceFieldState; 
    mama_quantity_t      myIrregVolume;             unsigned int  myIrregVolumeFieldState; 
    const char*          myIrregPartId;             unsigned int  myIrregPartIdFieldState; 
    const char*          myIrregTime;               unsigned int  myIrregTimeFieldState;   
    mama_quantity_t      myAccVolume;               unsigned int  myAccVolumeFieldState;   
    mama_quantity_t      myOffExAccVolume;          unsigned int  myOffExAccVolumeFieldState;
    mama_quantity_t      myOnExAccVolume;           unsigned int  myOnExAccVolumeFieldState; 
    unsigned int                 myTradeDirection;          unsigned int  myTradeDirectionFieldState;
    double               myNetChange;               unsigned int  myNetChangeFieldState;     
    double               myPctChange;               unsigned int  myPctChangeFieldState;     
    double               myOpenPrice;               unsigned int  myOpenPriceFieldState;     
    double               myHighPrice;               unsigned int  myHighPriceFieldState;     
    double               myLowPrice;                unsigned int  myLowPriceFieldState;      
    double               myClosePrice;              unsigned int  myClosePriceFieldState;    
    double               myPrevClosePrice;          unsigned int  myPrevClosePriceFieldState;
    const char*          myPrevCloseDate;           unsigned int  myPrevCloseDateFieldState; 
    double               myAdjPrevClose;            unsigned int  myAdjPrevCloseFieldState;  
    mama_u32_t           myTradeCount;              unsigned int  myTradeCountFieldState;    
    mama_quantity_t      myBlockVolume;             unsigned int  myBlockVolumeFieldState;   
    mama_u32_t           myBlockCount;              unsigned int  myBlockCountFieldState;
    double               myVwap;                    unsigned int  myVwapFieldState;      
    double               myOffExVwap;               unsigned int  myOffExVwapFieldState; 
    double               myOnExVwap;                unsigned int  myOnExVwapFieldState;  
    double               myTotalValue;              unsigned int  myTotalValueFieldState;
    double               myOffExTotalValue;         unsigned int  myOffExTotalValueFieldState;
    double               myOnExTotalValue;          unsigned int  myOnExTotalValueFieldState; 
    double               myStdDev;                  unsigned int  myStdDevFieldState;  
    mama_u64_t           myOrderId;                 unsigned int  myOrderIdFieldState; 
    //bool                  myLastGenericMsgWasTrade;
    //bool                  myGotTradeTime; 
    //bool                  myGotTradePrice;
    //bool                  myGotTradeSize; 
    //bool                  myGotTradeCount;
    double               myStdDevSum;               unsigned int  myStdDevSumFieldState;      
    double               myStdDevSumSquares;        unsigned int  myStdDevSumSquaresFieldState;
    const char*          myTradeUnits;              unsigned int  myTradeUnitsFieldState;  
    mama_seqnum_t        myLastSeqNum;              unsigned int  myLastSeqNumFieldState;  
    mama_seqnum_t        myHighSeqNum;              unsigned int  myHighSeqNumFieldState;  
    mama_seqnum_t        myLowSeqNum;               unsigned int  myLowSeqNumFieldState;   
    mama_seqnum_t        myTotalVolumeSeqNum;       unsigned int  myTotalVolumeSeqNumFieldState; 
    const char*          myCurrencyCode;            unsigned int  myCurrencyCodeFieldState; 
    const char*          myUniqueId;                unsigned int  myUniqueIdFieldState;     
    const char*          myTradeAction;             unsigned int  myTradeActionFieldState;  
    //bool                 myIsSnapshot;
    double               mySettlePrice;             unsigned int  mySettlePriceFieldState;  
    const char*          mySettleDate;              unsigned int  mySettleDateFieldState;   
    double               myOffExchangeTradePrice;   unsigned int  myOffExchangeTradePriceFieldState; 
    double               myOnExchangeTradePrice;    unsigned int  myOnExchangeTradePriceFieldState;  

    // The following fields are used for caching the last reported
    // trade, which might have been out of order.  The reason for
    // cahcing these values is to allow a configuration that passes
    // the minimum amount of data  (unchanged fields not sent).
    mama_seqnum_t         myEventSeqNum;            unsigned int  myEventSeqNumFieldState;
    const char*           myEventTime;              unsigned int  myEventTimeFieldState;  
    //double                myTradePrice;              unsigned int  myTradePriceFieldState; 
    //mama_quantity_t       myTradeVolume;             unsigned int  myTradeVolumeFieldState;
    //const char*                myTradePartId;             unsigned int  myTradePartIdFieldState;
    const char*           myTradeQualStr;           unsigned int  myTradeQualStrFieldState; 
    const char*           myTradeQualNativeStr;     unsigned int  myTradeQualNativeStrFieldState;
    mama_u32_t            mySellersSaleDays;        unsigned int  mySellersSaleDaysFieldState; 
    char                 myStopStockInd;            unsigned int  myStopStockIndFieldState;
    //mama_u32_t          myTmpTradeCount;             unsigned int  myTmpTradeCountFieldState;
    unsigned int                  myTradeExecVenue;         unsigned int  myTradeExecVenueFieldState;

    // Additional fields for cancels/error/corrections:
    //bool                  myIsCancel;                  //bool  myIsCancelFieldState; 
    mama_seqnum_t         myOrigSeqNum;             unsigned int  myOrigSeqNumFieldState;
    double                myOrigPrice;              unsigned int  myOrigPriceFieldState; 
    mama_quantity_t       myOrigVolume;             unsigned int  myOrigVolumeFieldState;
    const char*           myOrigPartId;             unsigned int  myOrigPartIdFieldState;
    const char*           myOrigQualStr;            unsigned int  myOrigQualStrFieldState;
    const char*           myOrigQualNativeStr;      unsigned int  myOrigQualNativeStrFieldState;
    mama_u32_t            myOrigSellersSaleDays;    unsigned int  myOrigSellersSaleDaysFieldState;
    char                 myOrigStopStockInd;        unsigned int  myOrigStopStockIndFieldState;
    double                myCorrPrice;              unsigned int  myCorrPriceFieldState; 
    mama_quantity_t       myCorrVolume;             unsigned int  myCorrVolumeFieldState; 
    const char*           myCorrPartId;             unsigned int  myCorrPartIdFieldState; 
    const char*           myCorrQualStr;            unsigned int  myCorrQualStrFieldState;
    const char*           myCorrQualNativeStr;      unsigned int  myCorrQualNativeStrFieldState;
    mama_u32_t            myCorrSellersSaleDays;    unsigned int  myCorrSellersSaleDaysFieldState;
    char                 myCorrStopStockInd;        unsigned int  myCorrStopStockIndFieldState;
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
    const char*           myTempSymbol;                    unsigned int  myTempSymbolFieldState;  
    const char*           myTempPartId;                    unsigned int  myTempPartIdFieldState;  
    const char*           myTempSrcTime;                   unsigned int  myTempSrcTimeFieldState; 
    const char*           myTempActTime;                   unsigned int  myTempActTimeFieldState; 
    const char*           myTempLineTime;                  unsigned int  myTempLineTimeFieldState;
    //const char*           myTempSendTime;                  unsigned int  myTempSendTimeFieldState;
    const char*           myTempPubId;                     unsigned int  myTempPubIdFieldState;   
    bool                  myTempIsIrregular;               unsigned int  myTempIsIrregularFieldState; 
    //bool                  myTempWasIrregular;              unsigned int  myTempWasIrregularFieldState;
    double                myTempLastPrice;                 unsigned int  myTempLastPriceFieldState; 
    mama_quantity_t       myTempLastVolume;                unsigned int  myTempLastVolumeFieldState;
    const char*           myTempLastPartId;                unsigned int  myTempLastPartIdFieldState;
    const char*           myTempLastTime;                  unsigned int  myTempLastTimeFieldState;   
    const char*           myTempTradeDate;                 unsigned int  myTempTradeDateFieldState;  
    //double                myTempIrregPrice;                unsigned int  myTempIrregPriceFieldState; 
    mama_quantity_t       myTempIrregVolume;               unsigned int  myTempIrregVolumeFieldState; 
    const char*           myTempIrregPartId;               unsigned int  myTempIrregPartIdFieldState; 
    const char*           myTempIrregTime;                 unsigned int  myTempIrregTimeFieldState;   
    mama_quantity_t       myTempAccVolume;                 unsigned int  myTempAccVolumeFieldState;   
    mama_quantity_t       myTempOffExAccVolume;            unsigned int  myTempOffExAccVolumeFieldState;
    mama_quantity_t       myTempOnExAccVolume;             unsigned int  myTempOnExAccVolumeFieldState; 
    unsigned int                  myTempTradeDirection;            unsigned int  myTempTradeDirectionFieldState;
    double                myTempNetChange;                 unsigned int  myTempNetChangeFieldState;     
    double                myTempPctChange;                 unsigned int  myTempPctChangeFieldState;     
    double                myTempOpenPrice;                 unsigned int  myTempOpenPriceFieldState;     
    double                myTempHighPrice;                 unsigned int  myTempHighPriceFieldState;     
    double                myTempLowPrice;                  unsigned int  myTempLowPriceFieldState;      
    double                myTempClosePrice;                unsigned int  myTempClosePriceFieldState;    
    double                myTempPrevClosePrice;            unsigned int  myTempPrevClosePriceFieldState;
    const char*           myTempPrevCloseDate;             unsigned int  myTempPrevCloseDateFieldState; 
    double                myTempAdjPrevClose;              unsigned int  myTempAdjPrevCloseFieldState;  
    mama_u32_t            myTempTradeCount;                unsigned int  myTempTradeCountFieldState;    
    mama_quantity_t       myTempBlockVolume;               unsigned int  myTempBlockVolumeFieldState;   
    mama_u32_t            myTempBlockCount;                unsigned int  myTempBlockCountFieldState;
    double                myTempVwap;                      unsigned int  myTempVwapFieldState;      
    double                myTempOffExVwap;                 unsigned int  myTempOffExVwapFieldState; 
    double                myTempOnExVwap;                  unsigned int  myTempOnExVwapFieldState;  
    double                myTempTotalValue;                unsigned int  myTempTotalValueFieldState;
    double                myTempOffExTotalValue;           unsigned int  myTempOffExTotalValueFieldState;
    double                myTempOnExTotalValue;            unsigned int  myTempOnExTotalValueFieldState; 
    double                myTempStdDev;                    unsigned int  myTempStdDevFieldState;  
    mama_u64_t            myTempOrderId;                   unsigned int  myTempOrderIdFieldState; 
    //bool                myTempLastGenericMsgWasTrade;
    //bool                myTempGotTradeTime; 
    //bool                myTempGotTradePrice;
    //bool                myTempGotTradeSize; 
    //bool                myTempGotTradeCount;
    double                myTempStdDevSum;                 unsigned int  myTempStdDevSumFieldState;      
    double                myTempStdDevSumSquares;          unsigned int  myTempStdDevSumSquaresFieldState;
    const char*           myTempTradeUnits;                unsigned int  myTempTradeUnitsFieldState;  
    mama_seqnum_t         myTempLastSeqNum;                unsigned int  myTempLastSeqNumFieldState;  
    mama_seqnum_t         myTempHighSeqNum;                unsigned int  myTempHighSeqNumFieldState;  
    mama_seqnum_t         myTempLowSeqNum;                 unsigned int  myTempLowSeqNumFieldState;   
    mama_seqnum_t         myTempTotalVolumeSeqNum;         unsigned int  myTempTotalVolumeSeqNumFieldState; 
    const char*           myTempCurrencyCode;              unsigned int  myTempCurrencyCodeFieldState; 
    const char*           myTempUniqueId;                  unsigned int  myTempUniqueIdFieldState;     
    const char*           myTempTradeAction;               unsigned int  myTempTradeActionFieldState;  
    //bool                myTempIsSnapshot;
    double                myTempSettlePrice;               unsigned int  myTempSettlePriceFieldState;  
    const char*           myTempSettleDate;                unsigned int  myTempSettleDateFieldState;   
    double                myTempOffExchangeTradePrice;     unsigned int  myTempOffExchangeTradePriceFieldState; 
    double                myTempOnExchangeTradePrice;      unsigned int  myTempOnExchangeTradePriceFieldState;  

    // The following fields are used for caching the last reported
    // trade, which might have been out of order.  The reason for
    // cahcing these values is to allow a configuration that passes
    // the minimum amount of data  (unchanged fields not sent).
    mama_seqnum_t         myTempEventSeqNum;               unsigned int  myTempEventSeqNumFieldState;
    const char*           myTempEventTime;                 unsigned int  myTempEventTimeFieldState;  
    //double              myTempTradePrice;                unsigned int  myTempTradePriceFieldState; 
    //mama_quantity_t     myTempTradeVolume;               unsigned int  myTempTradeVolumeFieldState;
    //const char*         myTempTradePartId;               unsigned int  myTempTradePartIdFieldState;
    const char*           myTempTradeQualStr;              unsigned int  myTempTradeQualStrFieldState; 
    const char*           myTempTradeQualNativeStr;        unsigned int  myTempTradeQualNativeStrFieldState;
    mama_u32_t            myTempSellersSaleDays;           unsigned int  myTempSellersSaleDaysFieldState; 
    char                 myTempStopStockInd;               unsigned int  myTempStopStockIndFieldState;
    //mama_u32_t          myTempTmpTradeCount;             unsigned int  myTempTmpTradeCountFieldState;
    unsigned int                  myTempTradeExecVenue;            unsigned int  myTempTradeExecVenueFieldState;

    // Additional fields for cancels/error/corrections:
    //bool                  myTempIsCancel;                  //unsigned int  myTempIsCancelFieldState; 
    mama_seqnum_t         myTempOrigSeqNum;                unsigned int  myTempOrigSeqNumFieldState;
    double                myTempOrigPrice;                 unsigned int  myTempOrigPriceFieldState; 
    mama_quantity_t       myTempOrigVolume;                unsigned int  myTempOrigVolumeFieldState;
    const char*           myTempOrigPartId;                unsigned int  myTempOrigPartIdFieldState;
    const char*           myTempOrigQualStr;               unsigned int  myTempOrigQualStrFieldState;
    const char*           myTempOrigQualNativeStr;         unsigned int  myTempOrigQualNativeStrFieldState;
    mama_u32_t            myTempOrigSellersSaleDays;       unsigned int  myTempOrigSellersSaleDaysFieldState;
    char                 myTempOrigStopStockInd;           unsigned int  myTempOrigStopStockIndFieldState; 
    double                myTempCorrPrice;                 unsigned int  myTempCorrPriceFieldState; 
    mama_quantity_t       myTempCorrVolume;                unsigned int  myTempCorrVolumeFieldState; 
    const char*           myTempCorrPartId;                unsigned int  myTempCorrPartIdFieldState; 
    const char*           myTempCorrQualStr;               unsigned int  myTempCorrQualStrFieldState;
    const char*           myTempCorrQualNativeStr;         unsigned int  myTempCorrQualNativeStrFieldState;
    mama_u32_t            myTempCorrSellersSaleDays;       unsigned int  myTempCorrSellersSaleDaysFieldState;
    char                 myTempCorrStopStockInd;           unsigned int  myTempCorrStopStockIndFieldState;
    //const char*         myTempCancelTime;                //unsigned int  myTempCancelTimeFieldState;

    // Additional fields for gaps:
    //mama_seqnum_t         myTempGapBegin;                  unsigned int  myTempGapBeginFieldState;
    //mama_seqnum_t         myTempGapEnd;                    unsigned int  myTempGapEndFieldState;

    // Additional fields for closing summaries:
    //bool                  myTempIsIndicative;
} TradeTestCache;

typedef struct TradeControlFields
{
    const char*      mySymbol;      
    mama_u32_t       myTradeCount;
    mama_quantity_t  myTradeVolume;
    Wombat::MamaPrice myTradePrice;
    mama_seqnum_t    myTradeEventSeqNum;
    const char*      myTradeEventTime;
    const char*      myTradeActivityTime;
    const char*      myTradePartId;
    const char*      myTradeQual;
    const char*      myTradeQualNative;
    bool             myTradeIsIrregular;     
    const char*      myTradeMsgQualStr;
    unsigned int             myTradeMsgQual;
    const char*      myTradeMsgQualPossDupStr;
    unsigned int             myTradeMsgQualPossDup;
    mama_quantity_t  myAccVolume;
} TradeControlFields;

typedef struct TradeTestFields
{        
    const char*      myTempSymbol;   
    mama_u32_t       myTempTradeCount;
    mama_quantity_t  myTempTradeVolume;
    Wombat::MamaPrice myTempTradePrice;
    mama_seqnum_t    myTempTradeEventSeqNum;
    const char*      myTempTradeEventTime;
    const char*      myTempTradeActivityTime;
    const char*      myTempTradePartId;
    const char*      myTempTradeQual;
    const char*      myTempTradeQualNative;
    bool             myTempTradeIsIrregular;   
    const char*      myTempTradeMsgQualStr;
    unsigned int             myTempTradeMsgQual;        
    mama_quantity_t  myTempAccVolume;
} TradeTestFields;

typedef struct QuoteTestCache
{
    // The following fields are used for caching the offical quote and
    // related fields.  These fields can be used by applications for
    // reference and will be passed for recaps.
    const char*     myTempSymbol;                            unsigned int   myTempSymbolFieldState;   
    const char*     myTempPartId;                            unsigned int   myTempPartIdFieldState;   
    const char*     myTempSrcTime;                           unsigned int   myTempSrcTimeFieldState;  
    const char*     myTempActTime;                           unsigned int   myTempActTimeFieldState;  
    const char*     myTempLineTime;                          unsigned int   myTempLineTimeFieldState; 
    const char*     myTempSendTime;                          unsigned int   myTempSendTimeFieldState; 
    const char*     myTempPubId;                             unsigned int   myTempPubIdFieldState;
    double          myTempBidPrice;                          unsigned int   myTempBidPriceFieldState; 
    mama_quantity_t myTempBidSize;                           unsigned int   myTempBidSizeFieldState;
    mama_quantity_t myTempBidDepth;                          unsigned int   myTempBidDepthFieldState;
    const char*     myTempBidPartId;                         unsigned int   myTempBidPartIdFieldState;
    double          myTempBidClosePrice;                     unsigned int   myTempBidClosePriceFieldState; 
    const char*     myTempBidCloseDate;                      unsigned int   myTempBidCloseDateFieldState;

    double          myTempBidPrevClosePrice;                 unsigned int   myTempBidPrevClosePriceFieldState;
    const char*     myTempBidPrevCloseDate;                  unsigned int   myTempBidPrevCloseDateFieldState; 
    double          myTempBidHigh;                           unsigned int   myTempBidHighFieldState;          
    double          myTempBidLow;                            unsigned int   myTempBidLowFieldState;           
    double          myTempAskPrice;                          unsigned int   myTempAskPriceFieldState;         
    mama_quantity_t myTempAskSize;                           unsigned int   myTempAskSizeFieldState;          
    mama_quantity_t myTempAskDepth;                          unsigned int   myTempAskDepthFieldState;         
    const char*     myTempAskPartId;                         unsigned int   myTempAskPartIdFieldState;        
    double          myTempAskClosePrice;                     unsigned int   myTempAskClosePriceFieldState;     
    const char*     myTempAskCloseDate;                      unsigned int   myTempAskCloseDateFieldState;     
    double          myTempAskPrevClosePrice;                 unsigned int   myTempAskPrevClosePriceFieldState;
    const char*     myTempAskPrevCloseDate;                  unsigned int   myTempAskPrevCloseDateFieldState; 
    double          myTempAskHigh;                           unsigned int   myTempAskHighFieldState;          
    double          myTempAskLow;                            unsigned int   myTempAskLowFieldState;           
    //double          myTempMidPrice;                          unsigned int   myTempMidPriceFieldState;         
    mama_seqnum_t   myTempEventSeqNum;                       unsigned int   myTempEventSeqNumFieldState;      
    const char*     myTempEventTime;                         unsigned int   myTempEventTimeFieldState;        
    const char*     myTempQuoteDate;                         unsigned int   myTempQuoteDateFieldState;        
    const char*     myTempQuoteQualStr;                      unsigned int   myTempQuoteQualStrFieldState;     
    const char*     myTempQuoteQualNative;                   unsigned int   myTempQuoteQualNativeFieldState;  

    const char*     myTempAskTime;                           unsigned int   myTempAskTimeFieldState;          
    const char*     myTempBidTime;                           unsigned int   myTempBidTimeFieldState;          
    const char*     myTempAskIndicator;                      unsigned int   myTempAskIndicatorFieldState;     
    const char*     myTempBidIndicator;                      unsigned int   myTempBidIndicatorFieldState;     
    //mama_u32_t      myTempAskUpdateCount;                    unsigned int   myTempAskUpdateCountFieldState;   
    //mama_u32_t      myTempBidUpdateCount;                    unsigned int   myTempBidUpdateCountFieldState;   
    double          myTempAskYield;                          unsigned int   myTempAskYieldFieldState;         
    double          myTempBidYield;                          unsigned int   myTempBidYieldFieldState;         

    mama_u32_t      myTempQuoteCount;                        unsigned int   myTempQuoteCountFieldState;
    //mama_seqnum_t   myTempGapBegin;                          unsigned int   myTempGapBeginFieldState;         
    //mama_seqnum_t   myTempGapEnd;                            unsigned int   myTempGapEndFieldState;
    const char*     myTempBidSizesList;                      unsigned int   myTempBidSizesListFieldState;
    const char*     myTempAskSizesList;                      unsigned int   myTempAskSizesListFieldState;

    char            myTempShortSaleBidTick;                  unsigned int   myTempShortSaleBidTickFieldState;
} QuoteTestCache;

typedef struct QuoteControlCache
{
    
    // The following fields are used for caching the offical quote and
    // related fields.  These fields can be used by applications for
    // reference and will be passed for recaps.
    const char*     mySymbol;              unsigned int   mySymbolFieldState;   
    //const char*     myPartId;              unsigned int   myPartIdFieldState;   
    const char*     mySrcTime;             unsigned int   mySrcTimeFieldState;  
    const char*     myActTime;             unsigned int   myActTimeFieldState;  
    const char*     myLineTime;            unsigned int   myLineTimeFieldState; 
    //const char*     mySendTime;            unsigned int   mySendTimeFieldState; 
    const char*     myPubId;               unsigned int   myPubIdFieldState;
    double          myBidPrice;            unsigned int   myBidPriceFieldState; 
    mama_quantity_t myBidSize;             unsigned int   myBidSizeFieldState;
    mama_quantity_t myBidDepth;            unsigned int   myBidDepthFieldState;
    const char*     myBidPartId;           unsigned int   myBidPartIdFieldState;
    double          myBidClosePrice;       unsigned int   myBidClosePriceFieldState; 
    const char*     myBidCloseDate;        unsigned int   myBidCloseDateFieldState;
    double          myBidPrevClosePrice;   unsigned int   myBidPrevClosePriceFieldState;
    const char*     myBidPrevCloseDate;    unsigned int   myBidPrevCloseDateFieldState; 
    double          myBidHigh;             unsigned int   myBidHighFieldState;          
    double          myBidLow;              unsigned int   myBidLowFieldState;           
    double          myAskPrice;            unsigned int   myAskPriceFieldState;         
    mama_quantity_t myAskSize;             unsigned int   myAskSizeFieldState;          
    mama_quantity_t myAskDepth;            unsigned int   myAskDepthFieldState;         
    const char*     myAskPartId;           unsigned int   myAskPartIdFieldState;        
    double          myAskClosePrice;       unsigned int   myAskClosePriceFieldState;     
    const char*     myAskCloseDate;        unsigned int   myAskCloseDateFieldState;     
    double          myAskPrevClosePrice;   unsigned int   myAskPrevClosePriceFieldState;
    const char*     myAskPrevCloseDate;    unsigned int   myAskPrevCloseDateFieldState; 
    double          myAskHigh;             unsigned int   myAskHighFieldState;          
    double          myAskLow;              unsigned int   myAskLowFieldState;           
    //double          myMidPrice;            unsigned int   myMidPriceFieldState;
    mama_seqnum_t   myEventSeqNum;         unsigned int   myEventSeqNumFieldState;      
    const char*     myEventTime;           unsigned int   myEventTimeFieldState;        
    const char*     myQuoteDate;           unsigned int   myQuoteDateFieldState;        
    const char*     myQuoteQualStr;        unsigned int   myQuoteQualStrFieldState;     
    const char*     myQuoteQualNative;     unsigned int   myQuoteQualNativeFieldState;  

    const char*     myAskTime;             unsigned int   myAskTimeFieldState;          
    const char*     myBidTime;             unsigned int   myBidTimeFieldState;          
    const char*     myAskIndicator;        unsigned int   myAskIndicatorFieldState;     
    const char*     myBidIndicator;        unsigned int   myBidIndicatorFieldState;     
    //mama_u32_t      myAskUpdateCount;      unsigned int   myAskUpdateCountFieldState;   
    //mama_u32_t      myBidUpdateCount;      unsigned int   myBidUpdateCountFieldState;   
    double          myAskYield;            unsigned int   myAskYieldFieldState;         
    double          myBidYield;            unsigned int   myBidYieldFieldState;         

    //mama_u32_t      myQuoteCount;          unsigned int   myQuoteCountFieldState;
    //mama_seqnum_t   myGapBegin;            unsigned int   myGapBeginFieldState;         
    //mama_seqnum_t   myGapEnd;              unsigned int   myGapEndFieldState;           
    const char*     myBidSizesList;        unsigned int   myBidSizesListFieldState;
    const char*     myAskSizesList;        unsigned int   myAskSizesListFieldState;
    char            myShortSaleBidTick;    unsigned int   myShortSaleBidTickFieldState;//doesnt getupdated in quotelistener.
} QuoteControlCache;

typedef struct QuoteControlFields
{
    const char*     mySymbol;
    mama_u32_t      myQuoteCount;
    Wombat::MamaPrice myBidPrice;
    Wombat::MamaPrice myBidClosePrice;
    mama_quantity_t myBidSize;
    mama_quantity_t myAskSize;
    Wombat::MamaPrice myAskPrice;
    Wombat::MamaPrice myAskClosePrice;
    mama_seqnum_t   myEventSeqNum;
    const char*     myEventTimeStr;
    const char*     myQuoteQualStr;
    const char*     myMsgQualStr;
    unsigned int            myMsgQual;
    const char*     myMsgQualPossDupStr;
    unsigned int            myMsgQualPossDup;       
    mama_seqnum_t   myGapBeginSeqNum;
    mama_seqnum_t   myEndGapSeqNum;       
} QuoteControlFields;

typedef struct QuoteTestFields
{
    const char*     myTempSymbol;
    mama_u32_t      myTempQuoteCount;
    Wombat::MamaPrice myTempBidPrice;
    Wombat::MamaPrice myTempBidClosePrice;
    mama_quantity_t myTempBidSize;
    mama_quantity_t myTempAskSize;
    Wombat::MamaPrice myTempAskPrice;
    Wombat::MamaPrice myTempAskClosePrice;
    mama_seqnum_t   myTempEventSeqNum;
    const char*     myTempEventTimeStr;
    const char*     myTempQuoteQualStr;
    const char*     myTempMsgQualStr;
    unsigned int            myTempMsgQual;
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


