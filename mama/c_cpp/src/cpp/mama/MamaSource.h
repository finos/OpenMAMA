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

#ifndef MAMA_SOURCE_CPP_H__
#define MAMA_SOURCE_CPP_H__

#include <mama/source.h>
#include <mama/MamaSourceManager.h>

static const int MAX_SOURCE_STR_LEN = 32;


namespace Wombat 
{
    class MamaSubscription;
    class MamaTransport;

    class MamaSubscriptionIteratorCallback
    {
    public:
        virtual void onSubscription (MamaSource*       source, 
                                     MamaSubscription* subscription, 
                                     void*             closure)
        {
            return;
        }

        virtual ~MamaSubscriptionIteratorCallback() 
        {};
    };

    /**
     * A MAMA source maintains information about a data source, including
     * the quality of the data coming from that source.  It inherits
     * MamaSourceManager because a source can have sub-sources.
     */
    class MAMACPPExpDLL MamaSource : public MamaSourceManager
    {
    public:
        MamaSource ();

        MamaSource (const char*     id,
                    const char*     transportName,
                    const char*     subscSourceName,
                    mamaBridge      bridge,
                    bool            createTransport = true);

        MamaSource (const char*     id,
                    MamaTransport*  transport,
                    const char*     subscSourceName);

        virtual ~MamaSource ();

        void                      setId                  (const char*      id);
        void                      setMappedId            (const char*      id);
        void                      setDisplayId           (const char*      id);
        void                      setQuality             (mamaQuality      quality);
        void                      setState               (mamaSourceState  state);
        void                      setParent              (MamaSource*      parent);
        void                      setTransport           (MamaTransport*   transport);
        void                      setTransportName       (const char*      transportName);
        void                      setPublisherSourceName (const char*      sourceName);

        virtual const char*       getId                  () const;
        virtual const char*       getMappedId            () const;
        virtual const char*       getDisplayId           () const;
        virtual mamaQuality       getQuality             () const;
        virtual mamaSourceState   getState               () const;
        virtual MamaSource*       getParent              ();
        virtual const MamaSource* getParent              () const;
        virtual MamaTransport*    getTransport           () const;
        virtual const char*       getTransportName       () const;
        virtual const char*       getPublisherSourceName () const;

        // Access to C types for implementation of related classes.
        mamaSource                getCValue              ();
        const mamaSource          getCValue              () const;

        /**
         * Is this source part of the supplied source 
         * i.e. Is it the same as the supplied source or is the supplied
         *      source a parent (or parent of a parent) of this source
         */
        bool isPartOf (const MamaSource* source) const;
        // Association of subscriptions with the MamaSource.

        /**
         * Add a subscription.
         */
        void addSubscription  (const char*         symbol,
                               MamaSubscription*   sub);

        /**
         * Look up a MamaSubscription.  NULL is returned if not found.
         */
        MamaSubscription*  findSubscription (const char*  symbol);

        /**
         * Remove a MamaSubscription. Subscription return is the subscription
         * removed from list of associated subscriptions. NULL is returned 
         * if not found.
         */
        MamaSubscription* removeSubscription (const char*  symbol);

        /**
         * Activate all subscriptions for this source.
         */
        void deactivateSubscriptions ();
        /**
         * deactivate all subscriptions for this source.
         */
        void activateSubscriptions ();

        /**
         * Iterate through all subscriptions and call the MamaSubscriptionIterator's
         * onSubscription method for each.
         */
        void forEachSubscription (MamaSubscriptionIteratorCallback* callback,
                                  void*                             closure);

    private:
        mamaSource                          mySource;
        MamaSource*                         myParent;
        void*                               mySubs;
        MamaTransport*                      myTransport;
        mamaBridge                          myBridge;
        MamaSubscriptionIteratorCallback*   mySubscriptionIteratorCallback;
        void*                               mySubscriptionIteratorClosure;

        // No copy contructor nor assignment operator.
        MamaSource             (const MamaSource& rhs);
        MamaSource&  operator= (const MamaSource& rhs);
    };

} // namespace Wombat


#endif // MAMA_SOURCE_CPP_H__
