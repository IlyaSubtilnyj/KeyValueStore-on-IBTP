#define NETHIMPL
#include "net_h.h"
#undef NETHIMPL

char* nc_error;
DWORD dw_error;

void get_msg_text(DWORD dw_error, char** pnc_msg)
{
	DWORD dw_flags;
	/*                                                                            */
	/* Set message options:                                                       */
	/*                                                                            */
	dw_flags = FORMAT_MESSAGE_ALLOCATE_BUFFER
		| FORMAT_MESSAGE_FROM_SYSTEM
		| FORMAT_MESSAGE_IGNORE_INSERTS;
	/*                                                                            */
	/* Create the message string:                                                 */
	/*                                                                            */
	FormatMessage(dw_flags, NULL, dw_error, LANG_SYSTEM_DEFAULT, (LPTSTR)pnc_msg, 0, NULL);
}