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

#ifndef MemoryTestGeneratorH
#define MemoryTestGeneratorH

#include <gtest/gtest.h>
#include <valgrind/memcheck.h>

/**
 * This uses hooks into valgrind/memcheck when program is running
 * under valgrind. It will fail the test if not being run under valgrind
 */
#define START_RECORDING_MEMORY() \
    if (RUNNING_ON_VALGRIND == 0) \
    { \
        FAIL()<<"Needs to be run with valgrind"; \
        return; \
    } \
    VALGRIND_DO_QUICK_LEAK_CHECK; \
    long base_reachable, base_suppressed, base_definite, base_dubious; \
    base_definite = base_dubious = base_reachable = base_suppressed = 0; \
    VALGRIND_COUNT_LEAKS(base_definite, base_dubious, base_reachable, base_suppressed)

#define STOP_RECORDING_MEMORY() \
    { \
    VALGRIND_DO_QUICK_LEAK_CHECK; \
    long definite, dubious, reachable, suppressed; \
    definite = dubious = reachable = suppressed = 0; \
    VALGRIND_COUNT_LEAKS(definite, dubious, reachable, suppressed); \
    definite   -= base_definite; \
    dubious    -= base_dubious; \
    if (definite > 0) \
        EXPECT_EQ (base_definite , definite) << "Definite Memory Leak found - check code"; \
    if (dubious > 0) \
        EXPECT_EQ (base_dubious, dubious) << "Dubious Memory Leak found - check code"; \
    }

#endif // MemoryTestGeneratorH

