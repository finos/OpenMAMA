/* $Id: wtable.c,v 1.12.10.2.12.4 2011/09/07 09:45:08 emmapollock Exp $
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lookup2.h"
#include "wombat/wtable.h"

#define HASH_INITVAL 1

typedef struct bucket
{
    char*           key;
    void*           data;
    struct bucket*  next;
} bucket_t;

typedef struct wtable
{
    char*          name;
    bucket_t**     buckets;  /* pointer to array of (initially NULL) buckets */
    unsigned short size;     /* size of table (in bits) */
} _wtable_t;

static const char* libname = "wtable";

int wtable_debug = 0;

void wtable_for_each( wtable_t table, wTableCallback cb, void* closure )
{
    if (table)
    {
        int i = 0;
        _wtable_t* wtable = (_wtable_t*)table;
        long size = 1 << wtable->size;

        for (i = 0; i < size; i++)
        {
            bucket_t* b = wtable->buckets[i];
            while (b)
            {
                bucket_t* next = b->next;
                cb (table, b->data, b->key, closure);
                b = next;
            }/* end while */
        }/* end for */
    }
}

/*
 * MLS: For debugging.
 */
void dumptable( wtable_t table )
{
    if (table)
    {
        int i = 0;
        _wtable_t*  wtable = (_wtable_t*)table;
        long size = 1 << wtable->size;

        for (i = 0; i < size; i++)
        {
            bucket_t* b = wtable->buckets[i];
            fprintf( stderr, "bucket %d %p:\n", i, (void*)b );
            while (b)
            {
                fprintf( stderr, "\tkey: %s\n", b->key );
                b = b->next;
            }
        }
    }
}

void wtable_free_all( wtable_t table )
{
    int i = 0;
    _wtable_t *   wtable = (_wtable_t *)table;

    long size = 1 << wtable->size;

    for( i = 0; i < size; i++ )
    {
        bucket_t *b = wtable->buckets[i];
        while(b)
        {
            bucket_t *blast;
            if( wtable_debug )
            {
                fprintf( stderr,
                         "wtable: freeing key and data: %s\n", 
                         b->key );
            }

            free( b->key );
            free( b->data );
            blast = b;
            b = b->next;
            free(blast);
        }

        wtable->buckets[i] = NULL;
    }
}

void wtable_free_all_xdata( wtable_t table )
{
    int i = 0;
    _wtable_t *   wtable = (_wtable_t *)table;

    long size = 1 << wtable->size;

    for( i = 0; i < size; i++ )
    {
        bucket_t *b = wtable->buckets[i];
        while(b)
        {
            bucket_t *blast;
            if( wtable_debug )
            {
                fprintf( stderr,
                         "wtable: freeing key and data: %s\n", 
                         b->key );
            }

            free( b->key );
            
            blast = b;
            b = b->next;
            free(blast);
        }

        wtable->buckets[i] = NULL;
    }
}

wtable_t wtable_create (const char * name, unsigned long size)
{
    static char   func[] = "wtable_create";
    _wtable_t *   wtable = (_wtable_t *)calloc (1, sizeof (_wtable_t));
    unsigned long tsize;

    /* return NULL if calloc failed */
    if (!wtable)
    {
        return NULL;
    }

    /* Size is the approximate size of the table from the caller's perspective. */
    /* We need to make this larger for our hashtable. */
    size *= 10;

    /* store the name (or and empty string, if name is NULL) */
    wtable->name = (name) ? strdup(name) : strdup("");

    /* set the table-size to the smallest power of 2 greater-or-equal to 'size' */
    wtable->size = 1;
    tsize = hashsize(1);
    while (tsize < size)
    {
        wtable->size++;
        tsize = hashsize(wtable->size);
    }

    /* return NULL if calloc failed */
    wtable->buckets = (bucket_t **) calloc (tsize, sizeof (bucket_t *));
    if (!wtable->buckets)
    {
        return NULL;
    } 
    
    if (wtable_debug)
    {
        fprintf (stderr, 
                 "%s: %s: created a table (name: \"%s\") with size: %lu (0x%0lx)\n",
                 libname, func, wtable->name, tsize, tsize);
    }

    return (wtable_t) wtable;
}


void wtable_destroy (wtable_t table)
{
    _wtable_t*  wtable = (_wtable_t*)table;
    if (wtable)
    {
        /* Note: we do not clear the table here because there are
         * three ways to do that and we prefer to force the "user" to
         * pick the correct one. */
        if (wtable->buckets) free (wtable->buckets);
        if (wtable->name)    free (wtable->name);
        free (wtable);
    }
}


int wtable_insert (wtable_t table, const char* key, void* data)
{
    static char  func[] = "wtable_insert";
    if (table)
    {
        ub4          h;
        int          len;
        _wtable_t *  wtable = (_wtable_t *)table;
        bucket_t *   b;

        if (!key || !table) 
        {
            if (wtable_debug)
            {
                fprintf (stderr, 
                         "%s: %s: error: non-existent table or key passed\n",
                         libname, func);
            }
            return -1;
        }

        len = strlen(key);
        h = whash((unsigned char *)key, len, HASH_INITVAL);
        h = (h & hashmask(wtable->size));
        if (wtable_debug)
        {
            fprintf (stderr, 
                     "%s: %s: %s: key = \"%s\"; hashval = %lu (0x%lx)\n",
                     libname, func, wtable->name, key, h, h);
        }
        b = wtable->buckets[h];
        while (b)
        {
            if (strcmp(key, b->key) == 0)
            {
                /* same key, just update the data */
                if (wtable_debug)
                {
                    fprintf (stderr,
                             "%s: %s: %s: key already present: \"%s\"\n",
                             libname, func, wtable->name, key);
                }
                b->data = data;
                return 0;
            }
            if (wtable_debug)
            {
                fprintf (stderr, 
                         "%s: %s: %s: collision for key: \"%s\" (collided with \"%s\")\n",
                         libname, func, wtable->name, key, b->key);
            }
            b = b->next;
        }

        /* if we get here, we must need to create a new bucket */
        b = (bucket_t *) calloc (1, sizeof(bucket_t));
        if (!b)
        {
            /* calloc failed, return error */
            return -1;
        }
        b->key = strdup(key);
        b->data = data;
        b->next = wtable->buckets[h];
        wtable->buckets[h] = b;
        return 1;
    }
    else
    {
        return 0;
    }
}


