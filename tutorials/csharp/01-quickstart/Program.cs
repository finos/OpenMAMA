using System;
using System.Collections;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using Wombat;


namespace _01_quickstart
{
    class Program
    {
        static void Main(string[] args)
        {
            MamaBridge m_bridge = Mama.loadBridge("qpid");
            Console.WriteLine(Mama.getVersion(m_bridge));
        }
    }
}
