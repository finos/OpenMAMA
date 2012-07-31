/* $Id: syncresponder.h,v 1.3.22.4 2011/09/01 16:34:38 emmapollock Exp $
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
#ifndef SYNC_RESPONDER_H__
#define SYNC_RESPONDER_H__

#if defined(__cplusplus)
extern "C" {
#endif

mama_status
mamaSyncCommand_create (mamaCommand*     command,
                        mamaMsg          msg,
                        mamaTransport    tport,
                        int              transportIndex,
                        mamaPublisher    publisher,
                        mamaCommandEndCB endCB,
                        void*            closure);

mama_status
mamaSyncCommand_run (mamaCommand *command);

#if defined(__cplusplus)
}
#endif

#endif /* SYNC_RESPONDER_H__ */
