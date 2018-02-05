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

#ifndef MamaFtH__
#define MamaFtH__

#include <mama/mama.h>

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * The enumeration of fault tolerant states
 */
typedef enum mamaFtState_
{
    MAMA_FT_STATE_STANDBY  = 0, /**< The mamaFtMember is currently not
                                     the highest weighted member in the
                                     group */
    MAMA_FT_STATE_ACTIVE   = 1, /**< The mamaFtMember is currently the
                                     highest weighted member in the
                                     group */
    MAMA_FT_STATE_UNKNOWN  = 99 /**< The mamaFtMember has yet to
                                     establish its state */
} mamaFtState;

/**
 * The enumeration of fault tolerant types
 */
typedef enum mamaFtType_
{
    MAMA_FT_TYPE_MULTICAST = 1, /**< MAMA creates sockets directly for
                                     sending and receiving heartbeats.
                                     The middleware is not used */
    MAMA_FT_TYPE_BRIDGE    = 2, /**< Use the middleware and MAMA
                                     subscriptions for sending and
                                     receiving heartbeats */
    MAMA_FT_TYPE_MAX       = 3 /**< Used internally */
} mamaFtType;

typedef void* mamaFtMember;


/**
 * @brief Callback function type for MAMA fault tolerant state change
 *        events.
 *
 * @param[in] ftMember  The mamaFtMember to which the callback event
 *                      applies
 * @param[in] groupName The group name of the mamaFtMember for which the
 *                      fault tolerant event applies
 * @param[in] mamaFtState The new fault tolerant state for the group
 *                        member
 * @param[in] closure The closure passed to the mamaFtMember when
 *                    mamaFtMember_setup() is called
 */
typedef void (
MAMACALLTYPE
*mamaFtMemberOnFtStateChangeCb) (mamaFtMember  ftMember,
                                               const char*   groupName,
                                               mamaFtState   state,
                                               void*         closure);

/**
 * @brief Allocate a MAMA fault tolerance group member.  This function is
 * typically followed by mamaFtMember_setup() and
 * mamaFtMember_activate().
 *
 * @param[out] member A pointer to a mamaFtMember.  When the function is
 *                    called successfully the ftMember will be a pointer
 *                    to a block of allocated memory.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NOMEM
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFtMember_create (
    mamaFtMember*  member);

/**
 * @brief Set up a MAMA fault tolerance group member.  This is only an
 * initialization function.  In order to actually start the fault
 * tolerance monitoring, use mamaFtMember_activate().
 *
 * @param[in] member The mamaFtMember to be initialized
 * @param[in] fttype The type of fault tolerance to be used for the
 *            member
 * @param[in] eventQueue The queue on which the mamaFtMember sends and
 *            receives heartbeats
 * @param[in] transport The mamaTransport used for sending and receiving
 *            heartbeats
 * @param[in] groupName The group name for the mamaFtMember
 * @param[in] weight The initial weight of the mamaFtMember
 * @param[in] heartbeatInterval The time in seconds between sending
 *            heartbeats
 * @param[in] timeoutInterval The time in seconds that a mamaFtMember in
 *            state MAMA_FT_STATE_STANBY or MAMA_FT_STATE_UNKNOWN will
 *            wait to receive a heartbeat before changing state to
 *            MAMA_FT_STATE_ACTIVE
 * @param[in] closure The closure will be passed to subsequent callback
 *            invocations for thie mamaFtMember
 *
 * @return mama_status MAMA_STATUS_OK will be returned on success,
 *         otherwise a mama_status code indicating failure will be
 *         returned
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
 * @brief Destroy a MAMA fault tolerance group member.
 *
 * @param[in] member The mamaFtMember.  When the function returns, memory
 *                   previously allocated for the member will have been
 *                   freed
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFtMember_destroy (
    mamaFtMember    member);

/**
 * @brief Activate the MAMA fault tolerance group member.  The member
 *        will begin to send and receive heartbeats.  If the member is
 *        already active, this function call will deactivate it
 *
 * @param[in] member The mamaFtMember
 *
 * @return mama_status MAMA_STATUS_OK will be returned on success,
 *         otherwuse a mama_status code indicating failure will be
 *         returned
 */
