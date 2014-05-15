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

/*************************************************************************
 * Includes
**************************************************************************/

#include <wombat/fileparser.h>
#include <wombat/port.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/*************************************************************************
 * Typedefs, structs, enums and globals
**************************************************************************/

/* Internal implementation */
typedef struct 
{
    int                 mMmapOpenFlags;         /* Used for mmap */
    int                 mMmapOpenMode;          /* Used for mmap */
    int                 mMmapProtect;           /* Used for mmap */
    int                 mMmapFlags;             /* Used for mmap */
    int                 mFileDescriptor;        /* Used for mmap */
    uint8_t*            mFileRootPosition;      /* Used for mmap */
    uint8_t*            mFileCurrentPosition;   /* Used for mmap */
    FILE*               mFileHandle;            /* Used for file streams */
    const char*         mFileMode;              /* Used for file streams */
    uint64_t            mFileSize;
    uint64_t            mFileCurrentOffset;
    const char*         mFileName;
    fileParserType      mType;
} fileParserImpl;


/*************************************************************************
 * Public implementation functions
**************************************************************************/

fileParserStatus
fileParser_allocate (fileParser* parser)
{
    fileParserImpl* newReader = NULL;

    if (NULL == parser)
    {
        return FILE_PARSER_STATUS_NULL_ARG;
    }

    newReader = (fileParserImpl*) calloc (1, sizeof(fileParserImpl));

    if (NULL == newReader)
    {
        return FILE_PARSER_STATUS_NOMEM;
    }

    newReader->mMmapOpenFlags = O_RDONLY;
    newReader->mMmapProtect = PROT_READ;
    newReader->mMmapFlags = MAP_SHARED;
    newReader->mType = FILE_PARSER_TYPE_UNKNOWN;
    newReader->mFileMode = strdup ("rb");

    if (NULL == newReader->mFileMode)
    {
        free (newReader);
        return FILE_PARSER_STATUS_NOMEM;
    }

    *parser = (fileParser) newReader;
    return FILE_PARSER_STATUS_OK;
}

fileParserStatus
fileParser_create (fileParser parser, fileParserType type, const char* fileName)
{
    struct stat     fileInfo;
    int             result  = 0;
    fileParserImpl* impl    = (fileParserImpl*) parser;

    if (NULL == impl || NULL == fileName)
    {
        return FILE_PARSER_STATUS_NULL_ARG;
    }

    if (0 == strlen(fileName))
    {
        return FILE_PARSER_STATUS_INVALID_ARG;
    }

    result = stat(fileName, &fileInfo);

    /* If we failed to access the file */
    if (result)
    {
        return FILE_PARSER_STATUS_NO_FILE;
    }

    impl->mFileSize = fileInfo.st_size;
    impl->mFileName = strdup (fileName);

    if (NULL == impl->mFileName)
    {
        /* Clean up what we can */
        fileParser_destroy (parser);
        return FILE_PARSER_STATUS_NOMEM;
    }

    impl->mType = type;

    switch (impl->mType)
    {
    case FILE_PARSER_TYPE_MMAP:
    {
        /* Open up the file */
        impl->mFileDescriptor = open (fileName, impl->mMmapOpenFlags);
        if (impl->mFileDescriptor < 0)
        {
            return FILE_PARSER_STATUS_NO_HANDLE;
        }

        /* Map the file to memory */
        impl->mFileRootPosition = 
            (uint8_t*) mmap (NULL, 
                             (size_t) impl->mFileSize,
                             impl->mMmapProtect,
                             impl->mMmapFlags,
                             impl->mFileDescriptor,
                             0); /* 0 = Map from the first byte */
        if (MAP_FAILED == impl->mFileRootPosition)
        {
            return FILE_PARSER_STATUS_FAILURE;
        }

        /* Mark current position as the start */
        impl->mFileCurrentPosition = impl->mFileRootPosition;
        break;
    }
    case FILE_PARSER_TYPE_FILE_STREAM:
    {
        /* Open the file handle */
        impl->mFileHandle = fopen (fileName, impl->mFileMode);
        if (NULL == impl->mFileHandle)
        {
            return FILE_PARSER_STATUS_NO_HANDLE;
        }
        break;
    }
    default:
    {
        return FILE_PARSER_STATUS_INVALID_ARG;
        break;
    }
    }
    return FILE_PARSER_STATUS_OK;
}

