using NUnit.Framework;
using System;
using System.IO;
using Wombat;

namespace NUnitTest
{
    /// <summary>
    /// This class provide base functionality needed by all of the logging tests.
    /// It will create a TestLogCallback instance and install this with MAMA so
    /// that it is then invoke every time a log entry is written.
    /// </summary>
    public class MamaBaseLogTest
    {
        /* ****************************************************** */
        #region Protected Member Variables

        /// <summary>
        /// Used to receive log messages.
        /// </summary>
        protected MamaLogCallbackTest m_callback;

        #endregion

        /* ****************************************************** */
        #region Setup and Teardown

        [SetUp]
        public virtual void Setup()
        {
            // Create the callback class
            m_callback = new MamaLogCallbackTest();

            // Install the callback as the log function
            Mama.setLogCallback(m_callback);

            // Install the callback as the log size exceeded function
            Mama.setLogSizeCb(m_callback);
        }

        [TearDown]
        public virtual void Teardown()
        {
            // Reset member variables
            m_callback = null;
        }

        #endregion

        /* ****************************************************** */
        #region Protected Functions

        protected void TestLogFile(string file, string text)
        {
            // Open the file for reading
            using (StreamReader reader = File.OpenText(file))
            {
                // Read the first line from the file
                string fileMessage = reader.ReadLine();

                // Check this against the one written above, note that a time stamp will be written as well
                if (fileMessage.IndexOf(text) == -1)
                {
                    throw new InvalidOperationException("The log message was corrupt.");
                }
            }
        }

        #endregion
    }
}
