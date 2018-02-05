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

#ifndef MamdaSecStatusSymbolSourceAdapterH
#define MamdaSecStatusSymbolSourceAdapterH

#include <mamda/MamdaConfig.h>
#include <mamda/MamdaSymbolSourceEvent.h>
#include <mamda/MamdaSecStatusHandler.h>

namespace Wombat
{

    class MamdaSymbolSourceHandler;
    class MamdaSecStatusHandler;

    /**
     * MamdaSecStatusSymbolSourceAdapter is a simple adapter class that
     * can be added as a handler to the MamdaSecStatusListener 
     * turning it into a MamdaSymbolSource. Objects implementing the
     * MamdaSymbolSource interface can register with this adapter and
     * receive notification of newly announced symbols indirectly from the
     * MamdaSecStatusListener via this adapter.
     */
    class MAMDAExpDLL MamdaSecStatusSymbolSourceAdapter 
        : public MamdaSymbolSourceEvent
        , public MamdaSecStatusHandler
    { 
    public:
        MamdaSecStatusSymbolSourceAdapter ();
        virtual ~MamdaSecStatusSymbolSourceAdapter ();

        /**
         * Register a handler implementing the MamdaSymbolSourceHandler interface. 
         *   
         * @param handler  The symbol source handler
         */
        void addHandler (MamdaSymbolSourceHandler* handler);

        /**
         * Return the sourced symbol.
         * Implementation of the MamdaSymbolSourceEvent interface
         * 
         * @return sourced symbol 
         */ 
        virtual const char*  getSourcedSymbol() const; 

        /**
         * Implementation of MamdaSecStatusHandler interface.
         *
         * @param subscription  The security status subscription.
         * @param listener      The security status listener.
         * @param msg           The security status update message.
         */
        virtual  void onSecStatusRecap  (MamdaSubscription*       subscription,
                                         MamdaSecStatusListener&  listener,
                                         const MamaMsg&           msg,
                                         MamdaSecStatusRecap&     recap);

        virtual  void onSecStatusUpdate (MamdaSubscription*       subscription,
                                         MamdaSecStatusListener&  listener,
                                         const MamaMsg&            msg);

        struct MamdaSecStatusSymbolSourceAdapterImpl;

    private:
        MamdaSecStatusSymbolSourceAdapterImpl& mImpl;
    };

}

#endif // MamdaSecStatusSymbolSourceAdapterH
