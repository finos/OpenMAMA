/* $Id$
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

#include <mamda/MamdaOrderImbalanceType.h>

using namespace Wombat;

const char* MamdaOrderImbalanceType::valueToString (const int value)
{
    switch (value)
    {
        case MARKET_IMBALANCE_BUY_VALUE:    return "MktImbBuy";
        case MARKET_IMBALANCE_SELL_VALUE:   return "MktImbSell";
        case NO_MARKET_IMBALANCE_VALUE:     return "NoMktImb";
        case MOC_IMBALANCE_BUY_VALUE:       return "MocImbBuy";
        case MOC_IMBALANCE_SELL_VALUE:      return "MocImbSell";
        case NO_MOC_IMBALANCE_VALUE:        return "NoMocImb";
        case ORDER_IMB_VALUE:               return "OrderImb";
        case ORDER_INF_VALUE:               return "OrderInf";
        case ORDER_IMBALANCE_BUY_VALUE:     return "OrderImbBuy";
        case ORDER_IMBALANCE_SELL_VALUE:    return "OrderImbSell";
        case NO_ORDER_IMBALANCE_VALUE:      return "OrderImbNone";
        default:                            return "UNKNOWN";
    }
    
}

const int  MamdaOrderImbalanceType::stringToValue (const char* type)
{
    int typeValue = atoi (type);

    if (strcmp (MAMDA_MARKET_IMBALANCE_BUY,  type)==0)      return MARKET_IMBALANCE_BUY_VALUE;
    if (strcmp (MAMDA_MARKET_IMBALANCE_SELL, type)==0)      return MARKET_IMBALANCE_SELL_VALUE;
    if (strcmp (MAMDA_NO_MARKET_IMBALANCE,   type)==0)      return NO_MARKET_IMBALANCE_VALUE;
    if (strcmp (MAMDA_MOC_IMBALANCE_BUY ,    type)==0)      return MOC_IMBALANCE_BUY_VALUE;
    if (strcmp (MAMDA_MOC_IMBALANCE_SELL,    type)==0)      return MOC_IMBALANCE_SELL_VALUE;
    if (strcmp (MAMDA_NO_MOC_IMBALANCE,      type)==0)      return NO_MOC_IMBALANCE_VALUE;
    if (strcmp (MAMDA_ORDER_IMB,             type)==0)      return ORDER_IMB_VALUE;
    if (strcmp (MAMDA_ORDER_INF,             type)==0)      return ORDER_INF_VALUE;
    if (strcmp (MAMDA_ORDER_IMBALANCE_BUY,   type)==0)      return ORDER_IMBALANCE_BUY_VALUE;
    if (strcmp (MAMDA_ORDER_IMBALANCE_SELL,  type)==0)      return ORDER_IMBALANCE_SELL_VALUE;
    if (strcmp (MAMDA_NO_ORDER_IMBALANCE,    type)==0)      return NO_ORDER_IMBALANCE_VALUE;
    
    //for fh configured as int 
    if (typeValue != 0) //success
    {
        if      (typeValue == MARKET_IMBALANCE_BUY_VALUE)   return MARKET_IMBALANCE_BUY_VALUE;
        else if (typeValue == MARKET_IMBALANCE_SELL_VALUE)  return MARKET_IMBALANCE_SELL_VALUE;
        else if (typeValue == NO_MARKET_IMBALANCE_VALUE)    return NO_MARKET_IMBALANCE_VALUE;
        else if (typeValue == MOC_IMBALANCE_BUY_VALUE)      return MOC_IMBALANCE_BUY_VALUE;
        else if (typeValue == MOC_IMBALANCE_SELL_VALUE)     return MOC_IMBALANCE_SELL_VALUE;
        else if (typeValue == NO_MOC_IMBALANCE_VALUE)       return NO_MOC_IMBALANCE_VALUE;
        else if (typeValue == ORDER_IMB_VALUE)              return ORDER_IMB_VALUE;
        else if (typeValue == ORDER_INF_VALUE)              return ORDER_INF_VALUE;     
        else if (typeValue == ORDER_IMBALANCE_BUY_VALUE)    return ORDER_IMBALANCE_BUY_VALUE;
        else if (typeValue == ORDER_IMBALANCE_SELL_VALUE)   return ORDER_IMBALANCE_SELL_VALUE;
        else if (typeValue == NO_ORDER_IMBALANCE_VALUE)     return NO_ORDER_IMBALANCE_VALUE;
        else                                                return UNKNOWN;
    }
    return UNKNOWN;
}

/**
 * Determines if the value is of type order imbalance
 * @param value
 * @return bool
 */
bool MamdaOrderImbalanceType::isMamdaOrderImbalanceType (const int value)
{
    bool imbalanceType  = false;

    switch (value)
    {
        case MARKET_IMBALANCE_BUY_VALUE:
        case MARKET_IMBALANCE_SELL_VALUE:
        case MOC_IMBALANCE_BUY_VALUE:
        case MOC_IMBALANCE_SELL_VALUE:
        case ORDER_IMB_VALUE:
        case ORDER_INF_VALUE:
        case ORDER_IMBALANCE_BUY_VALUE:
        case ORDER_IMBALANCE_SELL_VALUE:
        case NO_MARKET_IMBALANCE_VALUE:
        case NO_MOC_IMBALANCE_VALUE: 
        case NO_ORDER_IMBALANCE_VALUE:
            imbalanceType = true;
            break;
        default:
            imbalanceType = false;
            break;
   }
   return imbalanceType;  
}

/**
 * Determines if the value is of an order imbalance
 * @param value
 * @return bool
 */
bool MamdaOrderImbalanceType::isMamdaImbalanceOrder (const int value)
{
    bool imbalanceOrder  = false;
    switch (value)
    {
        case MARKET_IMBALANCE_BUY_VALUE:
        case MARKET_IMBALANCE_SELL_VALUE:
        case MOC_IMBALANCE_BUY_VALUE:
        case MOC_IMBALANCE_SELL_VALUE:
        case ORDER_IMB_VALUE:
        case ORDER_INF_VALUE:
        case ORDER_IMBALANCE_BUY_VALUE:
        case ORDER_IMBALANCE_SELL_VALUE:
            imbalanceOrder = true;
            break;
        case NO_MARKET_IMBALANCE_VALUE:
        case NO_MOC_IMBALANCE_VALUE: 
        case NO_ORDER_IMBALANCE_VALUE:
            imbalanceOrder = false;
            break;
        default:
            imbalanceOrder = false;
    }
    return imbalanceOrder;  
}
    
