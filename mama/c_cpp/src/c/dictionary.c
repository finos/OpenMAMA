/* $Id: dictionary.c,v 1.47.22.2 2011/08/10 14:53:25 nicholasmarriott Exp $
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

#include "wombat/wincompat.h"

#include "stdlib.h"
#include "string.h"
#include "stdio.h"

#include "mama/mama.h"
#include "mama/reservedfields.h"
#include "dictionaryimpl.h"
#include "bridge.h"
#include "fileutils.h"

#define self ((mamaDictionaryImpl*)(dictionary))
#define userSymbolFormatted userSymbol ? userSymbol : "", \
                            userSymbol ? ":" : ""


typedef struct mamaDictionaryImpl_
{
    mamaFieldDescriptor*        mDict;    /* dictionary - array of fields,
                                                        not always packed */
    size_t                      mDictSize;               /* size of array */
    size_t                      mSize;      /* number of entries in mDict */
    mama_fid_t                  mMaxFid;   /* derived from the incoming 
                                 mamaMsg, this will determine mDictSize */
    int                         mHasDuplicates; /* boolean - are there fields
                                   with the same name (and different fids)? */
    mamaDictionaryCallbackSet   mCallbackSet;
    void*                       mClosure; 
    mamaSubscription            mSubscription;
    char*                       mFeedHost;
    char*                       mFeedName;
} mamaDictionaryImpl;


/* Forward declarations */
static void determineMaxFid( mamaDictionary dictionary, mamaMsg msg );
static void getAndStoreFields( mamaDictionary dictionary, mamaMsg msg );
static void checkForDuplicateNames( mamaDictionary dictionary );
static char* copyString( const char* str );
static void checkFree( char** str );

/*Used to grow the dictionary array if necessary to contain new field 
 descritors.*/
static mama_status ensureCapacity (mamaDictionaryImpl* impl, mama_fid_t fid);

static mama_status createDictArray (mamaDictionaryImpl* impl);

/*Used from getDictionaryMessage() to create a dict message for subsequent
 * publishing*/
static void populateMessageFromDictionary (mamaDictionaryImpl* impl,
                                           mamaMsg             msg);


/* These are the default callback methods. Users may specify an alternative
 * set of callbacks by calling the mamaDictionary_setXXXCallback()
 * methods.
 */
static void MAMACALLTYPE
mamaDictionaryDefaultCallback_onTimeout( mamaDictionary dict,
                                             void *closure )
{
    mamaDictionaryImpl* impl = (mamaDictionaryImpl*)dict;
    const char*         userSymbol = NULL;

    mamaSubscription_getSymbol (impl->mSubscription, &userSymbol);
    mama_log (MAMA_LOG_LEVEL_NORMAL,
              "%s%s Error: mamaDictionary: timed out waiting for data dictionary.",
              userSymbolFormatted);
}

static void MAMACALLTYPE
mamaDictionaryDefaultCallback_onError ( mamaDictionary dict,
                                        const char* errorStr,
                                        void *closure )
{
    mamaDictionaryImpl* impl = (mamaDictionaryImpl*)dict;
    const char*         userSymbol = NULL;

    mamaSubscription_getSymbol (impl->mSubscription, &userSymbol);
    mama_log (MAMA_LOG_LEVEL_NORMAL, "%s%s %s",
              userSymbolFormatted, errorStr != NULL ? errorStr : "" );
}

static void MAMACALLTYPE
mamaDictionaryDefaultCallback_onComplete( mamaDictionary dict,
                                              void *closure )
{
}

mama_status
mamaDictionary_create (mamaDictionary* dictionary)
{
    mamaDictionaryImpl* impl =
        (mamaDictionaryImpl*)calloc( 1, sizeof( mamaDictionaryImpl ) );
    if (impl == NULL) return MAMA_STATUS_NOMEM;

    impl->mCallbackSet.onError    = mamaDictionaryDefaultCallback_onError;
    impl->mCallbackSet.onTimeout  = mamaDictionaryDefaultCallback_onTimeout;
    impl->mCallbackSet.onComplete = mamaDictionaryDefaultCallback_onComplete;

    impl->mMaxFid = 0;/*Default for when manually creating a dictionary*/
    impl->mFeedHost = NULL;
    impl->mFeedName = NULL;
    
    *dictionary =  impl;

    return MAMA_STATUS_OK;
}

