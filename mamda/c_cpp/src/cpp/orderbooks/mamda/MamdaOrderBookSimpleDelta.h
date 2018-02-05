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

#ifndef MamdaOrderBookSimpleDeltaH
#define MamdaOrderBookSimpleDeltaH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaBasicEvent.h>
#include <mamda/MamdaOrderBookBasicDelta.h>

namespace Wombat
{

    /**
     * MamdaOrderBookSimpleDelta is a class that saves information about a
     * simple order book delta.  A simple delta is one that affects a
     * single order book entry.
     */

    class MAMDAOPTExpDLL MamdaOrderBookSimpleDelta : public MamdaOrderBookBasicDelta
                                    , public MamdaBasicEvent
    {
    public:
        MamdaOrderBookSimpleDelta (const MamdaOrderBookSimpleDelta&  copy);
        
        MamdaOrderBookSimpleDelta () {}
        virtual ~MamdaOrderBookSimpleDelta () {}
    };


} // namespace

#endif // MamdaOrderBookSimpleDeltaH
