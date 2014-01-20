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

#ifndef MamdaErrorListenerH
#define MamdaErrorListenerH

#include <mamda/MamdaConfig.h>

namespace Wombat
{

    class MamdaSubscription;
    class MamdaBasicSubscription;

    /**
     * MAMDA error severities are intended to provide a hint to the
     * application as to the severity of an error.  In relation to a
     * MamdaSubscription, a MAMDA_SEVERITY_HIGH means that the
     * subscription has been deactivated and a MAMDA_SEVERITY_LOW means
     * that the condition may be temporary. 
     */
    enum MamdaErrorSeverity
    {
        MAMDA_SEVERITY_OK,
        MAMDA_SEVERITY_LOW,
        MAMDA_SEVERITY_HIGH
    };


    /**
     * MAMDA error codes are currently a subset of the MAMA MsgStatus
     * codes.
     */
    enum MamdaErrorCode
    {
        MAMDA_ERROR_NO_ERROR                   = 0,
        MAMDA_ERROR_LINE_DOWN                  = 1,
        MAMDA_ERROR_NO_SUBSCRIBERS             = 2,
        MAMDA_ERROR_BAD_SYMBOL                 = 3,
        MAMDA_ERROR_EXPIRED                    = 4,
        MAMDA_ERROR_TIME_OUT                   = 5,
        MAMDA_ERROR_MISC                       = 6,
        MAMDA_ERROR_STALE                      = 7,
        MAMDA_ERROR_PLATFORM_STATUS            = 8,
        MAMDA_ERROR_ENTITLEMENT                = 9,
        MAMDA_ERROR_NOT_FOUND                  = 10,
        MAMDA_ERROR_POSSIBLY_STALE             = 11,
        MAMDA_ERROR_NOT_PERMISSIONED           = 12,
        MAMDA_ERROR_TOPIC_CHANGE               = 13,
        MAMDA_ERROR_BANDWIDTH_EXCEEDED         = 14,
        MAMDA_ERROR_DELETE                     = 17,
        MAMDA_ERROR_EXCEPTION                  = 999
    };

    /**
     * MamdaErrorListener defines an interface for handling error
     * notifications for a MamdaSubscription.
     */
    class MAMDAExpDLL MamdaErrorListener
    {
    public:
        /**
         * Provide a callback to handle errors.  The severity is intended
         * as a hint to indicate whether the error is recoverable.
         */
        virtual void onError (MamdaSubscription*  subscription,
                              MamdaErrorSeverity  severity,
                              MamdaErrorCode      errorCode,
                              const char*         errorStr) = 0;
                              
        virtual ~MamdaErrorListener() {};
        
    };

    /**
     * MamdaBasicErrorListener defines an interface for handling error
     * notifications for a MamdaBasicSubscription.
     */
    class MAMDAExpDLL MamdaBasicErrorListener
    {
    public:
        /**
         * Provide a callback to handle errors.  The severity is intended
         * as a hint to indicate whether the error is recoverable.
         */
        virtual void onError (MamdaBasicSubscription*  subscription,
                              MamdaErrorSeverity       severity,
                              MamdaErrorCode           errorCode,
                              const char*              errorStr) = 0;
        
        virtual ~MamdaBasicErrorListener() {};                        
    };

} // namespace

#endif // MamdaErrorListenerH
