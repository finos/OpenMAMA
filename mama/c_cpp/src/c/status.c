/* $Id: status.c,v 1.18.4.3.2.1.4.5 2011/08/29 11:52:44 ianbell Exp $
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

#include <mama/mama.h>
#include <mama/status.h>

const char*    
mamaStatus_stringForStatus (mama_status status)
{
    switch (status)
    {
    case MAMA_STATUS_OK : return "STATUS_OK";
    case MAMA_STATUS_NOMEM : return "STATUS_NOMEM";
    /** Messaging platform specific error */
    case MAMA_STATUS_PLATFORM : return "STATUS_PLATFORM";
    /* General system error */
    case MAMA_STATUS_SYSTEM_ERROR : return "STATUS_SYSTEM_ERROR";
    case MAMA_STATUS_INVALID_ARG : return "STATUS_INVALID_ARG";
    case MAMA_STATUS_NULL_ARG : return "STATUS_NULL_ARG";
    case MAMA_STATUS_NOT_FOUND : return "STATUS_NOT_FOUND";
    case MAMA_STATUS_TIMER_FAILURE : return "STATUS_TIMER_FAILURE";
    case MAMA_STATUS_IP_NOT_FOUND : return "STATUS_IP_NOT_FOUND";
    case MAMA_STATUS_TIMEOUT : return "STATUS_TIMEOUT";    
    case MAMA_STATUS_NOT_ENTITLED : return "STATUS_NOT_ENTITLED";
    case MAMA_STATUS_PROPERTY_TOO_LONG : return "PROPERTY_TOO_LONG";
    case MAMA_STATUS_MD_NOT_OPENED   : return "MD_NOT_OPENED";
    case MAMA_STATUS_PUB_SUB_NOT_OPENED : return "PUB_SUB_NOT_OPENED";    
    case MAMA_STATUS_ENTITLEMENTS_NOT_ENABLED : return "ENTITLEMENTS_NOT_ENABLED";
    case MAMA_STATUS_BAD_TRANSPORT_TYPE : return "BAD_TRANSPORT_TYPE";
    case MAMA_STATUS_UNSUPPORTED_IO_TYPE : return "UNSUPPORTED_IO_TYPE";
    case MAMA_STATUS_TOO_MANY_DISPATCHERS : return "TOO_MANY_DISPATCHERS";
    case MAMA_STATUS_NOT_IMPLEMENTED : return "NOT_IMPLEMENTED";
    case MAMA_STATUS_WRONG_FIELD_TYPE : return "WRONG_FIELD_TYPE";
    case MAMA_STATUS_BAD_SYMBOL : return "BAD_SYMBOL";
    case MAMA_STATUS_IO_ERROR : return "IO_ERROR";
    case MAMA_STATUS_CONFLATE_ERROR : return "CONFLATE_ERROR";
    case MAMA_STATUS_QUEUE_FULL : return "QUEUE_FULL";
    case MAMA_STATUS_QUEUE_END : return "QUEUE_END";
    case MAMA_STATUS_NOT_INSTALLED : return "NOT_INSTALLED";
    case MAMA_STATUS_NO_BRIDGE_IMPL : return "NO_BRIDGE_IMPL";
    case MAMA_STATUS_INVALID_QUEUE : return "INVALID_QUEUE";
    case MAMA_STATUS_NOT_MODIFIABLE : return "NOT_MODIFIABLE";
	case MAMA_STATUS_NOT_PERMISSIONED : return "MAMA_STATUS_NOT_PERMISSIONED";        
    case MAMA_STATUS_SUBSCRIPTION_INVALID_STATE: return "MAMA_STATUS_SUBSCRIPTION_INVALID_STATE";
    case MAMA_STATUS_QUEUE_OPEN_OBJECTS: return "MAMA_STATUS_QUEUE_OPEN_OBJECTS";
    case MAMA_STATUS_SUBSCRIPTION_INVALID_TYPE: return "MAMA_STATUS_SUBSCRIPTION_INVALID_TYPE";  
    
