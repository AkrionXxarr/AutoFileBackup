#include "Globals.hpp"
#include "ServerClient.hpp"
#include "Config.hpp"

#include <fstream>
#include <iostream>
using namespace std;

template <class T>
void Encryption(T message, char key);

unsigned int __stdcall ServerMain(void* baseConfig)
{
	ConfigInfo* config = (ConfigInfo*)baseConfig;
	EnterCriticalSection(&outputCritical);
	cout << "<Server> thread initialized" << endl;
	LeaveCriticalSection(&outputCritical);

	// variables //
	const unsigned short SLEEP_TIME = 500;
	const unsigned short DEFAULT_BUFLEN = 1024;

	bool allowed;
	bool clientConnected;

	char key = '^';
	char recvbuf[DEFAULT_BUFLEN + 1];
	recvbuf[DEFAULT_BUFLEN] = '\0';

	int iResult = 0;
	int fileSize = 0;

	string password;
	string message;

	Server server(config->serverPort);
	Client client;
	// ========= //

	for (int i = 0; i < DEFAULT_BUFLEN; i++)
	{
		recvbuf[i] = '\0';
	}

	while (!gQuitProgram)
	{
		iResult = 0;
		allowed = false;

		server.Initialize();
		server.SetSocketBlockStatus(NON_BLOCKED);
		server.SetListenState(OPEN_STATE);
		while (!server.AcceptClient(client.GetSocketRef()) && !gQuitProgram)
		{
			Sleep(SLEEP_TIME);
		}

		server.SetListenState(CLOSED_STATE);

		if (!gQuitProgram)
		{
			clientConnected = true;
			client.SetSocketBlockStatus(NON_BLOCKED);

			EnterCriticalSection(&outputCritical);
			cout << "<Server> Checking password..." << endl;
			LeaveCriticalSection(&outputCritical);
			for (int i = 0; i < 20; i++)
			{
				Sleep(SLEEP_TIME);

				iResult = recv(client.GetSocketRef(), recvbuf, DEFAULT_BUFLEN, 0);
				if (iResult == 0 || WSAGetLastError() == WSAECONNRESET)
				{
					if (iResult == 0)
					{
						EnterCriticalSection(&outputCritical);
						cout << "<Server> Connection with client closed" << endl;
						LeaveCriticalSection(&outputCritical);
					}
					else
					{
						EnterCriticalSection(&outputCritical);
						cout << "<Server> Connection was reset by peer" << endl;
						LeaveCriticalSection(&outputCritical);
					}
					clientConnected = false;
					break;
				}
				else if (iResult < 0 && WSAGetLastError() != WSAEWOULDBLOCK)
				{
					EnterCriticalSection(&outputCritical);
					cout << "<Server> recv() failed with error: " << WSAGetLastError() << endl;
					LeaveCriticalSection(&outputCritical);
					break;
				}
				else if (iResult > 0)
				{
					Encryption<char[]>(recvbuf, key);
					password = recvbuf;

					for (int i = 0; i < DEFAULT_BUFLEN; i++)
					{
						recvbuf[i] = '\0';
					}
					break;
				}
			}

			if (strcmp(config->password.c_str(), password.c_str()) == 0)
			{
				EnterCriticalSection(&outputCritical);
				cout << "<Server> Password check passed." << endl;
				LeaveCriticalSection(&outputCritical);

				allowed = true;
			}
			else
			{
				EnterCriticalSection(&outputCritical);
				cout << "<Server> Password check failed." << endl;
				LeaveCriticalSection(&outputCritical);

				allowed = false;
			}
		}
		else
		{
			clientConnected = false;
		}

		if (allowed)
		{
			gKillClient = false;
			gClientSend.push("Connected\n");
			while (clientConnected && !gQuitProgram && !gKillClient)
			{
				do
				{
					iResult = recv(client.GetSocketRef(), recvbuf, DEFAULT_BUFLEN, 0);

					if (iResult == 0 || WSAGetLastError() == WSAECONNRESET)
					{
						if (iResult == 0)
						{
							EnterCriticalSection(&outputCritical);
							cout << "<Server> Connection with client closed" << endl;
							LeaveCriticalSection(&outputCritical);
						}
						else
						{
							EnterCriticalSection(&outputCritical);
							cout << "<Server> Connection was reset by peer" << endl;
							LeaveCriticalSection(&outputCritical);
						}
						clientConnected = false;
						break;
					}
					else if (iResult < 0 && WSAGetLastError() != WSAEWOULDBLOCK)
					{
						EnterCriticalSection(&outputCritical);
						cout << "<Server> recv() failed with error: " << WSAGetLastError() << endl;
						LeaveCriticalSection(&outputCritical);
						break;
					}
					else if (iResult > 0)
					{
						Encryption<char[]>(recvbuf, key);
						gClientRecv.push(recvbuf);
						for (int i = 0; i < DEFAULT_BUFLEN; i++)
						{
							recvbuf[i] = '\0';
						}
					}
				} while (iResult > 0);

				iResult = 0;

				if (gSendLog)
				{
					client.SetSocketBlockStatus(BLOCKED);
					
					EnterCriticalSection(&outputCritical);
					cout << "<Server> Sending backup log to client" << endl;
					LeaveCriticalSection(&outputCritical);

					ifstream backupLog("backup_log.txt");
					
					backupLog.seekg(0, ios::end);
					fileSize = backupLog.tellg();
					backupLog.seekg(0, ios::beg);

					char *buffer = new char[fileSize];
					backupLog.read(buffer, fileSize);
					Encryption<char[]>(buffer, key);

					iResult = send(client.GetSocketRef(), buffer, fileSize, NULL);
					if (iResult == SOCKET_ERROR)
					{
						EnterCriticalSection(&outputCritical);
						cout << "<Server> send() failed with error: " << WSAGetLastError() << endl;
						LeaveCriticalSection(&outputCritical);
					}

					backupLog.close();
					client.SetSocketBlockStatus(NON_BLOCKED);

					delete[] buffer;
					buffer = NULL;
					gSendLog = false;
				}

				iResult = 0;
			
				while (!gClientSend.empty())
				{
					message = gClientSend.front();
					Encryption<string&>(message, key);
					iResult = send(client.GetSocketRef(), message.c_str(), message.size(), NULL);
					if (iResult == SOCKET_ERROR)
					{
						EnterCriticalSection(&outputCritical);
						cout << "<Server> send() failed with error: " << WSAGetLastError() << endl;
						LeaveCriticalSection(&outputCritical);
					}

					gClientSend.pop();
				}

				Sleep(SLEEP_TIME);
			}
		}

		if (!client.InvalidSocket())
		{
			iResult = shutdown(client.GetSocketRef(), SD_SEND);
			if (iResult == SOCKET_ERROR)
			{
				cout << "<Server> shutdown() failed with error: " << WSAGetLastError() << endl;
			}
		}
	}

	

	return 0;
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