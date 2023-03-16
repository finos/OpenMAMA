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

#include "wombat/port.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


#include "wombat/property.h"
#include "propertyinternal.h"
#include "wombat/lookup2.h"
#include "wombat/memnode.h"
#include "wombat/wtable.h"
#include "wombat/strutils.h"

#define KEY_BLOCK_SIZE 64

int gPropertyDebug = 0;

typedef struct
{
    wtable_t     mTable;
} propertiesImpl_;

extern int
propertiesImpl_ImportPropertiesFromFile( propertiesImpl this,
                                         const char* path,
                                         const char* fileName );

extern int
propertiesImpl_AddString( propertiesImpl this,
              const char* string );
extern int
propertiesImpl_destroy( propertiesImpl this);

/**
 * Load the properties file.
 */
wproperty_t
properties_Load( const char* path, const char* fileName )
{
    int i             = 0, j=0;
    int pointer       = 0;
    size_t length     = 0;
    const char* tempPath    = NULL;
    char subPath[256] = "";
    char newPath[256] = "";

    propertiesImpl_ *this =
        (propertiesImpl_*)calloc( 1, sizeof( propertiesImpl_ ) );

    if( this == NULL )
    {
        return NULL;
    }

    this->mTable = wtable_create( "", 10 );

    if(this->mTable == NULL)
    {
        free (this);
        return NULL;
    }

    if (path != NULL && strlen (path) != 0)
    {
        length = strlen(path);

		for (i = 0; i < strlen( path ); i++)
		{
			if (path[i] != '"')
				newPath[j++] = path[i];
		}

        tempPath = newPath;
    	for(i = 0; i < length + 1; i++)
    	{
            /* Traverse the path and look for ':' or the '\0' character */
       	    if (path[i] == PATH_DELIM || path[i] == '\0')
            {
                /*Copying the particular sub path*/
                strncpy (subPath, tempPath, i-pointer);
                subPath[i-pointer] = '\0';
                if( propertiesImpl_ImportPropertiesFromFile( (propertiesImpl)this,
                                                              subPath,
                                                              fileName ) )
                {
                    return this;
                }
                i++;
                tempPath = &path[i];
                pointer = i ;
            }
        }
    }
    else
    {
        if( propertiesImpl_ImportPropertiesFromFile( (propertiesImpl)this,
                                                      subPath,
                                                      fileName ) )
        {
            return this;
        }
    }

    wtable_destroy( this->mTable );
    free( this );
    return NULL;
}

/**
 * Create an empty properties object.
 */
wproperty_t
properties_Create( void )
{
    propertiesImpl_ *this =
	(propertiesImpl_*)calloc( 1, sizeof( propertiesImpl_ ) );

    if( this == NULL )
    {
        return NULL;
    }
    this->mTable = wtable_create( "", 10 );
    if(this->mTable == NULL)
    {
        free(this);
        return NULL;
    }

    return this;
}

/**
 * Add (parse) an individual property string.
 */
int
properties_AddString( wproperty_t handle, const char* propString )
{
	/* Returns. */
	int ret = 0;

	/* Only continue if the arguments are valid. */
	if((NULL != handle) && (NULL != propString))
	{
		/* Get the properties impl. */
		propertiesImpl_ *impl = (propertiesImpl_ *)handle;

		/* Replace any environment variables in the properties string. */
		char* localString = strReplaceEnvironmentVariable(propString);
		if(NULL == localString)
		{
			/* No variables were replaced, just make a straight copy. */
			localString = strdup(propString);
		}

		if(NULL != localString)
		{
			/* Add the property. */
			propertiesImpl_AddString((propertiesImpl)impl, localString);

			/* Release the memory. */
			free(localString);
		}
	}

    return ret;
}

static void
properties_copy( wtable_t from, void* data, const char* key, void* to )
{
    /* First of all, check the destination doesn't already have this key, and
     * if it does, free it, because if you don't, new entry will cause leak. */
    void* staleData = wtable_lookup ((wtable_t) to, key);
    if (staleData)
    {
        free (staleData);
    }
    wtable_insert( (wtable_t) to, key, data );
}

/**
 * Merge two properties objects.
 */
