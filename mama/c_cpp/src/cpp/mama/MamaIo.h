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

#ifndef MAMA_IO_CPP_H__
#define MAMA_IO_CPP_H__

#include "mama/mamacpp.h"

namespace Wombat 
{

    class MamaIoImpl;

    /**
     * A repeating IO. The callback will be repeatedly called at the specified
     * interval until the IO is destroyed. See Mama::createMamaIo ().
     *
     * The IO relies on the underlying middleware so its resolution is also
     * dependent on the middleware.
     *
     */
    class MAMACPPExpDLL MamaIo
    {
    public:
        virtual ~MamaIo (void);
        
        MamaIo (void);
        
       /**
         * Create an IO handler.
         *
         * @param queue The event queue for the io events. NULL specifies the Mama
         *  default queue.
         * @param action The callback to be invoked when an event occurs.
         * @param descriptor Wait for IO on this descriptor.
         * @param ioType Wait for occurrences of this type. See mama/io.h
         * @param closure The caller supplied closure.
         */
        virtual void create (MamaQueue*        queue,
                             MamaIoCallback*   action, 
                             uint32_t          descriptor,
                             mamaIoType        ioType,
                             void*             closure = NULL);

        virtual uint32_t getDescriptor (void) const;
        virtual void     destroy       ();

        /**
         * Return the closure for the IO.
         *
         * @return the closure.
         */
        virtual void*    getClosure    (void) const;

        MamaIoImpl* mPimpl;
    };

} // namespace Wombat
#endif // MAMA_IO_CPP_H__
