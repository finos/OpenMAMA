/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Technologies, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;

namespace Wombat
{
    #region Public Enumerations

    /// <summary>
	/// Level of logging within MAMA
	/// </summary>
	public enum MamaLogLevel
	{
		/// <summary>
		/// No logging
		/// </summary>
		MAMA_LOG_LEVEL_OFF = 0,
		/// <summary>
		/// Severe log level
		/// </summary>
		MAMA_LOG_LEVEL_SEVERE = 1,
		/// <summary>
		/// Error log level
		/// </summary>
		MAMA_LOG_LEVEL_ERROR = 2,
		/// <summary>
		/// Error log level
		/// </summary>
		MAMA_LOG_LEVEL_WARN = 3,
	    /// <summary>
	    /// Standard log level
	    /// </summary>
		MAMA_LOG_LEVEL_NORMAL = 4,
        /// <summary>
        /// Fine log level
        /// </summary>
        MAMA_LOG_LEVEL_FINE   = 5,
        /// <summary>
        /// Finer log level
        /// </summary>
        MAMA_LOG_LEVEL_FINER  = 6,
        /// <summary>
        /// Finest log level
        /// </summary>
        MAMA_LOG_LEVEL_FINEST = 7
	}

	/// <summary>
	/// Logging policy within MAMA
	/// </summary>
	public enum MamaLogFilePolicy
	{
		/// <summary>
		/// Unbounded log size
		/// </summary>
		LOGFILE_UNBOUNDED = 1,
		/// <summary>
		/// Roll logfiles on max size
		/// </summary>
		LOGFILE_ROLL = 2,
		/// <summary>
		/// Overwrite logfile on max size
		/// </summary>
		LOGFILE_OVERWRITE = 3,
		/// <summary>
		/// Call user implementation on max size
		/// </summary>
		LOGFILE_USER = 4
	}

    #endregion

    /// <summary>
	/// MAMA - Middleware Agnostic Messaging API
	/// </summary>
    /// <remarks>
    /// MAMA provides an
    /// abstraction layer over various messaging middleware platforms. In
    /// particular, MAMA adds market data semantics to messaging platforms
    /// that would otherwise be too limited for use as a market data
    /// distribution middleware. Features that MAMA adds to any messaging
    /// middleware are:
    /// <list type="bullet">
    /// <item>Subscription management (initial values, throttling).</item>
    /// <item>Entitlements/permissioning.</item>
    /// <item>Data quality.</item>
    /// </list>
    /// MAMA currently supports the following middleware platforms:
    /// <list type="bullet">
    /// <item>Wombat TCP Middleware</item>
    /// <item>29West Latency Busters Messaging (see http://29west.com)</item>
    /// <item>Tibco TIB/RV versions 6 and 7 (see http://tibco.com)</item>
    /// </list>
    /// <para>
    /// Future supported platforms may include:
    /// <list type="bullet">
    /// <item>Tibco SmartSockets (see http://tibco.com)</item>
    /// <item>Reuters SSL, SFC, TibMsg, etc.</item>
    /// </list>
    /// </para>
    /// <para>
    /// A higher level market data API is also available: the Middleware
    /// Agnostic Market Data API (MAMDA).  While MAMA provides a
    /// field-based abstraction to market data, MAMDA provides smart,
    /// specialized data types to deal with specific market data events,
    /// such as trades, quotes, order books, etc.  MAMDA is particularly
    /// suitable for applications such as program trading and tick capture
    /// systems, where context is important.  MAMA is more suited to
    /// applications that don't care about the meaning of fields, such as a
    /// simple, field-based market data display application.
    /// </para>
    /// </remarks>
	public class Mama
    {
        /* ************************************************************** */
        #region Class Member Definition

        /* ************************************************************** */
        #region Internal Delegates

        /// <summary>
        /// This delegate is used for communication with the native C function.
        /// </summary>
        /// <param name="level">
        /// The log level, will actually be an entry from the .Net LogLevel
        /// enumeration.
        /// </param>
        /// <param name="message">
        /// The message.
        /// </param>
        internal delegate void LogFileCallbackDelegate(
            int level,
            [MarshalAs(UnmanagedType.LPStr)]
                string message);