void
properties_Merge( wproperty_t to, wproperty_t from )
{
    propertiesImpl_ *iTo = (propertiesImpl_ *) to;
    propertiesImpl_ *iFrom = (propertiesImpl_ *) from;

    if( to == NULL || from == NULL )
    {
        return;
    }

    wtable_for_each( iFrom->mTable, properties_copy, iTo->mTable );
}

/**
 * Boolean test of property value
 */
int
properties_GetPropertyValueAsBoolean(const char* propertyValue)
{
    return strtobool(propertyValue);
}

/**
 * Retrieve the property for the specified name. Returns NULL if the property
 * does not exist.
 */
const char*
properties_Get( wproperty_t handle, const char* name )
{
    propertiesImpl_ *this = (propertiesImpl_ *)handle;
    const char*      rval = NULL;

    if( name == NULL || NULL == this || strlen( name ) == 0 )
    {
        return NULL;
    }

    rval = (char* )wtable_lookup( this->mTable, (char* )name );

    if( gPropertyDebug )fprintf( stderr, "Get property: %s=%s\n", name, rval );

    return rval;
}

const char*
properties_GetPropertyValueUsingFormatString (wproperty_t handle,
                                              const char* defaultVal,
                                              const char* format,
                                              ...)
{
    char        paramName[PROPERTY_NAME_MAX_LENGTH];
    const char* returnVal = NULL;

    /* Create list for storing the properties passed in */
    va_list     arguments;

    /* Populate list with arguments passed in */
    va_start (arguments, format);

    returnVal = properties_GetPropertyValueUsingVaList (
        handle, defaultVal, paramName, format, arguments);

    /* Clean up the list */
    va_end(arguments);

    return returnVal;
}

const char* properties_GetPropertyValueUsingVaList (
    wproperty_t handle,
    const char* defaultVal,
    char* paramName,
    const char* format,
    va_list arguments)
{
    const char* property = NULL;

    /* Create the complete transport property string */
    vsnprintf (paramName, PROPERTY_NAME_MAX_LENGTH,
               format, arguments);

    /* Get the property out for analysis */
    property = properties_Get (handle, paramName);

    /* Properties will return NULL if parameter is not specified in configs */
    if (property == NULL)
    {
        property = defaultVal;
    }

    return property;
}

int
properties_setProperty (wproperty_t handle,
        const char* name,
        const char* value)
{
    const char* name_c  =   NULL;
    const char* value_c =   NULL;
    int ret = 0;

    propertiesImpl_ *this = (propertiesImpl_ *)handle;
    if (!this||!name||!value) return 0;

    /*Acceptable leak for the moment*/
    name_c  =   strdup (name);

    /* The value will need to have any environment variables replaced. */
    value_c = (const char* )strReplaceEnvironmentVariable(value);
    if(NULL == value_c)
    {
        value_c = strdup(value);
    }

    ret = propertiesImpl_AddProperty ((propertiesImpl)this, name_c, value_c);
    free ((void*)name_c);
    return ret;
}

struct propCallbackContainer
{
    propertiesCallback cb;
    void* closure;
    const char* prefix;
    size_t prefixLen;
};

static void
propertiesImpl_ForEach( wtable_t props, void* data, const char* key,
			void* cookie )
{
    struct propCallbackContainer * container =
	(struct propCallbackContainer *) cookie;

    if (container->prefix == NULL) {
        (*container->cb)(key, data, container->closure);
    } else if (0 == strncmp(key, container->prefix, container->prefixLen)
        && strlen(key) > container->prefixLen) {
        // Only pass bare key name when using this callback
        (*container->cb)(key + container->prefixLen, data, container->closure);
    }
}

/**
 * Iterate over all keys in the properties table.
 */
void
properties_ForEach( wproperty_t handle, propertiesCallback cb, void* closure)
{
    struct propCallbackContainer container;

    propertiesImpl_ *this = (propertiesImpl_ *)handle;
    container.cb = cb;
    container.closure = closure;
    container.prefix = NULL;
    container.prefixLen = 0;

    wtable_for_each( this->mTable, propertiesImpl_ForEach, &container );
}

/**
 * Iterate over all keys in the properties table matching provided prefix
 */
