/*
 * This file tests the log file policy.
 */
using NUnit.Framework;
using System;
using System.IO;
using System.Security.AccessControl;
using Wombat;

namespace NUnitTest
{
    [TestFixture]
    public class MamaSetLogFilePolicyTest : MamaBaseLogTest
    {
        /* ****************************************************** */
        #region Private Member Variables

        /// <summary>
        /// Valid path to a temporary file.
        /// </summary>
        private string m_tempFile;

        #endregion

        /* ****************************************************** */
        #region Setup and Teardown

        public override void Setup()
        {
            // Load a bridge to allow mama to be opened
            Mama.loadBridge(MamaCommon.middlewareName);

            // Open mama to initialise logging
            Mama.open();

            // Create a temporary file
            m_tempFile = Path.GetTempFileName();
                        
            // Set this as the log file
            Mama.logToFile(m_tempFile, MamaLogLevel.MAMA_LOG_LEVEL_NORMAL);

            // Set the size to 1 byte
            Mama.setLogSize(1);

            // Call the base class to establish the log callbacks
            base.Setup();
        }

        public override void Teardown()
        {
            // Call the base class to remove the callbacks
            base.Teardown();

            // Close mama to close the log file
            Mama.close();

            // Close the mama log file handles from native layer
            Mama.logDestroy();

            // Delete the file
            File.Delete(m_tempFile);
        }

        #endregion

        /* ****************************************************** */
        #region Private Functions

        private void CheckForRollingLogFile(int index)
        {
            // Format the path to the log file
            string path = string.Format("{0}{1}", m_tempFile, index);

            // Check to see if it exists
            if (!File.Exists(path))
            {
                throw new FileNotFoundException("The rolling log file could not be found.");
            }
        }

        #endregion

        /* ****************************************************** */
        #region Tests

        [Test]
        public void RollPolicyNormal()
        {
            // Set rolling log file policy
            Mama.setLogFilePolicy(MamaLogFilePolicy.LOGFILE_ROLL);

            // Set the max log files to be 10
            Mama.setNumLogFiles(10);

            // Roll the log files by writing 2 logs
            Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_NORMAL, "Test log 1");
            Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_NORMAL, "Test log 2");

            // Verify that the second file has been created
            CheckForRollingLogFile(1);
        }

        [Test]
        public void RollPolicyExceedMax()
        {
            // Set rolling log file policy
            Mama.setLogFilePolicy(MamaLogFilePolicy.LOGFILE_ROLL);

            // Set the max log files to be 10
            Mama.setNumLogFiles(2);

            // Roll the log files by writing 2 logs
            Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_NORMAL, "Test log 1");
            Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_NORMAL, "Test log 2");

            // This third log shoud roll again with the top file being deleted
            Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_NORMAL, "Test log 3");

            // Verify that the third file has been created
            CheckForRollingLogFile(1);

            // Verify that the oldest file contains the right log statement
            string path = string.Format("{0}1", m_tempFile);
            TestLogFile(path, "Test log 2");
        }

        [Test]
        public void RollPolicyZeroMax()
        {
            // Set rolling log file policy
            Mama.setLogFilePolicy(MamaLogFilePolicy.LOGFILE_ROLL);

            // Set the max log files to be 0
            Assert.Throws<ArgumentOutOfRangeException>(() => Mama.setNumLogFiles(0));
        }

        [Test]
        public void RollPolicyInvalidMax()
        {
            // Set rolling log file policy
            Mama.setLogFilePolicy(MamaLogFilePolicy.LOGFILE_ROLL);

            // Set the max log files to be negative
            Assert.Throws<ArgumentOutOfRangeException>(() => Mama.setNumLogFiles(-16));
        }

        [Test]
        public void OverwritePolicyNormal()
        {
            // Set rolling log file policy
            Mama.setLogFilePolicy(MamaLogFilePolicy.LOGFILE_OVERWRITE);

            // Roll the log files by writing 2 logs
            Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_NORMAL, "Test log 1");
            Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_NORMAL, "Test log 2");

            // Change the policy to roll
            Mama.setLogFilePolicy(MamaLogFilePolicy.LOGFILE_ROLL);

            // Write another log to roll the file and unlock it so it can be read
            Mama.log(MamaLogLevel.MAMA_LOG_LEVEL_NORMAL, "Test log 3");

            // Verify the text
            string path = string.Format("{0}1", m_tempFile);
            TestLogFile(path, "Test log 2");
        }

        #endregion
    }
}
