/* $Id: symbollist.h,v 1.15.24.2 2011/08/10 14:53:28 nicholasmarriott Exp $
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

#ifndef MamaSymbolListH__
#define MamaSymbolListH__

/* *************************************************** */
/* Includes. */
/* *************************************************** */
#include "mama/mama.h"
#include <mama/config.h>
#include "mama/symbollisttypes.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* *************************************************** */
/* Type Defines. */
/* *************************************************** */

/**
 * Prototype for add symbol callback. 
 * The registered add symbol callback is executed for each 
 * symbol added to the symbol list.
 *
 * @param symbol   The symbol to be added to the list
 * @param userData User-data to be associated with symbol
 */
typedef mama_status (MAMACALLTYPE *addSymbolCbType) (
  mamaSymbolListMember  symbol, 
  void*                 closure);

/**
 * Function invoked when completing the iteration over the symbol list
 * using mamaSymbolList_iterate().
 */
typedef void (MAMACALLTYPE *mamaSymbolListIterateCompleteFunc) (
    mamaSymbolList        symbolList,
    void*                 closure);

/**
 * Function invoked for each member of the symbol list when iterating
 * using mamaSymbolList_iterate().
 */
typedef void (MAMACALLTYPE *mamaSymbolListIterateMemberFunc) (
    mamaSymbolList        symbolList,
    mamaSymbolListMember  member,
    void*                 closure);

/**
 * Prototype for remove symbol callback.
 * The registered remove symbol callback is executed just prior 
 * to a symbol being removed from the symbol list.
 *
 * @param symbol   The symbol to be removed from the list
 * @param userData User-data previously associated with symbol
 */
typedef mama_status (MAMACALLTYPE *removeSymbolCbType) (
  mamaSymbolListMember  symbol, 
  void*                 closure);

/* *************************************************** */
/* Public Function Prototypes. */
/* *************************************************** */

/**
 * Add a symbol member to the symbol list, this will cause the add callback to be invoked if it
 * has been installed.
 *
 * @param[in] symbolList The symbolList.
 * @param[in] member The symbol member.
 *
 * @return mama_status return code can be one of
 *              MAMA_STATUS_NULL_ARG 
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSymbolList_addMember(
    mamaSymbolList symbolList,
    mamaSymbolListMember member);

/**
 * Allocate and initialize memory for a new symbolList, mamaSymbolList_deallocate should
 * be called on the returned symbol list.
 *
 * @param[out] symbolList The address of the allocated symbolList.
 *
 * @return mama_status return code can be one of
 *              MAMA_STATUS_NOMEM
 *              MAMA_STATUS_NULL_ARG 
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSymbolList_allocate(
    mamaSymbolList *symbolList);

/**
 * Allocate a symbolListMember. 
 *
 * @param[in] symbolList  The symbolList the member to be allocated in.
 *
 * @return Pointer to the new member.
 */
MAMAExpDLL
extern mamaSymbolListMember 
mamaSymbolList_allocateMember(
    mamaSymbolList symbolList);

