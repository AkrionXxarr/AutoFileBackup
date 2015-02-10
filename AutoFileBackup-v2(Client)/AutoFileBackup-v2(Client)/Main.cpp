#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Input / Output
#include <conio.h>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

// Windows
#include <windows.h>

// Win Sockets
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "Ws2_32.lib")

enum BLOCK_STATUS { BLOCKED, NON_BLOCKED };
void SetSocketBlockStatus(SOCKET& ConnectSocket, unsigned long iMode);

template <class T>
void Encryption(T message, char key);
//void EncryptMessage(string message);
//void EncryptMessage(char* message[]);

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	addrinfo *result = NULL;
	addrinfo *ptr = NULL;
	addrinfo hints;

	bool connected = false;

	const unsigned short SLEEP_TIME = 100;
	const unsigned short DEFAULT_BUFLEN = 1024;

	char key = '^';
	char recvbuf[DEFAULT_BUFLEN + 1];
	recvbuf[DEFAULT_BUFLEN] = '\0';

	string userCommand;

	string serverAddress;
	string serverPort;
	string serverPassword;

	SOCKET ConnectSocket;

	userCommand.resize(DEFAULT_BUFLEN);

	int iResult;

	if (argc == 3)
	{
		serverAddress = argv[1];
		serverPort = argv[2];
	}
	else if (argc == 4)
	{
		serverAddress = argv[1];
		serverPort = argv[2];
		serverPassword = argv[3];
	}
	else
	{
		cout << "Server address: "; getline(cin, serverAddress);
		cout << "Server port: "; getline(cin, serverPort);
		cout << "Server password: "; getline(cin, serverPassword);
	}

	if (serverPassword.size() == 0)
	{
		serverPassword = " ";
	}

	for (int i = 0; i < DEFAULT_BUFLEN; i++)
	{
		recvbuf[i] = '\0';
	}

	cout << "Connecting..." << endl;

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		cout << "WSAStartup failed with error: " << iResult << endl;
		cout << "Press <ENTER> to quit..." << endl;
		cin.ignore(INT_MAX, '\n');
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(serverAddress.c_str(), serverPort.c_str(), &hints, &result);
	if (iResult != 0)
	{
		cout << "getaddrinfo failed with error: " << iResult << endl;
		cout << "Press <ENTER> to quit..." << endl;
		cin.ignore(INT_MAX, '\n');
		WSACleanup();
		return 1;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (ConnectSocket == INVALID_SOCKET)
		{
			cout << "socket failed with error: " << WSAGetLastError();
			cout << "Press <ENTER> to quit..." << endl;
			cin.ignore(INT_MAX, '\n');
			WSACleanup();
			return 1;
		}

		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET)
	{
		cout << "Unable to connect to server." << endl;
		cout << "Press <ENTER> to quit..." << endl;
		cin.ignore(INT_MAX, '\n');
		WSACleanup();
		return 1;
	}

	Encryption<string&>(serverPassword, key);
	send(ConnectSocket, serverPassword.c_str(), serverPassword.size(), NULL);

	SetSocketBlockStatus(ConnectSocket, NON_BLOCKED);

	while (1)
	{
		Sleep(SLEEP_TIME);
		if (_kbhit())
		{
			getline(cin, userCommand);

			if (_stricmp(userCommand.c_str(), "exit-program") == 0)
			{
				closesocket(ConnectSocket);
				cout << "Exiting program..." << endl;
				break;
			}
			else if (_stricmp(userCommand.c_str(), "help") == 0)
			{
				cout << "  === Commands ===" << endl;
				cout << "> exit-program : Shuts the program down, ensures graceful disconnect" << endl;
				cout << "> help : Displays this list" << endl;
				cout << "> force : Force-saves a backup to a unique folder" << endl;
				cout << "> get-log : Requests the backup log from the server" << endl;
				cout << "> folder:restore : Lets you restore a backup" << endl;
				cout << "> status : Shows program status, including most recent backup" << endl;
				cout << "> start-server : Runs batch file to start the Minecraft server" << endl;
				cout << endl;
			}
			else if (_stricmp(userCommand.c_str(), "force") == 0)
			{
				Encryption<string&>(userCommand, key);
				send(ConnectSocket, userCommand.c_str(), userCommand.size(), NULL);
			}
			else if (_stricmp(userCommand.c_str(), "get-log") == 0)
			{
				Encryption<string&>(userCommand, key);
				send(ConnectSocket, userCommand.c_str(), userCommand.size(), NULL);

				ofstream backupLog("backup_log.txt", ios::trunc);

				SetSocketBlockStatus(ConnectSocket, BLOCKED);
				do
				{
					iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, NULL);
					if (iResult == 0 || WSAGetLastError() == WSAECONNRESET)
					{
						if (iResult == 0)
						{
							cout << "Connection with server closed" << endl;
						}
						else
						{
							cout << "Connection was reset by peer" << endl;
						}
						break;
					}
					else if (iResult < 0 && WSAGetLastError() != WSAEWOULDBLOCK)
					{
						cout << "recv() failed with error: " << WSAGetLastError() << endl;
						break;
					}
					else if (iResult > 0)
					{
						Encryption<char[]>(recvbuf, key);
						backupLog << recvbuf;

						for (int i = 0; i < DEFAULT_BUFLEN; i++)
						{
							recvbuf[i] = '\0';
						}
						SetSocketBlockStatus(ConnectSocket, NON_BLOCKED);
					}
				} while (iResult > 0);

				backupLog.close();

				cout << "Log recieved, check local directory" << endl << endl;

				continue;
			}
			else if (userCommand.find(":restore") != -1)
			{
				Encryption<string&>(userCommand, key);
				send(ConnectSocket, userCommand.c_str(), userCommand.size(), NULL);
			}
			else if (_stricmp(userCommand.c_str(), "status") == 0)
			{
				Encryption<string&>(userCommand, key);
				send(ConnectSocket, userCommand.c_str(), userCommand.size(), NULL);
			}
			else if (_stricmp(userCommand.c_str(), "start-server") == 0)
			{
				Encryption<string&>(userCommand, key);
				send(ConnectSocket, userCommand.c_str(), userCommand.size(), NULL);
			}
			else
			{
				cout << "<UserInput> Error: unknown command <" << userCommand << ">" << endl;
				cout << "See 'help' for a list of commands" << endl;
			}
		}

		iResult = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, NULL);
		if (iResult == 0 || WSAGetLastError() == WSAECONNRESET)
		{
			if (iResult == 0)
			{
				cout << "Connection with server closed" << endl;
			}
			else
			{
				cout << "Connection was reset by peer" << endl;
			}
			break;
		}
		else if (iResult < 0 && WSAGetLastError() != WSAEWOULDBLOCK)
		{
			cout << "recv() failed with error: " << WSAGetLastError() << endl;
			break;
		}
		else if (iResult > 0)
		{
			Encryption<char[]>(recvbuf, key);
			cout << recvbuf << endl;

			for (int i = 0; i < DEFAULT_BUFLEN; i++)
			{
				recvbuf[i] = '\0';
			}
		}
	}

	cout << "Press <ENTER> to quit..." << endl;
	cin.ignore(INT_MAX, '\n');
	
	return 0;
}

void SetSocketBlockStatus(SOCKET& ConnectSocket, unsigned long iMode)
{
	int iResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
	if (iResult != NO_ERROR)
	{
		cout << "ioctlsocket() failed with error: " << iResult << endl;

		iResult = 0;
	}
}

template <class T>
void Encryption(T message, char key)
{
	int i = 0;
	while (message[i] != '\0')
	{
		message[i] = message[i] ^ ((int)key + 1) % 254 + 1;
		i++;
	}
}