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

#include "MamdaUnitTestUtils.h"
#include "mamda/MamdaCommonFields.h"
#include "mamda/MamdaDataException.h"

#include <sstream>
#include <iostream>
#include <fstream>

#include <wombat/wMessageStats.h>
#include <sys/stat.h>

#include <mamda/MamdaFieldState.h>
#include <mama/msgtype.h>

#include <mamda/MamdaOrderBookFields.h>
#include <mama/mamacpp.h>
#include <mama/MamaMsg.h>
#include <mamda/MamdaSubscription.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaOrderBookHandler.h>
#include <mamda/MamdaOrderBookListener.h>
#include <mamda/MamdaOrderBook.h>
#include <mamda/MamdaOrderBookPriceLevel.h>
#include <mamda/MamdaOrderBookEntry.h>
#include <mamda/MamdaOrderBookTypes.h>

using namespace std;

namespace Wombat
{
    void addMamaHeaderFields (MamaMsg&  msg,
                              short     msgType,
                              short     msgStatus,
                              int       seqNum)
    {  
        msg.addU8  (NULL, 1,  msgType);
        msg.addU8  (NULL, 2,  msgStatus);
        msg.addU32 (NULL, 10, seqNum);             
        msg.addU64 (NULL, 20, 1);   
    }    
   
    void SetTradeFields (MamaMsg&       msg, 
                         MamaDateTime&  val)
    {          
        msg.addU8("MdMsgType", 1, 14);  
        msg.addU8("MdMsgStatus",2, 0); 
        msg.addU32("MdSeqNum", 10, 42868);    
        msg.addU8("MamaAppMsgType", 18, 14);
        msg.addU64("MamaSenderId", 20, 68471603);
        msg.addString("wIssueSymbol", 305, "SPY");     
        msg.addString("wSrcTime", 465," 14:40:33.760");  
        msg.addString("wTradePartId",480, "D");
        msg.addF64("wTotalVolume",475,113561846);    
        msg.addF64("wTotalValue",474,16349.5002);
        msg.addF64("wVwap",494, 144.035272); 
        msg.addI32("wTradeCount",901,1);
        msg.addBoolean("wIsIrregular",304, 0);
        msg.addString("wTradeQualifier",482, "Normal");   
        msg.addI32("wTradeSeqNum",483, 1);
        msg.addString("wSaleCondition",450, "@"); 
        msg.addF64("wTradePrice",481, 144.057500);    
        msg.addPrice("wNetChange",341, 0.607500);    
        msg.addF64("wPctChange",431, 0.420000); 
        msg.addString("wTradeUnits",487, "4DP");   
        msg.addU64("wTradeTick",484, 1); 
        msg.addDateTime("wTradeTime",485, val); 
        msg.addI64("wTradeVolume",488, 100);   
        msg.addDateTime("wActivityTime",102,val);  
        //msg.addDateTime("wSendTime",?, val);    
        msg.addString("wLineTime", 1174,"14:40:33.769");    
        msg.addU16("wMsgQual",21,1);      
        msg.addString("wPubId",495,"cta");     
        msg.addI32("wEventSeqNum",3036,18);    
        msg.addDateTime("wEventTime",1696, val);       
        msg.addString("wIrregPartId",299, "ccc");      
        msg.addF64("wIrregSize",301, 12.23);     
        msg.addDateTime("wIrregTime",302, val);      
        msg.addDateTime("wTradeDate",476, val);    
        msg.addF64("wOffExchangeTotalVolume",4553, 12.23);     
        msg.addF64("wOnExchangeTotalVolume",4552, 12.23);    
        msg.addPrice("wOpenPrice",407, 12.23);         
        msg.addPrice("wHighPrice",278, 12.23);   
        msg.addPrice("wLowPrice",327, 12.23);     
        msg.addPrice("wClosePrice",242, 12.23);     
        msg.addPrice("wPrevClosePrice",436, 12.23);     
        msg.addPrice("wAdjPrevClose",520, 12.23);     
        msg.addDateTime("wPrevCloseDate",435, val);     
        msg.addI32("wBlockCount",902,18);    
        msg.addI32("wBlockVolume",903,18);   
        msg.addF64("wOffExchangeVWAP",4557, 12.23);     
        msg.addF64("wOnExchangeVWAP",4556, 12.23);     
        msg.addF64("wOffExchangeTotalValue",4555, 12.23);     
        msg.addF64("wOnExchangeTotalValue",4554, 12.23);     
        msg.addF64("wStdDev",1011, 12.23);     
        msg.addF64("wStdDevSum",1012, 12.23);     
        msg.addF64("wStdDevSumSquares",1013, 12.23);    
        //msg.addU32("wLastTradeSeqNum",4788, 1);  This fid is incorrect(given in cta) 
        //msg.addI32("wHighSeqNum",4796, 1);   This fid is incorrect(given in cta)    
        //msg.addI64("wLowSeqNum",4745, 1);   This fid is incorrect(given in cta)       
        //msg.addI32("wTotalVolumeSeqNum",4744, 1); This fid is incorrect(given in cta)      
        msg.addString("wCurrency",255, "ccc");      
        msg.addPrice("wSettlePrice",458, 12.23);      
        msg.addDateTime("wSettleDate",456, val);     
        msg.addU64("wOrderId",408,1);     
        msg.addString("wUniqueId",1656, "ccc");      
        //msg.addString("wTradeAction",4792, "ccc");  This fid is incorrect(given in cta)        
        msg.addU32("wTradeExecVenueEnum",2742, 1);    
        msg.addU32("wSellersSaleDays",452, 1); 
        msg.addChar("wStopStockIndicator",467, 'c');  
        msg.addI32("wOrigSeqNum",423, 1); 
        msg.addPrice("wOrigPrice",420, 12.23);    
        msg.addF64("wOrigSize",424, 12.23);     
        msg.addString("wOrigPartId",419, "ccc");     
        msg.addString("wOrigQualifier",421, "ccc");     
        msg.addString("wOrigCondition",418, "ccc");     
        msg.addU32("wOrigSaleDays",422, 1); 
        msg.addChar("wOrigStopStockInd",425, 'c');       
        msg.addPrice("wCorrPrice",248, 12.23);    
        msg.addF64("wCorrPrice",248, 12.23);     
        msg.addString("wCorrPartId",247, "ccc");     
        msg.addString("wCorrQualifier",249, "ccc");     
        msg.addString("wCorrCondition",246, "ccc");     
        msg.addU32("wCorrSaleDays",250, 1); 
        msg.addChar("wCorrStopStockInd",252, 'c');     
        msg.addPrice("wOffExchangeTradePrice",4594, 12.23);     
        msg.addPrice("wOnExchangeTradePrice",4593, 12.23);     
        msg.addF64("wCorrSize",251,12.23);
    }
       
    void SetTradeRecapFields(MamaMsg& msg)
    {      
        msg.addU8(" MdMsgType",1, 6);
        msg.addU8("  MdMsgStatus",2,0);
        msg.addU32("  MdSeqNum",10,42868);
        msg.addU8("MamaAppMsgType",18,14);
        msg.addU64("MamaSenderId",20,68471603);
        msg.addI32("wEntitleCode",496,64);
        msg.addString("wLineTime",1174,"15:33:43.484");
        msg.addString("wSrcTime",465,"15:33:43.480");
        msg.addString("wIssueSymbol",305,"SPY");
        msg.addString("wTradePartId",480,"D");
        msg.addI64(" wTotalVolume",475,113561846);
        msg.addF64("wTotalValue",474,16356911349.500002);
        msg.addF64("wVwap",494,144.035272);
        msg.addI32("wTradeCount",901,879756);
        msg.addBoolean("wIsIrregular",304,0);
        msg.addString("wTradeQualifier",482, "Normal");
        msg.addI32("wTradeSeqNum",483, 125864);
        msg.addString("wSaleCondition",450,"@");
        msg.addF64("wTradePrice",481,144.057500);
        msg.addF64("wNetChange",341,0.607500);
        msg.addF64("wPctChange",431,0.420000);
        msg.addString("wTradeUnits",487,"4DP");
        msg.addString("wTradeTick",484,"+");
        msg.addString("wTradeTime",485,"15:33:43.480");
        msg.addI64("wTradeVolume",488,100);
    } 

