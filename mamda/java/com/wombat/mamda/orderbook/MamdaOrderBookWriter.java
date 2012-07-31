/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2012 NYSE Technologies, Inc.
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

package com.wombat.mamda.orderbook;

import com.wombat.mamda.*;
import com.wombat.mama.*;
import java.util.*; 

class MamdaOrderBookWriter 
{
    public BookMsgHolder  mPriceLevels              = null;
    public BookMsgHolder  mEntries                  = null; 

    public static char    defaultPlAction           = MamdaOrderBookTypes.MAMDA_BOOK_ACTION_ADD; 
    public static char    defaultPlSide             = MamdaOrderBookTypes.MAMDA_BOOK_SIDE_BID;
    public static char    defaultEntryAction        = MamdaOrderBookTypes.MAMDA_BOOK_ACTION_DELETE;
    public static int     defaultNumEntries         = 1;
    public static int     defaultNumAttachedEntries = 1;

    public MamdaOrderBookWriter()
    {
        mPriceLevels = new BookMsgHolder();
        mEntries = new BookMsgHolder();   
    }

    public void BookWriterClear()
    {
        mPriceLevels.clear();
        mEntries.clear();
    }  

    public void populateMsg(MamaMsg msg, MamdaOrderBookComplexDelta delta)
    {        
        mPriceLevels.clear();
        mEntries.clear(); 
        msg.updateU8 (null,MamaReservedFields.MsgType.getId(),MamaMsgType.TYPE_BOOK_UPDATE);
        addComplexDeltaFields (msg, delta);     
    }

    public void populateMsg(MamaMsg msg, MamdaOrderBookSimpleDelta delta)
    {
        mPriceLevels.clear();
        mEntries.clear();

        msg.updateU8 (null,MamaReservedFields.MsgType.getId(),MamaMsgType.TYPE_BOOK_UPDATE);
        addSimpleDeltaFields (msg, delta);
    }

    public void populateMsg(MamaMsg msg, MamdaOrderBook book)
    {
        mPriceLevels.clear();
        mEntries.clear();
        msg.updateU8 (null,MamaReservedFields.MsgType.getId(),MamaMsgType.TYPE_BOOK_INITIAL);
        addBookLevels (msg,book);    
    } 
   
    public void addSimpleDeltaFields (
        MamaMsg                      msg,
        MamdaOrderBookSimpleDelta    delta)
    { 
        MamdaOrderBookPriceLevel pl = delta.getPriceLevel();
        short entryCount = 0;
        mPriceLevels.grow(1);
        
        addBookLevel (msg, pl,
                      delta.getPlDeltaSize(),
                      delta.getPlDeltaAction(),
                      delta.getEventTime());

        if (delta.getEntry() != null)
        {
            entryCount = 1; 
            addBookEntry (msg,
                          delta.getEntry(),
                          delta.getEntryDeltaAction(),
                          pl.getTime());
        }

        msg.addI16 (null, MamdaOrderBookFields.NUM_LEVELS.getFid(),
                    (short)1);

        if (defaultNumAttachedEntries != entryCount)
        {
            msg.addU32 (null,
                        MamdaOrderBookFields.PL_NUM_ATTACH.getFid(),
                        (long)entryCount);
        }

        if (delta.getEventTime() != null)
            msg.addDateTime (null, MamdaOrderBookFields.BOOK_TIME.getFid(),
                             delta.getEventTime());

        if (delta.getSrcTime() != null)
            msg.addDateTime (null, MamdaCommonFields.SRC_TIME.getFid(),
                             delta.getSrcTime());

        if (delta.getEventSeqNum() != 0)
        {
            msg.addI64 (null, MamdaCommonFields.SEQ_NUM.getFid(),
                        (long)delta.getEventSeqNum());
        }              
    }