mama_status
mamaDictionary_destroy (mamaDictionary dictionary)
{
    uint32_t i = 0; 

    if (dictionary == NULL) return MAMA_STATUS_OK;

    if (self->mDict)
    {
        for (i = 0; i <= self->mMaxFid; ++i)
        {
            if (self->mDict[i] )
	        {   
                mamaFieldDescriptor_destroy (self->mDict[i]);
	        }
        }
    }

    if (self->mSubscription)
    {
        mamaSubscription_destroy (self->mSubscription);
        mamaSubscription_deallocate (self->mSubscription);
    }

    if (self->mDict)
    {
        free(self->mDict);
    }
    
    checkFree(&self->mFeedName);
    checkFree(&self->mFeedHost);

    memset (self, 0, sizeof( mamaDictionaryImpl));
    free (self);

    return MAMA_STATUS_OK;
}


mama_status
mamaDictionary_setSubscPtr( mamaDictionary dictionary, 
                            mamaSubscription subsc )
{
    self->mSubscription = subsc;
    return MAMA_STATUS_OK;
}

mama_status
mamaDictionary_getFeedName( mamaDictionary dictionary,
                            const char** result)
{
    if (NULL == self)
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    if (NULL != self->mFeedName)
    {
        *result = self->mFeedName;
        return MAMA_STATUS_OK;
    }

    return MAMA_STATUS_NOT_FOUND;
}

mama_status
mamaDictionary_getFeedHost( mamaDictionary dictionary,
                            const char** result)
{
    if (NULL == self)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
  
    if (NULL != self->mFeedName)
    {
        *result = self->mFeedHost;
        return MAMA_STATUS_OK;
    }

    return MAMA_STATUS_NOT_FOUND;
}

/* param result (out) might be sent back NULL */
mama_status
mamaDictionary_getFieldDescriptorByFid (
    mamaDictionary        dictionary,
    mamaFieldDescriptor*  result,
    mama_fid_t            fid)
{
    if (self == NULL)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (fid >= self->mDictSize)
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    *result = self->mDict[fid];

    return MAMA_STATUS_OK;
}


mama_status
mamaDictionary_getFieldDescriptorByIndex (
    mamaDictionary        dictionary,
    mamaFieldDescriptor*  result,
    unsigned short        index)
{

    size_t i;
    unsigned short curIndex = 0;

    if (self == NULL)
    {
        return MAMA_STATUS_NULL_ARG;
    }

    if (index >= self->mSize)
    {
        return MAMA_STATUS_INVALID_ARG; 
    }


    for (i = 0; i < self->mDictSize; ++i)
    {
        if (self->mDict[i] != NULL)
        {
            if (curIndex == index)
            {
                *result = self->mDict[i];
                return MAMA_STATUS_OK;
            }
            curIndex++;
        }
    }

    return MAMA_STATUS_NOT_FOUND;
}

/* 
 * If more than one field has the same name, the one with the lowest 
 * fid is returned 
 */
mama_status
mamaDictionary_getFieldDescriptorByName(
    mamaDictionary        dictionary,
    mamaFieldDescriptor*  result,
    const char*           fname)
{
    size_t  i;
    mamaFieldDescriptor     rval = NULL;

    *result = NULL;

    if( self == NULL || fname == NULL )     return MAMA_STATUS_NULL_ARG;


    for (i = 0; i < self->mDictSize; ++i)
    {
        rval = self->mDict[i];
        
        if (rval &&
            (strcmp(mamaFieldDescriptor_getName(rval), fname) == 0))
        {
            *result = rval;
            return MAMA_STATUS_OK;
        }
    }

    return MAMA_STATUS_NOT_FOUND;
}



