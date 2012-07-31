/* $Id: captureconvert.c,v 1.1.2.2 2011/10/02 19:04:28 ianbell Exp $
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
#include "mama/mama.h"
#include "mama/msg.h"
#include "playback/playbackFileParser.h"
#include "playback/playbackcapture.h"

#include <string.h>

static const char*              gInputFilename   = NULL;
static const char*              gOutputFilename   = NULL;

static void parseCommandLine (int argc, const char** argv)
{
    int i = 0;
    for (i = 1; i < argc;)
    {
        if (strcmp (argv[i], "-input") == 0)
        {
            gInputFilename = argv[i + 1];
            i += 2;
        }
        else if (strcmp (argv[i], "-output") == 0)
        {
            gOutputFilename = argv[i + 1];
            i += 2;
        }
        else
        	i++;
    }
}
#define DELIM    ":"

int main (int argc, const char **argv)
{

	mamaPlaybackFileParser	fileParser		= NULL;
	mamaPlaybackCapture		fileCapture		= NULL;
	char*           		headerString	= NULL;
	mamaMsg        			sourceMsg		= NULL;
	mamaMsg					targetMsg		= NULL;
	mamaMsgField			aField 			= NULL;
	mamaBridge        		gMamaBridge     = NULL;
    mamaMsgIterator 		iterator 		= NULL;
    mamaFieldType 			fieldType		= MAMA_FIELD_TYPE_UNKNOWN;
    parseCommandLine (argc, argv);

     mama_loadBridge (&gMamaBridge, "wmw");
     mama_loadBridge (&gMamaBridge, "avis");
     mama_open ();

    mamaPlaybackFileParser_allocate (&fileParser);
    mamaPlaybackFileParser_openFile(fileParser, (char*)gInputFilename);

    mamaCapture_allocate(&fileCapture);
    mamaCapture_openFile (&fileCapture, gOutputFilename);

    mamaMsgIterator_create(&iterator, NULL);

    mamaMsg_createForPayload(&targetMsg, 'A');

    while (mamaPlaybackFileParser_getNextHeader(fileParser, &headerString))
    {
        if (mamaPlaybackFileParser_getNextMsg (fileParser,
                                                &sourceMsg))
        {
            char 	temp[64];
            char*	start = headerString;
            char*	end = strchr (headerString,':');

            strncpy (temp,start, end-start); temp[end-start]='\0';
            mamaCapture_setFeedSource (&fileCapture, temp);
            end++;
            start=end;
            end = strchr (start,':');
            strncpy (temp,start, end-start);temp[end-start]='\0';
            mamaCapture_setTransportName (&fileCapture, temp);
            end++;
            start=end;
            end = strchr (start,'\0');
            strncpy (temp,start, end-start);temp[end-start]='\0';
            mamaCapture_setSymbol (&fileCapture, temp);

			mamaMsgIterator_associate(iterator, sourceMsg);
			mamaMsg_clear(targetMsg);

			while ((aField = mamaMsgIterator_next(iterator)) != NULL)
			{
				uint16_t        fid             = 0;
				mamaMsgField_getFid(aField, &fid);
				mamaMsgField_getType(aField, &fieldType);
				switch (fieldType)
				{

					case  MAMA_FIELD_TYPE_BOOL:
		               { mama_bool_t result;
		                mamaMsgField_getBool (aField, &result);
						mamaMsg_addBool(targetMsg, NULL, fid, result);
						}break;
					case MAMA_FIELD_TYPE_CHAR:
						{char result;
		                mamaMsgField_getChar (aField, &result);
						mamaMsg_addChar(targetMsg, NULL, fid, result);
						}break;
					case  MAMA_FIELD_TYPE_I8:
		                {mama_i8_t result;
		                mamaMsgField_getI8 (aField, &result);
						mamaMsg_addI8(targetMsg, NULL, fid, result);
						}break;
					case  MAMA_FIELD_TYPE_U8:
		                {mama_u8_t result;
		                mamaMsgField_getU8 (aField, &result);
						mamaMsg_addU8(targetMsg, NULL, fid, result);
						}break;
					case MAMA_FIELD_TYPE_I16:
		                {mama_i16_t result;
		                mamaMsgField_getI16 (aField, &result);
						mamaMsg_addI16(targetMsg, NULL, fid, result);
						}break;
					case MAMA_FIELD_TYPE_U16:
		                {mama_u16_t result;
		                mamaMsgField_getU16 (aField, &result);
						mamaMsg_addU16(targetMsg, NULL, fid, result);
						}break;
					case  MAMA_FIELD_TYPE_I32:
		                {mama_i32_t result;
		                mamaMsgField_getI32 (aField, &result);
						mamaMsg_addI32(targetMsg, NULL, fid, result);
						}break;
					case  MAMA_FIELD_TYPE_U32:
		                {mama_u32_t result;
		                mamaMsgField_getU32 (aField, &result);
						mamaMsg_addU32(targetMsg, NULL, fid, result);
						}break;
					case MAMA_FIELD_TYPE_I64:
		                {mama_i64_t result;
		                mamaMsgField_getI64 (aField, &result);
						mamaMsg_addI64(targetMsg, NULL, fid, result);
						}break;
					case  MAMA_FIELD_TYPE_U64:
		                {mama_u64_t result;
		                mamaMsgField_getU64 (aField, &result);
						mamaMsg_addU64(targetMsg, NULL, fid, result);
						}break;
					case  MAMA_FIELD_TYPE_F32:
		                {mama_f32_t result;
		                mamaMsgField_getF32 (aField, &result);
						mamaMsg_addF32(targetMsg, NULL, fid, result);
						}break;
					case MAMA_FIELD_TYPE_F64:
		                {mama_f64_t result;
		                mamaMsgField_getF64 (aField, &result);
						mamaMsg_addF64(targetMsg, NULL, fid, result);
						}break;
					case MAMA_FIELD_TYPE_STRING:
						{const char* result;
		                mamaMsgField_getString (aField, &result);
						mamaMsg_addString(targetMsg, NULL, fid, result);
						}break;
					case  MAMA_FIELD_TYPE_TIME:
						{
							mamaDateTime result;
							mamaMsgField_getDateTime (aField, result);
							mamaMsg_addDateTime(targetMsg, NULL, fid, result);
						}break;
					case MAMA_FIELD_TYPE_PRICE:
						{
							mamaPrice result;
							mamaMsgField_getPrice(aField, result);
							mamaMsg_addPrice(targetMsg, NULL, fid, result);
						}break;
					default:
						case MAMA_FIELD_TYPE_VECTOR_I8:
						case MAMA_FIELD_TYPE_VECTOR_U8:
						case MAMA_FIELD_TYPE_VECTOR_I16:
						case MAMA_FIELD_TYPE_VECTOR_U16:
						case MAMA_FIELD_TYPE_VECTOR_I32:
						case MAMA_FIELD_TYPE_VECTOR_U32:
						case MAMA_FIELD_TYPE_VECTOR_I64:
						case MAMA_FIELD_TYPE_VECTOR_U64:
						case MAMA_FIELD_TYPE_VECTOR_F32:
						case MAMA_FIELD_TYPE_VECTOR_F64:
						case MAMA_FIELD_TYPE_VECTOR_STRING:
						case MAMA_FIELD_TYPE_VECTOR_MSG:
						case MAMA_FIELD_TYPE_VECTOR_TIME:
						case MAMA_FIELD_TYPE_VECTOR_PRICE:
						case MAMA_FIELD_TYPE_QUANTITY:
						case MAMA_FIELD_TYPE_COLLECTION :
						case MAMA_FIELD_TYPE_UNKNOWN:
						case MAMA_FIELD_TYPE_OPAQUE:
						case MAMA_FIELD_TYPE_MSG:
							break;
				}
			}
            mamaCapture_saveMamaMsg (&fileCapture, &targetMsg);
        }
    }

    mamaMsgIterator_destroy(iterator);
    mamaCapture_closeFile(fileCapture);
    mamaCapture_deallocate (fileCapture);
    mamaPlaybackFileParser_closeFile(fileParser);
    mamaPlaybackFileParser_deallocate (fileParser);
    return (0);
}
