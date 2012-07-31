/* $Id: log.c,v 1.30.4.8.10.6 2011/09/27 12:41:57 emmapollock Exp $
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
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "wombat/wincompat.h"
#include <mama/mama.h>
#include "mama/log.h"

#include "wombat/MRSWLock.h"
#include "property.h"
#include "mamainternal.h"

/* ******************************************************************* */
/* Defines */
/* ******************************************************************* */
/* This define contains the number of characters required to format a complete time
 * string including milliseconds.
 */
#define MAMALOG_TIME_BUFFER_LENGTH 40

/* This boolean property is used to control whether milliseconds are written to the time
 * log.
 */
#define MAMALOG_MILLISECONDS_PROPERTY_NAME "mama.logging.milliseconds"

#define LEVELPROPERTY       "mama.logging.level"
#define FILENAMEPROPERTY    "mama.logging.file.name"
#define POLICYPROPERTY      "mama.logging.file.policy"
#define FILESIZEPROPERTY    "mama.logging.file.maxsize"
#define ROLLPROPERTY        "mama.logging.file.maxroll"
#define APPENDPROPERTY      "mama.logging.file.append"


/* ******************************************************************* */
/* Globals */
/* ******************************************************************* */

/* The log file policy, this is one of the mamaLogFilePolicy enumeration values defined
 * in the header file.
 */
static mamaLogFilePolicy g_logFilePolicy = LOGFILE_UNBOUNDED;

/* This is the maximum size of the log file before it needs to roll over. */
static unsigned long g_maxLogSize = 500000000;		/* 500Mb  */

/* This flag will be set if milliseconds are to be included in time logging strings.
 * This is controlled by the boolean property mama.logging.milliseconds
 */
static int g_milliseconds = 0;

/* This lock controls access to all global resources. */
static PMRSWLock g_lock = NULL;

FILE*        gMamaLogFile               = NULL;
FILE*        gMamaControlledLogFile     = NULL;
MamaLogLevel gMamaLogLevel              = MAMA_LOG_LEVEL_WARN;
mamaLogCb    gMamaLogFunc               = mama_logDefault;
mamaLogCb    gMamaForceLogFunc          = mama_forceLogDefault;
static char* gMamaControlledLogFileName = NULL;

/* This static global is used to suppor the setLogCallback2 function required for C#. */
static mamaLogCb2    gMamaLogFunc2      = NULL;
static mama_bool_t   loggingToFile      = 0;
static logSizeCbType logSizeCb          = NULL;
static int           numRolledLogfiles  = 10;
static int           appendToFile       = 0;

/* ******************************************************************* */
/* Function Prototypes */
/* ******************************************************************* */

MRSW_RESULT			mamaLog_acquireLock(int read);
void				mamaLog_getTime(char *buffer, int bufferLength);
void				mamaLog_logLimitReached(void);
void MAMACALLTYPE	mamaLog_onLogCallback2(MamaLogLevel level, const char *format, va_list argumentList);
mama_status			mamaLog_openLogFile(const char* fileName, const char* mode, FILE** file);
mama_status			mamaLog_rollLogFiles(void);

/* ******************************************************************* */
/* Private Functions */
/* ******************************************************************* */

/**
 * This function will acquire the lock, this should be done before accessing any of the global resources.
 * @param read (int) 0 to acquire a write lock, 1 to acquire a read lock.
 */
MRSW_RESULT
mamaLog_acquireLock(int read)
{
	/* Returns. */
	MRSW_RESULT ret = MRSW_S_OK;

	/* If the lock hasn't been created then it must be created now. */
	if(NULL == g_lock)
	{
		ret = MRSWLock_create(&g_lock);
	}

	/* Acquire the lock. */
	if(MRSW_S_OK == ret)
	{
		ret = MRSWLock_acquire(g_lock, read);
	}

	return ret;
}

/*  Description :   This function will format a buffer with the current date and time, if the
 *                  g_milliseconds flag is set then this will also contain milliseconds.
 *  Arguments   :   buffer          [I] The buffer to format the string into, this should typically
 *                                      be at least MAMALOG_TIME_BUFFER_LENGTH.
 *                  bufferLength    [I] The number of characters in the buffer including the
 *                                      NULL terminator.
 */
