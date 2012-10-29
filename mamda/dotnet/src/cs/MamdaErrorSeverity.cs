/* $Id: MamdaErrorSeverity.cs,v 1.1.40.5 2012/08/24 16:12:11 clintonmcdowell Exp $
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
	/// MamdaErrorSeverity defines MAMDA error severities.  The severity is
	/// a hint that can be used to determine what action to take (e.g.,
	/// destroy the subscription).
	/// </summary>
	public enum MamdaErrorSeverity
	{
		/// <summary>
		/// never sent
		/// </summary>
		MAMDA_SEVERITY_OK = 0,

		/// <summary>
		/// low severity
		/// </summary>
		MAMDA_SEVERITY_LOW = 1,

		/// <summary>
		/// high severity
		/// </summary>
		MAMDA_SEVERITY_HIGH = 2
	}

	/// <summary>
	/// Helper class
	/// </summary>
	public sealed class MamdaErrorSeverities
	{
		private MamdaErrorSeverities()
		{
		}

		/// <summary>
		/// Determines a MamdaErrorCode's severity
		/// </summary>
		/// <param name="code">The error code.</param>
		/// <returns>The severity</returns>
		public static MamdaErrorSeverity severityForErrorCode(MamdaErrorCode code)
		{
			switch (code)
			{
				case MamdaErrorCode.MAMDA_NO_ERROR:        return MamdaErrorSeverity.MAMDA_SEVERITY_OK;
				case MamdaErrorCode.MAMDA_ERROR_NOT_FOUND: return MamdaErrorSeverity.MAMDA_SEVERITY_LOW;
				default:                                   return MamdaErrorSeverity.MAMDA_SEVERITY_HIGH;
			}
		}
	}
}
