/* $Id: io.h,v 1.10.32.3 2011/08/10 14:53:28 nicholasmarriott Exp $
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
 * IO Types. Not all implementation support all mamaIoTypes.
 *
 * MAMA_IO_READ: the socket is readable.
 * MAMA_IO_WRITE: the socket is writable.
 * MAMA_IO_CONNECT: the socket is connected
 * MAMA_IO_ACCEPT: the socket accepted a connection
 * MAMA_IO_CLOSE: the socket was closed
 * MAMA_IO_ERROR: an error occurred
 * MAMA_IO_EXCEPT: An exceptional event like out of band data occurred.
 */
typedef enum
{
    MAMA_IO_READ,
    MAMA_IO_WRITE,
    MAMA_IO_CONNECT,
    MAMA_IO_ACCEPT,
    MAMA_IO_CLOSE,
    MAMA_IO_ERROR,
    MAMA_IO_EXCEPT 
} mamaIoType;

/**
 * Prototype for callback invoked by IO handler.
 *
 * @param io The mamaIo handle.
 * @param ioType The mamaIoType for the event.
 * @param closure Caller supplied closure.
 */
typedef void (MAMACALLTYPE *mamaIoCb) (mamaIo     io,
                                       mamaIoType ioType,
                                       void*      closure); 

/**
 * Create a IO handler.
 *
 * If the underlying infrastructure does not support the requested mamaIoType,
 * mamaIo_create returns MAMA_STATUS_UNSUPPORTED_IO_TYPE. For example RV only
 * supports READ, WRITE, and EXCEPT. LBM supports all types except ERROR.
 *
 * @param result A pointer to the io handle.
 * @param queue The event queue for the io events. NULL specifies the Mama
 *  default queue.
 * @param action The callback to be invoked when an event occurs.
 * @param descriptor Wait for IO on this descriptor.
 * @param ioType Wait for occurrences of this type.
 * @param closure The closure that is passed to the callback.
 */
MAMAExpDLL extern
mama_status mamaIo_create (mamaIo*    result,
                           mamaQueue  queue,
                           uint32_t   descriptor,
                           mamaIoCb   action, 
                           mamaIoType ioType,
                           void*      closure);

/**
 * Get the descriptor.
 */
MAMAExpDLL extern
mama_status mamaIo_getDescriptor (mamaIo io, uint32_t* d);

/**
 * Destroy the IO.
 */
MAMAExpDLL extern
mama_status mamaIo_destroy (mamaIo io);


#if defined( __cplusplus )
}
#endif

#endif /* MamaIoH__ */
