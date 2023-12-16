#ifndef CONFIG_VALIDATION_H
#define CONFIG_VALIDATION_H

#include "user_config.h"

#if !defined LOG_LEVEL
#define LOG_FILE LOG_LEVEL_WARN
#endif

#if !defined LOG_FILE
#define LOG_FILE "./logging.txt"
#endif

#if !defined LOGGING_FILE_MODE
#define LOGGING_FILE_MODE "at"
#endif

#if !defined NET_NAMESPACE
#define NET_NAMESPACE NET_NAMESPACE_DISABLED
#endif

#if !defined IBTP_NAMESPACE
#define IBTP_NAMESPACE IBTP_NAMESPACE_DISABLED
#endif

#endif