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

package com.wombat.mama;

import java.util.logging.Level;

public class MamaLogLevel
{
    public static final MamaLogLevel OFF    = new MamaLogLevel(0);
    public static final MamaLogLevel SEVERE = new MamaLogLevel(1);
    public static final MamaLogLevel ERROR  = new MamaLogLevel(2);
    public static final MamaLogLevel WARN   = new MamaLogLevel(3);
    public static final MamaLogLevel NORMAL = new MamaLogLevel(4);
    public static final MamaLogLevel FINE   = new MamaLogLevel(5);
    public static final MamaLogLevel FINER  = new MamaLogLevel(6);
    public static final MamaLogLevel FINEST = new MamaLogLevel(7);

    private final int myLevel;

    private MamaLogLevel(int level)
    {
        myLevel = level;
    }

    public int getValue()
    {
        return myLevel;
    }

    public static MamaLogLevel tryStringToLogLevel(String level)
    {
        if (level.equalsIgnoreCase("Off"))
        {
            return MamaLogLevel.OFF;
        }
        else if (level.equalsIgnoreCase("Severe"))
        {
            return MamaLogLevel.SEVERE;
        }
        else if (level.equalsIgnoreCase("Error"))
        {
            return MamaLogLevel.ERROR;
        }
        else if (level.equalsIgnoreCase("Warn"))
        {
            return MamaLogLevel.WARN;
        }
        else if (level.equalsIgnoreCase("Normal"))
        {
            return MamaLogLevel.NORMAL;
        }
        else if (level.equalsIgnoreCase("Fine"))
        {
            return MamaLogLevel.FINE;
        }
        else if (level.equalsIgnoreCase("Finer"))
        {
            return MamaLogLevel.FINER;
        }
        else if (level.equalsIgnoreCase("Finest"))
        {
            return MamaLogLevel.FINEST;
        }
        else
        {
            return MamaLogLevel.WARN;
        }
    }

    public static MamaLogLevel getMamaLogLevel(int level)
    {
        switch(level)
        {
            case 0:
            {
                return MamaLogLevel.OFF;
            }
            case 1:
            {
                return MamaLogLevel.SEVERE;
            }
            case 2:
            {
                return MamaLogLevel.ERROR;
            }
            case 3:
            {
                return MamaLogLevel.WARN;
            }
            case 4:
            {
                return MamaLogLevel.NORMAL;
            }
            case 5:
            {
                return MamaLogLevel.FINE;
            }
            case 6:
            {
                return MamaLogLevel.FINER;
            }
            case 7:
            {
                return MamaLogLevel.FINEST;
            }
            default:
            {
                return MamaLogLevel.WARN;
            }
        }
    }

    public static MamaLogLevel getMamaLogLevel(Level level)
    {
        if(level.equals(Level.ALL) ||
            level.equals(Level.FINEST))
        {
            return MamaLogLevel.FINEST;
        }
        else if(level.equals(Level.FINER))
        {
            return MamaLogLevel.FINER;
        }
        else if(level.equals(Level.FINE))
        {
            return MamaLogLevel.FINE;
        }
        else if(level.equals(Level.CONFIG) ||
                level.equals(Level.INFO))
        {
            return MamaLogLevel.NORMAL;
        }
        else if(level.equals(Level.WARNING))
        {
            return MamaLogLevel.WARN;
        }
        else if(level.equals(Level.SEVERE))
        {
            return MamaLogLevel.SEVERE;
        }
        else if(level.equals(Level.OFF))
        {
            return MamaLogLevel.OFF;
        }
        else
        {
            return MamaLogLevel.WARN;
        }
    }

    public static Level getLevel(MamaLogLevel mamalevel)
    {
        if(MamaLogLevel.FINEST == mamalevel)
        {
            return Level.FINEST;
        }
        else if(MamaLogLevel.FINER == mamalevel)
        {
            return Level.FINER;
        }
        else if(MamaLogLevel.FINE == mamalevel)
        {
            return Level.FINE;
        }
        else if(MamaLogLevel.NORMAL == mamalevel)
        {
            return Level.INFO;
        }
        else if(MamaLogLevel.WARN == mamalevel)
        {
            return Level.WARNING;
        }
        else if(MamaLogLevel.SEVERE == mamalevel)
        {
            return Level.SEVERE;
        }
        else if(MamaLogLevel.OFF == mamalevel)
        {
            return Level.OFF;
        }
        else
        {
            return Level.WARNING;
        }
    }
}
