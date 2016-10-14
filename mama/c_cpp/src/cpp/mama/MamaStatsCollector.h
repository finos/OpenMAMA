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

#ifndef MAMA_STATS_COLLECTOR_CPP_H__
#define MAMA_STATS_COLLECTOR_CPP_H__

#include "mama/types.h"
#include "mama/statscollector.h"

namespace Wombat
{
    class MamaStatsCollectorImpl;

    class MAMACPPExpDLL MamaStatsCollector
    {
        public:
            MamaStatsCollector (void);

            virtual ~MamaStatsCollector (void);

            virtual void create          (mamaStatsCollectorType type,
                                          const char*            name,
                                          const char*            middleware);

            virtual void incrementStat   (mama_fid_t             identifier);

            virtual void setName         (const char*            name);

            virtual void setLog          (int                    log);

            virtual void setPublish      (int                    publish);

            virtual void destroy         (void);

            virtual mamaStatsCollector  getStatsCollector       ();

        protected:
            MamaStatsCollector      (MamaStatsCollectorImpl*);

            MamaStatsCollectorImpl* mSimpl;
    };

} // namespace Wombat

#endif //MAMA_STATS_COLLECTOR_CPP_H__
