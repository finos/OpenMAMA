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

#ifndef MAMA_STAT_IMPL_CPP_H__
#define MAMA_STAT_IMPL_CPP_H__

#include <mama/MamaStat.h>

namespace Wombat
{
    class MamaStatImpl
    {
    public:
        MamaStatImpl  (void)
            : mParent (NULL)
            , mStat   (0)
        {
        };

        MamaStatImpl  (MamaStat* stat)
            : mParent (stat)
            , mStat   (0)
        {
        };

        virtual ~MamaStatImpl (void) 
        { 
            destroy (); 
        }
        virtual void create             (MamaStatsCollector*    statsCollector, 
                                         int                    lockable, 
                                         const char*            name, 
                                         mama_fid_t             fid);

        virtual void increment          ();

        virtual void decrement          ();

        virtual void reset              ();

        virtual void setLog             (int                    log);

        virtual void setPublish         (int                    publish);

        virtual void destroy            ();

    protected:
        MamaStat*    mParent;

        mamaStat     mStat;
    };

} //namespace Wombat

#endif //MAMA_STAT_IMPL_CPP_H__
