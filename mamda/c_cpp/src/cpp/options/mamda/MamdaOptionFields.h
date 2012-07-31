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

#ifndef MamdaOptionFieldsH
#define MamdaOptionFieldsH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaFields.h>

using namespace Wombat;

namespace Wombat
{

    class MamaFieldDescriptor;
    class MamaDictionary;

    /**
     * Utility cache of <code>MamaFieldDescriptor</code>s which are used
     * internally
     * by the API when accessing options related fields from update messages.
     * This class should be initialized prior to using the
     * <code>MamdaOptionChainListener</code> by calling
     * <code>setDictionary()</code> with a
     * valid dictionary object which contains options related fields.
     */
    class MAMDAOPTExpDLL MamdaOptionFields : public MamdaFields
    {
    public:
        static const MamaFieldDescriptor*  CONTRACT_SYMBOL;
        static const MamaFieldDescriptor*  UNDERLYING_SYMBOL;
        static const MamaFieldDescriptor*  EXPIRATION_DATE;
        static const MamaFieldDescriptor*  STRIKE_PRICE;
        static const MamaFieldDescriptor*  PUT_CALL;
        static const MamaFieldDescriptor*  OPEN_INTEREST;
        static const MamaFieldDescriptor*  EXERCISE_STYLE;

        static void setDictionary (const MamaDictionary&  dictionary);
        static void reset ();
        static bool isSet ();
    };

} // namespace

#endif // MamdaOptionFieldsH