/**
 * Clear the symbol list.
 *
 * @param[in] symbolList  The symbolList.
 * @param[in] membersToo Whether to also clear all members of the list.
 *
 * @return mama_status return code can be one of
 *              MAMA_STATUS_NULL_ARG 
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSymbolList_clear(
    mamaSymbolList   symbolList,
    int              membersToo);

/**
 * Free a symbol list created by mamaSymbolList_allocate.
 *
 * @param[in] symbolList The symbolList to be deallocated.
 *
 * @return mama_status return code can be one of
 *              MAMA_STATUS_NULL_ARG 
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSymbolList_deallocate(
    mamaSymbolList symbolList);

/**
 * Deallocate a symbolListMember. 
 *
 * @param[in] symbolList The symbolList the member belongs to.
 * @param[in] member The member of the list to deallocate.
 *
 * @return mama_status return code can be one of
 *              MAMA_STATUS_NULL_ARG 
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status 
mamaSymbolList_deallocateMember (
    mamaSymbolList symbolList,
    mamaSymbolListMember member);

/** 
 * Free the memory for the symbolList and
 * all members.
 * 
 * @param symbolList  The symbolList to be deallocated.
 *
 * @return mama_status return code can be one of
 *              MAMA_STATUS_NULL_ARG 
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSymbolList_deallocateWithMembers(
    mamaSymbolList  symbolList);

/**
 * Find a symbol member in the symbol list.
 *
 * @param[in] symbolList  The symbolList.
 * @param[in] symbol      The name of the symbol to find.
 * @param[in] source      The source of the symbol to find.
 * @param[in] transport   The tport  of the symbol to find.
 * @param[out] member      The return symbol member (set to NULL if not found).
 *
 * @return mama_status return code can be one of
 *              MAMA_STATUS_INVALID_ARG - the member can't be found.
 *              MAMA_STATUS_NULL_ARG 
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSymbolList_findMember(
    const mamaSymbolList   symbolList,
    const char*            symbol,
    const char *           source,
    mamaTransport          transport,
    mamaSymbolListMember*  member);

/**
 * Get the closure associated with the this symbolList
 *
 * @param[in] symbolList  The symbolList.
 * @param[out] closure     The closure
 *
 * @return mama_status return code can be one of
 *              MAMA_STATUS_NULL_ARG 
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSymbolList_getClosure(
    const mamaSymbolList  symbolList, 
    void**                closure);

/**
 * Get the size of the symbolList
 *
 * @param[in] symbolList The symbolList.
 * @param[in] size The size
 *
 * @return mama_status value can be one of:
 *          MAMA_STATUS_NULL_ARG
 *          MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSymbolList_getSize(
    const mamaSymbolList symbolList, 
    unsigned long *size); 

/**
 * Iterate over the symbol list.  The "handler" function will be
 * invoked for each
 *
 * @param[in] symbolList  The symbolList.
 * @param[in] memberFunc  The function invoked for each symbol list member.
 * @param[in] completeFunc  The function invoked upon completion.
 * @param[in] iterateClosure The closure passed to each callback
 *
 * @return mama_status return code can be one of
 *              MAMA_STATUS_NULL_ARG 
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSymbolList_iterate(
    mamaSymbolList                      symbolList,
    mamaSymbolListIterateMemberFunc     memberFunc,
    mamaSymbolListIterateCompleteFunc   completeFunc,
    void*                               iterateClosure);

/**
 * Remove a symbol member from the symbol list.
 *
 * @param[in] symbolList  The symbolList.
 * @param[in] symbol      The name of the symbol to be removed.
 * @param[in] source      The source of the symbol to be removed.
 * @param[in] transport   The tport of the symbol to be removed.
 * @param[out] member The return symbol member (set to NULL if not found).
 *                    Note that this will be freed by the symbol list and should
 *                    be used for reference only.
 *
 * @return mama_status return code can be one of
 *              MAMA_STATUS_INVALID_ARG - the member can't be found.
 *              MAMA_STATUS_NULL_ARG 
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSymbolList_removeMember (
    mamaSymbolList   symbolList,
    const char*            symbol,
    const char *           source,
    mamaTransport          transport,
    mamaSymbolListMember*  member);

/**
 * Remove a symbol member from the symbol list.
 *
 * @param symbolList  The symbolList.
 * @param member      The symbol to be removed.
 *
 * @return mama_status return code can be one of
 *              MAMA_STATUS_NULL_ARG 
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status mamaSymbolList_removeMemberByRef(
  mamaSymbolList symbolList,
  mamaSymbolListMember member);

/**
 * Registers the user defined add symbol callback with the symbolList.
 * The registered callback will get called each time a symbol is added to 
 * the symbol list.
 *
 * @param[in] symbolList  The symbolList.
 * @param[in] addCb       Pointer to the user defined callback. Must conform to 
 *                        function prototype <code>addSymbolCbType</code>.
 *
 * @return mama_status return code can be one of
 *              MAMA_STATUS_NULL_ARG 
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status 
mamaSymbolList_setAddSymbolHandler(
    mamaSymbolList   symbolList, 
    addSymbolCbType  addCb);

/**
 * Set the closure associated with the this symbolList
 *
 * @param[in] symbolList  The symbolList.
 * @param[in] closure     The closure
 *
 * @return mama_status return code can be one of
 *              MAMA_STATUS_NULL_ARG 
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaSymbolList_setClosure(
    mamaSymbolList  symbolList, 
    void*           closure);

/**
 * Registers the user defined remove symbol callback with the symbolList.
 * The registered callback will get called each time a symbol is deleted 
 * from the symbol list.
 *
 * @param[in] symbolList  The symbolList.
 * @param[in] removeCb    Pointer to the user defined callback. Must conform to 
 *                        function prototype <code>removeSymbolCbType</code>.
 *
 * @return mama_status return code can be one of
 *              MAMA_STATUS_NULL_ARG 
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status 
mamaSymbolList_setRemoveSymbolHandler(
    mamaSymbolList      symbolList, 
    removeSymbolCbType  removeCb);

#if defined(__cplusplus)
}
#endif

#endif

