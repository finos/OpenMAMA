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

#ifndef MamdaOrderBookWriterH
#define MamdaOrderBookWriterH

#include <mama/mamacpp.h>
#include <mamda/MamdaOrderBookComplexDelta.h>
#include <mamda/MamdaOrderBookSimpleDelta.h>
#include <mamda/MamdaOrderBookBasicDelta.h>
#include <mamda/MamdaOrderBookBasicDeltaList.h>
#include <mamda/MamdaOrderBookFields.h>
#include <mamda/MamdaCommonFields.h>
#include <mamda/MamdaOptionalConfig.h>

namespace Wombat
{

class BookMsgHolder
{
public:
    BookMsgHolder()
        : mMsgVectorSize (0)
    {
        mMsgVector = new MamaMsg*[mMsgVectorSize];
        for (int i=0; i<mMsgVectorSize;i++)
        {
            mMsgVector[i] = NULL;
        }
        
    }
    mama_size_t               mMsgVectorSize;
    MamaMsg**                 mMsgVector;
    
    ~BookMsgHolder();
    void    grow (mama_size_t     size);
    void    clear();
    void    print();
};

class MAMDAOPTExpDLL MamdaOrderBookWriter
{
public:

    MamdaOrderBookWriter ();
    ~MamdaOrderBookWriter ();
    
    void    populateMsg (MamaMsg& msg, const MamdaOrderBookComplexDelta& delta);
    void    populateMsg (MamaMsg& msg, const MamdaOrderBookSimpleDelta& delta);
    void    populateMsg (MamaMsg& msg, const MamdaOrderBook& book);

private: 

    struct MamdaOrderBookWriterImpl;
    MamdaOrderBookWriterImpl& mImpl;
    
    // No copy constructor nor assignment operator.
    MamdaOrderBookWriter (const MamdaOrderBookWriter&);
    MamdaOrderBookWriter& operator= (const MamdaOrderBookWriter&);
};

} //namespace

#endif // MamdaOrderBookPublishingH
