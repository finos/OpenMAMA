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

#ifndef MamdaOptionChainListenerH
#define MamdaOptionChainListenerH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaMsgListener.h>
#include <mamda/MamdaOptionSeriesUpdate.h>
#include <mamda/MamdaFieldState.h>

namespace Wombat
{

class MamdaOptionChain;
class MamdaOptionChainHandler;

/**
 * MamdaOptionChainListener is a class that specializes in handling
 * and managing option chain updates.  Developers provide their own
 * implementation of the MamdaOptionChainHandler interface and will be
 * delivered notifications for various types of options-related events.
 */
class MAMDAOPTExpDLL MamdaOptionChainListener 
    : public MamdaMsgListener
    , public MamdaOptionSeriesUpdate
{
public:

    /**
     * Create a specialized option chain listener.  This listener
     * handles option chain updates.
     * 
     * @param underlyingSymbol The underlying symbol for the option chain.
     */
    MamdaOptionChainListener (const char*  underlyingSymbol);

    /**
     * Create a specialized option chain listener.  This listener
     * handles option chain updates.
     *
     * @param chain The option chain object.
     */
    MamdaOptionChainListener (MamdaOptionChain*  chain);

    /**
     * Destructor.
     */
    virtual ~MamdaOptionChainListener();

    /**
     * Call this function to control whether or not option contracts, 
     * which are missing expireDate, strikePrice, or putCall fields 
     * are exposed to the user app by calling the registered 
     * onOptionContractCreate(..) callbacks. Passing a value 
     * of false will allow contracts without the particular field to 
     * be exposed, passing a value of true will ensure options
     * without this field will not reach the user app.
     * By default all three fields are considered mandatory.
     *
     * @param expireDate Whether the expiration date is mandatory.
     * @param strikePrice Whether the strikePrice is mandatory.
     * @param putCall Whether the put/call indicator is mandatory.
     */
    void setManditoryFields (bool  expireDate, 
                             bool  strikePrice, 
                             bool  putCall);

    /**
     * Add a specialized option chain handler.  Currently, only one
     * handler can (and must) be registered.
     *
     * @param handler An instance of the handler which will receive callback
     * updates for activity on the option chain.
     */
    void addHandler (MamdaOptionChainHandler* handler);

    /**
     * Return the option chain associated with this listener.
     *
     * @return The option chain being maintained by this listener instance.
     */
    MamdaOptionChain& getOptionChain ();

    const char* getSymbol         () const;
    const char* getPartId         () const;

    /**
     * Implementation of MamdaListener interface.
     */
    void onMsg (MamdaSubscription*  subscription,
                const MamaMsg&      msg,
                short               msgType);

    // Inherited from MamdaBasicRecap and MamdaBasicEvent
    const MamaDateTime&   getSrcTime        () const;
    const MamaDateTime&   getActivityTime   () const;
    const MamaDateTime&   getLineTime       () const;
    const MamaDateTime&   getSendTime       () const;
    const MamaMsgQual&    getMsgQual        () const;
    const MamaDateTime&   getEventTime      () const;
    mama_seqnum_t         getEventSeqNum    () const;
    MamdaOptionContract*  getOptionContract () const;
    char                  getOptionAction   () const;
    
    MamdaFieldState       getSymbolFieldState()       const;
    MamdaFieldState       getPartIdFieldState()       const;
    MamdaFieldState       getEventSeqNumFieldState()  const;
    MamdaFieldState       getEventTimeFieldState()    const;
    MamdaFieldState       getSrcTimeFieldState()      const;
    MamdaFieldState       getActivityTimeFieldState() const;
    MamdaFieldState       getLineTimeFieldState()     const;
    MamdaFieldState       getSendTimeFieldState()     const;
    MamdaFieldState       getMsgQualFieldState()      const;

private:
    struct MamdaOptionChainListenerImpl;
    MamdaOptionChainListenerImpl& mImpl;
};


} // namespace

#endif // MamdaOptionChainListenerH
