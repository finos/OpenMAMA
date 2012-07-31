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

#ifndef MAMA_SOURCE_DERIVATIVE_CPP_H__
#define MAMA_SOURCE_DERIVATIVE_CPP_H__

#include <mama/MamaSource.h>


namespace Wombat 
{

    /**
     * A MamaSourceDerivative provides a reference to a common MamaSource object
     * but can have attributes (such as the quality state) overridden.  This class
     * is intended to be associated with individually subscribed items, including
     * order books.  
     */

    class MAMACPPExpDLL MamaSourceDerivative : public MamaSource
    {
    public:
        /**
         * Construct an instance of a derived MAMA source.  The resulting source
         * derivative will have derived sub-sources for each of the sub-sources in
         * baseSource.
         */
        MamaSourceDerivative                               (const MamaSource* baseSource);
        virtual ~MamaSourceDerivative                      ();

        virtual void                         setQuality    (mamaQuality      quality);
        virtual void                         setState      (mamaSourceState  state);
        virtual mamaQuality                  getQuality    () const;
        virtual mamaSourceState              getState      () const;

        virtual       MamaSourceDerivative*  find          (const char*  sourceName);
        virtual const MamaSourceDerivative*  find          (const char*  sourceName) const;


        const MamaSource*                    getBaseSource () const 
        { 
            return myBaseSource; 
        }

    private:
        const MamaSource*  myBaseSource;
        mamaQuality        myQuality;
        mamaSourceState    myState;

        // No copy contructor nor assignment operator.
        MamaSourceDerivative             (const MamaSourceDerivative& rhs);
        MamaSourceDerivative&  operator= (const MamaSourceDerivative& rhs);
    };

} // namespace Wombat


#endif // MAMA_SOURCE_DERIVATIVE_CPP_H__
