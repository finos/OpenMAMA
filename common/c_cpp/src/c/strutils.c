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

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include "wombat/port.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <wombat/strutils.h>

static const char* gIntegerStrings[] = {
     "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9",
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
    "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
    "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
    "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
    "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
    "60", "61", "62", "63", "64", "65", "66", "67", "68", "69",
    "70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
    "80", "81", "82", "83", "84", "85", "86", "87", "88", "89",
    "90", "91", "92", "93", "94", "95", "96", "97", "98", "99",
    "100"
};

static const char* gLeadingZeroIntegerStrings[] = {
    "00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
    "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
    "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
    "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
    "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
    "60", "61", "62", "63", "64", "65", "66", "67", "68", "69",
    "70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
    "80", "81", "82", "83", "84", "85", "86", "87", "88", "89",
    "90", "91", "92", "93", "94", "95", "96", "97", "98", "99",
    "100"
};

void wmMemCpy (void* dest, const void* src, const size_t n);

void wmMemCpy (void* dest, const void* src, const size_t n)
{
    if (n < 10)
    {
        switch (n)
        {
        case 9 : ((char*)dest)[8] = ((const char*)src)[8];
        case 8 : ((char*)dest)[7] = ((const char*)src)[7];
        case 7 : ((char*)dest)[6] = ((const char*)src)[6];
        case 6 : ((char*)dest)[5] = ((const char*)src)[5];
        case 5 : ((char*)dest)[4] = ((const char*)src)[4];
        case 4 : ((char*)dest)[3] = ((const char*)src)[3];
        case 3 : ((char*)dest)[2] = ((const char*)src)[2];
        case 2 : ((char*)dest)[1] = ((const char*)src)[1];
        case 1 : ((char*)dest)[0] = ((const char*)src)[0];
        case 0 : break;
        }
    }
    else
    {
        memcpy(dest, src, n);
    }
}

int wmFastPrintU32 (
    char*    result,
    size_t   maxLen,
    w_u32_t  value,
    size_t   minWidth)
{
    return wmFastPrintU64 (result, maxLen, value, minWidth);
}

int wmFastPrintI32 (
    char*    result,
    size_t   maxLen,
    w_i32_t  value,
    size_t   minWidth)
{
    return wmFastPrintI64 (result, maxLen, value, minWidth);
}