uint64_t
fileParser_getFileSize (fileParser parser)
{
    fileParserImpl* impl = (fileParserImpl*) parser;

    if (NULL == impl)
    {
        return FILE_PARSER_STATUS_NULL_ARG;
    }

    return impl->mFileSize;
}

fileParserStatus
fileParser_readFileToBuffer (fileParser parser,
                            void* destBuffer,
                            uint64_t destBufferSize,
                            uint64_t* bytesWritten)
{
    fileParserImpl* impl        = (fileParserImpl*) parser;
    char*           destAsBytes = (char*) destBuffer;
    int             i           = 0;
    uint64_t        bytesToCopy = destBufferSize;

    if (NULL == parser || NULL == destBuffer || NULL == bytesWritten)
    {
        return FILE_PARSER_STATUS_NULL_ARG;
    }

    if (0 == destBufferSize)
    {
        return FILE_PARSER_STATUS_INVALID_ARG;
    }

    switch (impl->mType)
    {
    case FILE_PARSER_TYPE_MMAP:
    {
        /* If there are fewer bytes in the file than requested, copy only the
         * bytes remaining in the file to avoid overrun. */
        if (impl->mFileSize < (impl->mFileCurrentOffset + bytesToCopy))
        {
            bytesToCopy = impl->mFileSize - impl->mFileCurrentOffset;
        }

        memcpy (destBuffer, impl->mFileCurrentPosition, (size_t) bytesToCopy);
        impl->mFileCurrentPosition += bytesToCopy;
        impl->mFileCurrentOffset += bytesToCopy;
        break;
    }
    case FILE_PARSER_TYPE_FILE_STREAM:
    {
        /* Returns the number of bytes read / written */
        *bytesWritten = fread (destBuffer,
                                1,
                                (size_t) bytesToCopy,
                                impl->mFileHandle);

        /* Find out where the file handle position is now */
        impl->mFileCurrentOffset = ftell (impl->mFileHandle);
        break;
    }
    default:
    {
        /* This would indicate an unknown or unpopulated parser type */
        return FILE_PARSER_STATUS_INVALID_STATE;
    }
    }

    return FILE_PARSER_STATUS_OK;
}

fileParserStatus
fileParser_readFileToBufferUntilCharacter (fileParser parser,
                            void* destBuffer,
                            uint64_t destBufferSize,
                            char delim,
                            uint64_t* bytesWritten)
{
    fileParserImpl* impl        = (fileParserImpl*) parser;
    char*           destAsBytes = (char*) destBuffer;
    int             i           = 0;

    if (NULL == parser || NULL == destBuffer || NULL == bytesWritten)
    {
        return FILE_PARSER_STATUS_NULL_ARG;
    }

    if (0 == destBufferSize)
    {
        return FILE_PARSER_STATUS_INVALID_ARG;
    }

    switch (impl->mType)
    {
    case FILE_PARSER_TYPE_MMAP:
    {
        /*  While we have bytes remaining in destination and we have not yet
         * reached the delimiter */
        do
        {
            destAsBytes[i] = (char) *(impl->mFileCurrentPosition);
            impl->mFileCurrentOffset++;
            impl->mFileCurrentPosition++;
        } while (i < destBufferSize
                    && impl->mFileCurrentOffset < impl->mFileSize
                    && destAsBytes[i++] != delim);

        /* Update the bytes written for the calling function */
        *bytesWritten = i - 1;

        break;
    }
    case FILE_PARSER_TYPE_FILE_STREAM:
    {
        /*  While we have bytes remaining in destination and we have not yet
         * reached the delimiter */
        do
        {
            destAsBytes[i] = fgetc (impl->mFileHandle);
        } while (i < destBufferSize && destAsBytes[i++] != delim);

        /* Find out where the file handle position is now */
        impl->mFileCurrentOffset = (uint64_t) ftell (impl->mFileHandle);

        /* The position we are in now relative to the original offset */
        *bytesWritten = i - 1;

        break;
    }
    default:
    {
        return FILE_PARSER_STATUS_INVALID_STATE;
    }
    }

    return FILE_PARSER_STATUS_OK;
}

