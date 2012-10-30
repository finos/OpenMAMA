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

using System;
using System.Collections;

namespace Wombat
{
	/// <summary>
	/// MamdaOptionChainListener is a class that specializes in handling
	/// and managing option chain updates.  Developers provide their own
	/// implementation of the MamdaOptionChainHandler interface and will be
	/// delivered notifications for various types of options-related events.
	/// </summary>
	public class MamdaOptionChainListener : MamdaMsgListener, MamdaOptionSeriesUpdate
	{
		/// <summary>
		/// Create a specialized option chain listener.  This listener
		/// handles option chain updates.
		/// </summary>
		/// <param name="underlyingSymbol"></param>
		public MamdaOptionChainListener(string underlyingSymbol)
		{
			mChain = new MamdaOptionChain(underlyingSymbol);
		}

		/// <summary>
		/// Create a specialized option chain listener.  This listener
		/// handles option chain updates.
		/// </summary>
		/// <param name="chain"></param>
		public MamdaOptionChainListener(MamdaOptionChain chain)
		{
			mChain = chain;
		}

		/// <summary>
		/// Add a specialized option chain handler.
		/// </summary>
		/// <param name="handler"></param>
		public void addHandler(MamdaOptionChainHandler handler)
		{
			mHandlers.Add(handler);
		}

		/// <summary>
		/// Return the option chain associated with this listener.
		/// </summary>
		/// <returns></returns>
		public MamdaOptionChain getOptionChain()
		{
			return mChain;
		}

		/// <summary>
		/// Implementation of MamdaListener interface.
		/// </summary>
		/// <param name="subscription"></param>
		/// <param name="msg"></param>
		/// <param name="msgType"></param>
		public void onMsg(
			MamdaSubscription	subscription,
			MamaMsg				msg,
			mamaMsgType			msgType)
		{
			if(!MamdaOptionFields.isSet())
			{
				return;
			}

			if (msgType == mamaMsgType.MAMA_MSG_TYPE_END_OF_INITIALS)
			{
				foreach (MamdaOptionChainHandler handler in mHandlers)
				{
					handler.onOptionChainRecap(subscription, this, msg, mChain);
				}
				return;
			}

			MamdaOptionContract contract = findContract(subscription, msg);

			switch (msgType)
			{
				case mamaMsgType.MAMA_MSG_TYPE_INITIAL:
				case mamaMsgType.MAMA_MSG_TYPE_RECAP:
					handleQuoteMsg(contract, subscription, msg, msgType);
					handleTradeMsg(contract, subscription, msg, msgType);
					break;
				case mamaMsgType.MAMA_MSG_TYPE_QUOTE:
					handleQuoteMsg(contract, subscription, msg, msgType);
					break;
				case mamaMsgType.MAMA_MSG_TYPE_TRADE:
				case mamaMsgType.MAMA_MSG_TYPE_CANCEL:
				case mamaMsgType.MAMA_MSG_TYPE_ERROR:
				case mamaMsgType.MAMA_MSG_TYPE_CORRECTION:
					handleTradeMsg(contract, subscription, msg, msgType);
					break;
			}
		}

		public DateTime getSrcTime()
		{
			return mSrcTime;
		}

		public DateTime getActivityTime()
		{
			return mActTime;
		}

		public long getEventSeqNum()
		{
			return mEventSeqNum;
		}

		public DateTime getEventTime()
		{
			return mEventTime;
		}

		public MamdaOptionContract getOptionContract()
		{
			return mLastActionContract;
		}

		public MamdaOptionAction getOptionAction()
		{
			return mLastAction;
		}

        /*  Field State Accessors   */
        public MamdaFieldState   getSrcTimeFieldState()
        {
          return mSrcTimeFieldState;
        }

        public MamdaFieldState   getActivityTimeFieldState()
        {
          return mActTimeFieldState;
        }

        public MamdaFieldState   getEventSeqNumFieldState()
        {
          return mEventSeqNumFieldState;
        }

