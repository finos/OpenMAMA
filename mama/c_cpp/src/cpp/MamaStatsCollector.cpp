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

#include <stdint.h>
#include "MamaStatsCollectorImpl.h"
#include "mamacppinternal.h"

namespace Wombat
{
    MamaStatsCollector::~MamaStatsCollector (void)
    {
        if (mSimpl)
        {
            delete mSimpl;
            mSimpl = NULL;
        }
    }

    MamaStatsCollector::MamaStatsCollector (MamaStatsCollectorImpl* impl)
        : mSimpl (impl)
    {
    }

    MamaStatsCollector::MamaStatsCollector (void)
        : mSimpl (new MamaStatsCollectorImpl (this))
    {
    }

    void MamaStatsCollector::create (mamaStatsCollectorType type,
                                     const char*            name,
                                     const char*            middleware)
    {
        mSimpl->create (type, name, middleware);
    }

    void MamaStatsCollector::incrementStat (mama_fid_t   identifier)
    {
        mSimpl->incrementStat (identifier);
    }

    void MamaStatsCollector::setName (const char* name)
    {
        mSimpl->setName (name);
    }

    void MamaStatsCollector::setLog (int log)
    {
        mSimpl->setLog (log);
    }

    void MamaStatsCollector::setPublish (int publish)
    {
        mSimpl->setPublish (publish);
    }

    void MamaStatsCollector::destroy ()
    {
        mSimpl->destroy ();
    }

    mamaStatsCollector MamaStatsCollector::getStatsCollector ()
    {
        return (mSimpl->mStatsCollector);
    }

    void MamaStatsCollectorImpl::create (mamaStatsCollectorType   type,
                                         const char*              name,
                                         const char*              middleware)
    {
        mamaTry (mamaStatsCollector_create (&mStatsCollector,
                                            type,
                                            name,
                                            middleware));
    }

    void MamaStatsCollectorImpl::incrementStat (mama_fid_t identifier)
    {
        mamaTry (mamaStatsCollector_incrementStat (mStatsCollector,
                                                   identifier));
    }

    void MamaStatsCollectorImpl::setName (const char* name)
    {
        mamaTry (mamaStatsCollector_setName (mStatsCollector,
                                             name));
    }

    void MamaStatsCollectorImpl::setLog (int log)
    {
        mamaTry (mamaStatsCollector_setLog (mStatsCollector,
                                             log));
    }

    void MamaStatsCollectorImpl::setPublish (int publish)
    {
        mamaTry (mamaStatsCollector_setPublish (mStatsCollector,
                                                publish));
    }

    void MamaStatsCollectorImpl::destroy (void)
    {
        if (mStatsCollector)
        {
            mamaTry (mamaStatsCollector_destroy (mStatsCollector));
            mStatsCollector = NULL;
        }
    }

} //namespace Wombat