void
mamaLog_getTime(char *buffer,
                int   bufferLength)
{
    /* Get the time now */
    struct timeval timeNow;
    memset(&timeNow, 0, sizeof(timeNow));
    gettimeofday(&timeNow, NULL);   
    {         
        /* Convert to local time zone */
        struct tm localTime;
        struct tm timeIn;
        memset(&localTime, 0, sizeof(localTime));
        memset(&timeIn, 0, sizeof(timeIn));        
        timeIn.tm_sec = timeNow.tv_sec;
        localtime_r ((const time_t*)&timeIn, &localTime);
        {
            /* Format the time string */
            size_t ft = strftime(buffer, bufferLength, "%Y-%m-%d %H:%M:%S:", &localTime);

            /* If 0 was returned above then the function has failed and the buffer is indeterminate */
            if(ft == 0)
            {
                /* Reset the buffer */
                strcpy(buffer, "");
            }
            else
            {
                /* Check to see if milliseconds should be appended */
                if(g_milliseconds != 0)
                {
                    /* Format the number */
                    char milliseconds[7] = "";
                    snprintf(milliseconds, 6, "%03d: ", (unsigned int)(timeNow.tv_usec / 1000));

                    /* Append the string */
                    strcat(buffer, milliseconds);
                }
                /* Alternatively just append a space */
                else
                {
                    strcat(buffer, " ");
                }
            }
        }
    }
}

/**
 * This function will check to see if the log file size limit has been reached and if so will perform
 * the action appropriate to the current log policy setting.
 * Note that if the policy is unbounded then nothing will be done.
 * This function must be called under a reader lock, depending on the policy this may be temporarily
 * upgraded to a writer lock.
 */
void
mamaLog_logLimitReached(void)
{
	/* The log limit only comes into effect if the policy is not unbounded. */
	if(g_logFilePolicy != LOGFILE_UNBOUNDED)
	{
		/* Check to see if the log file has reached the maximum size. */
		if(ftell(gMamaControlledLogFile) > g_maxLogSize)
		{
			/* Upgrade to a writer lock. */
			MRSW_RESULT lu = MRSWLock_upgrade(g_lock);
			if(MRSW_S_OK == lu)
			{
				/* Check the file size again under the writer lock in case another
				 * thread has rolled the file while this one has been waiting.
				 */
				if(ftell(gMamaControlledLogFile) > g_maxLogSize)
				{
				printf("Logfile size has been reached.\n");
				switch (g_logFilePolicy)
				{
					case LOGFILE_OVERWRITE:
						rewind (gMamaControlledLogFile);
						break;

					case LOGFILE_USER:
						if(logSizeCb)
						{
							logSizeCb();
						}
						else
						{
							if (0 >= numRolledLogfiles)
							{
								rewind (gMamaLogFile);
							}
							else
							{
								mamaLog_rollLogFiles ();
							}
						}
						break;

					default:
					case LOGFILE_ROLL:
						mamaLog_rollLogFiles ();
						break;
					}
				}

				/* Downgrade the lock. */
				MRSWLock_downgrade(g_lock);
			}
		}
	}
}

/*	Description	:	This function is used by mama_setLogCallback2 and will catch log messages
 *					and format the variable argument list as a stirng. This will then be passed
 *					back to the client via the gMamaLogFunc2 callback.
 *	Arguments	:	level			[I] The log level.
 *					format			[I] The format specifier.
 *					argumentList	[I] The variable argument list.
 */
void MAMACALLTYPE
mamaLog_onLogCallback2(MamaLogLevel level,
                       const char   *format,
                       va_list      argumentList)
{	
	/* Only continue if the callback has been supplied. */
	if(gMamaLogFunc2 != NULL)
	{
		/* Format the message as a string. */
		char buffer[4096] = "";
		/* Format the message into the buffer. */
		vsnprintf(buffer, (sizeof(buffer) - 1), format, argumentList);

		/* Add a null terminator. */
		strcat(buffer, "\0");

		/* Invoke the callback function. */
		(*gMamaLogFunc2)(level, buffer);		
	}
}

mama_status
mamaLog_openLogFile(const char *fileName,
                    const char *mode,
                    FILE       **file)
{
	/* Returns. */
	mama_status ret = MAMA_STATUS_INVALID_ARG;

	/* Open the file. */
    FILE *f = fopen(fileName, mode);
	if(NULL != f)
	{
		/* Write back the file handle. */
		*file = f;

		/* The function has succeeded. */
		ret = MAMA_STATUS_OK;
	}

    return ret;
}

