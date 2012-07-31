/* $Id: stat.h,v 1.7.22.4 2011/08/10 14:53:28 nicholasmarriott Exp $
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

#ifndef MamaStatsH__
#define MamaStatsH__

#include "mama/status.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define MAMA_STAT_NOT_LOCKABLE 0
#define MAMA_STAT_LOCKABLE     1

typedef enum mamaStatType
{
    MAMA_STAT_TYPE_INITIALS                      =  105,
    MAMA_STAT_TYPE_RECAPS                        =  106,
    MAMA_STAT_TYPE_NUM_MESSAGES                  =  107,
    MAMA_STAT_TYPE_FT_TAKEOVERS                  =  108,
    MAMA_STAT_TYPE_QUEUE_SIZE                    =  109,
    MAMA_STAT_TYPE_SUBSCRIPTIONS                 =  110,
    MAMA_STAT_TYPE_TIMEOUTS                      =  111,
    MAMA_STAT_TYPE_MSG_INDEX                     =  112,
    MAMA_STAT_TYPE_NAK_PACKETS_SENT              =  113,
    MAMA_STAT_TYPE_NAKS_SENT                     =  114,
    MAMA_STAT_TYPE_MSGS_LOST                     =  115,
    MAMA_STAT_TYPE_NCFS_IGNORE                   =  116,
    MAMA_STAT_TYPE_NCFS_SHED                     =  117,
    MAMA_STAT_TYPE_NCFS_RX_DELAY                 =  118,
    MAMA_STAT_TYPE_NCFS_UNKNOWN                  =  119,
    MAMA_STAT_TYPE_DUPLICATE_MSGS                =  120,
    MAMA_STAT_TYPE_UNRECOVERABLE_WINDOW_ADVANCE  =  121,
    MAMA_STAT_TYPE_UNRECOVERABLE_TIMEOUT         =  122,
    MAMA_STAT_TYPE_LBM_MSGS_RECEIVED_NO_TOPIC    =  123,
    MAMA_STAT_TYPE_LBM_REQUESTS_RECEIVED         =  124,
    MAMA_STAT_TYPE_WOMBAT_MSGS                   =  125,
    MAMA_STAT_TYPE_RV_MSGS                       =  126,
    MAMA_STAT_TYPE_FAST_MSGS                     =  127,
    MAMA_STAT_TYPE_UNKNOWN                       =  999
}mamaStatType;

/**
 * Create a mamaStats object
 *
 * @param stat  The location of a mamaStats to store the result
 */
MAMAExpDLL
extern mama_status
mamaStat_create (mamaStat* stat, mamaStatsCollector* collector, int lockable, const char* name, mama_fid_t type);

/**
 * Destroy a mamaStats object
 *
 * @param stat  The stats object to destroy
 */
MAMAExpDLL
extern mama_status
mamaStat_destroy (mamaStat stat);

/**
 * Increment the stats object counter
 *
 * @param stat  The stats object to increment
 */
MAMAExpDLL
extern mama_status
mamaStat_increment (mamaStat stat);

/**
 * Decrement the stats object counter
 *
 * @param stat  The stats object to decrement
 */
MAMAExpDLL
extern mama_status
mamaStat_decrement (mamaStat stat);

/**
 * Reset the stats object counter
 *
 * @param stat  The stat object to reset
 */
MAMAExpDLL
extern mama_status
mamaStat_reset (mamaStat stat);

/**
 * Get the FID used when publishing the
 * stat via the stats logger
 *
 * @param stat  The stat object from which
 *              to get the FID
 */
MAMAExpDLL
extern mama_fid_t
mamaStat_getFid (mamaStat stat);

/**
 * Get the value of the stat for the current
 * interval
 *
 * @param stat  The stat object from which to
 *              get the invterval value
 */
MAMAExpDLL
extern int
mamaStat_getIntervalValue (mamaStat stat);

/**
 * Get the maximum value of the stat
 *
 * @param stat  The stat object from which
 *              to get the maximum value
 */
MAMAExpDLL
extern int
mamaStat_getMaxValue (mamaStat stat);

/**
 * Get the total value of the stat
 *
 * @param stat  The stat object from which
 *              to get the total value
 */
MAMAExpDLL
extern int
mamaStat_getTotalValue (mamaStat stat);

/**
 * Get the interval, maximum, and total values
 * for the stat
 *
 * @param stat           The stat object from
 *                       which to get the values
 * @param intervalValue  Address to which the
 *                       interval value will
 *                       be written
 * @param maxValue       Address to which the
 *                       maximum value will
 *                       be written
 * @param totalValue     Address to which the
 *                       total value will be
 *                       written
 */
MAMAExpDLL
extern void
mamaStat_getStats (mamaStat stat, mama_u32_t* intervalValue, mama_u32_t* maxValue, mama_u32_t* totalValue);

/**
 * Get the name of the stat
 *
 * @param  The stat object from which to get the name
 */
MAMAExpDLL
extern const char*
mamaStat_getName (mamaStat stat);

/**
 * Set whether or not this stat should be logged in the MAMA log.
 *
 * @param stat  The stat object to set logging for
 * @param log   Whether or not to log the stat
 */
MAMAExpDLL
extern mama_status
mamaStat_setLog (mamaStat stat, int log);

/**
 * Returns whether or not the stat is currently being logged to
 * the MAMA log.
 *
 * @param  The stat object to get logging for
 */
MAMAExpDLL
extern int
mamaStat_getLog (mamaStat stat);

/**
 * Set whether or not to publish this stat.
 *
 * @parameter stat     The stat object to set publishing for
 * @paramter  publish  Whether or not to publish the stat
 */
MAMAExpDLL
extern mama_status
mamaStat_setPublish (mamaStat stat, int publish);

/**
 * Returns whether or not the stat is currently being published.
 *
 * @param stat  The stat object to check whether or not it is being
 *              published
 */
MAMAExpDLL
extern int
mamaStat_getPublish (mamaStat stat);

/**
 * Returns a mamaStatType from a string.
 *
 * @param statTypeString  The stat type as a string
 */
MAMAExpDLL
extern mamaStatType
mamaStatType_fromString (const char* statTypeString);

/**
 * Returns a string representation of a mamaStatType.
 *
 * @param statType  The statType to return as a string
 */
MAMAExpDLL
extern const char*
mamaStatType_toString (mamaStatType statType);

#if defined(__cplusplus)
}
#endif

#endif /* MamaStatsH__ */