    public void addComplexDeltaFields (
        MamaMsg                      msg,
        MamdaOrderBookComplexDelta   delta)
    {
        // Make sure the reusable price level and entry submessage vectors
        // are large enough.  In the worst case, we would have one entry
        // in each of "delta-size" price levels or "delta-size" entries
        // within one price level, so let's make sure both vectors are
        // large enough.

        mPriceLevels.grow ((int)delta.getSize());
        mEntries.grow     ((int)delta.getSize());    

        Iterator  itr = delta.iterator();      
        
        MamdaOrderBookBasicDelta  basicDelta       = null;
        MamdaOrderBookBasicDelta  savedBasicDelta  = null;
        MamdaOrderBookPriceLevel  pl               = null;
        MamdaOrderBookPriceLevel  lastPl           = null;
        
        MamdaOrderBookPriceLevel  flatEntryPl      = null;
        MamdaOrderBookBasicDelta  flatEntryDelta   = null;
        
        int  plCount    = 0;
        int  entryCount = 0;
        
        while (itr.hasNext())
        {
            basicDelta =(MamdaOrderBookBasicDelta) itr.next(); 
            pl = basicDelta.getPriceLevel();

            if (lastPl != pl)
            {
                if (lastPl != null)
                {   
                    MamaMsg plMsg = mPriceLevels.mMsgVector[plCount];

                    addBookLevel (plMsg, 
                                  lastPl, 
                                  savedBasicDelta.getPlDeltaSize(), 
                                  savedBasicDelta.getPlDeltaAction(),
                                  delta.getEventTime());                     
                                    
                    if (1 == entryCount)
                    {  
                        if (flatEntryDelta.getEntry() != null)
                        {
                            addBookEntry (plMsg,
                                          flatEntryDelta.getEntry(),
                                          flatEntryDelta.getEntryDeltaAction(), 
                                          flatEntryPl.getTime());       
                        }
                    }
                    if (entryCount > 1)
                    {
                        plMsg.addArrayMsgWithLength (null,
                                                     MamdaOrderBookFields.PL_ENTRIES.getFid(),
                                                     mEntries.mMsgVector, entryCount);                       
                    
                    }
                    if (defaultNumAttachedEntries != entryCount)
                    { 
                        plMsg.addU32 (null,
                                      MamdaOrderBookFields.PL_NUM_ATTACH.getFid(),
                                      (long) entryCount);
                    }
                    ++plCount;
                    entryCount = 0;
                }
                lastPl = pl;
                savedBasicDelta = basicDelta;                
            }
            if (basicDelta.getEntry() != null)
            {                   
                MamaMsg entMsg = mEntries.mMsgVector[entryCount];  

                addBookEntry (entMsg,
                              basicDelta.getEntry(),
                              basicDelta.getEntryDeltaAction(), 
                              pl.getTime());                

                ++entryCount;   
                // save this delta and pl
                // used to flatten entry info into pl when no. of entries = 1
                flatEntryDelta = basicDelta;
                flatEntryPl = pl;               
            }
        }    

        // Add the last entry vector and other price level fields to the
        // last price level message.   

        MamaMsg plMsg = mPriceLevels.mMsgVector[plCount];

        addBookLevel (plMsg, pl,
                      savedBasicDelta.getPlDeltaSize(),
                      savedBasicDelta.getPlDeltaAction(),
                      delta.getEventTime());

        // flatten
        if (1 == entryCount)
        {          
            addBookEntry (plMsg, basicDelta.getEntry(),
                          basicDelta.getEntryDeltaAction(),
                          pl.getTime());
        }
                    
        if (entryCount > 1)
        { 
            plMsg.addArrayMsgWithLength (null,
                                         MamdaOrderBookFields.PL_ENTRIES.getFid(),
                                         mEntries.mMsgVector, entryCount);
        }        
        if (defaultNumAttachedEntries != entryCount)
        {
            plMsg.addU32 (null,
                          MamdaOrderBookFields.PL_NUM_ATTACH.getFid(),
                          entryCount);   
        }

        ++plCount;

        // flatten - in case of one pricelevel
        if (1 == plCount)
        {    
            addBookLevel (msg, pl,
                          basicDelta.getPlDeltaSize(),
                          basicDelta.getPlDeltaAction(),
                          delta.getEventTime());

            if (basicDelta.getEntry() != null)
            {
                if (1 == entryCount)
                {
                    addBookEntry (msg, basicDelta.getEntry(),
                                  basicDelta.getEntryDeltaAction(),
                                  pl.getTime());
                }
            }               

            if (entryCount > 1)
            {   
                msg.addArrayMsgWithLength (null,
                                           MamdaOrderBookFields.PL_ENTRIES.getFid(),
                                           mEntries.mMsgVector,entryCount);  

                if (defaultNumAttachedEntries != entryCount)
                {           
                    msg.addU32 (null,
                                MamdaOrderBookFields.PL_NUM_ATTACH.getFid(),
                                entryCount);
                }                        
            }
        }
        
        // Add the vector of plMsgs to the main msg if plCount > 1
        if (plCount > 1)
        {              
            msg.addArrayMsgWithLength (null, MamdaOrderBookFields.PRICE_LEVELS.getFid(),
                                       mPriceLevels.mMsgVector,plCount); 
        }

        msg.addU32 (null, MamdaOrderBookFields.NUM_LEVELS.getFid(),
                   (int)plCount);             
        
        if (delta.getEventTime() != null)
        msg.addDateTime (null, MamdaOrderBookFields.BOOK_TIME.getFid(),
                         delta.getEventTime());
        
        if (delta.getSrcTime() != null)
        msg.addDateTime (null, MamdaCommonFields.SRC_TIME.getFid(),
                         delta.getSrcTime());

        if (delta.getEventSeqNum() != 0)
        {
            msg.addI64 (null, MamdaCommonFields.SEQ_NUM.getFid(),
                       (long)delta.getEventSeqNum());
        }   
    }   