/**
 * This function will roll the log files, note that is should be called under a write lock.
 * @return One of the mama_status return codes.
 */
mama_status
mamaLog_rollLogFiles(void)
{
    int i;
    char origFile[1024];
    char destFile[1024];
    const char* mode = "w";

    fclose (gMamaControlledLogFile);

	/* Need to delete the file with the highest number if it's there or 
	   the rolling breaks when there are numRolledLogfiles files on disk */
	snprintf (destFile, 1024, "%s%d", gMamaControlledLogFileName, numRolledLogfiles - 1);
	remove(destFile);

    for (i=(numRolledLogfiles-1); i > 0; --i)
    {
        /*add number to old files*/
        if (i > 1)
        {
            snprintf (origFile, 1024, "%s%d", gMamaControlledLogFileName, i-1);
        }
        else
        {
            snprintf (origFile, 1024, "%s", gMamaControlledLogFileName);
        }
        snprintf (destFile, 1024, "%s%d", gMamaControlledLogFileName, i);

		rename (origFile, destFile);
    }

    if (MAMA_STATUS_OK!=mamaLog_openLogFile ((const char*)gMamaControlledLogFileName,
                                      mode, &gMamaControlledLogFile))
    {
        return MAMA_STATUS_IO_ERROR;
    }

    return MAMA_STATUS_OK;
}

/* ******************************************************************* */
/* Public Functions */
/* ******************************************************************* */

/** 
 * This function should be called to clean up after the log and will
 * free all memory held.
 */
void
mama_logDestroy(void)
{
    /* Acquire the write lock. */
	mamaLog_acquireLock(0);	

    /* Destroy the file name */
    if(gMamaControlledLogFileName != NULL)
    {
        free(gMamaControlledLogFileName);
        gMamaControlledLogFileName = NULL;
    }

    /* Close the log file */
    if(gMamaControlledLogFile != NULL)
    {
        fclose(gMamaControlledLogFile);
        gMamaControlledLogFile = NULL;
    }

	/* Reset globals. */
	loggingToFile = 0;

	/* Release the write lock. */
	MRSWLock_release(g_lock, 0);

	/* Destroy the lock. */
	MRSWLock_free(g_lock);
	g_lock = NULL;
}

void
mama_loginit(void)
{
	/* Acquire the write lock. */
	MRSW_RESULT al = mamaLog_acquireLock(0);
	if(MRSW_S_OK == al)
	{
		/* Find out if milliseconds should be written, this must be done first before any logs are
		 * written.
		 */
		const char *propstring = properties_Get(mamaInternal_getProperties(), MAMALOG_MILLISECONDS_PROPERTY_NAME);
		if (propstring)
		{
			g_milliseconds = properties_GetPropertyValueAsBoolean(propstring);
		}

		propstring = properties_Get(mamaInternal_getProperties(), LEVELPROPERTY);
		if (propstring)
		{
			MamaLogLevel level = MAMA_LOG_LEVEL_OFF;
			if (1 == mama_tryStringToLogLevel(propstring, &level))
			{
				gMamaLogLevel = level;
			}
			else
			{
				mama_log (MAMA_LOG_LEVEL_WARN,
						"%s=%s not recognised.  Using default.",
						LEVELPROPERTY, propstring);
			}
		}

		propstring = properties_Get(mamaInternal_getProperties(), POLICYPROPERTY);
		if (propstring)
		{
			if  (0 == strcasecmp(propstring,"ROLL"))
			{
				g_logFilePolicy = LOGFILE_ROLL;
			}
			else if  (0 == strcasecmp(propstring,"OVERWRITE"))
			{
				g_logFilePolicy = LOGFILE_OVERWRITE;
			}
			else if  (0 == strcasecmp(propstring,"USER"))
			{
				g_logFilePolicy = LOGFILE_USER;
			}
			else
			{
				g_logFilePolicy = LOGFILE_UNBOUNDED;
			}
		}

		propstring = properties_Get(mamaInternal_getProperties(), FILESIZEPROPERTY);
		if (propstring)
		{
			g_maxLogSize = atoi(propstring);
		}

		propstring = properties_Get(mamaInternal_getProperties(), ROLLPROPERTY);
		if (propstring)
		{
			numRolledLogfiles = atoi(propstring);
		}

		propstring = properties_Get(mamaInternal_getProperties(), APPENDPROPERTY);
		if (propstring)
		{
			appendToFile = properties_GetPropertyValueAsBoolean(propstring);
		}

		propstring = properties_Get(mamaInternal_getProperties(), FILENAMEPROPERTY);
		if (propstring)
		{
			mama_logToFile (propstring, gMamaLogLevel);
		}

		/* Release the write lock. */
		MRSWLock_release(g_lock, 0);
	}    
}

