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

#ifndef MamdaunittestUtilsH
#define MamdaunittestUtilsH

#include <mamda/MamdaConfig.h>
#include <mama/mamacpp.h>

#include "../../../../../mama/c_cpp/src/c/playback/playbackpublisher.h"
#include "../../../../../mama/c_cpp/src/c/playback/playbackcapture.h"
#include "../../../../../mama/c_cpp/src/c/playback/playbackFileParser.h"

#include "MamdaUnitTestCache.h"

namespace Wombat
{
    typedef struct playbackImpl
    {
	    char*     					myInputFileName;
        mamaPlaybackFileParser 		myParser;
	    mamaMsg         			myTempMsg;
	    char*						header;
	
    } playbackImpl;

    void addMamaHeaderFields (MamaMsg&  msg,
                              short     msgType,
                              short     msgStatus,
                              int       seqNum);

    void SetTradeFields (MamaMsg& msg, MamaDateTime& val);

    void SetTradeRecapFields (MamaMsg& msg);

    void SetTradeReportFields (MamaMsg& msg, MamaDateTime& val);

    void SetTradeCancelOrErrorFields (MamaMsg& msg);

    void SetTradeGapFields1 (MamaMsg& msg);

    void SetTradeGapFields2 (MamaMsg& msg);

    void SetTradeCorrectionFields (MamaMsg& msg);

    void SetTradeOutOfSeqFields (MamaMsg& msg);

    void SetTradeClosingFields (MamaMsg& msg);

    void SetTradePossDupFields (MamaMsg& msg);   
    
    void SetQuoteRecapFields (MamaMsg& msg);
    
    void SetQuoteUpdateFields (MamaMsg& msg);
    
    void SetQuoteClosingFields (MamaMsg& msg);
    
    void SetQuoteOutOfSeqFields (MamaMsg& msg);
    
    void SetQuotePossDupFields (MamaMsg& msg);
    
    void SetQuoteGapFields1 (MamaMsg& msg);
    
    void SetQuoteGapFields2 (MamaMsg& msg);
    
    void SetQuoteCommonFields (MamaMsg& msg, MamaDateTime& val);
    
    void SetQuoteClosingFields (MamaMsg& msg, MamaDateTime& val);
    
    void createRecap (MamaMsg& msg);

    void createReport (MamaMsg& msg);

    void createUpdate (MamaMsg& msg);

    void createUpdateWithoutTrade (MamaMsg& msg);
    
    void SetRecapMsgFields (MamaMsg& msg);
    
    void SetAtomicGap1MsgFields (MamaMsg& msg);

    void SetAtomicGap1MsgFields (MamaMsg& msg);
    
    void SetDeltaMsgFields (MamaMsg& msg);

    void SetPriceLevelEntryMsg (MamaMsg& msg, MamaDateTime& val);

    void SetPriceLevelMsg (MamaMsg& msg);
    
    void SetMsgFields (MamaMsg& msg);

    void resetMsg (MamaMsg& msg);
    
    MamaMsg* createBlankMsg (short  msgType, 
                             short  msgStatus, 
                             int    seqNum);
                   
    bool CompareTest (const char*  file1, 
                      const char*  file2 , 
                      const char*  file3 );
    
    MamaMsg*    createBookMsg (mamaMsgType type, int depth);

    MamaMsg* createBookMsg (mamaMsgType  type, 
                            int          depth, 
                            int          numEntriesPerLevel);
        
    MamaMsg* createBookMsgOneSide (mamaMsgType type, int depth);

    bool fileExists (const char* fileName);

    void cleanup (playbackImpl* impl);

    void initializeQuoteControlCache  (QuoteControlCache&   quoteControlCache);
    void initializeQuoteControlFields (QuoteControlFields&  quoteControlFields);
    void initializeTradeControlFields (TradeControlFields&  tradeControlFields);
}

#endif // MamdaunittestUtilsH