    public void addBookLevel (
        MamaMsg                   msg,
        MamdaOrderBookPriceLevel  level,
        double                    plDeltaSize,
        char                      plDeltaAction,
        MamaDateTime              bookTime)
    {             
        msg.addF64(null, MamdaOrderBookFields.PL_PRICE.getFid(),
                   level.getPrice().getValue());
        
        if (defaultPlAction != plDeltaAction)
        {  
            msg.addChar (null, MamdaOrderBookFields.PL_ACTION.getFid(),
                         plDeltaAction);          
        }

        if (defaultPlSide != (char)level.getSide())
        {
            msg.addI8 (null, MamdaOrderBookFields.PL_SIDE.getFid(),
                       (byte)level.getSide());
        }
            
        msg.addU32  (null, MamdaOrderBookFields.PL_SIZE.getFid(), 
                    (long)level.getSize());        
        
        msg.addU32  (null, MamdaOrderBookFields.PL_SIZE_CHANGE.getFid(),
                    (int)level.getSizeChange());
      
        if (level.getTime() != null)
        {
            if (null == bookTime ) 
            {                
                msg.addDateTime (null, MamdaOrderBookFields.PL_TIME.getFid(),
                                 level.getTime());   
            }      
        }

        if (bookTime != null)
        {
            if(bookTime.compareTo(level.getTime())!=0)
            {                
                msg.addDateTime (null, MamdaOrderBookFields.PL_TIME.getFid(),
                                 level.getTime());   
            }
        }
               
        if (defaultNumEntries != level.getNumEntries())
        {
            msg.addU16 (null, MamdaOrderBookFields.PL_NUM_ENTRIES.getFid(),
                       (int)level.getNumEntries());
        }   
    }

