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

#ifndef MamaSourceManager_CPP_H__
#define MamaSourceManager_CPP_H__

#include <mama/sourceman.h>
#include <mama/types.h>

namespace Wombat 
{

    class MamaSource;

    /**
     * A MAMA source manager maintains information about a set of data
     * sources, including the quality of the data coming from those sources.
     */

    class MAMACPPExpDLL MamaSourceManager
    {
    public:
        MamaSourceManager                      ();
        virtual ~MamaSourceManager             ();

        virtual MamaSource*       create       (const char*  sourceName);
        virtual MamaSource*       findOrCreate (const char*  sourceName);
        virtual MamaSource*       find         (const char*  sourceName);
        virtual const MamaSource* find         (const char*  sourceName) const;
        virtual void              add          (MamaSource*  source);
        virtual void              add          (const char*  sourceName,
                                                MamaSource*  source);
        mama_size_t               size         () const;


        class MAMACPPExpDLL iterator
        {
        protected:
            friend class          MamaSourceManager;
            struct                iteratorImpl;
            iteratorImpl&         mImpl;

        public:
            iterator                           ();
            iterator                           (const iterator& copy);
            iterator                           (const iteratorImpl& copy);
            ~iterator                          ();

            iterator&             operator=    (const iterator& rhs);
            const iterator&       operator=    (const iterator& rhs) const;
            iterator&             operator++   ();
            const iterator&       operator++   () const;
            bool                  operator==   (const iterator& rhs) const;
            bool                  operator!=   (const iterator& rhs) const;
            MamaSource*           operator*    ();
            const MamaSource*     operator*    () const;
        };

        typedef const             iterator const_iterator;

        iterator                  begin        ();
        const_iterator            begin        () const;
        iterator                  end          ();
        const_iterator            end          () const;

        mamaSourceManager         getCValue    ()       
        { 
            return myManager; 
        }

        const mamaSourceManager   getCValue    () const 
        { 
            return myManager; 
        }

    private:
        struct                    MamaSourceManagerImpl;
        MamaSourceManagerImpl&    myImpl;
        mamaSourceManager         myManager;

        // No copy constructor nor assignment operator.
        MamaSourceManager                      (const MamaSourceManager& rhs);
        MamaSourceManager&        operator=    (const MamaSourceManager& rhs);
    };

} // namespace Wombat

#endif // MamaSourceManager_CPP_H__
