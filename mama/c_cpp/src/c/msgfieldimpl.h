/* $Id: msgfieldimpl.h,v 1.6.6.1.14.5 2011/10/02 19:02:17 ianbell Exp $
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

#ifndef MamaMsgFieldImplH__
#define MamaMsgFieldImplH__


#if defined(__cplusplus)
extern "C" {
#endif

/*The impl for the mamaMsgField structure.*/
typedef struct mamaMsgFieldImpl_
{
    mamaFieldDescriptor  myFieldDesc;
    mamaDictionary       myDictionary;
    mamaMsg*             myLastVectorMsg;
    size_t               myLastVectorMsgLen;
    mamaMsg*             myLastVectorPayloadMsg;
    size_t               myLastVectorPayloadMsgLen;
    mamaMsg              mySubMsg;
    msgFieldPayload      myPayload;
    mamaMsg              myMsg;
    mamaPayloadBridge    myPayloadBridge;
} mamaMsgFieldImpl;


MAMAExpDLL
extern mama_status
mamaMsgFieldImpl_setmamaPayloadBridge (mamaMsgField         msgField,
                                   mamaPayloadBridgeImpl*   mamaPayloadBridge);

MAMAExpDLL
extern mama_status
mamaMsgFieldImpl_setPayload       (mamaMsgField         msgField,
                                   msgFieldPayload      payload);

MAMAExpDLL
extern mama_status
mamaMsgFieldImpl_setFieldDesc     (mamaMsgField         msgField,
                                   mamaFieldDescriptor  descriptor);


#if defined(__cplusplus)
}
#endif

#endif /* MamaMsgFieldImplH__ */