mama_status
mamaDictionary_getFieldDescriptorByNameAll (
    mamaDictionary        dictionary,
    const char*           fname,
    mamaFieldDescriptor*  descList,
    size_t*               size)
{
    mamaFieldDescriptor  info     = 0;
    size_t               count    = 0;
    mama_fid_t           i;

    mamaFieldDescriptor* list = (mamaFieldDescriptor *)descList;

    if (self == NULL || fname == NULL || size == NULL || (*size <= 0) || descList == NULL)
    {
        return MAMA_STATUS_INVALID_ARG;
    }

    for (i = 0; i < self->mDictSize; ++i)
    {
        info = self->mDict[i];
        if (info &&
            (strcmp(mamaFieldDescriptor_getName(info), fname) == 0))
        {
             list[count++] = info;
             if (count == *size)
                 break;
        }
    }
    *size = count;

    return MAMA_STATUS_OK;
}

mama_status
mamaDictionary_getMaxFid (
    mamaDictionary  dictionary,
    mama_fid_t*     value)
{
    if (self == NULL)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *value = self->mMaxFid;
    return MAMA_STATUS_OK;
}


mama_status 
mamaDictionary_getSize(
    mamaDictionary  dictionary,
    size_t*         value)
{
    if (self == NULL)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *value = self->mSize;
    return MAMA_STATUS_OK;
}


mama_status
mamaDictionary_hasDuplicates(
    mamaDictionary  dictionary,
    int*            value)
{
    if (self == NULL)
    {
        return MAMA_STATUS_INVALID_ARG;
    }
    *value = self->mHasDuplicates;
    return MAMA_STATUS_OK;
}



mama_status
mamaDictionary_setCompleteCallback( mamaDictionary dictionary,
                    mamaDictionary_completeCallback cb )
{
    if( self == NULL ) return MAMA_STATUS_NULL_ARG;

    self->mCallbackSet.onComplete = cb;
    
    return MAMA_STATUS_OK;
}


mama_status
mamaDictionary_setTimeoutCallback( mamaDictionary dictionary,
                    mamaDictionary_timeoutCallback cb  )
{
    if( self == NULL ) return MAMA_STATUS_NULL_ARG;

    self->mCallbackSet.onTimeout = cb;
    
    return MAMA_STATUS_OK;
}


mama_status
mamaDictionary_setErrorCallback( 
    mamaDictionary dictionary,
    mamaDictionary_errorCallback cb )
{
    if( self == NULL ) return MAMA_STATUS_NULL_ARG;

    self->mCallbackSet.onError = cb;
    
    return MAMA_STATUS_OK;
}

static void 
processMsg( mamaDictionary dictionary, const mamaMsg msg )
{
    mamaMsgStatus     msgStatus = -1;
    mamaMsgType       msgType = -1;

    msgStatus = mamaMsgStatus_statusForMsg( msg ); 
    msgType   = mamaMsgType_typeForMsg( msg );

    if( msgStatus == MAMA_MSG_STATUS_TIMEOUT )
    {
        self->mCallbackSet.onTimeout( dictionary, self->mClosure );
        return;
    }

    if( msgStatus != MAMA_MSG_STATUS_OK && 
        msgType   != MAMA_MSG_TYPE_DDICT_SNAPSHOT &&
        msgType   != MAMA_MSG_TYPE_UPDATE ) 
    {
        char errBuf[1024];

        snprintf( errBuf, 1023, "Error: mamaDictionary: unexpected "
                                    "MsgType/MsgStatus: %s/%s\n",
               mamaMsgType_stringForType(msgType),
               mamaMsgStatus_stringForStatus(msgStatus));

        self->mCallbackSet.onError( dictionary, errBuf, self->mClosure );
        return;
    }

    mamaDictionary_buildDictionaryFromMessage( dictionary, msg );

    mamaSubscription_destroy (self->mSubscription);
    mamaSubscription_deallocate (self->mSubscription);
    self->mSubscription = NULL;

    /* do this last in case the dictionary transport is destroyed
       in the callback */
    self->mCallbackSet.onComplete( dictionary, self->mClosure );
}

mama_status 
mamaDictionary_buildDictionaryFromMessage(
        mamaDictionary dictionary,
        const mamaMsg msg )
{
    determineMaxFid( dictionary, msg );
    getAndStoreFields( dictionary, msg );
    checkForDuplicateNames( dictionary );
    return MAMA_STATUS_OK;
}

