#include "Globals.hpp"
#include "ServerClient.hpp"

#include <iostream>
using namespace std;

bool Client::SetSocketBlockStatus(SOCKET_STATUS status)
{
	iMode = (unsigned long)status;

	iResult = ioctlsocket(ClientSocket, FIONBIO, &iMode);
	if (iResult != NO_ERROR)
	{
		EnterCriticalSection(&outputCritical);
		cout << "<Server> ioctlsocket() (in Client) failed with error: " << iResult << endl;
		LeaveCriticalSection(&outputCritical);

		iResult = 0;
		return false;
	}

	EnterCriticalSection(&outputCritical);
	cout << "<Server> Client::ListenSocket block status: ";

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