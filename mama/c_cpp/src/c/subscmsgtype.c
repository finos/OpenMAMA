/* $Id: subscmsgtype.c,v 1.4.34.3 2011/08/10 14:53:26 nicholasmarriott Exp $
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

#include "mama/mama.h"
#include "mama/subscmsgtype.h"


const char* MamaSubscMsgTypeStr (mamaSubscMsgType type)
{
    switch (type)
    {
    case MAMA_SUBSC_SUBSCRIBE:
        return "SUBSCRIBE";

    case MAMA_SUBSC_SNAPSHOT:
        return "SNAPSHOT";

    case MAMA_SUBSC_UNSUBSCRIBE:
        return "UNSUBSCRIBE";
        
    case MAMA_SUBSC_RESUBSCRIBE:
        return "RESUBSCRIBE";

    case MAMA_SUBSC_REFRESH:
        return "REFRESH";

    case MAMA_SUBSC_DDICT_SNAPSHOT:
        return "DDICT_SNAPSHOT";

    case MAMA_SUBSC_WORLDVIEW_RQST:
        return "WORLDVIEW_RQST";

    case MAMA_SUBSC_DQ_SUBSCRIBER:
        return "DQ_SUBSCRIBER";

    case MAMA_SUBSC_DQ_PUBLISHER:
        return "DQ_PUBLISHER";
        
    case MAMA_SUBSC_DQ_NETWORK:
        return "DQ_NETWORK";
        
    case MAMA_SUBSC_DQ_UNKNOWN:
        return "DQ_UNKNOWN";
        
    case MAMA_SUBSC_FEATURE_SET_RQST:
        return "FEATURE_SET_RQST";

    case MAMA_SUBSC_DQ_GROUP_SUBSCRIBER:
        return "DQ_GROUP";

    default:
        return "unknown";
    }
}
