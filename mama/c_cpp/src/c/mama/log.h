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

/**
 * @brief MAMA Log Level
 *
 * @details The level of detail when logging is enabled within the API
 */
typedef enum
{
    MAMA_LOG_LEVEL_OFF    = 0,  /**< No logging */
    MAMA_LOG_LEVEL_SEVERE = 1,  /**< Severe logging level */
    MAMA_LOG_LEVEL_ERROR  = 2,  /**< Error logging level */
    MAMA_LOG_LEVEL_WARN   = 3,  /**< Warning logging level */
    MAMA_LOG_LEVEL_NORMAL = 4,  /**< Normal log level */
    MAMA_LOG_LEVEL_FINE   = 5,  /**< Start/Shutdown logging level */
    MAMA_LOG_LEVEL_FINER  = 6,  /**< Object creation logging level */
    MAMA_LOG_LEVEL_FINEST = 7   /**< Message logging level */
} MamaLogLevel;

/**
 * @brief MAMA Log Policy
 *
 * @details The policy to control log file size
 */
typedef enum
{
    LOGFILE_UNBOUNDED   = 1,    /**< Default - No restrictions */
    LOGFILE_ROLL        = 2,    /**< Logfile will roll */
    LOGFILE_OVERWRITE   = 3,    /**< Logfile will overwrite */
    LOGFILE_USER        = 4     /**< User defined callback */
} mamaLogFilePolicy;


/**
 * @brief Definition of a callback
 */
typedef void (MAMACALLTYPE *mamaLogCb)  (MamaLogLevel level, const char *format, va_list ap);

/**
 * @brief Definition of a callback
 */
typedef void (MAMACALLTYPE *mamaLogCb2) (MamaLogLevel level, const char *message);

/**
 * @brief Definition of a callback
 */
typedef void (*logSizeCbType) (void);

/**
 * @brief The default logging within the API unless otherwise specified
 *
 * @param[in] level The log level.
 * @param[in] format The format of log message.
 * @param[in] ap Variable argument list.
 */
MAMAExpDLL
extern void MAMACALLTYPE
mama_logDefault (MamaLogLevel level, const char *format, va_list ap);

/** 
 * @brief Second Log Function
 *
 * @details This second logging function takes only a message and not a format string with
 * a variable argument list. It is required for interoperability with all platforms
 * that do not support C variable argument list, (e.g. .Net).
 * Other than that it performs in exactly the same way as the first.
 *
 * @param[in] level The log level.
 * @param[in] message The message to log.
 */
MAMAExpDLL
extern void MAMACALLTYPE
mama_logDefault2 (MamaLogLevel level, const char *message);

/**
 * @brief Log to Standard Output
 *
 * @param[in] level The log level.
 * @param[in] format The format of log message.
 * @param[in] ... Variable argument list.
 */
MAMAExpDLL
extern void
mama_logStdout (MamaLogLevel level, const char *format, ...);

/**
 * @brief Forced Log Function
 *
 * @details The default function used within the API for the mama_forceLog function
 * pointer.  If no logfile is available logging it to stderr.
 *
 * @param[in] level The log level.
 * @param[in] format The format of log message.
 * @param[in] ap Variable argument list.
 */
MAMAExpDLL
extern void MAMACALLTYPE
mama_forceLogDefault (MamaLogLevel level, const char *format, va_list ap);


/**
 * The current log level within the API
 * deprecated - The helper functions should be used instead of this
 */
MAMAExpDLL
extern MamaLogLevel   gMamaLogLevel;

/**
 * The file to which all logging will be written by default
 * deprecated - The helper functions should be used instead of this
 */
MAMAExpDLL
extern FILE*          gMamaLogFile;