        /// <summary>
        /// This delegate is used for communication with the native SetLogSizeCb
        /// function.
        /// </summary>
        internal delegate void LogSizeCallbackDelegate();

        #endregion

        /* ************************************************************** */
        #region Private Member Variables


        /// <summary>
        /// This wrapper is used to store the log file callback.
        /// </summary>
        private static MamaCallbackWrapper<MamaLogFileCallback2, LogFileCallbackDelegate> m_logFileCbWrapper;

        /// <summary>
        /// This wrapper is used to store the log size callback.
        /// </summary>
        private static MamaCallbackWrapper<MamaLogFileCallback, LogSizeCallbackDelegate> m_logSizeCbWrapper;

        #endregion

        #endregion

        public static MamaLogLevel GetLogLevelForString(string level)
        {
	        if (level.Equals("OFF", StringComparison.OrdinalIgnoreCase))
	        {
		        return MamaLogLevel.MAMA_LOG_LEVEL_OFF;
	        }
	        if (level.Equals("SEVERE", StringComparison.OrdinalIgnoreCase))
	        {
		        return MamaLogLevel.MAMA_LOG_LEVEL_SEVERE;
	        }
	        if (level.Equals("ERROR", StringComparison.OrdinalIgnoreCase))
	        {
		        return MamaLogLevel.MAMA_LOG_LEVEL_ERROR;
	        }
	        if (level.Equals("WARN", StringComparison.OrdinalIgnoreCase))
	        {
		        return MamaLogLevel.MAMA_LOG_LEVEL_WARN;
	        }
	        if (level.Equals("NORMAL", StringComparison.OrdinalIgnoreCase))
	        {
		        return MamaLogLevel.MAMA_LOG_LEVEL_NORMAL;
	        }
	        if (level.Equals("FINE", StringComparison.OrdinalIgnoreCase))
	        {
		        return MamaLogLevel.MAMA_LOG_LEVEL_FINE;
	        }
	        if (level.Equals("FINER", StringComparison.OrdinalIgnoreCase))
	        {
		        return MamaLogLevel.MAMA_LOG_LEVEL_FINER;
	        }
	        if (level.Equals("FINEST", StringComparison.OrdinalIgnoreCase))
	        {
		        return MamaLogLevel.MAMA_LOG_LEVEL_FINEST;
	        }

	        return MamaLogLevel.MAMA_LOG_LEVEL_NORMAL;
        }

        /// <summary>
        /// Sets the log callback, this will be invoked every time a log is written in MAMA.
        /// </summary>
        /// <param name="callback">
        /// The callback to invoke.
        /// </param>
        public static void setLogCallback(MamaLogFileCallback2 callback)
        {
            // Dispose the old callback wrapper
            if (m_logFileCbWrapper != null)
            {
                ((IDisposable)m_logFileCbWrapper).Dispose();
                m_logFileCbWrapper = null;
            }

            // Null is allowed to clear the callback
            if (callback == null)
            {
                // Call the native function with null
                MamaWrapper.CheckResultCode(NativeMethods.mama_setLogCallback2(null));
            }

            else
            {
                // Create a new callback wrapper
                m_logFileCbWrapper = new MamaCallbackWrapper<MamaLogFileCallback2, LogFileCallbackDelegate>(
                        callback,
                        null,
                        new LogFileCallbackDelegate(onNativeLogCallback));

                // Call the native function
                MamaWrapper.CheckResultCode(NativeMethods.mama_setLogCallback2((LogFileCallbackDelegate)m_logFileCbWrapper.NativeDelegate));
            }
        }

