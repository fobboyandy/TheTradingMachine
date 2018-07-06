#pragma once

#ifndef python_socket_ipc_h
#define python_socket_ipc_h

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#define DEFAULT_PORT 27015

using namespace std;

class PythonSocketIPC
{

public:
	PythonSocketIPC(int port);
	~PythonSocketIPC();

	bool send(const char* buf, int len);
	bool send(double val);

private:

	WSADATA wsaData;
	SOCKET ConnectSocket;
	struct addrinfo *result;
	struct addrinfo *ptr;
	struct addrinfo hints;
	int iResult;
	int port;
	bool validIPC;
};

#endif