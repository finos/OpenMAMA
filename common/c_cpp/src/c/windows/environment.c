/*
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

#include "stdlib.h"
#include "wombat/environment.h"

int environment_deleteVariable(const char *name)
{
    /* Returns. */
    int ret = -1;

    /* Check arguments. */
    if(name != NULL)
    {
        /* Set the value. */
        errno_t pe = _putenv_s(name, "");
        if(pe == 0)    
        {
            ret = 0;
        }
    }

    return ret;
}

const char *environment_getVariable(const char *name)
{
    /* Returns. */
    const char *ret = NULL;

    /* Check the argument. */
    if(name != NULL)
    {
        ret = getenv(name);  

        /* Return NULL for a blank string. */
        if((ret != NULL) && (ret[0] == '\0'))
        {
            ret = NULL;
        }
    }

    return ret;
}

int environment_setVariable(const char *name, const char *value)
{
    /* Returns. */
    int ret = -1;

    /* Check the arguments. */
    if((name != NULL) && (value != NULL))
    {
        /* Set the value. */
        errno_t pe = _putenv_s(name, value);
        if(pe == 0)    
        {
            ret = 0;
        }
    }

    return ret;
}

