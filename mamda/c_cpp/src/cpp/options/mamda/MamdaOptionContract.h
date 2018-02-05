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

#ifndef MamdaOptionContractH
#define MamdaOptionContractH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaOptionTypes.h>
#include <mama/mamacpp.h>
#include <vector>

namespace Wombat
{

    class MamdaTradeHandler;
    class MamdaQuoteHandler;
    class MamdaFundamentalHandler;
    class MamdaTradeRecap;
    class MamdaQuoteRecap;
    class MamdaFundamentals;
    class MamdaMsgListener;
    class MamdaTradeListener;
    class MamdaQuoteListener;
    class MamdaFundamentalListener;
    class MamaDateTime;

    /**
     * A class that represents a single option contract.  Instances of
     * this object are typically created by the MamdaOptionChainListener.
     * Applications may attach a custom object to each instance of
     * MamdaOptionContract.
     *
     * Note: User applications can be notified of creation of
     * MamdaOptionContract instances via the
     * MamdaOptionChainListener.onOptionContractCreate() method.  
     *
     * Note: It is possible to provide individual MamdaTradeHandler and
     * MamdaQuoteHandler handlers for trades and quotes, even though the
     * MamdaOptionChainHandler also provides a general callback interface
     * for updates to individual contracts.  
     */
    class MAMDAOPTExpDLL MamdaOptionContract
    {
        // No default constructor:
        MamdaOptionContract();

    public:

        /**
         * Constructor from expiration date, strike price, and put/call
         * indicator.
         *
         * @param symbol        The option symbol.
         * @param exchange      The exchange identifier.
         * @param expireDate    The options expiration date.
         * @param strikePrice   The options strike price.
         * @param putCall       Whether this is a put or call option.
         */
        MamdaOptionContract (const char*          symbol,
                             const char*          exchange,
                             const MamaDateTime&  expireDate,
                             double               strikePrice,
                             MamdaOptionPutCall   putCall);

        /**
         * Constructor without expiration date, strike price, and put/call
         * indicator.
         *
         * @param symbol    The option symbol.
         * @param exchange  The exchange identifier.
         */
        MamdaOptionContract (const char*          symbol,
                             const char*          exchange);

        /**
         * Destructor.
         */
        ~MamdaOptionContract ();

        /**
         * Set the expiration date.
         *
         * @param expireDate The options expiration date.
         */
        void setExpireDate (const MamaDateTime& expireDate);

        /**
         * Set the strike price.
         * 
         * @param strikePrice The options strike price.
         */
        void setStrikePrice (double strikePrice);

        /**
         * Set the put/call indicator.
         *
         * @param putCall Whether this is a put or call option.
         */
        void setPutCall (MamdaOptionPutCall putCall);

        /**
         * Set the open interest size.
         *
         * @param openInterest The open interest for the option.
         */
        void setOpenInterest (uint32_t  openInterest);

        /**
         * Set the exercise style.
         *
         * @param exerciseStyle The exercise style for the option.
         */
        void setExerciseStyle (MamdaOptionExerciseStyle  exerciseStyle);

        /**
         * Set the recap required field. Typically only used by the
         * <code>MamdaOptionChainListener</code>
         *
         * @param recapRequired Whether a recap is required by the contract.
         */
        void setRecapRequired(bool recapRequired);
     
        /**
         * Return the OPRA contract symbol.
         *
         * @return The OPRA contract symbol.
         */
        const char* getSymbol () const;

        /**
         * Return the exchange.
         *
         * @return The exchange identifier.
         */
        const char* getExchange () const;

        /**
         * Return the expiration date.
         *
         * @return The options expiration date.
         */
        const MamaDateTime& getExpireDate () const;

        /**
         * Return the expiration date as a string.
         *
         * @return The options expiration date as a string.
         */
        const char* getExpireDateStr () const;
     
        /**
         * Return whether or not expiration date has been set.
         *
         * @return Whether the contract has an expiration date set.
         */
        bool gotExpireDate() const;

        /**
         * Return the strike price.
         *
         * @return The option contracts strike price.
         */
        double getStrikePrice () const;
     
        /**
         * Return whether or not strike price has been set.
         *
         * @return Whether the strike price has been set for the contract.
         */
        bool gotStrikePrice() const;

