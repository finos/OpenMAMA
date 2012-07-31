/* $Id: refreshtransport.h,v 1.11.24.6 2011/09/27 11:39:47 emmapollock Exp $
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

#ifndef REFRESH_TRANSPORT_H__
#define REFRESH_TRANSPORT_H__

#if defined(__cplusplus)
extern "C" {
#endif

#define REFRESH_GRANULARITY        13.99 /* Seconds */

typedef struct refreshTransportImpl_* refreshTransport;

extern mama_status 
refreshTransport_create (refreshTransport*  result, 
                         mamaTransport      transport,
                         wList              listeners,
                         mamaBridgeImpl*    bridgeImpl);

extern mama_status 
refreshTransport_destroy (refreshTransport transport);

extern void 
refreshTransport_resetRefreshForListener (refreshTransport transport, 
                                          void*            handle);

extern void
refreshTransport_addSubscription (refreshTransport  transport,
                                  SubscriptionInfo* info);

extern void
refreshTransport_removeListener (refreshTransport transport,  
                                 void*            handle, 
                                 int              freeElement);
    
long refreshTransport_numListeners (refreshTransport transport);

void refreshTransport_iterateListeners(refreshTransport transport,
                                       wListCallback    cb, 
                                       void*            closure);

SubscriptionInfo* 
refreshTransport_allocateSubscInfo (refreshTransport transport);

extern void
refreshTransport_startStaleRecapTimer (struct refreshTransportImpl_ *impl);


#if defined(__cplusplus)
}
#endif

#endif /* REFRESH_TRANSPORT_H_*/
