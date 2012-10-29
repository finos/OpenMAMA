using System;
using System.Collections.Generic;
using System.Collections;
using NUnit.Framework;
using Wombat;

namespace NUnitTest
{
    [TestFixture]
    public class MamdaTradeListenerShortSaleTests
    {
        public MamdaSubscription mSubscription = null;
        public MamdaTradeListener mTradeListener = null;
        public MamaMsg mMsg = null;
        public MamaBridge myBridge = null;
        public MamaDictionary mDictionary = null;
        public tradetickerCB ticker = null;

        [SetUp]
        public void SetUp()
        {
            try
            {
                //Intialise mama
                myBridge = new MamaBridge("lbm");
                Mama.open();

                mDictionary = new MamaDictionary();
                mDictionary.create("dictionary.txt");

                MamdaCommonFields.setDictionary(mDictionary, null);
                MamdaTradeFields.reset();
                MamdaTradeFields.setDictionary(mDictionary, null);

                mSubscription = new MamdaSubscription();
                mTradeListener = new MamdaTradeListener();
                mSubscription.addMsgListener(mTradeListener);

                ticker = new tradetickerCB();
                mTradeListener.addHandler(ticker);
            }
            catch (Exception e)
            {
                Console.Error.WriteLine(e.ToString());
                Environment.Exit(1);
            }
        }

        [Test]
        public void testShortSaleCircuitBreaker()
        {
            mMsg = new MamaMsg();
            addMamaHeaderFields(mMsg,
                                (byte)mamaMsgType.MAMA_MSG_TYPE_TRADE,
                                (byte)mamaMsgStatus.MAMA_MSG_STATUS_OK,
                                (uint)1);
            mMsg.addChar("wShortSaleCircuitBreaker", 5248, 'C');
            ticker.callMamdaOnMsg(mSubscription, mMsg);
            Assert.AreEqual('C', ticker.myShortSaleCircuitBreaker);
		}

        [Test]
        public void testOrigShortSaleCircuitBreaker()
        {
            mMsg = new MamaMsg();
            addMamaHeaderFields(mMsg,
                                (byte)mamaMsgType.MAMA_MSG_TYPE_CANCEL,
                                (byte)mamaMsgStatus.MAMA_MSG_STATUS_OK,
                                (uint)1);
            mMsg.addChar("wOrigShortSaleCircuitBreaker", 5249, 'C');
            ticker.callMamdaOnMsg(mSubscription, mMsg);
            Assert.AreEqual('C', ticker.myOrigShortSaleCircuitBreaker);
        }

        [Test]
        public void testCorrShortSaleCircuitBreaker()
        {
            mMsg = new MamaMsg();
            addMamaHeaderFields(mMsg,
                                (byte)mamaMsgType.MAMA_MSG_TYPE_CORRECTION,
                                (byte)mamaMsgStatus.MAMA_MSG_STATUS_OK,
                                (uint)1);
            mMsg.addChar("wCorrShortSaleCircuitBreaker", 5250, 'C');
            ticker.callMamdaOnMsg(mSubscription, mMsg);
            Assert.AreEqual('C', ticker.myCorrShortSaleCircuitBreaker);
        }
           
        public void addMamaHeaderFields(
                            MamaMsg msg,
                            byte msgType,
                            byte msgStatus,
                            uint seqNum)
        {
            msg.addU8(null, 1, msgType);
            msg.addU8(null, 2, msgStatus);
            msg.addU32(null, 10, seqNum);
            msg.addU64(null, 20, 1);
        }

        public class tradetickerCB : MamdaTradeHandler,
                                     MamdaErrorListener,
                                     MamdaStaleListener
        {
            public char myShortSaleCircuitBreaker;
            public char myOrigShortSaleCircuitBreaker;
            public char myCorrShortSaleCircuitBreaker;

            public void callMamdaOnMsg(MamdaSubscription sub, MamaMsg msg)
            {
                try
                {
                        ArrayList msgListeners = new ArrayList();
                        msgListeners = sub.getMsgListeners();
                        IEnumerator iter = msgListeners.GetEnumerator();
                        int size = msgListeners.Count;
                        for (int i = 0; i < size; i++)
                        {
                            iter.MoveNext();
                            MamdaMsgListener listener = (MamdaMsgListener)iter.Current;

                            listener.onMsg(sub, msg, msg.getType());
                        }
                    }
					catch (Exception ex)
                {
                        throw new MamdaDataException(ex.Message, ex);
                    }
            }
			
			public void onTradeRecap(
                MamdaSubscription sub,
                MamdaTradeListener listener,
                MamaMsg msg,
                MamdaTradeRecap recap)
            {
            }

            public void onTradeReport(
                MamdaSubscription sub,
                MamdaTradeListener listener,
                MamaMsg msg,
                MamdaTradeReport trade,
                    MamdaTradeRecap recap)
            {
                myShortSaleCircuitBreaker = trade.getShortSaleCircuitBreaker();
                Console.WriteLine("myShortSaleCircuitBreaker: " + myShortSaleCircuitBreaker);
            }

            public void onTradeGap(
                MamdaSubscription sub,
                MamdaTradeListener listener,
                MamaMsg msg,
                MamdaTradeGap gap,
                MamdaTradeRecap recap)
            {
            }

            public void onTradeCancelOrError(
                MamdaSubscription sub,
                MamdaTradeListener listener,
                MamaMsg msg,
                MamdaTradeCancelOrError cancelOrError,
                MamdaTradeRecap recap)
            {
                myOrigShortSaleCircuitBreaker = cancelOrError.getOrigShortSaleCircuitBreaker();
                Console.WriteLine("myOrigShortSaleCircuitBreaker: " + myOrigShortSaleCircuitBreaker);
            }

            public void onTradeCorrection(
                MamdaSubscription sub,
                MamdaTradeListener listener,
                MamaMsg msg,
                MamdaTradeCorrection correction,
                MamdaTradeRecap recap)
            {
                myCorrShortSaleCircuitBreaker = correction.getCorrShortSaleCircuitBreaker();
                Console.WriteLine("myCorrShortSaleCircuitBreaker: " + myCorrShortSaleCircuitBreaker);
            }

            public void onTradeClosing(
                MamdaSubscription sub,
                MamdaTradeListener listener,
                MamaMsg msg,
                MamdaTradeClosing closing,
                MamdaTradeRecap recap)
            {
            }

            public void onStale(
                MamdaSubscription subscription,
                mamaQuality quality)
            {
            }

            public void onError(
                MamdaSubscription subscription,
                MamdaErrorSeverity severity,
                MamdaErrorCode errorCode,
                String errorStr)
            {
            }
        }
    }
}