MAMAExpDLL
extern mama_status
mamaFtMember_activate (
    mamaFtMember    member);

/**
 * @brief Deactivate the MAMA fault tolerance group member.  The member
 *        will stop sending and listening for heartbeats.
 *
 * @param[in] member the mamaFtMember
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAAM_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFtMember_deactivate (
    mamaFtMember    member);

/**
 * @brief Get whether the MAMA fault tolerance member is actively running
 * (not related to its state).  Use mamaFtMember_getState() to
 * determine the actual state of the member.
 *
 * @param[in] member The mamaFtMember
 * @param[out] result This will be set to 1 if the member is currently
 *             active, 0 if it is not
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFtMember_isActive (
    const mamaFtMember  member,
    int*                result);

/**
 * @brief Get the name of the group to which this MAMA FT member belongs.
 *
 * @param[in] member The mamaFtMember
 * @param[out] result When the function is called successfully, this
 *             will point to the member's group name
 *
 * @result mama_status return code can be on of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFtMember_getGroupName (
    const mamaFtMember  member,
    const char**        result);

/**
 * @brief Get the fault tolerance weight of the MAMA FT member.
 *
 * @param[in] member The mamaFtMember
 * @param[out] result When the function is called successfully, this
 *             will store the value of the member's weight
 *
 * @result mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFtMember_getWeight (
    const mamaFtMember  member,
    mama_u32_t*         result);

/**
 * @brief Get the fault tolerance heartbeat interval of the MAMA FT
 *        member.
 * @param[in] member The mamaFtMember
 * @param[out] result When the function is called successfully, this will
 *             store the value of the member's heartbeat interval
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFtMember_getHeartbeatInterval (
    const mamaFtMember  member,
    mama_f64_t*         result);

/**
 * @brief Get the fault tolerance timeout interval of the MAMA FT member.
 *
 * @param[in] member The mamaFtMember
 * @param[out] result When the function is called successfully, this will
 *             store the value of the member's timeout interval
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFtMember_getTimeoutInterval (
    const mamaFtMember  member,
    mama_f64_t*         result);

/**
 * Get the current heartbeat tick of the MAMA FT member.
 */
MAMAExpDLL
extern mama_status
mamaFtMember_getHeartbeatTick (
    const mamaFtMember  member,
    mama_u32_t*         result);

/**
 * @brief Get the closure argument (provided in the mamaFtMember_setup()
 * function) of the MAMA FT member.
 *
 * @param[in] member The mamaFtMember
 * @param[out] result When the function is called successfully, this will
 *                    point to member's closure
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFtMember_getClosure (
    const mamaFtMember  member,
    void**              result);

/**
 * @brief Set the fault tolerance weight of the MAMA FT member.  The FT
 * weight can be changed dynamically, if desired.  The member with the
 * highest weight will become the active member.
 *
 * @param[in] member The mamaFtMember
 * @param[in] value The new fault tolerance weight
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFtMember_setWeight (
    mamaFtMember  member,
    mama_u32_t    value);

/**
 * @brief Set the instance ID of the MAMA FT member.  The instance ID
 *        is used to uniquely identify members of a fault tolerant group.
 *
 * @details Most applications should allow the MAMA API to automatically
 *        set the instance ID and this function would not be called.  If
 *        not set explicitly before activation, then the instance ID is
 *        automatically set according to the following format:
 *    {group-name}.{hex-ip-addr}.{hex-pid}.
 * For example, if the group name is "FOO", the IP address is
 *        192.168.187.9, and the PId is 10777, the default instance ID
 *        would be:
 *    FOO.c0a8bb9.2a19
 *
 * @param[in] member The mamaFtMember
 * @param[in] The new instance ID.  This string is copied by the function
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_NOMEM
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mamaFtMember_setInstanceId (
    mamaFtMember  member,
    const char*   id);

/**
 * @brief Convert a fault tolerant state to a string.
 *
 * @param[in] state The state to convert.
 *
 * @return const char* A string representation of the state.  This will
 *         be either "standby", "active", "unknown", or "error" in the
 *         case where the current state is unrecognised
 */
MAMAExpDLL
extern const char*
mamaFtStateToString (mamaFtState state);


#if defined(__cplusplus)
}
#endif

#endif /* MamaFtH  __ */