        /// <summary>
        /// Set callback for log max size exceeded.
        /// Applies only to USER policy.
        /// </summary>
        /// <param name="callback">
        /// The callback object to invoke whenever the log size is exceeded.
        /// </param>
        public static void setLogSizeCb(MamaLogFileCallback callback)
        {
            // Dispose the old callback wrapper
            if (m_logSizeCbWrapper != null)
            {
                ((IDisposable)m_logSizeCbWrapper).Dispose();
                m_logSizeCbWrapper = null;
            }

            // Null is allowed to clear the callback
            if (callback == null)
            {
                // Call the native function with null
                MamaWrapper.CheckResultCode(NativeMethods.mama_setLogSizeCb(null));
            }

            else
            {
                // Create a new callback wrapper
                m_logSizeCbWrapper = new MamaCallbackWrapper<MamaLogFileCallback, LogSizeCallbackDelegate>(
                        callback,
                        null,
                        new LogSizeCallbackDelegate(onNativeLogSizeExceeded));

                // Call the native function
                MamaWrapper.CheckResultCode(NativeMethods.mama_setLogSizeCb((LogSizeCallbackDelegate)m_logSizeCbWrapper.NativeDelegate));
            }
        }

        /// <summary>
        /// This handler is called by the C layer whenever a log comes in, it will
        /// invoke the managed object.
        /// </summary>
        /// <param name="level">
        /// The log level.
        /// </param>
        /// <param name="message">
        /// The log message.
        /// </param>
        public static void onNativeLogCallback(int level, string message)
        {
            // Get the callback from the static member variable
            MamaLogFileCallback2 callback = (MamaLogFileCallback2)m_logFileCbWrapper.Callback;
            if (callback != null)
            {
                // Invoke the delegate
                callback.onLog((MamaLogLevel)level, message);
            }
        }

        /// <summary>
        /// This event handler is called whenever the log size is exceeded.
        /// </summary>
        private static void onNativeLogSizeExceeded()
        {
            // Get the callback from the static member variable
            MamaLogFileCallback callback = (MamaLogFileCallback)m_logSizeCbWrapper.Callback;
            if (callback != null)
            {
                // Invoke the delegate
                callback.onLogSizeExceeded();
            }
        }

        /// <summary>
        /// Name of DLL containing Native code
        /// </summary>
#if _GNU
        public const string DllName = "mama";
#elif _OSX
        public const string DllName = "mama";
#elif DEBUG
		public const string DllName = "libmamacmdd.dll";
#else
		public const string DllName = "libmamacmd.dll";
#endif

		/// <summary>
        /// Load the bridge specified by middleware string.
        /// If the bridge has already been loaded then the existing bridge instance
        /// will be returned.
        /// </summary>
        /// <param name="middleware"></param> The middleware string. Can be "wmw", "lbm" or
        /// "tibrv".
        /// <returns>mama_status Whether the call was successful or not</returns>
        public static MamaBridge loadBridge (string middleware)
		{
			return new MamaBridge (middleware);
		}


        /// <summary>
        /// Load the bridge specified by middleware string using the path
        /// specified by the user.
        /// If the bridge has already been loaded then the existing bridge instance
        /// will be returned
        /// </summary>
        /// <param name="middleware"></param> The middleware string. Can be "wmw", "lbm" or
         /// "tibrv".
        /// <param name="path"></param> The path to the bridge library
        /// <returns>mama_status Whether the call was successful or not</returns>
        public static MamaBridge loadBridge (string middleware, string path)
        {
            return new MamaBridge (middleware, path);
        }

		/// <summary>
		/// Load the payload brige specified by the payload string.
		/// </summary>
		/// <param name="payload"></param>The payload string.
		public static MamaPayloadBridge loadPayloadBridge (string payload)
		{
			return new MamaPayloadBridge (payload);
		}

		/// <summary>
        ///
        /// Initialize MAMA.
        ///
        /// MAMA employs a reference count to track multiple calls to Mama.open() and
        /// Mama.close(). The count is incremented every time Mama.open() is called
        /// and decremented when Mama.close() is called. The resources are not actually
        /// released until the count reaches zero.
        ///
        /// If entitlements are enabled for the library, the available entitlement
        /// server names are read from the entitlement.servers property in the
        /// mama.properties file located in the %WOMBAT_PATH% directory.
        ///
        /// This function is thread safe.
        ///
		/// </summary>
		public static void open ()
		{
            initGetVersionWrapper();
			MamaWrapper.CheckResultCode(NativeMethods.mama_open());
            Interlocked.Increment(ref mMamaopen);
		}

