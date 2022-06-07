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
	///     A container of <see cref="MamaSource">MamaSource</see> objects. Because each MamaSource can
	///     contain sub-sources (and so on), MamaSource is also a source
	///     manager and implements this interface.
	/// </summary>
	public interface ISourceManager : IDisposable
    {
	    /// <summary>
	    ///     Create a new <see cref="MamaSource">MamaSource</see> and add it to the manager.
	    /// </summary>
	    /// <param name="name">The string identifier for the MamaSource.</param>
	    /// <returns>The created MamaSource object if execution is successful.</returns>
	    MamaSource create(string name);

	    /// <summary>
	    ///     Locates an existing <see cref="MamaSource">MamaSource</see> for the given name. If none exists creates
	    ///     and adds a new MamaSource.
	    /// </summary>
	    /// <param name="name">The string identifier for the MamaSource</param>
	    /// <returns>The MamaSource object if execution is successful.</returns>
	    MamaSource findOrCreate(string name);

	    /// <summary>
	    ///     Locates an existing <see cref="MamaSource">MamaSource</see> with the specified string 'name' identifier.
	    ///     Returns null if no source was located.
	    /// </summary>
	    /// <param name="name">The string identifier for the required MamaSource</param>
	    /// <returns>
	    ///     The MamaSource object if found, or null if not found
	    ///     (if execution is successful)
	    /// </returns>
	    MamaSource find(string name);

	    /// <summary>
	    ///     Add an existing <see cref="MamaSource">MamaSource</see>.
	    ///     The id of the source will be used instead of the name to uniquely identify
	    ///     the source within the manager.
	    /// </summary>
	    /// <param name="source">The <see cref="MamaSource">MamaSource</see> being added</param>
	    void add(MamaSource source);

	    /// <summary>
	    ///     Add an existing <see cref="MamaSource">MamaSource</see> using the specified name as a unique identifier.
	    /// </summary>
	    /// <param name="name">The string identifier for the MamaSource</param>
	    /// <param name="source">The MamaSource being added</param>
	    void add(string name, MamaSource source);

        // NB:
        // the destroy method does not belong here because if we decide to
        // expose an ISourceManager property from the MamaSource the user would be
        // able to destroy the underlying source manager in the source, which should
        // not be possible; hence, the destroy method exists in the MamaSource and
        // MamaSourceManager classes
    }
}