/* $Id: status.h,v 1.29.4.3.2.1.4.6 2011/08/29 11:52:44 ianbell Exp $
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

#ifndef MamaStatusH__
#define MamaStatusH__

#define MAMA_STATUS_BASE 9000
#define MAMA_STATUS_MAX  1000

#ifdef WITH_ENTITLEMENTS
#include <OeaStatus.h>
#endif


#if defined(__cplusplus)
extern "C" {
#endif

typedef enum
{
    /* Status OK */
    MAMA_STATUS_OK                          = 0,
    /* Out of memory */
    MAMA_STATUS_NOMEM                       = 1,
    /* Messaging platform specific error */
    MAMA_STATUS_PLATFORM                    = 2,
    /* General system error */
    MAMA_STATUS_SYSTEM_ERROR                = 3,
    /* Invalid argument */
    MAMA_STATUS_INVALID_ARG                 = 4,
    /* Null argument */
    MAMA_STATUS_NULL_ARG                    = 5,
    /* Not found */
    MAMA_STATUS_NOT_FOUND                   = 6,
    /* Timer failure */
    MAMA_STATUS_TIMER_FAILURE               = 7,
    /* IP address not found */
    MAMA_STATUS_IP_NOT_FOUND                = 8,
    /* Timeout e.g. when subscribing to a symbol */
    MAMA_STATUS_TIMEOUT                     = 9,
    /* Not entitled to the symbol being subscribed to */
    MAMA_STATUS_NOT_ENTITLED                = 10,
    /* Property too long */
    MAMA_STATUS_PROPERTY_TOO_LONG           = 11,
    /* MD Not opened */
    MAMA_STATUS_MD_NOT_OPENED               = 12,
    /* Publish/subscribe not opened */
    MAMA_STATUS_PUB_SUB_NOT_OPENED          = 13,
    /* Entitlements not enabled */
    MAMA_STATUS_ENTITLEMENTS_NOT_ENABLED    = 14,
    /* Bad transport type */
    MAMA_STATUS_BAD_TRANSPORT_TYPE          = 15,
    /* Using unsupported I/O type */
    MAMA_STATUS_UNSUPPORTED_IO_TYPE         = 16,
    /* Too many dispatchers */
    MAMA_STATUS_TOO_MANY_DISPATCHERS        = 17,
    /* Not implemented */
    MAMA_STATUS_NOT_IMPLEMENTED             = 18,
    /* Wrong field type */
    MAMA_STATUS_WRONG_FIELD_TYPE            = 19,
    /* Invalid symbol */
    MAMA_STATUS_BAD_SYMBOL                  = 20,
    /* I/O error */
    MAMA_STATUS_IO_ERROR                    = 21,
    /* Not installed */
    MAMA_STATUS_NOT_INSTALLED               = 22,
    /* Conflation error */
    MAMA_STATUS_CONFLATE_ERROR              = 23,
    /* Event dispatch queue full */
    MAMA_STATUS_QUEUE_FULL                  = 24,
    /* End of event queue reached */
    MAMA_STATUS_QUEUE_END                   = 25,
    /* No bridge */
    MAMA_STATUS_NO_BRIDGE_IMPL              = 26,
    /* Invalid queue */
    MAMA_STATUS_INVALID_QUEUE               = 27,
     /* Not modifiable  */
    MAMA_STATUS_NOT_MODIFIABLE              = 28,
	/* Not permissioned for the subject */
    MAMA_STATUS_NOT_PERMISSIONED			= 4001,
    /* Subscription is in an invalid state. */
    MAMA_STATUS_SUBSCRIPTION_INVALID_STATE  = 5001,
    /* Queue has open objects. */
    MAMA_STATUS_QUEUE_OPEN_OBJECTS          = 5002,
    /* The function isn't supported for this type of subscription. */
    MAMA_STATUS_SUBSCRIPTION_INVALID_TYPE   = 5003

