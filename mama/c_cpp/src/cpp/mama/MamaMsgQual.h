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

#ifndef MAMA_MSG_QUAL_CPP_H__
#define MAMA_MSG_QUAL_CPP_H__

#include <mama/msgqualifier.h>


namespace Wombat 
{

    /**
     * The MamaMsgQual class is a wrapper/utility class which provides useful
     * interrogation, comparison and manipulation facilities for the
     * Mama Message Qualifier data field (wMsgQual) which is a "bit-map"
     * used to convey duplicate, delayed and out-of-sequence information 
     * about messages
     */
    class MAMACPPExpDLL MamaMsgQual
    {
    public:
        MamaMsgQual ();
        MamaMsgQual (mama_u16_t         value);
        MamaMsgQual (const MamaMsgQual& copy);

        ~MamaMsgQual ();

        MamaMsgQual&        operator=  (const MamaMsgQual& rhs);
        bool                operator== (const MamaMsgQual& rhs) const;
        bool                operator== (mama_u16_t         rhs) const;
        bool                operator!= (const MamaMsgQual& rhs) const
        { 
            return ! operator== (rhs); 
        }
        bool                operator!= (mama_u16_t         rhs) const
        {
            return ! operator== (rhs); 
        }

        void                clear ();

        void                setValue                  (mama_u16_t value);
        void                setIsDefinatelyDuplicate  (bool tf);
        void                setIsPossiblyDuplicate    (bool tf);
        void                setIsDefinatelyDelayed    (bool tf);
        void                setIsPossiblyDelayed      (bool tf);
        void                setIsOutOfSequence        (bool tf);
        
        mama_u16_t          getValue                  () const;
        bool                getIsDefinatelyDuplicate  () const;
        bool                getIsPossiblyDuplicate    () const;
        bool                getIsDefinatelyDelayed    () const;
        bool                getIsPossiblyDelayed      () const;
        bool                getIsOutOfSequence        () const;

        void                getAsString     (char*        result,
                                             mama_size_t  maxLen) const;
        /**
         * Return a string representation of the message qualifier.  
         * Note that the alternative getAsString() method is more efficient 
         * because this method must allocate a temporary buffer (automatically
         * destroyed upon object destruction).
         */
        const char*         getAsString     () const;

        /**
         * Static helper function to convert from the raw 16bit integer
         * representation directly to a colon delimited string of conditions.
         */
        static void         getAsString     (const mama_u16_t&  value,
                                             char*              result,
                                             mama_size_t        maxLen);
    private:
        mamaMsgQual     myMsgQual;
        mutable char*   myStrRep;
    };

} // namespace Wombat

#endif // MAMA_MSG_QUAL_CPP_H__
