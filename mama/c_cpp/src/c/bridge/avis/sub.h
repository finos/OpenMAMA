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

#ifndef MAMA_BRIDGE_AVIS_SUBSCRIPTION_H__
#define MAMA_BRIDGE_AVIS_SUBSCRIPTION_H__

#if defined(__cplusplus)
extern "C" {
#endif

/*=========================================================================
  =                  Public implementation functions                      =
  =========================================================================*/

/**
 * This function will generate a string which is unique to the root, source
 * and topic provided. Centralization of this function means that it can be used
 * in both the publisher and the subscriber in order to generate a consistent
 * topic for use throughout the platform.
 *
 * @param root      Prefix to associate with the subject (e.g. _MDDD)
 * @param inbox     Source to base the subject key on (e.g. EXCHANGENAME).
 * @param topic     Topic to base the subject key on (e.g. ISIN.CURRENCY).
 * @param keyTarget Pointer to populate with the generated subject key.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
avisBridgeMamaSubscriptionImpl_generateSubjectKey (const char* root,
                                                   const char* source,
                                                   const char* topic,
                                                   char**      keyTarget);

#if defined(__cplusplus)
}
#endif

#endif /* MAMA_BRIDGE_AVIS_SUBSCRIPTION_H__ */
