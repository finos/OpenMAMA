using System;
using System.Collections.Generic;
using System.Collections;
using NUnit.Framework;
using Wombat;

namespace NUnitTest
{
	[TestFixture]
    public class MamdaTradeListenerAggressorSideTests
    {
		MamaMsg mMsg = null;
        MamdaSubscription mSubscription = null;
        MamdaTradeListener mTradeListener = null;
        MamdaConcreteTradeRecap recap = null;

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
                MamdaTradeFields.reset();
                MamdaTradeFields.setDictionary(mDictionary, null);

                mSubscription = new MamdaSubscription();
                mTradeListener = new MamdaTradeListener();
                mSubscription.addMsgListener(mTradeListener);
                recap = new MamdaConcreteTradeRecap();
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
        public void testMamdaTradeSide()
        {
            MamdaTradeSide tradeside = new MamdaTradeSide();
            Assert.AreEqual(0, tradeside.getState());
            tradeside.setState((short)1);
            Assert.AreEqual(1, tradeside.getState());
            Assert.AreEqual("Buy", MamdaTradeSide.toString(tradeside.getState()));
        }

        [Test]
        public void testAggressorSide()
        {
            mMsg = new MamaMsg();       
            addMamaHeaderFields(
                                mMsg,
                                (byte)mamaMsgType.MAMA_MSG_TYPE_TRADE,
                                (byte)mamaMsgStatus.MAMA_MSG_STATUS_OK,
                                1);
            mMsg.addChar("wAggressorSide", 4486, '1');
            callMamdaOnMsg(mSubscription, mMsg);
            Assert.AreEqual("1", mTradeListener.getSide());
            Assert.AreEqual(MamdaFieldState.MODIFIED, mTradeListener.getSideFieldState());
            mMsg = null;
        }

        [Test]
        public void testTradeSideString()
        {
            mMsg = new MamaMsg();
            addMamaHeaderFields(
                                mMsg,
                                (byte)mamaMsgType.MAMA_MSG_TYPE_TRADE,
                                (byte)mamaMsgStatus.MAMA_MSG_STATUS_OK,
                                1);
            mMsg.addString("wTradeSide", 1161, "1");
            callMamdaOnMsg(mSubscription, mMsg);
            Assert.AreEqual("1", mTradeListener.getSide());
            Assert.AreEqual(MamdaFieldState.MODIFIED, mTradeListener.getSideFieldState());
            mMsg = null;
        }

        [Test]
        public void testTradeSideInt()
        {
            mMsg = new MamaMsg();
            addMamaHeaderFields(
                                mMsg,
                                (byte)mamaMsgType.MAMA_MSG_TYPE_TRADE,
                                (byte)mamaMsgStatus.MAMA_MSG_STATUS_OK,
                                1);
            mMsg.addU32("wTradeSide", 1161, 1);
            callMamdaOnMsg(mSubscription, mMsg);
            Assert.AreEqual("Buy", mTradeListener.getSide());
            Assert.AreEqual(MamdaFieldState.MODIFIED, mTradeListener.getSideFieldState());
            mMsg = null;
        }                       

        [Test]
        public void testMamdaConcreteTradeRecap()
        {
            mMsg = new MamaMsg();
            addMamaHeaderFields(
                                mMsg,
                                (byte)mamaMsgType.MAMA_MSG_TYPE_TRADE,
                                (byte)mamaMsgStatus.MAMA_MSG_STATUS_OK,
                                1);
            mMsg.addU32("wTradeSide", 1161, 1);
            callMamdaOnMsg(mSubscription, mMsg);
            mTradeListener.populateRecap(recap);
            Assert.AreEqual("Buy", recap.getSide());
            Assert.AreEqual(MamdaFieldState.MODIFIED, recap.getSideFieldState());
        }                      
    }
}
