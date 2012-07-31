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

#ifndef MamdaDataExceptionH
#define MamdaDataExceptionH

#include <mamda/MamdaConfig.h>
#include <stdexcept>
#include <string>

using std::string;
using std::invalid_argument;

namespace Wombat
{

    /**
     * MAMDA data exceptions.
     */
    class MAMDAExpDLL MamdaDataException : public invalid_argument
    {
    public:
        /**
         * Constructs a new exception with the specified detail message.
         */
        MamdaDataException (const string& message)
            : invalid_argument (message)
        {
        }

    };

} // namespace

#endif // MamdaDataExceptionH