void MAMACALLTYPE
mama_logDefault(MamaLogLevel level,
                const char   *format,
                va_list      ap)
{
	/* Acquire the read lock. */
	MRSW_RESULT al = mamaLog_acquireLock(1);
	if(MRSW_S_OK == al)
	{
		if ((gMamaLogLevel >= level) && (gMamaLogLevel != MAMA_LOG_LEVEL_OFF))
		{
			char    ts[MAMALOG_TIME_BUFFER_LENGTH] = "";       
			FILE*   f;
       
			if (loggingToFile)
			{
				mamaLog_logLimitReached ();            
				f = gMamaControlledLogFile;            
			}
			else
				f = (gMamaLogFile == NULL) ? stderr : gMamaLogFile;

			/* Format the current time */
			mamaLog_getTime(ts, MAMALOG_TIME_BUFFER_LENGTH);

			fprintf (f, "%s", ts);
			if (gMamaLogLevel == MAMA_LOG_LEVEL_FINEST)
			{
        		fprintf (f, "(%x) : ", (unsigned int)wGetCurrentThreadId());
			}

			vfprintf (f, format, ap);
			fprintf (f, "\n");
			fflush (f);
		}

		/* Release the read lock. */
		MRSWLock_release(g_lock, 1);
	}
}

void MAMACALLTYPE
mama_logStdout(MamaLogLevel level, const char *format, ...)
{
	/* Acquire the read lock. */
	MRSW_RESULT al = mamaLog_acquireLock(1);
	if(MRSW_S_OK == al)
	{
		if ((gMamaLogLevel >= level) && (gMamaLogLevel != MAMA_LOG_LEVEL_OFF))
		{
			va_list ap;
			FILE*   f;

			if (loggingToFile)
			{
				mamaLog_logLimitReached ();

				f = gMamaControlledLogFile;
			}
			else
				f = (gMamaLogFile == NULL) ? stdout : gMamaLogFile;

			va_start (ap, format);
			va_start (ap, format);
			vfprintf (f, format, ap);
			fflush (f);
			va_end(ap);
		}

		/* Release the read lock. */
		MRSWLock_release(g_lock, 1);
	}
}

void MAMACALLTYPE
mama_forceLogDefault (MamaLogLevel level,
                      const char* format,
                      va_list ap)
{
	/* Acquire the read lock. */
	MRSW_RESULT al = mamaLog_acquireLock(1);
	if(MRSW_S_OK == al)
	{
		char    ts[MAMALOG_TIME_BUFFER_LENGTH] = "";
		FILE*   f;

		if (loggingToFile)
		{
			mamaLog_logLimitReached ();
			f = gMamaControlledLogFile;
		}
		else
        {
			f = (gMamaLogFile == NULL) ? stderr : gMamaLogFile;
        }
		/* Format the current time */
		mamaLog_getTime(ts, MAMALOG_TIME_BUFFER_LENGTH);

		fprintf (f, "%s", ts);
		vfprintf (f, format, ap);
		fprintf (f, "\n");
		fflush (f);

		/* Release the read lock. */
		MRSWLock_release(g_lock, 1);
	}
}

mama_status
mama_setLogSize(unsigned long size)
{	
	/* Returns. */
	mama_status ret = MAMA_STATUS_PLATFORM;

	/* Acquire the write lock. */
	MRSW_RESULT al = mamaLog_acquireLock(0);
	if(MRSW_S_OK == al)
	{
		/* Write the value. */
		g_maxLogSize = size;

		/* Release the write lock. */
		MRSWLock_release(g_lock, 0);

		/* The function has succeeded. */
		ret = MAMA_STATUS_OK;
	}

    return ret;
}

mama_status
mama_setNumLogFiles(int numFiles)
{	
	/* Returns. */
	mama_status ret = MAMA_STATUS_PLATFORM;

	/* Acquire the write lock. */
	MRSW_RESULT al = mamaLog_acquireLock(0);
	if(MRSW_S_OK == al)
	{
		/* Write the value. */
		numRolledLogfiles = numFiles;

		/* Release the write lock. */
		MRSWLock_release(g_lock, 0);

		/* The function has succeeded. */
		ret = MAMA_STATUS_OK;
	}

    return ret;
}