/**
 * @brief Enable Logging.
 *
 * @details No per-message or per-tick messages appear at
 * <code>WOMBAT_LOG_LEVEL_FINE</code>. <code>WOMBAT_LOG_LEVEL_FINER</code> and
 * <code>WOMBAT_LOG_LEVEL_FINEST</code> provide successively more detailed
 * logging.
 *
 * @param[in] file  File to write to.
 * @param[in] level Output level.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_PLATFORM
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mama_enableLogging (FILE *file, MamaLogLevel level);

/**
 * @brief Behaves as mama_enableLogging() but accepts a string representing the file
 * location.
 *
 * @param[in] file The path to the file. Can be relative, absolute or on
 * $WOMBAT_PATH.
 * @param[in] level The level at which the API should log messages.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_PLATFORM
 *              MAMA_STATUS_IO_ERROR
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mama_logToFile (const char* file, MamaLogLevel level);

/**
 * @brief Disable logging
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_INVALID_ARG
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mama_disableLogging(void);

/**
 * @brief Used for the majority of logging within the API
 *
 * @param[in] level The log level.
 * @param[in] format The format of log message.
 * @param[in] ... Variable argument list.
 */
MAMAExpDLL
extern void
mama_log (MamaLogLevel level, const char *format, ...);

/**
 * @brief Second Log Function
 *
 * @param[in] level The log level.
 * @param[in] message The message it log.
 */
MAMAExpDLL
extern void MAMACALLTYPE
mama_log2 (MamaLogLevel level, const char *message);

/**
 * @brief Used for of logging within the API
 *
 * @param[in] level The log level.
 * @param[in] format The format of log message.
 * @param[in] args Variable argument list.
 */
MAMAExpDLL
extern void
mama_logVa (MamaLogLevel level, const char *format, va_list args);

/**
 * @brief Used the force logging using variable argument parameters
 *
 * @param[in] format The format of log message.
 * @param[in] args Variable argument list.
 */
MAMAExpDLL
extern void
mama_forceLogVa(const char   *format, va_list  args);

/** Used to add a custom prefix to the start of the line being loggedf */
MAMAExpDLL
extern void
mama_forceLogVaWithPrefix (const char* level,
                           const char* format,
                           va_list     args);


/**
 * @brief Used for of logging within the API
 *
 * @param[in] level The log level.
 * @param[in] format The format of log message.
 * @param[in] ... Variable argument list.
 */
MAMAExpDLL
extern void
mama_forceLog (MamaLogLevel level, const char *format, ...);

/**
 * @brief Set the callback to be used for mama_log calls. If not set then
 * mama_logDefault will be used.
 *
 * @param[in] callback The callback to be set
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_PLATFORM
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mama_setLogCallback (mamaLogCb callback);

/**
 * @brief Second set callback function
 *
 * @details Set the callback to be used for mama_log calls. This function
 * will set a log callback that receives a formatted string and not
 * a variable argument list. This function is used mainly to support
 * managed clients.
 *
 * @param[in] callback The callback to be used. Pass NULL to restore
 *						the mama_logDefault function.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_PLATFORM
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mama_setLogCallback2 (mamaLogCb2 callback);

/**
 * @brief Set the callback to be used for mama logging. If not set then
 * mama_ForceLogDefault will be used.
 *
 * @param[in] callback Callback to be set.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_PLATFORM
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mama_setForceLogCallback (mamaLogCb callback);

/**
 * @brief Sets the log level for Mama.
 *
 * @param[in] level The log level to be set.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_PLATFORM
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mama_setLogLevel (MamaLogLevel level);

/**
 * @brief Returns the current log level for Mama.
 *
 * @return MamaLogLevel.
 */
MAMAExpDLL
extern MamaLogLevel
mama_getLogLevel (void);

/**
 * @brief Set the maxmum size of the log file (bytes).
 *
 * @details When this size is reached the logsize callback is called, or
 * if no callback is set then the default action is to overwrite file
 * from the start.
 * Default max size is 500 Mb
 *
 * @param[in] size The max size of a logfile
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_PLATFORM
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mama_setLogSize (unsigned long size);

/**
 * @brief Set the number of rolled logfiles to keep before overwriting.
 *
 * @details Default is 10.
 *
 * @param[in] numFiles The number of files to keep
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_PLATFORM
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mama_setNumLogFiles(int numFiles);

/**
 * @brief Set the policy regarding how to handle files when Max file size is reached.
 *
 * @details Default is LOGFILE_UNBOUNDED.
 *
 * @param[in] policy The MAMA logfile policy to use.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_PLATFORM
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mama_setLogFilePolicy(mamaLogFilePolicy policy);

/**
 * @brief Set append to existing log file
 *
 * @param[in] append Boolean value indicating whether to append to the logfile.
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_PLATFORM
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mama_setAppendToLogFile(int append);

/**
 * @brief Return status of loggingToFile.
 *
 * @return Boolean value indication whether logginToFile is set.
 */