void
properties_ForEachWithPrefix( wproperty_t handle, const char* prefix, propertiesCallback cb, void* closure)
{
    struct propCallbackContainer container;

    propertiesImpl_ *this = (propertiesImpl_ *)handle;
    container.cb = cb;
    container.closure = closure;
    container.prefix = prefix;
    container.prefixLen = strlen (prefix);

    wtable_for_each( this->mTable, propertiesImpl_ForEach, &container );
}

/**
 * Free the resources associated with the properties.
 */
void
properties_Free( wproperty_t handle )
{
   propertiesImpl_ *this = (propertiesImpl_ *)handle;

   if( gPropertyDebug )fprintf( stderr, "properties_Free\n" );

   propertiesImpl_destroy((propertiesImpl)this);

   /* This should destroy *all* keys *and* values in wtable */
   wtable_free_all (this->mTable);

   wtable_destroy( this->mTable );

   free( this );
}

/**
 * Now points to properties_Free - was originally a workaround to free mKeys
 * strdup-ed in properties_setProperty, but really, those strings simply
 * should be freed or not strdup'd *inside* that function at all because the
 * wtable will strdup the key anyway and destroying the wtable will in turn
 * destroy that key
 */
void
properties_FreeEx( wproperty_t handle )
{
    properties_Free (handle);
}


/* This function will free all the properties contained in the supplied impl and
 * the impl itself. Note that the memory associated with the property data will
 * not be freed. This function should be used whenever one set of properties
 * have been merged with another. The merging merely copies over the data
 * pointer rather than making a new copy of the memory.
 *
 * @pram handle The properties object to free.
 */
void properties_FreeEx2( wproperty_t handle )
{
    /* Get the impl */
    propertiesImpl_ *this = (propertiesImpl_ *)handle;
    if(this != NULL)
    {
       /* Write a log if appropriate */
       if( gPropertyDebug )
       {
           fprintf( stderr, "properties_FreeEx2\n" );
       }

       /* Destroy the impl */
       propertiesImpl_destroy((propertiesImpl)this);

       /* Free all memory in the table with exception of the data pointers */
       wtable_free_all_xdata (this->mTable);

       /* Free the table itself */
       wtable_destroy( this->mTable );

       /* Free the impl */
       free( this );
   }
}

/**
 * Provides a duplicate string to src except any char matched in array
 * chars will be escaped by a \
 */
char*
properties_AddEscapes (const char* src, const char chars[], int num)
{
   int i = 0, j = 0, retIdx = 0, matches = 0;
   char* retStr;
   size_t strln = 0;

   if (!src)
      return NULL;

   strln = strlen (src);

   /* First need to number chars to escape to malloc the correct string len */
   for (;i < strln; i++)
   {
       for (j = 0; j < num; j++)
       {
           if (src[i] == chars[j])
           {
               matches++;
               break;
           }
       }
    }

    /* If nothing to escape then just dup the string */
    if (!matches)
        return strdup (src);

    retStr = (char* )malloc (strln+matches+1);
    if (!retStr)
        return NULL;

    for (i = 0; i < strln; i++, retIdx++)
    {
        for (j = 0; j < num; j++)
        {
            if (src[i] == chars[j])
            {
                retStr[retIdx++] = '\\';
                break;
            }
        }
        retStr[retIdx] = src[i];
    }
    retStr[retIdx] = '\0';
    return retStr;
}

int
propertiesImpl_AddProperty( propertiesImpl properties,
                            const char* name,
                            const char* value )
{
    propertiesImpl_ *this = (propertiesImpl_*)properties;
    void* data = NULL;
    int ret = 0;

    if( NULL == this )
        return 0;

    data = wtable_lookup(this->mTable, (char*)name);

    if (-1 == (ret = wtable_insert( this->mTable, (char* )name, (caddr_t)value)))
    {
        return 0;
    }

    if (0 == ret) /* If 0 is returned then data has been replaced. */
    {
        /* If existing data in the table has now been replaced then the old data must be freed. */
        if(NULL != data)
        {
            free(data);
        }

        if(gPropertyDebug)
        {
            fprintf(stderr, "\nAddProperty KEY: %s, VALUE: %s\n", name, value);
        }
    }

    return 1;
}

uint32_t properties_Count (wproperty_t handle)
{
    /* Get the impl */
    propertiesImpl_ *this = (propertiesImpl_ *)handle;
    return wtable_get_count (this->mTable);
}
