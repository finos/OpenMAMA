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

#ifndef MAMA_PAYLOAD_QPIDMSG_PAYLOAD_H__
#define MAMA_PAYLOAD_QPIDMSG_PAYLOAD_H__


/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include "qpidcommon.h"


/*=========================================================================
  =                   Public implementation prototypes                    =
  =========================================================================*/

/**
 * This function exists to centralize the allocation of buffer memory. When
 * called for the first time with an empty buffer, it will calloc the requested
 * memory. When requesting a size smaller than the current size, it will do
 * nothing. When requesting a size larger than the current size, it will perform
 * a realloc. On success, MAMA_STATUS_OK will be returned, and if necessary,
 * the buffer pointer will be written back to reflect the new value. The
 * currentSize pointer will also be modified to reflect the new size. On
 * failure, a mama_status will be returned indicating an error and buffer and
 * currentSize will remain unchanged.
 *
 * @param buffer      Pointer to the buffer to examine and resize if necessary.
 * @param size        Current size of the buffer. Will be updated with the newly
 *                    requested newSize on success.
 * @param newSize     The newly requested buffer size.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayloadImpl_allocateBufferMemory      (void**                   buffer,
                                              mama_size_t*             size,
                                              mama_size_t              newSize);

/**
 * This function examines the current proton buffer position to correctly
 * populate a qpidmsgFieldPayloadImpl field object.
 *
 * @param buffer      Pointer to the buffer to examine for content.
 * @param target      Field payload to populate with the field found.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayloadImpl_getFieldFromBuffer        (pn_data_t*               buffer,
                                              qpidmsgFieldPayloadImpl* target);

/**
 * This will rewind the provided buffer, step inside the list which is used in
 * the MAMA qpid payload and point to the first element inside.
 *
 * @param buffer      Pointer to the data element so it can be rewound.
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayloadImpl_moveDataToContentLocation (pn_data_t*               buffer);

/**
 * This will rewind the provided buffer, step inside the list which is used in
 * the MAMA qpid payload and step over the top of ever top level field to reach
 * the end of the proton message where new elements are inserted.
 *
 * @param buffer      Pointer to the data element to step through and reach the
 *                    end of.
 * @param impl        Payload implementation which is referenced in order to
 *                    determine whether or not inserts should be inline rather
 *                    than appended to the end (e.g. when inserting fields which
 *                    are members of a vector message).
 *
 * @return mama_status indicating whether the method succeeded or failed.
 */
mama_status
qpidmsgPayloadImpl_moveDataToInsertLocation  (pn_data_t*                buffer,
                                              qpidmsgPayloadImpl*       impl);

#endif /* MAMA_PAYLOAD_QPIDMSG_PAYLOAD_H */
