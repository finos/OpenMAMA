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

#ifndef WINTERLOCKED_H
#define WINTERLOCKED_H

#include "port.h"

/* The base interlocked type on Windows. */
typedef volatile long wInterlockedInt;

/**
 * This function will initialise a wInterlockedInt.
 *
 * @param[in] value Pointer to the item to be initialized.
 * @return 0 on success.
 */

WCOMMONINLINE int wInterlocked_initialize(wInterlockedInt *value)
{
    return 0;
}

/**
 * This function will destroy a wInterlockedInt.
 *
 * @param[in] value Pointer to the item to be destroyed.
 * @return 0 on success.
 */
WCOMMONINLINE int wInterlocked_destroy(wInterlockedInt *value)
{
    return 0; 
}

/**
 * This function will atomically decrement a 32-bit integer value.
 *
 * @param[in] value Pointer to the value to be decremented.
 * @return The decremented integer.
 */
WCOMMONINLINE int wInterlocked_decrement(wInterlockedInt *value)
{
    return (int)InterlockedDecrement(value);
}

/**
 * This function will atomically increment a 32-bit integer value.
 *
 * @param[in] value Pointer to the value to be incremented.
 * @return The incremented integer.
 */
WCOMMONINLINE int wInterlocked_increment(wInterlockedInt *value)
{
    return (int)InterlockedIncrement(value);
}

/**
 * This function will return the value of the interlocked variable.
 *
 * @param[in] value Pointer to the value to be read.
 * @return The value itself.
 */
WCOMMONINLINE int wInterlocked_read(wInterlockedInt *value)
{
    return *value;
}

/**
 * This function will atomically set a 32-bit integer value.
 *
 * @param[in] newValue The new value to set.
 * @param[in] value Pointer to the value to be set.
 * @return The updated integer.
 */
WCOMMONINLINE int wInterlocked_set(int newValue, wInterlockedInt *value)
{
    return (int)InterlockedExchange(value, (long)newValue);
}

#endif