        /**
         * Return the put/call indicator.
         *
         * @return Indicating whether this is a put or call option contract.
         */
        MamdaOptionPutCall getPutCall () const;
     
        /**
         * Return whether or not put call has been set.
         *
         * @return Whether the put/call indicator has been set for this contract.
         */
        bool gotPutCall() const;

        /**
         * Return the level of interest at opening.
         *
         * @return The opening interest for the option contract.
         */
        uint32_t getOpenInterest () const;
     
        /**
         * Return whether or not open interest has been set.
         *
         * @return Whether a value for opening interest has been set for the
         * option contract.
         */
        bool gotOpenInterest() const;

        /**
         * Return the style indicator - American(A)/European(E)/Capped(C).
         *
         * @return The exercise style for the option contract.
         */
        MamdaOptionExerciseStyle getExerciseStyle () const;
     
        /**
         * Return whether or not exercise style  has been set.
         *
         * @return Whether the exercise style for the option contract has been
         * set.
         */
        bool gotExerciseStyle() const;

        /**
         * Get the recap required field. Reserved for use by the
         * <code>MamdaOptionChainListener</code>.
         *
         * @return Whether a recap is required for this contract.
         */
        bool getRecapRequired() const;

        /**
         * Add a generic MamdaMsgListener to this option contract.
         *
         * @param listener A instance of the listener interface.
         */
        void addMsgListener (MamdaMsgListener* listener);

        /**
         * Add a MamdaTradeHandler for handling trade updates to this
         * option contract.
         *
         * @param handler A trade handler.
         */
        void addTradeHandler (MamdaTradeHandler* handler);

        /**
         * Add a MamdaQuoteHandler for handling quote updates to this
         * option contract.
         *
         * @param handler A quote handler.
         */
        void addQuoteHandler (MamdaQuoteHandler* handler);

        /**
         * Add a MamdaFundamentalHandler for handling fundamental data fields.
         * 
         * @param handler A fundamental handler.
         */
        void addFundamentalHandler (MamdaFundamentalHandler* handler);

        /**
         * Add a custom object to this option contract.  Such an object
         * might contain customer per-contract data.
         *
         * @param object Associate closure data with the option contract.
         */
        void setCustomObject (void* object);

        /**
         * Return the current trade fields.
         *
         * @return The current trade related fields for the option contract.
         */
        MamdaTradeRecap& getTradeInfo () const;

        /**
         * Return the current quote fields.
         *
         * @return The current quote related fields for the option contract.
         */
        MamdaQuoteRecap& getQuoteInfo () const;

        /**
         * Return the current fundamentals fields.
         *
         * @return The current fundamental fields for the option contract.
         */
        MamdaFundamentals& getFundamentalsInfo () const;

        /**
         * Return the custom object.
         *
         * @return The closure.
         */
        void* getCustomObject () const;

        /**
         * Return the vector of message listeners.
         *
         * @return Vector of message listeners registered with the object.
         */ 
        std::vector<MamdaMsgListener*>& getMsgListeners ();

        /**
         * Return the trade listener.
         *
         * @return The registered trade listener.
         */
        MamdaTradeListener& getTradeListener ();

        /**
         * Return the quote listener.
         *
         * @return The registered quote listener.
         */
        MamdaQuoteListener& getQuoteListener ();

        /**
         * Return the fundamental listener.
         *
         * @return The registered fundamental listener.
         */
        MamdaFundamentalListener& getFundamentalListener ();

        /**
         * Set whether this contract is in the "view" within the option
         * chain.  
         *
         * @param inView Set whether this contract is in the "view" within the
         * option chain.
         *
         * @see MamdaOptionChain
         */
        void setInView (bool inView);

        /**
         * Return whether this contract is in the "view" within the option
         * chain.  
         *
         * @return Whether this contract is in the "view" within the option
         * chain.
         *
         * @see MamdaOptionChain
         */
        bool getInView ();

        /*
         * MLS Temporary hack until this is moved to mama 
         */
        int64_t getSeqNum ();
        void setSeqNum (int64_t num);

    private:
        struct MamdaOptionContractImpl;
        MamdaOptionContractImpl& mImpl;
    };

} // namespace

#endif // MamdaOptionContractH