        /// <summary>
        ///
        /// Initialize MAMA.
        ///
        /// Allows users of the API to override the default behavior of mama_open()
        /// where a file mama.properties is required to be located in the directory
        /// specified by %WOMBAT_PATH%.
        ///
        /// The properties file must have the same structure as a standard
        /// mama.properties file.
        ///
        /// If null is passed as the path the API will look for the properties file on
        /// the %WOMBAT_PATH%.
        ///
        /// If null is passed as the filename the API will look for the default
        /// filename of mama.properties
        /// </summary>
        /// <param name="path">
        /// Fully qualified path to the directory containing the properties
        /// file
        /// </param>
        /// <param name="filename">
        /// The name of the file containing MAMA properties.
        /// </param>
		public static void openWithProperties (string path, string filename)
		{
            initGetVersionWrapper();
            MamaWrapper.CheckResultCode (NativeMethods.mama_openWithProperties (path, filename));
            Interlocked.Increment(ref mMamaopen);
		}

		/// <summary>
		/// Set a specific property for the API.
		/// If the property being set has already been given a value from a
		/// properties file that value will be replaced.
		/// See the example mama.properties provided with the distribution for
		/// examples of property formatting. The properties set via this function
		/// should be formatted in the same manner as those specified in
		/// mama.properties.
		/// The strings passed to the function are copied.
		/// </summary>
		public static void setProperty (string name, string value)
		{
			MamaWrapper.CheckResultCode (NativeMethods.mama_setProperty (name, value));
		}

		/// <summary>
		/// Retrieve a specific property from the API.
		/// If the property has not been set, a null value will be returned.
		/// </summary>
		public static string getProperty (string name)
		{
			return Marshal.PtrToStringAnsi(NativeMethods.mama_getProperty (name));
		}

		/// <summary>
		/// Load in default mama.properties from the default WOMBAT_PATH directory.
		/// </summary>
		public static void loadDefaultProperties ()
		{
			MamaWrapper.CheckResultCode (NativeMethods.mama_loadDefaultProperties ());
		}

		/// <summary>
		/// Retrieve a specific property from the API.
		/// If the property has not been set, the default value will be returned
		/// </summary>
		public static string getProperty (string name, string defaultValue)
		{
			string property = getProperty(name);
			if (null == property)
			{
				return defaultValue;
			}
			else
			{
				return property;
			}
		}

		/// <summary>
		/// Retrieve all configured properties as a dictionary from the current configuration.
		/// </summary>
		public static Dictionary<string, string> getProperties ()
		{
			IntPtr propertiesAsStringPtr = NativeMethods.mama_getPropertiesAsString();
			string propertiesAsString = Marshal.PtrToStringAnsi(propertiesAsStringPtr);
			// We have our C# string now - free underlying native memory
			NativeMethods.mama_freeAllocatedBuffer(propertiesAsStringPtr);

			Dictionary<string, string> result = new Dictionary<string, string>();
			if (null == propertiesAsString)
			{
				return result;
			}
			string[] propPairs = propertiesAsString.Split('\n');
			foreach (string propStringPair in propPairs) {
				String[] pair = propStringPair.Split('=');
				if (pair.Length > 1) {
					result.Add(pair[0], pair[1]);
				}
			}
			return result;
		}

		/// <summary>
		/// Return the version information for the library
		/// The version of Mama follows in parenthesis
		/// </summary>
		/// <returns>Version string</returns>
		public static string getVersion (MamaBridge bridgeImpl)
		{
			return MamaVersion.MAMADOTNET_VERSION + " (" + Marshal.PtrToStringAnsi(NativeMethods.mama_getVersion (bridgeImpl.NativeHandle)) + ")";
		}

		/// <summary>
		/// Return the version information for the library
        /// The version of Mama follows in parenthesis
		/// </summary>
		/// <returns></returns>
		private static string getVersion (IntPtr bridgeImpl)
		{
			return MamaVersion.MAMADOTNET_VERSION + " (" + Marshal.PtrToStringAnsi(NativeMethods.mama_getVersion (bridgeImpl)) + ")";
		}