        public MamdaFieldState   getEventTimeFieldState()
        {
          return mEventTimeFieldState;
        }

        public MamdaFieldState getOptionContractFieldState()
        {
          return mLastActionContractFieldState;
        }

        public MamdaFieldState getOptionActionFieldState()
        {
          return mLastActionFieldState;
        }
        /*      End Field State Accessors   */

        private void handleQuoteMsg(
			MamdaOptionContract  contract,
			MamdaSubscription    subscription,
			MamaMsg              msg,
			mamaMsgType          msgType)
		{
			MamdaQuoteListener quoteListener = contract.getQuoteListener();
			if (quoteListener != null)
			{
				quoteListener.onMsg(subscription, msg, msgType);
			}
		}

		private void handleTradeMsg(
			MamdaOptionContract  contract,
			MamdaSubscription    subscription,
			MamaMsg              msg,
			mamaMsgType          msgType)
		{
			MamdaTradeListener tradeListener = contract.getTradeListener();
			if (tradeListener != null)
			{
				tradeListener.onMsg(subscription, msg, msgType);
			}
		}	

        private MamdaOptionContract.ExerciseStyle extractExerciseStyle(
                MamaMsg            msg,
                string             fullSymbol)
            {
                MamdaOptionContract.ExerciseStyle exerciseStyle = MamdaOptionContract.ExerciseStyle.Unknown;
                int exerciseStyleInt = 0;   
               
                if (!msg.tryField (MamdaOptionFields.EXERCISE_STYLE, ref tmpfield_) )
                {
                    Console.WriteLine("findContract:CANNOT find exercisestyle in msg:"+fullSymbol+exerciseStyle);                    
                }
                else
                {
                    switch(tmpfield_.getType())
                    {
                        case mamaFieldType.MAMA_FIELD_TYPE_I8:
                        case mamaFieldType.MAMA_FIELD_TYPE_U8:
                        case mamaFieldType.MAMA_FIELD_TYPE_I16:
                        case mamaFieldType.MAMA_FIELD_TYPE_U16:                
                            exerciseStyleInt =tmpfield_.getU16();
                        switch(exerciseStyleInt)
                        {
                            case 1:                                    
                                exerciseStyle = MamdaOptionContract.ExerciseStyle.American;
                                break;
                            case 2:
                                exerciseStyle = MamdaOptionContract.ExerciseStyle.European;
                                break;
                            case 3:                                    
                                exerciseStyle = MamdaOptionContract.ExerciseStyle.Capped;
                                break;
                            case 99:
                                exerciseStyle = MamdaOptionContract.ExerciseStyle.Unknown;
                                break;
                            default:
                                exerciseStyle = MamdaOptionContract.ExerciseStyle.Unknown;
                                Console.Write("Unhandled value for wExerciseStyle."+exerciseStyleInt);
                                break;
                            }
                            break;       
                            case mamaFieldType.MAMA_FIELD_TYPE_STRING:                      
                            string exerciseStyleStr = tmpfield_.getString();                             ;
                            switch (exerciseStyleStr[0])
                            {
                                case '1':
                                case 'A':                                        
                                    exerciseStyle = MamdaOptionContract.ExerciseStyle.American;
                                    break;
                                case '2':
                                case 'E':                                        
                                    exerciseStyle = MamdaOptionContract.ExerciseStyle.European;
                                    break;
                                case '3':
                                case 'C':                                       
                                    exerciseStyle = MamdaOptionContract.ExerciseStyle.Capped;
                                    break;
                                default:
                                    exerciseStyle = MamdaOptionContract.ExerciseStyle.Unknown;
                                    if ((exerciseStyleStr=="99") && (exerciseStyleStr=="Z"))
                                    {
                                        Console.WriteLine("Unhandled value for wExerciseStyle."+exerciseStyleStr);
                                    }
                                    break;
                            }
                            break;
                        default:
                        exerciseStyle = MamdaOptionContract.ExerciseStyle.Unknown;
                        Console.WriteLine("Unhandled type for wExerciseStyle. Expected string or integer but returned:"+ tmpfield_.getType());
                        break;
                    }
                }
                return  exerciseStyle;
            }
        private MamdaOptionContract.PutOrCall extractPutCall(
                MamaMsg        msg,
                string         fullSymbol)
            {
                MamdaOptionContract.PutOrCall putCall = MamdaOptionContract.PutOrCall.Unknown;
                int putCallInt = 0;               
                if (!msg.tryField (MamdaOptionFields.PUT_CALL, ref tmpfield_))
                {   
                    
                   Console.WriteLine("findContract:CANNOT find put/call in msg:"+fullSymbol+putCall);
                }
                else
                {
                    switch(tmpfield_.getType())
                    {
                        case mamaFieldType.MAMA_FIELD_TYPE_I8:
                        case mamaFieldType.MAMA_FIELD_TYPE_U8:
                        case mamaFieldType.MAMA_FIELD_TYPE_I16:
                        case mamaFieldType.MAMA_FIELD_TYPE_U16:                  
                            putCallInt = tmpfield_.getU16();
                        switch(putCallInt)
                        {
                            case 1:
                                putCall = MamdaOptionContract.PutOrCall.Put;
                                break;
                            case 2:
                                putCall = MamdaOptionContract.PutOrCall.Call;
                                break;
                            case 99:
                                putCall = MamdaOptionContract.PutOrCall.Unknown;
                                break;
                            default:
                                putCall =  MamdaOptionContract.PutOrCall.Unknown;                       
                                Console.WriteLine("Unhandled value for wPutCall."+ putCallInt);
                                break;
                        }
                        break; 
                        case mamaFieldType.MAMA_FIELD_TYPE_STRING:                       
                        string putCallStr = tmpfield_.getString();                      
                        switch (putCallStr[0])
                        {
                            case '1':
                            case 'P':
                                putCall = MamdaOptionContract.PutOrCall.Put;
                                break;
                            case '2':
                            case 'C':
                                putCall = MamdaOptionContract.PutOrCall.Call;
                                break;
                            default:
                                putCall = MamdaOptionContract.PutOrCall.Unknown; 
                                if ((putCallStr=="99") && (putCallStr=="Z"))
                                {
                                    Console.WriteLine("Unhandled value for wPutCall."+ putCallStr);

                                }
                                break;
                        }
                        break;
                        default:
                            putCall = MamdaOptionContract.PutOrCall.Unknown;               
                            Console.WriteLine("Unhandled type for wPutCall. Expected string or integer but returned: "+tmpfield_.getType());
                            break;
                    }
                }
                return putCall;
            }

