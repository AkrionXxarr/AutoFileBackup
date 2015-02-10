#include "Globals.hpp"
#include "ServerClient.hpp"

#include <iostream>
using namespace std;

bool Server::Initialize()
{
	addrinfo *result = NULL;
	addrinfo hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		EnterCriticalSection(&outputCritical);
		cout << "<Server> WSAStartup() failed with error: " << iResult << endl;
		LeaveCriticalSection(&outputCritical);

		iResult = 0;
		return false;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	ListenSocket = INVALID_SOCKET;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, port.c_str(), &hints, &result);
	if (iResult != 0)
	{
		EnterCriticalSection(&outputCritical);
		cout << "<Server> getaddrinfo() failed with error: " << iResult << endl;
		LeaveCriticalSection(&outputCritical);
		WSACleanup();

		iResult = 0;
		return false;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET)
	{
		EnterCriticalSection(&outputCritical);
		cout << "<Server> socket() failed with error: " << WSAGetLastError() << endl;
		LeaveCriticalSection(&outputCritical);
		freeaddrinfo(result);
		WSACleanup();

		iResult = 0;
		return false;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR)
	{
		EnterCriticalSection(&outputCritical);
		cout << "<Server> bind() failed with error: " << WSAGetLastError() << endl;
		LeaveCriticalSection(&outputCritical);
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();

		iResult = 0;
		return false;
	}

	EnterCriticalSection(&outputCritical);
	cout << "<Server> Server::ListenSocket initialized" << endl;
	LeaveCriticalSection(&outputCritical);

	iResult = 0;
	return true;
}

bool Server::AcceptClient(SOCKET &ClientSocket)
{
	ClientSocket = accept(ListenSocket, NULL, NULL);

	if (ClientSocket == INVALID_SOCKET)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			return false;
		}
		else
		{
			EnterCriticalSection(&outputCritical);
			cout << "<Server> accept() failed with error: " << WSAGetLastError() << endl;
			LeaveCriticalSection(&outputCritical);
			closesocket(ListenSocket);
			return false;
		}
	}
	EnterCriticalSection(&outputCritical);
	cout << "<Server> Client connected.." << endl;
	LeaveCriticalSection(&outputCritical);

	return true;
}

bool Server::SetSocketBlockStatus(SOCKET_STATUS status)
{
	iMode = (unsigned long)status;

	iResult = ioctlsocket(ListenSocket, FIONBIO, &iMode);
	if (iResult != NO_ERROR)
	{
		EnterCriticalSection(&outputCritical);
		cout << "<Server> ioctlsocket() (in Server) failed with error: " << iResult << endl;
		LeaveCriticalSection(&outputCritical);

		iResult = 0;
		return false;
	}

	EnterCriticalSection(&outputCritical);
	cout << "<Server> Server::ListenSocket block status: ";

	switch (status)
	{
	case BLOCKED:
		cout << "Blocked" << endl;
		break;
	case NON_BLOCKED:
		cout << "Non-Blocked" << endl;
		break;
	}
	LeaveCriticalSection(&outputCritical);
	
	iResult = 0;
	return true;
}

bool Server::SetListenState(LISTEN_STATE state)
{
	switch (state)
	{
	case OPEN_STATE:
		// Set ListenSocket into listen state
		iResult = listen(ListenSocket, 0);
		if (iResult == SOCKET_ERROR)
		{
			EnterCriticalSection(&outputCritical);
			cout << "<Server> listen() failed with error: " << WSAGetLastError() << endl;
			LeaveCriticalSection(&outputCritical);
			closesocket(ListenSocket);
			WSACleanup();

			iResult = 0;
			return false;
		}
		EnterCriticalSection(&outputCritical);
		cout << "<Server> Server::ListenSocket listen state: Open" << endl;
		LeaveCriticalSection(&outputCritical);
		break;

	case CLOSED_STATE:
		iResult = closesocket(ListenSocket);
		if (iResult == SOCKET_ERROR)
		{
			EnterCriticalSection(&outputCritical);
			cout << "<Server> closesocket() failed with error: " << WSAGetLastError() << endl;
			LeaveCriticalSection(&outputCritical);
			closesocket(ListenSocket);
			WSACleanup();

			iResult = 0;
			return false;
		}
		EnterCriticalSection(&outputCritical);
		cout << "<Server> Server::ListenSocket listen state: Closed" << endl;
		LeaveCriticalSection(&outputCritical);
		break;
	}

	return true;
}