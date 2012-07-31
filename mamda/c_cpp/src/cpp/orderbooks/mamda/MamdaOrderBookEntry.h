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

#ifndef MamdaOrderBookEntryH
#define MamdaOrderBookEntryH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaOrderBookTypes.h>
#include <mamda/MamdaOrderBookPriceLevel.h>
#include <mamda/MamdaOrderBookExceptions.h>
#include <mama/MamaSource.h>
#include <mama/MamaSourceDerivative.h>
#include <mama/mamacpp.h>

namespace Wombat
{

    class MamdaOrderBook;
    class MamdaOrderBookPriceLevel;
    class MamdaOrderBookEntryManager;

    /**
     * MamdaOrderBookEntry is a class that represents an entry within a
     * price level of an order book.
     *
     * In addition to being referenced in a MamdaOrderBookPriceLevel, an order
     * book entry may also be stored in a MamdaOrderBookEntryManager.
     */

    class MAMDAOPTExpDLL MamdaOrderBookEntry
    {
    public:
        /**
         * An enumeration for book entry actions.  Price level actions
         * differ from entry actions because, for example, a price level
         * message with ACTION_UPDATE may consist of entries with
         * ACTION_ADD, ACTION_UPDATE or ACTION_DELETE.
         */
        enum Action
        {
         MAMDA_BOOK_ACTION_ADD     = MamdaOrderBookTypes::MAMDA_BOOK_ACTION_ADD,
         MAMDA_BOOK_ACTION_UPDATE  = MamdaOrderBookTypes::MAMDA_BOOK_ACTION_UPDATE,
         MAMDA_BOOK_ACTION_DELETE  = MamdaOrderBookTypes::MAMDA_BOOK_ACTION_DELETE,
         MAMDA_BOOK_ACTION_UNKNOWN = MamdaOrderBookTypes::MAMDA_BOOK_ACTION_UNKNOWN
        };

        MamdaOrderBookEntry ();

        /**
         * Copy constructor.  Note that the associated price level of the
         * original copy is not copied.
         */
        MamdaOrderBookEntry (const MamdaOrderBookEntry&  copy);

        /**
         * Constructor that takes the basic information for the entry ID,
         * size, action and time.
         */
        MamdaOrderBookEntry (const char*                  entryId,
                             mama_quantity_t              size,
                             Action                       action,
                             const MamaDateTime&          entryTime,
                             const MamaSourceDerivative*  deriv);

        ~MamdaOrderBookEntry ();

        /**
         * Assignment operator.  Note that the associated price level of
         * the original copy is not copied.
         */
        MamdaOrderBookEntry& operator= (const MamdaOrderBookEntry&  rhs);

        void clear         ();

        /**
         * Copy an order book entry.  Note that the associated price level
         * of the original copy is not copied.
         */
        void copy          (const MamdaOrderBookEntry&  copy);

        void setId         (const char*          id);
        void setUniqueId   (const char*          id);
        void setSize       (mama_quantity_t      size);
        void setAction     (Action               action);
        void setReason     (MamdaOrderBookTypes::Reason reason);
        void setTime       (const MamaDateTime&  time);
        void setStatus     (mama_u16_t           status);
        void setDetails    (const MamdaOrderBookEntry&  copy);

        /**
         * If supported, Order book entry ID (order ID, participant ID,
         * etc.)
         *
         * @return The entry id
         */
        const char*          getId () const;

        /**
         * If supported, Order book entry unique ID (order ID, participant ID,
         * etc.).  The unique ID should be unique throughout the order book.  If
         * no explicit unique ID has been set, then it assumed that the basic ID
         * is unique and that is returned.
         *
         * @return The unique entry id
         */
        const char*          getUniqueId () const;

        /**
         * The size of the order entry.
         *
         * @return The size of the order entry.
         */
        mama_quantity_t      getSize ()   const;

        /**
         * Whether to ADD, UPDATE or DELETE the entry.
         *
         * @return The order entry action.
         */
        Action               getAction () const;

        /**
         * Reason for a change.
         *
         * @return The order entry reason
         */ 
        MamdaOrderBookTypes::Reason getReason () const;

        /**
         * Time of order book entry update.
         *
         * @return The time of the order entry update.
         */
        const MamaDateTime&  getTime () const;

        /**
         * Get the status.
         *
         * @return the status.
         */
        mama_u16_t getStatus () const;

        /**
         * Get the price for this entry.  This method will throw a
         * MamdaOrderBookInvalidEntry if no MamdaPriceLevel is associated
         * with it because order book price information is only stored in
         * MamdaOrderBookPriceLevel objects.
         *
         * @return  The price for this entry.
         */
        double               getPrice     () const;
        MamaPrice            getMamaPrice () const;
        /**
         * Get the price for this entry.  This method will throw a
         * MamdaOrderBookInvalidEntry if no MamdaPriceLevel is associated
         * with it because order book price information is only stored in
         * MamdaOrderBookPriceLevel objects.
         *
         * @return  The side for this entry.
         */
        MamdaOrderBookPriceLevel::Side  getSide () const;

        /**
         * Get the position in the order book for this entry.  If maxPos
         * is not zero, then the method will return a result no greater
         * than maxPos.  This is to prevent searching the entire book when
         * only a limited search is necessary.  Note: the logic used in
         * the positional search is to use the number of entries that
         * MamdaOrderBookPriceLevel::getNumEntries() returns for price
         * levels above the entry's price level.  -1 is return if the
         * entry is in the book but not currently "visible" (i.e., it is
         * being omitted because the MAMA source is turned off).  A
         * MamdaOrderBookInvalidEntry is thrown if the entry is not found
         * in the book.
         *
         * @param maxPos  The maximum position to return;
         *
         * @return  The position of this entry in the order book.
         */
        mama_u32_t  getPosition (mama_u32_t  maxPos = 0) const;

