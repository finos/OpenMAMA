using System;
using Wombat;

namespace NUnitTest
{
    public class MamaLogCallbackTest : MamaLogFileCallback2
    {
        // Used to receive log messages
        private string m_buffer;

        // Indicates if the log size has been exceeded
        private bool m_logSizeExceeded;

        public string Buffer
        {
            get
            {
                return m_buffer;
            }

            set
            {
                m_buffer = value;
            }
        }

        public bool LogSizeExceeded
        {
            get
            {
                return m_logSizeExceeded;
            }

            set
            {
                m_logSizeExceeded = value;
            }
        }

        public void onLog(MamaLogLevel level, string message)
        {
            /* Format the log as a string, do this separately so we can
             * see the string in the debugger.
             */
            m_buffer = message;

            /* Write it out. */
            Console.WriteLine(m_buffer);

            // Call the default function if the message isn't blank
            if (!string.IsNullOrEmpty(message))
            {
                Mama.defaultLogFunction(level, message);
            }
        }

        public void onLogSizeExceeded()
        {
            // Set the flag
            m_logSizeExceeded = true;
        }
    }
}
