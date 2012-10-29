using NUnit.Framework;
using System;
using System.IO;
using System.Security.AccessControl;
using Wombat;

namespace NUnitTest
{
    [TestFixture]
    public class MamaLogToFileTest : MamaBaseLogTest
    {
        /* ****************************************************** */
        #region Setup and Teardown

        public override void Setup()
        {
            // Load the bridge
            Mama.loadBridge("lbm");

            // Open mama to initialise all logging
            Mama.open();

            // Don't call the base class
        }

        public override void Teardown()
        {
            // Close mama to clean up logging
            Mama.close();

            // Don't call the base class
        }

        #endregion

        /* ****************************************************** */
        #region Tests

        [Test]
        [ExpectedException(typeof(ArgumentNullException))]
        public void NullFile()
        {
            // Null file name
            Mama.logToFile(null, MamaLogLevel.MAMA_LOG_LEVEL_NORMAL);
        }

        [Test]
        [ExpectedException(typeof(ArgumentNullException))]
        public void BlankFile()
        {
            // Blank file name
            Mama.logToFile(string.Empty, MamaLogLevel.MAMA_LOG_LEVEL_NORMAL);
        }

        [Test]
        [ExpectedException(typeof(ArgumentOutOfRangeException))]
        public void InvalidLogLevel()
        {
            // Invalid log level
            MamaLogLevel logLevel = (MamaLogLevel)19;
            Mama.logToFile(Path.GetTempPath(), logLevel);
        }

        [Test]
        [ExpectedException(typeof(ArgumentOutOfRangeException))]
        public void InvalidPathCharacters()
        {
            // Invalid characters
            Mama.logToFile("<>", MamaLogLevel.MAMA_LOG_LEVEL_NORMAL);
        }

        [Test]
        public void LoggingToFile()
        {   
            // Ensure logging to file is off
            if (Mama.loggingToFile())
            {
                throw new InvalidOperationException("Logging to file should be off.");
            }

            // Turn file logging on
            Mama.logToFile(Path.GetTempFileName(), MamaLogLevel.MAMA_LOG_LEVEL_NORMAL);

            // Ensure logging to file is on
            if (!Mama.loggingToFile())
            {
                throw new InvalidOperationException("Logging to file should be on.");
            }            
        }

        [Test]
        [ExpectedException(typeof(UnauthorizedAccessException))]
        public void ReadOnlyFile()
        {
            // Create a temporary file
            string tempFile = Path.GetTempFileName();
            try
            {
                // Make the file read only
                File.SetAttributes(tempFile, FileAttributes.ReadOnly);

                // Set this as the log file
                Mama.logToFile(tempFile, MamaLogLevel.MAMA_LOG_LEVEL_NORMAL);

                // Write a log
                Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_NORMAL, "This is a test");
            }

            finally
            {
                // Delete the file
                File.Delete(tempFile);
            }
        }

        [Test]
        public void NormalTest()
        {
            // Create a temporary file
            string tempFile = Path.GetTempFileName();
            try
            {
                // This string will contain the log message
                string logMessage = "This is a test";

                // Set this as the log file
                Mama.logToFile(tempFile, MamaLogLevel.MAMA_LOG_LEVEL_NORMAL);

                // Write a log                    
                Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_NORMAL, logMessage);
                
                // Close mama to close the log file
                Mama.close();
                
                // Check the contents of the log file
                TestLogFile(tempFile, logMessage);                
            }

            finally
            {
                // Delete the file
                File.Delete(tempFile);
            }
        }

        #endregion
    }
}