        private MamdaOptionContract findContract(
            MamdaSubscription  subscription,
            MamaMsg            msg)
        {
            /*
            * NOTE: fields which are enums can be pubished as integers if feedhandler
            * uses mama-publish-enums-as-ints.  It may also be possible for a feed to
            * publish the numerical value as a string. All enumerated fields must be handled
            * by getting the value based on the field type.
            */

            // Look up the strike price and expiration date
            string contractSymbol = null;
            if (!msg.tryString(MamdaOptionFields.CONTRACT_SYMBOL, ref contractSymbol))
            {
                throw new MamdaDataException ("cannot find contract symbol");
            }

            string fullSymbol = contractSymbol;
            MamdaOptionContract contract = mChain.getContract(fullSymbol);
            if (contract == null)
            {
                string expireDateStr = String.Empty;
                double strikePrice = 0.0;
                string putCall = String.Empty;
                uint openInterest = 0;
                
                msg.tryString(MamdaOptionFields.EXPIRATION_DATE, ref expireDateStr);
                msg.tryF64(MamdaOptionFields.STRIKE_PRICE, ref strikePrice);

                if (msg.tryField (MamdaOptionFields.PUT_CALL, ref tmpfield_))
                {
                    putCall = getFieldAsString(tmpfield_);
                }

                int symbolLen = fullSymbol.Length;
                string symbol   = null;
                string exchange = null;
                int dotIndex = fullSymbol.LastIndexOf('.');
                if (dotIndex > 0)
                {
                    // Have exchange in symbol.
                    exchange = fullSymbol.Substring(dotIndex + 1);
                    symbol   = fullSymbol.Substring(0, dotIndex);
                }
                else
                {
                    exchange = "";
                    symbol = fullSymbol;
                }

                DateTime expireDate = DateTime.MinValue;
                try
                {
                    expireDate = mDateFormat.Parse(expireDateStr);
                }
                catch (FormatException e)
                {
                    throw new MamdaDataException (
                        String.Format("cannot parse expiration date: {0}", expireDateStr));
                }

                MamdaOptionContract.PutOrCall putCallchar = extractPutCall(msg,fullSymbol);
                contract = new MamdaOptionContract(
                    symbol,
                    exchange,
                    expireDate,
                    strikePrice,
                    putCallchar);

               MamdaOptionContract.ExerciseStyle exerciseStyleChar = extractExerciseStyle(msg,fullSymbol);
               contract.setExerciseStyle(exerciseStyleChar);

               
                if (msg.tryU32 (MamdaOptionFields.OPEN_INTEREST, ref openInterest))
                {
                    contract.setOpenInterest( (long)openInterest );
                }

                mChain.addContract(fullSymbol, contract);

                mLastActionContract = contract;
                mLastActionContractFieldState = MamdaFieldState.MODIFIED;
                mLastAction = MamdaOptionAction.Add;
                mLastActionFieldState = MamdaFieldState.MODIFIED;
                foreach (MamdaOptionChainHandler handler in mHandlers)
                {
                    handler.onOptionContractCreate(subscription, this, msg, contract, mChain);
                }
            }
            return contract;
        }