mama_status
mama_setLogFilePolicy(mamaLogFilePolicy policy)
{
	/* Returns. */
	mama_status ret = MAMA_STATUS_PLATFORM;

	/* Acquire the write lock. */
	MRSW_RESULT al = mamaLog_acquireLock(0);
	if(MRSW_S_OK == al)
	{
		/* Write the value. */
		g_logFilePolicy = policy;

		/* Release the write lock. */
		MRSWLock_release(g_lock, 0);

		/* The function has succeeded. */
		ret = MAMA_STATUS_OK;
	}

    return ret;
}

mama_status
mama_setAppendToLogFile(int append)
{
	/* Returns. */
	mama_status ret = MAMA_STATUS_PLATFORM;

	/* Acquire the write lock. */
	MRSW_RESULT al = mamaLog_acquireLock(0);
	if(MRSW_S_OK == al)
	{
		/* Write the value. */
		appendToFile = append;

		/* Release the write lock. */
		MRSWLock_release(g_lock, 0);

		/* The function has succeeded. */
		ret = MAMA_STATUS_OK;
	}

    return ret;
}

int
mama_loggingToFile(void)
{
	/* Returns. */
	int ret = -1;

	/* Acquire the read lock. */
	MRSW_RESULT al = mamaLog_acquireLock(1);
	if(MRSW_S_OK == al)
	{
		/* Read the value. */
		ret = loggingToFile;
		
		/* Release the read lock. */
		MRSWLock_release(g_lock, 1);
	}

    return ret;
}

mama_status
mama_setLogSizeCb(logSizeCbType callback)
{
	/* Returns. */
	mama_status ret = MAMA_STATUS_NULL_ARG;
	if(callback != NULL)
	{		
		/* Acquire the write lock. */
		MRSW_RESULT al = mamaLog_acquireLock(0);
		ret = MAMA_STATUS_PLATFORM;
		if(MRSW_S_OK == al)
		{
			/* Write the value. */
			logSizeCb = callback;

			/* Release the write lock. */
			MRSWLock_release(g_lock, 0);

			/* The function has succeeded. */
			ret = MAMA_STATUS_OK;
		}
	}

    return ret;
}

mama_status
mama_setLogCallback(mamaLogCb callback)
{
	/* Returns. */
	mama_status ret = MAMA_STATUS_NULL_ARG;
	if(callback != NULL)
	{		
		/* Acquire the write lock. */
		MRSW_RESULT al = mamaLog_acquireLock(0);
		ret = MAMA_STATUS_PLATFORM;
		if(MRSW_S_OK == al)
		{
			/* Write the value. */
			gMamaLogFunc = callback;

			/* Release the write lock. */
			MRSWLock_release(g_lock, 0);

			/* The function has succeeded. */
			ret = MAMA_STATUS_OK;
		}
	}

    return ret;
}

mama_status 
mama_setLogCallback2(mamaLogCb2 callback)
{
	/* Returns. */
	mama_status ret = MAMA_STATUS_PLATFORM;

	/* Acquire the write lock. */
	MRSW_RESULT al = mamaLog_acquireLock(0);	
	if(MRSW_S_OK == al)
	{
		/* Save the callback in the global variable. */
		gMamaLogFunc2 = callback;

		/* If NULL is supplied then return the log callback to the default value. */	
		if(callback == NULL)
		{
			gMamaLogFunc = (mamaLogCb)mama_logDefault;
		}

		else
		{
			/* Set a custom function that will format the variable argument list passed into the
			 * normal log callback.
			 */
			gMamaLogFunc = (mamaLogCb)mamaLog_onLogCallback2;
		}

		/* Release the write lock. */
		MRSWLock_release(g_lock, 0);

		/* The function has succeeded. */
		ret = MAMA_STATUS_OK;
	}

	return ret;
}

mama_status
mama_setForceLogCallback (mamaLogCb callback)
{
	/* Returns. */
	mama_status ret = MAMA_STATUS_NULL_ARG;
	if(callback != NULL)
	{		
		/* Acquire the write lock. */
		MRSW_RESULT al = mamaLog_acquireLock(0);
		ret = MAMA_STATUS_PLATFORM;
		if(MRSW_S_OK == al)
		{
			/* Write the value. */
			gMamaForceLogFunc = callback;

			/* Release the write lock. */
			MRSWLock_release(g_lock, 0);

			/* The function has succeeded. */
			ret = MAMA_STATUS_OK;
		}
	}

    return ret;
}