mama_status
mamaDictionary_getDictionaryMessage (
        mamaDictionary dictionary,
        mamaMsg* msg)
{
    mamaDictionaryImpl* impl = (mamaDictionaryImpl*)dictionary;

    mamaMsg tempMsg = NULL;

    if (!impl) return MAMA_STATUS_NULL_ARG;
    if (!msg) return MAMA_STATUS_INVALID_ARG;

    *msg  = NULL;
    
    mamaMsg_create (&tempMsg);

    populateMessageFromDictionary (impl, tempMsg);

    *msg = tempMsg;

    return MAMA_STATUS_OK;
}

mama_status
mamaDictionary_createFieldDescriptor (
                mamaDictionary       dictionary,
                mama_fid_t           fid,
                const char*          name, 
                mamaFieldType        type,
                mamaFieldDescriptor* descriptor)
{
    mamaFieldDescriptor    newDescriptor = NULL;
    mamaDictionaryImpl* impl = (mamaDictionaryImpl*)dictionary;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    /*The first time in we will have to create the array for Fd's*/
    if (!impl->mDict) createDictArray (impl);
    
    mamaFieldDescriptor_create (&newDescriptor,
                                fid,
                                type,
                                name);

    /*Make sure we have room to hold the field descriptor*/
    ensureCapacity (impl, fid);
   
    /*Add the new descriptor to the array*/
    self->mDict[fid] = newDescriptor;

    /*All fd's get added here so increment the total*/
    impl->mSize++;

    /*Users may not care about receiving the address of the new FD*/
    if (descriptor) *descriptor = newDescriptor;

    return MAMA_STATUS_OK;
}

mama_status
mamaDictionary_setMaxFid (
                mamaDictionary       dictionary,
                mama_size_t          maxFid)
{
    mamaDictionaryImpl* impl = (mamaDictionaryImpl*)dictionary;
    if (!impl) return MAMA_STATUS_NULL_ARG;
    /*If the dictionary has already been created simply ensure that it
     is big enough. I.e. a platform supplied data dictionary*/
    if (impl->mDict)
    {
        ensureCapacity (impl, maxFid);
    }
    else
    {
        /*The dictionary array will be created on the first call to 
         addFieldDescriptor()*/
        impl->mMaxFid = maxFid;
    }
    return MAMA_STATUS_OK;
}

mama_status
mamaDictionary_writeToFile (
                mamaDictionary       dictionary,
                const char*          fileName)
{
    char                lineBuffer[256];
    FILE*               outfile =   NULL;
    int                 i       =   0;
    mamaDictionaryImpl* impl    =   (mamaDictionaryImpl*)dictionary;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (MAMA_STATUS_OK!=openWombatFile (fileName, "w", &outfile))
    {
        mama_log (MAMA_LOG_LEVEL_FINE, "Could not open file for writing [%s]",
                                        fileName);
        return MAMA_STATUS_IO_ERROR;
    }

    for (i = 0;i<impl->mDictSize;i++)
    {
        if (impl->mDict[i])
        {
            snprintf (lineBuffer, 256, "%d|%s|%d\n",
                    mamaFieldDescriptor_getFid (impl->mDict[i]),
                    mamaFieldDescriptor_getName (impl->mDict[i]),
                    mamaFieldDescriptor_getType (impl->mDict[i]));

            if (EOF == fputs (lineBuffer, outfile))
            {
                mama_log (MAMA_LOG_LEVEL_FINE, "Could not write line to dict"
                        "file. [%s, %s]",fileName, lineBuffer);
                return MAMA_STATUS_IO_ERROR;
            }
        }
    }
    fclose (outfile);
    return MAMA_STATUS_OK;
}

/*Return a pointer to the next string in the delimited buffer*/
static char* getNextDelimStr (char* c)
{
    while (*c!='|') c++;
        *c = '\0';
    c++;/*move past nul char*/
    return c;
}

/*
   Simply confirms that there are two delimeters in the string
*/
static mama_status validateDelimLine (char* c)
{
    char* firstDelim = NULL;

    /*Check for the first delimeter*/
    if (NULL==(firstDelim=strchr (c, '|')))
    {
        mama_log (MAMA_LOG_LEVEL_FINE, "No delimeter found [%s]",c);
        return MAMA_STATUS_PLATFORM;
    }

    /*Check that there are actually two distinct delimeters*/
    if (firstDelim==strrchr (c, '|'))
    {
        mama_log (MAMA_LOG_LEVEL_FINE, "Only one delimeter found [%s]",c);
        return MAMA_STATUS_PLATFORM;
    }
    return MAMA_STATUS_OK;
}

