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

#include "MamaStatImpl.h"
#include "mamacppinternal.h"

namespace Wombat
{

    MamaStat::~MamaStat (void) 
    {
        if (mSimpl)
        {
            delete mSimpl;
            mSimpl = NULL;
        }
    }

    MamaStat::MamaStat (MamaStatImpl* impl)
        : mSimpl (impl) 
    {
    }

    MamaStat::MamaStat (void) 
        : mSimpl (new MamaStatImpl (this))
    {
    }

    void MamaStat::create (MamaStatsCollector*  statsCollector,
                           int                  lockable,
                           const char*          name,
                           mama_fid_t           fid)
    {
        mSimpl->create (statsCollector, lockable, name, fid);
    }

    void MamaStat::increment ()
    {
        mSimpl->increment ();
    }

    void MamaStat::decrement ()
    {
        mSimpl->decrement ();
    }

    void MamaStat::reset ()
    {
        mSimpl->reset ();
    }

    void MamaStat::setLog (int log)
    {
        mSimpl->setLog (log);
    }

    void MamaStat::setPublish (int publish)
    {
        mSimpl->setPublish (publish);
    }

    void MamaStat::destroy ()
    {
        mSimpl->destroy();
    }

    void MamaStatImpl::create (MamaStatsCollector* statsCollector,
                               int                 lockable,
                               const char*         name,
                               mama_fid_t          fid)
    {
        if (mStat)
        {
            throw MamaStatus (MAMA_STATUS_INVALID_ARG);
        }

        mamaTry (mamaStat_create (&mStat,
                                  statsCollector->getStatsCollector(),
                                  lockable,
                                  name,
                                  fid)); 

    }

    void MamaStatImpl::increment ()
    {
        mamaTry (mamaStat_increment (mStat));
    }

    void MamaStatImpl::decrement ()
    {
        mamaTry (mamaStat_decrement (mStat));
    }

    void MamaStatImpl::reset ()
    {
        mamaTry (mamaStat_reset (mStat));
    }

    void MamaStatImpl::setLog (int log)
    {
        mamaTry (mamaStat_setLog (mStat, log));
    }

    void MamaStatImpl::setPublish (int publish)
    {
        mamaTry (mamaStat_setPublish (mStat, publish));
    }

    void MamaStatImpl::destroy ()
    {
        if (mStat)
        {
            mamaTry (mamaStat_destroy (mStat));
            mStat = NULL;
        }
    }

} //namespace Wombat


