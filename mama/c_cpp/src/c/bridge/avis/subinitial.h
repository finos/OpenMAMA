/* $Id: subinitial.h,v 1.1.2.2 2011/09/01 16:34:38 emmapollock Exp $
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

#ifndef AVIS_INBOX_H__
#define AVIS_INBOX_H__

#include "../../bridge.h"


#if defined(__cplusplus)
extern "C" {
#endif

const char*
avisInboxImpl_getReplySubject(inboxBridge inbox);

mama_status
avisMamaInbox_send( mamaInbox inbox,
                    mamaMsg msg );

#if defined(__cplusplus)
}
#endif

#endif /* AVIS_INBOX_H__ */
