/* $Id$
 *
 * OpenMAMA: The open middleware agnostic messaging API
 * Copyright (C) 2011 NYSE Inc.
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


/*=========================================================================
  =                             Includes                                  =
  =========================================================================*/

#include <stdint.h>
#include <mama/mama.h>
#include <msgimpl.h>
#include <mama/integration/bridge/base.h>
#include "codec.h"
#include "msgimpl.h"
#include "qpiddefs.h"


/*=========================================================================
 =                           Public functions                             =
 =========================================================================*/

mama_status
qpidBridgeMsgCodec_pack (msgBridge      bridgeMessage,
                         mamaMsg        target,
                         pn_message_t** protonMessage)
{
    pn_data_t*          properties      = NULL;
    pn_data_t*          body            = NULL;
    char                payloadType     = MAMA_PAYLOAD_ID_UNKNOWN;
    const void*         buffer          = NULL;
    mama_size_t         bufferLen       = 0;
    char*               subject         = NULL;
    char*               destination     = NULL;
    char*               inboxName       = NULL;
    char*               replyTo         = NULL;
    char*               targetSubject   = NULL;
    mama_status         status          = MAMA_STATUS_OK;
    baseMsgType         type            = BASE_MSG_PUB_SUB;

    if (NULL == bridgeMessage || NULL == target)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Get the underlying payload type */
    mamaMsgImpl_getPayloadId (target, &payloadType);

    /* If this is a qpid payload, we don't need to serialize */
    if (MAMA_PAYLOAD_QPID == payloadType)
    {
        /* This will extract only the underlying handle */
        mamaMsgImpl_getPayloadBuffer (target, &buffer, &bufferLen);

        /* Don't use function's proton message - use the one just extracted */
        *protonMessage = (pn_message_t*) buffer;
    }
    else
    {
        /* This will extract a serialized version of the payload */
        mamaMsg_getByteBuffer   (target, &buffer, &bufferLen);

        /* Use the function's proton message if this is not a qpid payload */
        body = pn_message_body  (*protonMessage);
        pn_data_put_binary      (body, pn_bytes(bufferLen, (char*)buffer));
    }

    /* Set the subject for the middleware according to the bridge msg */
    status = baseBridgeMamaMsgImpl_getSendSubject (bridgeMessage, &subject);
    if (MAMA_STATUS_OK != status)
    {
        return status;
    }
    pn_message_set_subject (*protonMessage, subject);

    /* Set the URL destination for the middleware according to the bridge msg */
    status = baseBridgeMamaMsgImpl_getDestination (bridgeMessage, &destination);
    if (MAMA_STATUS_OK != status)
    {
        return status;
    }
    pn_message_set_address (*protonMessage, destination);

    /* Get the properties from the message */
    properties = pn_message_properties (*protonMessage);

    /* Ensure the properties are cleared first before populating */
    pn_data_clear (properties);

    /* Ensure position is at the start */
    pn_data_rewind (properties);

    /* Container for application properties is a map */
    pn_data_put_map(properties);

    /* Enter into the list for access to its elements */
    pn_data_enter (properties);

    /* Set the message type for the middleware according to the bridge msg */
    status = baseBridgeMamaMsgImpl_getMsgType (bridgeMessage, &type);
    if (MAMA_STATUS_OK != status)
    {
        return status;
    }
    pn_data_put_string (properties,pn_bytes(strlen(QPID_KEY_MSGTYPE),QPID_KEY_MSGTYPE));
    pn_data_put_ubyte (properties,type);

    switch (type)
    {
    /* For inbox requests, set inbox name and reply to URLs */
    case BASE_MSG_INBOX_REQUEST:

        status = baseBridgeMamaMsgImpl_getInboxName (bridgeMessage, &inboxName);
        if (MAMA_STATUS_OK != status)
        {
            return status;
        }
        pn_data_put_string (properties,pn_bytes(strlen(QPID_KEY_INBOXNAME),QPID_KEY_INBOXNAME));
        pn_data_put_string (properties, pn_bytes (strlen(inboxName),
                                                      inboxName));

        status = baseBridgeMamaMsgImpl_getReplyTo (bridgeMessage, &replyTo);
        if (MAMA_STATUS_OK != status)
        {
            return status;
        }
        pn_data_put_string (properties,pn_bytes(strlen(QPID_KEY_REPLYTO),QPID_KEY_REPLYTO));
        pn_data_put_string (properties, pn_bytes (strlen(replyTo),
                                                  replyTo));

        break;
    /* For inbox responses, set the target subject (e.g. initial for XX) */
    case BASE_MSG_INBOX_RESPONSE:
        status = baseBridgeMamaMsgImpl_getTargetSubject (bridgeMessage,
                                                         &targetSubject);
        if (MAMA_STATUS_OK != status)
        {
            return status;
        }
        pn_data_put_string (properties,pn_bytes(strlen(QPID_KEY_TARGETSUBJECT),QPID_KEY_TARGETSUBJECT));
        pn_data_put_string (properties, pn_bytes (strlen(targetSubject),
                                                  targetSubject));
        break;
    /* The following message types require no further meta data */
    case BASE_MSG_TERMINATE:
    case BASE_MSG_SUB_REQUEST:
    case BASE_MSG_PUB_SUB:
    default:
        break;
    }

    /* Exit out of the list previously entered */
    pn_data_exit (properties);

    return MAMA_STATUS_OK;
}

