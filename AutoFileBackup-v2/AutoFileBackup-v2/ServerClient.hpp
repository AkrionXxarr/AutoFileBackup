#ifndef _SERVER_CLIENT_HPP_
#define _SERVER_CLIENT_HPP_

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "Ws2_32.lib")
#include <string>
using namespace std;

unsigned int __stdcall ServerMain(void* portNum);

enum SOCKET_STATUS { BLOCKED, NON_BLOCKED };
enum LISTEN_STATE { OPEN_STATE, CLOSED_STATE };
class Server
{
public:
	Server(string serverPort) : port(serverPort),
								ListenSocket(NULL), 
								result(NULL),
								iMode(BLOCKED),
								iResult(0) { }

	bool Initialize();
	bool AcceptClient(SOCKET &ClientSocket);

	bool SetSocketBlockStatus(SOCKET_STATUS status); 
	bool SetListenState(LISTEN_STATE state);

private:
	string port;
	SOCKET ListenSocket;
	unsigned long iMode;
	int iResult;

	WSADATA wsaData;
	addrinfo *result;
	addrinfo hints;
};

class Client
{
public:
	Client() : isConnected(false),
			   ClientSocket(NULL),
			   iMode(BLOCKED),
			   iResult(0) { }

	SOCKET& GetSocketRef() { return ClientSocket; }
	SOCKET InvalidSocket() 
	{
		if (ClientSocket == INVALID_SOCKET)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	bool SetSocketBlockStatus(SOCKET_STATUS status);

protected:
	bool isConnected;
	SOCKET ClientSocket;
	unsigned long iMode;
	int iResult;
};
#endif