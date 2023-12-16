#ifndef USERCONFIG_H
#define USERCONFIG_H
#include "config.h"

/*
	short description:...
	LOGGING_ENABLED
	LOGGING_DISABLED
*/
#define LOGGING LOGGING_ENABLED

/*
	short description:...
	LOGGING LEVEL
	Default value: LOG_LEVEL_TRACE
*/
#define LOG_LEVEL LOG_LEVEL_TRACE

/*
	short description:...

*/
#define LOG_USE_STDOUT 1


/*
	short description:...

*/
#define LOG_USE_FILE 1


/*
	short description:...
	file to which logging messages will ne wrote
	Default Value: "./logging.txt"
*/
//#define LOG_FILE "./logging.txt"

/*
	short description:...
	file writing mode
	Default Value: "at"
*/
//#define LOGGING_FILE_MODE "at"


/*
	short description:...

*/
#define LOG_USE_STRUCTURED 1

//[NETNAMESPACE]
/*
	short description:...
	to enable namespacing set FOOBAR_NAMESPACE to FOOBAR_NAMESPACE_ENABLED reserved value
	to disable namespacing set FOOBAR_NAMESPACE to FOOBAR_NAMESPACE_DISABLED reservedd value
	Default value: FOOBAR_NAMESPACE_DISABLED
*/
#define NET_NAMESPACE NET_NAMESPACE_ENABLED

//[UTILSNAMESPACE]
/*
	short description:...
	to enable namespacing set FOOBAR_NAMESPACE to FOOBAR_NAMESPACE_ENABLED reserved value
	to disable namespacing set FOOBAR_NAMESPACE to FOOBAR_NAMESPACE_DISABLED reservedd value
	Default value: FOOBAR_NAMESPACE_DISABLED
*/
#define UTILS_NAMESPACE UTILS_NAMESPACE_ENABLED

//[IBTPNAMESPACE]
/*
	short description:...
	to enable namespacing set FOOBAR_NAMESPACE to FOOBAR_NAMESPACE_ENABLED reserved value
	to disable namespacing set FOOBAR_NAMESPACE to FOOBAR_NAMESPACE_DISABLED reservedd value
	Default value: FOOBAR_NAMESPACE_DISABLED
*/
#define IBTP_NAMESPACE IBTP_NAMESPACE_ENABLED

#endif