    void SetTradeReportFields (MamaMsg&       msg, 
                               MamaDateTime&  val)
    {      
        msg.addU8(" MdMsgType",1, 14);
        msg.addU8("  MdMsgStatus",2,0);
        msg.addU32("  MdSeqNum",10,42868);
        msg.addU8("MamaAppMsgType",18,14);
        msg.addU64("MamaSenderId",20,68471603);
        msg.addI32("wEntitleCode",496,64);
        msg.addString("wLineTime",1174,"15:33:43.484");
        msg.addString("wSrcTime",465,"15:33:43.480");
        msg.addString("wIssueSymbol",305,"SPY");
        msg.addString("wTradePartId",480,"D");
        msg.addI64(" wTotalVolume",475,113561846);
        msg.addF64("wTotalValue",474,16356911349.500002);
        msg.addF64("wVwap",494,144.035272);
        msg.addI32("wTradeCount",901,1);
        msg.addBoolean("wIsIrregular",304,0);
        msg.addString("wTradeQualifier",482, "Normal");
        msg.addI32("wTradeSeqNum",483, 1);
        msg.addString("wSaleCondition",450,"@");
        msg.addF64("wTradePrice",481,144.057500);
        msg.addF64("wNetChange",341,0.607500);
        msg.addF64("wPctChange",431,0.420000);
        msg.addString("wTradeUnits",487,"4DP");
        msg.addString("wTradeTick",484,"+");
        msg.addString("wTradeTime",485,"15:33:43.480");
        msg.addI64("wTradeVolume",488,100);
        msg.addDateTime("wActivityTime",102,val);  
    }

    void SetTradeCancelOrErrorFields (MamaMsg& msg)
    {      
        msg.addU8("MdMsgType",1, MAMA_MSG_TYPE_CANCEL);
        msg.addU8("MdMsgStatus",2,0);
        msg.addU32("MdSeqNum",10,42868);
        msg.addU8("MamaAppMsgType",18,14);
        msg.addU64("MamaSenderId",20,68471603);
        msg.addI32("wEntitleCode",496,64);
        msg.addString("wLineTime",1174,"15:33:43.484");
        msg.addString("wSrcTime",465,"15:33:43.480");
        msg.addString("wIssueSymbol",305,"SPY");
        msg.addString("wTradePartId",480,"D");
        msg.addI64("wTotalVolume",475,113561846);
        msg.addF64("wTotalValue",474,16356911349.500002);
        msg.addF64("wVwap",494,144.035272);
        msg.addI32("wTradeCount",901,879756);
        msg.addBoolean("wIsIrregular",304,0);
        msg.addString("wTradeQualifier",482, "Normal");
        msg.addI32("wTradeSeqNum",483, 125864);
        msg.addString("wSaleCondition",450,"@");
        msg.addF64("wTradePrice",481,144.057500);
        msg.addF64("wNetChange",341,0.607500);
        msg.addF64("wPctChange",431,0.420000);
        msg.addString("wTradeUnits",487,"4DP");
        msg.addString("wTradeTick",484,"+");
        msg.addString("wTradeTime",485,"15:33:43.480");
        msg.addI64("wTradeVolume",488,100);
    }

    void SetTradeGapFields1 (MamaMsg& msg)
    {      
        msg.addU8(" MdMsgType",1, 14);
        msg.addU8("  MdMsgStatus",2,0);
        msg.addU32("  MdSeqNum",10,42868);
        msg.addU8("MamaAppMsgType",18,14);
        msg.addU64("MamaSenderId",20,68471603);
        msg.addI32("wEntitleCode",496,64);
        msg.addString("wLineTime",1174,"15:33:43.484");
        msg.addString("wSrcTime",465,"15:33:43.480");
        msg.addString("wIssueSymbol",305,"SPY");
        msg.addString("wTradePartId",480,"D");
        msg.addI64(" wTotalVolume",475,113561846);
        msg.addF64("wTotalValue",474,16356911349.500002);
        msg.addF64("wVwap",494,144.035272);
        msg.addI32("wTradeCount",901,1);
        msg.addBoolean("wIsIrregular",304,0);
        msg.addString("wTradeQualifier",482, "Normal");
        msg.addI32("wTradeSeqNum",483, 1);
        msg.addString("wSaleCondition",450,"@");
        msg.addF64("wTradePrice",481,144.057500);
        msg.addF64("wNetChange",341,0.607500);
        msg.addF64("wPctChange",431,0.420000);
        msg.addString("wTradeUnits",487,"4DP");
        msg.addString("wTradeTick",484,"+");
        msg.addString("wTradeTime",485,"15:33:43.480");
        msg.addI64("wTradeVolume",488,100);
    }

    void SetTradeGapFields2 (MamaMsg& msg)
    {      
        msg.addU8(" MdMsgType",1, 14);
        msg.addU8("  MdMsgStatus",2,0);
        msg.addU32("  MdSeqNum",10,42868);
        msg.addU8("MamaAppMsgType",18,14);
        msg.addU64("MamaSenderId",20,68471603);
        msg.addI32("wEntitleCode",496,64);
        msg.addString("wLineTime",1174,"15:33:43.484");
        msg.addString("wSrcTime",465,"15:33:43.480");
        msg.addString("wIssueSymbol",305,"SPY");
        msg.addString("wTradePartId",480,"D");
        msg.addI64(" wTotalVolume",475,113561846);
        msg.addF64("wTotalValue",474,16356911349.500002);
        msg.addF64("wVwap",494,144.035272);
        msg.addI32("wTradeCount",901,3);
        msg.addBoolean("wIsIrregular",304,0);
        msg.addString("wTradeQualifier",482, "Normal");
        msg.addI32("wTradeSeqNum",483, 3);
        msg.addString("wSaleCondition",450,"@");
        msg.addF64("wTradePrice",481,144.057500);
        msg.addF64("wNetChange",341,0.607500);
        msg.addF64("wPctChange",431,0.420000);
        msg.addString("wTradeUnits",487,"4DP");
        msg.addString("wTradeTick",484,"+");
        msg.addString("wTradeTime",485,"15:33:43.480");
        msg.addI64("wTradeVolume",488,100);
    }

    void SetTradeCorrectionFields (MamaMsg& msg)
    {      
        msg.addU8(" MdMsgType",1, MAMA_MSG_TYPE_CORRECTION);
        msg.addU8("  MdMsgStatus",2,0);
        msg.addU32("  MdSeqNum",10,42868);
        msg.addU8("MamaAppMsgType",18,14);
        msg.addU64("MamaSenderId",20,68471603);
        msg.addI32("wEntitleCode",496,64);
        msg.addString("wLineTime",1174,"15:33:43.484");
        msg.addString("wSrcTime",465,"15:33:43.480");
        msg.addString("wIssueSymbol",305,"SPY");
        msg.addString("wTradePartId",480,"D");
        msg.addI64(" wTotalVolume",475,113561846);
        msg.addF64("wTotalValue",474,16356911349.500002);
        msg.addF64("wVwap",494,144.035272);
        msg.addI32("wTradeCount",901,879756);
        msg.addBoolean("wIsIrregular",304,0);
        msg.addString("wTradeQualifier",482, "Normal");
        msg.addI32("wTradeSeqNum",483, 125864);
        msg.addString("wSaleCondition",450,"@");
        msg.addF64("wTradePrice",481,144.057500);
        msg.addF64("wNetChange",341,0.607500);
        msg.addF64("wPctChange",431,0.420000);
        msg.addString("wTradeUnits",487,"4DP");
        msg.addString("wTradeTick",484,"+");
        msg.addString("wTradeTime",485,"15:33:43.480");
        msg.addI64("wTradeVolume",488,100);
    }