        /// <summary>
        /// Start processing messages on the internal queue. This starts Mama's
        /// internal throttle, refresh logic, and other internal Mama processes as well
        /// as dispatching messages from the internal queue.
        /// <p>
        /// Mama.start( ) blocks until an invocation of Mama.stop() occurs.
        ///
        /// MAMA employs a reference count to track multiple calls to Mama.start() and
        /// Mama.stop(). The count is incremented every time Mama.start() is called and
        /// decremented when Mama.stop() is called. The first Mama.start() call does not
        /// unblock until the count reaches zero.
        ///
        /// This function is thread safe.
        /// </p>
        /// </summary>
        /// <param name="bridgeImpl">
        /// The bridge specific structure.
        /// </param>
        public static void start(MamaBridge bridgeImpl)
        {
            MamaWrapper.CheckResultCode(NativeMethods.mama_start(bridgeImpl.NativeHandle));
        }

		/// <summary>
		/// Starts and starts dispatching on all currently loaded MAMA bridges and blocks until they have been stopped.
		/// </summary>
        public static void startAll()
        {
	        MamaWrapper.CheckResultCode(NativeMethods.mama_startAll(true));
        }

        /// <summary>
		/// Starts and starts dispatching on all currently loaded MAMA bridges and optionally blocks until they have
		/// been stopped.
		/// </summary>
        public static void startAll(bool isBlocking)
        {
	        MamaWrapper.CheckResultCode(NativeMethods.mama_startAll(isBlocking));
        }

        /// <summary>
		/// Stops dispatching for all currently started MAMA bridges.
		/// </summary>
        public static void stopAll()
        {
	        MamaWrapper.CheckResultCode(NativeMethods.mama_stopAll());
        }
        
		/// <summary>
        ///
        /// Close MAMA and free all associated resource.
        ///
        /// MAMA employs a reference count to track multiple calls to Mama.open() and
        /// Mama.close(). The count is incremented every time Mama.open() is called and
        /// decremented when Mama.close() is called. The
        /// resources are not actually released until the count reaches zero.
        ///
        /// This function is thread safe.
		/// </summary>
		public static void close ()
		{
            try
            {
                MamaWrapper.CheckResultCode(NativeMethods.mama_close());
            }

            finally
            {
                /* Decrement the ref count even if something went wrong or there will be crashes
                 * when the various objects are finalized.
                 * The objects will only destroy the native objects if MAMA is still open.
                 */
                Interlocked.Decrement(ref mMamaopen);
            }
		}

        /// <summary>
        /// Start Mama in the background. This method invokes Mama.start() in a separate thread.
        /// </summary>
        /// <param name="bridgeImpl">
        /// The bridge specific structure.
        /// </param>
        /// <param name="callback">
        /// The callback for asynchronous status.
        /// </param>
        public static void startBackground(MamaBridge bridgeImpl, MamaStartBackgroundCallback callback)
        {
            // Allocate a fowarder object
            mStartBackgroundCallbackForwarder = new StartBackgroundCallbackForwarder(callback);
            mStartBackgroundShimCallback = new StartBackgroundCallbackForwarder.StartBackgroundCompleteDelegate(mStartBackgroundCallbackForwarder.onStartBackgroundCompleted);

            // Call the native function
            MamaWrapper.CheckResultCode(NativeMethods.mama_startBackground(bridgeImpl.NativeHandle, mStartBackgroundShimCallback));
        }


        public static bool opened
		{
			get
			{
				return (mMamaopen > 0);
			}
		}

        /// <summary>
        /// Stop dispatching on the default event queue for the specified bridge.
        ///
        /// MAMA employs a reference count to track multiple calls to Mama.start() and
        /// Mama.stop(). The count is incremented every time Mama.start() is called and
        /// decremented when Mama.stop() is called. The first Mama.start() call does not
        /// unblock until the count reaches zero.
        ///
        /// This function is thread safe.
        /// </summary>
        /// <param name="bridgeImpl">
        /// The bridge specific structure.
        /// </param>
        public static void stop (MamaBridge bridgeImpl)
		{
			MamaWrapper.CheckResultCode(NativeMethods.mama_stop(bridgeImpl.NativeHandle));
		}

