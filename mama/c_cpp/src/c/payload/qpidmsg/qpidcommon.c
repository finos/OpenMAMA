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
#include <stdint.h>
#include <string.h>

#include <mama/mama.h>
#include <proton/message.h>
#include <proton/error.h>

#include "payloadbridge.h"
#include "msgfieldimpl.h"
#include "qpidcommon.h"


/*=========================================================================
  =                  Public implementation functions                      =
  =========================================================================*/

void
qpidmsgPayloadInternal_checkLengthAndIncDest (mama_size_t  written,
                                              mama_size_t* length,
                                              char**       dest)
{
    if (written > 0 && written <= *length)
    {
        *dest   += written;
        *length -= written;
    }
}

mamaFieldType
qpidmsgPayloadInternal_toMamaType (pn_type_t type)
{
    switch (type)
    {
    case PN_NULL:       return MAMA_FIELD_TYPE_UNKNOWN;
    case PN_BOOL:       return MAMA_FIELD_TYPE_BOOL;
    case PN_UBYTE:      return MAMA_FIELD_TYPE_U8;
    case PN_BYTE:       return MAMA_FIELD_TYPE_I8;
    case PN_USHORT:     return MAMA_FIELD_TYPE_U16;
    case PN_SHORT:      return MAMA_FIELD_TYPE_I16;
    case PN_UINT:       return MAMA_FIELD_TYPE_U32;
    case PN_INT:        return MAMA_FIELD_TYPE_I32;
    case PN_CHAR:       return MAMA_FIELD_TYPE_CHAR;
    case PN_ULONG:      return MAMA_FIELD_TYPE_U64;
    case PN_LONG:       return MAMA_FIELD_TYPE_I64;
    case PN_TIMESTAMP:  return MAMA_FIELD_TYPE_TIME;
    case PN_FLOAT:      return MAMA_FIELD_TYPE_F32;
    case PN_DOUBLE:     return MAMA_FIELD_TYPE_F64;
    case PN_DECIMAL32:  return MAMA_FIELD_TYPE_UNKNOWN;
    case PN_DECIMAL64:  return MAMA_FIELD_TYPE_UNKNOWN;
    case PN_DECIMAL128: return MAMA_FIELD_TYPE_UNKNOWN;
    case PN_UUID:       return MAMA_FIELD_TYPE_UNKNOWN;
    case PN_BINARY:     return MAMA_FIELD_TYPE_OPAQUE;
    case PN_STRING:     return MAMA_FIELD_TYPE_STRING;
    case PN_SYMBOL:     return MAMA_FIELD_TYPE_UNKNOWN;
    case PN_DESCRIBED:  return MAMA_FIELD_TYPE_UNKNOWN;
    case PN_ARRAY:      return MAMA_FIELD_TYPE_UNKNOWN;
    case PN_LIST:       return MAMA_FIELD_TYPE_UNKNOWN;
    case PN_MAP:        return MAMA_FIELD_TYPE_UNKNOWN;
    default:            return MAMA_FIELD_TYPE_UNKNOWN;
    }
}

mama_status
qpidmsgPayloadInternal_toMamaStatus (int status)
{
    switch (status)
    {
    case PN_OK:        return MAMA_STATUS_OK;
    case PN_EOS:       return MAMA_STATUS_PLATFORM;
    case PN_ERR:       return MAMA_STATUS_PLATFORM;
    case PN_OVERFLOW:  return MAMA_STATUS_PLATFORM;
    case PN_UNDERFLOW: return MAMA_STATUS_PLATFORM;
    case PN_STATE_ERR: return MAMA_STATUS_PLATFORM;
    case PN_ARG_ERR:   return MAMA_STATUS_INVALID_ARG;
    case PN_TIMEOUT:   return MAMA_STATUS_TIMEOUT;
    case PN_INTR:      return MAMA_STATUS_PLATFORM;
    default:           return MAMA_STATUS_PLATFORM;
    }
}