    void SetTradeOutOfSeqFields (MamaMsg& msg)
    {      
        msg.addU8("MdMsgType",1, 14);
        msg.addU8("MdMsgStatus",2,0);
        msg.addU32("MdSeqNum",10,42868);
        msg.addU8("MamaAppMsgType",18,14);
        msg.addU64("MamaSenderId",20,68471603);
        msg.addI32("wEntitleCode",496,64);
        msg.addString("wLineTime",1174,"15:33:43.484");
        msg.addString("wSrcTime",465,"15:33:43.480");
        msg.addString("wIssueSymbol",305,"SPY");
        msg.addString("wTradePartId",480,"D");
        msg.addI64(" wTotalVolume",475,113561846);
        msg.addF64("wTotalValue",474,16356911349.500002);
        msg.addF64("wVwap",494,144.035272);
        msg.addI32("wTradeCount",901,879756);
        msg.addBoolean("wIsIrregular",304,0);
        msg.addString("wTradeQualifier",482, "Normal");
        msg.addI32("wTradeSeqNum",483, 1);
        msg.addString("wSaleCondition",450,"@");
        msg.addF64("wTradePrice",481,144.057500);
        msg.addF64("wNetChange",341,0.607500);
        msg.addF64("wPctChange",431,0.420000);
        msg.addString("wTradeUnits",487,"4DP");
        msg.addString("wTradeTick",484,"+");
        msg.addString("wTradeTime",485,"15:33:43.480");
        msg.addI64("wTradeVolume",488,100);
        msg.addU16("wMsgQual",21,16);    
    }

    void SetTradeClosingFields (MamaMsg& msg)
    {      
        msg.addU8(" MdMsgType",1, MAMA_MSG_TYPE_CLOSING);
        msg.addU8("  MdMsgStatus",2,0);
        msg.addU32("  MdSeqNum",10,42868);
        msg.addU8("MamaAppMsgType",18,14);
        msg.addU64("MamaSenderId",20,68471603);
        msg.addI32("wEntitleCode",496,64);
        msg.addString("wLineTime",1174,"15:33:43.484");
        msg.addString("wSrcTime",465,"15:33:43.480");
        msg.addString("wIssueSymbol",305,"SPY");
        msg.addString("wTradePartId",480,"D");
        msg.addI64(" wTotalVolume",475,113561846);
        msg.addF64("wTotalValue",474,16356911349.500002);
        msg.addF64("wVwap",494,144.035272);
        msg.addI32("wTradeCount",901,879756);
        msg.addBoolean("wIsIrregular",304,0);
        msg.addString("wTradeQualifier",482, "closing");
        msg.addI32("wTradeSeqNum",483, 125864);
        msg.addString("wSaleCondition",450,"@");
        msg.addF64("wTradePrice",481,144.057500);
        msg.addF64("wNetChange",341,0.607500);
        msg.addF64("wPctChange",431,0.420000);
        msg.addString("wTradeUnits",487,"4DP");
        msg.addString("wTradeTick",484,"+");
        msg.addString("wTradeTime",485,"15:33:43.480");
        msg.addI64("wTradeVolume",488,100);
        msg.addU16("wMsgQual",21,1);    
    }

    void SetTradePossDupFields (MamaMsg& msg)
    {      
        msg.addU8(" MdMsgType",1, 14);
        msg.addU8("  MdMsgStatus",2,0);
        msg.addU32("  MdSeqNum",10,42868);
        msg.addU8("MamaAppMsgType",18,14);
        msg.addU64("MamaSenderId",20,68471603);
        msg.addI32("wEntitleCode",496,64);
        msg.addString("wLineTime",1174,"15:33:43.484");
        msg.addString("wSrcTime",465,"15:33:43.480");
        msg.addString("wIssueSymbol",305,"SPY");
        msg.addString("wTradePartId",480,"D");
        msg.addI64(" wTotalVolume",475,113561846);
        msg.addF64("wTotalValue",474,16356911349.500002);
        msg.addF64("wVwap",494,144.035272);
        msg.addI32("wTradeCount",901,879756);
        msg.addBoolean("wIsIrregular",304,0);
        msg.addString("wTradeQualifier",482, "Normal");
        msg.addI32("wTradeSeqNum",483, 1);
        msg.addString("wSaleCondition",450,"@");
        msg.addF64("wTradePrice",481,144.057500);
        msg.addF64("wNetChange",341,0.607500);
        msg.addF64("wPctChange",431,0.420000);
        msg.addString("wTradeUnits",487,"4DP");
        msg.addString("wTradeTick",484,"+");
        msg.addString("wTradeTime",485,"15:33:43.480");
        msg.addI64("wTradeVolume",488,100);
        msg.addU16("wMsgQual",21,1);    
    }
      
    void SetQuoteRecapFields (MamaMsg& msg)
    {     
        msg.addU8("MdMsgType", 1, 6);  
        msg.addU8("MdMsgStatus",2, 0); 
        msg.addU32("MdSeqNum", 10, 3849);    
        msg.addU8("MamaAppMsgType", 18, 13);
        msg.addU64("MamaSenderId", 20, 6847160); 
        msg.addI32("wEntitleCode",496,64);
        msg.addString("wLineTime", 1174,"14:40:33.769");
        msg.addString("wSrcTime", 465,"14:40:33.760");  
        //msg.addString("wEventTime", 1696," 14:40:33.760");  
        msg.addString("wIssueSymbol", 305, "SPY");      
        msg.addString("wBidPartId",236, "P");   
        msg.addPrice("wBidPrice",237, 143.950000);
        msg.addI64("wBidSize",238, 736);
        msg.addF64("wAskPrice",109, 143.960000);
        msg.addI64("wAskSize",110,331);
        msg.addString("wQuoteQualifier",440,"normal");
        msg.addString("wAskPartId",108,"P");
        msg.addString("wQuoteTime",442,"14:40:33.760");
        msg.addI32("wQuoteSeqNum",441,18);
        msg.addI32("wQuoteCount",1034,469136);
        msg.addString("wCondition",243, "R");      
        msg.addU16("wMsgQual",21,1);    
    }

    void SetQuoteUpdateFields (MamaMsg& msg)
    {      
        msg.addU8("MdMsgType", 1, 13);  
        msg.addU8("MdMsgStatus",2, 0); 
        msg.addU32("MdSeqNum", 10, 3849);    
        msg.addU8("MamaAppMsgType", 18, 13);
        msg.addU64("MamaSenderId", 20, 6847160);
        msg.addI32("wEntitleCode",496,64);  
        msg.addString("wLineTime", 1174,"14:40:33.769");
        msg.addString("wSrcTime", 465,"14:40:33.760");  
        //msg.addString("wEventTime", 1696," 14:40:33.760");  
        msg.addString("wIssueSymbol", 305, "SPY");  
        msg.addString("wBidPartId",236, "P");   
        msg.addF64("wBidPrice",237, 143.950000);
        msg.addI64("wBidSize",238, 736);
        msg.addF64("wAskPrice",109, 143.960000);
        msg.addI64("wAskSize",110,331);
        msg.addString("wQuoteQualifier",440,"normal");   
        msg.addString("wAskPartId",108,"P");
        msg.addString("wQuoteTime",442,"14:40:33.760");
        msg.addI32("wQuoteSeqNum",441,18);
        msg.addI32("wQuoteCount",1034,469136);
        msg.addString("wCondition",243, "R");     
        msg.addU16("wMsgQual",21,1);     
    }
      
