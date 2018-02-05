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

#ifndef MamdaPubStatusH
#define MamdaPubStatusH

#include <mamda/MamdaConfig.h>

namespace Wombat
{

    /**
     * MamdaPubStatus is an interface that provides access to the 
     * Security Status fields such as symbol announce messages.  
     */

    class MAMDAExpDLL MamdaPubStatus 
    {
    public:
      virtual const char*    getSymbol()             const = 0;
      virtual const char*    getActivityTime()       const = 0;
      virtual const char*    getActivityDate()       const = 0;
      virtual const char*    getPubId()              const = 0;
      virtual const char*    getPubClass()           const = 0;
      virtual const char*    getPubHostName()        const = 0;
      virtual const char*    getStatusMsgType()      const = 0;
      virtual const char*    getPubFtMode()          const = 0;
      virtual const char*    getPubMhMode()          const = 0;
      virtual const char*    getLineStatus()         const = 0;
      virtual long           getStatusSeqNum()       const = 0;
      virtual long           getBeginGapSeqNum()     const = 0;
      virtual long           getEndGapSeqNum()       const = 0;
      virtual long           getPubCacheSize()       const = 0;
      virtual const char*    getFreeText()           const = 0;
      
      virtual ~MamdaPubStatus() {};
    };

} // namespace

#endif // MamdaPubStatusH
