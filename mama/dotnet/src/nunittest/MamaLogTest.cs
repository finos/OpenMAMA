using NUnit.Framework;
using System;
using Wombat;

namespace NUnitTest
{
    [TestFixture]
    public class Log : MamaBaseLogTest
    {
        /* ****************************************************** */
        #region Private Operations

        /// <summary>
        /// This function will enumerate all the available log levels and write an entry
        /// at that level.
        /// If the supplied target log level is lower or equal to the level at which the
        /// log has been written then a message should be sent to the log callback function.
        /// This function will verify that messages are sent at the correct times.
        /// </summary>
        /// <param name="targetLevel">
        /// The target log level.
        /// </param>
        /// <exception cref="InvalidOperationException">
        /// Thrown if a log is written or not written at the wrong time.
        /// </exception>
        private void CheckLogStatement(MamaLogLevel targetLevel)
        {
            // The test log statement
            string testLog = "This is a test";

            // Enumerate all the log levels
            foreach (MamaLogLevel level in Enum.GetValues(typeof(MamaLogLevel)))
            {
                // Ignore Off
                if (level == MamaLogLevel.MAMA_LOG_LEVEL_OFF)
                {
                    continue;
                }

                // Clear the log buffer
                m_callback.Buffer = null;

                // Write a log at this level
                Mama.log(level, testLog);

                // Verify that the log has been written
                if (level <= targetLevel)
                {
                    // If the log statement isn't correct then throw an exception
                    if (string.Compare(testLog, m_callback.Buffer, false) != 0)
                    {
                        throw new InvalidOperationException("Log entry text has become corrupt.");
                    }
                }

                    /* If the target level is lower than the level that the log has been written
                     * then the log should be null.
                     */
                else
                {
                    if (m_callback.Buffer != null)
                    {
                        throw new InvalidOperationException("A log should not be written at this level.");
                    }
                }
            }
        }

        #endregion

        /* ****************************************************** */
        #region Tests

        [Test]
        [ExpectedException(typeof(ArgumentOutOfRangeException))]
        public void InvalidLogLevel()
        {
            // Create an invalid log level
            MamaLogLevel logLevel = (MamaLogLevel)19;

            // Log with an invalid level
            Mama.log(logLevel, "This is a test");
        }

        [Test]
        public void BlankLogMessage()
        {
            // Set the log level to warn
            Mama.setLogLevel(MamaLogLevel.MAMA_LOG_LEVEL_WARN);

            // Log with a blank message
            Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_WARN, string.Empty);

            // Verify that the buffer is empty
            if (m_callback.Buffer != string.Empty)
            {
                throw new InvalidOperationException();
            }
        }

        [Test]
        public void NullLogMessage()
        {
            // Set the log level to warn
            Mama.setLogLevel(MamaLogLevel.MAMA_LOG_LEVEL_WARN);

            // Log with a null message, note that this won't throw an exception
            Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_WARN, null);

            // Verify that the buffer is empty
            if (m_callback.Buffer != string.Empty)
            {
                throw new InvalidOperationException();
            }
        }

        [Test]
        public void LargeStringTest()
        {
            // Set the log level to warn
            Mama.setLogLevel(MamaLogLevel.MAMA_LOG_LEVEL_WARN);

            // Create an array of unicode characters that will constitute the string
            char[] chars = new char[5096];
            for(int index=0; index<5096; index++)
            {
                chars[index] = 'g';
            }

            // Create a corresponding string
            string largeString = new string(chars);

            // Log the string
            Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_WARN, largeString);

            // Verify that the length is correct
            if (m_callback.Buffer.Length != 4095)
            {
                throw new InvalidOperationException();
            }
        }

        [Test]
        public void LogLevels()
        {
            // Enumerate all the available log levels
            foreach (MamaLogLevel level in Enum.GetValues(typeof(MamaLogLevel)))
            {
                // Set the logging to this level
                Mama.setLogLevel(level);

                // Verify that logs are correctly written for this level
                CheckLogStatement(level);

                // Disable logging
                Mama.disableLogging();
            }
        }

        #endregion
    }
}
