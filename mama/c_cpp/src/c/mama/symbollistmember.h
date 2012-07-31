/* $Id: symbollistmember.h,v 1.3.32.2 2011/08/10 14:53:28 nicholasmarriott Exp $
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

#ifndef  MAMA_SYMBOL_LIST_MEMBER_H__
#define  MAMA_SYMBOL_LIST_MEMBER_H__

/* *************************************************** */
/* Includes. */
/* *************************************************** */
#include "mama/mama.h"
#include "mama/symbollisttypes.h"

#if defined(__cplusplus)
extern "C" {
#endif
    
/* *************************************************** */
/* Public Function Prototypes. */
/* *************************************************** */

/**
 * This function will allocate a new symbol list member.
 * The returned member should be freed using mamaSymbolListMember_deallocate.
 *
 * @param[in] symbolList The parent symbol list that the member will
 *                       become part of.
 * @param[out] member To return the new member.
 *
 * @return mama_status value can be one of
 *          MAMA_STATUS_NULL_ARG
 *          MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSymbolListMember_allocate(
    mamaSymbolList symbolList,
    mamaSymbolListMember *member);

/**
 * Frees a symbol list member previously created by a called to 
 * mamaSymbolListMember_allocate.
 *
 * @param[in] member The member to free. 
 *
 * @return mama_status value can be one of
 *          MAMA_STATUS_NULL_ARG
 *          MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSymbolListMember_deallocate(
    mamaSymbolListMember member);

/**
 * This function returns the closure assocated with the symbol list member.
 *
 * @param[in] member The member to free. 
 * @param[out] closure To return the closure.
 *
 * @return mama_status value can be one of
 *          MAMA_STATUS_NULL_ARG
 *          MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSymbolListMember_getClosure(
    const mamaSymbolListMember member,
    void **closure);

int mamaSymbolListMember_getSize (void);

/**
 * This function returns the source assocated with the symbol list member.
 *
 * @param[in] member The member to free. 
 * @param[out] source To return the source.
 *
 * @return mama_status value can be one of
 *          MAMA_STATUS_NULL_ARG
 *          MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSymbolListMember_getSource(
    const mamaSymbolListMember member, 
    const char **source);

/**
 * This function returns the symbol assocated with the symbol list member.
 *
 * @param[in] member The member to free. 
 * @param[out] symbol To return the symbol.
 *
 * @return mama_status value can be one of
 *          MAMA_STATUS_NULL_ARG
 *          MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSymbolListMember_getSymbol(
    const mamaSymbolListMember member,
    const char **symbol);

/**
 * This function returns the symbol list assocated with the symbol list member.
 *
 * @param[in] member The member to free. 
 * @param[out] symbolList To return the symbol list.
 *
 * @return mama_status value can be one of
 *          MAMA_STATUS_NULL_ARG
 *          MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSymbolListMember_getSymbolList(
    const mamaSymbolListMember member,
    mamaSymbolList *symbolList);

/**
 * This function returns the transport assocated with the symbol list member.
 *
 * @param[in] member The member to free. 
 * @param[out] transport To return the transport.
 *
 * @return mama_status value can be one of
 *          MAMA_STATUS_NULL_ARG
 *          MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSymbolListMember_getTransport(
    const mamaSymbolListMember member,
    mamaTransport *transport);





MAMAExpDLL
extern mama_status
mamaSymbolListMember_setClosure (mamaSymbolListMember  member,
                                 void*                 closure);

MAMAExpDLL
extern mama_status
mamaSymbolListMember_setSource (mamaSymbolListMember    member,
                                const char*             source);

MAMAExpDLL
extern mama_status
mamaSymbolListMember_setSymbol (mamaSymbolListMember    member,
                                const char*             symbol);

MAMAExpDLL
extern mama_status
mamaSymbolListMember_setSymbolList (mamaSymbolListMember  member,
                                    mamaSymbolList        symbolList);

MAMAExpDLL
extern mama_status
mamaSymbolListMember_setTransport (mamaSymbolListMember member,
                                   mamaTransport        transport);

#if defined(__cplusplus)
}
#endif


#endif

