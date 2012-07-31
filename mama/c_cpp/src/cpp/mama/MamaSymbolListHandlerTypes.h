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

#ifndef MAMA_SYMBOL_LIST_HANDLER_TYPES_CPP_H__
#define MAMA_SYMBOL_LIST_HANDLER_TYPES_CPP_H__

#include <mama/mamacpp.h>

namespace Wombat 
{
    class MamaSymbolList;


    /**
     * The MamaSymbolListIteratorHandler interface.  Instances of
     * subclasses of this class can be passed to the
     * MamaSymbolList::iterate() method and the onMember() interface will
     * be invoked for each member in the list.
     */
    class MAMACPPExpDLL MamaSymbolListIteratorHandler
    {
    public:
        virtual ~MamaSymbolListIteratorHandler () {};

        /**
         * Method invoked for every member in the symbol list.
         *
         * @param symbolList      The symbol list.
         * @param member          The member of the symbol list.
         * @param iterateClosure  The closure argument to MamaSymbolList::iterate().
         */
        virtual void onMember (MamaSymbolList&        symbolList,
                               MamaSymbolListMember&  member,
                               void*                  iterateClosure) = 0;

        /**
         * Method invoked after all symbol list members have been iterated over.
         *
         * @param symbolList      The symbol list.
         * @param iterateClosure  The closure argument to MamaSymbolList::iterate().
         */
        virtual void onComplete (MamaSymbolList&        symbolList,
                                 void*                  iterateClosure) {}
    };


    /**
     * The MamaSymbolListMembershipHandler interface.  Instances of
     * subclasses of this class can be registered with a MamaSymbolList in
     * order to be notified of symbol add/remove events to the symbol
     * list.
     */
    class MamaSymbolListMembershipHandler
    {
    public:
        virtual ~MamaSymbolListMembershipHandler () {};

        /**
         * Method invoked when a symbol has been added to the list.
         * Invocation of this function is conditional and is subject to a
         * positive return (true) from the prior invocation of
         * <code>onDeclareInterest</code> where the registered object has
         * the opportunity to declare their interest in subsequent events
         * on the given symbol.
         *
         * @param symbolList  The symbolList.
         * @param member      The member just added to the list.
         */
        virtual void onMemberAdd (MamaSymbolList&        symbolList,
                                  MamaSymbolListMember&  member) = 0;

        /**
         * Method invoked immediately prior to a symbol being removed from
         * the list. Invocation of this function is conditional and is
         * subject to a positive return (true) from the prior invocation
         * of <code>onDeclareInterest</code> where the registered object
         * has the opportunity to declare their interest in subsequent
         * events on the given symbol.
         *
         * @param symbolList  The symbolList.
         * @param member      The member just removed from the list.
         */
        virtual void onMemberRemove (MamaSymbolList&        symbolList,
                                     MamaSymbolListMember&  member) = 0;
        
        /**
         * Method invoked when the sorting of the symbol list has changed.
         *
         * @param symbolList   The symbol list.
         */
        virtual void onOrderChange (MamaSymbolList&  symbolList) = 0;
    };


    /**
     * The MamaSymbolListResortHandler interface.  Instances of
     * subclasses of this class can be registered with a MamaSymbolList
     * and will be invoked when the sorting of the symbol list has changed.
     */
    class MamaSymbolListResortHandler
    {
    public:
        virtual ~MamaSymbolListResortHandler () {};

        /**
         * Method invoked when the sorting of the symbol list has changed.
         *
         * @param symbolList   The symbol list.
         */
        virtual void onResort (MamaSymbolList*  symbolList) = 0;
    };

} // namespace Wombat
#endif // MAMA_SYMBOL_LIST_HANDLER_CPP_H__