mama_status
qpidBridgeMsgCodec_unpack (msgBridge        bridgeMessage,
                           mamaMsg          target,
                           pn_message_t*    protonMessage)
{
    pn_data_t*          properties      = NULL;
    pn_data_t*          body            = NULL;
    mama_status         status          = MAMA_STATUS_OK;
    baseMsgType         type            = BASE_MSG_PUB_SUB;
    pn_bytes_t          prop;
    pn_atom_t           firstAtom;
    int                 found = 0;

    if (NULL == bridgeMessage || NULL == protonMessage)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* grab the body */
    body = pn_message_body (protonMessage);

    /* Ensure position is at the start */
    pn_data_rewind (body);

    /* Skip over the initial null atom */
    pn_data_next (body);

    /* Grab the first atom and see what it is - PN_LIST = qpid */
    firstAtom = pn_data_get_atom (body);

    /* If this is a proton message */
    if (PN_LIST == firstAtom.type)
    {
        status = mamaMsgImpl_setMsgBuffer (target,
                                           (void*) protonMessage,
                                           sizeof (pn_message_t*),
                                           MAMA_PAYLOAD_QPID);
    }
    /* If this looks like another MAMA payload type */
    else if (PN_BINARY == firstAtom.type)
    {
        char payloadType =  MAMA_PAYLOAD_ID_NULL;

        if (firstAtom.u.as_bytes.size == 0)
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "baseBridgeMamaMsgImpl_unpack(): "
                      "Binary blob of zero length found - not processing");
            return MAMA_STATUS_INVALID_ARG;
        }

        /* The payload type is the first character */
        payloadType = (char) firstAtom.u.as_bytes.start[0];

        /* Use byte buffer to populate MAMA message */
        status = mamaMsgImpl_setMsgBuffer (
                            target,
                            (void*) firstAtom.u.as_bytes.start,
                            (mama_size_t) firstAtom.u.as_bytes.size,
                            payloadType);

        if (MAMA_STATUS_OK != status)
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "baseBridgeMamaMsgImpl_unpack(): "
                      "Could not set msg buffer. Cannot unpack (%s).",
                      mamaStatus_stringForStatus (status));
            return status;
        }
    }
    /* If this looks like something we cannot handle */
    else
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "baseBridgeMamaMsgImpl_unpack(): "
                  "Unable to unpack data received: incompatible format %s",
                  pn_type_name (firstAtom.type));
    }

    /* Get the properties */
    properties  = pn_message_properties (protonMessage);

    /* Ensure position is at the start */
    pn_data_rewind (properties);

    /* Skip over the initial null atom */
    pn_data_next (properties);

    /* Container for application properties is a map */
    pn_data_get_map (properties);
    pn_data_enter (properties);

    found = pn_data_lookup (properties,QPID_KEY_MSGTYPE);
    if (found)
    {
        type = (baseMsgType) pn_data_get_ubyte (properties);
    }
    else
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "baseBridgeMamaMsgImpl_unpack(): "
                  "Unable to retrieve message type from message");
        return MAMA_STATUS_PLATFORM;
    }

    baseBridgeMamaMsgImpl_setMsgType (bridgeMessage, type);

    switch (type)
    {
    case BASE_MSG_INBOX_REQUEST:

        /* Move onto inbox name and extract / copy */
        found = pn_data_lookup (properties,QPID_KEY_INBOXNAME);
        if (found)
        {
            prop = pn_data_get_string (properties);
        }
        else
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "baseBridgeMamaMsgImpl_unpack(): "
                      "Unable to retrieve inbox name");
            return MAMA_STATUS_PLATFORM;
        }

        status = baseBridgeMamaMsgImpl_setInboxName (bridgeMessage, prop.start);
        if (MAMA_STATUS_OK != status)
        {
            return status;
        }

        found = pn_data_lookup (properties,QPID_KEY_REPLYTO);
        if (found)
        {
            prop = pn_data_get_string (properties);
        }
        else
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "baseBridgeMamaMsgImpl_unpack(): "
                      "Unable to retrieve reply to url");
            return MAMA_STATUS_PLATFORM;
        }

        status = baseBridgeMamaMsgImpl_setReplyTo (bridgeMessage, prop.start);
        if (MAMA_STATUS_OK != status)
        {
            return status;
        }
        break;
    case BASE_MSG_INBOX_RESPONSE:

        found = pn_data_lookup (properties,QPID_KEY_TARGETSUBJECT);
        if (found)
        {
            prop = pn_data_get_string (properties);
        }
        else
        {
            mama_log (MAMA_LOG_LEVEL_ERROR,
                      "baseBridgeMamaMsgImpl_unpack(): "
                      "Unable to retrieve target container");
            return MAMA_STATUS_PLATFORM;
        }

        status = baseBridgeMamaMsgImpl_setTargetSubject (bridgeMessage,
                                                         prop.start);
        if (MAMA_STATUS_OK != status)
        {
            return status;
        }
        break;
    /* The following message types require no further meta data */
    case BASE_MSG_TERMINATE:
    case BASE_MSG_SUB_REQUEST:
    case BASE_MSG_PUB_SUB:
    default:
        break;
    }

    pn_data_exit (properties);

    return status;
}
