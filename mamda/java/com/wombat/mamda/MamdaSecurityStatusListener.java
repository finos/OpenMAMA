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
import java.util.logging.*;
import java.util.Iterator;
import java.util.LinkedList;

/**
 * MamdaSecurityStatusListener is a class that specializes in handling
 * security status updates.  Developers provide their own
 * implementation of the MamdaSecurityStatusHandler interface and will
 * be delivered notifications for security status updates.
 *
 * Note: The MamdaSecurityStatusListener class caches some field
 * values.  Among other reasons, caching of these fields makes it
 * possible to provide complete certain callbacks, even when the
 * publisher (e.g., feed handler) is only publishing deltas containing
 * modified fields.
 */

public class MamdaSecurityStatusListener implements MamdaMsgListener, 
                                                    MamdaSecurityStatusRecap,
                                                    MamdaSecurityStatusUpdate
{
    private static Logger mLogger =
                      Logger.getLogger("com.wombat.mamda.MamdaSecurityStatusListener");

    private static SecurityStatusUpdate mUpdaters []        = null;
    private static boolean              mUpdatersComplete   = false;
    private static Object               mUpdatersLock       = new Object ();

    private final LinkedList            mHandlers           = new LinkedList();

    protected final MamdaSecurityStatusCache securityStatusCache =
                                       new MamdaSecurityStatusCache ();

    // Used for all field iteration processing
    private FieldIterator mFieldIterator = null;

    // The following fields are used for caching the current security
    // status.  These fields can be used by applications for reference
    // and will be passed for recaps.

    private class MamdaSecurityStatusCache 
    {
        public MamaDateTime    mSrcTime         = new MamaDateTime();
        public MamaDateTime    mActTime         = new MamaDateTime();
        public MamaDateTime    mSendTime        = new MamaDateTime();
        public MamaDateTime    mLineTime        = new MamaDateTime();
        public long            mEventSeqNum     = 0;
        public MamaDateTime    mEventTime       = new MamaDateTime();
        public long            mSecStatus       = 0;
        public long            mSecStatusQual   = 0;
        public String          mSecStatusStr    = null;
        public String          mSecStatusQualStr= null;
        public String          mSecStatusOrigStr= null;
        public char            mShortSaleCircuitBreaker = ' ';
        public String          mPartId          = null;
        public String          mSymbol          = null;
        public String          mIssueSymbol     = null;
        public String          mReason          = null;
        public boolean         mGotIssueSymbol  = false;
        public boolean         mUpdated  		= false;
        
        public MamdaFieldState    mSrcTimeFieldState           = new MamdaFieldState();
        public MamdaFieldState    mActTimeFieldState           = new MamdaFieldState();
        public MamdaFieldState    mSendTimeFieldState          = new MamdaFieldState();
        public MamdaFieldState    mLineTimeFieldState          = new MamdaFieldState();
        public MamdaFieldState    mEventSeqNumFieldState       = new MamdaFieldState();
        public MamdaFieldState    mEventTimeFieldState         = new MamdaFieldState();
        public MamdaFieldState    mSecStatusFieldState         = new MamdaFieldState();
        public MamdaFieldState    mSecStatusQualFieldState     = new MamdaFieldState();
        public MamdaFieldState    mSecStatusStrFieldState      = new MamdaFieldState();
        public MamdaFieldState    mSecStatusQualStrFieldState  = new MamdaFieldState();
        public MamdaFieldState    mSecStatusOrigStrFieldState  = new MamdaFieldState();
        public MamdaFieldState    mShortSaleCircuitBreakerFieldState  = new MamdaFieldState();
        public MamdaFieldState    mPartIdFieldState            = new MamdaFieldState();
        public MamdaFieldState    mSymbolFieldState            = new MamdaFieldState();
        public MamdaFieldState    mIssueSymbolFieldState       = new MamdaFieldState();
        public MamdaFieldState    mReasonFieldState            = new MamdaFieldState();
        public MamdaFieldState    mGotIssueSymbolFieldState    = new MamdaFieldState();
        public MamdaFieldState    mUpdatedFieldState           = new MamdaFieldState();

    }

    /**
     * Create a specialized trade listener.  This listener handles
     * trade reports, trade recaps, trade errors/cancels, trade
     * corrections, and trade gap notifications.
     */
    public MamdaSecurityStatusListener ()
    {
        clearCache (securityStatusCache);

        mFieldIterator = new FieldIterator (this);
    }

    /**
     * Add a specialized trade handler.  Currently, only one
     * handler can (and must) be registered.
     */
    public void addHandler (MamdaSecurityStatusHandler handler)
    {
        mHandlers.addLast(handler);
    }

    public void clearCache (MamdaSecurityStatusCache cache)
    {
        /*
         * NOTE: fields which are enums can be pubished as integers if feedhandler
         * uses mama-publish-enums-as-ints.  It may also be possible for a feed to
         * publish the numerical value as a string. All enumerated fields must be handled
         * by getting the value based on the field type.
         */

        cache.mSrcTime.clear    ();
        cache.mActTime.clear    ();
        cache.mSendTime.clear   ();
        cache.mLineTime.clear   ();
        cache.mEventSeqNum      = 0;
        cache.mEventTime.clear  ();
        cache.mSecStatus        = 0;
        cache.mSecStatusQual    = 0;
        cache.mSecStatusStr     = "Unknown";
        cache.mSecStatusQualStr = null;
        cache.mSecStatusOrigStr = null;
        cache.mShortSaleCircuitBreaker = ' ';
        cache.mPartId           = null;
        cache.mSymbol           = null;
        cache.mIssueSymbol      = null;
        cache.mReason           = null;
        
        cache.mSrcTimeFieldState.setState           (MamdaFieldState.NOT_INITIALISED);
        cache.mActTimeFieldState.setState           (MamdaFieldState.NOT_INITIALISED); 
        cache.mSendTimeFieldState.setState          (MamdaFieldState.NOT_INITIALISED);
        cache.mLineTimeFieldState.setState          (MamdaFieldState.NOT_INITIALISED);
        cache.mEventSeqNumFieldState.setState       (MamdaFieldState.NOT_INITIALISED); 
        cache.mEventTimeFieldState.setState         (MamdaFieldState.NOT_INITIALISED);
        cache.mSecStatusFieldState.setState         (MamdaFieldState.NOT_INITIALISED);
        cache.mSecStatusQualFieldState.setState     (MamdaFieldState.NOT_INITIALISED);
        cache.mSecStatusStrFieldState.setState      (MamdaFieldState.NOT_INITIALISED); 
        cache.mSecStatusQualStrFieldState.setState  (MamdaFieldState.NOT_INITIALISED); 
        cache.mSecStatusOrigStrFieldState.setState  (MamdaFieldState.NOT_INITIALISED); 
        cache.mShortSaleCircuitBreakerFieldState.setState(MamdaFieldState.NOT_INITIALISED);
        cache.mPartIdFieldState.setState            (MamdaFieldState.NOT_INITIALISED);
        cache.mSymbolFieldState.setState            (MamdaFieldState.NOT_INITIALISED);
        cache.mIssueSymbolFieldState.setState       (MamdaFieldState.NOT_INITIALISED);
        cache.mReasonFieldState.setState            (MamdaFieldState.NOT_INITIALISED);
        cache.mGotIssueSymbolFieldState.setState    (MamdaFieldState.NOT_INITIALISED);
        cache.mUpdatedFieldState.setState           (MamdaFieldState.NOT_INITIALISED);
    }

    // Inherited from MamdaBasicRecap and MamdaBasicEvent:

    public MamaDateTime getSrcTime()
    {
        return securityStatusCache.mSrcTime;
    }

    public MamaDateTime getActivityTime()
    {
        return securityStatusCache.mActTime;
    }

    public MamaDateTime getSendTime()
    {
        return securityStatusCache.mSendTime;
    }

    public MamaDateTime getLineTime()
    {
        return securityStatusCache.mLineTime;
    }

    public long getEventSeqNum()
    {
        return securityStatusCache.mEventSeqNum;
    }

    public MamaDateTime getEventTime()
    {
        return securityStatusCache.mEventTime;
    }

    public String getPartId()
    {
        return securityStatusCache.mPartId;
    }

    public String getSymbol()
    {
        return securityStatusCache.mSymbol;
    }

    // SecurityStatus recap fields access:

    public long getSecurityStatus()
    {
        return (long)MamdaSecurityStatus.mamdaSecurityStatusFromString (securityStatusCache.mSecStatusStr);
    }

    public long getSecurityStatusQualifier()
    {
        return (long)MamdaSecurityStatusQual.mamdaSecurityStatusQualFromString (securityStatusCache.mSecStatusQualStr);
    }

    public char getShortSaleCircuitBreaker()
    {
        return securityStatusCache.mShortSaleCircuitBreaker;
    }

    public short getSecurityStatusEnum()
    {
        return MamdaSecurityStatus.mamdaSecurityStatusFromString (securityStatusCache.mSecStatusStr);
    }

    public short getSecurityStatusQualifierEnum()
    {
        return MamdaSecurityStatusQual.mamdaSecurityStatusQualFromString(securityStatusCache.mSecStatusQualStr);
    }

    public String getSecurityStatusStr()
    {
        return securityStatusCache.mSecStatusStr;
    }

    public String getSecurityStatusQualifierStr()
    {
        return securityStatusCache.mSecStatusQualStr;
    }

    public String getSecurityStatusOrigStr()
    {    
        return securityStatusCache.mSecStatusOrigStr;
    }

    public String getReason()
    {
        return securityStatusCache.mReason;
    }


    /*      Field State Accessors       */
    
    public short getSrcTimeFieldState()
    {
        return securityStatusCache.mSrcTimeFieldState.getState();
    }

    public short getActivityTimeFieldState()
    {
        return securityStatusCache.mActTimeFieldState.getState();
    }

    public short getSendTimeFieldState()
    {
        return securityStatusCache.mSendTimeFieldState.getState();
    }

    public short getLineTimeFieldState()
    {
        return securityStatusCache.mLineTimeFieldState.getState();
    }
        
    public short getShortSaleCircuitBreakerFieldState()
    {          
        return securityStatusCache.mShortSaleCircuitBreakerFieldState.getState();
    }

    public short getEventSeqNumFieldState()
    {
        return securityStatusCache.mEventSeqNumFieldState.getState();
    }

    public short getEventTimeFieldState()
    {
        return securityStatusCache.mEventTimeFieldState.getState();
    }

    public short getPartIdFieldState()
    {
        return securityStatusCache.mPartIdFieldState.getState();
    }

    public short getSymbolFieldState()
    {
        return securityStatusCache.mSymbolFieldState.getState();
    }

    // SecurityStatus recap fields access:

    public short getSecurityStatusFieldState()
    {
        return securityStatusCache.mSecStatusStrFieldState.getState();
    }

    public short getSecurityStatusQualifierFieldState()
    {
        return securityStatusCache.mSecStatusQualStrFieldState.getState();
    }

    public short getSecurityStatusEnumFieldState()
    {
        return securityStatusCache.mSecStatusStrFieldState.getState();
    }

    public short getSecurityStatusQualifierEnumFieldState()
    {
        return securityStatusCache.mSecStatusQualStrFieldState.getState();
    }

    public short getSecurityStatusStrFieldState()
    {
        return securityStatusCache.mSecStatusStrFieldState.getState();
    }

    public short getSecurityStatusQualifierStrFieldState()
    {
        return securityStatusCache.mSecStatusQualStrFieldState.getState();
    }

    public short getSecurityStatusOrigStrFieldState()
    {
        return securityStatusCache.mSecStatusOrigStrFieldState.getState();
    }

    public short getReasonFieldState()
    {
        return securityStatusCache.mReasonFieldState.getState();
    }
 
    /**
     * Implementation of MamdaListener interface.
     */
    public void onMsg (MamdaSubscription subscription,
                       MamaMsg           msg,
                       short             msgType)
    {
       // Listeners may be created on multiple threads and we only
       // want a single list of updaters.
       if (!mUpdatersComplete)
       {
           synchronized (mUpdatersLock)
           {
               if (!MamdaSecurityStatusFields.isSet())
               {
                   mLogger.warning ("MamdaSecurityStatusFields::setDictionary() has not been called.");
                   return;
               }
               if (!mUpdatersComplete)
               {
                   createUpdaters ();
                   mUpdatersComplete = true;
               }
           }
       }

        updateFieldStates();
        // If msg is a trade-related message, invoke the
        // appropriate callback:
        synchronized (this)
        {
            securityStatusCache.mGotIssueSymbol = false;
            securityStatusCache.mUpdated = false;

            msg.iterateFields (mFieldIterator, null, null);
        }

        if (securityStatusCache.mGotIssueSymbol)
        {
            securityStatusCache.mSymbol = securityStatusCache.mIssueSymbol;
            securityStatusCache.mSymbolFieldState.setState (MamdaFieldState.MODIFIED);
        }

        switch (msgType)
        {
            case MamaMsgType.TYPE_INITIAL:
            case MamaMsgType.TYPE_RECAP:
                handleRecap (subscription, msg);
                break;
            case MamaMsgType.TYPE_PREOPENING:
            case MamaMsgType.TYPE_SEC_STATUS:
            case MamaMsgType.TYPE_UPDATE:
            case MamaMsgType.TYPE_QUOTE:
            case MamaMsgType.TYPE_TRADE:
            case MamaMsgType.TYPE_BOOK_UPDATE:
            case MamaMsgType.TYPE_MISC:
                handleSecurityStatus (subscription, msg);
                break;
        }
    }

    private void handleRecap (MamdaSubscription  subscription,
                              MamaMsg            msg)
    {
        Iterator i = mHandlers.iterator();
        while (i.hasNext())
        {
            MamdaSecurityStatusHandler handler = (MamdaSecurityStatusHandler) i.next();
            handler.onSecurityStatusRecap (subscription, this, msg, this);
        }
    }

    private void handleSecurityStatus (MamdaSubscription  subscription,
                                       MamaMsg            msg)
    {
        if (securityStatusCache.mUpdated)
        {            
            Iterator i = mHandlers.iterator();
            while (i.hasNext())
            {
                MamdaSecurityStatusHandler handler =
                    (MamdaSecurityStatusHandler) i.next();
                handler.onSecurityStatusUpdate (subscription, this, msg,
                                                this, this);
            }
        }
    }

    private void updateFieldStates()
    {
        if (securityStatusCache.mSrcTimeFieldState.getState() == MamdaFieldState.MODIFIED) 
            securityStatusCache.mSrcTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (securityStatusCache.mActTimeFieldState.getState() == MamdaFieldState.MODIFIED) 
            securityStatusCache.mActTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED); 
        if (securityStatusCache.mSendTimeFieldState.getState() == MamdaFieldState.MODIFIED)
            securityStatusCache.mSendTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (securityStatusCache.mLineTimeFieldState.getState() == MamdaFieldState.MODIFIED)
            securityStatusCache.mLineTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (securityStatusCache.mEventSeqNumFieldState.getState() == MamdaFieldState.MODIFIED)
            securityStatusCache.mEventSeqNumFieldState.setState(MamdaFieldState.NOT_MODIFIED); 
        if (securityStatusCache.mEventTimeFieldState.getState() == MamdaFieldState.MODIFIED)   
            securityStatusCache.mEventTimeFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (securityStatusCache.mSecStatusFieldState.getState() == MamdaFieldState.MODIFIED)
            securityStatusCache.mSecStatusFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (securityStatusCache.mSecStatusQualFieldState.getState() == MamdaFieldState.MODIFIED) 
            securityStatusCache.mSecStatusQualFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (securityStatusCache.mSecStatusStrFieldState.getState() == MamdaFieldState.MODIFIED) 
            securityStatusCache.mSecStatusStrFieldState.setState(MamdaFieldState.NOT_MODIFIED); 
        if (securityStatusCache.mSecStatusQualStrFieldState.getState() == MamdaFieldState.MODIFIED)  
            securityStatusCache.mSecStatusQualStrFieldState.setState(MamdaFieldState.NOT_MODIFIED); 
        if (securityStatusCache.mSecStatusOrigStrFieldState.getState() == MamdaFieldState.MODIFIED) 
            securityStatusCache.mSecStatusOrigStrFieldState.setState(MamdaFieldState.NOT_MODIFIED); 
        if (securityStatusCache.mShortSaleCircuitBreakerFieldState.getState() == MamdaFieldState.MODIFIED) 
            securityStatusCache.mShortSaleCircuitBreakerFieldState.setState(MamdaFieldState.NOT_MODIFIED); 
        if (securityStatusCache.mPartIdFieldState.getState() == MamdaFieldState.MODIFIED)
            securityStatusCache.mPartIdFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (securityStatusCache.mSymbolFieldState.getState() == MamdaFieldState.MODIFIED)
            securityStatusCache.mSymbolFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (securityStatusCache.mIssueSymbolFieldState.getState() == MamdaFieldState.MODIFIED)
            securityStatusCache.mIssueSymbolFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (securityStatusCache.mReasonFieldState.getState() == MamdaFieldState.MODIFIED)
            securityStatusCache.mReasonFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (securityStatusCache.mGotIssueSymbolFieldState.getState() == MamdaFieldState.MODIFIED)
            securityStatusCache.mGotIssueSymbolFieldState.setState(MamdaFieldState.NOT_MODIFIED);
        if (securityStatusCache.mUpdatedFieldState.getState() == MamdaFieldState.MODIFIED)
            securityStatusCache.mUpdatedFieldState.setState(MamdaFieldState.NOT_MODIFIED);
    }
    private static void createUpdaters ()
    {
        mUpdaters = new SecurityStatusUpdate [MamdaSecurityStatusFields.getMaxFid() + 1];

        addUpdaterToList (MamdaSecurityStatusFields.SRC_TIME,
                             new SecurityStatusSrcTime ());
        addUpdaterToList (MamdaSecurityStatusFields.ACTIVITY_TIME,
                             new SecurityStatusActivityTime ());
        addUpdaterToList (MamdaSecurityStatusFields.SEND_TIME,
                             new SecurityStatusSendTime ());
        addUpdaterToList (MamdaSecurityStatusFields.LINE_TIME,
                             new SecurityStatusLineTime ());
        addUpdaterToList (MamdaSecurityStatusFields.SECURITY_STATUS,
                             new SecurityStatusStr ());
        addUpdaterToList (MamdaSecurityStatusFields.SECURITY_STATUS_QUAL,
                             new SecurityStatusQualStr ());
        addUpdaterToList (MamdaSecurityStatusFields.SECURITY_STATUS_ORIG,
                             new SecurityStatusOrigStr ());
        addUpdaterToList (MamdaSecurityStatusFields.SECURITY_STATUS_TIME,
                             new SecurityStatusTime ());
        addUpdaterToList (MamdaSecurityStatusFields.SEQNUM,
                             new SecurityStatusSeqNum());
        addUpdaterToList (MamdaSecurityStatusFields.SHORT_SALE_CIRCUIT_BREAKER,
                             new ShortSaleCircuitBreaker());
        addUpdaterToList (MamdaSecurityStatusFields.PART_ID,
                             new SecurityStatusPartId());
        addUpdaterToList (MamdaSecurityStatusFields.SYMBOL,
                             new SecurityStatusSymbol());
        addUpdaterToList (MamdaSecurityStatusFields.ISSUE_SYMBOL,
                             new SecurityStatusIssueSymbol());
        addUpdaterToList (MamdaSecurityStatusFields.REASON,
                             new SecurityStatusReason());

    }

    private static void addUpdaterToList (MamaFieldDescriptor   fieldDesc,
                                          SecurityStatusUpdate  updater)
    {
        if (fieldDesc == null) return;
        int fieldId = fieldDesc.getFid();
        mUpdaters[fieldId] = updater;
    }

    public String getFieldAsString (MamaMsgField field)
    {
        switch (field.getType())
        {
            case MamaFieldDescriptor.I8:
            case MamaFieldDescriptor.U8:
            case MamaFieldDescriptor.I16:
            case MamaFieldDescriptor.U16:
            case MamaFieldDescriptor.I32:
            case MamaFieldDescriptor.U32:
                return String.valueOf(field.getU32());
            case MamaFieldDescriptor.STRING:
                return field.getString();
            default:
                mLogger.fine ("Unhandled type " + field.getType() +
                              " for " + field.getName() + 
                              ".  Expected string or integer.");
                break;
        }

        return null;
    }

    private interface SecurityStatusUpdate
    {
        public void onUpdate (MamdaSecurityStatusListener listener,
                              MamaMsgField                field);
    }

    private static class SecurityStatusSrcTime implements SecurityStatusUpdate
    {
        public void onUpdate (MamdaSecurityStatusListener listener,
                              MamaMsgField                field)
        {
            listener.securityStatusCache.mSrcTime.copy (field.getDateTime());
            listener.securityStatusCache.mSrcTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class SecurityStatusActivityTime implements SecurityStatusUpdate
    {
        public void onUpdate (MamdaSecurityStatusListener listener,
                              MamaMsgField                field)
        {
            listener.securityStatusCache.mActTime.copy (field.getDateTime());
            listener.securityStatusCache.mActTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class SecurityStatusSendTime implements SecurityStatusUpdate
    {
        public void onUpdate (MamdaSecurityStatusListener listener,
                              MamaMsgField                field)
        {
            listener.securityStatusCache.mSendTime.copy (field.getDateTime());
            listener.securityStatusCache.mSendTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class SecurityStatusLineTime implements SecurityStatusUpdate
    {
        public void onUpdate (MamdaSecurityStatusListener listener,
                              MamaMsgField                field)
        {
            listener.securityStatusCache.mLineTime.copy (field.getDateTime());
            listener.securityStatusCache.mLineTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class SecurityStatusQualStr implements SecurityStatusUpdate
    {
        public void onUpdate (MamdaSecurityStatusListener listener,
                              MamaMsgField                field)
        {
            String statusQual = listener.getFieldAsString(field);

            if (listener.securityStatusCache.mSecStatusQualStr != null)
            {
                if  (listener.securityStatusCache.mSecStatusQualStr.equals(statusQual))
                {
                    return;
                }
            }
            listener.securityStatusCache.mSecStatusQualStr = statusQual;
            listener.securityStatusCache.mSecStatusQualStrFieldState.setState (MamdaFieldState.MODIFIED);
            listener.securityStatusCache.mUpdated = true;
        }
    }

    private static class SecurityStatusTime implements SecurityStatusUpdate
    {
        public void onUpdate (MamdaSecurityStatusListener listener,
                              MamaMsgField                field)
        {
           listener.securityStatusCache.mEventTime.copy (field.getDateTime());
           listener.securityStatusCache.mEventTimeFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class SecurityStatusOrigStr implements SecurityStatusUpdate
    {

        public void onUpdate (MamdaSecurityStatusListener listener,
                              MamaMsgField                field)
        {
            if (listener.securityStatusCache.mSecStatusOrigStr != null)
            {
                if  (listener.securityStatusCache.mSecStatusOrigStr.equals(field.getString()))
                {
                    return;
                }
            }
            listener.securityStatusCache.mSecStatusOrigStr = field.getString();
            listener.securityStatusCache.mSecStatusOrigStrFieldState.setState (MamdaFieldState.MODIFIED);
            listener.securityStatusCache.mUpdated = true;
        }
    }

    private static class ShortSaleCircuitBreaker implements SecurityStatusUpdate
    {
        public void onUpdate (MamdaSecurityStatusListener listener,
                              MamaMsgField                field)
        {  
         
            if (listener.securityStatusCache.mShortSaleCircuitBreaker == field.getChar())
            {
                return;
            }
            listener.securityStatusCache.mShortSaleCircuitBreaker = field.getChar(); 
            listener.securityStatusCache.mShortSaleCircuitBreakerFieldState.setState (MamdaFieldState.MODIFIED);  
            listener.securityStatusCache.mUpdated = true;   
        }
    }

    private static class SecurityStatusSeqNum implements SecurityStatusUpdate
    {
        public void onUpdate (MamdaSecurityStatusListener listener,
                              MamaMsgField                field)
        {
            listener.securityStatusCache.mEventSeqNum = field.getI64 ();
            listener.securityStatusCache.mEventSeqNumFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class SecurityStatusStr implements SecurityStatusUpdate
    {
        public void onUpdate (MamdaSecurityStatusListener listener,
                              MamaMsgField                field)
        {
            String status = listener.getFieldAsString(field);
            
            if (listener.securityStatusCache.mSecStatusStr != null)
            {
                if  (listener.securityStatusCache.mSecStatusStr.equals(status))
                {
                    return;
                }
            }

            listener.securityStatusCache.mSecStatusStr = status;
            listener.securityStatusCache.mSecStatusStrFieldState.setState (MamdaFieldState.MODIFIED);
            listener.securityStatusCache.mUpdated = true;
        }
    }

    private static class SecurityStatusPartId implements SecurityStatusUpdate
    {
        public void onUpdate (MamdaSecurityStatusListener listener,
                              MamaMsgField                field)
        {
            listener.securityStatusCache.mPartId = field.getString();
            listener.securityStatusCache.mPartIdFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class SecurityStatusSymbol implements SecurityStatusUpdate
    {
        public void onUpdate (MamdaSecurityStatusListener listener,
                              MamaMsgField                field)
        {
            listener.securityStatusCache.mSymbol = field.getString();
            listener.securityStatusCache.mSymbolFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    private static class SecurityStatusIssueSymbol implements SecurityStatusUpdate
    {
        public void onUpdate (MamdaSecurityStatusListener listener,
                              MamaMsgField                field)
        {
            listener.securityStatusCache.mIssueSymbol = field.getString();
            listener.securityStatusCache.mIssueSymbolFieldState.setState (MamdaFieldState.MODIFIED);
            listener.securityStatusCache.mGotIssueSymbol = true;
        }
    }

    private static class SecurityStatusReason implements SecurityStatusUpdate
    {
        public void onUpdate (MamdaSecurityStatusListener listener,
                              MamaMsgField                field)
        {
            listener.securityStatusCache.mReason = field.getString();
            listener.securityStatusCache.mReasonFieldState.setState (MamdaFieldState.MODIFIED);
        }
    }

    /*************************************************************
     *Private class used for Field Iteration
     ************************************************************/
    private class FieldIterator implements MamaMsgFieldIterator
    {
        private MamdaSecurityStatusListener mListener;

        public FieldIterator (MamdaSecurityStatusListener listener)
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
                    SecurityStatusUpdate updater =
                        (SecurityStatusUpdate) mUpdaters[fieldId];

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
}
