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

#include <mama/MamaSourceDerivative.h>
#include "mamacppinternal.h"


namespace Wombat
{

    MamaSourceDerivative::MamaSourceDerivative (const MamaSource* baseSource)
        : myBaseSource (baseSource)
        , myQuality    (MAMA_QUALITY_UNKNOWN)
        , myState      (MAMA_SOURCE_STATE_UNKNOWN)
    {
        setId        (baseSource->getId ());
        setDisplayId (baseSource->getDisplayId ());

        MamaSource::iterator end = baseSource->end ();
        MamaSource::iterator i;

        for (i = baseSource->begin (); i != end; ++i)
        {
            MamaSource*            subBaseSource = *i;
            MamaSourceDerivative*  subDerivative = 
                new MamaSourceDerivative (subBaseSource);

            subDerivative->setParent (this);
            add (subBaseSource->getMappedId (), subDerivative);
        }
    }

    MamaSourceDerivative::~MamaSourceDerivative ()
    {
        // Clean up the sub-source derivatives
        MamaSource::iterator e = end ();
        MamaSource::iterator i;

        for (i = begin (); i != e; ++i)
        {
            delete *i;
        }
    }

    void MamaSourceDerivative::setQuality (mamaQuality  quality)
    {
        myQuality = quality;
    }

    void MamaSourceDerivative::setState (mamaSourceState  state)
    {
        myState = state;
    }

    mamaQuality MamaSourceDerivative::getQuality () const
    {
        if (myQuality != MAMA_QUALITY_UNKNOWN)
        {
            return myQuality;
        }
        else
        {
            return myBaseSource->getQuality ();
        }
    }

    mamaSourceState MamaSourceDerivative::getState () const
    {
        if (myState != MAMA_SOURCE_STATE_UNKNOWN)
        {
            return myState;
        }
        else
        {
            return myBaseSource->getState();
        }
    }

    MamaSourceDerivative* MamaSourceDerivative::find (
        const char*  sourceName)
    {
        return dynamic_cast<MamaSourceDerivative*>(
            MamaSourceManager::find (sourceName));
    }

    const MamaSourceDerivative* MamaSourceDerivative::find (
        const char*  sourceName) const
    {
        return dynamic_cast<const MamaSourceDerivative*>(
            MamaSourceManager::find (sourceName));
    }

} /* namespace  Wombat */