		/// <summary>
		/// Enable logging.
		/// </summary>
        /// <param name="level">
        /// The logging level allowed.
        /// </param>
        /// <exception cref="ArgumentOutOfRangeException">
        /// The file name contains invalid characters.
        /// </exception>
		public static void enableLogging (MamaLogLevel level)
		{
            // Check for a valid enumeration value
            if (!Enum.IsDefined(typeof(MamaLogLevel), (int)level))
            {
                throw new ArgumentOutOfRangeException("level");
            }
			MamaWrapper.CheckResultCode(NativeMethods.mama_enableLogging(IntPtr.Zero, (int)level));
		}

		/// <summary>
		/// Enable logging, accepts a string representing the file location.
		/// </summary>
		/// <param name="fileName">
        /// The full path to the log file.
        /// </param>
		/// <param name="level">
        /// The logging level allowed.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// The file name is null or blank.
        /// </exception>
        /// <exception cref="ArgumentOutOfRangeException">
        /// The file name contains invalid characters.
        /// </exception>
		public static void logToFile(string fileName, MamaLogLevel level)
		{
            // Check the arguments
            if ((fileName == null) || (fileName == string.Empty))
            {
                throw new ArgumentNullException("fileName");
            }

			// Check for invalid characters in the file name
			if (fileName.IndexOfAny(Path.GetInvalidPathChars()) != -1)
			{
				throw new ArgumentOutOfRangeException(fileName, "The file or path contains invalid characters");
			}

            // Check for a valid enumeration value
            if (!Enum.IsDefined(typeof(MamaLogLevel), (int)level))
            {
                throw new ArgumentOutOfRangeException("level");
            }

            // Call the native function
			MamaWrapper.CheckResultCode(NativeMethods.mama_logToFile(fileName, (int)level));
		}

        /// <summary>
        /// Disable logging.
        /// </summary>
        public static void disableLogging()
        {
            MamaWrapper.CheckResultCode(NativeMethods.mama_disableLogging());
        }

        /// <summary>
        /// Close any underlying log resources.
        /// </summary>
        public static void logDestroy()
        {
            NativeMethods.mama_logDestroy();
        }

        /// <summary>
        /// Set the log level.
        /// </summary>
        /// <param name="level">
        /// The logging level allowed.
        /// </param>
        /// <exception cref="ArgumentOutOfRangeException">
        /// The file name contains invalid characters.
        /// </exception>
		public static void setLogLevel(MamaLogLevel level)
		{
            // Check for a valid enumeration value
            if (!Enum.IsDefined(typeof(MamaLogLevel), (int)level))
            {
                throw new ArgumentOutOfRangeException("level");
            }

			MamaWrapper.CheckResultCode(NativeMethods.mama_setLogLevel((int)level));
		}

        /// <summary>
        /// Get the log level.
        /// </summary>
        /// <returns>Log level MamaLogLevel</returns>
		public static MamaLogLevel getLogLevel()
		{
			return (MamaLogLevel)NativeMethods.mama_getLogLevel();
		}

        /// <summary>
        /// Set the log file max size.
        /// </summary>
        /// <param name="size"></param>
        /// <exception cref="ArgumentOutOfRangeException">
        /// Thrown if the size is 0.
        /// </exception>
		public static void setLogSize(ulong size)
		{
            // Check the argument
            if (size == 0)
            {
                throw new ArgumentOutOfRangeException("size");
            }

			MamaWrapper.CheckResultCode(NativeMethods.mama_setLogSize(size));
		}

        /// <summary>
        /// Set the max number of log files.
        /// </summary>
        /// <param name="numFiles"></param>
        /// <exception cref="ArgumentOutOfRangeException">
        /// Thrown if the number of files is 0 or less.
        /// </exception>
		public static void setNumLogFiles(int numFiles)
		{
            // Check the argument
            if (numFiles < 1)
            {
                throw new ArgumentOutOfRangeException("numFiles");
            }

			MamaWrapper.CheckResultCode(NativeMethods.mama_setNumLogFiles(numFiles));
		}

