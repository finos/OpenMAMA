using System;
using NUnit.Framework;
using Wombat;

namespace NUnitTest
{
    [TestFixture]
    public class MamaMsgVectorPriceTest
    {
        #region Private Constants Variables

        /// <summary>
        /// The number of prices to create in the test.
        /// </summary>
        private const int m_numberPrices = 10;

        #endregion

        #region Private Member Variables

        /// <summary>
        /// The message to tests.
        /// </summary>
        private MamaMsg m_msg;

        /// <summary>
        /// Vector of prices to use in the test.
        /// </summary>
        private MamaPrice[] m_vectorPrice;

        /// <summary>
        /// A second vector of prices.
        /// </summary>
        private MamaPrice[] m_vectorPrice2;

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
            m_vectorPrice = InitialisePriceArray(m_numberPrices, 69);
            m_vectorPrice2 = InitialisePriceArray(m_numberPrices, 144);
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
			
			Mama.close ();
        }

        #endregion

        #region Private Operations

        private void CheckPriceArray(MamaPrice[] prices, int numberPrices, int offset)
        {
            // Verify that the array has the correct number of messages
            if (prices.Length != numberPrices)
            {
                throw new ArgumentOutOfRangeException("numberPrices");
            }

            // Chech each item in the array
            for (int nextPrice = 0; nextPrice < numberPrices; nextPrice++)
            {
                // Verify that it has the correct value
                if(prices[nextPrice].getValue() != (nextPrice + offset))                
                {
                    throw new InvalidOperationException();
                }
            }
        }

        private MamaPrice[] InitialisePriceArray(int numberPrices, int offset)
        {
            // Create the array of messages
            MamaPrice[] ret = new MamaPrice[numberPrices];
            for (int nextPrice = 0; nextPrice < numberPrices; nextPrice++)
            {
                // Allocate the price
                ret[nextPrice] = new MamaPrice();

                // Add a field
                ret[nextPrice].setValue((nextPrice + offset));
            }

            return ret;
        }

        #endregion

        #region Tests - AddVectorPrice

        [Test]
        public void AddVectorPriceNullArgument()
        {
            // Call the function with a null array
            Assert.Throws<ArgumentNullException>(() => m_msg.addVectorPrice(null, 1, null));
        }

        [Test]
        public void AddVectorPriceInvalidArgument()
        {
            // Create an empty array
            MamaPrice[] emptyArray = new MamaPrice[0];

            // Call the function with an invalid array
            Assert.Throws<ArgumentOutOfRangeException>(() => m_msg.addVectorPrice(null, 1, emptyArray));
        }

        #endregion

        #region Tests - UpdateVectorMsg

        [Test]
        public void UpdateVectorPriceNullArgument()
        {
            // Call the function with a null array
            Assert.Throws<ArgumentNullException>(() => m_msg.updateVectorPrice(null, 1, null));
        }

        [Test]
        public void UpdateVectorPriceInvalidArgument()
        {
            // Create an empty array
            MamaPrice[] emptyArray = new MamaPrice[0];

            // Call the function with an invalid array
            Assert.Throws<ArgumentOutOfRangeException>(() => m_msg.updateVectorPrice(null, 1, emptyArray));
        }

        #endregion
    }
}