        /**
         * Whether two participant ids are equal.
         *
         * @return true if the two id's are equal.
         */
        bool equalId (const char*  id) const;

        /**
         * Equality operator.  Two order book entries are equal if their
         * members are identical.
         *
         * @param rhs The order book entry to compare this entry to.
         *
         * @return Whether the two entries are equal.
         */
        bool operator== (const MamdaOrderBookEntry& rhs) const;

        /**
         * Non-equality operator.  Two order book entries are equal if
         * their members are identical.
         *
         * @param rhs The order book entry to compare this entry to.
         *
         * @return Whether the two entries are not equal.
         */
        bool operator!= (const MamdaOrderBookEntry&  rhs) const
        { return ! operator== (rhs); }

        /**
         * Set the MamdaOrderBookPriceLevel object to which this entry belongs.
         * This method is invoked automatically internally, by the MAMDA API, when
         * an entry is added to a price level.
         *
         * @param level  The price level to be associated with.
         */
        void setPriceLevel (MamdaOrderBookPriceLevel*  level);

        /**
         * Get the MamdaOrderBookPriceLevel object to which this entry
         * belongs.
         *
         * @return  The price level currently associated with this entry.
         */
        MamdaOrderBookPriceLevel* getPriceLevel () const;

        /**
         * Get the order book for this entry, if possible.  This can only
         * be done if the entry is part of a price level and the price
         * level is part of an order book.  NULL is returned if no order
         * book can be found.
         *
         * @return  The order book or NULL.
         */
        MamdaOrderBook* getOrderBook () const;

        /**
         * Set the MamdaOrderBookEntryManager object to which this entry belongs.
         * This method is invoked automatically internally, by the MAMDA API, when
         * an entry is added to an entry manager.
         *
         * @param manager pointer to the MamdaOrderBookEntryManager
         */
        void setManager(MamdaOrderBookEntryManager* manager);

        /**
         * Get the MamdaOrderBookEntryManager object to which this entry
         * belongs.
         *
         * @return  The manager currently associated with this entry.
         */
        MamdaOrderBookEntryManager* getManager () const;

        /**
         * Get the symbol for this entry, if possible.  This can only be
         * done if the entry is part of a price level and the price level
         * is part of an order book.  NULL is returned if no symbol can be
         * found.
         *
         * @return  The symbol or NULL.
         */
        const char* getSymbol () const;

        /**
         * Set the MamaSourceDerivative for this book entry.  The source
         * derivative is used to help determine what the quality of order book
         * entry is and to efficiently identify all of the entries for a given
         * source (e.g. for aggregated order books).
         *
         * @param deriv The MAMA source derivative
         */
        void setSourceDerivative (const MamaSourceDerivative* deriv);

        /**
         * Return the MamaSourceDerivative for this book entry.
         */
        const MamaSourceDerivative*  getSourceDerivative () const;

        /**
         * Return the MamaSource for this book entry.
         */
        const MamaSource*  getSource () const;

        /**
         * Get the source state.
         *
         * @return  The current MAMA source state.
         */
        mamaSourceState getSourceState () const;

        /**
         * Get whether this order book wants to check the source state.
         *
         * @return  Whether to check source state.
         */
        bool getCheckSourceState () const;

        /**
         * Set the entry-level quality factor.  This level, if not
         * MAMA_QUALITY_OK, overrides the source-level level.
         *
         * @param quality  The new entry-level MAMA quality level.
         */
        void setQuality (mamaQuality  quality);

        /**
         * Get the entry-level quality factor.  If the entry-level quality is
         * MAMA_QUALITY_OK, then this method returns the source-level quality.
         *
         * @return  The current MAMA quality level.
         */
        mamaQuality getQuality () const;

        /**
         * Get whether this entry is "visible" in this book. Visibility is
         * controlled by the status of the MamaSourceDerivative for the entry.
         *
         * @return  Whether the entry is visible.
         */
        bool isVisible () const;

        /**
         * Set the order book entry closure handle.
         *
         * @param closure The closure.
         */
        void setClosure (void*  closure);

        /**
         * Get the order book entry closure handle.
         * 
         * @return The entry closure.
         */
        void*  getClosure () const;

        /**
         * Order book entry equality verification.  A
         * MamdaOrderBookException is thrown if the entries within a price
         * level are not equal, along with the reason for the inequality.
         *
         * @param rhs The entry which this entry is being compared to.
         *
         * @throw <MamdaOrderBookException> Exception thrown if any errors
         * encountered during book processing.
         */
        void assertEqual (const MamdaOrderBookEntry&  rhs) const;

        /**
         * Enforce strict checking of order book modifications (at the
         * expense of some performance).  This setting is automatically
         * updated by MamdaOrderBook::setStrictChecking().
         *
         * @param strict Whether strict checking should be employed.
         */
        static void setStrictChecking (bool strict);

    private:
        char*                        mId;
        char*                        mUniqueId;
        mama_quantity_t              mSize;
        MamaDateTime                 mTime;
        MamdaOrderBookPriceLevel*    mPriceLevel;
        MamdaOrderBookEntryManager*  mManager;
        const MamaSourceDerivative*  mSourceDeriv;
        void*                        mClosure;
        mamaQuality                  mQuality;
        Action                       mAction;
        mama_u16_t                   mStatus;

        MamdaOrderBookTypes::Reason  mReason;
    };

} // namespace

#endif // MamdaOrderBookEntryH