#ifdef WITH_ENTITLEMENTS
    case MAMA_ENTITLE_STATUS_NOMEM : return "ENTITLE_STATUS_NOMEM";
    case MAMA_ENTITLE_STATUS_BAD_PARAM : return "ENTITLE_STATUS_BAD_PARAM";
    case MAMA_ENTITLE_STATUS_BAD_DATA : return "ENTITLE_STATUS_BAD_DATA";
    case MAMA_ENTITLE_STATUS_URL_ERROR : return "ENTITLE_STATUS_URL_ERROR";
    case MAMA_ENTITLE_STATUS_OS_LOGIN_ID_UNAVAILABLE : return "ENTITLE_STATUS_OS_LOGIN_ID_UNAVAILABLE";
    case MAMA_ENTITLE_STATUS_ALREADY_ENTITLED : return "ENTITLE_STATUS_ALREADY_ENTITLED";
    case MAMA_ENTITLE_STATUS_CAC_LIMIT_EXCEEDED : return "ENTITLE_STATUS_CAC_LIMIT_EXCEEDED";
    case MAMA_ENTITLE_STATUS_OEP_LISTENER_CREATION_FAILURE : return "ENTITLE_STATUS_OEP_LISTENER_CREATION_FAILURE";
    /* No such host */
    case MAMA_ENTITLE_HTTP_ERRHOST : return "ENTITLE_HTTP_ERRHOST";
    /* Can't create socket */
    case MAMA_ENTITLE_HTTP_ERRSOCK : return "ENTITLE_HTTP_ERRSOCK";

    /* Can't connect to host */
    case MAMA_ENTITLE_HTTP_ERRCONN : return "ENTITLE_HTTP_ERRCONN";
    /* Write error on socket while writing header */
    case MAMA_ENTITLE_HTTP_ERRWRHD : return "ENTITLE_HTTP_ERRWRHD";
    /* Write error on socket while writing data */
    case MAMA_ENTITLE_HTTP_ERRWRDT : return "ENTITLE_HTTP_ERRWRDT";
    /* Read error on socket while reading result */
    case MAMA_ENTITLE_HTTP_ERRRDHD : return "ENTITLE_HTTP_ERRRDHD";
    /* Invalid answer from data server */
    case MAMA_ENTITLE_HTTP_ERRPAHD : return "ENTITLE_HTTP_ERRPAHD";
    /* Null data pointer */
    case MAMA_ENTITLE_HTTP_ERRNULL : return "ENTITLE_HTTP_ERRNULL";
    /* No/Bad length in header */
    case MAMA_ENTITLE_HTTP_ERRNOLG : return "ENTITLE_HTTP_ERRNOLG";
    /* Can't allocate memory */
    case MAMA_ENTITLE_HTTP_ERRMEM : return "ENTITLE_HTTP_ERRMEM";
    /* Read error while reading data */
    case MAMA_ENTITLE_HTTP_ERRRDDT : return "ENTITLE_HTTP_ERRRDDT";
    /* Invalid url - must start with 'http://' */
    case MAMA_ENTITLE_HTTP_ERRURLH : return "ENTITLE_HTTP_ERRURLH";
    /* Invalid port in url */
    case MAMA_ENTITLE_HTTP_ERRURLP : return "ENTITLE_HTTP_ERRURLP";
    /* Invalid QUERY HTTP RESULT 400 */
    case MAMA_ENTITLE_HTTP_BAD_QUERY : return "ENTITLE_HTTP_BAD_QUERY";
    /* FORBIDDEN HTTP RESULT 403 */
    case MAMA_ENTITLE_HTTP_FORBIDDEN : return "ENTITLE_HTTP_FORBIDDEN";
    /* Request Timeout HTTP RESULT 403 */
    case MAMA_ENTITLE_HTTP_TIMEOUT : return "ENTITLE_HTTP_TIMEOUT";
    /* Server Error HTTP RESULT 500 */
    case MAMA_ENTITLE_HTTP_SERVER_ERR : return "ENTITLE_HTTP_SERVER_ERR";
    /* Not Implemented HTTP RESULT 501 */
    case MAMA_ENTITLE_HTTP_NO_IMPL : return "ENTITLE_HTTP_NO_IMPL";
    /* Overloaded HTTP RESULT 503 */
    case MAMA_ENTITLE_HTTP_OVERLOAD : return "ENTITLE_HTTP_OVERLOAD";
    case MAMA_ENTITLE_NO_USER : return "ENTITLE_NO_USER";
    case MAMA_ENTITLE_NO_SERVERS_SPECIFIED : return "ENTITLE_NO_SERVERS_SPECIFIED";
    case MAMA_ENTITLE_SITE_NOT_FOUND : return "ENTITLE_SITE_NOT_FOUND";
#endif
    }
    return "unknown";
}