MAMAExpDLL
extern int
mama_loggingToFile(void);

/**
 * @brief Set a callback for when the max log size is reached.
 *
 * @details  This can be used to override the default action which is to
 * wrap the file and continue logging at the beginning
 *
 * @param[in] logCallbacks The log size calback to be set
 *
 * @return mama_status return code can be one of:
 *              MAMA_STATUS_NULL_ARG
 *              MAMA_STATUS_PLATFORM
 *              MAMA_STATUS_OK
 */
MAMAExpDLL
extern mama_status
mama_setLogSizeCb(logSizeCbType logCallbacks);

/**
 * @brief Return string version of log level.
 *
 * @param[in] level The level to return as a string value.
 */
MAMAExpDLL
extern const char*
mama_logLevelToString(MamaLogLevel level);

/**
 * @brief Try to convert string to log level.
 *
 * @details Return non-zero for success, zero for failure.
 * The string comparison is case insensitive.
 *
 * @param[in] s The string representation of a log level.
 *
 * @param[out] level The MAMA log level equivalent.
 */
MAMAExpDLL
extern int
mama_tryStringToLogLevel(const char* s, MamaLogLevel* level);

/**
 * @brief Return string version of log policy.
 *
 * @param[in] level The MAMA logfile policy to convert into its string equi
valent.
 */
MAMAExpDLL
extern const char*
mama_logPolicyToString(mamaLogFilePolicy level);

/**
 * @brief Try to convert string to log policy.
 *
 * @details Return non-zero for success, zero for failure.
 * The string comparison is case insensitive.
 *
 * @param[in] s The string representation of a MAMA logfile policy.
 *
 * @param[out] policy The MAMA logfile policy string equivalent.
 */
MAMAExpDLL
extern int
mama_tryStringToLogPolicy(const char* s, mamaLogFilePolicy* policy);

/**
 * @brief Increase by one log level the verbosity of a MamaLogLevel variable.
 *
 * @details If the level is already at the maximum verbosity it will be unchanged
 * after calling the function, otherwise the level will be incremented.
 * Returns zero if level is not changed, or non-zero if it is changed
 * If an unrecognized level is passed, the function will return 
 * non-zero and the variable will be set to the minimum verbosity
 *
 * @param[in] level The MAMA Log level to increment.
 *
 * @return boolean value, 1 it the log level was incremented successfully.
 */
MAMAExpDLL
extern int
mama_logIncrementVerbosity(MamaLogLevel* level);

/**
 * @brief Decrease by one log level the verbosity of a MamaLogLevel variable.
 *
 * @details If the level is already at the minimum verbosity it will be unchanged
 * after calling the function, otherwise the level will be decremented.
 * Returns zero if level is not changed, or non-zero if it is changed
 * If an unrecognized level is passed, the function will return 
 * non-zero and the variable will be set to the maximum verbosity
 *
 * @param[in] level The MAMA Log level to increment.
 *
 * @return boolean value, 1 it the log level was decremented successfully.
 */
MAMAExpDLL
extern int
mama_logDecrementVerbosity(MamaLogLevel* level);

/**
 * @brief Force rolling the log file.
 * 
 * @return The status of the operation.
 */
MAMAExpDLL
extern mama_status
mama_logForceRollLogFiles(void);

/**
 * @brief Destroy memory held by the logging.
 */
void
mama_logDestroy(void);

/**
 * @brief Setup MAMA Logging.
 *
 * @details This will setup all the default values from the mama.properties
 file
 */
void
mama_loginit (void);

#ifdef __cplusplus
} /* MAMAExpDLL
extern "C" */
#endif

#endif /* MAMA_LOG_H__ */
