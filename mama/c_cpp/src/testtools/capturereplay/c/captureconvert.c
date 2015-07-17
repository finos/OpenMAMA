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
#include <mama/mama.h>
#include <mama/msg.h>
#include <playback/playbackFileParser.h>
#include <playback/playbackcapture.h>

#include <string.h>

#define MAMA_FIELD_COPY(SUFFIX, MAMATYPE)                                      \
do {                                                                           \
    MAMATYPE result;                                                           \
    status = mamaMsgField_get##SUFFIX (currentField, &result);                 \
    if (status != MAMA_STATUS_OK)                                              \
    {                                                                          \
        mama_log (                                                             \
                  MAMA_LOG_LEVEL_ERROR,                                        \
                  "Cannot get MAMA_FIELD_TYPE_"#SUFFIX" from mamaMsg (%s). "    \
                  "Exiting.", mamaStatus_stringForStatus(status));             \
        exit (1);                                                              \
    }                                                                          \
                                                                               \
    mama_log (MAMA_LOG_LEVEL_FINEST,                                           \
                      "Adding MAMA_FIELD_TYPE_"#SUFFIX" to output message: %d", \
                      result);                                                 \
    status = mamaMsg_add##SUFFIX (*targetMessage, NULL, fid, result);          \
    if (status != MAMA_STATUS_OK)                                              \
    {                                                                          \
        mama_log (                                                             \
                  MAMA_LOG_LEVEL_ERROR,                                        \
                  "Cannot add MAMA_FIELD_TYPE_"#SUFFIX" output message type "   \
                  "(%s). Exiting.", mamaStatus_stringForStatus(status));       \
        exit (1);                                                              \
    }                                                                          \
} while (0)                                                                    \

#define MAMA_VECTOR_FIELD_COPY(SUFFIX, MAMATYPE)                              \
do {                                                                           \
    const MAMATYPE* result;                                                    \
    mama_size_t resultLen;                                                     \
    status = mamaMsgField_getVector##SUFFIX (currentField, &result, &resultLen);\
    if (status != MAMA_STATUS_OK)                                              \
    {                                                                          \
        mama_log (                                                             \
                  MAMA_LOG_LEVEL_ERROR,                                        \
                  "Cannot get MAMA_FIELD_TYPE_VECTOR_"#SUFFIX" from mamaMsg "   \
                  "(%s). Exiting.", mamaStatus_stringForStatus(status));       \
        exit (1);                                                              \
    }                                                                          \
                                                                               \
    mama_log (MAMA_LOG_LEVEL_FINEST,                                           \
              "Adding MAMA_FIELD_TYPE_VECTOR_"#SUFFIX" to output message.");    \
    status = mamaMsg_addVector##SUFFIX (*targetMessage, NULL, fid, result,     \
                                        resultLen);                            \
    if (status != MAMA_STATUS_OK)                                              \
    {                                                                          \
        mama_log (                                                             \
                  MAMA_LOG_LEVEL_ERROR,                                        \
                  "Cannot add MAMA_FIELD_TYPE_VECTOR_"#SUFFIX" output message " \
                  "type (%s). Exiting.",                                       \
                  mamaStatus_stringForStatus(status));                         \
        exit (1);                                                              \
    }                                                                          \
} while (0)                                                                    \

static const char* gInputFilename = NULL;
static const char* gInputPayloadType = NULL;
static const char* gOutputFilename = NULL;
static const char* gOutputPayloadType = NULL;

static const char* gUsageString[] =
        { " CaptureConvert -  Application for converting a previously captured",
          "                   MAMA playback file into an alternative payload format",
          " ",
          " Usage: captureconvert -input-file <filename> -input-payload <payload_type> ",
          "            -output-file <filename> -output-payload <payload_type> [-v]",
          " ",
          " Options: ",
          " -input-file       The path of the input file containing the playback to be converted.",
          " -input-payload    The payload type of the input file.",
          " -output-file      The path of the output file.",
          " -output-payload   The payload type of the output file.",
          " -v                Increase verbosity of MAMA logging. May be passed multiple times. Optional.",
          "", NULL };