void* wtable_lookup (wtable_t table, const char* key)
{
    static char  func[] = "wtable_lookup";
    if (table)
    {
        ub4          h;
        int          len;
        _wtable_t *  wtable = (_wtable_t *)table;
        bucket_t *   b;

        if (!key || !table) 
        {
            if (wtable_debug)
            {
                fprintf (stderr, 
                         "%s: %s: error: non-existent table or key passed\n",
                         libname, func);
            }
            return NULL;
        }

        len = strlen(key);
        h = whash((unsigned char *)key, len, HASH_INITVAL);
        h = (h & hashmask(wtable->size));
        if (wtable_debug)
        {
            fprintf (stderr, 
                     "%s: %s: %s: key = \"%s\"; hashval = %lu (0x%lx)\n",
                     libname, func, wtable->name, key, h, h);
        }

        b = wtable->buckets[h];
        while (b)
        {
            if (strcmp(key, b->key) == 0)
            {
                /* fount it, return the data */
                if (wtable_debug)
                {
                    fprintf (stderr, "%s: %s: %s: found key: \"%s\"\n",
                             libname, func, wtable->name, key);
                }
                return (b->data);
            }
            if (wtable_debug)
            {
                fprintf (stderr, 
                         "%s: %s: %s: collision for key: \"%s\" (collided with \"%s\")\n",
                         libname, func, wtable->name, key, b->key);
            }
            b = b->next;
        }

        /* not found */
        if (wtable_debug)
        { 
            fprintf (stderr, 
                     "%s: %s: %s: key not found: \"%s\"\n",
                     libname, func, wtable->name, key);
        }
        return NULL;
    }
    else
    {
        return NULL;
    }
}

void* wtable_remove (wtable_t table, const char* key)
{
    static char  func[] = "wtable_remove";
    if (table)
    {
        ub4          h;
        int          len;
        _wtable_t *  wtable = (_wtable_t *)table;
        bucket_t *   b;
        bucket_t *   b0;
        void*        data;

        if (!key || !table) 
        {
            if (wtable_debug)
            {
                fprintf (stderr, 
                         "%s: %s: error: non-existent table or key passed\n",
                         libname, func);
            }
            return NULL;
        }

        len = strlen(key);
        h = whash((unsigned char *)key, len, HASH_INITVAL);
        h = (h & hashmask(wtable->size));
        if (wtable_debug)
        {
            fprintf (stderr, 
                     "%s: %s: %s: key = \"%s\"; hashval = %lu (0x%lx)\n",
                     libname, func, wtable->name, key, h, h);
        }

        b0 = NULL;
        b = wtable->buckets[h];
        while (b)
        {
            if (strcmp(key, b->key) == 0)
            {
                /* fount it, remove the key */
                if (wtable_debug)
                {
                    fprintf (stderr, "%s: %s: %s: found key: \"%s\"\n",
                             libname, func, wtable->name, key);
                }
                if (b0)
                {
                    b0->next = b->next;
                }
                else
                {
                    wtable->buckets[h] = b->next;
                }
                data = b->data;
                free (b->key);
                free (b);
                return data;
            }
            b0 = b;
            b = b->next;
        }

        /* not found */
        if (wtable_debug)
        { 
            fprintf (stderr, 
                     "%s: %s: %s: key not found: \"%s\"\n",
                     libname, func, wtable->name, key);
        }
        return NULL;
    }
    else
    {
        return NULL;
    }
}

void wtable_clear (wtable_t table)
{
    if (table)
    {
        int i = 0;
        _wtable_t* wtable = (_wtable_t*)table;
        long size = 1 << wtable->size;

        for (i = 0; i < size; i++)
        {
            bucket_t* b = wtable->buckets[i];
            while (b)
            {
                bucket_t* blast = b;
                if (wtable_debug)
                {
                    fprintf (stderr, "wtable: clearing key: %s\n", b->key);
                }
                free (b->key);  /* The key is our job. */
                b = b->next;
                free (blast);
            }/* end while */

            wtable->buckets[i] = NULL;
        }/* end for */
    }
}

void wtable_clear_for_each (wtable_t table, wTableCallback cb, void* closure)
{
    if (table)
    {
        int i = 0;
        _wtable_t* wtable = (_wtable_t*)table;
        long size = 1 << wtable->size;

        for (i = 0; i < size; i++)
        {
            bucket_t* b = wtable->buckets[i];
            while (b)
            {
                /* The idea here is that the callback frees the data if it
                 * needs to be freed (or deleted, in C++). */
                bucket_t* blast = b;
                if (wtable_debug)
                {
                    fprintf (stderr, "wtable: calling clear cb for key: %s\n",
                             b->key);
                }
                cb (table, b->data, b->key, closure);
                free (b->key);  /* The key is our job. */
                b = b->next;
                free (blast);
            }/* end while */

            wtable->buckets[i] = NULL;
        }/* end for */
    }
}
