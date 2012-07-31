/* $Id: statfields.c,v 1.5.6.1.16.3 2011/08/10 14:53:26 nicholasmarriott Exp $
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

#include "mama/statfields.h"
#include "mama/reservedfields.h"

const MamaReservedField  MamaStatTime
    = {"Time", 101};
const MamaReservedField  MamaStatName
    = {"Name", 102};
const MamaReservedField  MamaStatType
    = {"Type", 103};
const MamaReservedField  MamaStatMiddleware
    = {"Middleware", 104};
const MamaReservedField  MamaStatInitials
    = {"Initials", 105};
const MamaReservedField  MamaStatRecaps
    = {"Recaps", 106};
const MamaReservedField  MamaStatNumMessages
    = {"Messages", 107};
const MamaReservedField  MamaStatFtTakeovers
    = {"FT Takeovers", 108};
const MamaReservedField  MamaStatQueueSize
    = {"Queue Size", 109};
const MamaReservedField  MamaStatNumSubscriptions
    = {"Subscriptions", 110};
const MamaReservedField  MamaStatTimeouts
    = {"Timeouts", 111};
const MamaReservedField  MamaStatMsgIndex
    = {"Message Index", 112};
const MamaReservedField  MamaStatNakPacketsSent
    = {"Nak Packets Sent", 113};
const MamaReservedField  MamaStatNaksSent
    = {"Naks Sent", 114};
const MamaReservedField  MamaStatMsgsLost
    = {"Messages Lost", 115};
const MamaReservedField  MamaStatNcfsIgnore
    = {"Ncfs Ignore", 116};
const MamaReservedField  MamaStatNcfsShed
    = {"Ncfs Shed", 117};
const MamaReservedField  MamaStatNcfsRxDelay
    = {"Ncfs Retransmit Delay", 118};
const MamaReservedField  MamaStatNcfsUnknown
    = {"Ncfs Unknown", 119};
const MamaReservedField  MamaStatDuplicateDataMsgs
    = {"Duplicate Messages", 120};
const MamaReservedField  MamaStatUnrecoverableWindowAdvance
    = {"Window Advance Msgs Unrecoverable", 121};
const MamaReservedField  MamaStatUnrecoverableTimeout
    = {"Timeout Msgs Unrecoverable", 122};
const MamaReservedField  MamaStatLbmMsgsReceivedNoTopic
    = {"LBM Msgs Received With No Topic", 123};
const MamaReservedField  MamaStatLbmRequestsReceived
    = {"LBM Requests Received", 124};
const MamaReservedField  MamaStatWombatMsgs
    = {"Wombat Msgs", 125};
const MamaReservedField  MamaStatRvMsgs
    = {"RV Msgs", 126};
const MamaReservedField  MamaStatFastMsgs
    = {"FAST Msgs", 127};
const MamaReservedField  MamaStatUnknownMsgs
    = {"Unknown Msgs", 128};