static void displayUsageString (void);
static void parseCommandLine (int argc, const char** argv);
static void copyMamaMsg (mamaMsg sourceMessage,
                         mamaMsg* targetMessage,
                         mamaPayloadBridge outputPayloadBridge);

static void displayUsageString ()
{
    int i = 0;
    while (gUsageString[i] != NULL )
    {
        printf ("%s\n", gUsageString[i++]);
    }
    exit (0);
}

static void parseCommandLine (int argc, const char** argv)
{
    int i = 0;
    for (i = 1; i < argc;)
    {
        /* Input file for processing. */
        if (strcmp (argv[i], "-input-file") == 0)
        {
            gInputFilename = argv[i + 1];
            i += 2;
        }
        else if (strcmp (argv[i], "-input-payload") == 0)
        {
            gInputPayloadType = argv[i + 1];
            i += 2;
        }
        else if (strcmp (argv[i], "-output-file") == 0)
        {
            gOutputFilename = argv[i + 1];
            i += 2;
        }
        else if (strcmp (argv[i], "-output-payload") == 0)
        {
            gOutputPayloadType = argv[i + 1];
            i += 2;
        }
        else if (strcmp (argv[i], "-help") == 0)
        {
            displayUsageString ();
        }
        else if (strcmp (argv[i], "-v") == 0)
        {
            if (mama_getLogLevel () == MAMA_LOG_LEVEL_WARN)
            {
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_NORMAL);
            }
            else if (mama_getLogLevel () == MAMA_LOG_LEVEL_NORMAL)
            {
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINE);
            }
            else if (mama_getLogLevel () == MAMA_LOG_LEVEL_FINE)
            {
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINER);
            }
            else
            {
                mama_enableLogging (stderr, MAMA_LOG_LEVEL_FINEST);
            }
            i++;
        }
        else
            i++;
    }

    if (gInputFilename == NULL )
    {
        printf (" WARNING: Please provide an input file for processing.\n\n");
        displayUsageString ();
    }
    else if (gInputPayloadType == NULL )
    {
        printf (" WARNING: Please provide an input payload type for processing.\n\n");
        displayUsageString ();
    }
    else if (gOutputFilename == NULL )
    {
        printf (" WARNING: Please provide a filename to output the converted data to.\n\n");
        displayUsageString ();
    }
    else if (gOutputPayloadType == NULL )
    {
        printf (" WARNING: Please provide a payload type for the output data.\n\n");
        displayUsageString ();
    }
}
#define DELIM    ":"

