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

#ifndef MamdaSecStatusFieldsH
#define MamdaSecStatusFieldsH

#include <mamda/MamdaConfig.h>
#include <mama/mamacpp.h>


namespace Wombat
{

    class MamaFieldDescriptor;
    class MamaDictionary;

    /**
     * Utility cache of <code>MamaFieldDescriptor</code>s which are used
     * internally by the API when accessing security status related fields
     * from update messages.
     * This class should be initialized prior to using the
     * <code>MamdaSecurityStatusListener</code> by calling
     * <code>MamdaSecStatusListener.setDictionary()</code> with a
     * valid dictionary object which contains security status related
     * fields.
     */
    class MAMDAExpDLL MamdaSecStatusFields
    {
    public:
        // Symbol Announce
        static const MamaFieldDescriptor*  PUB_CLASS;
        static const MamaFieldDescriptor*  REASON;

        // Common to Announce and Security Status
        static const MamaFieldDescriptor*  SECURITY_ACTION;

        // Security Status
        static const MamaFieldDescriptor*  SECURITY_TYPE;
        static const MamaFieldDescriptor*  SECURITY_STATUS;
        static const MamaFieldDescriptor*  SECURITY_STATUS_QUAL;
        static const MamaFieldDescriptor*  SECURITY_STATUS_NATIVE;
        static const MamaFieldDescriptor*  SECURITY_STATUS_TIME;
        static const MamaFieldDescriptor*  EVENT_SEQ_NUM;
        static const MamaFieldDescriptor*  FREE_TEXT;
        static const MamaFieldDescriptor*  SHORT_SALE_CIRCUIT_BREAKER;

        // calling setDictionary will also set the Common Fields
        static void      setDictionary (const MamaDictionary&  dictionary);
        /**
        * Reset the dictionary for security status update fields
        */
        static void      reset     ();
        static uint16_t  getMaxFid ();
        static bool      isSet     ();
    };

}

#endif // MamdaSecStatusFieldsH