int wmFastPrintU64 (
    char*    result,
    size_t   maxLen,
    w_u64_t  value,
    size_t   minWidth)
{
    int extra = 0;
    if (maxLen == 0 || (minWidth >= maxLen))
    {
        return -1;
    }
    if (maxLen == 1)
    {
        result[0] = '\0';
        return 0;
    }
    if (value == 0)
    {
        while (minWidth > 1)
        {
            result[0] = '0';
            ++result;
            ++extra;
            --minWidth;
        }
        result[0]='0';
        result[1]='\0';
        return extra+ 1;
    }
    if (value < 10 && maxLen > 1)
    {
        while (minWidth > 1)
        {
            result[0] = '0';
            ++result;
            ++extra;
            --minWidth;
        }
        wmMemCpy (result, gIntegerStrings[value], 2);
        return extra + 1;
    }
    if ((value < 100) && (maxLen > 2))
    {
        while (minWidth > 2)
        {
            result[0] = '0';
            ++result;
            ++extra;
            --minWidth;
        }
        wmMemCpy (result, gIntegerStrings[value], 3);
        return extra + 2;
    }
    if ((value < 1000) && (maxLen > 3))
    {
        while (minWidth > 3)
        {
            result[0] = '0';
            ++result;
            ++extra;
            --minWidth;
        }
        result[0] = (char)((value/100) + '0');
        wmMemCpy (result+1, gLeadingZeroIntegerStrings[value%100], 3);
        return extra + 3;
    }
    if ((value < 10000) && (maxLen > 4))
    {
        while (minWidth > 4)
        {   
            result[0] = '0';
            ++result;
            ++extra;
            --minWidth;
        }
        wmMemCpy (result,   gLeadingZeroIntegerStrings[value/100], 2);
        wmMemCpy (result+2, gLeadingZeroIntegerStrings[value%100], 3);
        return extra + 4;
    }
    if ((value < 100000) && (maxLen > 5))
    {
        w_u64_t leading = value/10000;
        w_u64_t residual = value - (leading*10000);
        while (minWidth > 5)
        {   
            result[0] = '0';
            ++result;
            ++extra;
            --minWidth;
        }
        wmMemCpy (result,   gIntegerStrings[leading], 2);
        wmMemCpy (result+1, gLeadingZeroIntegerStrings[residual/100], 2);
        wmMemCpy (result+3, gLeadingZeroIntegerStrings[residual%100], 3);
        return extra + 5;
    }

    if ((value < 1000000) && (maxLen > 6))
    {
        w_u64_t leading = value/10000;
        w_u64_t residual = value - (leading*10000);
        while (minWidth > 6)
        {   
            result[0] = '0';
            ++result;
            ++extra;
            --minWidth;
        }
        wmMemCpy (result,   gLeadingZeroIntegerStrings[leading], 2);
        wmMemCpy (result+2, gLeadingZeroIntegerStrings[residual/100], 2);
        wmMemCpy (result+4, gLeadingZeroIntegerStrings[residual%100], 3);
        return extra + 6;
    } 
    {
        size_t size = 0;
        if(value < 10000000)
            size = 7;
        else if(value < 100000000)
            size = 8;
        if (maxLen > size)
        {
            while (minWidth > size)
            {
                result[0] = '0';
                ++result;
                ++extra;
                --minWidth;
            }
            return extra + snprintf (result, maxLen, "%" PRIu64, (uint64_t)value);
        }
    }
    /*  last resort */
    return snprintf (result, maxLen, "%"PRIu64, (uint64_t)value);
}

int wmFastPrintI64 (
    char*    result,
    size_t   maxLen,
    w_i64_t  value,
    size_t   minWidth)
{
    if (value < 0)
    {
        if (maxLen > 1)
        {
            *result = '-';
            result++;
            return wmFastPrintU64 (result, maxLen, -value, minWidth) + 1;
        } 
        else
        {
            return -1;
        }
    }
    else 
    {
        return wmFastPrintU64 (result, maxLen, value, minWidth);
    }
}

int wmFastPrintF32 (
    char*    result,
    size_t   maxLen,
    w_f32_t  value,
    size_t   places)
{
    return wmFastPrintF64 (result, maxLen, value, places);
}

int wmFastPrintF64 (
    char*    result,
    size_t   maxlen,
    w_f64_t  value,
    size_t   places)
{
    size_t actualMaxLen = maxlen;
    char*  tmpResult = result;
    double posValue = value;
    w_i64_t integral = 0;
    tmpResult[0] = '\0';
    if (value < 0.0)
    {
        posValue = -value;
        tmpResult[0] = '-';
        tmpResult++;
        maxlen--;
    }
    integral = (w_i64_t)(posValue);
    if (maxlen > 1)
    {
        int bytes = wmFastPrintI64 (tmpResult, maxlen, integral, 0);
        if (bytes < 0)
        {
            return bytes;
        }
        tmpResult += bytes;
        maxlen -= bytes;
    }
    if (maxlen > 2)
    {
        w_i64_t decimalValue = 0;
        double multiplier = 100.0;
        switch (places)
        {
        case 0:                    break;
        case 1: multiplier = 10.0; break;
        case 2: multiplier = 100.0; break;
        case 3: multiplier = 1000.0; break;
        case 4: multiplier = 10000.0; break;
        case 5: multiplier = 100000.0; break;
        case 6: multiplier = 1000000.0; break;
        case 7: multiplier = 10000000.0; break;
        case 8: multiplier = 100000000.0; break;
        default:
        {
            unsigned int i = 8;
            multiplier = 100000000.0;
            for (; i < places; i++)
                multiplier *= 10;

            break;
        }
        }
        posValue -= integral;
        posValue *= multiplier;
        decimalValue = (w_i64_t)(posValue);
        if (posValue - (double)decimalValue > 0.9)
        {
            /* Rounding error */
            ++decimalValue;
        }
        if (decimalValue > 0)
        {
            int bytes = 0;
            /* maxlen > 2 */
            tmpResult[0] = '.';
            tmpResult++;
            maxlen--;
            bytes = wmFastPrintU64 (tmpResult, maxlen, decimalValue, places);
            if (bytes < 0)
            {
                return bytes;
            }
            while (tmpResult[bytes-1] == '0')
            {
                tmpResult[bytes-1]='\0';
                bytes--; 
            }
            tmpResult += bytes;
            maxlen -= bytes;
            
        }
    }
    tmpResult[maxlen-1] = '\0';
    result  = tmpResult;
    return (int)(actualMaxLen - maxlen);
}

