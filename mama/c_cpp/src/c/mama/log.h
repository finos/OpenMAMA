/* $Id: log.h,v 1.30.4.2.16.4 2011/08/10 14:53:28 nicholasmarriott Exp $
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

#ifndef MamaLogH__
#define MamaLogH__

#include <stdio.h>
#include <mama/config.h>
#include <stdarg.h>
#ifdef __cplusplus
extern "C"
{
#endif

#include "mama/status.h"

/** The level of detail when logging is enabled within the API */
typedef enum
{
    MAMA_LOG_LEVEL_OFF    = 0,
    MAMA_LOG_LEVEL_SEVERE = 1,
    MAMA_LOG_LEVEL_ERROR  = 2,
    MAMA_LOG_LEVEL_WARN   = 3,
    MAMA_LOG_LEVEL_NORMAL = 4,
    MAMA_LOG_LEVEL_FINE   = 5,
    MAMA_LOG_LEVEL_FINER  = 6,
    MAMA_LOG_LEVEL_FINEST = 7
} MamaLogLevel;

/** The policy to control log file size */
typedef enum
{
    LOGFILE_UNBOUNDED   = 1,
    LOGFILE_ROLL        = 2,
    LOGFILE_OVERWRITE   = 3,
    LOGFILE_USER        = 4
} mamaLogFilePolicy;


/** The prototype of the MAMA logging functions */
typedef void (MAMACALLTYPE *mamaLogCb)  (MamaLogLevel level, const char *format, va_list ap);
typedef void (MAMACALLTYPE *mamaLogCb2) (MamaLogLevel level, const char *message);
typedef void (*logSizeCbType) (void);

/** The default logging within the API unless otherwise specified */
MAMAExpDLL
extern void MAMACALLTYPE
mama_logDefault (MamaLogLevel level, const char *format, va_list ap);

/** 
 * This second logging function takes only a message and not a format string with
 * a variable argument list. It is required for interoperability with all platforms
 * that do not support C variable argument list, (e.g. .Net).
 * Other than that it performs in exactly the same way as the first.
 *
 * @param level (in) The log level.
 * @param message (in) The message to log.
 */
MAMAExpDLL
extern void MAMACALLTYPE
mama_logDefault2 (MamaLogLevel level, const char *message);

MAMAExpDLL
extern void
mama_logStdout (MamaLogLevel level, const char *format, ...);

/** The default function used within the API for the mama_forceLog function
 * pointer
 */
MAMAExpDLL
extern void MAMACALLTYPE
mama_forceLogDefault (MamaLogLevel level, const char *format, va_list ap);

/** The current log level within the API*/
MAMAExpDLL
extern MamaLogLevel   gMamaLogLevel;

/** The file to which all logging will be written by default*/
MAMAExpDLL
extern FILE*          gMamaLogFile;

/**
 * Enable logging. No per-message or per-tick messages appear at
 * <code>WOMBAT_LOG_LEVEL_FINE</code>. <code>WOMBAT_LOG_LEVEL_FINER</code> and
 * <code>WOMBAT_LOG_LEVEL_FINEST</code> provide successively more detailed
 * logging.
 *
 * @param file    File to write to.
 * @param level   Output level.
 * @see Level
 */
MAMAExpDLL
extern mama_status
mama_enableLogging (FILE *file, MamaLogLevel level);

/**
 * Behaves as mama_enableLogging() but accepts a string representing the file
 * location.
 *
 * @param file The path to the file. Can be relative, absolute or on
 * $WOMBAT_PATH.
 * @param level The level at which the API should log messages.
 */
MAMAExpDLL
extern mama_status
mama_logToFile (const char* file, MamaLogLevel level);

/** Disable logging. */
MAMAExpDLL
extern mama_status
mama_disableLogging(void);

/** Used  for the majority of logging within the API*/
MAMAExpDLL
extern void
mama_log (MamaLogLevel level, const char *format, ...);

MAMAExpDLL
extern void MAMACALLTYPE
mama_log2 (MamaLogLevel level, const char *message);

MAMAExpDLL
extern void
mama_logVa (MamaLogLevel level, const char *format, va_list args);

MAMAExpDLL
extern void
mama_forceLog (MamaLogLevel level, const char *format, ...);

