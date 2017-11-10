using System;
using NUnit.Framework;
using Wombat;

namespace NUnitTest
{
    [TestFixture]
    public class MamaMsgVectorStringTest
    {
        #region Private Constants Variables

        /// <summary>
        /// The number of strings to create in the test.
        /// </summary>
        private const int m_numberStrings = 10;

        #endregion

        #region Private Member Variables

        /// <summary>
        /// The message to test.
        /// </summary>
        private MamaMsg m_msg;

        /// <summary>
        /// Vector of strings to use in the test.
        /// </summary>
        private string[] m_vectorString;

        /// <summary>
        /// A second vector of messages.
        /// </summary>
        private string[] m_vectorString2;

        #endregion

        #region Setup and Teardown

        [SetUp]
        public void Setup()
        {
			Mama.loadBridge (MamaCommon.middlewareName);
			Mama.open ();
			
            // Create the message
            m_msg = new MamaMsg();

            // Create the array of messages
            m_vectorString = InitialiseStringArray(m_numberStrings, 69);
            m_vectorString2 = InitialiseStringArray(m_numberStrings, 144);
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

        private void CheckStringArray(string[] strings, int numberStrings, int offset)
        {
            // Verify that the array has the correct number of messages
            if (strings.Length != numberStrings)
            {
                throw new ArgumentOutOfRangeException("numberStrings");
            }

            // Chech each item in the array
            for (int nextString = 0; nextString < numberStrings; nextString++)
            {
                // Format the required value
                string requiredString = string.Format("Test string {0}", (nextString + offset));

                // Verify that it has the correct value
                if (string.Compare(requiredString, strings[nextString]) != 0)
                {
                    throw new InvalidOperationException();
                }
            }
        }

        private string[] InitialiseStringArray(int numberStrings, int offset)
        {
            // Create the array of messages
            string[] ret = new string[numberStrings];
            for (int nextString = 0; nextString < numberStrings; nextString++)
            {
                // Allocate the string
                ret[nextString] = string.Format("Test string {0}", (nextString + offset));
            }

            return ret;
        }

        #endregion

        #region Tests - AddVectorMsg

        [Test]
        [ExpectedException(typeof(ArgumentNullException))]
        public void AddStringMsgNullArgument()
        {
            // Call the function with a null array
            m_msg.addVectorString(null, 1, null);
        }

        [Test]
        [ExpectedException(typeof(ArgumentOutOfRangeException))]
        public void AddStringMsgInvalidArgument()
        {
            // Create an empty array
            string[] emptyArray = new string[0];

            // Call the function with an invalid array
            m_msg.addVectorString(null, 1, emptyArray);
        }

        [Test]
        public void TestAddVectorString()
        {
            // Add the first array
            m_msg.addVectorString(null, 1, m_vectorString);

            // Get the array back
            string[] strings = m_msg.getVectorString(null, 1);
            
            // Verify that the array is valid
            CheckStringArray(strings, m_numberStrings, 69);
        }

        #endregion

        #region Tests - UpdateVectorMsg

        [Test]
        [ExpectedException(typeof(ArgumentNullException))]
        public void UpdateVectorMsgNullArgument()
        {
            // Call the function with a null array
            m_msg.updateVectorString(null, 1, null);
        }

        [Test]
        [ExpectedException(typeof(ArgumentOutOfRangeException))]
        public void UpdateVectorMsgInvalidArgument()
        {
            // Create an empty array
            string[] emptyArray = new string[0];

            // Call the function with an invalid array
            m_msg.updateVectorString(null, 1, emptyArray);
        }

        [Test]
        public void UpdateAddVectorMsg()
        {
            // Add the first array
            m_msg.addVectorString(null, 1, m_vectorString);

            // Get the array back
            string[] strings = m_msg.getVectorString(null, 1);

            // Verify that the array is valid
            CheckStringArray(strings, m_numberStrings, 69);

            // Update using the second array
            m_msg.updateVectorString(string.Empty, (ushort)1, m_vectorString2);

            // Get the array back
            strings = m_msg.getVectorString(null, 1);

            // Verify that the array is valid
            CheckStringArray(strings, m_numberStrings, 144);
        }

        #endregion
    }
}
