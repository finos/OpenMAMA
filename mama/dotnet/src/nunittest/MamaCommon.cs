using System;
using System.IO;

namespace NUnitTest
{
    /// <summary>
    /// This class provides access to cmd line args.
    /// </summary>
    public class MamaCommon
    {
        /* ****************************************************** */
        public static string middlewareName { get; private set; }

        public static string transportName { get; private set; }

        public static string source { get; private set; }

        public static string symbol { get; private set; }

        /* ****************************************************** */
        public static void getCmdLineArgs()
        {
            // nunit does not support cmd line args, so .....

            middlewareName = Environment.GetEnvironmentVariable("middlewareName");
            transportName = Environment.GetEnvironmentVariable("transportName");
            source = Environment.GetEnvironmentVariable("source");
            symbol = Environment.GetEnvironmentVariable("symbol");
        }
    }
}
