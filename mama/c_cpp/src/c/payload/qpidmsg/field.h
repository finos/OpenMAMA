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

#ifndef MAMA_PAYLOAD_QPIDMSG_FIELD_H__
#define MAMA_PAYLOAD_QPIDMSG_FIELD_H__


/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include "qpidcommon.h"

/*=========================================================================
  =                   Public implementation prototypes                    =
  =========================================================================*/

/**
 * Use this item for setting array sizes for vector arrays (msg vectors). This
 * should only get called when about to populate a data vector, so this will
 * also destroy the previous contents before population.
 *
 * @param impl       The qpid message field payload implementation to set the
 *                   data vector size for.
 * @param size       The new number of elements in the data vector used.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgFieldPayloadImpl_setDataVectorSize (qpidmsgFieldPayloadImpl* impl,
                                           mama_size_t size);

/**
 * Use this item for setting array sizes for atom arrays (scalar vectors). This
 * will resize the payload's atom buffer if required.
 *
 * @param impl  The qpid message field payload implementation to set the new
 *              data array size for.
 * @param size  The new number of elements in the data array used.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgFieldPayloadImpl_setDataArraySize (qpidmsgFieldPayloadImpl* impl,
                                          mama_size_t size);

#endif /* MAMA_PAYLOAD_QPIDMSG_FIELD_H__ */
