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

#ifndef MAMA_STATS_COLLECTOR_IMPL_H__
#define MAMA_STATS_COLLECTOR_IMPL_H__

#include <mama/MamaStatsCollector.h>

namespace Wombat
{

    class MamaStatsCollectorImpl
    {
    public:
        MamaStatsCollectorImpl (void)
            : mParent          (NULL)
            , mStatsCollector  (0)
        {
        };

        MamaStatsCollectorImpl (MamaStatsCollector* statsCollector)
            : mParent          (statsCollector)
            , mStatsCollector  (0)
        {
        };

        virtual ~MamaStatsCollectorImpl (void) 
        { 
            destroy (); 
        }

        virtual void create          (mamaStatsCollectorType type, 
                                      const char*            name,
                                      const char*            middleware);

        virtual void incrementStat   (mama_fid_t             identifier);

        virtual void setName         (const char*            name);

        virtual void setLog          (int                    log);

        virtual void setPublish      (int                    publish);

        virtual void destroy         ();

        MamaStatsCollector*          mParent;


        mamaStatsCollector         mStatsCollector;
    };

} //namespace Wombat

#endif //MAMA_STATS_COLLECTOR_IMPL_H__
