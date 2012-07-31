/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2012 NYSE Technologies, Inc.
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

package com.wombat.mamda;
import com.wombat.mama.*;

/**
 * MamdaAuctionUpdate is an interface that provides access to fields
 * related to aucion updates.
 */

public interface MamdaAuctionRecap extends MamdaBasicRecap
{
    /**
     * Get the uncross price.
     * 
     * @return uncross price.  The indicative or firm auction price .  
     */
    MamaPrice  getUncrossPrice();
  
    /**
     * Get the uncross vol.
     *
     * @return Ask price.   The indicative volume, or the volume turned over in the auction 
     */    
    long  getUncrossVolume();

    /**
     * Get the uncross price Ind.
     *
     * @return uncross price Ind.   Indicates whether the Price and Volume
     * is an indicative of the current state of the auction or whether its the (firm) auction price and volume 
     */    
    short  getUncrossPriceInd();
    
    /**
     * Get the uncross price fieldState
     *
     * @return short.  An enumeration representing field state.

     */    
    short   getUncrossPriceFieldState();
    
    /**
     * Get the uncross vol fieldState
     *
     * @return short.  An enumeration representing field state.
     */    
    short   getUncrossVolumeFieldState();

    /**
     * Get the uncross price ind fieldState
     *
     * @return short.  An enumeration representing field state.
     */    
    short   getUncrossPriceIndFieldState();  
}