    void SetQuoteClosingFields (MamaMsg& msg)
    {      
        msg.addU8("MdMsgType", 1, 13);  
        msg.addU8("MdMsgStatus",2, 0); 
        msg.addU32("MdSeqNum", 10, 3849);    
        msg.addU8("MamaAppMsgType", 18, 13);
        msg.addU64("MamaSenderId", 20, 6847160); 
        msg.addI32("wEntitleCode",496,64);  
        msg.addString("wLineTime", 1174,"14:40:33.769");
        msg.addString("wSrcTime", 465,"14:40:33.760");  
        msg.addString("wIssueSymbol", 305, "SPY");      
        msg.addString("wBidPartId",236, "P");   
        msg.addF64("wBidPrice",237, 143.950000);
        msg.addI64("wBidSize",238, 736);
        msg.addF64("wAskPrice",109, 143.960000);
        msg.addI64("wAskSize",110,331);
        msg.addString("wQuoteQualifier",440,"Closing");  
        msg.addString("wAskPartId",108,"P");
        msg.addString("wQuoteTime",442,"14:40:33.760");
        msg.addI32("wQuoteSeqNum",441,18);
        msg.addI32("wQuoteCount",1034,469136);
        msg.addString("wCondition",243, "R");  
        msg.addF64("wBidClose",504, 999.950000);
        msg.addF64("wAskClose",502, 2345.960000);      
        msg.addU16("wMsgQual",21,1);    
    }

    void SetQuoteOutOfSeqFields (MamaMsg& msg)
    {      
        msg.addU8("MdMsgType", 1, 13);  
        msg.addU8("MdMsgStatus",2, 0); 
        msg.addU32("MdSeqNum", 10, 3849);    
        msg.addU8("MamaAppMsgType", 18, 13);
        msg.addU64("MamaSenderId", 20, 6847160); 
        msg.addI32("wEntitleCode",496,64);    
        msg.addString("wLineTime", 1174,"14:40:11.769");    
        msg.addString("wSrcTime", 465,"14:40:11.760");  
        msg.addString("wIssueSymbol", 305, "SPY");      
        msg.addString("wBidPartId",236, "P");   
        msg.addF64("wBidPrice",237, 143.950000);
        msg.addI64("wBidSize",238, 736);
        msg.addF64("wAskPrice",109, 143.960000);
        msg.addI64("wAskSize",110,331);
        msg.addString("wQuoteQualifier",440,"normal");  
        msg.addString("wAskPartId",108,"P");
        msg.addString("wQuoteTime",442,"14:40:33.760");
        msg.addI32("wQuoteSeqNum",441,18);
        msg.addI32("wQuoteCount",1034,469136);
        msg.addString("wCondition",243, "R");  
        msg.addU16("wMsgQual",21,16);    
    }  
      
    void SetQuotePossDupFields (MamaMsg& msg)
    {      
        msg.addU8("MdMsgType", 1, 13);  
        msg.addU8("MdMsgStatus",2, 0); 
        msg.addU32("MdSeqNum", 10, 3849);    
        msg.addU8("MamaAppMsgType", 18, 13);
        msg.addU64("MamaSenderId", 20, 6847160); 
        msg.addI32("wEntitleCode",496,64);       
        msg.addString("wLineTime", 1174,"14:40:11.769");
        msg.addString("wSrcTime", 465,"14:40:11.760");  
        msg.addString("wIssueSymbol", 305, "SPY"); 
        msg.addString("wBidPartId",236, "P");   
        msg.addF64("wBidPrice",237, 143.950000);
        msg.addI64("wBidSize",238, 736);
        msg.addF64("wAskPrice",109, 143.960000);
        msg.addI64("wAskSize",110,331);
        msg.addString("wQuoteQualifier",440,"normal");
        msg.addString("wAskPartId",108,"P");
        msg.addString("wQuoteTime",442,"14:40:33.760");
        msg.addI32("wQuoteSeqNum",441,18);
        msg.addI32("wQuoteCount",1034,469136);
        msg.addString("wCondition",243, "R");  
        msg.addU16("wMsgQual",21,1);    
    }
      
    void SetQuoteGapFields1 (MamaMsg& msg)
    {      
        msg.addU8("MdMsgType",1, 13);
        msg.addU8("MdMsgStatus",2,0);
        msg.addU32("MdSeqNum",10,1);
        msg.addU8("MamaAppMsgType",18,14);
        msg.addU64("MamaSenderId",20,68471603);
        msg.addI32("wEntitleCode",496,64);
        msg.addString("wLineTime",1174,"15:33:43.484");
        msg.addString("wSrcTime",465,"15:33:43.480");
        msg.addString("wIssueSymbol",305,"SPY");
        msg.addI64(" wTotalVolume",475,113561846);
        msg.addF64("wTotalValue",474,16356911349.500002);  
        msg.addI32("wQuoteCount",1034,1);  
        msg.addI32("wQuoteSeqNum",483, 1);     
    }
     
    void SetQuoteGapFields2 (MamaMsg& msg)
    {      
        msg.addU8("MdMsgType",1, 13);
        msg.addU8("MdMsgStatus",2,0);
        msg.addU32("MdSeqNum",10,3);
        msg.addU8("MamaAppMsgType",18,14);
        msg.addU64("MamaSenderId",20,68471603);
        msg.addI32("wEntitleCode",496,64);
        msg.addString("wLineTime",1174,"15:33:43.484");
        msg.addString("wSrcTime",465,"15:33:43.480");
        msg.addString("wIssueSymbol",305,"SPY");
        msg.addI64(" wTotalVolume",475,113561846);
        msg.addF64("wTotalValue",474,16356911349.500002);  
        msg.addI32("wQuoteCount",1034,3);  
        msg.addI32("wQuoteSeqNum",483, 3);    
    }
       
    void SetQuoteCommonFields (MamaMsg&       msg,
                               MamaDateTime&  val)
    {           
       
        msg.addU8("MdMsgType", 1, 13);  
        msg.addU8("MdMsgStatus",2, 0); 
        msg.addU32("MdSeqNum", 10, 3849);    
        msg.addU8("MamaAppMsgType", 18, 13);
        msg.addU64("MamaSenderId", 20, 6847160);
        msg.addString("wIssueSymbol", 305, "SPY"); 
        msg.addString("wBidPartId",236, "P");   
        msg.addString("wAskPartId",108,"P");
        msg.addString("wSrcTime", 465,"14:40:33.760");  
        msg.addDateTime("wActivityTime",102,val);
        msg.addDateTime("wLineTime", 1174,val);
        msg.addU16("wMsgQual",21,1);    
        msg.addString("wPubId",495,"cta");
        msg.addString("wSymbol",470,"SPIED");
        msg.addF64("wBidPrice",237, 143.950000);
        msg.addF64("wBidSize",238, 736);
        msg.addF64("wBidDepth", 232,20);
        msg.addDateTime("wBidCloseDate",505,val);
        msg.addPrice("wBidClose",504,12.23);
        //msg.addPrice("wPrevBidClose",433, 123.23); possibily the wrong fid too(from cta)
        //msg.addDateTime("wPrevBidCloseDate",434, val);possibily the wrong fid too(from cta)
        msg.addPrice("wBidHigh",233, 123.23);
        msg.addPrice("wBidLow",234, 123.23);
        msg.addF64("wAskPrice",109, 143.960000);
        msg.addI64("wAskSize",110,331);
        msg.addI64("wAskDepth",104,331);
        msg.addDateTime("wAskCloseDate",503, val); 
        //msg.addPrice("wPrevAskClose",506, 123.23);possibily the wrong fid too(from cta)
        //msg.addDateTime("wPrevAskCloseDate",507, val);possibily the wrong fid too(from cta)
        msg.addPrice("wAskHigh",105, 12.23);
        msg.addPrice("wAskLow",106, 12.23);
        msg.addPrice("wMidPrice",537, 12.23);
        msg.addI32("wQuoteCount",1034,469136);
        msg.addString("wQuoteQualifier",440,"normal");   
        msg.addString("wCondition",243, "R");  
        msg.addDateTime("wAskTime",1006, val);  
        msg.addDateTime("wBidTime",1005, val); 
        msg.addString("wAskIndicator",1166, "P");  
        msg.addString("wBidIndicator",1165, "P");      
        msg.addPrice("wAskYield",1431, 12.23);
        msg.addPrice("wBidYield",1430, 12.23);    
        msg.addI32("wQuoteSeqNum",441,18);
        msg.addDateTime("wQuoteTime",442, val); 
        msg.addDateTime("wQuoteDate",519, val);    
        msg.addString("wShortSaleBidTick",460, "A");   
        msg.addString("wBidSizesList",3458, "A");   
        msg.addString("wAskSizesList",3457, "A");   
        msg.addPrice("wAskClose",502, 2345.960000);      
        //msg.addI64("wAskUpdateCount",?,?); this field isn't in the datadict or 
        //config files for cta but is in quotecache in quotelistener.
        //msg.addI64("wBidUpdateCount",?,?); this field isn't in the datadict or 
        //config files for cta but is in quotecache in quotelistener.
    }

