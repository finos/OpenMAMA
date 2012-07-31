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

#ifndef MAMA_SYMBOL_LIST_CPP_H__
#define MAMA_SYMBOL_LIST_CPP_H__

#include <mama/mamacpp.h>
#include <mama/MamaSymbolListMember.h>
#include <mama/MamaSymbolListHandlerTypes.h>

namespace Wombat 
{

    /**
     * MamaSymbolList manages a list of MAMA symbols and related
     * attributes.  Methods are provided for creating, updating and
     * sorting the members of the list.  Handler interfaces are provided
     * so that it is possible to handle asynchronous/external changes to
     * the symbol list, as many types of symbol lists can be quite
     * dynamic.
     */

    class MAMACPPExpDLL MamaSymbolList
    {
    public:
     
        MamaSymbolList ();
        virtual ~MamaSymbolList ();

        /**
         * Add a "membership" handler that implements the
         * MamaSymbolListMembershipHandler interface.  Multiple handlers
         * may be registered.
         *
         * @param handler   The handler to be registered.
         */
        void addMembershipHandler (MamaSymbolListMembershipHandler*  handler);

        /**
         * Add a symbol to the list. 
         * The list maintains a unique list of symbols.
         *
         * @param member     The symbol member to be added.
         */
        void addMember (MamaSymbolListMember*  member);

        /**
         * Find a symbol in the list. 
         *
         * @param symbol      The name of the symbol to be removed.
         * @param source      The source of the symbol to be removed.
         * @param transport   The transport of the symbol to be removed.
         * @return            The object containing additional information about the
         * symbol (or NULL).
         */
        MamaSymbolListMember*  findMember (const char*            symbol,
                                           const char *           source,
                                           mamaTransport          transport);

        /**
         * Remove a symbol from the list (providing it exists in the
         * list). The member itself is not destroyed but returned as the
         * result of this method.
         *
         * @param symbol  The symbol to be removed.
         * @param source      The source of the symbol to be removed
         * @param transport   The transport of the symbol to be removed
         * @return        The member just removed (or NULL if not found)
         */
        MamaSymbolListMember* removeMember (const char*             symbol,
                                            const char*             source,
                                            mamaTransport           transport);

        /**
         * Remove a symbol from the list (providing it exists in the
         * list).  The member itself is not destroyed.
         *
         * @param member  The member to be removed.
         */
        void removeMember (MamaSymbolListMember& member);

        /**
         * Remove all symbols from the list (providing it exists in the
         * list).  The member itself is not destroyed.
         *
         */
        void removeMemberAll (void);

        /**
         * Remove all symbols from the list.  Handlers remain registered.
         */
        void clear ();
        
        /**
         * Dump the contents of the list to stdout.  For debugging.
         */
        void dump ();

        /**
         * @return whether the symbol list is empty.
         */
        bool empty () const;

        /**
         * @return the size of the symbol list.
         */
        mama_size_t size () const;

        /**
         * Set the closure.
         *
         * @param closure  The closure.
         */
        void setClosure (void* closure);

        /**
         * Get the closure.
         *
         * @return  The closure.
         */
        void* getClosure () const;

        /**
         * Iterate over all members of the symbol list.
         *
         * @param handler  Handler invoked for each member of the symbol list.
         * @param iterateClosure  The closure passed to the
         *                MamaSymbolListIteratorHandler::onMember() interface.
         */
        void iterate (MamaSymbolListIteratorHandler&  handler,
                      void*                           iterateClosure = NULL);


        /**
         * Get the underlying Impl at C level
         * @return      The mamaSymbolList
         */
        mamaSymbolList getCValue ();

        /**
         * Get the underlying Impl at C level
         * @return      The mamaSymbolList
         */
        const mamaSymbolList getCValue () const;

        struct MamaSymbolListImpl;
        MamaSymbolListImpl* myPimpl;

    protected:
        mamaSymbolList  myList;
    };

} // namespace Wombat

#endif // MAMA_SYMBOL_LIST_CPP_H__