    public void addBookLevels (
        MamaMsg             msg,
        MamdaOrderBook      book)
    {
        long numLevels = book.getTotalNumLevels();

        if (0 == numLevels)
        {
            // Just add a zero NumLevels field
            msg.addI16 (null, MamdaOrderBookFields.NUM_LEVELS.getFid(),
                        (short)0);
            return;
        }

        mPriceLevels.grow ((int)numLevels);                 
        int plCount = 0;

        // Add order book fields
        Iterator mItrBid = book.bidIterator();

        while (mItrBid.hasNext())
        {
            MamdaOrderBookPriceLevel pl = (MamdaOrderBookPriceLevel) mItrBid.next();
            MamaMsg  plMsg = mPriceLevels.mMsgVector[plCount];

            addBookLevel (plMsg, pl, 0.0,
                          MamdaOrderBookPriceLevel.ACTION_ADD,
                          book.getBookTime());            
 
            addBookLevelEntries (plMsg, pl);           
            ++plCount;
        }
    
        Iterator mItrAsk = book.askIterator();

        while (mItrAsk.hasNext())
        {
            MamdaOrderBookPriceLevel pl = (MamdaOrderBookPriceLevel) mItrAsk.next();
            MamaMsg  plMsg = mPriceLevels.mMsgVector[plCount];

            addBookLevel (plMsg, pl, 0.0,
                          MamdaOrderBookPriceLevel.ACTION_ADD,
                          book.getBookTime());

            addBookLevelEntries (plMsg, pl);
            ++plCount;
        }           

        // Add the vector of plMsgs to the main msg
        msg.addArrayMsgWithLength (null, MamdaOrderBookFields.PRICE_LEVELS.getFid(),
                                   mPriceLevels.mMsgVector,plCount);
                
        msg.addI16 (null, MamdaOrderBookFields.NUM_LEVELS.getFid(),
                    (short)plCount);

        msg.addString (null, MamdaCommonFields.SYMBOL.getFid(),
                       book.getSymbol());
        
        if (book.hasPartId())
        {
            msg.addString (null, MamdaCommonFields.PART_ID.getFid(),
                           book.getPartId());    
        }           

        if (book.getBookTime() != null)
        msg.addDateTime (null,  MamdaOrderBookFields.BOOK_TIME.getFid(),
                         book.getBookTime());
    }

    public void addBookEntry (
        MamaMsg               msg,
        MamdaOrderBookEntry   entry,
        char                  entryDeltaAction, 
        MamaDateTime          plTime)
    {
        if (entry.getId() != null)
        {
            msg.addString (null, MamdaOrderBookFields.ENTRY_ID.getFid(),
                           entry.getId());
        }        

        msg.addChar (null, MamdaOrderBookFields.ENTRY_ACTION.getFid(),
                     entryDeltaAction);
        
        msg.addU32 (null, MamdaOrderBookFields.ENTRY_SIZE.getFid(),
                    (int)entry.getSize());
        
        if ((null == plTime) || (plTime.compareTo(entry.getTime())!=0 ))
        {
            msg.addDateTime (null, MamdaOrderBookFields.ENTRY_TIME.getFid(),
                             entry.getTime());
        }
        
        if (entry.getStatus() != 0)
        {
            msg.addU16 (null, MamdaOrderBookFields.ENTRY_STATUS.getFid(),
                        (short)entry.getStatus());
        }
    }

    public void addBookLevelEntries (
        MamaMsg                          plMsg,
        MamdaOrderBookPriceLevel         level)
    {
        // Add a vector of submsgs for each price level entry
        int numEntriesTotal = level.getNumEntriesTotal();

        // Make sure the reusable sub-submessage vector is large enough
        mEntries.grow(numEntriesTotal);
    
        int entCount = 0;    
        Iterator  mitr = level.entryIterator(); 
        
        MamdaOrderBookEntry ent = null;

        while (mitr.hasNext())
        {
            ent = (MamdaOrderBookEntry) mitr.next();          

            if (0 == ent.getSize())
                continue;
          
            MamaMsg entMsg =  mEntries.mMsgVector[entCount];     

            addBookEntry (entMsg, ent,
                          MamdaOrderBookEntry.ACTION_ADD,
                          level.getTime());
            entCount++;        
        }

        // flatten 
        if (1 == entCount)
        { 
            addBookEntry (plMsg, ent,
                          MamdaOrderBookEntry.ACTION_ADD,
                          level.getTime());
        }

        // Add entMsg vector to the plMsg
        if (entCount>1)
        {      
            plMsg.addArrayMsgWithLength (null, MamdaOrderBookFields.PL_ENTRIES.getFid(),
                                         mEntries.mMsgVector,entCount);
        }
        
        if (defaultNumAttachedEntries != (long) entCount)
        {
            plMsg.addU32 (null, MamdaOrderBookFields.PL_NUM_ATTACH.getFid(),
                          (long)entCount);
        }
    }        
 }