mama_status
mama_enableLogging (FILE         *f,
                    MamaLogLevel level)
{
	/* Returns. */
	mama_status ret = MAMA_STATUS_PLATFORM;

	/* Acquire the write lock. */
	MRSW_RESULT al = mamaLog_acquireLock(0);
	if(MRSW_S_OK == al)
	{
		/* Write the values. */
		gMamaLogFile	= f;
		gMamaLogLevel	= level;

		/* Release the write lock. */
		MRSWLock_release(g_lock, 0);

		/* The function has succeeded. */
		ret = MAMA_STATUS_OK;
	}

    return ret;
}

mama_status
mama_disableLogging(void)
{
    /* Returns. */
	mama_status ret = MAMA_STATUS_PLATFORM;

	/* Acquire the write lock. */
	MRSW_RESULT al = mamaLog_acquireLock(0);
	if(MRSW_S_OK == al)
	{
		if(gMamaControlledLogFile)
		{
			fclose (gMamaControlledLogFile);
			gMamaControlledLogFile = NULL;
		}

		if(gMamaLogFile)
		{
			/* We didn't open it so won't close it */
			gMamaLogFile = NULL;
		}

		if(logSizeCb)
		{
			logSizeCb = NULL;
		}

		gMamaControlledLogFileName = NULL;
		gMamaLogLevel = MAMA_LOG_LEVEL_OFF;
		loggingToFile = 0;

		/* Release the write lock. */
		MRSWLock_release(g_lock, 0);

		/* The function has succeeded. */
		ret = MAMA_STATUS_OK;
	}
	
    return ret;
}

mama_status
mama_logToFile (const char*  file,
                MamaLogLevel level)
{
	/* Returns. */
	mama_status ret = MAMA_STATUS_INVALID_ARG;
	if((NULL != file) && (0 != strcmp(file, "")))
	{
		/* Acquire the write lock. */
		MRSW_RESULT al = mamaLog_acquireLock(0);
		ret = MAMA_STATUS_PLATFORM;
		if(MRSW_S_OK == al)
		{
			const char* mode = "w";
			if(appendToFile)
			{
				mode = "a";
			}

			if ((file[0] != '/') && (file[0] != '.') && (file[1] != ':'))
			{
				/* Not an absolute pathname or an explicit relative pathname
				* so try to find it in $WOMBAT_PATH. */
				char* wombatPath = getenv ("WOMBAT_PATH");

				if (wombatPath)
				{
					char tmpFileName[1024];
					snprintf (tmpFileName, 1024, "%s%s%s", wombatPath, PATHSEP, file);
					gMamaControlledLogFileName = strdup (tmpFileName);
				}
				else
				{
					gMamaControlledLogFileName = strdup (file);
				}
			}
			else
			{
				gMamaControlledLogFileName = strdup (file);
			}

			/* Open the log file. */
			ret = mamaLog_openLogFile(gMamaControlledLogFileName, mode, &gMamaControlledLogFile);
			if(MAMA_STATUS_OK == ret)
			{
				/* Write the values. */
				gMamaLogLevel = level;
				loggingToFile = 1;
			}

			else
			{
				mama_log (MAMA_LOG_LEVEL_ERROR, "Could not open log file for writing [%s]",	file);
				ret = MAMA_STATUS_IO_ERROR;
			}

			/* Release the write lock. */
			MRSWLock_release(g_lock, 0);
			
			ret = MAMA_STATUS_OK;
		}		
	}

    return ret;
}

void
mama_log (MamaLogLevel level, const char *format, ...)
{
	/* Get the log function and the log level under the reader lock. */
	MamaLogLevel currentLevel	= 0;
	mamaLogCb	 logFunction	= NULL;

	/* Acquire the read lock. */
	mamaLog_acquireLock(1);

	/* Get the variables. */
	currentLevel	= gMamaLogLevel;
	logFunction		= gMamaLogFunc;
	
	/* Release the read lock as quickly as possible. */
	MRSWLock_release(g_lock, 1);
	
	/* Only log if this level is valid. */
	if((currentLevel >= level) && (currentLevel != MAMA_LOG_LEVEL_OFF))
    {
		/* Convert the variable argument list into a va_list object for passing amongst functions. */
        va_list ap;
        va_start (ap, format);

		/* Call the log function. */
        logFunction(level, format, ap);

		/* Finished with the variable argument list. */
        va_end (ap);
    }
}

