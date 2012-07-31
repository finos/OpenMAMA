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

#include "mama/MamaMsgQual.h"
#include "mamacppinternal.h"

namespace Wombat
{

    MamaMsgQual::MamaMsgQual ()
        : myStrRep (0)
    {
        mamaMsgQual_create (&myMsgQual);
    }

    MamaMsgQual::MamaMsgQual (mama_u16_t value)
        : myStrRep (0)
    {
        mamaMsgQual_create (&myMsgQual);
        mamaTry (mamaMsgQual_setValue (myMsgQual, value));
    }

    MamaMsgQual::MamaMsgQual (const MamaMsgQual& copy)
        : myStrRep (0)
    {
         mamaTry (mamaMsgQual_copy (myMsgQual, copy.myMsgQual));
    }

    MamaMsgQual::~MamaMsgQual ()
    {
        if (myStrRep)
        {
            delete myStrRep;
        }
        mamaTry (mamaMsgQual_destroy (myMsgQual));
    }

    MamaMsgQual& MamaMsgQual::operator= (const MamaMsgQual& rhs)
    {
        if (this != &rhs)
        {
            mamaTry (mamaMsgQual_copy (myMsgQual, rhs.myMsgQual));
        }
        return *this;
    }

    bool MamaMsgQual::operator== (const MamaMsgQual& rhs) const
    {
        return (this == &rhs) || mamaMsgQual_equal (myMsgQual, rhs.myMsgQual);
    }

    bool MamaMsgQual::operator== (mama_u16_t  rhs) const
    {
        return (getValue () == rhs);
    }

    void MamaMsgQual::clear ()
    {
        mamaTry (mamaMsgQual_clear (myMsgQual));
    }

    void MamaMsgQual::setValue (mama_u16_t value)
    {
        mamaTry (mamaMsgQual_setValue (myMsgQual, value));
    }

    void MamaMsgQual::setIsDefinatelyDuplicate  (bool tf)
    {
        mamaTry (mamaMsgQual_setIsDefinitelyDuplicate (myMsgQual, 
                                                       (tf == true ? 1 : 0)));
    }

    void MamaMsgQual::setIsPossiblyDuplicate    (bool tf)
    {
        mamaTry (mamaMsgQual_setIsPossiblyDuplicate (myMsgQual, 
                                                     (tf == true ? 1 : 0)));
    }

    void MamaMsgQual::setIsDefinatelyDelayed    (bool tf)
    {
        mamaTry (mamaMsgQual_setIsDefinitelyDelayed (myMsgQual, 
                                                     (tf == true ? 1 : 0)));
    }

    void MamaMsgQual::setIsPossiblyDelayed      (bool tf)
    {
        mamaTry (mamaMsgQual_setIsPossiblyDelayed (myMsgQual, 
                                                   (tf == true ? 1 : 0)));
    }

    void MamaMsgQual::setIsOutOfSequence        (bool tf)
    {
        mamaTry (mamaMsgQual_setIsOutOfSequence (myMsgQual, 
                                                 (tf == true ? 1 : 0)));
    }

    mama_u16_t MamaMsgQual::getValue            () const
    {
        mama_u16_t value;
        mamaTry (mamaMsgQual_getValue (myMsgQual, &value));
        return value;
    }

    bool MamaMsgQual::getIsDefinatelyDuplicate  () const
    {
        int result = 0;
        mamaTry (mamaMsgQual_getIsDefinitelyDuplicate (myMsgQual, &result));
        return (result == 0 ? false : true);
    }

    bool MamaMsgQual::getIsPossiblyDuplicate    () const
    {
        int result = 0;
        mamaTry (mamaMsgQual_getIsPossiblyDuplicate (myMsgQual, &result));
        return (result == 0 ? false : true);
    }

    bool MamaMsgQual::getIsDefinatelyDelayed    () const
    {
        int result = 0;
        mamaTry (mamaMsgQual_getIsDefinitelyDelayed (myMsgQual, &result));
        return (result == 0 ? false : true);
    }

    bool MamaMsgQual::getIsPossiblyDelayed      () const
    {
        int result = 0;
        mamaTry (mamaMsgQual_getIsPossiblyDelayed (myMsgQual, &result));
        return (result == 0 ? false : true);
    }

    bool MamaMsgQual::getIsOutOfSequence        () const
    {
        int result = 0;
        mamaTry (mamaMsgQual_getIsOutOfSequence (myMsgQual, &result));
        return (result == 0 ? false : true);
    }

    void MamaMsgQual::getAsString (char*        result,
                                   mama_size_t  maxLen) const
    {
        mamaTry (mamaMsgQual_getAsString (myMsgQual, NULL, result, maxLen));
    }

    void MamaMsgQual::getAsString (const mama_u16_t&  value,
                                   char*              result,
                                   mama_size_t        maxLen)
    {
        mamaTry (mamaMsgQual_valueToString (value, NULL, result, maxLen));
    }

    const char* MamaMsgQual::getAsString () const
    {
        if (!myStrRep)
        {
            myStrRep = new char [MAMA_MSG_QUAL_MAX_STR_LEN + 1];
        }
        getAsString (myStrRep, MAMA_MSG_QUAL_MAX_STR_LEN);
        return myStrRep;
    }

} /* namespace  Wombat */
