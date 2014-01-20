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

using System;

namespace Wombat
{
	/// <summary>
	/// MamdaSecurityStatusRecap is an interface that provides access to security status
	/// related fields.
	/// </summary>
	public interface MamdaSecurityStatusRecap
	{
		/// <summary>
		/// <see cref="MamdaSecurityStatusUpdate.getSecurityStatus()"/> 
		/// </summary>
		/// <returns></returns>
		long getSecurityStatus();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getSecurityStatusFieldState();

		/// <summary>
		/// <see cref="MamdaSecurityStatusUpdate.getSecurityStatus()"/>
		/// </summary>
		/// <returns></returns>
		long getSecurityStatusQualifier();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getSecurityStatusQualifierFieldState();

		/// <summary>
		/// <see cref="MamdaSecurityStatusUpdate.getSecurityStatusEnum()"/>
		/// </summary>
		/// <returns></returns>
		MamdaSecurityStatus.mamdaSecurityStatus getSecurityStatusEnum();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getSecurityStatusEnumFieldState();

		/// <summary>
		/// <see cref="MamdaSecurityStatusUpdate.getSecurityStatusQualifierEnum()"/>
		/// </summary>
		/// <returns></returns>
		MamdaSecurityStatusQual.mamdaSecurityStatusQual getSecurityStatusQualifierEnum();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getSecurityStatusQualifierEnumFieldState();

		/// <summary>
		/// <see cref="MamdaSecurityStatusUpdate.getSecurityStatusQualifier()"/>
		/// </summary>
		/// <returns></returns>
		string getSecurityStatusStr();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getSecurityStatusStrFieldState();

		/// <summary>
		/// <see cref="MamdaSecurityStatusUpdate.getSecurityStatusQualifier()"/>
		/// </summary>
		/// <returns></returns>
		string getSecurityStatusQualifierStr();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getSecurityStatusQualifierStrFieldState();

        /// <summary>
		/// Return the native security status qualifier
		/// </summary>
		/// <returns></returns>
        string getSecurityStatusOrigStr();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getSecurityStatusOrigStrFieldState();

		/// <summary>
		/// <see cref="MamdaSecurityStatusUpdate.getReason()"/>
		/// </summary>
		/// <returns></returns>
		string getReason();

        /// <summary>
        /// Get the field state
        /// </summary>
        /// <returns>Field State</returns>
        MamdaFieldState getReasonFieldState();

        /// <summary>
        /// <see cref="MamdaSecurityStatusUpdate.getShortSaleCircuitBreaker()"/>
        /// </summary>
        /// <returns></returns>
        char getShortSaleCircuitBreaker();

        /// <summary>
        /// <see cref="MamdaSecurityStatusUpdate.getShortSaleCircuitBreakerFieldState()"/>
        /// </summary>
        /// <returns></returns>
        MamdaFieldState getShortSaleCircuitBreakerFieldState();

        /// <summary>
        /// <see cref="MamdaSecurityStatusUpdate.getLuldTime()"/>
        /// </summary>
        /// <returns></returns>
        DateTime getLuldTime();

         /// <summary>
         /// Get the field state
         /// </summary>
         /// <returns>Field State</returns>
         MamdaFieldState getLuldTimeFieldState();

        /// <summary>
        /// <see cref="MamdaSecurityStatusUpdate.getLuldIndicator()"/>
        /// </summary>
        /// <returns></returns>
        char getLuldIndicator();

         /// <summary>
         /// Get the field state
         /// </summary>
         /// <returns>Field State</returns>
         MamdaFieldState getLuldIndicatorFieldState();

        // <summary>
        /// <see cref="MamdaSecurityStatusUpdate.getLuldHighLimit()"/>
        /// </summary>
        /// <returns></returns>
        MamaPrice getLuldHighLimit();

         /// <summary>
         /// Get the field state
         /// </summary>
         /// <returns>Field State</returns>
         MamdaFieldState getLuldHighLimitFieldState();

        /// <summary>
        /// <see cref="MamdaSecurityStatusUpdate.getLuldLowLimit()"/>
        /// </summary>
        /// <returns></returns>
        MamaPrice getLuldLowLimit();

         /// <summary>
         /// Get the field state
         /// </summary>
         /// <returns>Field State</returns>
         MamdaFieldState getLuldLowLimitFieldState();


	}
}
