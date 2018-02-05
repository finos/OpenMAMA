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

#ifndef MamdaCommonFieldsH
#define MamdaCommonFieldsH

#include <mamda/MamdaConfig.h>

namespace Wombat
{

    class MamaFieldDescriptor;
    class MamaDictionary;

    /**
     * Utility cache of <code>MamaFieldDescriptor</code>s which are used
     * internally by the API when accessing common fields from update messages.
     * This class should be initialized prior to using the listener objects
     * by calling <code>MamdaCommonFields.setDictionary()</code> with a
     * valid dictionary object which contains these common fields.
     */
    class MAMDAExpDLL MamdaCommonFields
    {
    public:
        static const MamaFieldDescriptor*  SYMBOL;
        static const MamaFieldDescriptor*  ISSUE_SYMBOL;
        static const MamaFieldDescriptor*  INDEX_SYMBOL;
        static const MamaFieldDescriptor*  PART_ID;
        static const MamaFieldDescriptor*  SEQ_NUM;
        static const MamaFieldDescriptor*  SRC_TIME;
        static const MamaFieldDescriptor*  LINE_TIME;
        static const MamaFieldDescriptor*  ACTIVITY_TIME;
        static const MamaFieldDescriptor*  SEND_TIME;
        static const MamaFieldDescriptor*  PUB_ID;
        static const MamaFieldDescriptor*  MSG_QUAL;
        static const MamaFieldDescriptor*  MSG_SEQ_NUM;
        static const MamaFieldDescriptor*  MSG_NUM;
        static const MamaFieldDescriptor*  MSG_TOTAL;
        static const MamaFieldDescriptor*  INITIAL_TOTAL;
        static const MamaFieldDescriptor*  SENDER_ID;

        static void setDictionary (const MamaDictionary&  dictionary);

        /**
         * Reset the dictionary for common update fields
        */
        static void reset ();
        static bool isSet ();
    };

} // namespace Wombat

#endif // MamdaCommonFieldsH