mama_status
mamaDictionary_populateFromFile (
        mamaDictionary       dictionary,
        const char*          fileName)
{
    char                lineBuffer[256];
    FILE*               infile =   NULL;
    mamaDictionaryImpl* impl    =   (mamaDictionaryImpl*)dictionary;
    if (!impl) return MAMA_STATUS_NULL_ARG;

    if (MAMA_STATUS_OK!=openWombatFile (fileName, "r", &infile))
    {
        mama_log (MAMA_LOG_LEVEL_FINE, "Could not open file for reading [%s]",
                                        fileName);
        return MAMA_STATUS_IO_ERROR;
    }

    while (fgets (lineBuffer, 255, infile) != NULL)
    {
        mama_status status      = MAMA_STATUS_OK;
        char*       c           = lineBuffer;
        char*       fidStr      = NULL;
        char*       fieldName   = NULL;
        char*       typeStr     = NULL;

        /*Get rid of any newline characters*/
        stripTrailingWhiteSpace (lineBuffer);

        /*Validate the line - continue if invalid*/
        if (MAMA_STATUS_OK!=validateDelimLine (c))
            continue;

        /*First field is the fid*/
        fidStr = c;

        /*Second field is the field name*/
        fieldName = getNextDelimStr (c);

        /*Third field is the field type*/
        typeStr = getNextDelimStr (c);

        /*Add the descriptor to the dictionary*/
        if (MAMA_STATUS_OK!=(status=mamaDictionary_createFieldDescriptor
                                     (dictionary,
                                     atoi (fidStr),
                                     fieldName,
                                     atoi (typeStr),
                                     NULL)))
         {
            mama_log (MAMA_LOG_LEVEL_FINE, "Could not create field descriptor"
                    "from file. [%s] [%s]",fileName, lineBuffer);
            return status;
         }
    
    }
    fclose (infile);
    return MAMA_STATUS_OK;
}

mama_status
mamaDictionary_setClosure(
    mamaDictionary                   dictionary,
    void*                            closure)
{
    if (!self) return MAMA_STATUS_INVALID_ARG;
    self->mClosure = closure;
    return MAMA_STATUS_OK;
}

static void MAMACALLTYPE
dictMsgIterator (const mamaMsg       msg,
                 const mamaMsgField  field,
                 void*               closure)
{
    mamaDictionaryImpl*  impl           = (mamaDictionaryImpl*) closure;
    const char*          fieldName      = NULL;
    mama_fid_t           fid            = 0;
    mamaFieldType        type           = 0;
    const char*          val            = NULL;

    mamaMsgField_getType(field,&type);
    mamaMsgField_getFid(field,&fid);
    mamaMsgField_getName(field,&fieldName);
    
    if (MamaFieldFeedName.mFid == fid)
    {
        if (MAMA_STATUS_OK == mamaMsg_getString (msg, fieldName, fid, &val))
        {
            checkFree(&impl->mFeedName);
            impl->mFeedName = copyString(val);
            mama_log ( MAMA_LOG_LEVEL_FINE,
                       "Dictionary source feed name: %s",
                       val);
        }
    }
    else if (MamaFieldFeedHost.mFid == fid)
    {
        if (MAMA_STATUS_OK == mamaMsg_getString (msg, fieldName, fid, &val))
        {
            checkFree(&impl->mFeedHost);
            impl->mFeedHost = copyString(val);
            mama_log ( MAMA_LOG_LEVEL_FINE,
                       "Dictionary source hostname: %s",
                       val);
        }
    }
    
    mamaDictionary_createFieldDescriptor (impl,
                                       fid,
                                       fieldName,
                                       type,
                                       NULL);
}

static void 
getAndStoreFields (mamaDictionary dictionary, mamaMsg msg)
{
    mamaMsg_iterateFields (msg, dictMsgIterator, NULL, self);
}

