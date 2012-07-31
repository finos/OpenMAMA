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

#ifndef MAMA_CPP_H__
#define MAMA_CPP_H__

#include <stdio.h>
#include <cstring>

#include <mama/mama.h>
#include <mama/MamaBridgeCallback.h>
#include <mama/MamaMsg.h>
#include <mama/MamaQueueGroup.h>
#include <mama/MamaBasicSubscription.h>
#include <mama/MamaBasicSubscriptionCallback.h>
#include <mama/MamaBasicWildCardSubscription.h>
#include <mama/MamaBasicWildCardSubscriptionCallback.h>
#include <mama/MamaSubscription.h>
#include <mama/MamaSubscriptionCallback.h>
#include <mama/MamaTransport.h>
#include <mama/MamaPublisher.h>
#include <mama/MamaInboxCallback.h>
#include <mama/MamaInbox.h>
#include <mama/MamaQueue.h>
#include <mama/MamaQueueEnqueueCallback.h>
#include <mama/MamaQueueMonitorCallback.h>
#include <mama/MamaQueueEventCallback.h>
#include <mama/MamaDispatcher.h>
#include <mama/MamaTimerCallback.h>
#include <mama/MamaTimer.h>
#include <mama/MamaIoCallback.h>
#include <mama/MamaIo.h>
#include <mama/MamaDictionaryCallback.h>
#include <mama/MamaDictionary.h>
#include <mama/MamaFieldDescriptor.h>
#include <mama/MamaDateTime.h>
#include <mama/MamaPrice.h>
#include <mama/MamaMsgFieldIterator.h>
#include <mama/MamaMsgField.h>
#include <mama/MamaStatus.h>
#include <mama/MamaSymbolMap.h>
#include <mama/MamaSymbolMapFile.h>
#include <mama/MamaLogFile.h>
#include <mama/MamaSymbolSourceCallback.h>
#include <mama/MamaSymbolSource.h>
#include <mama/MamaMsgQual.h>
#include <mama/msgstatus.h>
#include <mama/types.h>
#include <mama/MamaSendCompleteCallback.h>
#include <mama/MamaSource.h>
#include <mama/MamaSourceManager.h>
#include <mama/MamaSourceGroup.h>
#include <mama/MamaSourceGroupManager.h>


namespace Wombat
{
/**
 * \mainpage Middleware Agnostic Messaging API (MAMA) C++ API
 *
 * Middleware Agnostic Messaging API.
 * The Middleware Agnostic Messaging (MAMA) API provides an
 * abstraction layer over various messaging middleware platforms.  In
 * particular, MAMA adds market data semantics to messaging platforms
 * that would otherwise be too limited for use as a market data
 * distribution middleware.  Features that MAMA adds to any messaging
 * middleware are:
 * - Subscription management (initial values, throttling).
 * - Entitlements/permissioning.
 * - Data quality.
 *
 * The goal of MAMA is to maximize application portability.  Once an
 * application has been ported to MAMA, it should never have to be
 * ported to another market data messaging API again.  Many firms have
 * invested time in building and maintaining their own abstraction
 * APIs - and they should be commended for that.  Wombat hopes that
 * even those firms will see the benefit in migrating to MAMA and
 * thereby reducing costs further and, as more third party firms
 * migrate applications to MAMA (and MAMDA, see below), benefit even
 * more from this compatibility.
 *
 *
 * A higher level market data API is also available: the Middleware
 * Agnostic Market Data API (MAMDA).  While MAMA provides a
 * field-based abstraction to market data, MAMDA provides smart,
 * specialized data types to deal with specific market data events,
 * such as trades, quotes, order books, etc.  MAMDA is particularly
 * suitable for applications such as program trading and tick capture
 * systems, where context is important.  MAMA is more suited to
 * applications that don't care about the meaning of fields, such as a
 * simple, field-based market data display application.
 *
 */

/**
 * Subclass this to receive log notifications.
 */
class MAMACPPExpDLL MamaLogFileCallback
{
public:
    virtual ~MamaLogFileCallback () {}
    virtual void onLogSizeExceeded () = 0;
};

/**
* Callback object passed to Mama::startBackground(). The onStartComplete()
* method will be invoked if an error occurs calling Mama::startBackground()
* or when Mama::startBackground() exits normally in which case the status
* returned will be MAMA_STATUS_OK
*/
class MAMACPPExpDLL MamaStartCallback
{
public:
    virtual ~MamaStartCallback () {};
    virtual void onStartComplete (MamaStatus status) = 0;
};

#ifdef WITH_ENTITLEMENTS
/**
 * Callback object passed to Mama::registerEntitlmentCallbacks().  The 
 * onSessionDisconnect() method will be invoked when a disconnect request is 
 * received from the entitlements server.
 */
class MAMACPPExpDLL MamaEntitlementCallback
{
public:
    virtual ~MamaEntitlementCallback () {};
    /**
     * Function invoked when diconnect request is received from the 
     * entitlements server 
     * @param reason  Reason disconnect request was sent
     * @param userId  ID of user who initiated the disconnection, either by 
     *                attempting to initiate a new connection or an admin user
     *                explicitly issuing a disconnect
     * @param host    Hostname/IP address of the site server or where userId 
     *                is attempting to connect from
     * @param appName "site server" or application that userId is using to 
     *                disconnect (possibly NULL) 
     **/
    virtual void 
    onSessionDisconnect (const sessionDisconnectReason reason,
                         const char* userId,
                         const char* host,
                         const char* appName) = 0;

