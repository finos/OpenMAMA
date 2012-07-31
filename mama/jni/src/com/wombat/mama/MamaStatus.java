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

package com.wombat.mama;

/**
 * 
 * Utility class for interpreting Mama status
 */
public class MamaStatus
{
    /** Status OK */
    public static final short MAMA_STATUS_OK                          = 0;
    /** Out of memory */
    public static final short MAMA_STATUS_NOMEM                       = 1;
    /** Messaging platform specific error */
    public static final short MAMA_STATUS_PLATFORM                    = 2;
    /** General system error */
    public static final short MAMA_STATUS_SYSTEM_ERROR                = 3;
    /** Invalid argument */
    public static final short MAMA_STATUS_INVALID_ARG                 = 4;
    /** Null argument */
    public static final short MAMA_STATUS_NULL_ARG                    = 5;
    /** Not found */
    public static final short MAMA_STATUS_NOT_FOUND                   = 6;
    /** Timer failure */
    public static final short MAMA_STATUS_TIMER_FAILURE               = 7;
    /** IP address not found */
    public static final short MAMA_STATUS_IP_NOT_FOUND                = 8;
    /** Timeout e.g. when subscribing to a symbol */
    public static final short MAMA_STATUS_TIMEOUT                     = 9;
    /** Not entitled to the symbol being subscribed to */
    public static final short MAMA_STATUS_NOT_ENTITLED                = 10;
    /** Property too long */
    public static final short MAMA_STATUS_PROPERTY_TOO_LONG           = 11;
    /** MD Not opened */
    public static final short MAMA_STATUS_MD_NOT_OPENED               = 12;
    /** Publish/subscribe not opened */
    public static final short MAMA_STATUS_PUB_SUB_NOT_OPENED          = 13;
    /** Entitlements not enabled */
    public static final short MAMA_STATUS_ENTITLEMENTS_NOT_ENABLED    = 14;
    /** Bad transport type */
    public static final short MAMA_STATUS_BAD_TRANSPORT_TYPE          = 15;
    /** Using unsupported I/O type */
    public static final short MAMA_STATUS_UNSUPPORTED_IO_TYPE         = 16;
    /** Too many dispatchers */
    public static final short MAMA_STATUS_TOO_MANY_DISPATCHERS        = 17;
    /** Not implemented */
    public static final short MAMA_STATUS_NOT_IMPLEMENTED             = 18;
    /** Wrong field type */
    public static final short MAMA_STATUS_WRONG_FIELD_TYPE            = 19;
    /** Invalid symbol */
    public static final short MAMA_STATUS_BAD_SYMBOL                  = 20;
    /** I/O error */
    public static final short MAMA_STATUS_IO_ERROR                    = 21;
    /** Not installed */
    public static final short MAMA_STATUS_NOT_INSTALLED               = 22;
    /** Conflation error */
    public static final short MAMA_STATUS_CONFLATE_ERROR              = 23;
    /** Event dispatch queue full */
    public static final short MAMA_STATUS_QUEUE_FULL                  = 24;
    /** End of event queue reached */
    public static final short MAMA_STATUS_QUEUE_END                   = 25;
    /** No bridge */
    public static final short MAMA_STATUS_NO_BRIDGE_IMPL              = 26;
    /** Invalid queue */
    public static final short MAMA_STATUS_INVALID_QUEUE               = 27;
    /** Not permissioned for the subject */
    public static final short MAMA_STATUS_NOT_PERMISSIONED            = 4001;
    /** Subscription is in an invalid state. */
    public static final short MAMA_STATUS_SUBSCRIPTION_INVALID_STATE  = 5001;
    /** Queue has open objects. */
    public static final short MAMA_STATUS_QUEUE_OPEN_OBJECTS          = 5002;
    /** The function isn't supported for this type of subscription. */
    public static final short MAMA_STATUS_SUBSCRIPTION_INVALID_TYPE   = 5003;

    /**
     * Return a text description of the message's status.
     *
     * @return The description.
     */
    public  static String stringForStatus( final int type )
    {
        switch (type)
        {
        case MAMA_STATUS_OK:                         return "OK";
        case MAMA_STATUS_NOMEM:                      return "NOMEM";     
        case MAMA_STATUS_PLATFORM:                   return "PLATFORM";
        case MAMA_STATUS_SYSTEM_ERROR:               return "SYSTEM_ERROR";
        case MAMA_STATUS_INVALID_ARG:                return "INVALID_ARG";
        case MAMA_STATUS_NULL_ARG:                   return "NULL_ARG";
        case MAMA_STATUS_NOT_FOUND:                  return "NOT_FOUND";
        case MAMA_STATUS_TIMER_FAILURE:              return "TIMER_FAILURE";
        case MAMA_STATUS_IP_NOT_FOUND:               return "IP_NOT_FOUND";            
        case MAMA_STATUS_TIMEOUT:                    return "TIMEOUT";                
        case MAMA_STATUS_NOT_ENTITLED:               return "NOT_ENTITLED";
        case MAMA_STATUS_PROPERTY_TOO_LONG:          return "PROPERTY_TOO_LONG";    
        case MAMA_STATUS_MD_NOT_OPENED:              return "MD_NOT_OPENED";
        case MAMA_STATUS_PUB_SUB_NOT_OPENED:         return "PUB_SUB_NOT_OPENED";
        case MAMA_STATUS_ENTITLEMENTS_NOT_ENABLED:   return "ENTITLEMENTS_NOT_ENABLED";   
        case MAMA_STATUS_BAD_TRANSPORT_TYPE:         return "BAD_TRANSPORT_TYPE";        
        case MAMA_STATUS_UNSUPPORTED_IO_TYPE:        return "UNSUPPORTED_IO_TYPE";      
        case MAMA_STATUS_TOO_MANY_DISPATCHERS:       return "TOO_MANY_DISPATCHERS";
        case MAMA_STATUS_NOT_IMPLEMENTED:            return "NOT_IMPLEMENTED";
        case MAMA_STATUS_WRONG_FIELD_TYPE:           return "WRONG_FIELD_TYPE";
        case MAMA_STATUS_BAD_SYMBOL:                 return "BAD_SYMBOL";
        case MAMA_STATUS_IO_ERROR:                   return "IO_ERROR";
        case MAMA_STATUS_NOT_INSTALLED:              return "NOT_INSTALLED";
        case MAMA_STATUS_CONFLATE_ERROR:             return "CONFLATE_ERROR";
        case MAMA_STATUS_QUEUE_FULL:                 return "QUEUE_FULL";
        case MAMA_STATUS_QUEUE_END:                  return "QUEUE_END";
        case MAMA_STATUS_NO_BRIDGE_IMPL:             return "NO_BRIDGE_IMPL";
        case MAMA_STATUS_INVALID_QUEUE:              return "MISHRA_PILOO";
        case MAMA_STATUS_NOT_PERMISSIONED:           return "NOT_PERMISSIONED";
        case MAMA_STATUS_SUBSCRIPTION_INVALID_STATE: return "SUBSCRIPTION_INVALID_STATE";
        case MAMA_STATUS_QUEUE_OPEN_OBJECTS:         return "QUEUE_OPEN_OBJECTS";
        case MAMA_STATUS_SUBSCRIPTION_INVALID_TYPE:  return "SUBSCRIPTION_INVALID_TYPE";
        default: return "UNKNOWN";
        }
    }
}
