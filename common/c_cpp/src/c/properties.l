/* $Id: properties.l,v 1.7.2.1.12.3 2011/08/10 14:53:24 nicholasmarriott Exp $
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

/** This file should be processed by FLEX or LEX **/

%{  
#include "wombat/port.h"
#include "propertyinternal.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wombat/wincompat.h"

extern int gPropertyDebug;
wthread_static_mutex_t lexerLock = WSTATIC_MUTEX_INITIALIZER;

%}

NEWLINE             \n|\r\n
SEPARATOR           [:=]
SLASH               [\\]
WS                  [ \t]

        propertiesImpl properties = NULL;
        char*    curKey    = NULL;
        char*    curValue  = NULL;
        char*     replaceValue    = NULL;

        /* Function for building strings of unknown length 
         * this approach is inefficient but it will do for 
         * now. It should not be called very often since it is only
         * necessary to realloc when there are escapes in keys or values or 
         * when a value spans multiple lines.
         */
        char* concatString( const char* dest, const char* string )
        {
            char* result = dest != NULL 
                ? (char* )realloc( (char* )dest, strlen( (char*)dest ) + 
                    strlen( (char* )string ) + 1 )
                : (char* )calloc( strlen( string ) + 1, sizeof(char) );
            strcat( result, string );
            return result;
        }

        /** Append a translated escape to the end of the current string. 
         *  This is necessary to achieve results identical to java.*/
        char* concatEscape( char* toWhat, char escape )
        {
            char val[2];
            val[1] = '\0';

            switch( escape )
            {
            case 'n':
                val[0] = '\n';
                break;
            case 'r':
                val[0] = '\r';
                break;
            case 't':
                val[0] = '\t';
                break;
            case '\\':
                val[0] = '\\';
                break;
            default: 
                val[0] = escape;
                break;
           }

           return toWhat = concatString( toWhat, val );
        }

%x key
%x value
%%

^[#!].*{NEWLINE}                 { /* ignore comments */ }
^{WS}*{NEWLINE}                  { /* ignore blank lines */ }
<*>^{WS}*                        { /* always ignore WS at beginning of line */ }

<INITIAL,key>([^ \t:=\r\n\\])*   { /*Any non-WS in INITIAL state starts a key */
                                   curKey = concatString( curKey, yytext );
                                   BEGIN(key); }  

<key>{WS}*[:=]?{WS}*             { /* ignore assignment  and surrounding WS.
                                      Indicates start of value. */
                                   BEGIN(value); }

<value>[^\\\r\n \t]*             { /* Collect all non-escape/non-newline/
                                    * non-whitespace for value */
                                   curValue = concatString( curValue, yytext ); }

<value>{SLASH}{NEWLINE}[\t ]*    { /* continue with value: line ends in '\' */
                                   /* ignore space after newline before text on
                                    * next line.*/ }

<value>{NEWLINE}                 { /* value is complete. start over.*/
                                   replaceValue = 
                                       propertiesImpl_ReplaceEnvironmentVariable(curValue);
                                   if(replaceValue != NULL)
                                   {
                                        /* Delete the original value */
                                        free(curValue);
                                        
                                        /* Copy the replace value into the 
                                         * current value pointer. */
                                        curValue = replaceValue;
                                   }                                   
                                   propertiesImpl_AddProperty( properties,
                                       curKey, curValue );
                                   /* Freed when properties destroyed */
                                   if( gPropertyDebug )
                                   {
                                        fprintf( stderr, "key: %s value: %s\n", 
                                        curKey, curValue );
                                   }
                                   if (curKey) free (curKey);
                                   curKey = curValue = NULL;
                                   BEGIN(INITIAL); }

                                /* Handle escape characters */
<INITIAL,key>\\.                { curKey = concatEscape( curKey, yytext[1] );
                                  BEGIN(key); }
<value>\\.                      { curValue = concatEscape( curValue, yytext[1] ); }

<<EOF>>                         { /* Handle EOF right at the end of a */
                                    /* value string */
                                    if ( curKey && curValue )
                                    {
                                        propertiesImpl_AddProperty( properties,
                                            curKey, curValue );
                                        if( gPropertyDebug )
                                        {
                                        fprintf( stderr, "key: %s value: %s\n", 
                                        curKey, curValue );
                                        }
                                                        free (curKey);
                                        curKey = curValue = NULL;
                                        BEGIN(INITIAL);
                                    }
                                    yyterminate(); }
%%

int
propertiesImpl_ImportPropertiesFromFile( propertiesImpl this,
                                         const char* path, 
                                         const char* fileName )
{
    FILE* f;
    char* fullPath;
    int nameLength;

    if( path == NULL )
    path = "";
    if( fileName == NULL )
    fileName = "";

    /* +1 for path separator. +1 for trailing \0 */
    nameLength = strlen( path ) + strlen( fileName ) + 2;

    fullPath = (char* )calloc( nameLength, sizeof( char ) );

    if( fullPath == NULL )
    {
        return 0;
    }

    snprintf( fullPath, nameLength, "%s/%s", path, fileName );

    if( gPropertyDebug ) fprintf( stderr, "Opening property file %s\n", fullPath );

    f = fopen( fullPath, "r" );

    if( f == NULL )
    {
        free( fullPath );
        return 0;
    }

    wthread_static_mutex_lock (&lexerLock);
    yyrestart(f);
    properties = this;
    yylex();
    properties = NULL;
    wthread_static_mutex_unlock (&lexerLock);

    free( fullPath );
    fclose( f );
    return 1;
}

int
propertiesImpl_AddString( propertiesImpl this,
              const char* string )
{
    YY_BUFFER_STATE buf;

    if( gPropertyDebug ) fprintf( stderr, "Scanning property string %s\n", string );

    wthread_static_mutex_lock (&lexerLock);

    properties = this;
    /* yy_scan_string changes the global lex stack buffer there lock until this
     * removed */
    buf = yy_scan_string(string);
    yylex();
    yy_delete_buffer(buf);
    properties = NULL;
    wthread_static_mutex_unlock (&lexerLock);

    return 1;
}

int
propertiesImpl_destroy( propertiesImpl this)
{
    /* Just return */
    return 1;
}


void foo()
{
    /* Get rid of compiler warning */
    yyunput( 0, NULL);
}
