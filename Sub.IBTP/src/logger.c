#define LOGGERIMPL
#include "logger.h"
#undef LOGGERIMPL

void _log(FILE* file, const char* fmt, ...) {
	va_list ap;
	FILE* stream = NULL; //depends on config file
	errno_t err;
	if (file == NULL) {
		if ((err = fopen_s(&stream, "SubtilniyDB.log", "at")) != 0)
			assert(0 && "The file for logging is not opened");
		file = stream;
	}
	va_start(ap, fmt);
	vfprintf(file, fmt, ap);
	va_end(ap);
	if (stream) 
	{
		if (fclose(stream)) 
		{
			assert(0 && "The file for logging cannot be closed");
			file = NULL;
		}
	}		
}
