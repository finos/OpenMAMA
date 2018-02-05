//
// strcasecmp.cc
//
// strcasecmp: replacement of the strcasecmp functions for architectures that do
//             not have it.
//
// Part of the ht://Dig package   <http://www.htdig.org/>
// Copyright (c) 1999-2004 The ht://Dig Group
// For copyright details, see the file COPYING in your distribution
// or the GNU Library General Public License (LGPL) version 2 or later 
// <http://www.gnu.org/copyleft/lgpl.html>
//
// $Id$
//

//#ifdef HAVE_CONFIG_H
//#include "htconfig.h"
//#endif /* HAVE_CONFIG_H */

//#include "lib.h"
#include <ctype.h>

//*****************************************************************************
//
int strcasecmp(const char *str1, const char *str2)
{
    if (!str1 && !str2)
        return 0;
    if (!str1)
        return 1;
    if (!str2)
        return -1;
    while (*str1 &&
           *str2 &&
           tolower((unsigned char)*str1) == tolower((unsigned char)*str2))
    {
        str1++;
        str2++;
    }

    return tolower((unsigned char)*str1) - tolower((unsigned char)*str2);
}


//#define tolower(ch)	(isupper(ch) ? (ch) + 'a' - 'A' : (ch))
//*****************************************************************************
//
int strncasecmp(const char *str1, const char *str2, int n)
{
    if (!str1 && !str2)
        return 0;
    if (!str1)
        return 1;
    if (!str2)
        return -1;
    if (n < 0)
        return 0;
    while (n &&
           *str1 &&
           *str2 &&
           tolower((unsigned char)*str1) == tolower((unsigned char)*str2))
    {
        str1++;
        str2++;
        n--;
    }

    return n == 0 ? 0 :
        tolower((unsigned char)*str1) - tolower((unsigned char)*str2);
}


//*****************************************************************************
// char *mystrcasestr(const char *s, const char *pattern)
//
const char *
mystrcasestr(const char *s, const char *pattern)
{
    int length = strlen(pattern);

    while (*s)
    {
        if (strncasecmp(s, pattern, length) == 0)
            return s;
        s++;
    }
    return 0;
}