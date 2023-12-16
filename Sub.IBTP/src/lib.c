#define LIBIMPL
#include "lib.h"
#undef LIBIMPL

#include "logger/logger.h"

#include "net_h.h"

#include <stdio.h>

/*
* Local variables; need to provide functions to get them
*/
static WSADATA wsaData;
//End local variables

int lib_clear();

int lib_start(WORD sockLibVersion) {
	LOG_TRACEF("lib_start, %d", sockLibVersion);
	/*
	* Initialize Winsock and request 2.2 version
	*/
	int sockLibInitStatus = WSAStartup(sockLibVersion, &wsaData);
	/*
	* Check if initialization is succeed
	*/
	if (sockLibInitStatus != 0) {
		dw_error = (DWORD)sockLibInitStatus;
		get_msg_text(dw_error, &nc_error);
		LOG_ERRORF("WSAStartup failed with code %d.", sockLibInitStatus);
		LOG_ERRORF("%s", nc_error);
		LocalFree(nc_error);
		return STARTUP_FAIL;
	}
	/*
	* Verify that winsock version 2.2 is available
	*/
	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		LOG_ERRORF("Version 2.2 of Winsock is not available.");
		lib_clear();
		return VERSION_FAIL;
	}
	return 0;
}

int lib_clear() {
	LOG_TRACEF("lib_clear");
	return WSACleanup();
}