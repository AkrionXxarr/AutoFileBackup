#include "Globals.hpp"
#include "Config.hpp"
#include "ServerClient.hpp"
#include "FileOperations.hpp"
#include "UserInput.hpp"

#include <process.h>

#include <fstream>
#include <iostream>
using namespace std;

CRITICAL_SECTION outputCritical;
bool gSendLog = false;
bool gQuitProgram = false;
bool gKillClient = false;

queue<string> gLocalCommands;
queue<string> gClientRecv;
queue<string> gClientSend;

int main(int argc, char* argv[])
{
	// variables //
	HANDLE myHandles[3];
	// ========= //

	bool isOnlineMode;
	ConfigInfo baseConfig;
	ifstream configFile(CONFIGFILE);

	if (!configFile.is_open())
	{
		configFile.close();
		cout << "Error: " << CONFIGFILE << " not found.." << endl;
		if (!NewConfig())
		{
			cout << endl << "Press <Enter> to quit..." << endl;
			cin.ignore(INT_MAX, '\n');

			return 1;
		}

		cout << CONFIGFILE << " created, it can be found in this program's local directory," << endl
			 << "please fill it out and restart." << endl;

		cout << endl << "Press <ENTER> to quit..." << endl;
		cin.ignore(INT_MAX, '\n');

		return 0;
	}
	else
	{
		configFile.close();

		if (!LoadConfig(baseConfig))
		{
			cout << endl << "Press <Enter> to quit..." << endl;
			cin.ignore(INT_MAX, '\n');

			return 1;
		}
	}

	if (_stricmp(baseConfig.onlineMode.c_str(), "on") == 0)
	{
		cout << "Online Mode is on" << endl;
		isOnlineMode = true;
	}
	else if (_stricmp(baseConfig.onlineMode.c_str(), "off") == 0)
	{
		cout << "Online Mode is off" << endl;
		isOnlineMode = false;
	}
	else
	{
		cout << "Error: Invalid parameter for Online Mode, check config." << endl;
		cin.ignore(INT_MAX, '\n');

		return 1;
	}

	cout << "Threads initializing in 2 seconds" << endl;
	Sleep(2000);

	InitializeCriticalSection(&outputCritical);

	if (isOnlineMode)
	{
		myHandles[SERVER_THREAD] = (HANDLE)_beginthreadex(0, 0, &ServerMain, (void*)&baseConfig, 0, 0);
	}
	myHandles[FILEOPS_THREAD] = (HANDLE)_beginthreadex(0, 0, &FileOpMain, (void*)&baseConfig, 0, 0);
	myHandles[USERINPUT_THREAD] = (HANDLE)_beginthreadex(0, 0, &UserInputMain, NULL, 0, 0);


	if (isOnlineMode)
	{
		WaitForMultipleObjects(3, myHandles, TRUE, INFINITE);
	}
	else
	{
		WaitForSingleObject(myHandles[FILEOPS_THREAD], INFINITE);
		WaitForSingleObject(myHandles[USERINPUT_THREAD], INFINITE);
	}

	DeleteCriticalSection(&outputCritical);

	CloseHandle(myHandles[SERVER_THREAD]);
	CloseHandle(myHandles[FILEOPS_THREAD]);
	CloseHandle(myHandles[USERINPUT_THREAD]);

	return 0;
}