    void SetQuoteClosingFields(MamaMsg& msg,MamaDateTime& val)
    {          
        msg.addU8("MdMsgType", 1, 13);  
        msg.addU8("MdMsgStatus",2, 0); 
        msg.addU32("MdSeqNum", 10, 3849);    
        msg.addU8("MamaAppMsgType", 18, 13);
        msg.addU64("MamaSenderId", 20, 6847160);
        msg.addString("wIssueSymbol", 305, "SPY"); 
        msg.addString("wBidPartId",236, "P");   
        msg.addString("wAskPartId",108,"P");
        msg.addString("wSrcTime", 465,"14:40:33.760");  
        msg.addString("wActivityTime",102,"13:20:33.767");
        msg.addString("wLineTime", 1174,"14:40:33.769");
        msg.addU16("wMsgQual",21,1);    
        msg.addString("wPubId",495,"cta");
        msg.addF64("wBidPrice",237, 143.950000);
        msg.addI64("wBidSize",238, 736);
        msg.addF64("wBidDepth", 232,20);
        //msg.addPrice("wBidClosePrice",2542,123.23);
        msg.addDateTime("wBidCloseDate",505,val);
        msg.addPrice("wPrevBidClose",443, 123.23);
        msg.addDateTime("wPrevBidCloseDate",434, val);       
        //msg.addPrice("wBidHigh",233, 123.23);
        //msg.addPrice("wBidLow",234, 123.23);
        msg.addF64("wAskPrice",109, 143.960000);
        msg.addI64("wAskSize",110,331);
        msg.addI64("wAskDepth",104,331);
        //msg.addPrice("wAskClosePrice",2545, 123.23);
        msg.addDateTime("wAskCloseDate",503, val);  
        msg.addPrice("wPrevAskClose",506, 123.23);
        msg.addDateTime("wPrevAskCloseDate",507, val); 
        //msg.addPrice("wAskHigh",105, 12.23);
        //msg.addPrice("wAskLow",106, 12.23);
        msg.addPrice("wMidPrice",537, 12.23);
        msg.addI32("wQuoteCount",1034,469136);
        msg.addString("wQuoteQualifier",440,"Closing");   
        msg.addString("wCondition",243, "R");  
        msg.addDateTime("wAskTime",1006, val);  
        msg.addDateTime("wBidTime",1005, val); 
        msg.addString("wAskIndicator",1166, "P");  
        msg.addString("wBidIndicator",1165, "P");      
        msg.addPrice("wAskYield",1431, 12.23);
        msg.addPrice("wBidYield",1430, 12.23);    
        msg.addI32("wEventSeqNum",3036,18);
        msg.addDateTime("wEventTime",1696, val); 
        msg.addDateTime("wQuoteDate",519, val);    
        msg.addString("wShortSaleBidTick",460, "A");   
        msg.addString("wBidSizesList",3458, "A");   
        msg.addString("wAskSizesList",3457, "A");   
        msg.addF64("wBidClose",504, 999.950000);
        msg.addF64("wAskClose",502, 2345.960000);      
    }

    void createRecap (MamaMsg& msg)
    {
        //msg.createForWombatMsg();
        msg.create();
        msg.addU8("MdMsgType",1, 6);
        msg.addU8("MdMsgStatus",2,0);
        msg.addU32("MdSeqNum",10,42868);
        msg.addU8("MamaAppMsgType",18,14);
        msg.addU64("MamaSenderId",20,68471603);
        msg.addI32("wEntitleCode",496,64);
        msg.addString("wLineTime",1174,"15:33:43.484");
        msg.addString("wSrcTime",465,"15:33:43.480");
        msg.addString("wIssueSymbol",305,"SPY");
        msg.addString("wTradePartId",480,"D");
        msg.addI64(" wTotalVolume",475,113561846);
        msg.addF64("wTotalValue",474,16356911349.500002);
        msg.addF64("wVwap",494,144.035272);
        msg.addI32("wTradeCount",901,879756);
        msg.addBoolean("wIsIrregular",304,0);
        msg.addString("wTradeQualifier",482, "Normal");
        msg.addI32("wTradeSeqNum",483, 125864);
        msg.addString("wSaleCondition",450,"@");
        msg.addF64("wTradePrice",481,144.057500);
        msg.addF64("wNetChange",341,0.607500);
        msg.addF64("wPctChange",431,0.420000);
        msg.addString("wTradeUnits",487,"4DP");
        msg.addString("wTradeTick",484,"+");
        msg.addString("wTradeTime",485,"15:33:43.480");
        msg.addI64("wTradeVolume",488,100);
    }

    void createReport (MamaMsg& msg)
    {
        //msg.createForWombatMsg();
        msg.create();
        msg.addU8("MdMsgType",1, 14);
        msg.addU8("MdMsgStatus",2,0);
        msg.addU32("MdSeqNum",10,42868);
        msg.addU8("MamaAppMsgType",18,14);
        msg.addU64("MamaSenderId",20,68471603);
        msg.addI32("wEntitleCode",496,64);
        msg.addString("wLineTime",1174,"15:33:43.484");
        msg.addString("wSrcTime",465,"15:33:43.480");
        msg.addString("wIssueSymbol",305,"SPY");
        msg.addString("wTradePartId",480,"D");
        msg.addI64(" wTotalVolume",475,113561846);
        msg.addF64("wTotalValue",474,16356911349.500002);
        msg.addF64("wVwap",494,144.035272);
        msg.addI32("wTradeCount",901,1);
        msg.addBoolean("wIsIrregular",304,0);
        msg.addString("wTradeQualifier",482, "Normal");
        msg.addI32("wTradeSeqNum",483, 1);
        msg.addString("wSaleCondition",450,"@");
        msg.addF64("wTradePrice",481,144.057500);
        msg.addF64("wNetChange",341,0.607500);
        msg.addF64("wPctChange",431,0.420000);
        msg.addString("wTradeUnits",487,"4DP");
        msg.addString("wTradeTick",484,"+");
        msg.addString("wTradeTime",485,"15:33:43.480");
        msg.addI64("wTradeVolume",488,100);
    }