int main (int argc, const char **argv)
{
        mama_status  status     = MAMA_STATUS_OK;
	mamaPlaybackFileParser	fileParser		= NULL;
	mamaPlaybackCapture		fileCapture		= NULL;
	char*           		headerString	= NULL;
	mamaMsg        			sourceMsg		= NULL;
	mamaMsg					targetMsg		= NULL;
        mamaPayloadBridge   inputPayloadBridge  = NULL;
        mamaPayloadBridge   outputPayloadBridge = NULL;

    /* Parse command line options */
    parseCommandLine (argc, argv);

    /* Load the payload bridges we wish to use for processing both input 
     * and output files.
     */
    mama_log (MAMA_LOG_LEVEL_NORMAL, "Loading payload bridges.");
    mama_log (MAMA_LOG_LEVEL_NORMAL, "Input Bridge: %s", gInputPayloadType);
    mama_log (MAMA_LOG_LEVEL_NORMAL, "Output Bridge: %s", gOutputPayloadType);

    status = mama_loadPayloadBridge (&inputPayloadBridge, gInputPayloadType);
    if (status != MAMA_STATUS_OK)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Input payload bridge could not be loaded.");
        return 1;
    }

    status = mama_loadPayloadBridge (&outputPayloadBridge, gOutputPayloadType);
    if (status != MAMA_STATUS_OK)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Output payload bridge could not be loaded (%s).",
                  mamaStatus_stringForStatus(status));
        return 1;
    }

    /* Opening Playback file for parsing.*/
    mama_log (MAMA_LOG_LEVEL_NORMAL, "Opening playback file for parsing: %s",
              gInputFilename);
    status = mamaPlaybackFileParser_allocate (&fileParser);
    if (status != MAMA_STATUS_OK)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Could not allocate memory for playback file parser (%s).",
                  mamaStatus_stringForStatus(status));
        return 1;
    }

    status = mamaPlaybackFileParser_openFile (fileParser,
                                              (char*) gInputFilename);
    if (status != MAMA_STATUS_OK)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "Could not open playback file (%s).",
                  mamaStatus_stringForStatus(status));
        return 1;
    }

    /* Opening output file. */
    mama_log (MAMA_LOG_LEVEL_NORMAL, "Opening output file: %s",
              gOutputFilename);
    status = mamaCapture_allocate (&fileCapture);
    if (status != MAMA_STATUS_OK)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Could not allocate memory for playback output file (%s).",
                  mamaStatus_stringForStatus(status));
        return 1;
    }

    status = mamaCapture_openFile (&fileCapture, gOutputFilename);
    if (status != MAMA_STATUS_OK)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR, "Could not open output file (%s).",
                  mamaStatus_stringForStatus(status));
        return 1;
    }

    /* Create mama msg from output payload. */
    mama_log (MAMA_LOG_LEVEL_NORMAL,
              "Creating MAMA Message object for output payload.");
    status = mamaMsg_createForPayloadBridge (&targetMsg, outputPayloadBridge);

    if (status != MAMA_STATUS_OK)
    {
        mama_log (MAMA_LOG_LEVEL_ERROR,
                  "Could not create MAMA message for payload (%s).",
                  mamaStatus_stringForStatus(status));
        return 1;
    }

    while (mamaPlaybackFileParser_getNextHeader (fileParser, &headerString))
    {
        if (mamaPlaybackFileParser_getNextMsg (fileParser, &sourceMsg))
        {
            char 	temp[64];
            char*	start = headerString;
            char*	end = strchr (headerString,':');

            strncpy (temp, start, end - start);
            temp[end - start] = '\0';
            mamaCapture_setFeedSource (&fileCapture, temp);
            end++;

            start = end;
            end = strchr (start, ':');
            strncpy (temp, start, end - start);
            temp[end - start] = '\0';
            mamaCapture_setTransportName (&fileCapture, temp);
            end++;

            start = end;
            end = strchr (start, '\0');
            strncpy (temp, start, end - start);
            temp[end - start] = '\0';
            mamaCapture_setSymbol (&fileCapture, temp);

            copyMamaMsg (sourceMsg, &targetMsg, outputPayloadBridge);

            mamaCapture_saveMamaMsg (&fileCapture, &targetMsg);
        }
    }

    mamaCapture_closeFile (fileCapture);
    mamaCapture_deallocate (fileCapture);
    mamaPlaybackFileParser_closeFile(fileParser);
    mamaPlaybackFileParser_deallocate (fileParser);

    /* Successfully converted file. */
    return 0;
}

