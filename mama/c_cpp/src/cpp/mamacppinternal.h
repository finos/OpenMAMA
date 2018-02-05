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

/* Shorthand error handling. Either throw an exception or return the status. In
 * some cases we may return a status other than MAMA_STATUS_OK if it does not
 * warrant an exception.
 */

#ifndef MAMA_CPP_INTERNAL_H__
#define MAMA_CPP_INTERNAL_H__
#include <iostream>
#include "mama/MamaStatus.h"

namespace Wombat
{

inline mama_status 
mamaTryIgnoreNotFound( mama_status status )
{
    if (( status == MAMA_STATUS_NOT_FOUND )  ||
        ( status == MAMA_STATUS_NOT_IMPLEMENTED ))
    {
        return status;
    }
    
    if( status != MAMA_STATUS_OK )
    {
        throw MamaStatus( status );
    }
    return MAMA_STATUS_OK;
}

inline mama_status
mamaTry( mama_status status )
{
    if( status != MAMA_STATUS_OK )
    {
        throw MamaStatus( status );
    }
    return MAMA_STATUS_OK;
}

} // namespace Wombat

#endif // MAMA_CPP_INTERNAL_H__
