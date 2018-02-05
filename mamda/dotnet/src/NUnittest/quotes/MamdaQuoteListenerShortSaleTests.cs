using System;
using System.Collections.Generic;
using System.Collections;
using NUnit.Framework;
using Wombat;

namespace NUnitTest
{
	[TestFixture]
    public class MamdaQuoteListenerShortSaleTests
    {
        MamaMsg mMsg = null;
        MamdaSubscription mSubscription = null;
        MamdaQuoteListener mQuoteListener = null;
        QuoteTicker ticker = null;

        [SetUp]
        public void SetUp()
        {
            try
            {
                MamaBridge mBridge = Mama.loadBridge("lbm");
                Mama.open();

                MamaDictionary mDictionary = new MamaDictionary();
                mDictionary.create("dictionary.txt");

                MamdaCommonFields.setDictionary(mDictionary, null);
                MamdaQuoteFields.reset();
                MamdaQuoteFields.setDictionary(mDictionary, null);

                mSubscription = new MamdaSubscription();
                mQuoteListener = new MamdaQuoteListener();
                mSubscription.addMsgListener(mQuoteListener);

                ticker = new QuoteTicker();
                mQuoteListener.addHandler(ticker);


            }
            catch (Exception ex)
            {
                throw new MamdaDataException(ex.Message, ex);
            }
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

        public void clear()
        {
            mMsg.clear();
            addMamaHeaderFields(mMsg,
                                   1,
                                   0,
                                   1);
        }

        [Test]
        public void TestShortSaleCircuitBreaker()
        {
            mMsg = new MamaMsg();

            addMamaHeaderFields(mMsg,
                                (byte)mamaMsgType.MAMA_MSG_TYPE_QUOTE,
                                (byte)mamaMsgStatus.MAMA_MSG_STATUS_OK,
                                (uint)1);
            mMsg.addChar("wShortSaleCircuitBreaker", 5248, 'C');
            ticker.callMamdaOnMsg(mSubscription, mMsg);
            Assert.AreEqual('C', ticker.myShortSaleCircuitBreaker);
        }

        private class QuoteTicker : MamdaQuoteHandler,
                                    MamdaStaleListener,
                                    MamdaErrorListener
        {
            public char myShortSaleCircuitBreaker;

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

              
            public void onQuoteRecap(
                MamdaSubscription subscription,
                MamdaQuoteListener listener,
                MamaMsg msg,
                MamdaQuoteRecap recap)
            {                    
            }

            public void onQuoteUpdate(
                MamdaSubscription subscription,
                MamdaQuoteListener listener,
                MamaMsg msg,
                MamdaQuoteUpdate update,
                MamdaQuoteRecap recap)
            {
                myShortSaleCircuitBreaker = update.getShortSaleCircuitBreaker();
                Console.WriteLine("myShortSaleCircuitBreaker: " + myShortSaleCircuitBreaker);
            }

            public void onQuoteGap(
                MamdaSubscription subscription,
                MamdaQuoteListener listener,
                MamaMsg msg,
                MamdaQuoteGap gap,
                MamdaQuoteRecap recap)
            {                   
            }

            public void onQuoteClosing(
                MamdaSubscription subscription,
                MamdaQuoteListener listener,
                MamaMsg msg,
                MamdaQuoteClosing quoteClosing,
                MamdaQuoteRecap recap)
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
                string errorStr)
            {                  
            }
         
          
    }
}
}