void MAMACALLTYPE
mama_log2 (MamaLogLevel level, 
           const char   *message)
{
	/* Call mama_log with a format string. */
	mama_log (level, "%s", message);    
}

void MAMACALLTYPE
mama_logDefault2 (MamaLogLevel level,
                  const char *message)
{
	/* Acquire the read lock. */
	mamaLog_acquireLock(1);

	if ((gMamaLogLevel >= level) && (gMamaLogLevel != MAMA_LOG_LEVEL_OFF))
    {
        char    ts[MAMALOG_TIME_BUFFER_LENGTH] = "";       
        
        FILE*   f;      

        if (loggingToFile)
        {
            mamaLog_logLimitReached ();            
            f = gMamaControlledLogFile;            
        }
        else
            f = (gMamaLogFile == NULL) ? stderr : gMamaLogFile;

        /* Format the current time */
        mamaLog_getTime(ts, MAMALOG_TIME_BUFFER_LENGTH);

		fprintf (f, "%s", ts);
        if (gMamaLogLevel == MAMA_LOG_LEVEL_FINEST)
		{
        	fprintf (f, "(%x) : ", (unsigned int)wGetCurrentThreadId());
		}

		fprintf (f, message);        
        fprintf (f, "\n");
        fflush (f);
    }

	/* Release the read lock. */
	MRSWLock_release(g_lock, 1);
}

void
mama_logVa(MamaLogLevel level, 
           const char   *format, 
           va_list      args)
{
	/* Get the log function and the log level under the reader lock. */
	MamaLogLevel currentLevel	= 0;
	mamaLogCb	 logFunction	= NULL;

	/* Acquire the read lock. */
	mamaLog_acquireLock(1);

	/* Get the variables. */
	currentLevel	= gMamaLogLevel;
	logFunction		= gMamaLogFunc;
	
	/* Release the read lock as quickly as possible. */
	MRSWLock_release(g_lock, 1);
	
	/* Only log if this level is valid. */
	if((currentLevel >= level) && (currentLevel != MAMA_LOG_LEVEL_OFF))
    {
		/* Call the log function. */
        logFunction(level, format, args);
    }
}

void
mama_forceLog (MamaLogLevel level, const char *format, ...)
{
	/* Get the log function under the reader lock. */
	mamaLogCb forceLogFunction = NULL;

	/* Acquire the read lock. */
	mamaLog_acquireLock(1);

	/* Get the variables. */
	forceLogFunction = gMamaForceLogFunc;
	
	/* Release the read lock as quickly as possible. */
	MRSWLock_release(g_lock, 1);	
	{
		/* Create the va_list. */
		va_list ap;
		va_start (ap, format);

		/* Call the log function .*/
		forceLogFunction(level, format, ap);

		/* Finished with the va list. */
		va_end (ap);
	}
}

mama_status
mama_setLogLevel (MamaLogLevel level)
{
	/* Returns. */
	mama_status ret = MAMA_STATUS_NULL_ARG;

	/* Check that the supplied level is valid. */
	if ((level >= MAMA_LOG_LEVEL_OFF) && (level <= MAMA_LOG_LEVEL_FINEST))
    {		
		/* Acquire the write lock. */
		MRSW_RESULT al = mamaLog_acquireLock(0);
		ret = MAMA_STATUS_PLATFORM;
		if(MRSW_S_OK == al)
		{
			/* Write the value. */
			gMamaLogLevel = level;

			/* Release the write lock. */
			MRSWLock_release(g_lock, 0);

			/* The function has succeeded. */
			ret = MAMA_STATUS_OK;
		}
	}

    return ret;
}

MamaLogLevel
mama_getLogLevel(void)
{
	/* Returns. */
	MamaLogLevel ret = MAMA_LOG_LEVEL_OFF;

	/* Acquire the read lock. */
	MRSW_RESULT al = mamaLog_acquireLock(1);
	if(MRSW_S_OK == al)
	{
		/* Read the value. */
		ret = gMamaLogLevel;
		
		/* Release the read lock. */
		MRSWLock_release(g_lock, 1);
	}

    return ret;
}