mama_size_t
qpidmsgPayloadInternal_elementToString (pn_data_t* payload,
                                        pn_atom_t atom,
                                        char* dest,
                                        mama_size_t len)
{
    char*       originalPos = dest;
    int         written     = 0;

    switch (atom.type)
    {
    case PN_NULL:
        written = snprintf (dest, len, "NULL");
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    case PN_BOOL:
        written = snprintf (dest, len, "%u", atom.u.as_bool);
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    case PN_UBYTE:
        written = snprintf (dest, len, "%u", atom.u.as_ubyte);
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    case PN_BYTE:
        written = snprintf (dest, len, "%d", atom.u.as_byte);
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    case PN_USHORT:
        written = snprintf (dest, len, "%u", atom.u.as_ushort);
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    case PN_SHORT:
        written = snprintf (dest, len, "%d", atom.u.as_short);
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    case PN_UINT:
        written = snprintf (dest, len, "%u", atom.u.as_uint);
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    case PN_INT:
        written = snprintf (dest, len, "%d", atom.u.as_int);
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    case PN_CHAR:
        written = snprintf (dest, len, "%c", atom.u.as_char);
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    case PN_ULONG:
        written = snprintf (dest,
                          len,
                          "%llu",
                          (long long unsigned int) atom.u.as_ulong);
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    case PN_LONG:
        written = snprintf (dest, len, "%lld", (long long int) atom.u.as_long);
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    case PN_TIMESTAMP:
    {
        pn_timestamp_t  stamp       = atom.u.as_timestamp;
        uint32_t        micros      = (mama_u32_t) stamp;
        uint32_t        seconds     = (mama_u32_t) (stamp >> 32);

        written = snprintf (dest, len, "%u.%u", seconds, micros);
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    }
    case PN_FLOAT:
        written = snprintf (dest, len, "%f", atom.u.as_float);
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    case PN_DOUBLE:
        written = snprintf (dest, len, "%f", atom.u.as_double);
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    case PN_DECIMAL32:
        written = snprintf (dest,
                            len,
                            "%lu",
                            (long unsigned int) atom.u.as_decimal32);
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    case PN_DECIMAL64:
        written = snprintf (dest,
                            len,
                            "%llu",
                            (long long unsigned int) atom.u.as_decimal64);
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    case PN_DECIMAL128:
        written = snprintf (dest, len, "%s", atom.u.as_decimal128.bytes);
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    case PN_UUID:
        written = snprintf (dest, len, "%s", atom.u.as_uuid.bytes);
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    case PN_BINARY:
    {
        mama_size_t i          = 0;
        const char* bytePos    = NULL;
        pn_bytes_t  bytes;

        bytes    = atom.u.as_bytes;
        bytePos  = bytes.start;

        for (i = 0; i < bytes.size && len > 1; i++)
        {
            dest += sprintf (dest, "%#x ", *bytePos);
            bytePos++;
            len--;
        }
        if(bytes.size > 0)
        {
            dest--;
        }

        break;
    }
    case PN_STRING:
        written = snprintf (dest, len, "%s", atom.u.as_bytes.start);
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    case PN_SYMBOL:
        written = snprintf (dest, len, "PN_SYMBOL");
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    case PN_DESCRIBED:
        written = snprintf (dest, len, "PN_DESCRIBED");
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    case PN_ARRAY:
    {
        mama_size_t added = 0;

        /* load list in underlying implementation */
        pn_data_get_array (payload);

        /* enter list in underlying implementation */
        pn_data_enter (payload);

        written = snprintf (dest, len, "[");
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);

        /* move to first content in underlying implementation */
        while (0 != pn_data_next (payload) && len != 0)
        {
            pn_atom_t atom = pn_data_get_atom (payload);

            written = qpidmsgPayloadInternal_elementToString (payload,
                                                             atom,
                                                             dest,
                                                             len);

            qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);

            written = snprintf (dest, len, ",");
            qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);

            added++;
        }
        if (added > 0)
        {
            /* move on top of the trailing comma to overwrite */
            dest--;
            len++;
        }
        written = snprintf(dest, len, "]");
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);

        /* Name */
        pn_data_exit (payload);
        break;
    }
    case PN_LIST:
    {
        mama_size_t added = 0;

        /* load list in underlying implementation */
        pn_data_get_list (payload);

        /* enter list in underlying implementation */
        pn_data_enter    (payload);

        written = snprintf (dest, len, "{");
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);

        /* move to first content in underlying implementation */
        while (0 != pn_data_next (payload))
        {
            pn_atom_t atom = pn_data_get_atom (payload);
            written = qpidmsgPayloadInternal_elementToString (payload,
                                                              atom,
                                                              dest,
                                                              len);
            qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);

            written = snprintf (dest, len, ",");
            qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
            added++;
        }
        if (added > 0)
        {
            /* move on top of the trailing comma to overwrite */
            dest--;
        }
        written = snprintf (dest, len, "}");
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);

        /* Name */
        pn_data_exit (payload);
        break;
    }
    case PN_MAP:
        written = snprintf (dest, len, "PN_MAP");
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    default:
        written = snprintf (dest, len, "?");
        qpidmsgPayloadInternal_checkLengthAndIncDest (written, &len, &dest);
        break;
    }

    return (dest - originalPos);
}