size_t wmStrSizeCpy (char* dst, const char* src, size_t siz)
{
    size_t copyLen = strlen(src);

    if (copyLen < siz)
    {
        /* Src string is small enough, so copy it plus the NUL. */
        strcpy(dst,src);
        return copyLen;
    }
    /* Need to truncate. */
    copyLen = siz - 1;
    wmMemCpy (dst, src, copyLen);
    dst[copyLen] = '\0';
    return copyLen;
}

int wmFastCopyAndShiftStr  (char**       result0,
                            size_t*      maxLen0,
                            const char*  value)
{
    char* result = *result0;
    size_t maxLen = *maxLen0;
    size_t bytes;
    for (bytes = 0; (bytes < maxLen) && *value; ++bytes)
    {
        *result++ = *value++;
    }
    *result = '\0';
    *maxLen0 -= bytes;
    *result0 = result;
    return (int) bytes;
}

int wmFastCopyAndShiftChar (char**       result0,
                            size_t*      maxLen0,
                            char         value)
{
    char* result = *result0;
    size_t maxLen = *maxLen0;

    if ((value == '\0') || (maxLen == 0))
        return 0;
    result[0] = value;
    result[1] = '\0';
    (*maxLen0)--;
    (*result0)++;
    return 1;
}

int wmFastPrintAndShiftF64  (char**       result,
                             size_t*      maxLen,
                             w_f64_t      value,
                             size_t       places)
{
    size_t bytes  = wmFastPrintF64 (*result, *maxLen, value, places);
    *result += bytes;
    *maxLen  -= bytes;
    return (int) bytes;
}

/*
 * change all characters in a string to lowercase
 */
char* toLowerCase(char* s)
{
  int n;
  for (n=0;s[n];n++) 
    s[n]=tolower(s[n]);
  return s;
}

/*
 * Allocate a sufficiently large string and print into it
 */
char* printfAlloc(const char *fmt, ...)
{
  int bufferSize = 100;
  char *p;
  va_list ap;
  int OK;
  void* vp;
  int n;

  if (NULL==fmt) /* Silly */
    return NULL;            

  p = malloc(bufferSize);

  if (NULL==p) /* malloc failed */
    return NULL;

  while (1) 
  {
    va_start(ap, fmt);
    n = vsnprintf(p, bufferSize, fmt, ap);
    OK= (n > -1 && n < bufferSize);
    va_end(ap);

    if (OK)
      return p;

    bufferSize *= 2;  /* double the buffer size */
    vp = realloc(p, bufferSize);

    if (NULL!=vp)
      p=vp;
    else    
    { 
      /* realloc failed */
      free(p);
      return NULL;
    }
  }
}

