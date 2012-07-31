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

/**
 * MamdaSecurityStatusRecap is an interface that provides access to security status
 * related fields.
 */
public interface MamdaSecurityStatusRecap extends MamdaBasicRecap
{
    /**
     * @see MamdaSecurityStatusUpdate#getSecurityStatus() 
     * @deprecated
     */
    public long   getSecurityStatus();

    /**
     * @return the security status Field State
     */
    public short  getSecurityStatusFieldState();

    /**
     * @see MamdaSecurityStatusUpdate#getSecurityStatusQualifier()
     * @deprecated
     */
    public long   getSecurityStatusQualifier();

    /**
     * @return the security status Field State
     */
    public short  getSecurityStatusQualifierFieldState();

    /**
     * @see MamdaSecurityStatusUpdate#getSecurityStatusEnum()
     */ 
    public short  getSecurityStatusEnum();

    /**
     * @return the security status Field State
     */
    public short  getSecurityStatusEnumFieldState();

    /**
     * @see MamdaSecurityStatusUpdate#getSecurityStatusQualifierEnum()
     */
    public short  getSecurityStatusQualifierEnum(); 

    /**
     * @return the security status qualifier enum Field State
     */
    public short  getSecurityStatusQualifierEnumFieldState();

    /**
     * @see MamdaSecurityStatusUpdate#getSecurityStatusStr()
     */
    public String getSecurityStatusStr();

    /**
     * @return the security status string Field State
     */
    public short  getSecurityStatusStrFieldState();

    /**
     * @see MamdaSecurityStatusUpdate#getSecurityStatusQualifierStr()
     */
    public String getSecurityStatusQualifierStr();

    /**
     * @return the security status qualifier string Field State
     */
    public short  getSecurityStatusQualifierStrFieldState();

    /**
     * @see MamdaSecurityStatusUpdate#getSecurityStatusOrigStr()
     */
    public String getSecurityStatusOrigStr();

    /**
     * @return the security status orig string Field State
     */
    public short  getSecurityStatusOrigStrFieldState();

    /**
     * @see MamdaSecurityStatusUpdate#getReason()
     */
    public String getReason();

    /**
     * @return the reason Field State
     */
    public short  getReasonFieldState();

    /**
     * getShortSaleCircuitBreaker Returns the ShortSaleCircuitBreaker
     * @return ShortSaleCircuitBreaker
     * @see MamdaSecurityStatusUpdate#getShortSaleCircuitBreaker()
     */
    public char getShortSaleCircuitBreaker();  

    /**
     * @return Returns the FieldState, always MODIFIED.
     */
    public short getShortSaleCircuitBreakerFieldState();
   
}