/**
 * Set the callback to be used for mama_log calls. If not set then
 * mama_logDefault will be used
 */
MAMAExpDLL
extern mama_status
mama_setLogCallback (mamaLogCb callback);

/**
 * Set the callback to be used for mama_log calls. This function
 * will set a log callback that receives a formatted string and not
 * a variable argument list. This function is used mainly to support
 * managed clients.
 *
 * @param callback (in) The callback to be used. Pass NULL to restore
 *						the mama_logDefault function.
 * @return MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mama_setLogCallback2 (mamaLogCb2 callback);

/**
 * Set the callback to be used for mama logging. If not set then
 * mama_ForceLogDefault will be used
 */
MAMAExpDLL
extern mama_status
mama_setForceLogCallback (mamaLogCb callback);

/** Sets the log level for Mama */
MAMAExpDLL
extern mama_status
mama_setLogLevel (MamaLogLevel level);

/** Returns the current log level for Mama. */
MAMAExpDLL
extern MamaLogLevel
mama_getLogLevel (void);

/**
 * Set the maxmum size of the log file (bytes). When this size is reached the
 * logsize callback is called, or if no callback is set then the default action
 * is to overwrite file from the start.
 * Default max size is 500 Mb
 */
MAMAExpDLL
extern mama_status
mama_setLogSize (unsigned long size);

/**
 * Set the number of rolled logfiles to keep before overwriting.
 * Default is 10
 */
MAMAExpDLL
extern mama_status
mama_setNumLogFiles(int numFiles);

/**
 * Set the policy regarding how to handle files when Max file size is reached.
 * Default is LOGFILE_UNBOUNDED
 */
MAMAExpDLL
extern mama_status
mama_setLogFilePolicy(mamaLogFilePolicy policy);

/** Set append to existing log file */
MAMAExpDLL
extern mama_status
mama_setAppendToLogFile(int append);

/** Return status of loggingToFile */
MAMAExpDLL
extern int
mama_loggingToFile(void);

/**
 * Set a callback for when the max log size is reached.  This can be used to
 * override the default action which is to wrap the file and continue logging at
 * the beginning
 */
MAMAExpDLL
extern mama_status
mama_setLogSizeCb(logSizeCbType logCallbacks);

/** Return string version of log level */
MAMAExpDLL
extern const char*
mama_logLevelToString(MamaLogLevel level);

/**
 * Try to convert string to log level
 * Return non-zero for success, zero for failure
 * The string comparison is case insensitive
 */
MAMAExpDLL
extern int
mama_tryStringToLogLevel(const char* s, MamaLogLevel* level);

/** Return string version of log policy */
MAMAExpDLL
extern const char*
mama_logPolicyToString(mamaLogFilePolicy level);

/**
 * Try to convert string to log policy.
 * Return non-zero for success, zero for failure.
 * The string comparison is case insensitive.
 */
MAMAExpDLL
extern int
mama_tryStringToLogPolicy(const char* s, mamaLogFilePolicy* policy);

/**
 * Increase by one log level the verbosity of a MamaLogLevel variable.
 * If the level is already at the maximum verbosity it will be unchanged 
 * after calling the function, otherwise the level will be incremented.
 * Returns zero if level is not changed, or non-zero if it is changed
 * If an unrecognized level is passed, the function will return 
 * non-zero and the variable will be set to the minimum verbosity
 */
MAMAExpDLL
extern int
mama_logIncrementVerbosity(MamaLogLevel* level);

/**
 * Decrease by one log level the verbosity of a MamaLogLevel variable.
 * If the level is already at the minimum verbosity it will be unchanged 
 * after calling the function, otherwise the level will be decremented.
 * Returns zero if level is not changed, or non-zero if it is changed
 * If an unrecognized level is passed, the function will return 
 * non-zero and the variable will be set to the maximum verbosity
 */
MAMAExpDLL
extern int
mama_logDecrementVerbosity(MamaLogLevel* level);

/**  Destroy memory held by the logging */
void
mama_logDestroy(void);

void
mama_loginit (void);

#ifdef __cplusplus
} /* MAMAExpDLL
extern "C" */
#endif

#endif /* MAMA_LOG_H__ */
