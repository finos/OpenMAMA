/// This test will check both SetLogSize and SetLogSizeCb
using NUnit.Framework;
using System;
using System.IO;
using System.Security.AccessControl;
using Wombat;

namespace NUnitTest
{
    [TestFixture]
    public class MamaSetLogSizeTest : MamaBaseLogTest
    {
        /* ****************************************************** */
        #region Setup and Teardown

        public override void Setup()
        {
            // Call the base class to establish the log callbacks
            base.Setup();

            // Set the log file policy to invoke the callback when the size exceeds
            Mama.setLogFilePolicy(MamaLogFilePolicy.LOGFILE_USER);
        }

        #endregion

        /* ****************************************************** */
        #region Tests

        [Test]
        [ExpectedException(typeof(ArgumentOutOfRangeException))]
        public void InvalidSize()
        {
            // Set the size to 0 byte
            Mama.setLogSize(0);
        }
                
        [Test]
        public void SizeReached()
        {
            // Create a temporary file
            string tempFile = Path.GetTempFileName();
            try
            {
                // Load a bridge to allow mama to be opened
                Mama.loadBridge(MamaCommon.middlewareName);

                // Open mama
                Mama.open();
                try
                {
                    // Set this as the log file
                    Mama.logToFile(tempFile, MamaLogLevel.MAMA_LOG_LEVEL_NORMAL);

                    // Set the size to 1 byte
                    Mama.setLogSize(1);

                    // Write a log
                    Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_NORMAL, "This is a test");

                    // Write a second log, this should not invoke the callback
                    Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_NORMAL, "This is a test");

                    // Check that the size has been exceeded
                    if (!m_callback.LogSizeExceeded)
                    {
                        throw new InvalidOperationException("The log size was not exceeded.");
                    }
                }

                finally
                {
                    // Close mama to close the log file
                    Mama.close();
                }                
            }

            finally
            {
                // Close underlying log file handles
                Mama.logDestroy();

                // Delete the file
                File.Delete(tempFile);
            }
        }

        #endregion
    }
}
