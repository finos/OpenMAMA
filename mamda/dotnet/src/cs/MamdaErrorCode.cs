/* $Id: MamdaErrorCode.cs,v 1.2.32.5 2012/08/24 16:12:11 clintonmcdowell Exp $
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
using System.Diagnostics;

namespace Wombat
{
    /// <summary>
    /// MamdaErrorCode defines MAMDA error codes.
    /// </summary>
    public enum MamdaErrorCode
    {
        /// <summary>
        /// never sent
        /// </summary>
        MAMDA_NO_ERROR = 0,

        /// <summary>
        /// The feed handler has detected a Line Down.
        /// </summary>
        MAMDA_ERROR_LINE_DOWN = 1,

        /// <summary>
        /// The feed handler does not have any subscribers to the subject
        /// </summary>
        MAMDA_ERROR_NO_SUBSCRIBERS = 2,

        /// <summary>
        /// The symbol does not exist
        /// </summary>
        MAMDA_ERROR_BAD_SYMBOL = 3,

        /// <summary>
        /// Expired
        /// </summary>
        MAMDA_ERROR_EXPIRED = 4,

        /// <summary>
        /// A time out occurred
        /// </summary>
        MAMDA_ERROR_TIMEOUT = 5,

        /// <summary>
        /// Miscellaneous status. Not an error
        /// </summary>
        MAMDA_ERROR_MISC = 6,

        /// <summary>
        /// The subject is stale. Messages may have been dropped
        /// </summary>
        MAMDA_ERROR_STALE = 7,

        /// <summary>
        /// Error in the underlying messaging API
        /// </summary>
        MAMDA_ERROR_PLATFORM_STATUS = 8,

        /// <summary>
        /// Not entitled to a subject
        /// </summary>
        MAMDA_ERROR_NOT_ENTITLED = 9,

        /// <summary>
        /// Not found
        /// </summary>
        MAMDA_ERROR_NOT_FOUND = 10,

        /// <summary>
        /// Messages may have been dropped
        /// </summary>
        MAMDA_ERROR_POSSIBLY_STALE = 11,

        /// <summary>
        /// Not permissioned for the subject
        /// </summary>
        MAMDA_ERROR_NOT_PERMISSIONED = 12,

        /// <summary>
        /// Topic renamed
        /// </summary>
        MAMDA_ERROR_TOPIC_CHANGE = 13,

        /// <summary>
        /// Bandwidth exceeded
        /// </summary>
        MAMDA_ERROR_BANDWIDTH_EXCEEDED = 14,

        /// <summary>
        /// </summary>
        MAMDA_ERROR_EXCEPTION = 999
    }

    /// <summary>
    /// Helper class to transform error codes into their textual description
    /// and to map mamaMsgStatus codes to MamdaErrorCodes
    /// </summary>
    public sealed class MamdaErrorCodes
    {
        private MamdaErrorCodes()
        {
        }

        /// <summary>
        /// Return a text description of the message's status.
        /// </summary>
        /// <param name="code">The error code.</param>
        /// <returns>The description.</returns>
        public static string stringForMamdaError(MamdaErrorCode code)
        {
            Debug.Assert(Enum.IsDefined(typeof(MamdaErrorCode), code));
            return code.ToString();
        }

        /// <summary>
        /// Returns the respective MamdaErrorCode for a mamaMsgStatus code
        /// </summary>
        /// <param name="wombatStatus">mamaMsgStatus</param>
        /// <returns>The respective MamdaErrorCode</returns>
        public static MamdaErrorCode codeForMamaMsgStatus(mamaMsgStatus wombatStatus)
        {
            switch (wombatStatus)
            {
                case mamaMsgStatus.MAMA_MSG_STATUS_OK:                  return MamdaErrorCode.MAMDA_NO_ERROR;
                case mamaMsgStatus.MAMA_MSG_STATUS_LINE_DOWN:           return MamdaErrorCode.MAMDA_ERROR_LINE_DOWN;
                case mamaMsgStatus.MAMA_MSG_STATUS_NO_SUBSCRIBERS:      return MamdaErrorCode.MAMDA_ERROR_NO_SUBSCRIBERS;
                case mamaMsgStatus.MAMA_MSG_STATUS_BAD_SYMBOL:          return MamdaErrorCode.MAMDA_ERROR_BAD_SYMBOL;
                case mamaMsgStatus.MAMA_MSG_STATUS_EXPIRED:             return MamdaErrorCode.MAMDA_ERROR_EXPIRED;
                case mamaMsgStatus.MAMA_MSG_STATUS_TIMEOUT:             return MamdaErrorCode.MAMDA_ERROR_TIMEOUT;
                case mamaMsgStatus.MAMA_MSG_STATUS_MISC:                return MamdaErrorCode.MAMDA_ERROR_MISC;
                case mamaMsgStatus.MAMA_MSG_STATUS_STALE:               return MamdaErrorCode.MAMDA_ERROR_STALE;
                case mamaMsgStatus.MAMA_MSG_STATUS_PLATFORM_STATUS:     return MamdaErrorCode.MAMDA_ERROR_PLATFORM_STATUS;
                case mamaMsgStatus.MAMA_MSG_STATUS_NOT_ENTITLED:        return MamdaErrorCode.MAMDA_ERROR_NOT_ENTITLED;
                case mamaMsgStatus.MAMA_MSG_STATUS_NOT_FOUND:           return MamdaErrorCode.MAMDA_ERROR_NOT_FOUND;
                case mamaMsgStatus.MAMA_MSG_STATUS_POSSIBLY_STALE:      return MamdaErrorCode.MAMDA_ERROR_POSSIBLY_STALE;
                case mamaMsgStatus.MAMA_MSG_STATUS_NOT_PERMISSIONED:    return MamdaErrorCode.MAMDA_ERROR_NOT_PERMISSIONED;
                case mamaMsgStatus.MAMA_MSG_STATUS_TOPIC_CHANGE:        return MamdaErrorCode.MAMDA_ERROR_TOPIC_CHANGE;
                case mamaMsgStatus.MAMA_MSG_STATUS_BANDWIDTH_EXCEEDED:  return MamdaErrorCode.MAMDA_ERROR_BANDWIDTH_EXCEEDED;
                default:
                    Debug.Assert(false, String.Format("mamaMsgStatus {0} not mapped to an MamdaErrorCode", wombatStatus));
                    return MamdaErrorCode.MAMDA_ERROR_PLATFORM_STATUS;
            }
        }
    }
}
