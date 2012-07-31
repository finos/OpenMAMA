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

#ifndef MAMA_SYMBOL_LIST_MEMBER_CPP_H__
#define MAMA_SYMBOL_LIST_MEMBER_CPP_H__

#include <mama/mamacpp.h>
#include <mama/symbollisttypes.h>


namespace Wombat 
{
    class MamaSymbolList;

    /**
     * The MamaSymbolListMember class represents the information about the
     * symbol list member. In addition to the symbols, it is also possible
     * to obtain information about the "source" of the symbol and which
     * MamaTransport it can be found on.
     */
    class MAMACPPExpDLL MamaSymbolListMember
    {
    public:
        /**
         * Construct a symbol list member that can be added to the symbol list.
         */
        MamaSymbolListMember(MamaSymbolList* symbolList);

        /**
         * Construct  symbol list member that wraps the given C symbol list
         * member.
         */
        MamaSymbolListMember( MamaSymbolList* list, mamaSymbolListMember cMember);

        virtual ~MamaSymbolListMember();

        /**
         * Get the symbol name.
         */
        const char* getSymbol () const;

        /**
         * Get the source name.
         */
        const char* getSource () const;

        /**
         * Get the MAMA transport.
         */
        MamaTransport* getTransport () const;

        /**
         * Get the MAMA symbol list to which this member belongs.
         */
        MamaSymbolList* getSymbolList () const;

        /**
         * Get the closure argument.
         */
        void* getClosure () const;

        /**
         * Set the closure argument.
         */
        void setClosure (void*  closure);

        /**
         * Set the symbol name.
         */
        void setSymbol (const char*  symbol);

        /**
         * Set the source name.
         */
        void setSource (const char*  source);

        /**
         * Set the MAMA transport.
         */
        void setTransport (MamaTransport*  transport);

        /**
         * Set the MAMA symbol list to which this member belongs.
         */
        void setSymbolList (MamaSymbolList*  symbolList);

    protected:
        friend class MamaSymbolList;
        /**
         * Get a handle to the underlying C implementation.
         */
        mamaSymbolListMember getCimpl();

    private:
        struct MamaSymbolListMemberImpl;
        MamaSymbolListMemberImpl*  myPimpl;
    };

} // namespace Wombat

#endif // MAMA_SYMBOL_LIST_MEMBER_CPP_H__
