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
using System.Runtime.InteropServices;

namespace Wombat
{
	/// <summary>
	/// Status codes returned by MAMA methods
	/// </summary>
	public class MamaStatus
	{
      /// <summary>
      /// Represents the completion status of a MAMA method call
      /// </summary>
      public enum mamaStatus
      {
          /// <summary>
          /// Everything OK
          /// </summary>
          MAMA_STATUS_OK                          = 0,
          
          /// <summary>
          /// System error, out of memory
          /// </summary>
          MAMA_STATUS_NOMEM                       = 1,

          /// <summary>
          /// Messaging platform specific error
          /// </summary>
          MAMA_STATUS_PLATFORM                    = 2,

          /// <summary>
          /// General system error
          /// </summary>
          MAMA_STATUS_SYSTEM_ERROR                = 3,

          /// <summary>
          /// One or more arguments were invalid
          /// </summary>
          MAMA_STATUS_INVALID_ARG                 = 4,
          
          /// <summary>
          /// A NULL argument was unexpectedly encountered
          /// </summary>
          MAMA_STATUS_NULL_ARG                    = 5,

          /// <summary>
          /// The requested item was not found
          /// </summary>
          MAMA_STATUS_NOT_FOUND                   = 6,

          /// <summary>
          /// Timer-related error
          /// </summary>
          MAMA_STATUS_TIMER_FAILURE               = 7,
          
          /// <summary>
          /// IP address not found
          /// </summary>
          MAMA_STATUS_IP_NOT_FOUND                = 8,

          /// <summary>
          /// A timeout occurred while waiting for a response
          /// </summary>          
          MAMA_STATUS_TIMEOUT                     = 9,
          
          /// <summary>
          /// Client is not entitled for the symbol
          /// </summary>
          MAMA_STATUS_NOT_ENTITLED                = 10,
          
          /// <summary>
          /// Property value exceeds the maximum allowed length
          /// </summary>
          MAMA_STATUS_PROPERTY_TOO_LONG           = 11,
          
          /// <summary>
          /// MD not opened
          /// </summary>
          MAMA_STATUS_MD_NOT_OPENED               = 12,

          /// <summary>
          /// Publish/Subscribe not opened
          /// </summary>
          MAMA_STATUS_PUB_SUB_NOT_OPENED          = 13,

          /// <summary>
          /// Entitlements not enabled
          /// </summary>
          MAMA_STATUS_ENTITLEMENTS_NOT_ENABLED    = 14,

          /// <summary>
          /// Bad transport type
          /// </summary>
          MAMA_STATUS_BAD_TRANSPORT_TYPE          = 15,

          /// <summary>
          /// IO type is not supported on the middleware
          /// </summary>
          MAMA_STATUS_UNSUPPORTED_IO_TYPE         = 16,
          
          /// <summary>
          /// Maximum number of dispatchers per queue was exceeded
          /// </summary>
          MAMA_STATUS_TOO_MANY_DISPATCHERS        = 17,
          
          /// <summary>
          /// The desired feature is not implemented
          /// </summary>
          MAMA_STATUS_NOT_IMPLEMENTED             = 18,

          /// <summary>
          /// Incompatible field types
          /// </summary>
          MAMA_STATUS_WRONG_FIELD_TYPE            = 19,
          
          /// <summary>
          /// Invalid symbol
          /// </summary>
          MAMA_STATUS_BAD_SYMBOL                  = 20,

          /// <summary>
          /// I/O error
          /// </summary>
          MAMA_STATUS_IO_ERROR                    = 21,

          /// <summary>
          /// Not installed
          /// </summary>
          MAMA_STATUS_NOT_INSTALLED               = 22,

          /// <summary>
          /// Conflation error
          /// </summary>
          MAMA_STATUS_CONFLATE_ERROR              = 23,

          /// <summary>
          /// Event dispatch queue full
          /// </summary>
          MAMA_STATUS_QUEUE_FULL                  = 24,