        /// <summary>
        /// Set logging policy.
        /// </summary>
        /// <param name="policy"></param>
		public static void setLogFilePolicy(MamaLogFilePolicy policy)
		{
			MamaWrapper.CheckResultCode(NativeMethods.mama_setLogFilePolicy((int)policy));
		}

        /// <summary>
        /// Set append to prevent overwriting existing logfiles.
        /// </summary>
        /// <param name="append"></param>
		public static void setAppendToLogFile(bool append)
		{
			int appendC = 0;
			if(append)
				appendC = 1;

			MamaWrapper.CheckResultCode(NativeMethods.mama_setAppendToLogFile(appendC));
		}

        /// <summary>
        /// Get logging to file status
        /// </summary>
        /// <returns>Boolean true if logging to file</returns>
		public static bool loggingToFile()
		{
			int loggingToFile = NativeMethods.mama_loggingToFile();
			if(0 == loggingToFile)
				return false;
			else
				return true;
		}

        /// <summary>
		/// Add string to mama log at specified mama level.
		/// </summary>
        /// <param name="level">
        /// The level to log at.
        /// </param>
        /// <param name="text">
        /// The log message.
        /// </param>
        /// <exception cref="ArgumentOutOfRangeException">
        /// The logging level is invalid.
        /// </exception>
        public static void log(MamaLogLevel level, string text)
		{
            // Check for a valid enumeration value
            if (!Enum.IsDefined(typeof(MamaLogLevel), (int)level))
            {
                throw new ArgumentOutOfRangeException("level");
            }

            // If null has been passed for the text then use a blank string
            if (text == null)
            {
                NativeMethods.mama_log2((int)level, string.Empty);
            }

            else
            {
                NativeMethods.mama_log2((int)level, text);
            }
		}

        /// <summary>
        /// This function will invoke the default log function.
        /// </summary>
        /// <param name="level">
        /// The level to log at.
        /// </param>
        /// <param name="text">
        /// The log message.
        /// </param>
        /// <exception cref="ArgumentNullException">
        /// The logging text has not been supplied.
        /// </exception>
        /// <exception cref="ArgumentOutOfRangeException">
        /// The logging level is invalid.
        /// </exception>
        public static void defaultLogFunction(MamaLogLevel level, string text)
        {
            // Check the arguments
            if ((text == null) || (text == string.Empty))
            {
                throw new ArgumentNullException("text");
            }

            // Check for a valid enumeration value
            if (!Enum.IsDefined(typeof(MamaLogLevel), (int)level))
            {
                throw new ArgumentOutOfRangeException("level");
            }

            // Call the native method
            NativeMethods.mama_logDefault2((int)level, text);
        }



		/// <summary>
		/// Set the high watermark for the internal default MAMA queue.
		/// See <code>mamaQueue_setHighWatermark()</code> for details.
		///
		/// Although the monitoring callbacks on the default queue cannot be
		/// specified for RV, setting of the high water mark is still supported.
		/// </summary>
		/// <param name="highWatermark"></param>
		public static void setDefaultQueueHighWatermark (int highWatermark)
		{

		}

		/// <summary>
		/// Set the low watermark for the internal default MAMA queue.
		/// See <code>mamaQueue_setLowWatermark()</code> for details.
		///
		/// Currently supported only on Wombat TCP middleware.
		/// </summary>
		/// <param name="lowWatermark"></param>
		public static void setDefaultQueueLowWatermark (int lowWatermark)
		{

  		}

		/// <summary>
		/// </summary>
		public static MamaQueue getDefaultEventQueue (MamaBridge bridgeImpl)
		{
			IntPtr queuePtr = IntPtr.Zero;
			int code = NativeMethods.mama_getDefaultEventQueue (bridgeImpl.NativeHandle, ref queuePtr);
			MamaWrapper.CheckResultCode(code);
			return new MamaQueue (queuePtr);
		}

