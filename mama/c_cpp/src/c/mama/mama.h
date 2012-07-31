/* $Id: mama.h,v 1.74.4.4.8.10 2011/10/02 19:02:18 ianbell Exp $
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

#ifndef MamaH__
#define MamaH__

#include "mama/config.h"
#include <mama/log.h>
#include <mama/error.h>
#include <mama/types.h>
#include <mama/datetime.h>
#include <mama/servicelevel.h>
#include <mama/status.h>
#include <mama/msgstatus.h>
#include <mama/msgtype.h>
#include <mama/msgfield.h>
#include <mama/dictionary.h>
#include <mama/transport.h>
#include <mama/msg.h>
#include <mama/middleware.h>
#include <mama/price.h>
#include <mama/publisher.h>
#include <mama/reservedfields.h>
#include <mama/inbox.h>
#include <mama/timer.h>
#include <mama/queue.h>
#include <mama/senderId.h>
#include <mama/symbolmap.h>
#include <mama/symbolmapfile.h>
#include <mama/timezone.h>
#include <mama/source.h>
#include <mama/subscriptiontype.h>
#include <mama/quality.h>
#include <mama/ft.h>

#if defined(__cplusplus)
extern "C"
{
#endif

#define MAMA_OPEN_MD      ((uint32_t)(0x00000001))
#define MAMA_OPEN_PUB_SUB ((uint32_t)(0x00000002))

#define MAMA_DEFAULT_THROTTLE_RATE 500
#define MAMA_DEFAULT_RECAP_THROTTLE_RATE 250

#define MAMA_DEFAULT_RETRIES       2
#define MAMA_DEFAULT_TIMEOUT       30.0f
#define MAMA_MAX_SYMBOL_LEN        128
#define MAMA_MAX_SOURCE_LEN        64
#define MAMA_MAX_TRANSPORT_LEN     64

    /**
     * \mainpage Middleware Agnostic Messaging API (MAMA) C API
     *
     * Middleware Agnostic Messaging API.
     * The Middleware Agnostic Messaging (MAMA) API provides an
     * abstraction layer over various messaging middleware platforms.  In
     * particular, MAMA adds market data semantics to messaging platforms
     * that would otherwise be too limited for use as a market data
     * distribution middleware.  Features that MAMA adds to any messaging
     * middleware are:
     * - Subscription management (initial values, throttling).
     * - Entitlements/permissioning
     * - Data quality.
     *
     * The goal of MAMA is to maximize application portability.  Once an
     * application has been ported to MAMA, it should never have to be
     * ported to another market data messaging API again.  Many firms have
     * invested time in building and maintaining their own abstraction
     * APIs - and they should be commended for that.  We hope that
     * even those firms will see the benefit in migrating to MAMA and
     * thereby reducing costs further and, as more third party firms
     * migrate applications to MAMA (and MAMDA, see below), benefit even
     * more from this compatibility.
     *
     * MAMA currently supports the following middleware platforms:
     * - Wombat TCP Middleware (see http://wombatfs.com)
     * - 29West Latency Busters Messaging (see http://29west.com)
     * - Tibco TIB/RV versions 6 and 7 (see http://tibco.com)
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
     *
    */

    /**
     *
     * The <code>Mama</code> data type provides methods global initialization and
     * manipulating global options.
     * <p>
     * Related <code>Mama</code> functions also provide means for creating
     * transports, dictionaries, and subscriptions.
     *
     */

	mama_status
	mama_setDefaultPayload (char id);

   /** Load the bridge specified by middleware string.
     * If the bridge has already been loaded then the existing bridge instance
     * will be returned.
     * @param impl The bridge object
     * @param middleware  The middleware string. Can be "wmw", "lbm" or
     * "tibrv".
     * @return mama_status Whether the call was successful or not.
     */
    MAMAExpDLL
    extern mama_status
    mama_loadBridge (mamaBridge* impl, const char*  middleware);

    MAMAExpDLL
    extern mama_status
    mama_loadPayloadBridge (mamaPayloadBridge*  bridge,  const char* payloadName);

    /** Load the bridge specified by middleware string using the path specified by the user.
     * If the bridge has already been loaded then the existing bridge instance
     * will be returned.
     * @param impl The bridge object
     * @param middleware  The middleware string. Can be "wmw", "lbm" or
     * "tibrv".
     * @param path The path to the bridge library
     * @return mama_status Whether the call was successful or not.
     */
    MAMAExpDLL
    extern mama_status
    mama_loadBridgeWithPath (mamaBridge* impl, const char*  middleware, const char* path);

    #define MAMA_LINK_BRIDGE(implIdentifier, impl)                  \
    do                                                              \
    {                                                               \
        implIdentifier ## Bridge_createImpl (impl);                 \
    }                                                               \
    while(0)                                                        \

    #define MAMA_CREATE_BRIDGE(implIdentifier, impl)                \
    do                                                              \
    {                                                               \
        if (MAMA_LINK_##implIdentifier)                             \
        {                                                           \
            implIdentifier##Bridge_createImpl (impl);               \
        }                                                           \
        else                                                        \
        {                                                           \
            mama_loadBridge (impl, "##implIdentifier##");           \
        }                                                           \
    }                                                               \
    while(0)                                                        \

    /******** End of multi-impl section   ********/

    /******************************************************************************
     * Global initialization and configuration.
     */

    /**
     * mama_status mama_open (void)
     *
     * Initialize MAMA.
     *
     * MAMA employs a reference count to track multiple
     * calls to mama_open() and mama_close(). The count is incremented every time
     * mama_open() is called and decremented when mama_close() is called. The
     * resources are not actually released until the count reaches zero.
     *
     */
    MAMAExpDLL
    extern mama_status
    mama_open (void);

    /**
     * Initialize MAMA.
     *
     * Allows users of the API to override the default behavior of mama_open()
     * where a file mama.properties is required to be located in the directory
     * specified by \$WOMBAT_PATH.
     *
     * The properties file must have the same structure as a standard
     * mama.properties file.
     *
     * If null is passed as the path the API will look for the properties file on
     * the \$WOMBAT_PATH.
     *
     * If null is passed as the filename the API will look for the default
     * filename of mama.properties.
     *
     * @param path Fully qualified path to the directory containing the properties
     * file
     * @param filename The name of the file containing MAMA properties.
     *
     * @return mama_status Whether the call was successful or not.
     */
    MAMAExpDLL
    extern mama_status
    mama_openWithProperties (const char*    path,
                             const char*    filename);

    /**
     * Set a specific property for the API.
     *
     * If the property being set has already been given a value from a properties
     * file that value will be replaced.
     *
     * See the example mama.properties provided with the distribution for examples
     * of property formatting. The properties set via this function should be
     * formatted in the same manner as those specified in mama.properties.
     *
     * The strings passed to the function are copied.
     *
     * @param name The name of the property being set.
     * @param value The value of the property being set.
     *
     * @return MAMA_STATUS_NULL_ARG is either name or value is null.
     *         MAMA_STATUS_OK if the function completed successfully.
     */
    MAMAExpDLL
    extern mama_status
    mama_setProperty (const char* name,
                      const char* value);

    /**
         * Load a set of properties through the API.
         *
         * If the property being set has already been given a value from a properties
         * file that value will be replaced.
         *
         * The properties file must have the same structure as a standard
         * mama.properties file.
         *
         * If null is passed as the path the API will look for the properties file on
         * the \$WOMBAT_PATH.
         *
         *
         * @param path Fully qualified path to the directory containing the properties
         * file
         * @param filename The name of the file containing properties.
         *
         *
         * @return MAMA_STATUS_NULL_ARG is either name or value is null.
         *         MAMA_STATUS_OK if the function completed successfully.
         */

    MAMAExpDLL
    extern mama_status
    mama_setPropertiesFromFile (const char *path,
                                const char *filename);

    /**
     * Retrieve a specific property from the API.
     *
     * If the property has not been set, a NULL value will be returned.
     *
     * @param name The name of the property to retrieve.
     *
     * @return the value of the property or NULL if unset.
     */
    MAMAExpDLL
    extern const char *
    mama_getProperty (const char* name);

    /**
     * Close MAMA and free all associated resource.
     *
     */
    MAMAExpDLL
    extern mama_status
    mama_close (void);

    /**
    * Return the version information for the library.
    * The version of the underlying transport follows in parens.
    *
    * @param bridgeImpl The bridge specific structure.
    */
    MAMAExpDLL
    extern const char*
    mama_getVersion (mamaBridge bridgeImpl);


    /**
     * Start processing messages on the internal queue. This starts Mama's
     * internal throttle, refresh logic, and other internal Mama processes as well
     * as dispatching messages from the internal queue.
     * <p>
     * mama_start( ) blocks until an invocation of mama_stop() occurs.
     *
     * @param bridgeImpl The bridge specific structure.
     */
    MAMAExpDLL
    extern mama_status
    mama_start (mamaBridge bridgeImpl);

    /**
     * The callback invoked if an error occurs calling mama_startBackground() or
     * when mama_startBackground() exits normally in which case status will be
     * MAMA_STATUS_OK.
     */
    typedef void (MAMACALLTYPE *mamaStartCB) (mama_status status);

    /**
     * Start Mama in the background. This method invokes mama_start() in a
     * separate thread.
     *
     * @param bridgeImpl The bridge specific structure.
     * @param callback The callback for asynchronous status.
     * @return MAMA_STATUS_OK if successful.
     */
    MAMAExpDLL
    extern mama_status
    mama_startBackground (mamaBridge    bridgeImpl,
                          mamaStartCB   callback);

    /**
     * Stop dispatching on the default event queue for the specified bridge.
     *
     * @param bridgeImpl The bridge specific structure.
     */
    MAMAExpDLL
    extern mama_status
    mama_stop (mamaBridge bridgeImpl);

    /**
     * Stop dispatching on the default event queue for all bridges.
     */
    MAMAExpDLL
    extern mama_status
    mama_stopAll (void);

    /**
     mama_setApplicationName - sets the mama application name
     This should be called before mama_open
     *@param applicationName
     */
    MAMAExpDLL
    mama_status
    mama_setApplicationName (const char* applicationName);

    /**
     mama_setApplicationClass - sets the mama class name
     This should be called before mama_open
     *@param className
     */
    MAMAExpDLL
    mama_status
    mama_setApplicationClassName (const char* className);

#ifdef WITH_ENTITLEMENTS
    /**
     Entitlement disconnect status codes
     */
    typedef enum
    {
        /*Disconnection request sent by administrator via site server GUI*/
        SESSION_DISCONNECT_REASON_ADMIN               = 0,
        /*Disconnection request sent due to user reaching concurrent access limit*/
        SESSION_DISCONNECT_REASON_NEW_CONNECTION      = 1,
        /*Disconnection request sent due to an entitlements modification resulting in user now exceeding concurrent access limit*/
        SESSION_DISCONNECT_REASON_ENTITLEMENTS_UPDATE = 2,

        SESSION_DISCONNECT_REASON_INVALID             = 101
    } sessionDisconnectReason;

    /**
     Callback passed to registerEntitlementCallbacks().  Will be invoked when a disconnect
     request is recevied from the entitlements server
     @param   reason   Reason disconnect request was sent
     @param userId   ID of user who initiated the disconnection, either by attempting to initiate
                     a new connection or an admin user explicitly issuing a disconnect
     @param host     Hostname/IP address of the site server or where userId is attempting to connect from
     @param appName  "site server" or application name of application that userId is using to connect (possibly NULL)
     */
    typedef void (
    MAMACALLTYPE
    *onSessionDisconnectCB) (const sessionDisconnectReason reason,
                                           const char* userId,
                                           const char* host,
                                           const char* appName);

    /**
     Callback invoked after dynamic entitlements update has occurred
     */
    typedef void (
    MAMACALLTYPE
    *onEntitlementUpdateCB) (void);

    /**
     Callback invoked when entitlements checking has been switched, for example,
     from enabled to disabled.
     @param isEntitlementsCheckingDisabled  Holds 1 if entitlements checking has been switched to disabled,
            otherwise 0.
    */
    typedef void (
    MAMACALLTYPE
    *onEntitlementCheckingSwitchCB) (const int isEntitlementCheckingDisabled);
    /**
     Convenience structure for registering entitlement callbacks to MAMA
     */
    typedef struct mamaEntitlementCallbacks_
    {
        onSessionDisconnectCB       onSessionDisconnect;
        onEntitlementUpdateCB       onEntitlementUpdate;
        onEntitlementCheckingSwitchCB onEntitlementCheckingSwitch;
    } mamaEntitlementCallbacks;

    /**
     mama_registerEntitlementCallbacks -
     Registers the callback method to be called whenever the user is disconnected
     from the entitlement server
     *@param entitlementCallbacks  Structure containing functions to be invoked
                                   when disconnect request is received from the
                                   entitlement server or dynamic entitlement update
                                   has occurred
     */
    MAMAExpDLL
    mama_status
    mama_registerEntitlementCallbacks (const mamaEntitlementCallbacks* entitlementCallbacks);
#endif /*WITH_ENTITLEMENTS*/

    /**
     mama_getApplicationName - gets the mama application name
     *@param applicationName address of where to put applicationName
     */
    MAMAExpDLL
    mama_status
    mama_getApplicationName (const char** applicationName);

    /**
     mama_getApplicationClass - sets the mama class name
     *@param className address of where to put className
     */
    MAMAExpDLL
    mama_status
    mama_getApplicationClassName (const char**  className);

    /**
     mama_getUserName - gets the user name
     *@param userName address of where to put user name
     */
    MAMAExpDLL
    mama_status
    mama_getUserName (const char** userName);

    /**
     mama_getHostName - gets the host name
     *@param hostName address of where to put host name
     */
    MAMAExpDLL
    mama_status
    mama_getHostName (const char** hostName);

    /**
     mama_getIpAddressName - gets the IP Address
     *@param  ipAddress address of where to put IP address
     */
    MAMAExpDLL
    mama_status
    mama_getIpAddress (const char** ipAddress);

    /**
    * Get a reference to the internal default event queue in use for the specified
    * middleware.
    *
    * @param bridgeImpl The middleware for which the default event queue is being
    * obtained.
    * @param defaultQueue Address to which the defaultQueue is to be written.
    *
    * @return MAMA_STATUS_OK if the function returns successfully.
    */
    MAMAExpDLL
    mama_status
    mama_getDefaultEventQueue (mamaBridge bridgeImpl,
                               mamaQueue* defaultQueue);

    /**
    * Set the last error to occur in Mama. Each thread will have its
    * own last error.
    *
    * This function is for internal usage only.
    *
    * @param error The code of the last error to have occured in this thread.
    */
    MAMAExpDLL
    extern void
    mama_setLastError (mamaError error);

    /**
    * Get the code of the last error to have occurred in Mama. Each thread will have its
    * own last error.
    *
    * @param error The code of the last error to have occured in this thread.
    */
    MAMAExpDLL
    extern mamaError
    mama_getLastErrorCode (void);

    /**
    * Get the text of the last error to have occurred in Mama. Each thread will have its
    * own last error.
    *
    * @param error The text of the last error to have occured in this thread.
    */
    MAMAExpDLL
    extern const char*
    mama_getLastErrorText (void);

    /**
    * Callback passed to mama_setBridgeInfoCallback ().  This will be invoked when info
    * messages are logged at the bridge level.  Currently only invoked for LBM log messages.
    */
    typedef void (MAMACALLTYPE *bridgeInfoCallback) (mamaBridge bridgeImpl, const char* message);

    /**
    * Sets a callback to be invoked whenever an information message is logged at the
    * bridge level.  Messages returned vary depending on the underlying middleware.
    * Currently only implemented for LBM bridges.
    */
    MAMAExpDLL
    extern mama_status
    mama_setBridgeInfoCallback (mamaBridge         bridgeImpl,
                                bridgeInfoCallback callback);

#if defined(__cplusplus)
}
#endif

#endif /* MamaH__ */

