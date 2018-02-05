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

#ifndef WOMBAT_FILEREADER_H__
#define WOMBAT_FILEREADER_H__


/*************************************************************************
 * Includes
**************************************************************************/

#include <wombat/port.h>

#if defined (__cplusplus)
extern "C"
{
#endif


/*************************************************************************
 * Typedefs, structs, enums and globals
**************************************************************************/

/* fileParser is opaque */
typedef void* fileParser;

/* Possible return codes */
typedef enum
{
    FILE_PARSER_STATUS_OK,
    FILE_PARSER_STATUS_NOMEM,
    FILE_PARSER_STATUS_ERRNO, /* Underlying failure has set errno */
    FILE_PARSER_STATUS_NULL_ARG,
    FILE_PARSER_STATUS_INVALID_ARG,
    FILE_PARSER_STATUS_INVALID_STATE,
    FILE_PARSER_STATUS_NO_HANDLE,
    FILE_PARSER_STATUS_NO_FILE,
    FILE_PARSER_STATUS_EOF,
    FILE_PARSER_STATUS_FAILURE
} fileParserStatus;

/* Which type of file parser to use */
typedef enum
{
    FILE_PARSER_TYPE_UNKNOWN,
    FILE_PARSER_TYPE_MMAP, /* Start at 1 so 0 is invalid value */
    FILE_PARSER_TYPE_FILE_STREAM
} fileParserType;


/*************************************************************************
 * Public implementation functions
**************************************************************************/

/**
 * Allocate a fileParser. A file may then be opened with it at a later point
 * using a fileParser_create call.
 *
 * @param parser This pointer will be populated with the address of the newly
 *               created fileParser implementation on function success.
 *
 * @return fileParserStatus indicating whether the method succeeded or failed.
 */
COMMONExpDLL fileParserStatus
fileParser_allocate (fileParser* parser);

/**
 * Creates a fileParser and associates it with a file and a parser type. This
 * call will also open the file and perform any additional initialization
 * required for the specified parser type.
 *
 * @param parser   The fileParser to perform this operation on.
 * @param type     The type of fileParser to initialize (FILE_PARSER_TYPE_MMAP
 *                 / FILE_PARSER_TYPE_FILE_STREAM.
 * @param fileName The file to create a fileParser for and associate it with.
 *
 * @return fileParserStatus indicating whether the method succeeded or failed.
 */
COMMONExpDLL fileParserStatus
fileParser_create (fileParser parser, fileParserType type, const char* fileName);

/**
 * This will return the size of the opened file. Note this must be called 
 * after fileParser_create.
 *
 * @param parser   The fileParser to perform this operation on.
 *
 * @return uint64_t representing the size of the file being parser.
 */
COMMONExpDLL uint64_t
fileParser_getFileSize (fileParser parser);

/**
 * Read from the current position of the fileParser provided up to a maximum
 * of destBufferSize bytes. The bytesWritten value will then be populated by
 * the function to reflect the number of bytes which have been copied to
 * destBuffer.
 *
 * Calling this function will move the internal parser position of the provided
 * fileParser object by bytesWritten bytes.
 *
 * @param parser         The fileParser to perform this operation on.
 * @param destBuffer     The the destination to copy the file contents to.
 * @param destBufferSize The writable size of destBuffer.
 * @param bytesWritten   Pointer to a value which will be populated with the
 *                       number of bytes copied across on completion.
 *
 * @return fileParserStatus indicating whether the method succeeded or failed.
 */
COMMONExpDLL fileParserStatus
fileParser_readFileToBuffer (fileParser parser,
                             void* destBuffer,
                             uint64_t destBufferSize,
                             uint64_t* bytesWritten);

/**
 * Read from the current position of the fileParser provided up to a maximum
 * of destBufferSize bytes, up to and including the first detected instance of
 * the delim character. The bytesWritten value will then be populated by
 * the function to reflect the number of bytes which have been copied to
 * destBuffer.
 *
 * Calling this function will move the internal parser position of the provided
 * fileParser object by bytesWritten bytes.
 *
 * @param parser         The fileParser to perform this operation on.
 * @param destBuffer     The the destination to copy the file contents to.
 * @param destBufferSize The writable size of destBuffer.
 * @param delim          The character which, when encountered, will cause
 *                       copying to stop (destination buffer will then include
 *                       this delimiter).
 * @param bytesWritten   Pointer to a value which will be populated with the
 *                       number of bytes copied across on completion.
 *
 * @return fileParserStatus indicating whether the method succeeded or failed.
 */
COMMONExpDLL fileParserStatus
fileParser_readFileToBufferUntilCharacter (
                             fileParser parser,
                             void* destBuffer,
                             uint64_t destBufferSize,
                             char delim,
                             uint64_t* bytesWritten);

/**
 * Calling this function will move the internal parser position of the provided
 * fileParser object to the beginning of its file.
 *
 * @param parser         The fileParser to perform this operation on.
 *
 * @return fileParserStatus indicating whether the method succeeded or failed.
 */
COMMONExpDLL fileParserStatus
fileParser_rewindFile (fileParser parser);

/**
 * This function will return '1' if the file being parsed is at EOF. Otherwise
 * this will return '0'.
 *
 * @param parser         The fileParser to perform this operation on.
 *
 * @return int (acting as a boolean) indicating whether the method succeeded or
 *         failed.
 */
COMMONExpDLL int
fileParser_isEndOfFile (fileParser parser);

/**
 * This will return a string representation of the provided fileParser status
 * code.
 *
 * @param status         The status to perform this operation on.
 *
 * @return const char* referencing a string representation of the status.
 */
COMMONExpDLL const char*
fileParser_stringForStatus (fileParserStatus status);

/**
 * This will close the file being parsed by the file parser and perform any
 * additional cleanup required which is associated with the fileParser's type.
 *
 * @param parser         The fileParser to perform this operation on.
 *
 * @return fileParserStatus indicating whether the method succeeded or failed.
 */
COMMONExpDLL fileParserStatus
fileParser_closeFile (fileParser parser);

/**
 * Destroy the fileParser implementation and close any open files associated
 * with it.
 *
 * @param parser         The fileParser to perform this operation on.
 *
 * @return fileParserStatus indicating whether the method succeeded or failed.
 */
COMMONExpDLL fileParserStatus
fileParser_destroy (fileParser parser);

#if defined (__cplusplus)
}
#endif

#endif /* WOMBAT_FILEREADER_H__ */