static void copyMamaMsg (mamaMsg sourceMessage,
                         mamaMsg* targetMessage,
                         mamaPayloadBridge outputPayloadBridge)
{
    mama_status status = MAMA_STATUS_OK;
    mamaMsgIterator iterator = NULL;
    mamaMsgField currentField = NULL;
    mamaFieldType fieldType = MAMA_FIELD_TYPE_UNKNOWN;

    mamaMsgIterator_create (&iterator, NULL );
    mamaMsgIterator_associate (iterator, sourceMessage);

    mamaMsg_clear (*targetMessage);

    while ((currentField = mamaMsgIterator_next (iterator)) != NULL )
    {
        uint16_t fid = 0;
        mamaMsgField_getFid (currentField, &fid);
        mamaMsgField_getType (currentField, &fieldType);

        switch (fieldType)
        {
        case MAMA_FIELD_TYPE_BOOL:
        {
            MAMA_FIELD_COPY (Bool, mama_bool_t);
            break;
        }
        case MAMA_FIELD_TYPE_CHAR:
        {
            MAMA_FIELD_COPY (Char, char);
            break;
        }
        case MAMA_FIELD_TYPE_I8:
        {
            MAMA_FIELD_COPY (I8, mama_i8_t);
            break;
        }
        case MAMA_FIELD_TYPE_U8:
        {
            MAMA_FIELD_COPY (U8, mama_u8_t);
            break;
        }
        case MAMA_FIELD_TYPE_I16:
        {
            MAMA_FIELD_COPY (I16, mama_i16_t);
            break;
        }
        case MAMA_FIELD_TYPE_U16:
        {
            MAMA_FIELD_COPY (U16, mama_u16_t);
            break;
        }
        case MAMA_FIELD_TYPE_I32:
        {
            MAMA_FIELD_COPY (I32, mama_i32_t);
            break;
        }
        case MAMA_FIELD_TYPE_U32:
        {
            MAMA_FIELD_COPY (U32, mama_u32_t);
            break;
        }
        case MAMA_FIELD_TYPE_I64:
        {
            MAMA_FIELD_COPY (I64, mama_i64_t);
            break;
        }
        case MAMA_FIELD_TYPE_U64:
        {
            MAMA_FIELD_COPY (U64, mama_u64_t);
            break;
        }
        case MAMA_FIELD_TYPE_F32:
        {
            MAMA_FIELD_COPY (F32, mama_f32_t);
            break;
        }
        case MAMA_FIELD_TYPE_F64:
        {
            MAMA_FIELD_COPY (F64, mama_f64_t);
            break;
        }
        case MAMA_FIELD_TYPE_STRING:
        {
            MAMA_FIELD_COPY (String, const char*);
            break;
        }
        case MAMA_FIELD_TYPE_TIME:
        {
            mamaDateTime result = NULL;
            mamaDateTime_create (&result);
            status = mamaMsgField_getDateTime (currentField, result);
            if (status != MAMA_STATUS_OK)
            {
                mama_log (
                        MAMA_LOG_LEVEL_ERROR,
                        "Cannot get MAMA_FIELD_TYPE_TIME from mamaMsg (%s). "
                        "Exiting.", mamaStatus_stringForStatus(status));
                exit (1);
            }

            mama_log (MAMA_LOG_LEVEL_FINEST,
                      "Adding MAMA_FIELD_TYPE_TIME to output message: %lu",
                      result);
            status = mamaMsg_addDateTime (*targetMessage, NULL, fid, result);
            if (status != MAMA_STATUS_OK)
            {
                mama_log (
                        MAMA_LOG_LEVEL_ERROR,
                        "Cannot add MAMA_FIELD_TYPE_TIME output message "
                        "type (%s). Exiting.",
                        mamaStatus_stringForStatus(status));
                exit (1);
            }
            mamaDateTime_destroy (result);
            break;
        }
        case MAMA_FIELD_TYPE_PRICE:
        {
            mamaPrice result = NULL;
            mamaPrice_create (&result);
            status = mamaMsgField_getPrice (currentField, result);
            if (status != MAMA_STATUS_OK)
            {
                mama_log (
                        MAMA_LOG_LEVEL_ERROR,
                        "Cannot get MAMA_FIELD_TYPE_PRICE from mamaMsg (%s). "
                        "Exiting.", mamaStatus_stringForStatus(status));
                exit (1);
            }

            mama_log (MAMA_LOG_LEVEL_FINEST,
                      "Adding MAMA_FIELD_TYPE_PRICE to output message.");
            status = mamaMsg_addPrice (*targetMessage, NULL, fid, result);
            if (status != MAMA_STATUS_OK)
            {
                mama_log (
                        MAMA_LOG_LEVEL_ERROR,
                        "Cannot add MAMA_FIELD_TYPE_PRICE output message "
                        "type (%s). Exiting.",
                        mamaStatus_stringForStatus(status));
                exit (1);
            }
            mamaPrice_destroy (result);
            break;
        }
        case MAMA_FIELD_TYPE_OPAQUE:
        {
            const void* result;
            mama_size_t resultLen;
            status = mamaMsgField_getOpaque (currentField, &result, &resultLen);
            if (status != MAMA_STATUS_OK)
            {
                mama_log (
                        MAMA_LOG_LEVEL_ERROR,
                        "Cannot get MAMA_FIELD_TYPE_OPAQUE from mamaMsg (%s). "
                        "Exiting.", mamaStatus_stringForStatus(status));
                exit (1);
            }

            mama_log (MAMA_LOG_LEVEL_FINEST,
                      "Adding MAMA_FIELD_TYPE_OPAQUE to output message.");
            status = mamaMsg_addOpaque (*targetMessage, NULL, fid, result,
                                        resultLen);
            if (status != MAMA_STATUS_OK)
            {
                mama_log (
                        MAMA_LOG_LEVEL_ERROR,
                        "Cannot add MAMA_FIELD_TYPE_OPAQUE output message "
                        "type (%s). Exiting.",
                        mamaStatus_stringForStatus(status));
                exit (1);
            }
            break;
        }
        case MAMA_FIELD_TYPE_MSG:
        {
            /* Recursing into this function. */
            mamaMsg internalSource = NULL;
            mamaMsg internalTarget = NULL;

            mamaMsg_createForPayloadBridge (&internalTarget,
                                            outputPayloadBridge);

            status = mamaMsgField_getMsg (currentField, &internalSource);
            if (status != MAMA_STATUS_OK)
            {
                mama_log (
                        MAMA_LOG_LEVEL_ERROR,
                        "Cannot get MAMA_FIELD_TYPE_MSG from mamaMsg (%s). "
                        "Exiting.", mamaStatus_stringForStatus(status));
                exit (1);
            }

            copyMamaMsg (internalSource, &internalTarget, outputPayloadBridge);
            mama_log (MAMA_LOG_LEVEL_FINEST,
                      "Adding MAMA_FIELD_TYPE_MSG to output message: %s",
                      mamaMsg_toString (internalTarget));
            status = mamaMsg_addMsg (*targetMessage, NULL, fid, internalTarget);
            if (status != MAMA_STATUS_OK)
            {
                mama_log (
                        MAMA_LOG_LEVEL_ERROR,
                        "Cannot add MAMA_FIELD_TYPE_MSG output message type "
                        "(%s). Exiting.", mamaStatus_stringForStatus(status));
                exit (1);
            }

            mamaMsg_destroy (internalTarget);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_BOOL:
        {
            MAMA_VECTOR_FIELD_COPY (Bool, mama_bool_t);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_CHAR:
        {
            MAMA_VECTOR_FIELD_COPY (Char, char);
            break;
        }
       case MAMA_FIELD_TYPE_VECTOR_I8:
        {
            MAMA_VECTOR_FIELD_COPY (I8, mama_i8_t);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U8:
        {
            MAMA_VECTOR_FIELD_COPY (U8, mama_u8_t);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I16:
        {
            MAMA_VECTOR_FIELD_COPY (I16, mama_i16_t);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U16:
        {
            MAMA_VECTOR_FIELD_COPY (U16, mama_u16_t);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I32:
        {
            MAMA_VECTOR_FIELD_COPY (I32, mama_i32_t);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U32:
        {
            MAMA_VECTOR_FIELD_COPY (U32, mama_u32_t);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_I64:
        {
            MAMA_VECTOR_FIELD_COPY (I64, mama_i64_t);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_U64:
        {
            MAMA_VECTOR_FIELD_COPY (U64, mama_u64_t);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_F32:
        {
            MAMA_VECTOR_FIELD_COPY (F32, mama_f32_t);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_F64:
        {
            MAMA_VECTOR_FIELD_COPY (F64, mama_f64_t);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_STRING:
        {
            MAMA_VECTOR_FIELD_COPY (String, char*);
            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_MSG:
        {
            const mamaMsg* result;
            mama_size_t resultLen;
            mamaMsg* outputMessageList = NULL;
            mama_size_t counter;

            status = mamaMsgField_getVectorMsg (currentField, &result,
                                                &resultLen);
            if (status != MAMA_STATUS_OK)
            {
                mama_log (
                        MAMA_LOG_LEVEL_ERROR,
                        "Cannot get MAMA_FIELD_TYPE_VECTOR_MSG from mamaMsg "
                        "(%s). Exiting.", mamaStatus_stringForStatus(status));
                exit (1);
            }

            outputMessageList = (mamaMsg*) calloc (resultLen,
                                                   (sizeof(mamaMsg)));

            for (counter = 0; counter < resultLen; counter++)
            {
                mamaMsg internalTarget;
                mamaMsg_createForPayloadBridge (&internalTarget,
                                                outputPayloadBridge);

                copyMamaMsg (result[counter], &internalTarget,
                             outputPayloadBridge);
                outputMessageList[counter] = internalTarget;
            }

            mama_log (MAMA_LOG_LEVEL_FINEST,
                      "Adding MAMA_FIELD_TYPE_VECTOR_MSG to output message.");
            status = mamaMsg_addVectorMsg (*targetMessage, NULL, fid,
                                           outputMessageList, resultLen);
            if (status != MAMA_STATUS_OK)
            {
                mama_log (
                        MAMA_LOG_LEVEL_ERROR,
                        "Cannot add MAMA_FIELD_TYPE_VECTOR_MSG output message "
                        "type (%s). Exiting.",
                        mamaStatus_stringForStatus(status));
                exit (1);
            }

            for (counter = 0; counter < resultLen; counter++)
            {
                mamaMsg_destroy (outputMessageList[counter]);
            }
            free (outputMessageList);

            break;
        }
        case MAMA_FIELD_TYPE_VECTOR_TIME:
            /* Not implemented in mamaMsgField_getVector format... */
            mama_log (MAMA_LOG_LEVEL_FINEST,
                      "Cannot add MAMA_FIELD_VECTOR_TIME to output message");
            break;
        case MAMA_FIELD_TYPE_VECTOR_PRICE:
            /* Not implemented in mamaMsgField_getVector format... */
            mama_log (
                    MAMA_LOG_LEVEL_FINEST,
                    "Cannot add MAMA_FIELD_TYPE_VECTOR_PRICE to output message.");
            break;
        case MAMA_FIELD_TYPE_QUANTITY:
            /* Not implemented in mamaMsgField_get format... */
            mama_log (MAMA_LOG_LEVEL_FINEST,
                      "Cannot add MAMA_FIELD_TYPE_QUANTITY to output message.");
            break;
        case MAMA_FIELD_TYPE_COLLECTION:
            /* Not implemented in mamaMsgField_get format... */
            mama_log (
                    MAMA_LOG_LEVEL_FINEST,
                    "Cannot add MAMA_FIELD_TYPE_COLLECTION to output message.");
            break;
        case MAMA_FIELD_TYPE_UNKNOWN:
            mama_log (MAMA_LOG_LEVEL_FINEST,
                      "Cannot add MAMA_FIELD_TYPE_UNKNOWN to output message.");
            break;
        default:
            break;
        }
    }

    mamaMsgIterator_destroy (iterator);
}
