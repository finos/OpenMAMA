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

#ifndef MamdaNewsFieldsH
#define MamdaNewsFieldsH

#include <mamda/MamdaOptionalConfig.h>
#include <mama/mamacpp.h>

namespace Wombat
{

    class MamaFieldDescriptor;
    class MamaDictionary;

    class MAMDAOPTExpDLL MamdaNewsFields
    {
    public:
        static const MamaFieldDescriptor*  HEADLINE_TEXT;
        static const MamaFieldDescriptor*  HEADLINE_ID;
        static const MamaFieldDescriptor*  STORY_TEXT;
        static const MamaFieldDescriptor*  STORY_ID;
        static const MamaFieldDescriptor*  ORIG_STORY_ID;
        static const MamaFieldDescriptor*  SOURCE_ID;
        static const MamaFieldDescriptor*  ORIG_SOURCE_ID;
        static const MamaFieldDescriptor*  LANGUAGE_ID;
        static const MamaFieldDescriptor*  NATIVE_CODES;
        static const MamaFieldDescriptor*  NATIVE_SYMBOLS;
        static const MamaFieldDescriptor*  INDUSTRIES;
        static const MamaFieldDescriptor*  MARKET_SECTORS;
        static const MamaFieldDescriptor*  REGIONS;
        static const MamaFieldDescriptor*  COUNTRIES;
        static const MamaFieldDescriptor*  PRODUCTS;
        static const MamaFieldDescriptor*  TOPICS;
        static const MamaFieldDescriptor*  MISC_CODES;
        static const MamaFieldDescriptor*  SYMBOLS;
        static const MamaFieldDescriptor*  PRIORITY;
        static const MamaFieldDescriptor*  TIME;
        static const MamaFieldDescriptor*  REVISION_NUM;
        static const MamaFieldDescriptor*  STORY_STATUS;
        static const MamaFieldDescriptor*  STORY_TIME;
        static const MamaFieldDescriptor*  ORIG_STORY_TIME;
        static const MamaFieldDescriptor*  STORY_HEADLINES;
        static const MamaFieldDescriptor*  HAS_STORY;

        // calling setDictionary will also set the Common Fields
        static void      setDictionary (const MamaDictionary&  dictionary);
        static void      reset     ();
        static uint16_t  getMaxFid ();
        static bool      isSet     ();
    };

} // namespace Wombat

#endif // MamdaNewsFieldsH
