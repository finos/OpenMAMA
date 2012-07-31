/* $Id: transportbridge.h,v 1.1.2.3 2011/09/16 19:42:31 billtorpey Exp $
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

#ifndef AVIS_TRANSPORT_H__
#define AVIS_TRANSPORT_H__

#include <stdlib.h>
#include <string.h>

#include <avis/elvin.h>

#include <mama/mama.h>
#include <bridge.h>

#include "avisdefs.h"

#if defined(__cplusplus)
extern "C" {
#endif

Elvin* getAvis(mamaTransport transport);


mama_status avisTransportBridge_start(avisTransportBridge* transportBridge);
mama_status avisTransportBridge_stop(avisTransportBridge* transportBridge);


#if defined(__cplusplus)
}
#endif

#endif /* AVIS_TRANSPORT_H__*/