    /**
     * Function invoked after dynamic entitlements update has occurred
     */
    virtual void 
    onEntitlementUpdate () = 0;

    /**
     * Function invoked after entitlements checking has been switched,
     * for example, from enabled to disabled.
     **/
    virtual void 
    onEntitlementCheckingSwitch (const int isEntitlementsCheckingDisabled) {};
};
#endif /*WITH_ENTITLEMENTS*/

/**
 * The <code>Mama</code> class provides methods global initialization
 * and manipulating global options.
 */
class MAMACPPExpDLL Mama
{
public:

    /** Load the bridge specified by middleware string.  
     * If the bridge has already been loaded then the existing bridge instance
     * will be returned. 
     * @param impl The bridge object
     * @param middleware  The middleware string. Can be "wmw", "lbm" or 
     * "tibrv".      
     * @return mama_status Whether the call was successful or not. 
     */

    static mamaBridge loadBridge (const char* middleware);


   /** Load the bridge specified by middleware string using the path specified by the user.
     * If the bridge has already been loaded then the existing bridge instance
     * will be returned. 
     * @param impl The bridge object
     * @param middleware  The middleware string. Can be "wmw", "lbm" or 
     * "tibrv".      
     * @param path The path to the bridge library
     * @return mama_status Whether the call was successful or not. 
     */

    static mamaBridge loadBridge (const char* middleware, const char* path);
    /**
     * Returns the version of the mama binary. The version of the underlying 
     * transport is also returned in parens after the mama version.
     */
    static const char* getVersion (mamaBridge bridgeImpl);

    /**
     *
     * Initialize MAMA. 
     *
     * MAMA employs a reference count to track multiple
     * calls to Mama::open() and Mama::close(). The count is incremented every time
     * Mama::open() is called and decremented when Mama::close() is called. The
     * resources are not actually released until the count reaches zero. 
     *
     * If entitlements are enabled for the library, the available entitlement
     * server names are read from the entitlement.servers property in the
     * mama.properties file located in the \$WOMBAT_PATH directory.
     *
     * This function is thread safe.
     */
    static void open ();

    /**
     * Initialize MAMA.
     * Allows users of the API to override the default behaviour of Mama.open()
     * where a file mama.properties is required to be located in the directory
     * specified by \$WOMBAT_PATH.
     *
     * The properties file must have the same structure as a standard
     * mama.properties file.
     *
     * If null is passed as the path the API will look for the properties file
     * on
     * the \$WOMBAT_PATH.
     *
     * If null is passed as the filename the API will look for the default
     * filename of mama.properties.
     *
     * @param[in] path Fully qualified path to the directory containing the
     * properties
     * file
     *
     * @param[in] filename The name of the file containing MAMA properties.
     */
    static void open (const char*   path,
                      const char*   filename);

#ifdef WITH_ENTITLEMENTS
    /**
     * Registers the callback methods to be called whenever the user receives a
     * disconnect request or after a dynamic entitlements update occurrs
     * @param callback Data structure containing the function to be invoked
     *                 when disconnect request is received or after a dynamic
     *                 entitlement update occurrs
     */
    static void registerEntitlementCallbacks (MamaEntitlementCallback* callback);
#endif /*WITH_ENTITLEMENTS*/

    /**
     * Set a specific property for the API.
     *
     * If the property being set has already been given a value from a
     * properties file that value will be replaced.
     *
     * See the example mama.properties provided with the distribution for
     * examples of property formatting. The properties set via this function
     * should be formatted in the same manner as those specified in
     * mama.properties.
     *
     * The strings passed to the function are copied.
     *
     * @param name The name of the property
     * @param value The property value
     */
    static void setProperty (const char* name, const char* value);

    /**
     * Retrieve a specific property from the API.
     *
     * If the property has not been set, a NULL value will be returned.
     *
     * @param name The name of the property to retrieve.
     *
     * @return the value of the property or NULL if unset.
     */
    static const char * getProperty (const char* name);

    /**
     * Close MAMA and free all associated resource.
     *
     * MAMA employs a reference count to track multiple
     * calls to Mama::open() and Mama::close(). The count is incremented every time
     * Mama::open() is called and decremented when Mama::close() is called. The
     * resources are not actually released until the count reaches zero. 
     *
     * This function is thread safe.
     */
    static void close ();

    /**
     * Start processing messages on the internal queue. This starts Mama's
     * internal throttle, refresh logic, and other internal Mama processes as well
     * as dispatching messages from the internal queue. 
     * <p>
     * Mama::start( ) blocks until an invocation of Mama::stop() occurs.
     *
     * MAMA employs a reference count to track multiple calls to Mama::start() and 
     * Mama::stop(). The count is incremented every time Mama::start() is called and 
     * decremented when Mama::stop() is called. The first Mama::start() call does not 
     * unblock until the count reaches zero. 
     *
     * This function is thread safe.
     *
     * @param[in] bridgeImpl The bridge specific structure.
     */
    static void start (mamaBridge bridgeImpl);