static void MAMACALLTYPE
maxIterator(const mamaMsg       msg,
            const mamaMsgField  field,
            void*               closure)
{
    mamaDictionaryImpl*  impl = (mamaDictionaryImpl*) closure;
    mama_fid_t           fid  = 0;
    if (MAMA_STATUS_OK == mamaMsgField_getFid(field, &fid))
    {
        if (impl->mMaxFid < fid)
            impl->mMaxFid = fid;
    }
}

static void 
determineMaxFid (mamaDictionary dictionary, mamaMsg msg)
{
    const char* userSymbol;

    /* Determine the maximum field id */
    mamaMsg_iterateFields (msg, maxIterator, NULL, self);

    if (dictionary->mSubscription)
    {
        mamaSubscription_getSymbol (dictionary->mSubscription, &userSymbol);
        mama_log (MAMA_LOG_LEVEL_FINE, "%s%s DICT maxFid: %d",
              userSymbolFormatted, self->mMaxFid);
    }
    else
    {
        mama_log (MAMA_LOG_LEVEL_FINE, "DICT maxFid: %d", self->mMaxFid);
    }
}


static void
checkForDuplicateNames (mamaDictionary dictionary)
{
    uint32_t  i;
    uint32_t  j;

    mamaFieldDescriptor info1, info2;
    const char *name1, *name2;
    self->mHasDuplicates = 0;

    for (i = 0; i < self->mMaxFid; ++i)
    {
        info1 = self->mDict[i];

        if (!info1) continue;

        name1 = mamaFieldDescriptor_getName(info1);
        for (j = i+1; j <= self->mMaxFid; ++j)
        {
            info2 = self->mDict[j];

            if (!info2) continue;

            name2 = mamaFieldDescriptor_getName(info2);
            if (strcmp(name1, name2)==0)
            {
                self->mHasDuplicates = 1;
                goto END_OUTER_LOOP;
            }
        }
    }
    END_OUTER_LOOP:;
}

char* copyString (const char*  str)
{
    /* Windows does not like strdup */
    size_t len = strlen (str) + 1;
    char* result = (char*)calloc (len, sizeof (char));
    strncpy (result, str, len);
    return result;
}

void checkFree (char**  str)
{
    if (*str)
    {
        free (*str);
        *str = NULL;
    }
}

/******************************************************************************
 * Subscription callbacks. Process dictionary message and inform user of
 * errors, timouts, and completion.
 */

static void MAMACALLTYPE
dict_onCreate( mamaSubscription subsc, 
               void *dictionary )
{
    /* no op */
}

static void MAMACALLTYPE
dict_onError( mamaSubscription mamaSubsc,
              mama_status status, 
              void *platformError, 
              const char *subject,
              void *dictionary )
{
    if( status == MAMA_STATUS_TIMEOUT )
    {
        self->mCallbackSet.onTimeout( self, 
                                      self->mClosure );
    }
    else
    {
        self->mCallbackSet.onError( self, 
                                    mamaStatus_stringForStatus( status ), 
                                    self->mClosure );
    }
}


static void MAMACALLTYPE
dict_onMsg (mamaSubscription subsc, mamaMsg msg, void *dictionary, void *notUsed)
{
    processMsg( self, msg );
}

mamaMsgCallbacks gDictSubcCallbacks = { dict_onCreate, dict_onError, dict_onMsg };

const mamaMsgCallbacks*
mamaDictionary_getSubscCb( mamaDictionary dictionary )
{
    return &gDictSubcCallbacks;
}

mama_status ensureCapacity (mamaDictionaryImpl* impl, mama_fid_t fid)
{
    /*The fid is the index into the array. Therefore we need that the
     array has at least index+1 elements*/
    
    /*The maximum fid in the dictionary*/
    if (fid > impl->mMaxFid) impl->mMaxFid   = fid;

    if ((fid+1) > impl->mDictSize)
    {
        int i = 0;
        /*For now lets just grow the array by 100 more than we actually need*/
        int newDictNumElements = fid + 100;

        mamaFieldDescriptor* newFdArray = (mamaFieldDescriptor*)calloc(
                newDictNumElements, sizeof(mamaFieldDescriptor));
        if (!newFdArray) return MAMA_STATUS_NOMEM;

        /*Copy all existing descriptors over to new array*/
        for (i=0;i<impl->mDictSize;i++)
        {
            newFdArray[i] = impl->mDict[i];
        }

        /*Free the existing array*/
        free (impl->mDict);

        /*The number of elements in the array*/
        impl->mDictSize = newDictNumElements;
        impl->mDict     = newFdArray;
    }
    return MAMA_STATUS_OK;
}