const char*
mama_logLevelToString(MamaLogLevel level)
{
    switch(level)
    {
        case MAMA_LOG_LEVEL_OFF:    return "Off";
        case MAMA_LOG_LEVEL_SEVERE: return "Severe";
        case MAMA_LOG_LEVEL_ERROR:  return "Error";
        case MAMA_LOG_LEVEL_WARN:   return "Warn";
        case MAMA_LOG_LEVEL_NORMAL: return "Normal";
        case MAMA_LOG_LEVEL_FINE:   return "Fine";
        case MAMA_LOG_LEVEL_FINER:  return "Finer";
        case MAMA_LOG_LEVEL_FINEST: return "Finest";
        default:                    return "???";
    }
}

int
mama_tryStringToLogLevel(const char*   s,
                         MamaLogLevel* level)
{
    if (NULL == s)
    {
        return 0;
    }
    else if (0==strcasecmp(s,"Off"))
    {
        *level=MAMA_LOG_LEVEL_OFF;
    }
    else if (0==strcasecmp(s,"Severe"))
    {
        *level=MAMA_LOG_LEVEL_SEVERE;
    }
    else if (0==strcasecmp(s,"Error"))
    {
        *level=MAMA_LOG_LEVEL_ERROR;
    }
    else if (0==strcasecmp(s,"Warn"))
    {
        *level=MAMA_LOG_LEVEL_WARN;
    }
    else if (0==strcasecmp(s,"Normal"))
    {
        *level=MAMA_LOG_LEVEL_NORMAL;
    }
    else if (0==strcasecmp(s,"Fine"))
    {
        *level=MAMA_LOG_LEVEL_FINE;
    }
    else if (0==strcasecmp(s,"Finer"))
    {
        *level=MAMA_LOG_LEVEL_FINER;
    }
    else if (0==strcasecmp(s,"Finest"))
    {
        *level=MAMA_LOG_LEVEL_FINEST;
    }
    else
    {
        return 0;
    }

    return 1;
}

int
mama_logIncrementVerbosity(MamaLogLevel* level)
{
    if (NULL==level)
        return 0;

    switch(*level)
    {
        case MAMA_LOG_LEVEL_OFF:    *level = MAMA_LOG_LEVEL_SEVERE; return 1;
        case MAMA_LOG_LEVEL_SEVERE: *level = MAMA_LOG_LEVEL_ERROR;  return 1;
        case MAMA_LOG_LEVEL_ERROR:  *level = MAMA_LOG_LEVEL_WARN;   return 1;
        case MAMA_LOG_LEVEL_WARN:   *level = MAMA_LOG_LEVEL_NORMAL; return 1;
        case MAMA_LOG_LEVEL_NORMAL: *level = MAMA_LOG_LEVEL_FINE;   return 1;
        case MAMA_LOG_LEVEL_FINE:   *level = MAMA_LOG_LEVEL_FINER;  return 1;
        case MAMA_LOG_LEVEL_FINER:  *level = MAMA_LOG_LEVEL_FINEST; return 1;
        case MAMA_LOG_LEVEL_FINEST: return 0;
        /* No default case, we want a compiler warning for unhandled cases */
    }

    /* In case level does not match one of the enumerated types */
    *level = MAMA_LOG_LEVEL_OFF;
    return 1;
}

int mama_logDecrementVerbosity(MamaLogLevel* level)
{
    if (NULL==level)
        return 0;

    switch(*level)
    {
        case MAMA_LOG_LEVEL_OFF:    return 0;
        case MAMA_LOG_LEVEL_SEVERE: *level = MAMA_LOG_LEVEL_OFF;    return 1;
        case MAMA_LOG_LEVEL_ERROR:  *level = MAMA_LOG_LEVEL_SEVERE; return 1;
        case MAMA_LOG_LEVEL_WARN:   *level = MAMA_LOG_LEVEL_ERROR;  return 1;
        case MAMA_LOG_LEVEL_NORMAL: *level = MAMA_LOG_LEVEL_WARN;   return 1;
        case MAMA_LOG_LEVEL_FINE:   *level = MAMA_LOG_LEVEL_NORMAL; return 1;
        case MAMA_LOG_LEVEL_FINER:  *level = MAMA_LOG_LEVEL_FINE;   return 1;
        case MAMA_LOG_LEVEL_FINEST: *level = MAMA_LOG_LEVEL_FINER;  return 1;
        /* No default case, we want a compiler warning for unhandled cases */
    }

    /* In case level does not match one of the enumerated types */
    *level = MAMA_LOG_LEVEL_FINEST;
    return 1;
}