fileParserStatus
fileParser_rewindFile (fileParser parser)
{
    fileParserImpl* impl = (fileParserImpl*) parser;

    if (NULL == parser)
    {
        return FILE_PARSER_STATUS_NULL_ARG;
    }

    if (FILE_PARSER_TYPE_UNKNOWN == impl->mType)
    {
        return FILE_PARSER_STATUS_INVALID_STATE;
    }

    switch (impl->mType)
    {
    case FILE_PARSER_TYPE_MMAP:
    {
        /* For mmap, we just point back to the first byte again */
        impl->mFileCurrentPosition = impl->mFileRootPosition;
        impl->mFileCurrentOffset   = 0;
        break;
    }
    case FILE_PARSER_TYPE_FILE_STREAM:
    {
        /* For file streams, set file pointer to start of file */
        if (0 != fseek (impl->mFileHandle, 0, SEEK_SET))
        {
            return FILE_PARSER_STATUS_FAILURE;
        }
        impl->mFileCurrentOffset = 0;
        break;
    }
    default:
    {
        return FILE_PARSER_STATUS_INVALID_STATE;
    }
    }

    return FILE_PARSER_STATUS_OK;
}

int
fileParser_isEndOfFile (fileParser parser)
{
    fileParserImpl* impl = (fileParserImpl*) parser;

    if (impl->mFileCurrentOffset >= impl->mFileSize)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

const char*
fileParser_stringForStatus (fileParserStatus status)
{
    switch (status)
    {
    case FILE_PARSER_STATUS_OK : return "FILE_PARSER_STATUS_OK";
    case FILE_PARSER_STATUS_NOMEM : return "FILE_PARSER_STATUS_NOMEM";
    case FILE_PARSER_STATUS_ERRNO : return "FILE_PARSER_STATUS_ERRNO";
    case FILE_PARSER_STATUS_NULL_ARG : return "FILE_PARSER_STATUS_NULL_ARG";
    case FILE_PARSER_STATUS_INVALID_ARG : return "FILE_PARSER_STATUS_INVALID_ARG";
    case FILE_PARSER_STATUS_INVALID_STATE : return "FILE_PARSER_STATUS_INVALID_STATE";
    case FILE_PARSER_STATUS_NO_HANDLE : return "FILE_PARSER_STATUS_NO_HANDLE";
    case FILE_PARSER_STATUS_NO_FILE : return "FILE_PARSER_STATUS_NO_FILE";
    case FILE_PARSER_STATUS_EOF : return "FILE_PARSER_STATUS_EOF";
    case FILE_PARSER_STATUS_FAILURE : return "FILE_PARSER_STATUS_FAILURE";
    default : return "unknown";
    }
}

fileParserStatus
fileParser_closeFile (fileParser parser)
{
    fileParserImpl* impl = (fileParserImpl*) parser;

    if (NULL == parser)
    {
        return FILE_PARSER_STATUS_NULL_ARG;
    }

    if (0 == (int) impl->mType)
    {
        return FILE_PARSER_STATUS_INVALID_STATE;
    }

    /* If the file handle is set, close it */
    if (NULL != impl->mFileHandle)
    {
        if (0 != fclose (impl->mFileHandle))
        {
            return FILE_PARSER_STATUS_FAILURE;
        }
        impl->mFileHandle = NULL;
    }

    /* If the file descriptor is set, close it */
    if (0 != impl->mFileDescriptor)
    {
        if (0 != munmap (impl->mFileRootPosition, (size_t) impl->mFileSize))
        {
            return FILE_PARSER_STATUS_FAILURE;
        }
        impl->mFileDescriptor = 0;
    }

    /* Reset type to unknown as this is in an unusable state until create is
     * called again */
    impl->mType = FILE_PARSER_TYPE_UNKNOWN;

    return FILE_PARSER_STATUS_OK;
}

fileParserStatus
fileParser_destroy (fileParser parser)
{
    fileParserImpl*     impl    = (fileParserImpl*) parser;
    fileParserStatus    status  = FILE_PARSER_STATUS_OK;

    if (NULL == impl)
    {
        return FILE_PARSER_STATUS_NULL_ARG;
    }

    /* File mode was strdup'd - free it before impl is free'd */
    if (NULL != impl->mFileMode)
    {
        free ((void*) impl->mFileMode);
    }

    /* File name was strdup'd - free it before impl is free'd */
    if (NULL != impl->mFileName)
    {
        free ((void*) impl->mFileName);
    }

    /* Close the file. If already closed, this should still be safe */
    status = fileParser_closeFile (parser);

    free (impl);

    return status;
}

