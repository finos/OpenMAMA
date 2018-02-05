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
	/// Encapsulates both a source object and its weight in a source group
	/// </summary>
	public sealed class MamaWeightedSource
	{
		internal MamaWeightedSource(MamaSource source, uint weight)
		{
			mSource = source;
			mWeight = weight;
		}

		/// <summary>
		/// Returns the source object
		/// </summary>
		public MamaSource Source
		{
			get
			{
				return mSource;
			}
		}

		/// <summary>
		/// Returns the source's weight in the source group
		/// </summary>
		public uint weight
		{
			get
			{
				return mWeight;
			}
		}

		/// <summary>
		/// CLS compliant version of weight
		/// </summary>
		public int weightClsCompliant
		{
			get
			{
				return (int)(long)weight;
			}
		}

		/// <summary>
		/// Provides an implicit cast to MamaSource
		/// </summary>
		public static implicit operator MamaSource(MamaWeightedSource weightedSource)
		{
			return weightedSource.Source;
		}

		private MamaSource mSource;
		private uint mWeight;

	}
}
