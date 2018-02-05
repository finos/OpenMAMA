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

#ifndef MAMA_STATUS_CPP_H__
#define MAMA_STATUS_CPP_H__

#include "mama/mamacpp.h"

namespace Wombat 
{

    class MAMACPPExpDLL MamaStatus
    {
    public:
        virtual ~MamaStatus (void) {}

        MamaStatus (mama_status status) 
            : mStatus (status) 
        {}

        virtual const char* toString (void) const;

        mama_status getStatus () const 
        { 
            return mStatus; 
        }

        inline bool operator ==(mama_status const rhs_i)
        {
           return mStatus == rhs_i;
        }

        inline bool operator ==(MamaStatus const rhs_i)
        {
           return mStatus == rhs_i.getStatus();
        }
        
        
        inline bool operator !=(mama_status const rhs_i)
        {
           return !(mStatus == rhs_i);
        }

        inline bool operator !=(MamaStatus const rhs_i)
        {
           return !(mStatus == rhs_i.getStatus());
        }

        inline bool operator <(mama_status const rhs_i)
        {
           return mStatus < rhs_i;
        }

        inline bool operator <(MamaStatus const rhs_i)
        {
           return mStatus < rhs_i.getStatus();
        }

        inline bool operator >(mama_status const rhs_i)
        {
           return mStatus > rhs_i;
        }

        inline bool operator >(MamaStatus const rhs_i)
        {
           return mStatus > rhs_i.getStatus();
        }

        inline bool operator <=(mama_status const rhs_i)
        {
           return !(mStatus > rhs_i);
        }

        inline bool operator <=(MamaStatus const rhs_i)
        {
           return !(mStatus > rhs_i.getStatus());
        }

        inline bool operator >=(mama_status const rhs_i)
        {
           return !(mStatus < rhs_i);
        }

        inline bool operator >=(MamaStatus const rhs_i)
        {
           return !(mStatus < rhs_i.getStatus());
        }

    private:
        mama_status mStatus;
    };

} // namespace Wombat
#endif // MAMA_STATUS_CPP_H__
