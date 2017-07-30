/*
 * OpenMAMA: The open middleware agnostic messaging API
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

#include "wombat/port.h"

/* OSX doesn't currently allow for specifying a specific core to bind
 * a thread to. There is an affinity API 
 * [https://developer.apple.com/library/content/releasenotes/Performance/RN-AffinityAPI/]
 * but this has a focus on memory binding, which will provide a different result
 * than most apps expect. As such leaving this as a no-op. 
 */
int wthread_set_affinity_mask (wthread_t         thread,
                               size_t            cpu_set_size,
                               CPU_AFFINITY_SET* cpu_set_mask)
{
    // Return a non-zero so we know it has not bound. 
    return 1;
}

inline void CPU_SET (int i, cpu_set_t* affinity)
{
    affinity->count |= (1 << i);
}