/*
 * Concatenate two strings, allocating a sufficient amount of memory
 * Do not pass a string "s" that has had it's memory allocated by a
 * function other than this one, otherwise the block count calculation
 * may be incorrect
 */
int
strcatAlloc(char** s, const char* ct)
{
  size_t blockSize = 100;
  size_t currentBlocks, requiredBlocks;
  void* p;

  if (NULL==ct) return 0; /* nothing to do! */
  
  if (NULL==*s) /* first time */
  {
    *s = calloc(1, blockSize);
    if (NULL==*s) /* calloc failed */
      return 1;
    *s[0]='\0'; /* calloc() should have taken care of this, but to be safe... */
  }

  currentBlocks = ((strlen(*s) + 1) / blockSize) + 1;
  requiredBlocks = ((strlen(*s) + strlen(ct) + 1) / blockSize) + 1;

  if (requiredBlocks > currentBlocks)
  {
    p = realloc(*s, requiredBlocks * blockSize);
    if (NULL==p) /* realloc failed */ 
      return 1; 
    else
      *s = p;
  }

  strcat(*s, ct);
  
  return 0;
}

/*
 * Similar to standard strlen(), but will return 0 for s=NULL,
 * instead of crashing
 */
int strlenEx(const char* s)
{
  if (NULL==s)
    return 0;
  else
    return (int)strlen(s);
}

/* Could probably make this a lot faster... */
int strtobool(const char* s)
{
  switch (strlenEx(s))
  {
    case 0:
      return 0;
    
    case 1:
      if ('1'==s[0]) return 1;
      if ('y'==s[0]) return 1;
      if ('Y'==s[0]) return 1;
      if ('t'==s[0]) return 1;
      if ('T'==s[0]) return 1;
      return 0;
      
    default:
      if (0==strcmp(s,"true")) return 1;
      if (0==strcmp(s,"True")) return 1;
      if (0==strcmp(s,"TRUE")) return 1;
      if (0==strcmp(s,"yes"))  return 1;
      if (0==strcmp(s,"Yes"))  return 1;
      if (0==strcmp(s,"YES"))  return 1;
      return 0;
  }
}

COMMONExpDLL
int strToVersionInfo(const char* s, versionInfo* version)
{
    const char* delim     = ".";
    char*       token     = NULL;
    int         i         = 0;
    int         j         = 0;
    int         retVal    = 0;
    char        copy[VERSION_INFO_STR_MAX];
    char*       verStart  = NULL;
    size_t      verStrLen = 0;

    if (NULL == version)
    {
        return retVal;
    }

    /* Zero set version so all versions are guaranteed NULL */
    memset (version, 0, sizeof(versionInfo));

    verStrLen = strlenEx(s);
    if (verStrLen == 0 || verStrLen > VERSION_INFO_STR_MAX)
    {
        return retVal;
    }

    /* Operate on copy of input as we want to get destructive */
    strcpy (copy, s);

    /* Skip past any non-numeric values at the start */
    for (i = 0; i < verStrLen; i++)
    {
        if (isdigit(copy[i]))
        {
            verStart = copy + i;
            break;
        }
    }
    /* If no numeric information was found, exit */
    if (NULL == verStart)
    {
        return retVal;
    }

    /* Only iterate 3 times for major / minor / release while tokens exist */
    token = strtok((char*)verStart, delim);
    for (i = 0; i < 3; i++)
    {
        /* There are no more tokens - break out */
        if (NULL == token)
        {
            break;
        }

        switch (i)
        {
        case 0:
            version->mMajor = atoi (token);
            break;
        case 1:
            version->mMinor = atoi (token);
            break;
        case 2:
            {
                /* Need extra parsing here to prune out leading numbers */
                for (j = 0; j < strlenEx(token); j++)
                {
                    /* If this is not an ascii number, extra starts here */
                    if (token[j] < 48 || token[j] > 57)
                    {
                        /* From this point on is the extra string - grab it */
                        strncpy (version->mExtra, token + j, sizeof(version->mExtra));
                        version->mExtra[VERSION_INFO_EXTRA_MAX - 1] = '\0';

                        /* Chop the string where the numbers stop */
                        token[j] = '\0';
                        break;
                    }
                }

                version->mRelease = atoi (token);
                retVal = 1;
                break;
            }
        }
        token = strtok(NULL, delim);
    }
    return retVal;
}

