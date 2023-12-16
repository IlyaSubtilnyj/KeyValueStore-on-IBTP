#ifndef NETH_H
#define NETH_H

//prevent version problems
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

/*
* network stuff
*/
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)
//end network stuff

enum winsock
{
	STARTUP_FAIL = 1,
	VERSION_FAIL
};

extern char* nc_error;
extern DWORD dw_error;

void get_msg_text(DWORD dw_error/*in-Error code*/, char** pnc_msg /*out-Error message*/);

#endif // !NETH_H