    /**
     * Start processing MAMA internal events in the background. This
     * method invokes Mama::start () in a separate thread.
     *
     * @param[in] bridgeImpl The middleware-specific bridge structure
     * @param[in] callback The callback for asynchronous status.
     */
    static void startBackground (mamaBridge bridgeImpl,
                                 MamaStartCallback* callback);

    /**
     * Stop dispatching on the default event queue for the specified bridge.
     *
     * MAMA employs a reference count to track multiple calls to Mama::start() and 
     * Mama::stop(). The count is incremented every time Mama::start() is called and 
     * decremented when Mama::stop() is called. The first Mama::start() call does not 
     * unblock until the count reaches zero. 
     *
     * This function is thread safe.
     * 
     * @param[in] bridgeImpl The bridge specific structure.
     */
    static void stop (mamaBridge bridgeImpl);


     /**
     * Stop dispatching on the default event queue for all bridges.
     */
    static void stopAll (void);

    /**
     * Enable logging and direct the output to the specified stream.
     *
     * @param level The level
     * @param logFile the log file.
     */
    static void enableLogging (
        MamaLogLevel    level,
        FILE*           logFile);

    /**
     * Enable logging to the specified file.
     *
     * @param file the log filename
     * @param level The level
     *
     */
    static void logToFile (
        const char*     file,
        MamaLogLevel    level);

    /**
     * Disable logging.
     */
    static void disableLogging (void);

    /**
     * Set the logging level
     * @param level The level
     */
    static void setLogLevel (MamaLogLevel level);

    /**
     * Get the logging level
     * @return the logging level
     */
    static MamaLogLevel getLogLevel (void);

    /**
     * Set the maxmum size of the log file (bytes)
     * Default max size is 500 Mb
     * @param size the max size of file (bytes)
     */
    static void setLogSize (unsigned long size);

    /**
     * Set the number of rolled logfiles to keep before overwriting.
     * Default is 10
     * @param numFiles the max number of logfiles
     */
    static void setNumLogFiles(int numFiles);

    /**
     * Set the policy regarding how to handle files when Max file size is reached.
     * Default is LOGFILE_UNBOUNDED - uses a single logfile unlimited in size.
     * Other policies are:
     * LOGFILE_ROLL - keeps N logfiles specified with setNumLogFiles(N).
     * LOGFILE_OVERWRITE - uses a single logfile limited in size.
     * LOGFILE_USER - if user has registered a callback it will be calledt.
     * Otherwise the file will roll or get overwritten depending onthe value
     * specified with setNumLogFiles(N).
     * @param policy the policy to use when max size is reached
     */
    static void setLogFilePolicy(mamaLogFilePolicy policy);

    /**
     * Set the mode when opening an existing log file.
     * setAppendToLogFile(true) will add data to the end of an existing file.
     * Default is false which will overwrite any existing data in the file.
     * @param append boolean flag to set append mode on or off
     */
    static void setAppendToLogFile(bool append);

    /**
     * Get the status of loggingToFile
     * Returns true if logging to a file, false if not
     * @return the status of loggingToFile
     */
    static bool loggingToFile(void);

    /**
     * Set a callback for when the max log size is reached.  This will only be
     * called if the policy has been set to LOGFILE_USER.
     * @param LogSizeCallback function pointer for the callback
     */
    static void setLogSizeCb(MamaLogFileCallback* callback);

    /**
     * Set the mama application name
     * This should be called before Mama.open()
     *
     * @param applicationName
     */
    static void setApplicationName (const char* applicationName);

    /**
     * Set the mama application class
     * This should be called before Mama.open()
     *
     * @param className
     */
    static void setApplicationClassName (const char* className);

    /**
     * Get a pointer to the internal default MAMA event queue.
     *
     * @param bridgeImpl The middleware specific bridge structure.
     * @return A pointer to the internal MAMA default event queue.
     */
    static MamaQueue* getDefaultEventQueue (mamaBridge bridgeImpl);

    /**
     * Allow the MAMA API free memory for any objects which have been
     * allocated by the API but responsibility for deleting has been handed to
     * the application code.
     *
     * This enables users of the API to provide alternate memory management
     * implementations which may result in difficulties when deleting objects
     * allocated internally by the MAMA API.
     *
     * E.g. Detaching the MamaMsg in a subscription callback.
     *
     * Currently supported types:
     *
     * MamaMsg
     */
    template <typename T>
    static void deleteObject (T* object);


    /**
    * Set a MamaBridgeMessageCallback to be invoked whenever information messages
    * are logged at the bridge level.  Information messages vary depenging on the
    * underlying middleware.  Currently only supported for LBM.
    */
    static void setBridgeCallback (mamaBridge bridge, MamaBridgeCallback* callback);

private: 
    /**
     * Utility class. No instances.
     */
    Mama (void) {}
};

} /* namespace Wombat */
#endif // MAMA_CPP_H__
