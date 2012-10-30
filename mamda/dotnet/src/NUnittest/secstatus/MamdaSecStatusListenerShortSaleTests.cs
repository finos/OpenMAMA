using System;
using System.Collections.Generic;
using System.Collections;
using NUnit.Framework;
using Wombat;

namespace NUnitTest
{
	[TestFixture]
    public class MamdaSecStatusListenerShortSaleTests
    {
        MamaMsg mMsg = null;
        MamdaSubscription mSubscription = null;
        MamdaSecurityStatusListener mSecStatusListener = null;
          
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
                MamdaSecurityStatusFields.reset();
                MamdaSecurityStatusFields.setDictionary(mDictionary, null);

                mSubscription = new MamdaSubscription();                             
            }
            catch (Exception ex)
            {
                throw new MamdaDataException(ex.Message, ex);
            }
        }

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
        public void testNotInEffect()
        {
            mSecStatusListener = new MamdaSecurityStatusListener();
            mSubscription.addMsgListener(mSecStatusListener);            
            mMsg = new MamaMsg();
            addMamaHeaderFields(mMsg,
                                (byte)mamaMsgType.MAMA_MSG_TYPE_SEC_STATUS,
                                (byte)mamaMsgStatus.MAMA_MSG_STATUS_OK,
                                (uint)1);               
            callMamdaOnMsg(mSubscription, mMsg);
            Assert.AreEqual(' ', mSecStatusListener.getShortSaleCircuitBreaker());
            Assert.AreEqual(MamdaFieldState.NOT_INITIALISED, 
                                mSecStatusListener.getShortSaleCircuitBreakerFieldState());
            mMsg = null;
            mSecStatusListener = null;
        }

        [Test]
        public void testCached()
        {
            mSecStatusListener = new MamdaSecurityStatusListener();
            mSubscription.addMsgListener(mSecStatusListener);          
              
            mMsg = new MamaMsg();
            addMamaHeaderFields(mMsg,
                                (byte)mamaMsgType.MAMA_MSG_TYPE_SEC_STATUS,
                                (byte)mamaMsgStatus.MAMA_MSG_STATUS_OK,
                                (uint)1);
            mMsg.addChar(null, 5248, 'A');
            callMamdaOnMsg(mSubscription, mMsg);
            Assert.AreEqual('A', mSecStatusListener.getShortSaleCircuitBreaker());
            Assert.AreEqual(MamdaFieldState.MODIFIED,
                                mSecStatusListener.getShortSaleCircuitBreakerFieldState());
            mMsg = null;
            mSecStatusListener = null;
        }

        [Test]
        public void testUpdated()
        {
            mSecStatusListener = new MamdaSecurityStatusListener();
            mSubscription.addMsgListener(mSecStatusListener);            
                
            mMsg = new MamaMsg();
            addMamaHeaderFields(mMsg,
                                (byte)mamaMsgType.MAMA_MSG_TYPE_SEC_STATUS,
                                (byte)mamaMsgStatus.MAMA_MSG_STATUS_OK,
                                (uint)1);
            mMsg.addChar(null, 5248, 'C');
            callMamdaOnMsg(mSubscription, mMsg);
            Assert.AreEqual('C', mSecStatusListener.getShortSaleCircuitBreaker());
            Assert.AreEqual(MamdaFieldState.MODIFIED,
                                mSecStatusListener.getShortSaleCircuitBreakerFieldState());

            mMsg = null;
            mMsg = new MamaMsg();
            addMamaHeaderFields(mMsg,
                                (byte)mamaMsgType.MAMA_MSG_TYPE_SEC_STATUS,
                                (byte)mamaMsgStatus.MAMA_MSG_STATUS_OK,
                                (uint)2);
            callMamdaOnMsg(mSubscription, mMsg);
            Assert.AreEqual(MamdaFieldState.NOT_MODIFIED, 
                            mSecStatusListener.getShortSaleCircuitBreakerFieldState());
            mMsg = null;

            mMsg = new MamaMsg();
            addMamaHeaderFields(mMsg,
                                (byte)mamaMsgType.MAMA_MSG_TYPE_SEC_STATUS,
                                (byte)mamaMsgStatus.MAMA_MSG_STATUS_OK,
                                (uint)3);
            mMsg.addChar(null, 5248, 'D');
            callMamdaOnMsg(mSubscription, mMsg);
            Assert.AreEqual('D', mSecStatusListener.getShortSaleCircuitBreaker());
            Assert.AreEqual(MamdaFieldState.MODIFIED,
                                mSecStatusListener.getShortSaleCircuitBreakerFieldState());
            mMsg = null;
            mSecStatusListener = null;
        }     
    }
}

