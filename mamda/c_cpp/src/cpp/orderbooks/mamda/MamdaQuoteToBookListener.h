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

#ifndef MamdaQuoteToBookListenerH
#define MamdaQuoteToBookListenerH

#include <mamda/MamdaOptionalConfig.h>
#include <mamda/MamdaMsgListener.h>
#include <mamda/MamdaOrderBook.h>
#include <mamda/MamdaOrderBookHandler.h>
#include <mamda/MamdaFieldState.h>

using namespace Wombat;

class MamdaQuoteToBookListenerImpl;

class MamdaQuoteToBookListener : public MamdaOrderBookListener
{
public:
    /**
     * Create an order book listener using an optional user-provided
     * object for the full order book.  If "fullBook" is NULL, an
     * object will be allocated internally.  If this listener is
     * destroyed then the full order book object will only be
     * destroyed if it was created by the listener (i.e., if fullBook
     * was passed as NULL in this constructor).
     *
     * @param fullBook The MamdaOrderBook used to maintain the full book.
     */
    MamdaQuoteToBookListener (MamdaOrderBook*  fullBook = NULL);

    virtual ~MamdaQuoteToBookListener ();

    /**
     * Add a specialized order book handler.  Currently, only one
     * handler can (and must) be registered.
     *
     * @param handler The hadler registered to receive order book update
     * callbacks.
     */
    virtual void        addHandler (MamdaOrderBookHandler* handler);

    virtual void        setProcessEntries        (bool         process);
    virtual void        addIgnoreEntryId         (const char*  id);
    virtual void        removeIgnoreEntryId      (const char*  id);
    virtual void        setUseEntryManager       (bool         useManager);
    virtual void        setEntryIdsAreUnique     (bool         uniqueIds);
    virtual void        setKeepBasicDeltas       (bool         keep);
    virtual void        setConflateDeltas        (bool         conflate);
    virtual bool        getConflateDeltas        ();
    virtual void        setConflationInterval    (double       interval);
    virtual void        forceInvokeDeltaHandlers ();
    virtual void        clearConflatedDeltas     ();
    virtual void        setProcessProperties     (bool         value);
    virtual bool        getProcessProperties     () const;
    
    virtual MamdaFieldState     getSymbolFieldState()       const;
    virtual MamdaFieldState     getPartIdFieldState()       const;
    virtual MamdaFieldState     getEventSeqNumFieldState()  const;
    virtual MamdaFieldState     getEventTimeFieldState()    const;
    virtual MamdaFieldState     getSrcTimeFieldState()      const;
    virtual MamdaFieldState     getActivityTimeFieldState() const;
    virtual MamdaFieldState     getLineTimeFieldState()     const;
    virtual MamdaFieldState     getSendTimeFieldState()     const;
    virtual MamdaFieldState     getMsgQualFieldState()      const;

    /**
     * Remove the reference of handlers from the internal list.
     * Memory is not freed.
     */
    virtual void        removeHandlers    ();

    /**
     * Clear all cached data fields.
     */
    virtual void        clear             ();

    // Inherited from MamdaOrderBookGap
    virtual mama_seqnum_t        getBeginGapSeqNum () const;
    virtual mama_seqnum_t        getEndGapSeqNum   () const;
    // Inherited from MamdaOrderBookRecap
    virtual const MamdaOrderBook* getOrderBook     () const;
    // Inherited from MamdaBasicRecap and MamdaBasicEvent
    virtual const char*          getSymbol         () const;
    virtual const char*          getPartId         () const;
    virtual mama_seqnum_t        getEventSeqNum    () const;
    virtual const MamaDateTime&  getEventTime      () const;
    virtual const MamaDateTime&  getSrcTime        () const;
    virtual const MamaDateTime&  getActivityTime   () const;
    virtual const MamaDateTime&  getLineTime       () const;
    virtual const MamaDateTime&  getSendTime       () const;
    virtual const MamaMsgQual&   getMsgQual        () const;
    // quote count specific to quote consumer
    virtual mama_u32_t           getQuoteCount     () const;

    virtual void        setUpdateInconsistentBook  (bool update);
    virtual void        setUpdateStaleBook         (bool  update);
    virtual void        setClearStaleBook          (bool  clear);

    virtual void        resolvePossiblyDuplicate   (bool tf);

    /**
     * Change the order book quality and invoke appropriate callbacks.
     */
    virtual void  setQuality (MamdaSubscription*  sub,
                              mamaQuality         quality);

    void setQuoteSizeMultiplier (int multiplier);

    /**
     * Implementation of MamdaListener interface.
     */
    virtual void onMsg (MamdaSubscription*  subscription,
                        const MamaMsg&      msg,
                        short               msgType);

    virtual void acquireReadLock (); 
    virtual void releaseReadLock ();
    virtual void acquireWriteLock ();
    virtual void releaseWriteLock ();

    virtual void getBookSnapShot (MamdaOrderBook&  result);

private:
    MamdaQuoteToBookListenerImpl&  mImpl;

};

#endif // MamdaQuoteToBookListenerH