    void createUpdate (MamaMsg& msg)
    {
        //msg.createForWombatMsg();
        msg.create();
        msg.addU8("MdMsgType",1, 0);
        msg.addU8("MdMsgStatus",2,0);
        msg.addU32("MdSeqNum",10,42868);
        msg.addU8("MamaAppMsgType",18,14);
        msg.addU64("MamaSenderId",20,68471603);
        msg.addI32("wEntitleCode",496,64);
        msg.addString("wLineTime",1174,"15:33:43.484");
        msg.addString("wSrcTime",465,"15:33:43.480");
        msg.addString("wIssueSymbol",305,"SPY");
        msg.addString("wTradePartId",480,"D");
        msg.addI64(" wTotalVolume",475,113561846);
        msg.addF64("wTotalValue",474,16356911349.500002);
        msg.addF64("wVwap",494,144.035272);
        msg.addI32("wTradeCount",901,2);
        msg.addBoolean("wIsIrregular",304,0);
        msg.addString("wTradeQualifier",482, "Normal");
        msg.addI32("wTradeSeqNum",483, 1);
        msg.addString("wSaleCondition",450,"@");
        msg.addF64("wTradePrice",481,144.057500);
        msg.addF64("wNetChange",341,0.607500);
        msg.addF64("wPctChange",431,0.420000);
        msg.addString("wTradeUnits",487,"4DP");
        msg.addString("wTradeTick",484,"+");
        msg.addString("wTradeTime",485,"15:33:43.480");
        msg.addI64("wTradeVolume",488,100);
    }

    void createUpdateWithoutTrade (MamaMsg& msg)
    {
        //msg.createForWombatMsg();
        msg.create();
        msg.addU8("MdMsgType",1, 0);
        msg.addU8("MdMsgStatus",2,0);
        msg.addU32("MdSeqNum",10,42868);
        msg.addU8("MamaAppMsgType",18,14);
        msg.addU64("MamaSenderId",20,68471603);
        msg.addI32("wEntitleCode",496,64);
        msg.addString("wLineTime",1174,"15:33:43.484");
        msg.addString("wSrcTime",465,"15:33:43.480");
        msg.addString("wIssueSymbol",305,"SPY");
        msg.addString("wTradePartId",480,"D");
        msg.addI64(" wTotalVolume",475,113561846);
        msg.addF64("wTotalValue",474,16356911349.500002);
        msg.addF64("wVwap",494,144.035272);
        msg.addString("wSaleCondition",450,"@");
        msg.addF64("wNetChange",341,0.607500);
        msg.addF64("wPctChange",431,0.420000);
    } 
    
    void SetRecapMsgFields (MamaMsg& msg)
    {
        msg.addU8("wMdMsgType",1,19);
        msg.addU8("wMsgStatus",2,0);
        msg.addU32("wMdSeqNum",10,1);
        msg.addU8("MamaAppMsgType",18,15);
        msg.addU64("MamaSenderId",20,87657);
    }      
    
    void SetAtomicGap1MsgFields (MamaMsg& msg)
    {
        msg.addU8("wMdMsgType",1,17);
        msg.addU8("wMsgStatus",2,0);
        msg.addU32("wMdSeqNum",10,1);
        msg.addU8("MamaAppMsgType",18,15);
        msg.addU64("MamaSenderId",20,87657);
    }
    
      void SetAtomicGap2MsgFields (MamaMsg& msg)
    {
        msg.addU8("wMdMsgType",1,17);
        msg.addU8("wMsgStatus",2,0);
        msg.addU32("wMdSeqNum",10,3);
        msg.addU8("MamaAppMsgType",18,15);
        msg.addU64("MamaSenderId",20,87657);
    }
    
    void SetDeltaMsgFields (MamaMsg& msg)
    {
        msg.addU8("wMdMsgType",1,17);
        msg.addU8("wMsgStatus",2,0);
        msg.addU32("wMdSeqNum",10,1);
        msg.addU8("MamaAppMsgType",18,15);
        msg.addU64("MamaSenderId",20,87657);
    }  
    
    void SetPriceLevelEntryMsg (MamaMsg&       msg, 
                                MamaDateTime&  val)
    {
        msg.addString("wEntryId",681,"C");
        msg.addI8("wEntryAction",683,65);
        msg.addU32("wEntrySize",682,400);
        msg.addDateTime("wEntryTime",685,val);
        msg.addI8("wEntryReason",684,90);
        msg.addU16("wEntryStatus",686,82);        
    }
    
    void SetPriceLevelMsg (MamaMsg& msg)
    {
        msg.addF64("wPlPrice",653,34.810000);
        msg.addI8("wPlAction",652,85);
        msg.addI8("wPlSide",654,65);
        msg.addU32("wPlSize",655,5200);
        msg.addI32("wPlSizeChange",656,-1000);
        msg.addString("wPlTime",658,"14:22:21.969");
        msg.addU16("wPlNumEntries",657,1);
        msg.addI16("wPlNumAttach",659,1);     
    }
    
    void SetMsgFields (MamaMsg& msg)
    {
        msg.addU8("wMdMsgType",1,19);
        msg.addU8("wMsgStatus",2,0);
        msg.addU32("wMdSeqNum",10,1);
        msg.addU8("MamaAppMsgType",18,15);
        msg.addU64("MamaSenderId",20,87657);
    }
        
    /*
     * CompareTest:
     * Returns true of file1 and file2 are the same.
     * Returns false if they have differences and 
     * then generates an errorfile, file3.
     */ 
     bool CompareTest (const char*  file1,
                       const char*  file2,
                       const char*  file3)
    { 
        ifstream newTestFile;
        newTestFile.open (file1);
        if (!file1)
        {
            cout << "Cannot open file.\n";
            return 1;
        }
        
        ifstream originalTestFile;
        originalTestFile.open (file2);
        if(!file2)
        {
            cout << "Cannot open file.\n";
            return 1;
        }   
        
        ofstream ofs;  
        
        string  newTestFileLine;
        string  originalTestFileLine;
        bool    fail = false;
        long    linecount = 1;
        long    errorlinecount = 0;
        long    errorCount = 0;
        int     Count = 0;
        while (!originalTestFile.eof())
        {
            while (!newTestFile.eof())
            {
                getline (newTestFile,newTestFileLine);
                getline (originalTestFile, originalTestFileLine);        
                
                if (newTestFileLine.compare(originalTestFileLine) == 0)
                {
                    // Do nothing
                }
                else
                {   
                    Count++;
                    if(Count == 1)                    
                    {
                        ofs.open(file3);
                        if(!file3)
                        {
                            cout << "Cannot open file.\n";
                            return 1;
                        }    
                    }      
                    
                    ofs << "Error " << errorCount <<": Line Number = " << linecount << "\n";
                    ofs << "New Test Line " << newTestFileLine;
                    ofs << "\n";
                    ofs << "Original Line " << originalTestFileLine;
                    ofs << "\n";
                    fail = true;
                    errorCount++;
                }   
                linecount++;
            }    
        } 
        
        ofs.close();
        newTestFile.close();
        originalTestFile.close();    
        
        return fail;
    }

    void resetMsg (MamaMsg& mMsg)
    {    
        short msgType   = mMsg.getType();
        short msgStatus = mMsg.getStatus();
        int   seqNum    = mMsg.getSeqNum();
        
        mMsg.clear();       
        addMamaHeaderFields (mMsg,
                             msgType,
                             msgStatus,
                             seqNum);  
    }   

    MamaMsg* createBlankMsg (short  msgType, 
                             short  msgStatus, 
                             int    seqNum)
    {
        MamaMsg* mMsg = NULL;
        mMsg = new MamaMsg();
        mMsg->create();
        
        addMamaHeaderFields (*mMsg,
                              msgType,
                              msgStatus,
                              seqNum);    
        return mMsg;
    }

