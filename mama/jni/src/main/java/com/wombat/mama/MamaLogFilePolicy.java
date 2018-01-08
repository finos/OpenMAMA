/* $Id:
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

package com.wombat.mama;

public class MamaLogFilePolicy
{
    public static final MamaLogFilePolicy UNBOUNDED = new MamaLogFilePolicy(1);
    public static final MamaLogFilePolicy ROLL      = new MamaLogFilePolicy(2);
    public static final MamaLogFilePolicy OVERWRITE = new MamaLogFilePolicy(3);
    public static final MamaLogFilePolicy USER      = new MamaLogFilePolicy(4);

    private final int myPolicy;

    private MamaLogFilePolicy(int policy)
    {
        myPolicy = policy;
    }

    public int getValue()
    {
        return myPolicy;
    }

    public static MamaLogFilePolicy tryStringToLogPolicy(String policy)
    {
        if (policy.equalsIgnoreCase("Unbounded"))
        {
            return MamaLogFilePolicy.UNBOUNDED;
        }
        else if (policy.equalsIgnoreCase("Roll"))
        {
            return MamaLogFilePolicy.ROLL;
        }
        else if (policy.equalsIgnoreCase("Overwrite"))
        {
            return MamaLogFilePolicy.OVERWRITE;
        }
        else if (policy.equalsIgnoreCase("User"))
        {
            return MamaLogFilePolicy.USER;
        }
        else
        {
            throw new MamaException( "Log policy string not recognised: " + policy );
        }
    }
}
