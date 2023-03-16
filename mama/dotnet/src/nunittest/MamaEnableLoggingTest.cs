using NUnit.Framework;
using System;
using Wombat;

namespace NUnitTest
{
    [TestFixture]
    public class EnableLogging : MamaBaseLogTest
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
        #region Setup and Teardown

        public override void Setup()
        {
            // Base class setup
            base.Setup();

            // Enable logging at normal level
            Mama.enableLogging(MamaLogLevel.MAMA_LOG_LEVEL_NORMAL);
        }

        #endregion

        /* ****************************************************** */
        #region Tests

        [Test]
        public void InvalidArguments()
        {
            // Create an invalid log level
            MamaLogLevel logLevel = (MamaLogLevel)19;

            // Attempt to enable logging
            Assert.Throws<ArgumentOutOfRangeException>(() => Mama.setLogLevel(logLevel));
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