		#region Implementation details


        // For WAM, allows the C-layer to call up to the wrapper so we can get
        // the wrapper version in addition to the C version
        private delegate string wrapperGetVersion(IntPtr bridgeImpl);
        private static wrapperGetVersion wrapperVersionGetter = new wrapperGetVersion(getVersion);
        private static void initGetVersionWrapper()
        {
            NativeMethods.mama_setWrapperGetVersion(wrapperVersionGetter);
        }

        /// <summary>
        /// This class is used for forward callback events from the native mama_startBackground
        /// function to the client's of this class.
        /// </summary>
        private class StartBackgroundCallbackForwarder
        {
            /// <summary>
            /// The callback function.
            /// </summary>
            private MamaStartBackgroundCallback mCallback;

            /// <summary>
            /// Delegate is use
            /// </summary>
            /// <param name="status">
            /// The status returned from mama_start.
            /// </param>
            public delegate void StartBackgroundCompleteDelegate(MamaStatus.mamaStatus status);

            public StartBackgroundCallbackForwarder(MamaStartBackgroundCallback callback)
            {
                // Save arguments in member variables
                mCallback = callback;
            }

            /// <summary>
            /// Handler is called whenever mama_start completes,
            /// </summary>
            /// <param name="status">
            /// The status returned from mama_start.
            /// </param>
            internal void onStartBackgroundCompleted(MamaStatus.mamaStatus status)
            {
                // Invoke the callback function
                if (mCallback != null)
                {
                    // Create a new mama status using the data passed in
                    mCallback.onStartComplete(status);
                }
            }

        }

		// Interop API
		private struct NativeMethods
		{
			// export definitions
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mama_open();
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mama_openWithProperties(string path, string filename);
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mama_setProperty (string name, string value);
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern IntPtr mama_getProperty (string name);
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern void mama_loadDefaultProperties ();
			[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern IntPtr mama_getPropertiesAsString ();
			[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern void mama_freeAllocatedBuffer (IntPtr buffer);
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern IntPtr mama_getVersion (IntPtr bridgeImpl);
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mama_close();
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mama_start (IntPtr bridgeImpl);
			[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mama_startAll(bool isBlocking);
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mama_startBackground(IntPtr bridgeImpl, StartBackgroundCallbackForwarder.StartBackgroundCompleteDelegate callback);
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mama_stop (IntPtr bridgeImpl);
			[DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mama_stopAll();
			[DllImport(DllName, CallingConvention=CallingConvention.Cdecl)]
			public static extern int mama_enableLogging (IntPtr f, int level);
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mama_logToFile(string fileName, int level);
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mama_disableLogging();
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern void mama_logDestroy();
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mama_setLogLevel(int level);
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mama_getLogLevel();
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mama_setLogSize(ulong size);
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mama_setNumLogFiles(int numFiles);
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mama_setLogFilePolicy(int policy);
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mama_setAppendToLogFile(int append);
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mama_loggingToFile();
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern int mama_setLogSizeCb(LogSizeCallbackDelegate callback);
            [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
			public static extern void mama_log2(int level, string txt);
            [DllImport(DllName, CallingConvention = CallingConvention.StdCall)]
            public static extern void mama_logDefault2(int level, string txt);
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
            public static extern void mama_setWrapperGetVersion(wrapperGetVersion getVer);
			[DllImport(DllName, CallingConvention=CallingConvention.Cdecl)]
			public static extern int mama_getDefaultEventQueue (IntPtr bridgeImpl, ref IntPtr queuePtr);
            [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
			public static extern int mama_setLogCallback2(LogFileCallbackDelegate callback);
        }

		// Static variables used to dispatch callback events from mama_startbackground.
        private static StartBackgroundCallbackForwarder mStartBackgroundCallbackForwarder;
        private static StartBackgroundCallbackForwarder.StartBackgroundCompleteDelegate mStartBackgroundShimCallback;

        private static int mMamaopen = 0;

#endregion // Implementation details
	}
}
