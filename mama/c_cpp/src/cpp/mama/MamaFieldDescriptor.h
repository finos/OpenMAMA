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

#ifndef MAMA_FIELD_DESCRIPTOR_CPP_H__
#define MAMA_FIELD_DESCRIPTOR_CPP_H__

#include <mama/mamacpp.h>

namespace Wombat 
{
    class FieldDescriptorImpl;

    /**
     * The <code>MamaFieldDescriptor</code> class describes a field within a
     * <code>MamaDictionary</code>
     *
     * @see MamaDictionary
     * @see MamaMsg
     */

    class MAMACPPExpDLL MamaFieldDescriptor
    {
    public:
        virtual ~MamaFieldDescriptor ();

        MamaFieldDescriptor (mamaFieldDescriptor field);

        /**
         * Create a new field descriptor based on supplied info.
         *
         * @param fid The field id.
         * @param type The field type.
         * @param name The field name.
         *
         */
        MamaFieldDescriptor (mama_fid_t            fid,
                             mamaFieldType         type,
                             const char*           name);

        /**
         * Return the field identifier.
         *
         * @return The fid.
         */
        mama_fid_t getFid (void) const;

        /**
         * Return the data type.
         *
         * @return The type.
         */
        mamaFieldType getType (void) const;

        /**
         * Return the human readable name of the field.
         *
         * @return The name.
         */
        const char* getName (void) const;

        /**
         * Return a human readable string for mamaMsgType.
         */
        const char* getTypeName (void) const;

        /**
         * Associate user supplied data with the field descriptor.
         *
         * @param closure The user supplied data to associate with the field
         * descriptor.
         */
        void setClosure (void* closure);

        /**
         * Return the user supplied data associated with the field descriptor.
         *
         * @return The user supplied data associated with the field descriptor.
         */
        void* getClosure () const;

        /**
         * Track the modification state of the field. 
         *
         */
        void setTrackModState (bool on);

        /**  
         * Track the modification state of the field?
         *
         */
        bool getTrackModState () const;

        /**
         * Set the publish name for this field
         *
         */
        void setPubName (const char* pubName);

        /**  
         * Get the publish name for this field
         *
         */
        const char* getPubName () const;

        FieldDescriptorImpl* mPimpl;
    };

} // namespace Wombat
#endif // MAMA_FIELD_DESCRIPTOR_CPP_H__
