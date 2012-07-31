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

#ifndef MamaSourceGroup_CPP_H__
#define MamaSourceGroup_CPP_H__

#include <mama/types.h>
#include <mama/MamaSourceStateChangeCallback.h>

namespace Wombat 
{

    class MamaSource;
    class MamaSourceStateChangeCallback;

    /**
     * A MAMA source group monitors a set of MAMA sources that presumably
     * provide a duplicate set of data.  Each member of the group is given
     * a priority and the member with the highest priority is given the
     * mamaSourceState, MAMA_SOURCE_STATE_OK; the other members of the
     * group are given the state, MAMA_SOURCE_STATE_OFF.
     */

    class MAMACPPExpDLL MamaSourceGroup
    {
    public:
        MamaSourceGroup               (const char*  name);
        ~MamaSourceGroup              ();

        const char*        getName    () const;

        MamaSource*        find       (const char*  sourceName);
        const MamaSource*  find       (const char*  sourceName) const;

        void               add        (MamaSource*  source,
                                       mama_u32_t   weight);
        void               add        (const char*  sourceName,
                                       mama_u32_t   weight,
                                       MamaSource*  source);

        void               setWeight  (const char*  sourceName,
                                       mama_u32_t   weight);
        mama_u32_t         getWeight  (const char*  sourceName) const;

        mama_size_t        size       () const;
        
        /**
        * Applications interested in event notifications can register
        * for events.
        * @param cb callback to register
        */
        void               registerStateChangeCallback   (MamaSourceStateChangeCallback& cb);
        
        /**
        * Applications interested in event notifications can unregister
        * for events.
        * @param cb callback to unregister
        */
        void               unregisterStateChangeCallback (MamaSourceStateChangeCallback& cb);
        
        /**
         * Re-evaluate the group by checking all of the relative weights
         * and changing the state of each MamaSource in the group as
         * appropriate.  Returns true if any states were changed;
         * otherwise false.
         */
        bool               reevaluate ();

        class MAMACPPExpDLL iterator
        {
        protected:
            friend class      MamaSourceGroup;
            struct            iteratorImpl;
            iteratorImpl&     mImpl;

        public:
            iterator                     ();
            iterator                     (const iterator& copy);
            iterator                     (const iteratorImpl& copy);
            ~iterator                    ();

            iterator&         operator=  (const iterator& rhs);
            const iterator&   operator=  (const iterator& rhs) const;
            iterator&         operator++ ();
            const iterator&   operator++ () const;
            bool              operator== (const iterator& rhs) const;
            bool              operator!= (const iterator& rhs) const;
            MamaSource*       operator*  ();
            const MamaSource* operator*  () const;
        };

        typedef  const        iterator const_iterator;

        iterator              begin      ();
        const_iterator        begin      () const;
        iterator              end        ();
        const_iterator        end        () const;

    private:
        struct                MamaSourceGroupImpl;
        MamaSourceGroupImpl&  myImpl;

        // No copy contructor nor assignment operator.
        MamaSourceGroup                  (const MamaSourceGroup& rhs);
        MamaSourceGroup&      operator=  (const MamaSourceGroup& rhs);
    };

} // namespace Wombat


#endif // MamaSourceGroup_CPP_H__
