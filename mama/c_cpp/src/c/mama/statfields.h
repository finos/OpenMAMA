/* $Id: statfields.h,v 1.6.4.1.16.3 2011/08/10 14:53:28 nicholasmarriott Exp $
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

#ifndef MamaStatFieldsH__
#define MamaStatFieldsH__

#include "mama/mama.h"

#if defined(__cplusplus)
extern "C" {
#endif

MAMAExpDLL
extern const MamaReservedField  MamaStatTime;                           /* FID 101 */
MAMAExpDLL
extern const MamaReservedField  MamaStatName;                           /* FID 102 */
MAMAExpDLL
extern const MamaReservedField  MamaStatType;                           /* FID 103 */
MAMAExpDLL
extern const MamaReservedField  MamaStatMiddleware;                     /* FID 104 */
MAMAExpDLL
extern const MamaReservedField  MamaStatInitials;                       /* FID 105 */
MAMAExpDLL
extern const MamaReservedField  MamaStatRecaps;                         /* FID 106 */
MAMAExpDLL
extern const MamaReservedField  MamaStatNumMessages;                    /* FID 107 */
MAMAExpDLL
extern const MamaReservedField  MamaStatFtTakeovers;                    /* FID 108 */
MAMAExpDLL
extern const MamaReservedField  MamaStatQueueSize;                      /* FID 109 */
MAMAExpDLL
extern const MamaReservedField  MamaStatNumSubscriptions;               /* FID 110 */
MAMAExpDLL
extern const MamaReservedField  MamaStatTimeouts;                       /* FID 111 */
MAMAExpDLL
extern const MamaReservedField  MamaStatMsgIndex;                       /* FID 112 */
MAMAExpDLL
extern const MamaReservedField  MamaStatNakPacketsSent;                 /* FID 113 */
MAMAExpDLL
extern const MamaReservedField  MamaStatNaksSent;                       /* FID 114 */
MAMAExpDLL
extern const MamaReservedField  MamaStatMsgsLost;                       /* FID 115 */
MAMAExpDLL
extern const MamaReservedField  MamaStatNcfsIgnore;                     /* FID 116 */
MAMAExpDLL
extern const MamaReservedField  MamaStatNcfsShed;                       /* FID 117 */
MAMAExpDLL
extern const MamaReservedField  MamaStatNcfsRxDelay;                    /* FID 118 */
MAMAExpDLL
extern const MamaReservedField  MamaStatNcfsUnknown;                    /* FID 119 */
MAMAExpDLL
extern const MamaReservedField  MamaStatDuplicateDataMsgs;              /* FID 120 */
MAMAExpDLL
extern const MamaReservedField  MamaStatUnrecoverableWindowAdvance;     /* FID 121 */
MAMAExpDLL
extern const MamaReservedField  MamaStatUnrecoverableTimeout;           /* FID 122 */
MAMAExpDLL
extern const MamaReservedField  MamaStatLbmMsgsReceivedNoTopic;         /* FID 123 */
MAMAExpDLL
extern const MamaReservedField  MamaStatLbmRequestsReceived;            /* FID 124 */
MAMAExpDLL
extern const MamaReservedField  MamaStatWombatMsgs;                     /* FID 125 */
MAMAExpDLL
extern const MamaReservedField  MamaStatRvMsgs;                         /* FID 126 */
MAMAExpDLL
extern const MamaReservedField  MamaStatFastMsgs;                       /* FID 127 */
MAMAExpDLL
extern const MamaReservedField  MamaStatUnknownMsgs;                    /* FID 128 */

#if defined(__cplusplus)
}
#endif

#endif /* MamaStatFieldsH__*/
