/* $Id: environment.c,v 1.2.2.3.12.2 2011/08/10 14:53:23 nicholasmarriott Exp $
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

/* ********************************************************** */
/* Includes. */
/* ********************************************************** */
#include "stdlib.h"
#include "wombat/environment.h"

int environment_deleteVariable(const char *name)
{
    return unsetenv(name);
}

const char *environment_getVariable(const char *name)
{    
    /* Returns. */
    const char *ret = NULL;

    /* Check the argument. */
    if(name != NULL)
    {
        ret = getenv(name);    
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
        ret = setenv(name, value, 1);
    }

    return ret;
}