    MamaMsg* createBookMsgOneSide (mamaMsgType  type, 
                                   int          depth)
    {
        MamaMsg* msg = new MamaMsg();
        msg->create();
        int vectorSize = depth;
        
        addMamaHeaderFields (*msg,
                              type,
                              MAMA_MSG_STATUS_OK,
                              1);    

        MamdaOrderBookPriceLevel::Side side;
        side = MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK;
        
        //set up entry Action
        MamdaOrderBookEntry::Action entryAction;  
        if (type == MAMA_MSG_TYPE_BOOK_INITIAL)
            entryAction = MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD;
        else
            entryAction = MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UPDATE;
        
        //set up plAction
        MamdaOrderBookPriceLevel::Action plAction;  
        if (type == MAMA_MSG_TYPE_BOOK_INITIAL)
            plAction = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD;
        else
            plAction = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE;
        
        //set up price
        MamaPrice* price = new MamaPrice();
        price->setValue (100.0);    
        
        MamaMsg** vectorMsg = new MamaMsg*[vectorSize];

        for (int i=0; i< vectorSize; i++)
        {
            vectorMsg[i] = new MamaMsg;
            vectorMsg[i]->create();     
            
            MamaMsg* plMsg = vectorMsg[i];

            plMsg->addPrice(NULL, 653, *price);
            plMsg->addU32(NULL, 655, 1000);
            plMsg->addI32(NULL, 656, 1000);
            plMsg->addI8(NULL, 652, plAction);
            
            plMsg->addString(NULL, 681, "P");
            plMsg->addI8(NULL, 683, entryAction);
            plMsg->addI32(NULL, 682, 1000);
            plMsg->addI8(NULL, 654, side);
            
            price->setValue(price->getValue() - 1);
        }

        MamaDateTime* now = new MamaDateTime();
        now->setToNow();
        
        msg->addVectorMsg (NULL, 699, vectorMsg, vectorSize);    
        msg->addI16(NULL,651, vectorSize);
        msg->addString(NULL, 470, "AA");
        msg->addString(NULL, 429, "NYS");
        msg->addU32(NULL, 498, 1);
        msg->addDateTime(NULL, 671, *now);  
        
        //printf("\n createBookMsg: Msg = %s \n", msg->toString());
        return msg;
    }


    MamaMsg* createBookMsg (mamaMsgType  type, 
                            int          depth)
    {
        MamaMsg* msg = new MamaMsg();
        msg->create();
        int vectorSize = depth*2;
        
        addMamaHeaderFields (*msg,
                              type,
                              MAMA_MSG_STATUS_OK,
                              1);    

        MamdaOrderBookPriceLevel::Side side;
        side = MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK;
        
        //set up entry Action
        MamdaOrderBookEntry::Action entryAction;  
        if (type == MAMA_MSG_TYPE_BOOK_INITIAL)
            entryAction = MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD;
        else
            entryAction = MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UPDATE;
        
        //set up plAction
        MamdaOrderBookPriceLevel::Action plAction;  
        if (type == MAMA_MSG_TYPE_BOOK_INITIAL)
            plAction = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD;
        else
            plAction = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE;
        
        //set up price
        MamaPrice* price = new MamaPrice();
        price->setValue (100.0);    
        
        MamaMsg** vectorMsg = new MamaMsg*[vectorSize];

        for (int i=0; i< vectorSize; i++)
        {
            vectorMsg[i] = new MamaMsg;
            vectorMsg[i]->create();     
            
            MamaMsg* plMsg = vectorMsg[i];
       
            if (i == depth)
            {
                side = MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID;
                price->setValue(100.0);
            }        
            
            plMsg->addPrice(NULL, 653, *price);
            plMsg->addU32(NULL, 655, 1000);
            plMsg->addI32(NULL, 656, 1000);
            plMsg->addI8(NULL, 652, plAction);
            
            plMsg->addString(NULL, 681, "P");
            plMsg->addI8(NULL, 683, entryAction);
            plMsg->addI32(NULL, 682, 1000);
            plMsg->addI8(NULL, 654, side);
            
            price->setValue(price->getValue() - 1);
        }

        MamaDateTime* now = new MamaDateTime();
        now->setToNow();
        
        msg->addVectorMsg (NULL, 699, vectorMsg, vectorSize);    
        msg->addI16(NULL,651, vectorSize);
        msg->addString(NULL, 470, "AA");
        msg->addString(NULL, 429, "NYS");
        msg->addU32(NULL, 498, 1);
        msg->addDateTime(NULL, 671, *now);  
        
        //printf("\n createBookMsg: Msg = %s \n", msg->toString());
        return msg;
    }


    MamaMsg* createBookMsg (mamaMsgType  type, 
                            int          depth, 
                            int          numEntriesPerLevel)
    {
        MamaMsg* msg = new MamaMsg();
        msg->create();
        int vectorSize = depth*2;

        
        MamaMsg** entryVectorMsg = new MamaMsg*[numEntriesPerLevel];

        for (int i=0; i< numEntriesPerLevel; i++)
        {
            entryVectorMsg[i] = new MamaMsg;
            entryVectorMsg[i]->create();    
        }
       
        addMamaHeaderFields(*msg,
                             type,
                             MAMA_MSG_STATUS_OK,
                             1);    

        MamdaOrderBookPriceLevel::Side side;
        side = MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_ASK;
        
        // set up entry Action
        MamdaOrderBookEntry::Action entryAction;  
        //if (type == MAMA_MSG_TYPE_BOOK_INITIAL)
        //    entryAction = MamdaOrderBookEntry::MAMDA_BOOK_ACTION_ADD;
        //else
            entryAction = MamdaOrderBookEntry::MAMDA_BOOK_ACTION_UPDATE;
        
        // set up plAction
        MamdaOrderBookPriceLevel::Action plAction;  
        //if (type == MAMA_MSG_TYPE_BOOK_INITIAL)
        //    plAction = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_ADD;
        //else
            plAction = MamdaOrderBookPriceLevel::MAMDA_BOOK_ACTION_UPDATE;
        
        // set up price
        MamaPrice* price = new MamaPrice();
        price->setValue (100.0);    
        
        MamaMsg** vectorMsg = new MamaMsg*[vectorSize];

        for (int i=0; i< vectorSize; i++)
        {
            vectorMsg[i] = new MamaMsg;
            vectorMsg[i]->create();     
            
            MamaMsg* plMsg = vectorMsg[i];
       
            if (i == depth)
            {
                side = MamdaOrderBookPriceLevel::MAMDA_BOOK_SIDE_BID;
                price->setValue(100.0);
            }        
            
            plMsg->addPrice(NULL, 653, *price);
            plMsg->addU32(NULL, 655, 1000);
            plMsg->addI32(NULL, 656, 1000);
            plMsg->addI8(NULL, 652, plAction);
            
            for (int i=0; i< numEntriesPerLevel; i++)
            {            
                MamaMsg* entMsg = entryVectorMsg[i];
                
                entMsg->clear();

                std::string s;
                std::stringstream out;
                out << i;
                s = out.str(); 
                
                entMsg->addString(NULL, 681, s.c_str() );
                entMsg->addI8(NULL, 683, entryAction);
                entMsg->addI32(NULL, 682, 1000);
                entMsg->addI8(NULL, 654, side);           
            } 

            plMsg->addVectorMsg (NULL, 700, entryVectorMsg, numEntriesPerLevel);    
            
            price->setValue(price->getValue() - 1);
        }

        MamaDateTime* now = new MamaDateTime();
        now->setToNow();
        
        msg->addVectorMsg (NULL, 699, vectorMsg, vectorSize);    
        msg->addI16(NULL,651, vectorSize);
        msg->addString(NULL, 470, "AA");
        msg->addString(NULL, 429, "NYS");
        msg->addU32(NULL, 498, 1);
        msg->addDateTime(NULL, 671, *now);  
        
        //printf("\n createBookMsg Entries: Msg = %s \n", msg->toString());
        return msg;
    }

    bool fileExists (const char* fileName)
    {
	    struct stat buf;
	    int i = stat (fileName, &buf);
	    if (i == 0)
		    return 1;
	    else return 0;
    }

    void cleanup (playbackImpl* impl)
    {
        if (impl)
        {
	        if (impl->myParser)
	        {
		        mamaPlaybackFileParser_closeFile (impl->myParser);
		        mamaPlaybackFileParser_deallocate (impl->myParser);   
		        impl->myParser = NULL;
	        }
	        free (impl);
        }
    }

