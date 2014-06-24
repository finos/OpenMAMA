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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "qpiddefs.h"
#include "qpidcommon.h"
#include "transport.h"

/*=========================================================================
  =                  Public implementation functions                      =
  =========================================================================*/

mama_status
qpidBridgeCommon_parseSubjectKey (char*             key,
                                  const char**      root,
                                  const char**      source,
                                  const char**      topic,
                                  transportBridge   transport)
{
    /* We can assume subject key is of this length */
    char subject[MAX_SUBJECT_LENGTH];

    char* subjectPosition = subject;

    /* Reset return values */
    *root   = NULL;
    *source = NULL;
    *topic  = NULL;

    if (NULL == key || 0 == strlen(key))
    {
        return MAMA_STATUS_NULL_ARG;
    }

    /* Copy the key across to writable memory */
    strcpy (subject, key);

    subjectPosition = strtok (subjectPosition, ".");

    if (0 == strcmp (MAMA_ROOT_MARKET_DATA, subjectPosition) ||
            0 == strcmp (MAMA_ROOT_MARKET_DATA_DICT, subjectPosition))
    {
        *root = strdup (subjectPosition);

        /* This should NULL terminate the end of the source string */
        subjectPosition = strtok (NULL, ".");
        if (NULL != subjectPosition)
        {
            uint32_t parsedBytes = 0;

            *source = strdup (subjectPosition);

            qpidBridgeMamaTransportImpl_addKnownMamaSymbolNamespace (
                    transport,
                    *source);

            /* String lengths plus NULL characters */
            parsedBytes = strlen (*root) + 1 + strlen (*source) + 1;

            /* Only parse topic if it exists... */
            if (parsedBytes < strlen (key))
            {
                /* The topic is then whatever is left after source NULL character */
                *topic = strdup (subject + parsedBytes);
            }
        }
    }
    /* Might have a symbol namespace... might not... */
    else
    {
        char* firstSubjectPart = subjectPosition;

        /* If a MAMA symbol namespace is involved, there should always be
         * another part */
        char* secondSubjectPart = strtok (NULL, ".");

        /* If there is no second part, this looks like a basic topic. Also if
         * this is a symbol namespace we have never seen before, assume it's
         * basic. */
        if (NULL == secondSubjectPart)
        {
            *topic = strdup (firstSubjectPart);
        }
        /* There is a second part, but we are unconvinced unless the transport
         * has seen this source somewhere before */
        else if (qpidBridgeMamaTransportImpl_isKnownMamaSymbolNamespace (
                                transport,
                                firstSubjectPart))
        {
            *source = strdup (firstSubjectPart);
            /* Use offsets from provided strings rather than the one strtok
             * may have just modified with NULL characters for the topic */
            *topic  = strdup (key + strlen (firstSubjectPart) + 1);
        }
        /* If this is a subject without a symbol namespace, just use the
         * original string rather than try and revert strtok's trail of
         * destruction */
        else
        {
            *topic = strdup (key);
        }
    }
    return MAMA_STATUS_OK;
}


/*
 * Internal function to ensure that the topic names are always calculated
 * in a particular way
 */