mama_status createDictArray (mamaDictionaryImpl* impl)
{
    if (!impl) return MAMA_STATUS_NULL_ARG;
    impl->mDictSize = impl->mMaxFid + 1;
    impl->mDict = (mamaFieldDescriptor*)calloc( impl->mDictSize,
                                                sizeof( mamaFieldDescriptor ) );
    if (!impl->mDict) return MAMA_STATUS_NOMEM;
    return MAMA_STATUS_OK;
}

#define ADD_TO_DICT(TYPE, VALUE)                                            \
do                                                                          \
{                                                                           \
    if (MAMA_STATUS_OK!=(status=mamaMsg_add ## TYPE                         \
                                    (msg, name, fid, VALUE)))               \
    {                                                                       \
       mama_log (MAMA_LOG_LEVEL_NORMAL,"Could not add field "               \
        "descriptor to message %s", mamaStatus_stringForStatus (status));   \
    }                                                                       \
}                                                                           \
while(0)                                                                    \

#define ADD_VECTOR_TO_DICT(TYPE, VALUE)                                     \
do                                                                          \
{                                                                           \
    if (MAMA_STATUS_OK!=(status=mamaMsg_addVector ## TYPE                   \
                                    (msg, name, fid, VALUE, 1)))            \
    {                                                                       \
       mama_log (MAMA_LOG_LEVEL_NORMAL,"Could not add field "               \
        "descriptor to message %s", mamaStatus_stringForStatus (status));   \
    }                                                                       \
}                                                                           \
while(0)                                                                    \


