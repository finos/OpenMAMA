/* $Id: MamdaOptionChainViewRangeHandler.cs,v 1.2.38.5 2012/08/24 16:12:12 clintonmcdowell Exp $
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
	/// Interface to handle change in a MamdaOptionChainView range.
	/// </summary>
	public interface MamdaOptionChainViewRangeHandler
	{
		/// <summary>
		/// Action to take when the strike price range is reset to a new
		/// range.
		/// </summary>
		/// <param name="view"></param>
		/// <param name="lowStrike"></param>
		/// <param name="highStrike"></param>
		void onOptionViewStrikeRangeReset(
			MamdaOptionChainView  view,
			double                lowStrike,
			double                highStrike);
	}
}
