/* $Id: ft.h,v 1.1.2.3 2011/09/01 16:34:38 emmapollock Exp $
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

#ifndef MamaFtH__
#define MamaFtH__

#include <mama/mama.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum mamaFtState_
{
    MAMA_FT_STATE_STANDBY  = 0,
    MAMA_FT_STATE_ACTIVE   = 1,
    MAMA_FT_STATE_UNKNOWN  = 99
} mamaFtState;

typedef enum mamaFtType_
{
    MAMA_FT_TYPE_MULTICAST = 1,
    MAMA_FT_TYPE_BRIDGE	   = 2,
    MAMA_FT_TYPE_MAX	   = 3
} mamaFtType;

typedef void* mamaFtMember;


/**
 * Callback function type for MAMA fault tolerance events.
 */
typedef void (
MAMACALLTYPE
*mamaFtMemberOnFtStateChangeCb) (mamaFtMember  ftMember,
                                               const char*   groupName,
                                               mamaFtState   state,
                                               void*         closure);

/**
 * Allocate a MAMA fault tolerance group member.  This function is
 * typically followed by mamaFtMember_setup() and
 * mamaFtMember_activate().
 */
MAMAExpDLL
extern mama_status
mamaFtMember_create (
    mamaFtMember*  member);

/**
 * Set up a MAMA fault tolerance group member.  This is only an
 * initialization function.  In order to actually start the fault
 * tolerance monitoring, use mamaFtMember_activate().
 */
MAMAExpDLL
extern mama_status
mamaFtMember_setup (
    mamaFtMember                   member,
    mamaFtType                     fttype,
    mamaQueue                      eventQueue,
    mamaFtMemberOnFtStateChangeCb  callback,
    mamaTransport                  transport,
    const char*                    groupName,
    mama_u32_t                     weight,
    mama_f64_t                     heartbeatInterval,
    mama_f64_t                     timeoutInterval,
    void*                          closure);

/**
 * Create a MAMA fault tolerance group member.
 */
MAMAExpDLL
extern mama_status
mamaFtMember_destroy (
    mamaFtMember    member);

/**
 * Activate the MAMA fault tolerance group member.
 */
MAMAExpDLL
extern mama_status
mamaFtMember_activate (
    mamaFtMember    member);

/**
 * Deactivate the MAMA fault tolerance group member.
 */
MAMAExpDLL
extern mama_status
mamaFtMember_deactivate (
    mamaFtMember    member);

/**
 * Get whether the MAMA fault tolerance member is actively running
 * (not related to its state).  Use mamaFtMember_getState() to
 * determine the actual state of the member.
 */
MAMAExpDLL
extern mama_status
mamaFtMember_isActive (
    const mamaFtMember  member,
    int*                result);

/**
 * Get the group name to which this MAMA FT member belongs.
 */
MAMAExpDLL
extern mama_status
mamaFtMember_getGroupName (
    const mamaFtMember  member,
    const char**        result);

/**
 * Get the fault tolerance weight of the MAMA FT member.
 */
MAMAExpDLL
extern mama_status
mamaFtMember_getWeight (
    const mamaFtMember  member,
    mama_u32_t*         result);

/**
 * Get the fault tolerance heartbeat interval of the MAMA FT member.
 */
MAMAExpDLL
extern mama_status
mamaFtMember_getHeartbeatInterval (
    const mamaFtMember  member,
    mama_f64_t*         result);

/**
 * Get the fault tolerance timeout interval of the MAMA FT member.
 */
MAMAExpDLL
extern mama_status
mamaFtMember_getTimeoutInterval (
    const mamaFtMember  member,
    mama_f64_t*         result);

/**
 * Get the closure argument (provided in the mamaFtMember_create()
 * function) of the MAMA FT member.
 */
MAMAExpDLL
extern mama_status
mamaFtMember_getClosure (
    const mamaFtMember  member,
    void**              result);

/**
 * Set the fault tolerance weight of the MAMA FT member.  The FT
 * weight can be changed dynamically, if desired.  The member with the
 * highest weight will become the active member.
 */
MAMAExpDLL
extern mama_status
mamaFtMember_setWeight (
    mamaFtMember  member,
    mama_u32_t    value);

/**
 * Set the instance ID of the MAMA FT member.  The instance ID is used
 * to uniquely identify members of a fault tolerant group.  Most
 * applications should allow the MAMA API to automatically set the
 * instance ID and this function would not be called.  If not set
 * explicitly before activation, then the instance ID is automatically
 * set according to the following format:
 *    {group-name}.{hex-ip-addr}.{hex-pid}.
 * For example, if the group name is "FOO", the IP address is
 * 192.168.187.9, and the PId is 10777, the default instance ID would be:
 *    FOO.c0a8bb9.2a19
 */
MAMAExpDLL
extern mama_status
mamaFtMember_setInstanceId (
    mamaFtMember  member,
    const char*   id);

/**
 * Convert a fault tolerant state to a string.
 *
 * @param state The state to convert.
 */
MAMAExpDLL
extern const char*
mamaFtStateToString (mamaFtState state);


#if defined(__cplusplus)
}
#endif

#endif /* MamaFtH  __ */
