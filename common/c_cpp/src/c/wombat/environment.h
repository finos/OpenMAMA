/* $Id: environment.h,v 1.2.2.3.14.3 2011/08/10 14:53:24 nicholasmarriott Exp $
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

#ifndef _WOMBAT_ENVIRONMENT_H
#define _WOMBAT_ENVIRONMENT_H

#include "wombat/wConfig.h"

/**
 * This function will delete an environment varible.
 *
 * @param name (in) The name of the variable to delete.
 * @return -1 on failure, 0 on success.
 */
COMMONExpDLL
extern int
environment_deleteVariable(
    const char *name);

/**
 * This function will return the value of an environment variable.
 *
 * @param name (in) The name of the environment variable.
 * @return The value of the varaible as a string.
 */
COMMONExpDLL
extern const char *
environment_getVariable(
    const char *name);

/**
 * This function will set the value of an environment variable.
 *
 * @param name (in) The name of the environment variable.
 * @param value (in) The value of the environment variable.
 * @return -1 on failure, 0 on success.
 */
COMMONExpDLL
extern int
environment_setVariable(
    const char *name,
    const char *value);

#endif /* _WOMBAT_ENVIRONMENT_H */
