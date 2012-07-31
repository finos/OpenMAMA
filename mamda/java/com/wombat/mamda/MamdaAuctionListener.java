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

package com.wombat.mamda;

import com.wombat.mama.*;
import java.util.*;
import java.util.logging.*;
import java.io.*;

/**
 * A MamdaAuctionListener is class that specializes in handling
 * Auction order updates. An Auction order occurs when too many 
 * orders of a particular type - either buy, sell or limit - for listed
 * securities and not enough of the other, matching orders are received by 
 * an exchange.
 * Developers provide their own implementation of the
 * MamdaAuctionHandler interface and will be delivered notifications 
 * for Auction updates.  
 */

public class MamdaAuctionListener implements MamdaMsgListener,
                                             MamdaBasicEvent,
                                             MamdaBasicRecap, 
                                             MamdaAuctionRecap,
                                             MamdaAuctionUpdate

{
    private final List mHandlers = new LinkedList();    
    
    private static Logger mLogger =
        Logger.getLogger ("com.wombat.mamda.MamdaAuctionListener");

    private static AuctionUpdate  mUpdaters [] = null;
    private static Object mUpdatersLock = new Object ();
    
    // Used for all field iteration processing
    private FieldIterator mFieldIterator = null;

    public MamaDateTime     mSrcTime               = new MamaDateTime();
    public MamaDateTime     mActTime               = new MamaDateTime();
    public MamaDateTime     mSendTime              = new MamaDateTime();
    public MamaDateTime     mLineTime              = new MamaDateTime();
    public MamaDateTime     mEventTime             = new MamaDateTime();
    public MamaLong         mEventSeqNum           = new MamaLong();

    public MamaString       mIssueSymbol           = new MamaString();
    public MamaString       mPartId                = new MamaString();
    public MamaString       mSymbol                = new MamaString();

    public MamaPrice            mUncrossPrice      = new MamaPrice();
    public MamaLong             mUncrossVolume     = new MamaLong();
    public MamdaUncrossPriceInd mUncrossPriceInd   = new MamdaUncrossPriceInd();
        
    public MamdaFieldState  mSrcTimeFieldState     = new MamdaFieldState();
    public MamdaFieldState  mActTimeFieldState     = new MamdaFieldState();
    public MamdaFieldState  mSendTimeFieldState    = new MamdaFieldState();
    public MamdaFieldState  mLineTimeFieldState    = new MamdaFieldState();
    public MamdaFieldState  mEventTimeFieldState   = new MamdaFieldState();

    public MamdaFieldState  mIssueSymbolFieldState = new MamdaFieldState();
    public MamdaFieldState  mPartIdFieldState      = new MamdaFieldState();
    public MamdaFieldState  mSymbolFieldState      = new MamdaFieldState();
    public MamdaFieldState  mEventSeqNumFieldState = new MamdaFieldState();

    public MamdaFieldState  mUncrossPriceFieldState    = new MamdaFieldState();
    public MamdaFieldState  mUncrossVolumeFieldState   = new MamdaFieldState();
    public MamdaFieldState  mUncrossPriceIndFieldState = new MamdaFieldState();

    public MamaPrice        tmpPrice        = new MamaPrice();
    public MamaDouble       tmpDouble       = new MamaDouble();
   
    /**
     * clearCache - clears all cached data by resetting to 
     * default values.
     * @param cache The Auction order cache to clear.
     */
    public void clearCache ()
    {
      
        mSrcTime.clear ();    mSrcTimeFieldState.setState  (MamdaFieldState.NOT_INITIALISED);
        mActTime.clear ();    mActTimeFieldState.setState  (MamdaFieldState.NOT_INITIALISED);
        mSendTime.clear();    mSendTimeFieldState.setState (MamdaFieldState.NOT_INITIALISED);
        mLineTime.clear();    mLineTimeFieldState.setState (MamdaFieldState.NOT_INITIALISED);

        mIssueSymbol.setValue (null);  mIssueSymbolFieldState.setState (MamdaFieldState.NOT_INITIALISED);
        mPartId.setValue      (null);  mPartIdFieldState.setState      (MamdaFieldState.NOT_INITIALISED);
        mSymbol.setValue      (null);  mSymbolFieldState.setState      (MamdaFieldState.NOT_INITIALISED);
      
        mUncrossPrice.clear     ();
        mUncrossVolume.setValue (0);
        mUncrossPriceInd.set    (MamdaUncrossPriceInd.UNCROSS_NONE);
      
        mUncrossPriceFieldState.setState    (MamdaFieldState.NOT_INITIALISED);
        mUncrossVolumeFieldState.setState   (MamdaFieldState.NOT_INITIALISED);
        mUncrossPriceIndFieldState.setState (MamdaFieldState.NOT_INITIALISED);
    }

    /**
     * constructor
     */
    public MamdaAuctionListener ()
    {
        clearCache ();        
        mFieldIterator = new FieldIterator (this);
    }
    
    /**
     * addHandler - Registers handler interested in receiving update
     * update and recap events.
     * Currently only one handler can and must be registered.
     * @param handler - the handler to be registered.
    */
    public void addHandler (MamdaAuctionHandler handler)
    {
        mHandlers.add (handler);
    }    

    /**
     * getActivityTime
     * @return mActTime
     * @see MamdaBasicEvent#getActivityTime
     */
    public MamaDateTime getActivityTime ()
    {
        return mActTime;
    }

    /**
     * getSrcTime
     * @return mSrcTime
     * @see MamdaBasicEvent#getSrcTime
     */
    public MamaDateTime getSrcTime ()
    {
        return mSrcTime;
    }

    /**
     * getSendTime
     * @return mSendTime
     * @see MamdaBasicEvent#getSendTime
     */
    public MamaDateTime getSendTime ()
    {
        return mSendTime;
    } 

    /**
     * getLineTime
     * @return mLineTime
     * @see MamdaBasicEvent#getLineTime
     */
    public MamaDateTime getLineTime ()
    {
        return mLineTime;
    }

    /**
     * getIssueSymbol
     * @return mIssueSymbol
     */
    public String getIssueSymbol ()
    {
        return mIssueSymbol.getValue();
    }

    /**
     * getPartId 
     * @return mPartId
     */
    public String getPartId ()
    {
        return mPartId.getValue();
    }

    /**
     * getSymbol
     * @return mSymbol
     */
    public String getSymbol ()
    {
        return mSymbol.getValue();
    }

    /**
     * getEventTime
     * @return mEventTime
     */
    public MamaDateTime getEventTime()
    {
        return mEventTime;
    }

    /**
     * getEventSeqNum
     * @return mEventSeqNum
     */
    public long getEventSeqNum()
    {
        return mEventSeqNum.getValue();
    }
    
    /**
     * getUncrossPrice
     * @return mUncrossPrice
     */
    public MamaPrice getUncrossPrice()
    {
        return mUncrossPrice;
    }
    
    /**
     * getUncrossVolume
     * @return mUncrossVolume
     */
    public long getUncrossVolume()
    {
        return mUncrossVolume.getValue();
    }

    /**
     * getUncrossPriceInd
     * @return mUncrossPriceInd
     */
    public short getUncrossPriceInd()
    {
        return mUncrossPriceInd.get();
    }
    

    /*      FieldState Accessors        */
    /**
     * getActivityTimeFieldState
     * @return mActTimeFieldState
     * @see MamdaBasicEvent#getActivityTimeFieldState
     */
    public short getActivityTimeFieldState()
    {
        return mActTimeFieldState.getState();
    }

    /**
     * getSrcTimeFieldState
     * @return mSrcTimeFieldState
     * @see MamdaBasicEvent#getSrcTimeFieldState
     */
    public short getSrcTimeFieldState()
    {
        return mSrcTimeFieldState.getState();
    }

    /**
     * getSendTimeFieldState
     * @return mSendTimeFieldState
     * @see MamdaBasicEvent#getSendTimeFieldState
     */
    public short getSendTimeFieldState()
    {
        return mSendTimeFieldState.getState();
    } 

    /**
     * getLineTimeFieldState
     * @return mLineTimeFieldState
     * @see MamdaBasicEvent#getLineTimeFieldState
     */
    public short getLineTimeFieldState()
    {
        return mLineTimeFieldState.getState();
    }

    /**
     * getIssueSymbolFieldState
     * @return mIssueSymbolFieldState
     */
    public short getIssueSymbolFieldState()
    {
        return mIssueSymbolFieldState.getState();
    }

    /**
     * getPartIdFieldState
     * @return mPartIdFieldState
     */
    public short getPartIdFieldState()
    {
        return mPartIdFieldState.getState();
    }

    /**
     * getSymbolFieldState
     * @return mSymbolFieldState
     */
    public short getSymbolFieldState()
    {
        return mSymbolFieldState.getState();
    }

    /**
     * getEventTimeFieldState
     * @return mEventTimeFieldState
     */
    public short getEventTimeFieldState()
    {
        return mEventTimeFieldState.getState();
    }
    
    /**
     * getEventSeqNumFieldState
     * @return mEventSeqNumFieldState
     */
    public short getEventSeqNumFieldState()
    {
        return mEventSeqNumFieldState.getState();
    }    
    
    /**
     * getUncrossPriceFieldState
     * @return mUncrossPriceFieldState
     */
    public short getUncrossPriceFieldState()
    {
        return mUncrossPriceFieldState.getState();
    }

    /**
     * getUncrossVolumeFieldState
     * @return mUncrossVolumeFieldState
     */
    public short getUncrossVolumeFieldState()
    {
        return mUncrossVolumeFieldState.getState();
    }
    
    /**
     * getUncrossPriceIndFieldState
     * @return mUncrossPriceIndFieldState
     */
    public short getUncrossPriceIndFieldState()
    {
        return mUncrossPriceIndFieldState.getState();
    }     
    /*      End fieldState accessors        */

    /**
     * Implementation of MamdaListener interface.
     */
    public void onMsg (MamdaSubscription subscription,
                       final MamaMsg     msg,
                       short             msgType)
    {
       // Listeners may be created on multiple threads and we only
       // want a single list of updaters.
       if (null == mUpdaters)
       {
           synchronized (mUpdatersLock)
           {
               if (!MamdaAuctionFields.isSet())
               {
                   mLogger.warning ("MamdaAuctionFields::setDictionary() has not been called.");
                   return;
               }
               if (null==mUpdaters) 
               {
                   createUpdaters ();
               }
           }
       }

        switch (msgType)
        {
        case MamaMsgType.TYPE_INITIAL:
        case MamaMsgType.TYPE_RECAP:
        case MamaMsgType.TYPE_UPDATE:
        {
            handleAuctionMessage (subscription,msg,msgType);
        }
        break;
        default:
            break;
        }
    }

    private void handleRecap (MamdaSubscription subscription,
                              final MamaMsg     msg)
    {
        Iterator iter = mHandlers.iterator ();
        while (iter.hasNext())
        {
            MamdaAuctionHandler handler =
                (MamdaAuctionHandler) iter.next();
            handler.onAuctionRecap (subscription, this, msg, this);
        }
    }
   
    private void handleUpdate (MamdaSubscription subscription,
                               final MamaMsg     msg)
    {
        Iterator iter = mHandlers.iterator ();
        while (iter.hasNext())
        {
            MamdaAuctionHandler handler =
                (MamdaAuctionHandler) iter.next();
            handler.onAuctionUpdate (subscription, this, msg, this, this);
        }
    }

    private void updateFieldStates ()
    {
        if (mSrcTimeFieldState.getState() == MamdaFieldState.MODIFIED)     
            mSrcTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);

        if (mActTimeFieldState.getState() == MamdaFieldState.MODIFIED)
            mActTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);

        if (mSendTimeFieldState.getState() == MamdaFieldState.MODIFIED)
            mSendTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);

        if (mLineTimeFieldState.getState() == MamdaFieldState.MODIFIED)
            mLineTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
    
        if (mIssueSymbolFieldState.getState() == MamdaFieldState.MODIFIED)
            mIssueSymbolFieldState.setState(MamdaFieldState.NOT_MODIFIED);

        if (mPartIdFieldState.getState() == MamdaFieldState.MODIFIED)     
            mPartIdFieldState.setState(MamdaFieldState.NOT_MODIFIED);

        if (mSymbolFieldState.getState() == MamdaFieldState.MODIFIED)
            mSymbolFieldState.setState(MamdaFieldState.NOT_MODIFIED);

        if (mUncrossPriceFieldState.getState() == MamdaFieldState.MODIFIED)
            mUncrossPriceFieldState.setState(MamdaFieldState.NOT_MODIFIED);

        if (mUncrossVolumeFieldState.getState() == MamdaFieldState.MODIFIED)     
            mUncrossVolumeFieldState.setState(MamdaFieldState.NOT_MODIFIED);

        if (mUncrossPriceIndFieldState.getState() == MamdaFieldState.MODIFIED)
            mUncrossPriceIndFieldState.setState(MamdaFieldState.NOT_MODIFIED);        
    }

    private void handleAuctionMessage (final MamdaSubscription subscription,
                                       final MamaMsg           msg,
                                       final int               msgType)
    {
        // Reset field states
        updateFieldStates();
        synchronized (this)
        {
            msg.iterateFields (mFieldIterator, null, null);
        }
        
        switch (msgType)
        {
            case MamaMsgType.TYPE_INITIAL:
            case MamaMsgType.TYPE_RECAP:
                handleRecap (subscription, msg);
                break;
            case MamaMsgType.TYPE_UPDATE:
                handleUpdate (subscription,msg); 
                break;
            default:
                break;
        }
        
    }
    

    private static void createUpdaters ()
    {
        mUpdaters = new AuctionUpdate [MamdaAuctionFields.getMaxFid() + 1];


        addUpdaterToList (MamdaCommonFields.ISSUE_SYMBOL,
                           new AuctionIssueSymbol ());

        addUpdaterToList (MamdaCommonFields.PART_ID,
                           new AuctionPartId ());

        addUpdaterToList (MamdaCommonFields.SYMBOL,
                           new AuctionSymbol ());

        addUpdaterToList (MamdaCommonFields.SRC_TIME,
                           new AuctionSrcTime ());

        addUpdaterToList (MamdaCommonFields.ACTIVITY_TIME,
                           new AuctionActivityTime ());

        addUpdaterToList (MamdaCommonFields.SEND_TIME,
                           new AuctionSendTime ());

        addUpdaterToList (MamdaCommonFields.LINE_TIME,
                           new AuctionLineTime ());
                           
        addUpdaterToList (MamdaAuctionFields.UNCROSS_PRICE,
                           new AuctionUncrossPrice ());

        addUpdaterToList (MamdaAuctionFields.UNCROSS_VOLUME,
                           new AuctionUncrossVolume ());

        addUpdaterToList (MamdaAuctionFields.UNCROSS_PRICE_IND,
                           new AuctionUncrossPriceInd ());                           
    }

    private static void addUpdaterToList (MamaFieldDescriptor fieldDesc,
                                          AuctionUpdate       updater)
    {
        if (fieldDesc == null) return;
        int fieldId = fieldDesc.getFid();
        mUpdaters[fieldId]= updater;
    }

    /*************************************************************
     *Private class used for Field Iteration
     ************************************************************/
    private class FieldIterator implements MamaMsgFieldIterator
    {
        private MamdaAuctionListener mListener;
        
        public FieldIterator (MamdaAuctionListener listener)
        {
            mListener = listener;
        }
        public void onField (MamaMsg        msg,
                             MamaMsgField   field,
                             MamaDictionary dictionary,
                             Object         closure)
        {
            try
            {
                int fieldId = field.getFid ();
                if (fieldId < mUpdaters.length)
                {
                    AuctionUpdate updater = 
                        (AuctionUpdate) mUpdaters[fieldId];
                
                    if (updater != null)
                    {
                        updater.onUpdate (mListener,field);
                    }
                }
            }
            catch (Exception ex )
            {
                mLogger.fine ("Error processing field - fid: " + field.getFid () +
                                  " type: " + field.getTypeName());
                throw new MamaException (ex.getMessage());
            }
        }

    }  
    
    private interface AuctionUpdate
    {
        public void onUpdate (MamdaAuctionListener listener,            
                              MamaMsgField         field);
    }

    private static class AuctionSrcTime implements AuctionUpdate
    {
        public void onUpdate (MamdaAuctionListener listener,
                              MamaMsgField         field)
        {
            listener.mSrcTime.copy (
                field.getDateTime ());
            listener.mSrcTimeFieldState.setState(
                MamdaFieldState.MODIFIED);
        }
    }

    private static class AuctionActivityTime implements AuctionUpdate
    {
        public void onUpdate (MamdaAuctionListener listener,
                              MamaMsgField         field)
        {
            listener.mActTime.copy (
                field.getDateTime ());
            listener.mActTimeFieldState.setState(
                MamdaFieldState.MODIFIED);
        }
    }

    private static class AuctionSendTime implements AuctionUpdate
    {
        public void onUpdate (MamdaAuctionListener listener,
                              MamaMsgField         field)
        {
            listener.mSendTime.copy (
                field.getDateTime ());
            listener.mSendTimeFieldState.setState(
                MamdaFieldState.MODIFIED);
        }
    }

    private static class AuctionLineTime implements AuctionUpdate
    {
        public void onUpdate (MamdaAuctionListener listener,
                              MamaMsgField         field)
        {
            listener.mLineTime.copy (field.getDateTime());
            listener.mLineTimeFieldState.setState(
                MamdaFieldState.MODIFIED);
        }
    }

    private static class AuctionIssueSymbol implements AuctionUpdate
    {
        public void onUpdate (MamdaAuctionListener listener,
                              MamaMsgField         field)
        {
            listener.mIssueSymbol.setValue (field.getString());
            listener.mIssueSymbolFieldState.setState(
                MamdaFieldState.MODIFIED);
        }
    }

    private static class AuctionPartId implements AuctionUpdate
    {
        public void onUpdate (MamdaAuctionListener listener,
                              MamaMsgField         field)
        {
            listener.mPartId.setValue (field.getString());
            listener.mPartIdFieldState.setState(
                MamdaFieldState.MODIFIED);
        }
    }

    private static class AuctionSymbol implements AuctionUpdate
    {
        public void onUpdate (MamdaAuctionListener listener,
                              MamaMsgField         field)
        {
            listener.mSymbol.setValue (field.getString());
            listener.mSymbolFieldState.setState(
                MamdaFieldState.MODIFIED);
        }
    }
    
    private static class AuctionUncrossPrice implements AuctionUpdate
    {
        public void onUpdate (MamdaAuctionListener listener,
                              MamaMsgField         field)
        {
            listener.mUncrossPrice.setValue (field.getPrice().getValue());
            listener.mUncrossPriceFieldState.setState (MamdaFieldState.MODIFIED);
        }

    }
    
    private static class AuctionUncrossVolume implements AuctionUpdate
    {
        public void onUpdate (MamdaAuctionListener listener,
                              MamaMsgField         field)
        {
            listener.mUncrossVolume.setValue (field.getI64());
            listener.mUncrossVolumeFieldState.setState (MamdaFieldState.MODIFIED);
        }

    }  
    
    private static class AuctionUncrossPriceInd implements AuctionUpdate
    {
        public void onUpdate (MamdaAuctionListener listener,
                              MamaMsgField         field)
        {
            switch (field.getType())
            {
                case MamaFieldDescriptor.I8:
                case MamaFieldDescriptor.U8:
                case MamaFieldDescriptor.I16:
                case MamaFieldDescriptor.U16:
                case MamaFieldDescriptor.I32:
                case MamaFieldDescriptor.U32:
                {
                    listener.mUncrossPriceInd.set ((short)field.getU32());
                    listener.mUncrossPriceIndFieldState.setState (MamdaFieldState.MODIFIED);
                    break;
                }
                case MamaFieldDescriptor.STRING:
                {
                    listener.mUncrossPriceInd.set(
                    MamdaUncrossPriceInd.mamdaUncrossPriceIndFromString (field.getString()));
                    listener.mUncrossPriceIndFieldState.setState (MamdaFieldState.MODIFIED);
                    break;
                }
                    
                default:
                {
                    mLogger.fine ("Unhandled type " + field.getType() +
                                " for wUncrossPriceInd. Expected string or integer.");
                }
            }
        }
    }  
}


