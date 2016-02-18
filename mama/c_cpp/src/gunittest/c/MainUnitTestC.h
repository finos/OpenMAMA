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


#define ALLOW_NON_IMPLEMENTED(strictness, status)                              \
 do {                                                                          \
     if (strictness >= getStrictness()                                         \
             && MAMA_STATUS_NOT_IMPLEMENTED == status)                         \
     {                                                                         \
         SUCCEED();                                                            \
         return;                                                               \
     }                                                                         \
 } while (0);

#define CHECK_NON_IMPLEMENTED_MANDATORY(status)   ALLOW_NON_IMPLEMENTED(MANDATORY,    status)
#define CHECK_NON_IMPLEMENTED_RECOMMENDED(status) ALLOW_NON_IMPLEMENTED(RECOMMENDED,  status)
#define CHECK_NON_IMPLEMENTED_OPTIONAL(status)    ALLOW_NON_IMPLEMENTED(DISCRETIONAL, status)

typedef enum gtest_strictness {
    MANDATORY,
    RECOMMENDED,
    DISCRETIONAL,
    NONE
} gtest_strictness;

gtest_strictness getStrictness (void);

const char* getMiddleware (void);
const char* getTransport (void);
const char* getPayload (void);
const char  getPayloadId (void);
const char* getTransport (void);
const char* getSource (void);
const char* getBadSource (void);
const char* getSymbol (void);

#endif /* MAINUNITTESTC_H_ */