        public string getFieldAsString(MamaMsgField field)
        {
            switch (field.getType())
            {
                case mamaFieldType.MAMA_FIELD_TYPE_I8:
                case mamaFieldType.MAMA_FIELD_TYPE_U8:
                case mamaFieldType.MAMA_FIELD_TYPE_I16:
                case mamaFieldType.MAMA_FIELD_TYPE_U16:
                case mamaFieldType.MAMA_FIELD_TYPE_I32:
                case mamaFieldType.MAMA_FIELD_TYPE_U32:
                    return field.getU32().ToString();
                case mamaFieldType.MAMA_FIELD_TYPE_STRING:
                    return field.getString();
                default:
                    break;
            }
			
			return null;
        }

        private readonly MamdaOptionChain  mChain;
        private readonly ArrayList         mHandlers     = new ArrayList();
        private readonly SimpleDateFormat  mDateFormat   = new SimpleDateFormat("yyyy/MM/dd");

        // MamdaBasicEvent implementation members:
        private readonly DateTime      mSrcTime          = DateTime.MinValue;
        private readonly DateTime      mActTime          = DateTime.MinValue;
        private readonly long          mEventSeqNum      = 0;
        private readonly DateTime      mEventTime        = DateTime.MinValue;

        // MamdaOptionSeriesUpdate implementation members:
        private MamdaOptionContract mLastActionContract  = null;
        private MamdaOptionAction   mLastAction          = MamdaOptionAction.Unknown;

        private MamaMsgField        tmpfield_              = new MamaMsgField();
        
        //FieldState
        private MamdaFieldState  mSrcTimeFieldState        = new MamdaFieldState();
        private MamdaFieldState  mActTimeFieldState        = new MamdaFieldState();
        private MamdaFieldState  mEventSeqNumFieldState    = new MamdaFieldState();
        private MamdaFieldState  mEventTimeFieldState      = new MamdaFieldState();
        private MamdaFieldState  mLastActionContractFieldState  = new MamdaFieldState();
        private MamdaFieldState  mLastActionFieldState          = new MamdaFieldState();
	}
}
