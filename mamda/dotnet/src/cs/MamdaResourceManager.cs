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
using System.Collections;

namespace Wombat
{
	public abstract class MamdaResourceContainer : IDisposable
	{
		protected MamdaResourceContainer()
		{
		}

		/// <summary>
		/// </summary>
		/// <param name="resource"></param>
		protected void AddResource(IDisposable resource)
		{
			if (resource == null)
			{
				throw new ArgumentNullException("resource");
			}
			if (mResources == null)
			{
				mResources = new ArrayList(4);
			}
			mResources.Add(resource);
		}

		/// <summary>
		/// Finalizes the resource container
		/// </summary>
		~MamdaResourceContainer()
		{
			Dispose(false);
		}

		/// <summary>
		/// Destroys the resources
		/// </summary>
		public void Dispose()
		{
			Dispose(true);
		}

		/// <summary>
		/// The actual implementation of Dispose - called by both the Dispose method and the finalizer.
		/// </summary>
		/// <param name="disposing">true if the object is being disposed (false if being finalized)</param>
		private void Dispose(bool disposing)
		{
			if (disposing)
			{
				// prevent the object from being finalized later
				GC.SuppressFinalize(this);
			}
			if (mResources != null)
			{
				foreach (IDisposable resource in mResources)
				{
					resource.Dispose();
				}
				mResources.Clear();
				mResources = null;
			}
		}

		private ArrayList mResources;
	}
}
