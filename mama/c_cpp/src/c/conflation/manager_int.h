/* $Id: manager_int.h,v 1.1.2.3 2011/09/01 16:34:38 emmapollock Exp $
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

/**
 * This class contains information regarding outbound connections for
 * per/connection conflation.
 */

#ifndef MAMA_MANAGER_INT_H__
#define MAMA_MANAGER_INT_H__
#include "wombat/port.h"

#include "mama/conflation/manager.h"

#if defined(__cplusplus)
extern "C" {
#endif

typedef mama_status (*conflateProcessCb) (mamaConflationManager mgr, 
                                          mamaMsg               msg,
                                          const char*           topic);

/**
 * Return the message wrapper used by this manager.
 */
typedef mamaMsg (*conflateGetMsgCb) (mamaConflationManager mgr); 

#if defined(__cplusplus)
}
#endif
#endif /* MAMA_CONNECTION_INT_H__ */