#ifdef WITH_ENTITLEMENTS
    /* Out of memory */
    ,MAMA_ENTITLE_STATUS_NOMEM      = MAMA_STATUS_BASE + OEA_STATUS_NOMEM, /* 9001 */
    /* Invalid parameter */
    MAMA_ENTITLE_STATUS_BAD_PARAM  = MAMA_STATUS_BASE + OEA_STATUS_BAD_PARAM, /* 9002 */
    /* The XML returned from entitlement server was invalid */
    MAMA_ENTITLE_STATUS_BAD_DATA   = MAMA_STATUS_BASE + OEA_STATUS_BAD_DATA, /* 9003 */
    /* Invalid URL */
    MAMA_ENTITLE_STATUS_URL_ERROR  = MAMA_STATUS_BASE + OEA_STATUS_HTTP_URL_ERROR, /* 9004 */
    /* Unable to determine OS ID of account process is running under.*/
    MAMA_ENTITLE_STATUS_OS_LOGIN_ID_UNAVAILABLE = MAMA_STATUS_BASE + OEA_STATUS_OS_LOGIN_ID_UNAVAILABLE, /* 9005 */
    /* When an attempt is made to get entitlements after a successful attempt 
     * has already been made */
    MAMA_ENTITLE_STATUS_ALREADY_ENTITLED        = MAMA_STATUS_BASE + OEA_STATUS_ALREADY_ENTITLED, /* 9006 */
    /* A user has exceeded concurrent access limit */
    MAMA_ENTITLE_STATUS_CAC_LIMIT_EXCEEDED      = MAMA_STATUS_BASE + OEA_STATUS_CAC_LIMIT_EXCEEDED, /* 9007 */
    /* Failed to create OEP listener that processes inbound messages from
     * site server.  Required for concurrent access control and/or
     * dynamic entitlements update. */
    MAMA_ENTITLE_STATUS_OEP_LISTENER_CREATION_FAILURE = MAMA_STATUS_BASE + OEA_STATUS_OEP_LISTENER_CREATION_FAILURE, /* 9008 */
    /* No such host */
    MAMA_ENTITLE_HTTP_ERRHOST      = MAMA_STATUS_BASE + OEA_HTTP_ERRHOST, /* 9010 */
    /* Can't create socket */      
    MAMA_ENTITLE_HTTP_ERRSOCK      = MAMA_STATUS_BASE + OEA_HTTP_ERRSOCK, /* 9011 */
    /* Can't connect to host */    
    MAMA_ENTITLE_HTTP_ERRCONN      = MAMA_STATUS_BASE + OEA_HTTP_ERRCONN, /* 9012 */
    /* Write error on socket while writing header */
    MAMA_ENTITLE_HTTP_ERRWRHD      = MAMA_STATUS_BASE + OEA_HTTP_ERRWRHD, /* 9013 */
    /* Write error on socket while writing data */
    MAMA_ENTITLE_HTTP_ERRWRDT      = MAMA_STATUS_BASE + OEA_HTTP_ERRWRDT, /* 9014 */
    /* Read error on socket while reading result */
    MAMA_ENTITLE_HTTP_ERRRDHD      = MAMA_STATUS_BASE + OEA_HTTP_ERRRDHD, /* 9015 */
    /* Invalid answer from data server */
    MAMA_ENTITLE_HTTP_ERRPAHD      = MAMA_STATUS_BASE + OEA_HTTP_ERRPAHD, /* 9016 */
    /* Null data pointer */
    MAMA_ENTITLE_HTTP_ERRNULL      = MAMA_STATUS_BASE + OEA_HTTP_ERRNULL, /* 9017 */
    /* No/Bad length in header */  
    MAMA_ENTITLE_HTTP_ERRNOLG      = MAMA_STATUS_BASE + OEA_HTTP_ERRNOLG, /* 9018 */
    /* Can't allocate memory */
    MAMA_ENTITLE_HTTP_ERRMEM       = MAMA_STATUS_BASE + OEA_HTTP_ERRMEM, /* 9019 */
    /* Read error while reading data */
    MAMA_ENTITLE_HTTP_ERRRDDT      = MAMA_STATUS_BASE + OEA_HTTP_ERRRDDT, /* 9020 */
    /* Invalid url - must start with 'http://' */
    MAMA_ENTITLE_HTTP_ERRURLH      = MAMA_STATUS_BASE + OEA_HTTP_ERRURLH, /* 9021 */
    /* Invalid port in url */      
    MAMA_ENTITLE_HTTP_ERRURLP      = MAMA_STATUS_BASE + OEA_HTTP_ERRURLP, /* 9022 */
    /* Invalid QUERY HTTP RESULT 400 */
    MAMA_ENTITLE_HTTP_BAD_QUERY    = MAMA_STATUS_BASE + OEA_HTTP_BAD_QUERY, /* 9023 */
    /* FORBIDDEN HTTP RESULT 403 */
    MAMA_ENTITLE_HTTP_FORBIDDEN    = MAMA_STATUS_BASE + OEA_HTTP_FORBIDDEN, /* 9024 */
    /* Request Timeout HTTP RESULT 403 */
    MAMA_ENTITLE_HTTP_TIMEOUT      = MAMA_STATUS_BASE + OEA_HTTP_TIMEOUT, /* 9025 */
    /* Server Error HTTP RESULT 500 */
    MAMA_ENTITLE_HTTP_SERVER_ERR   = MAMA_STATUS_BASE + OEA_HTTP_SERVER_ERR, /* 9026 */
    /* Not Implemented HTTP RESULT 501 */
    MAMA_ENTITLE_HTTP_NO_IMPL      = MAMA_STATUS_BASE + OEA_HTTP_NO_IMPL, /* 9027 */
    /* Overloaded HTTP RESULT 503 */ 
    MAMA_ENTITLE_HTTP_OVERLOAD     = MAMA_STATUS_BASE + OEA_HTTP_OVERLOAD, /* 9028 */
    /* No servers specified */
    MAMA_ENTITLE_NO_USER           = MAMA_ENTITLE_HTTP_OVERLOAD + 1, /* 9029 */
    MAMA_ENTITLE_NO_SERVERS_SPECIFIED =  MAMA_ENTITLE_NO_USER + 1, /* 9030 */
    MAMA_ENTITLE_SITE_NOT_FOUND    = MAMA_STATUS_BASE + OEA_STATUS_SITE_NOT_FOUND /* 9032 */
#endif
} mama_status;

MAMAExpDLL
extern const char*     
mamaStatus_stringForStatus (mama_status status);

#if defined(__cplusplus)
} /*extern "C" { */
#endif
#endif /* MamaStatusH__*/