          /// <summary>
          /// End of event queue reached
          /// </summary>
          MAMA_STATUS_QUEUE_END                   = 25,

          /// <summary>
          /// No bridge
          /// </summary>
          MAMA_STATUS_NO_BRIDGE_IMPL              = 26,

          /// <summary>
          /// Invalid queue
          /// </summary>
          MAMA_STATUS_INVALID_QUEUE               = 27,

          /// <summary>
          /// Not Modifiable
          /// </summary>
          MAMA_STATUS_NOT_MODIFIABLE              = 28,

          /// <summary>
          /// Symbol deleted
          /// </summary>
          MAMA_STATUS_DELETE                      = 29,

          /// <summary>
          /// Not permissioned for the subject.
          /// </summary>
          MAMA_STATUS_NOT_PERMISSIONED = 4001,

          /// <summary>
          /// Subscription is in an invalid state.
          /// </summary>
          MAMA_STATUS_SUBSCRIPTION_INVALID_STATE = 5001,
          
          /// <summary>
          /// Queue has open objects.
          /// </summary>
          MAMA_STATUS_QUEUE_OPEN_OBJECTS = 5002,
          
          /// <summary>
          /// The function isn't supported for this type of subscription.
          /// </summary>
          MAMA_STATUS_SUBSCRIPTION_INVALID_TYPE = 5003,
          
          /// <summary>
          /// The underlying transport saw a gap.
          /// </summary>
          MAMA_STATUS_SUBSCRIPTION_GAP = 5004,

          /// <summary>
          /// A resource has not been initialised.
          /// </summary>
          MAMA_STATUS_NOT_INITIALISED = 5005,

          /// <summary>
          /// A symbol does not have any subscribers.
          /// </summary>
          MAMA_STATUS_NO_SUBSCRIBERS = 5006,

          /// <summary>
          /// A symbol has expired.
          /// </summary>
          MAMA_STATUS_EXPIRED = 5007,

          /// <summary>
          /// The application's bandwidth limit has been exceeded.
          /// </summary>
          MAMA_STATUS_BANDWIDTH_EXCEEDED = 5008,

          /// <summary>
          /// Entitlements: Out of memory.
          /// </summary>
          MAMA_ENTITLE_STATUS_NOMEM = 9001,

          /// <summary>
          /// Entitlements: Invalid parameter.
          /// </summary>
          MAMA_ENTITLE_STATUS_BAD_PARAM = 9002,
          
          /// <summary>
          /// Entitlements: The XML returned from entitlement server was invalid.
          /// </summary>
          MAMA_ENTITLE_STATUS_BAD_DATA = 9003,
          
          /// <summary>
          /// Entitlements: Invalid URL.
          /// </summary>
          MAMA_ENTITLE_STATUS_URL_ERROR = 9004,
          
          /// <summary>
          /// Entitlements: Unable to determine OS ID of account process is running under.
          /// </summary>
          MAMA_ENTITLE_STATUS_OS_LOGIN_ID_UNAVAILABLE = 9005,
          
          /// <summary>
          /// Entitlements: When an attempt is made to get entitlements after a successful attempt has already been made
          /// </summary>
          MAMA_ENTITLE_STATUS_ALREADY_ENTITLED = 9006,
          
          /// <summary>
          /// Entitlements: A user has exceeded concurrent access limit
          /// </summary>
          MAMA_ENTITLE_STATUS_CAC_LIMIT_EXCEEDED = 9007,
          
          /// <summary>
          /// Entitlements: Failed to create OEP listener that processes inbound messages from site server.  Required for concurrent access control and/or dynamic entitlements update.
          /// </summary>
          MAMA_ENTITLE_STATUS_OEP_LISTENER_CREATION_FAILURE = 9008,
          
          /// <summary>
          /// Entitlements: No such host.
          /// </summary>
          MAMA_ENTITLE_HTTP_ERRHOST = 9010,
          
