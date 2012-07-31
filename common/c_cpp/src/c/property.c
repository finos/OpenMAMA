/* $Id: property.c,v 1.22.2.2.2.1.2.4 2011/09/07 09:45:08 emmapollock Exp $
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>


#include "property.h"
#include "propertyinternal.h"
#include "lookup2.h"
#include "wombat/wtable.h"

#define KEY_BLOCK_SIZE 64

int gPropertyDebug = 0;

typedef struct
{
    wtable_t     mTable;
    const char** mKeys;
    int          mNumKeys;
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
    int length        = 0;
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

    if( this->mTable == NULL && this->mKeys != NULL )
    {
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

/* This function will replace any environment variables with their
 * equivalents in the string passed in.
 *
 * @param value [I] The string that may contain environment variables.
 *
 * @return NULL if the value string did not containing any environment variables.
 * A valid string is variables were replaced, delete the memory with free.
 */
char* propertiesImpl_ReplaceEnvironmentVariable( const char* value )
{
	/* Returns */
	char* ret = NULL;
    if( value != NULL )
    {
	    /* Scan the property value for a $( which indicates an environment
         * variable */
	    char* envStart = strstr(value, "$(");
	    if( envStart != NULL )
	    {
		    /* Get a pointer to the first character of the variable itself. */
		    char* variableStart = (envStart + 2);

		    /* Locate the end of the string */
		    char* envEnd = strchr( variableStart, ')' );
		    if( envEnd != NULL )
		    {
                /* Obtain the length of the value string, this must be done
                 * before any replacements. */
			    int valueLength = strlen( value );

			    /* Save the character at the end of the string */
			    char endChar = *envEnd;

			    /* Replace it will a NULL to extract the variable name. */
			    *envEnd = '\0';
			    {
				    /* Extract the variable name. */
				    char* envValue = getenv( variableStart );

                    /* If a value was returned then a new property value must
                     * be formatted. */
				    if(envValue != NULL)
				    {
                        /* Determine the number of characters needed to store
                         * the new string. This is the length of the original
                         * value plus the length of the environment variable
                         * value minus the length of the environment variable
                         * name.  The -2 at the end is due to the 3 characters
                         * that wrap the environment variable name plus 1 for
                         * the null terminator.
					     */
					    int newStringLength = (valueLength -
                                strlen(variableStart) + strlen(envValue) - 2);

					    /* Allocate a buffer to contain the new string */
					    ret = (char* )calloc(newStringLength, sizeof(char));
					    if(ret != NULL)
					    {
                            /* Write a temporary NULL into the input string
                             * where the environment variable symbol starts.
						     */
						    char startChar = *envStart;
						    *envStart = '\0';

						    /* Format the buffer */
						    sprintf(ret, "%s%s%s", value, envValue, (envEnd + 1));

						    /* Restore the NULL at the start of the string. */
						    *envStart = startChar;
					    }
				    }
			    }

			    /* Restore the NULL at the end of the string. */
			    *envEnd = endChar;
		    }
	    }
    }

	return ret;
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
    if( this->mTable == NULL && this->mKeys != NULL )
    {
        free( (void* )this->mKeys );
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
		char* localString = propertiesImpl_ReplaceEnvironmentVariable(propString);
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

    wtable_for_each( iFrom->mTable, properties_copy,
		     iTo->mTable );
}

/**
 * Boolean test of property value
 */
int
properties_GetPropertyValueAsBoolean(const char* propertyValue)
{
  if (
      (strcmp(propertyValue,"1")==0) ||
      (strcmp(propertyValue,"y")==0) ||
      (strcmp(propertyValue,"Y")==0) ||
      (strcmp(propertyValue,"yes")==0) ||
      (strcmp(propertyValue,"YES")==0) ||
      (strcmp(propertyValue,"true")==0) ||
      (strcmp(propertyValue,"TRUE")==0) ||
      (strcmp(propertyValue,"t")==0) ||
      (strcmp(propertyValue,"T")==0)
    )
  {
    return 1;
  }
  return 0;
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

   if( gPropertyDebug )fprintf( stderr, "Get property: %s\n", rval );

   return rval;
}

int
properties_setProperty (wproperty_t handle,
        const char* name,
        const char* value)
{
    const char* name_c  =   NULL;
    const char* value_c =   NULL;

    propertiesImpl_ *this = (propertiesImpl_ *)handle;
    if (!this||!name||!value) return 0;

    /*Acceptable leak for the moment*/
    name_c  =   strdup (name);

    /* The value will need to have any environment variables replaced. */
    value_c = (const char* )propertiesImpl_ReplaceEnvironmentVariable(value);
    if(NULL == value_c)
    {
        value_c = strdup(value);
    }

    return propertiesImpl_AddProperty ((propertiesImpl)this,
                                        name_c, value_c);
}

struct propCallbackContainer
{
    propertiesCallback cb;
    void* closure;
};

static void
propertiesImpl_ForEach( wtable_t props, void* data, const char* key,
			void* cookie )
{
    struct propCallbackContainer * container =
	(struct propCallbackContainer *) cookie;

    (*container->cb)(key, data, container->closure);
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

   wtable_free_all (this->mTable);

   wtable_destroy( this->mTable );

   free( (void* )this->mKeys );
   free( this );
}

/**
 * Free the resources associated with the properties.
 * This is a work around for fixing wmw-830
 * This ensures the memory leak does not happen by freeing
 * the mKeys strdup-ed in properties_setProperty
 */
void
properties_FreeEx( wproperty_t handle )
{
   propertiesImpl_ *this = (propertiesImpl_ *)handle;
   int             i     = 0;

   if( gPropertyDebug )fprintf( stderr, "properties_FreeEx\n" );

   propertiesImpl_destroy((propertiesImpl)this);

   wtable_free_all (this->mTable);

   for( i = 0; i < this->mNumKeys; i++ )
   {
       if( this->mKeys != NULL && this->mKeys[i] != NULL )
       {
           free( (void*)this->mKeys[i] );
       }
   }

   wtable_destroy( this->mTable );

   free( (void* )this->mKeys );
   free( this );
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

       /* Free the keys */
       if(this->mKeys != NULL)
       {
            free( (void* )this->mKeys );
       }

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
   int strln = strlen (src);

   if (!src)
      return NULL;

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

static int
propertiesImpl_AddKey( propertiesImpl_ *this, const char* name )
{
    int blocks = (this->mNumKeys / KEY_BLOCK_SIZE) + 1;

    if( 0 == (this->mNumKeys % KEY_BLOCK_SIZE) )
    {
        int allocSize = blocks * KEY_BLOCK_SIZE;

        if( blocks == 1 )
        {
            this->mKeys = (const char**)calloc( allocSize,
                                                sizeof( const char* ) );
        }
        else
        {
            void* reallocBlock = 
                realloc((void* )this->mKeys, (allocSize * sizeof(const char*)));
            
            if(NULL != reallocBlock)
            {
                this->mKeys = (const char**)reallocBlock;
            }
            else
            {
                free((void* )this->mKeys);
                this->mKeys = NULL;
                return 0;
            }
        }
    }

    this->mKeys[ this->mNumKeys++ ] = name;

    return 1;
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

    if ( NULL == (data = wtable_lookup(this->mTable, (char*)name)))
    {
        if( !propertiesImpl_AddKey( this, name ))
            return 0;
    }

    if(-1 == (ret = wtable_insert( this->mTable, (char* )name, (caddr_t)value)))
    {
        return 0;
    }

    if(0 == ret) /* If 0 is returned then data has been replaced. */
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