COMMONExpDLL
char* wstrsep (char** sp, char* sep)
{
    char *p, *s;
    if (sp == NULL || *sp == NULL || **sp == '\0')
        return (NULL);
    s = *sp;
    p = s + strcspn (s, sep);
    if (*p != '\0')
        *p++ = '\0';
    *sp = p;
    return (s);
}

COMMONExpDLL
char* strReplaceEnvironmentVariable(const char* value)
{
    /* Returns */
    char* variableStart = NULL;
    char* envValue      = NULL;
    char* envStart      = NULL;
    char* envEnd        = NULL;
    char* startString   = "$(";
    char* ret           = NULL;
    char envVar[1024]   = "";
    char startStr[4096] = "";
    char endStr[4096]   = "";
    char endChar        = ')';
    int valueLength     = 0;
    int startLength     = 0;
    int newStringLength = 0;
    size_t envVarLen    = 0;
    
    if (value != NULL && strchr(value, '$'))
    {
        ret = strdup(value);

        /* Make sure strdup didn't fail */
        if (NULL == ret)
        {
            goto out;
        }

        /* Scan the property for a $( which indicates an environment
         * variable or set ${ as the environment variable notation */
        if (NULL == strstr(ret, startString))
        {
            /* Change the start string */
            startString = "${";
            /* Change the end character */
            endChar = '}';
        }

        while (envStart = strstr(ret, startString))
        {

            /* Get a pointer to the first character of the variable itself. */
            variableStart = (envStart + 2);

            /* Locate the end of the string */
            envEnd = strchr( variableStart, endChar );

            if (envEnd == NULL)
            {
                goto out;
            }

            /* Save off the end of the string as later we realloc the source of the string */
            snprintf (endStr, strlen(envEnd)+1, "%s", envEnd);

            /* Obtain the length of the ret string, this must be done
             * before any replacements. */
            valueLength = strlen( ret );

            /* Obtain the length of the string before the $ variable */
            startLength = valueLength - strlen(variableStart) - 1;

            /* Make sure that we wouldn't overrun the buffer, otherwise return NULL */
            if (startLength > sizeof(startStr))
            {
                goto out;
            }

            /* Save off the string up to the $ variable */
            snprintf (startStr, startLength, "%s", ret);

            /* Work out the length of the environment variable */
            envVarLen = strlen(variableStart)-strlen(endStr)+1;

            snprintf (envVar, envVarLen, "%s", variableStart);

            /* Extract the variable name. */
            envValue = getenv (envVar);

            /* If a value was returned then a new property value must
             * be formatted. */
            if (envValue == NULL)
            {
                goto out;
            }

            /* Determine the number of characters needed to store
             * the new string. This is the length of the original
             * value plus the length of the environment variable
             * value minus the length of the environment variable
             * name.  The -2 at the end is due to the 3 characters
             * that wrap the environment variable name plus 1 for
             * the null terminator.
             */
            newStringLength = (valueLength -
                    strlen(envVar) + strlen(envValue) - 2);

            /* Reallocate a buffer to contain the new string if needed */
            if (newStringLength > strlen(ret))
            {
                ret = (char* )realloc((char*)ret, newStringLength);
            }

            if (ret == NULL)
            {
                goto out;
            }

            snprintf(ret, newStringLength, "%s%s%s", startStr, envValue, (endStr + 1));
        }
    }

    return ret;

out:
    free (ret);
    return NULL;
}

