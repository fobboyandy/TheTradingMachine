#include "PythonSocketIPC.h"

using namespace std;

PythonSocketIPC::PythonSocketIPC(int portIn)
{
	ConnectSocket = INVALID_SOCKET;
	result = NULL;
	ptr = NULL;
	port = portIn;
	validIPC = false;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
		return;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo("localhost", to_string(port).c_str(), &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET)
		{
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		//success
		if (iResult != SOCKET_ERROR)
			break;

		//look for the next address
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
	}

	validIPC = true;

}

PythonSocketIPC::~PythonSocketIPC()
{
	iResult = shutdown(ConnectSocket, SD_SEND);	
	closesocket(ConnectSocket);
	WSACleanup();
}

bool PythonSocketIPC::send(const char* buf, int len)
{
	iResult = ::send(ConnectSocket, buf, len, 0);
	if (iResult == SOCKET_ERROR)
		return false;
	return true;
}