mama_status
qpidBridgeCommon_generateSubjectKey (const char*  root,
                                     const char*  source,
                                     const char*  topic,
                                     char**       keyTarget)
{
    char        subject[MAX_SUBJECT_LENGTH];
    char*       subjectPos     = subject;
    size_t      bytesRemaining = MAX_SUBJECT_LENGTH;
    size_t      written        = 0;

    if (NULL != root)
    {
        mama_log (MAMA_LOG_LEVEL_FINEST,
                  "qpidBridgeMamaSubscriptionImpl_generateSubjectKey(): R.");
        written         = snprintf (subjectPos, bytesRemaining, "%s", root);
        subjectPos     += written;
        bytesRemaining -= written;
    }

    if (NULL != source)
    {
        mama_log (MAMA_LOG_LEVEL_FINEST,
                  "qpidBridgeMamaSubscriptionImpl_generateSubjectKey(): S.");
        /* If these are not the first bytes, prepend with a period */
        if(subjectPos != subject)
        {
            written     = snprintf (subjectPos, bytesRemaining, ".%s", source);
        }
        else
        {
            written     = snprintf (subjectPos, bytesRemaining, "%s", source);
        }
        subjectPos     += written;
        bytesRemaining -= written;
    }

    if (NULL != topic)
    {
        mama_log (MAMA_LOG_LEVEL_FINEST,
                  "qpidBridgeMamaSubscriptionImpl_generateSubjectKey(): T.");
        /* If these are not the first bytes, prepend with a period */
        if (subjectPos != subject)
        {
            snprintf (subjectPos, bytesRemaining, ".%s", topic);
        }
        else
        {
            snprintf (subjectPos, bytesRemaining, "%s", topic);
        }
    }

    /*
     * Allocate the memory for copying the string. Caller is responsible for
     * destroying.
     */
    *keyTarget = strdup (subject);
    if (NULL == *keyTarget)
    {
        return MAMA_STATUS_NOMEM;
    }
    else
    {
        return MAMA_STATUS_OK;
    }
}

/*
 * Internal function to ensure that the topic names are always calculated
 * in a particular way
 */
mama_status
qpidBridgeCommon_generateSubjectUri (const char*  format,
                                     const char*  root,
                                     const char*  source,
                                     const char*  topic,
                                     const char*  uuid,
                                     const char** uriTarget)
{
    char    original[MAX_URI_LENGTH];
    char    uri[MAX_URI_LENGTH];
    char    lastByte;
    char*   uriPos              = uri;
    char*   nextSeg             = NULL;
    size_t  bytesRemaining      = MAX_URI_LENGTH;
    size_t  written             = 0;

    strncpy (original, format, MAX_URI_LENGTH);

    nextSeg = strtok ((char*) original, "%");

    while (nextSeg != NULL)
    {
        written = snprintf (uriPos, bytesRemaining, "%s", nextSeg);
        bytesRemaining -= written;
        uriPos += written;

        nextSeg = strtok (NULL, "%");
        if (nextSeg != NULL)
        {
            switch (*nextSeg)
            {
                case 'r':
                    if (NULL != root)
                    {
                        written = snprintf (uriPos, bytesRemaining, "%s", root);
                        bytesRemaining -= written;
                        uriPos += written;
                    }
                    nextSeg++;
                    break;
                case 'S':
                    if (NULL != source)
                    {
                        written = snprintf (uriPos, bytesRemaining, "%s", source);
                        bytesRemaining -= written;
                        uriPos += written;
                    }
                    nextSeg++;
                    break;
                case 's':
                    if (NULL != topic)
                    {
                        written = snprintf (uriPos, bytesRemaining, "%s", topic);
                        bytesRemaining -= written;
                        uriPos += written;
                    }
                    nextSeg++;
                    break;
                case 'u':
                    if (NULL != uuid)
                    {
                        written = snprintf (uriPos, bytesRemaining, "%s", uuid);
                        bytesRemaining -= written;
                        uriPos += written;
                    }
                    nextSeg++;
                    break;
                default: /* do nothing -just go on copying in next iteration */
                    break;
            }
            /* This will ensure there are no double forward slashes */
            if (NULL != nextSeg && '/' == *nextSeg && '/' == uri[strlen(uri) - 1])
            {
                nextSeg++;
            }
        }
    }

    /* Trim trailing forward slashes */
    if ('/' == uri[strlen(uri) - 1])
    {
        uri[strlen(uri) - 1] = '\0';
    }

    /*
     * Allocate the memory for copying the string. Caller is responsible for
     * destroying.
     */
    *uriTarget = strdup (uri);
    if (NULL == *uriTarget)
    {
        return MAMA_STATUS_NOMEM;
    }
    else
    {
        return MAMA_STATUS_OK;
    }
}
