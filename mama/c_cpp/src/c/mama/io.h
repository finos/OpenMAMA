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

#ifndef MamaIoH__
#define MamaIoH__

#include <mama/types.h>
#include <mama/status.h>

#if defined( __cplusplus )
extern "C"
{
#endif

/**
 * @brief IO Types.
 *
 * @details Note not all implementations support all mamaIoTypes.
 */
typedef enum
{
    MAMA_IO_READ,       /**< the socket is readable */
    MAMA_IO_WRITE,      /**< the socket is writable */
    MAMA_IO_CONNECT,    /**< the socket is connected */
    MAMA_IO_ACCEPT,     /**< the socket accepted a connection */
    MAMA_IO_CLOSE,      /**< the socket was closed */
    MAMA_IO_ERROR,      /**< an error occurred */
    MAMA_IO_EXCEPT      /**< An exceptional event like out of band data occurred */
} mamaIoType;

/**
 * @brief Prototype for callback invoked by IO handler.
 *
 * @param[in] io The mamaIo handle.
 * @param[in] ioType The mamaIoType for the event.
 * @param[in] closure Caller supplied closure.
 */
typedef void (MAMACALLTYPE *mamaIoCb) (mamaIo     io,
                                       mamaIoType ioType,
                                       void*      closure);

/**
 * @brief Create a IO handler.
 *
 * @details If the underlying infrastructure does not support the requested mamaIoType,
 * mamaIo_create returns MAMA_STATUS_UNSUPPORTED_IO_TYPE. For example RV only
 * supports READ, WRITE, and EXCEPT. LBM supports all types except ERROR.
 *
 * @param[in] result A pointer to the io handle.
 * @param[in] queue The event queue for the io events. NULL specifies the Mama
 *  default queue.
 * @param[in] action The callback to be invoked when an event occurs.
 * @param[in] descriptor Wait for IO on this descriptor.
 * @param[in] ioType Wait for occurrences of this type.
 * @param[in] closure The closure that is passed to the callback.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_INVALID_QUEUE
 *              MAMA_STATUS_NO_BRIDGE_IMPL
 *              MAMA_STATUS_OK
 */
MAMAExpDLL extern
mama_status mamaIo_create (mamaIo*    result,
                           mamaQueue  queue,
                           uint32_t   descriptor,
                           mamaIoCb   action,
                           mamaIoType ioType,
                           void*      closure);

/**
 * @brief Get the IO descriptor.
 *
 * @param[in] io The mamaIo handle.
 *
 * @param[out] d The associated descriptor for the mamaIO handle.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_NO_BRIDGE_IMPL
 */
MAMAExpDLL extern
mama_status mamaIo_getDescriptor (mamaIo io, uint32_t* d);

/**
 * @brief Destroy the IO.
 *
 * @param[in] io The mamaIo handle to be destroyed.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_NO_BRIDGE_IMPL
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL extern
mama_status mamaIo_destroy (mamaIo io);


#if defined( __cplusplus )
}
#endif

#endif /* MamaIoH__ */