    void initializeQuoteControlCache (QuoteControlCache& quoteControlCache)
    {
        // The following fields are used for caching the offical quote and
        // related fields.  These fields can be used by applications for
        // reference and will be passed for recaps.
        quoteControlCache.mySymbol = "SPY";                     quoteControlCache.mySymbolFieldState = 2;   
        //    myPartId;                             myPartIdFieldState;   
        quoteControlCache.mySrcTime = "14:40:33.760";           quoteControlCache.mySrcTimeFieldState = 2;  
        quoteControlCache.myActTime = "2010-02-02";             quoteControlCache.myActTimeFieldState = 2;  
        quoteControlCache.myLineTime = "2010-02-02";            quoteControlCache.myLineTimeFieldState = 2; 
        //    mySendTime;                           mySendTimeFieldState; 
        quoteControlCache.myPubId = "cta";                      quoteControlCache.myPubIdFieldState = 2;
        quoteControlCache.myBidPrice = 143.950000;              quoteControlCache.myBidPriceFieldState = 2; 
        quoteControlCache.myBidSize = 736;                      quoteControlCache.myBidSizeFieldState =2;
        quoteControlCache.myBidDepth = 20;                      quoteControlCache.myBidDepthFieldState = 2;
        quoteControlCache.myBidPartId ="P";                     quoteControlCache.myBidPartIdFieldState = 2;
        quoteControlCache.myBidClosePrice = 12.23;              quoteControlCache.myBidClosePriceFieldState = 2; 
        quoteControlCache.myBidCloseDate = "2010-02-02";        quoteControlCache.myBidCloseDateFieldState = 2;

        quoteControlCache.myBidPrevClosePrice = 123.23;         quoteControlCache.myBidPrevClosePriceFieldState = 2;
        quoteControlCache.myBidPrevCloseDate = "2010-02-02";    quoteControlCache.myBidPrevCloseDateFieldState = 2; 
        quoteControlCache.myBidHigh = 123.23;                   quoteControlCache.myBidHighFieldState = 2;          
        quoteControlCache.myBidLow = 123.23;                    quoteControlCache.myBidLowFieldState = 2;           
        quoteControlCache.myAskPrice = 143.960000;              quoteControlCache.myAskPriceFieldState = 2;         
        quoteControlCache.myAskSize = 331;                      quoteControlCache.myAskSizeFieldState = 2;          
        quoteControlCache.myAskDepth = 331;                     quoteControlCache.myAskDepthFieldState = 2;         
        quoteControlCache.myAskPartId = "P";                    quoteControlCache.myAskPartIdFieldState = 2;        
        quoteControlCache.myAskClosePrice = 2345.960000;        quoteControlCache.myAskClosePriceFieldState = 2;     
        quoteControlCache.myAskCloseDate = "2010-02-02";        quoteControlCache.myAskCloseDateFieldState = 2;     
        quoteControlCache.myAskPrevClosePrice = 123.23;         quoteControlCache.myAskPrevClosePriceFieldState = 2;
        quoteControlCache.myAskPrevCloseDate = "2010-02-02";    quoteControlCache.myAskPrevCloseDateFieldState = 2; 
        quoteControlCache.myAskHigh = 12.23;                    quoteControlCache.myAskHighFieldState = 2;          
        quoteControlCache.myAskLow =  12.23;                    quoteControlCache.myAskLowFieldState = 2;           
        //         myMidPrice;                           myMidPriceFieldState = 2;
        quoteControlCache.myEventSeqNum = 18;                   quoteControlCache.myEventSeqNumFieldState = 2;      
        quoteControlCache.myEventTime = "2010-02-02";           quoteControlCache.myEventTimeFieldState = 2;        
        quoteControlCache.myQuoteDate = "2010-02-02";           quoteControlCache.myQuoteDateFieldState = 2;        
        quoteControlCache.myQuoteQualStr = "normal";            quoteControlCache.myQuoteQualStrFieldState = 2;     
        quoteControlCache.myQuoteQualNative = "R";              quoteControlCache.myQuoteQualNativeFieldState = 2;  

        quoteControlCache.myAskTime = "2010-02-02";             quoteControlCache.myAskTimeFieldState = 2;          
        quoteControlCache.myBidTime = "2010-02-02";             quoteControlCache.myBidTimeFieldState = 2;          
        quoteControlCache.myAskIndicator = "P";                 quoteControlCache.myAskIndicatorFieldState = 2;     
        quoteControlCache.myBidIndicator = "P";                 quoteControlCache.myBidIndicatorFieldState = 2;     
        //mama_u32_t      myAskUpdateCount = 1;                 myAskUpdateCountFieldState = 2;   
        //mama_u32_t      myBidUpdateCount = 1;                 myBidUpdateCountFieldState = 2;   
        quoteControlCache.myAskYield = 12.23;                   quoteControlCache.myAskYieldFieldState = 2;         
        quoteControlCache.myBidYield = 12.23;                   quoteControlCache.myBidYieldFieldState = 2;         

        //mama_u32_t      myQuoteCount;                         myQuoteCountFieldState = 2;
        //  myGapBegin;                           myGapBeginFieldState = 2;         
        //  myGapEnd;                             myGapEndFieldState = 2;           
        quoteControlCache.myBidSizesList = "A";                 quoteControlCache.myBidSizesListFieldState = 2;
        quoteControlCache.myAskSizesList = "A";                 quoteControlCache.myAskSizesListFieldState = 2;

        quoteControlCache.myShortSaleBidTick = 'A';             quoteControlCache.myShortSaleBidTickFieldState = 2;//doesnt getupdated in quotelistener.
    }

    void initializeQuoteControlFields (QuoteControlFields&  quoteControlFields)
    {
        quoteControlFields.mySymbol = "SPY";
        quoteControlFields.myQuoteCount = 469136;
        quoteControlFields.myBidPriceStr   = "143.95";
        quoteControlFields.myBidClosePriceStr = "999.95";
        quoteControlFields.myBidSize  = 736;
        quoteControlFields.myAskSize = 331;
        quoteControlFields.myAskPriceStr = "143.96";
        quoteControlFields.myAskClosePriceStr = "2345.96";
        quoteControlFields.myEventSeqNum = 18;
        quoteControlFields.myEventTimeStr = "14:40:33.760";
        quoteControlFields.myQuoteQualStr = "normal";
        quoteControlFields.myMsgQualStr = "Out of sequence";
        quoteControlFields.myMsgQual = 16 ;
        quoteControlFields.myMsgQualPossDupStr = "Possibly duplicate";
        quoteControlFields.myMsgQualPossDup = 1 ;       
        quoteControlFields.myGapBeginSeqNum = 2;
        quoteControlFields.myEndGapSeqNum   = 2;       
    }

    void initializeTradeControlFields (TradeControlFields&  tradeControlFields)
    {
        tradeControlFields.mySymbol = "SPY";      
        tradeControlFields.myTradeCount = 1;
        tradeControlFields.myTradeVolume = 100;
        tradeControlFields.myTradePrice = "144.058";
        tradeControlFields.myTradeEventSeqNum = 1;
        tradeControlFields.myTradeEventTime = "15:33:43.480";
        tradeControlFields.myTradeActivityTime = "13:20:33.767";
        tradeControlFields.myTradePartId = "D";
        tradeControlFields.myTradeQual = "Normal";
        tradeControlFields.myTradeQualNative = "@";
        tradeControlFields.myTradeIsIrregular = 0 ;     
        tradeControlFields.myTradeMsgQualStr = "Out of sequence";
        tradeControlFields.myTradeMsgQual = 16;
        tradeControlFields.myTradeMsgQualPossDupStr = "Possibly duplicate";
        tradeControlFields.myTradeMsgQualPossDup = 1;
        tradeControlFields.myAccVolume = 113561846;
    }

} // namespace
