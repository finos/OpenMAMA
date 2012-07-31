/* $Id: subscmsgtype.h,v 1.7.32.4 2011/10/02 19:02:18 ianbell Exp $
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

#ifndef MAMA_SUBSC_MSG_TYPE_H__
#define MAMA_SUBSC_MSG_TYPE_H__

/*
 * This file provides subscription management details for MAMA.
 */

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum mamaSubscMsgType
{
    MAMA_SUBSC_SUBSCRIBE            = 0,
    MAMA_SUBSC_SNAPSHOT             = 1,
    MAMA_SUBSC_UNSUBSCRIBE          = 2,
    MAMA_SUBSC_RESUBSCRIBE          = 3,
    MAMA_SUBSC_REFRESH              = 4,
    MAMA_SUBSC_DDICT_SNAPSHOT       = 5,
    MAMA_SUBSC_WORLDVIEW_RQST       = 6,
    MAMA_SUBSC_DQ_SUBSCRIBER        = 7,  /* Subscriber knows it lost data itself  */
    MAMA_SUBSC_DQ_PUBLISHER         = 8,  /* Subscriber knows publisher lost data  */
    MAMA_SUBSC_DQ_NETWORK           = 9,  /* Subscriber knows network lost data    */
    MAMA_SUBSC_DQ_UNKNOWN           = 10, /* Subscriber lost data but not know why */
    MAMA_SUBSC_FEATURE_SET_RQST     = 11,
    MAMA_SUBSC_DQ_GROUP_SUBSCRIBER  = 12  /* Subscriber knows it lost data itself  */
} mamaSubscMsgType;

MAMAExpDLL
extern const char* MamaSubscMsgTypeStr    (mamaSubscMsgType    msgType);

#define  MAMA_SUBSCROOT           "_MD"
#define  MAMA_SUBSCMANROOT        "_MDSM"
#define  MAMA_DATADICTROOT        "_MDDD"
#define  MAMA_WORLDVIEWROOT       "_MDWV"
#define  MAMA_WORLDVIEWREQUEST    "_MDWV.WORLDVIEW_REQUEST"
#define  MAMA_WORLDVIEWUPDATE     "_MDWV.WORLDVIEW_UPDATE"
#define  MAMA_TIMEOUT             (10.0)
#define  MAMA_RETRIES             (2)
#define  MAMA_REFRESHINTERVALMIN  (5.0)
#define  MAMA_REFRESHINTERVALRAND (55.0)

#if defined(__cplusplus)
}
#endif

#endif /* MAMA_SUBSC_MSG_TYPE_H__ */