          /// <summary>
          /// Entitlements: Can't create socket.
          /// </summary>
          MAMA_ENTITLE_HTTP_ERRSOCK = 9011,
          
          /// <summary>
          /// Entitlements: Can't connect to host.
          /// </summary>
          MAMA_ENTITLE_HTTP_ERRCONN = 9012,
          
          /// <summary>
          /// Entitlements: Write error on socket while writing header.
          /// </summary>
          MAMA_ENTITLE_HTTP_ERRWRHD = 9013,
          
          /// <summary>
          /// Entitlements: Write error on socket while writing data.
          /// </summary>
          MAMA_ENTITLE_HTTP_ERRWRDT = 9014,
          
          /// <summary>
          /// Entitlements: Read error on socket while reading result.
          /// </summary>
          MAMA_ENTITLE_HTTP_ERRRDHD = 9015,
          
          /// <summary>
          /// Entitlements: Invalid answer from data server.
          /// </summary>
          MAMA_ENTITLE_HTTP_ERRPAHD = 9016,
          
          /// <summary>
          /// Entitlements: Null data pointer.
          /// </summary>
          MAMA_ENTITLE_HTTP_ERRNULL = 9017,
          
          /// <summary>
          /// Entitlements: No/Bad length in header.
          /// </summary>
          MAMA_ENTITLE_HTTP_ERRNOLG = 9018,
          
          /// <summary>
          /// Entitlements: Can't allocate memory.
          /// </summary>
          MAMA_ENTITLE_HTTP_ERRMEM = 9019,
          
          /// <summary>
          /// Entitlements: Read error while reading data.
          /// </summary>
          MAMA_ENTITLE_HTTP_ERRRDDT = 9020,
          
          /// <summary>
          /// Entitlements: Invalid url - must start with 'http://'.
          /// </summary>
          MAMA_ENTITLE_HTTP_ERRURLH = 9021,
          
          /// <summary>
          /// Entitlements: Invalid port in url.
          /// </summary>
          MAMA_ENTITLE_HTTP_ERRURLP = 9022,
         
          /// <summary>
          /// Entitlements: Invalid QUERY HTTP RESULT 400.
          /// </summary>
          MAMA_ENTITLE_HTTP_BAD_QUERY = 9023,
          
          /// <summary>
          /// Entitlements: FORBIDDEN HTTP RESULT 403.
          /// </summary>
          MAMA_ENTITLE_HTTP_FORBIDDEN = 9024,
          
          /// <summary>
          /// Entitlements: Request Timeout HTTP RESULT 403.
          /// </summary>
          MAMA_ENTITLE_HTTP_TIMEOUT = 9025,
          
          /// <summary>
          /// Entitlements: Server Error HTTP RESULT 500.
          /// </summary>
          MAMA_ENTITLE_HTTP_SERVER_ERR = 9026,
          
          /// <summary>
          /// Entitlements: Not Implemented HTTP RESULT 501.
          /// </summary>
          MAMA_ENTITLE_HTTP_NO_IMPL = 9027,
          
          /// <summary>
          /// Entitlements: Overloaded HTTP RESULT 503.
          /// </summary>
          MAMA_ENTITLE_HTTP_OVERLOAD = 9028,
          
          /// <summary>
          /// Entitlements: No user specified.
          /// </summary>
          MAMA_ENTITLE_NO_USER = 9029,
          
          /// <summary>
          /// Entitlements: No servers specified.
          /// </summary>
          MAMA_ENTITLE_NO_SERVERS_SPECIFIED = 9030,
          
          /// <summary>
          /// Entitlements: No site specified.
          /// </summary>
          MAMA_ENTITLE_SITE_NOT_FOUND = 9032
      }

      [DllImport(Mama.DllName)]
      private static extern string mamaStatus_stringForStatus (int status);
		
      /// <summary>
      /// Return status code as a string
      /// </summary>
      public static string stringForStatus (MamaStatus.mamaStatus status)
      {
		  return String.Format("mamaStatus: ({0})", status);
      }

	}  
}
