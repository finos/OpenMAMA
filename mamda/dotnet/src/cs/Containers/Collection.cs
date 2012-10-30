/* $Id: Collection.cs,v 1.2.38.5 2012/08/24 16:12:10 clintonmcdowell Exp $
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

namespace Wombat.Containers
{
	public interface Collection : Iterable
	{
		int size();
		bool isEmpty();
		bool contains(object o);
		object[] toArray();
		bool add(object o);
		bool remove(object o);
		bool containsAll(Collection c);
		bool addAll(Collection c);
		bool removeAll(Collection c);
		bool retainAll(Collection c);
		void clear();
	}
}
