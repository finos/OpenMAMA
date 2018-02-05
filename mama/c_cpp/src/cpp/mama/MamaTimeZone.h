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

#ifndef MamaTimeZoneHpp__
#define MamaTimeZoneHpp__

#include <mama/timezone.h>


namespace Wombat 
{

    /**
     * A time zone representation to make conversion of timestamps to and
     * from particular time zones more convenient.
     *
     * Note: The addition of instance monitoring to the MamaTimeZone
     * implementation has resulted in the following limitation in its
     * usage.  Do not create short lived objects of this type on the
     * method stack or delete long lived objects before program
     * termination.  Pointers to all instances are maintained in a global
     * vector.  At the moment there is no mechanism by which we can detect
     * deleted objects or those which are popped off the method stack. An
     * internal thread will always iterate over all objects ever
     * created. A call to an object removed from the stack will result in
     * nondeterminable behaviour.  Pointers could be stored in a map
     * against a unique object id; however, addition and removal from the
     * map would have to be synchronized which would impact on
     * performance.
     */

    class MAMACPPExpDLL MamaTimeZone
    {
    public:
        /** Return a reference to a MamaTimeZone corresponding to the local
         * time zone. */
        static const MamaTimeZone& local ();

        /** Return a reference to a MamaTimeZone corresponding to UTC time zone. */
        static const MamaTimeZone& utc ();

        /** Return a reference to a MamaTimeZone corresponding to the US
         * Eastern time zone. */
        static const MamaTimeZone& usEastern ();

        /** Constructor. */
        MamaTimeZone ();

        /** Constructor.  NULL argument is equivalent to local timezone. */
        MamaTimeZone (const char* tz);

        /** Copy constructor. */
        MamaTimeZone (const MamaTimeZone&  copy);

        /** Destructor. */
        ~MamaTimeZone ();

        /** Assignment operator. */
        MamaTimeZone& operator= (const MamaTimeZone& rhs);

        /** Assign new timezones to this object. */
        void set (const char* tz);

        /** Clear this object. */
        void clear ();

        /** Return the time zone string. */
        const char* tz () const;

        /** Return the offset from UTC (in seconds).  Can be positive or
         * negative, depending upon the direction. */
        mama_i32_t offset () const;

        /** Check (recalculate) the UTC offset in case it has changed due
         * to daylight savings adjustments. */
        void check();

        mamaTimeZone        getCValue ()       { return myCimpl; }
        const mamaTimeZone  getCValue () const { return myCimpl; }

        /**
        * Use to set the interval of the thread updating each MamaTimeZone
        * instance offset
        */
        static void setScanningInterval (mama_f64_t seconds);

    private:
        mamaTimeZone  myCimpl;

        /** Constructor, given an existing C mamaTimeZone instance. */
        MamaTimeZone (mamaTimeZone  cimpl);
    }; // MamaTimeZone

} // namespace Wombat

#endif // MamaTimeZoneHpp__
