#ifndef LOGGER_H
#define LOGGER_H

#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include "../vconfig.h"

//https://www.frogtoss.com/labs/low-overhead-structured-logging-in-c.html
//https://tuttlem.github.io/2012/12/08/simple-logging-in-c.html
//https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html

void _log(FILE* file, const char* fmt, ...);

#if !defined(LOG_LEVEL)
#    define LOG_LEVEL LOG_LEVEL_TRACE
#endif

#if LOGGING == LOGGING_DISABLED
#define LOG_LEVEL 1000
#endif

#define LOG__FLAT(file, line, level, msg) \
    level " tm(" __TIME__ ") " file ":fn(" __FUNCTION__ "):" line " | " msg "\n"

#define LOG__STRUCTURED(file, line, level, msg) \
    level           LOG__EOL \
    "\tfile: " file LOG__EOL \
    "\tline: " line LOG__EOL \
    "\tfunc: " __FUNCTION__ LOG__EOL \
    "\ttime: " __TIME__ LOG__EOL \
    "\tcomment: "  msg  LOG__EOM "\n"

// implementation
#if LOG_USE_STDOUT
#  define LOG__STDIO_FPRINTF(stream, fmt, ...) \
      _log(stream, fmt, __VA_ARGS__);
#else
#  define LOG__STDIO_FPRINTF(stream, fmt, ...)
#endif

#if LOG_USE_FILE
#  define LOG__FILE(fmt, ...)          \
      _log(NULL, fmt, __VA_ARGS__);
#else
#  define LOG__FILE(fmt, ...)
#endif


#if LOG_USE_STRUCTURED
#  define LOG__LINE(file, line, level, msg) \
      LOG__STRUCTURED(file, line, level, msg)
#else
#  define LOG__LINE(file, line, level, msg) \
      LOG__FLAT(file, line, level, msg)
#endif


#define LOG__XSTR(x) #x
#define LOG__STR(x) LOG__XSTR(x)

#define LOG__DECL_LOGLEVELF(TColor, T, fmt, ...)           \
{                                                  \
    LOG__STDIO_FPRINTF(stdout,                     \
        LOG__LINE(__FILE__, LOG__STR(__LINE__), TColor T, fmt "\x1b[39m"), \
            __VA_ARGS__);                          \
                                                   \
    LOG__FILE(  \
            LOG__LINE(__FILE__, LOG__STR(__LINE__), #T, #fmt), \
            __VA_ARGS__);                          \
}


#if LOG_LEVEL == LOG_LEVEL_TRACE
#  define LOG_TRACEF(fmt, ...) LOG__DECL_LOGLEVELF("\x1b[36m", "TRC", fmt, __VA_ARGS__);
#else
#  define LOG_TRACEF(fmt, ...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_WARN
#  define LOG_WARNF(fmt, ...) LOG__DECL_LOGLEVELF("\x1b[33m", "WRN", fmt, __VA_ARGS__);
#else
#  define LOG_WARNF(fmt, ...)
#endif

#if LOG_LEVEL <= LOG_LEVEL_ERROR
#  define LOG_ERRORF(fmt, ...) LOG__DECL_LOGLEVELF("\x1b[31m", "ERR", fmt, __VA_ARGS__);
#else
#  define LOG_ERRORF(fmt, ...)
#endif

/*
* for c99 standart
#define logf99(f, message) __log(f, message)
#define log99(message) __log(NULL, message)
*/

#endif