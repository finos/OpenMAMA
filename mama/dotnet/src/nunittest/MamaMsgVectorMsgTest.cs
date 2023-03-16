using System;
using System.Text;
using System.Collections.Generic;
using NUnit.Framework;
using Wombat;

namespace NUnitTest
{
    [TestFixture]
    public class MamaMsgVectorMsgTest
    {
        #region Private Constants Variables

        /// <summary>
        /// The number of messages to create in the test.
        /// </summary>
        private const int m_numberMessages = 10;

        #endregion

        #region Private Member Variables

		private MamaBridge mBridge;

        /// <summary>
        /// The message to tests.
        /// </summary>
        private MamaMsg m_msg;

        /// <summary>
        /// Vector of messages to use in the test.
        /// </summary>
        private MamaMsg[] m_vectorMsg;

        /// <summary>
        /// A second vector of messages.
        /// </summary>
        private MamaMsg[] m_vectorMsg2;

        #endregion

        #region Setup and Teardown

        [SetUp]
        public void Setup()
        {
            MamaCommon.getCmdLineArgs();

			mBridge = Mama.loadBridge (MamaCommon.middlewareName);
			Mama.open ();

            // Create the message
            m_msg = new MamaMsg();

            // Create the array of messages
            m_vectorMsg = InitialiseMessageArray(m_numberMessages, 69);
            m_vectorMsg2 = InitialiseMessageArray(m_numberMessages, 144);
        }


        [TearDown]
        public void Teardown()
        {
            // Reset member variables
            if (m_msg != null)
            {
                m_msg.destroy();
                m_msg = null;
            }
        }

        #endregion

        #region Private Operations

        private void CheckMessageArray(MamaMsg[] messages, int numberMessages, int offset)
        {
            // Verify that the array has the correct number of messages
            if (messages.Length != numberMessages)
            {
                throw new ArgumentOutOfRangeException("numberMessages");
            }

            // Chech each item in the array
            for (int nextMessage = 0; nextMessage < numberMessages; nextMessage++)
            {
                // Get the int32 field
                int intValue = messages[nextMessage].getI32(string.Empty, (ushort)(nextMessage + 1));

                // Verify that it has the correct value
                if (intValue != (nextMessage + offset))
                {
                    throw new InvalidOperationException();
                }
            }
        }

        private MamaMsg[] InitialiseMessageArray(int numberMessages, int offset)
        {
            // Create the array of messages
            MamaMsg[] ret = new MamaMsg[numberMessages];
            for (int nextMessage = 0; nextMessage < numberMessages; nextMessage++)
            {
                // Allocate the message
                ret[nextMessage] = new MamaMsg();

                // Add a field
                ret[nextMessage].addI32(null, (ushort)(nextMessage + 1), (int)(nextMessage + offset));
            }

            return ret;
        }

        #endregion

        #region Tests - AddVectorMsg

        [Test]
        public void AddVectorMsgNullArgument()
        {
            // Call the function with a null array
            Assert.Throws<ArgumentNullException>(() => m_msg.addVectorMsg(null, 1, null));
        }

        [Test]
        public void AddVectorMsgInvalidArgument()
        {
            // Create an empty array
            MamaMsg[] emptyArray = new MamaMsg[0];

            // Call the function with an invalid array
            Assert.Throws<ArgumentOutOfRangeException>(() => m_msg.addVectorMsg(null, 1, emptyArray));
        }

        [Test]
        public void TestAddVectorMsg()
        {
            // Add the first array
            m_msg.addVectorMsg(null, 1, m_vectorMsg);

            // Get the array back
            MamaMsg[] messages = m_msg.getVectorMsg(null, 1);

            // Verify that the array is valid
            CheckMessageArray(messages, m_numberMessages, 69);
        }

        #endregion

        #region Tests - UpdateVectorMsg

        [Test]
        public void UpdateVectorMsgNullArgument()
        {
            // Call the function with a null array
            Assert.Throws<ArgumentNullException>(() => m_msg.updateVectorMsg(null, 1, null));
        }

        [Test]
        public void UpdateVectorMsgInvalidArgument()
        {
            // Create an empty array
            MamaMsg[] emptyArray = new MamaMsg[0];

            // Call the function with an invalid array
            Assert.Throws<ArgumentOutOfRangeException>(() => m_msg.updateVectorMsg(null, 1, emptyArray));
        }

        [Test]
        public void UpdateAddVectorMsg()
        {
            // Add the first array
            m_msg.addVectorMsg(null, 1, m_vectorMsg);

            // Get the array back
            MamaMsg[] messages = m_msg.getVectorMsg(null, 1);

            // Verify that the array is valid
            CheckMessageArray(messages, m_numberMessages, 69);

            // Update using the second array
            m_msg.updateVectorMsg(string.Empty, (ushort)1, m_vectorMsg2);

            // Get the array back
            messages = m_msg.getVectorMsg(null, 1);

            // Verify that the array is valid
            CheckMessageArray(messages, m_numberMessages, 144);
        }

        #endregion
    }
}