void populateMessageFromDictionary (mamaDictionaryImpl* impl,
                                    mamaMsg             msg)
{
    int         i      = 0;
    mama_status status = MAMA_STATUS_OK;

    const mama_i8_t i8_vector[] = {1};
    const mama_u8_t u8_vector[] = {1};
    const mama_i16_t i16_vector[] = {1};
    const mama_u16_t u16_vector[] = {1};
    const mama_i32_t i32_vector[] = {1};
    const mama_u32_t u32_vector[] = {1};
    const mama_i64_t i64_vector[] = {1};
    const mama_u64_t u64_vector[] = {1};
    const mama_f32_t f32_vector[] = {1.0f};
    const mama_f64_t f64_vector[] = {1.0};
    const char*      string_vector[] = {" "};

    mamaMsg msgArray[1];
    mamaMsg tempMsg   =   NULL;

    mamaPrice price_vector[1];
    mamaPrice price = NULL;

    mamaDateTime dateTime_vector[1];
    mamaDateTime dateTime = NULL;


    mamaMsg_create (&tempMsg);
    mamaMsg_addI32 (tempMsg, NULL, 1, 0);
    msgArray[0] = tempMsg;

    mamaPrice_create (&price);
    mamaPrice_setValue (price,123.45);
    price_vector[0] = price;

    mamaDateTime_create (&dateTime);
    mamaDateTime_setToNow (dateTime);
    dateTime_vector[0] = dateTime;
    
    for (i=0;i<impl->mDictSize;i++)
    {
        if (impl->mDict[i])
        {
            mamaFieldDescriptor fd = impl->mDict[i];
            mama_fid_t      fid     = mamaFieldDescriptor_getFid  (fd);
            const char*     name    = mamaFieldDescriptor_getName (fd);
            mamaFieldType   type    = mamaFieldDescriptor_getType (fd);

            switch (type)
            {
                case MAMA_FIELD_TYPE_MSG:
                    ADD_TO_DICT (Msg, tempMsg);
                    break;
                case MAMA_FIELD_TYPE_OPAQUE:
                    mamaMsg_addOpaque (msg, name, fid, (void*)" ", 2);
                    break;
                case MAMA_FIELD_TYPE_STRING:
                    ADD_TO_DICT (String, "");
                    break;
                case MAMA_FIELD_TYPE_BOOL:
                    ADD_TO_DICT (Bool, 0);
                    break;
                case MAMA_FIELD_TYPE_CHAR:
                    ADD_TO_DICT (Char, ' ');
                    break;
                case MAMA_FIELD_TYPE_I8:
                    ADD_TO_DICT (I8, 0);
                    break;
                case MAMA_FIELD_TYPE_U8:
                    ADD_TO_DICT (U8, 0);
                    break;
                case MAMA_FIELD_TYPE_I16:
                    ADD_TO_DICT (I16, 0);
                    break;
                case MAMA_FIELD_TYPE_U16:
                    ADD_TO_DICT (U16, 0);
                    break;
                case MAMA_FIELD_TYPE_I32:
                    ADD_TO_DICT (I32, 0);
                    break;
                case MAMA_FIELD_TYPE_U32:
                    ADD_TO_DICT (U32, 0);
                    break;
                case MAMA_FIELD_TYPE_I64:
                    ADD_TO_DICT (I64, 0);
                    break;
                case MAMA_FIELD_TYPE_U64:
                    ADD_TO_DICT (U64, 0);
                    break;
                case MAMA_FIELD_TYPE_F32:
                    ADD_TO_DICT (F32, 0.0f);
                    break;
                case MAMA_FIELD_TYPE_F64:
                    ADD_TO_DICT (F64, 0.0);
                    break;
                case MAMA_FIELD_TYPE_TIME:
                    ADD_TO_DICT (DateTime, dateTime);
                    break;
                case MAMA_FIELD_TYPE_PRICE:
                    ADD_TO_DICT (Price, price);
                    break;
                case MAMA_FIELD_TYPE_VECTOR_I8:
                    ADD_VECTOR_TO_DICT (I8, i8_vector);
                    break;
                case MAMA_FIELD_TYPE_VECTOR_U8:
                    ADD_VECTOR_TO_DICT (U8, u8_vector);
                    break;
                case MAMA_FIELD_TYPE_VECTOR_I16:
                    ADD_VECTOR_TO_DICT (I16, i16_vector);
                    break;
                case MAMA_FIELD_TYPE_VECTOR_U16:
                    ADD_VECTOR_TO_DICT (U16, u16_vector);
                    break;
                case MAMA_FIELD_TYPE_VECTOR_I32:
                    ADD_VECTOR_TO_DICT (I32, i32_vector);
                    break;
                case MAMA_FIELD_TYPE_VECTOR_U32:
                    ADD_VECTOR_TO_DICT (U32, u32_vector);
                    break;
                case MAMA_FIELD_TYPE_VECTOR_I64:
                    ADD_VECTOR_TO_DICT (I64, i64_vector);
                    break;
                case MAMA_FIELD_TYPE_VECTOR_U64:
                    ADD_VECTOR_TO_DICT (U64, u64_vector);
                    break;
                case MAMA_FIELD_TYPE_VECTOR_F32:
                    ADD_VECTOR_TO_DICT (F32, f32_vector);
                    break;
                case MAMA_FIELD_TYPE_VECTOR_F64:
                    ADD_VECTOR_TO_DICT (F64, f64_vector);
                    break;
                case MAMA_FIELD_TYPE_VECTOR_STRING:
                    ADD_VECTOR_TO_DICT (String, string_vector);
                    break;
                case MAMA_FIELD_TYPE_VECTOR_MSG:
                    ADD_VECTOR_TO_DICT (Msg, msgArray);
                    break;
                case MAMA_FIELD_TYPE_VECTOR_TIME:
                    ADD_VECTOR_TO_DICT (DateTime, dateTime_vector);
                    break;
                case MAMA_FIELD_TYPE_VECTOR_PRICE:
                    ADD_VECTOR_TO_DICT (Price, price_vector);
                    break;
                default:
                    mama_log (MAMA_LOG_LEVEL_NORMAL, "Could not add type to "
                              "dict message - not supported. [%s]",
                              mamaFieldTypeToString (type));
                    break;
            }
        }
    }

    mamaPrice_destroy (price);
    mamaDateTime_destroy (dateTime);
    mamaMsg_destroy (tempMsg);
}

