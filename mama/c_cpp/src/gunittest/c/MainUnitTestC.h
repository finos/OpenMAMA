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

#ifndef MAINUNITTESTC_H_
#define MAINUNITTESTC_H_

#define NOT_NULL ( (void*)1 )


#define ALLOW_NON_IMPLEMENTED(status)                                          \
 do {                                                                          \
     if (MAMA_STATUS_NOT_IMPLEMENTED == status)                                \
     {                                                                         \
         SUCCEED();                                                            \
         return;                                                               \
     }                                                                         \
 } while (0);

#ifndef GTEST_STRICTNESS
#define GTEST_STRICTNESS 3
#endif

#if GTEST_STRICTNESS < 1 || GTEST_STRICTNESS > 4
#error "Please set an appropriate GTEST_STRICTNESS level (1,2,3 or 4)"
#endif

/* Most lenient. Allows all methods to return non-implemented */
#if GTEST_STRICTNESS == 1
#define CHECK_NON_IMPLEMENTED_MANDATORY(status) ALLOW_NON_IMPLEMENTED(status)
#define CHECK_NON_IMPLEMENTED_RECOMMENDED(status) ALLOW_NON_IMPLEMENTED(status)
#define CHECK_NON_IMPLEMENTED_OPTIONAL(status) ALLOW_NON_IMPLEMENTED(status)

#elif GTEST_STRICTNESS == 2
#define CHECK_NON_IMPLEMENTED_MANDATORY(status)
#define CHECK_NON_IMPLEMENTED_RECOMMENDED(status) ALLOW_NON_IMPLEMENTED(status)
#define CHECK_NON_IMPLEMENTED_OPTIONAL(status) ALLOW_NON_IMPLEMENTED(status)

#elif GTEST_STRICTNESS == 3
#define CHECK_NON_IMPLEMENTED_MANDATORY(status)
#define CHECK_NON_IMPLEMENTED_RECOMMENDED(status)
#define CHECK_NON_IMPLEMENTED_OPTIONAL(status) ALLOW_NON_IMPLEMENTED(status)

#elif GTEST_STRICTNESS == 4
#define CHECK_NON_IMPLEMENTED_ALL(status)
#define CHECK_NON_IMPLEMENTED_OPTIONAL(status)
#define CHECK_NON_IMPLEMENTED_RECOMMENDED(status)
#endif /* GTEST_STRICTNESS_SETTINGS */

const char* getMiddleware (void);

const char* getPayload (void);

const char getPayloadId (void);

const char* getTransport (void);
#endif /* MAINUNITTESTC_H_ */
