/* $Id: subsctype.c,v 1.8.34.3 2011/08/10 14:53:26 nicholasmarriott Exp $
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

#include "mama/subscriptiontype.h"

const char* MamaSubscTypeStr (mamaSubscriptionType type)
{
    switch (type)
    {
    case MAMA_SUBSC_TYPE_NORMAL:
        return "NORMAL";

    case MAMA_SUBSC_TYPE_BOOK:
        return "BOOK";

    case MAMA_SUBSC_TYPE_GROUP:
        return "GROUP";

    case MAMA_SUBSC_TYPE_SYMBOL_LIST:
    case MAMA_SUBSC_TYPE_SYMBOL_LIST_NORMAL:
        return "SYMBOL_LIST_NORMAL";

    case MAMA_SUBSC_TYPE_SYMBOL_LIST_GROUP:
        return "SYMBOL_LIST_GROUP";
        
    case MAMA_SUBSC_TYPE_SYMBOL_LIST_BOOK:
        return "SYMBOL_LIST_BOOK";

    default:
        return "unknown";
    }
}

