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

package com.wombat.mamda.options;

import com.wombat.mamda.*;
import com.wombat.mama.*;
import java.util.logging.*;
import java.text.SimpleDateFormat;
import java.text.ParseException;
import java.util.Date;
import java.util.Iterator;
import java.util.LinkedList;


/**
 * MamdaOptionChainListener is a class that specializes in handling
 * and managing option chain updates.  Developers provide their own
 * implementation of the MamdaOptionChainHandler interface and will be
 * delivered notifications for various types of options-related events.
 */

public class MamdaOptionChainListener implements MamdaMsgListener,
                                                 MamdaOptionSeriesUpdate
{
    private static Logger mLogger =
              Logger.getLogger("com.wombat.mamda.MamdaOptionChainListener");

    private final MamdaOptionChain   mChain;
    private final LinkedList         mHandlers                 = new LinkedList();
    private final SimpleDateFormat   mDateFormat               = new SimpleDateFormat("yyyy-MM-dd");

    // MamdaBasicEvent implementation members:
    private final MamaDateTime       mSrcTime                  = new MamaDateTime();
    private final MamaDateTime       mActTime                  = new MamaDateTime();
    private final MamaLong           mEventSeqNum              = new MamaLong();
    private final MamaDateTime       mEventTime                = new MamaDateTime();

    private final MamdaFieldState    mSrcTimeFieldState        = new MamdaFieldState();
    private final MamdaFieldState    mActTimeFieldState        = new MamdaFieldState();
    private final MamdaFieldState    mEventSeqNumFieldState    = new MamdaFieldState();
    private final MamdaFieldState    mEventTimeFieldState      = new MamdaFieldState();
    
    private MamaDouble               strikePrice               = new MamaDouble();
    private MamaString               putCall                   = new MamaString();
    private MamaLong                 openInterest              = new MamaLong();
    private MamaDateTime             expireDateField           = new MamaDateTime ("00:00:00");
    private MamaString               contractSymbol            = new MamaString();
    private Date                     expireDate                = new Date(0,0,0);
    private boolean                  gotExpireDate             = true;
    
    
    // MamdaOptionSeriesUpdate implementation members:
    public static final char         ACTION_UNKNOWN            = ' ';
    public static final char         ACTION_ADD                = 'A';
    public static final char         ACTION_DELETE             = 'D';
    private MamdaOptionContract      mLastActionContract       = null;
    private char                     mLastAction               = ACTION_UNKNOWN;

    private MamaMsgField             mTmpField                 = new MamaMsgField();

    /**
     * Create a specialized option chain listener.  This listener
     * handles option chain updates.
     */
    public MamdaOptionChainListener (String underlyingSymbol)
    {
        mChain = new MamdaOptionChain(underlyingSymbol);
        mLogger.fine ("Created new option chain and listener for: " + 
                       underlyingSymbol);
    }

    /**
     * Create a specialized option chain listener.  This listener
     * handles option chain updates.
     */
    public MamdaOptionChainListener (MamdaOptionChain  chain)
    {
        mChain = chain;
        mLogger.fine ("Received option chain and creating listener for: " + 
                       chain.getSymbol());
    }

    /**
     * Add a specialized option chain handler.  Currently, only one
     * handler can (and must) be registered.
     */
    public void addHandler (MamdaOptionChainHandler handler)
    {
        mHandlers.addLast(handler);
    }

    /**
     * Return the option chain associated with this listener.
     */
    public MamdaOptionChain getOptionChain ()
    {
        return mChain;
    }

    /**
     * Implementation of MamdaListener interface.
     */
    public void onMsg (MamdaSubscription subscription,
                       MamaMsg           msg,
                       short             msgType)
    {
        mLogger.finer ("Got message for " + subscription.getSymbol());

        if (!MamdaOptionFields.isSet())
        {
            mLogger.warning ("MamdaOptionFields::setDictionary() has not been called.");
            return;
        }

        if (msgType == MamaMsgType.TYPE_END_OF_INITIALS)
        {
            Iterator i = mHandlers.iterator();
            while (i.hasNext())
            {
                MamdaOptionChainHandler handler =
                    (MamdaOptionChainHandler) i.next();
                handler.onOptionChainRecap (subscription, this, msg, mChain);
            }
            return;
        }

        try
        {
            MamdaOptionContract contract = findContract (subscription, msg);
            
            switch (msgType)
            {
                case MamaMsgType.TYPE_INITIAL:
                case MamaMsgType.TYPE_RECAP:
                    handleQuoteMsg (contract, subscription, msg, msgType);
                    handleTradeMsg (contract, subscription, msg, msgType);
                    break;
                case MamaMsgType.TYPE_QUOTE:
                    handleQuoteMsg (contract, subscription, msg, msgType);
                    break;
                case MamaMsgType.TYPE_TRADE:
                case MamaMsgType.TYPE_CANCEL:
                case MamaMsgType.TYPE_ERROR:
                case MamaMsgType.TYPE_CORRECTION:
                    handleTradeMsg (contract, subscription, msg, msgType);
                    break;
            }
        }
        catch (MamdaDataException e)
        {
            mLogger.fine ("caught MamdaDataException: " + e +
                              ";  message was " + msg);
        }
    }

    public MamaDateTime getSrcTime()
    {
        return mSrcTime;
    }

    public MamaDateTime getActivityTime()
    {
        return mActTime;
    }

    public long   getEventSeqNum()
    {
        return mEventSeqNum.getValue();
    }

    public MamaDateTime getEventTime()
    {
        return mEventTime;
    }

    public short getSrcTimeFieldState()
    {
      return mSrcTimeFieldState.getState();
    }

    public short getActivityTimeFieldState()
    {
      return mActTimeFieldState.getState();
    }

    public short   getEventSeqNumFieldState()
    {
      return mEventSeqNumFieldState.getState();
    }

    public short getEventTimeFieldState()
    {
      return mEventTimeFieldState.getState();
    }

    public MamdaOptionContract  getOptionContract()
    {
        return mLastActionContract;
    }

    public char  getOptionAction()
    {
        return mLastAction;
    }

    private void handleQuoteMsg (MamdaOptionContract  contract,
                                 MamdaSubscription    subscription,
                                 MamaMsg              msg,
                                 short                msgType)
    {
        MamdaQuoteListener quoteListener = contract.getQuoteListener();
        if (quoteListener != null)
        {
            quoteListener.onMsg (subscription, msg, msgType);
        }
    }

    private void handleTradeMsg (MamdaOptionContract  contract,
                                 MamdaSubscription    subscription,
                                 MamaMsg              msg,
                                 short                msgType)
    {
        MamdaTradeListener tradeListener = contract.getTradeListener();
        if (tradeListener != null)
        {
            tradeListener.onMsg (subscription, msg, msgType);
        }
    }

    private char extractExerciseStyle (MamaMsg            msg,
                                       String             fullSymbol)
    {
        char exerciseStyle   = 'Z';
        int exerciseStyleInt = 0;   

        mLogger.fine ("findContract: trying to get exercise style field: " + fullSymbol);
        mTmpField = msg.getField (null, MamdaOptionFields.EXERCISE_STYLE.getFid(), null);

        if (mTmpField == null )
        {
            mLogger.fine("Cannot find exercise style in msg, " + fullSymbol);
        }
        else
        {
            switch(mTmpField.getType())
            {
                case MamaFieldDescriptor.I8:
                case MamaFieldDescriptor.U8:
                case MamaFieldDescriptor.I16:
                case MamaFieldDescriptor.U16:
                    exerciseStyleInt =mTmpField.getU16();
                    switch(exerciseStyleInt)
                    {
                        case 1:
                            // American
                            exerciseStyle = 'A';
                            break;
                        case 2:
                            // European
                            exerciseStyle = 'E';
                            break;
                        case 3:
                            // Capped
                            exerciseStyle = 'C';
                            break;
                        case 99:
                            exerciseStyle = 'Z';
                            break;
                        default:
                            exerciseStyle = 'Z';
                            mLogger.fine("Unhandled value for wExerciseStyle."+exerciseStyleInt);
                            break;
                    }
                    break;            
                case MamaFieldDescriptor.STRING:
                    String exerciseStyleStr = mTmpField.getString();
                    char exStyleChar = exerciseStyleStr.charAt(0);
                    switch (exStyleChar)
                    {
                        case '1':
                        case 'A':
                            // American
                            exerciseStyle = 'A';
                            break;
                        case '2':
                        case 'E':
                            // European
                            exerciseStyle = 'E';
                            break;
                        case '3':
                        case 'C':
                            // Capped
                            exerciseStyle = 'C';
                            break;
                        default:
                            exerciseStyle = 'Z';
                            if (exerciseStyleStr.equals("99") && exerciseStyleStr.equals("Z"))
                            {
                                mLogger.fine("Unhandled value for wExerciseStyle." + exerciseStyleStr);
                            }
                            break;
                    }
                    break;
                default:
                    exerciseStyle = 'Z';
                    mLogger.fine ("Unhandled type for wExerciseStyle. Expected string or integer but returned:" + 
                                  mTmpField.getType());
                    break;
            }
        }
        return  exerciseStyle;
    }

    private char extractPutCall (MamaMsg        msg,
                                 String         fullSymbol)
    {
        char putCall    = 'Z';
        int  putCallInt = 0;
        mTmpField = msg.getField (null, MamdaOptionFields.PUT_CALL.getFid(), null);

        if (mTmpField == null)
        {        
            mLogger.fine("findContract:CANNOT find put/call in msg:" + fullSymbol);
        }
        else
        {
            switch(mTmpField.getType())
            {
                case MamaFieldDescriptor.I8:
                case MamaFieldDescriptor.U8:
                case MamaFieldDescriptor.I16:
                case MamaFieldDescriptor.U16:       
                    putCallInt = mTmpField.getU16();
                    switch(putCallInt)
                    {
                        case 1:
                            putCall = 'P';
                            break;
                        case 2:
                            putCall = 'C';
                            break;
                        case 99:
                            putCall = 'Z';
                            break;
                        default:
                            putCall = 'Z';                       
                            mLogger.fine("Unhandled value for wPutCall."+ putCallInt);
                            break;
                    }
                    break; 
                case MamaFieldDescriptor.STRING:
                    String putCallStr = mTmpField.getString();
                    char putCallChar = putCallStr.charAt(0);
                    switch (putCallChar)
                    {
                        case '1':
                        case 'P':
                            putCall = 'P';
                            break;
                        case '2':
                        case 'C':
                            putCall = 'C';
                            break;
                        default:
                            putCall = 'Z';
                            if ((putCallStr.equals("99")) && (putCallStr.equals("Z")))
                            {
                                mLogger.fine("Unhandled value for wPutCall."+ putCallStr);

                            }
                            break;
                    }
                    break;
                default:
                    putCall = 'Z';               
                    mLogger.fine ("Unhandled type for wPutCall. Expected string or integer but returned: " + 
                                  mTmpField.getType());
                    break;
            }
        }
        return putCall;
    }

    private MamdaOptionContract findContract (MamdaSubscription  subscription,
                                              MamaMsg            msg)
    {
        /*
         * NOTE: fields which are enums can be pubished as integers if feedhandler
         * uses mama-publish-enums-as-ints.  It may also be possible for a feed to
         * publish the numerical value as a string. All enumerated fields must be handled
         * by getting the value based on the field type.
         */
    
        // Look up the strike price and expiration date
        if (!msg.tryString (MamdaOptionFields.CONTRACT_SYMBOL, contractSymbol))
        {
            throw new MamdaDataException ("cannot find contract symbol");
        }
        mLogger.finer ("contract=" + contractSymbol);

        String fullSymbol = contractSymbol.getValue();
        MamdaOptionContract contract = mChain.getContract (fullSymbol);

        if (contract == null)
        {
            if (!msg.tryDateTime (MamdaOptionFields.EXPIRATION_DATE,expireDateField))
            {
                mLogger.fine ("findContract: CANNOT find expiration date in msg");
                gotExpireDate = false;
            }
            msg.tryF64 (MamdaOptionFields.STRIKE_PRICE, strikePrice);

            int symbolLen = fullSymbol.length();
            String symbol   = null;
            String exchange = null;
            int    dotIndex = fullSymbol.lastIndexOf('.');

            if (dotIndex > 0)
            {
                // Have exchange in symbol.
                exchange = fullSymbol.substring(dotIndex+1);
                symbol   = fullSymbol.substring(0,dotIndex);
            }
            else
            {
                exchange = "";
                symbol = new String(fullSymbol);
            }

            if (gotExpireDate)
            {
                try
                {
                    expireDate = mDateFormat.parse(expireDateField.getAsString());
                }
                catch (ParseException e)
                {
                    throw new MamdaDataException (
                        "cannot parse expiration date: " + expireDateField.getAsString());
                }
                mLogger.finer ("expireDate=" + expireDate + ", strikePrice=" +
                    strikePrice + ", p/c=" + putCall);
            }
            else
            {
                mLogger.finer ("expireDate= NULL, strikePrice=" +
                           strikePrice + ", p/c=" + putCall);
            } 
            char putCallChar = extractPutCall(msg, fullSymbol);
            contract = new MamdaOptionContract (symbol,
                                                exchange,
                                                expireDate,
                                                strikePrice.getValue(),
                                                putCallChar);

            char exerciseStyleChar = extractExerciseStyle (msg, fullSymbol);
            contract.setExerciseStyle (exerciseStyleChar);

            if (msg.tryU32 (MamdaOptionFields.OPEN_INTEREST, openInterest))
            {
                contract.setOpenInterest(openInterest.getValue());
            }

            mChain.addContract (fullSymbol, contract);
            mLastActionContract = contract;
            mLastAction = ACTION_ADD;
            Iterator i = mHandlers.iterator();
            while (i.hasNext())
            {
                MamdaOptionChainHandler handler =
                    (MamdaOptionChainHandler) i.next();
                handler.onOptionContractCreate (subscription, this,
                                                msg, contract, mChain);
            }
        }
        return contract;
    }

    private String getFieldAsString(MamaMsgField field)
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
}
