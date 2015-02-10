#include "Globals.hpp"
#include "UserInput.hpp"

#include <Shellapi.h>
#include <iostream>
#include <conio.h>
using namespace std;

unsigned int __stdcall UserInputMain(void* params)
{
	EnterCriticalSection(&outputCritical);
	cout << "<UserInput> thread initialized" << endl;
	LeaveCriticalSection(&outputCritical);

	const unsigned short SLEEP_TIME = 100;

	string userCommand;

	while(!gQuitProgram)
	{
		Sleep(SLEEP_TIME);
		if (_kbhit())
		{
			getline(cin, userCommand);

			if (_stricmp(userCommand.c_str(), "exit-program") == 0)
			{
				EnterCriticalSection(&outputCritical);
				cout << "Exiting program..." << endl;
				LeaveCriticalSection(&outputCritical);

				gQuitProgram = true;
			}
			else if (_stricmp(userCommand.c_str(), "help") == 0)
			{
				EnterCriticalSection(&outputCritical);
				cout << "  === Commands ===" << endl;
				cout << "> exit-program : Shuts the program down" << endl;
				cout << "> help : Displays this list" << endl;
				cout << "> force : Force-saves a backup to a unique folder" << endl;
				cout << "> folder:restore : Lets you restore a backup" << endl;
				cout << "> kill-client : Forces the connection between client and server to close" << endl;
				cout << "> status : Shows program status, including most recent backup" << endl;
				cout << "> start-server : Runs batch file to start the Minecraft server" << endl;
				LeaveCriticalSection(&outputCritical);
			}
			else if (_stricmp(userCommand.c_str(), "force") == 0)
			{
				gLocalCommands.push(userCommand);
			}
			else if (userCommand.find(":restore") != -1)
			{
				gLocalCommands.push(userCommand);
			}
			else if (_stricmp(userCommand.c_str(), "kill-client") == 0)
			{
				gKillClient = true;
			}
			else if (_stricmp(userCommand.c_str(), "status") == 0)
			{
				gLocalCommands.push(userCommand);
			}
			else if (_stricmp(userCommand.c_str(), "start-server") == 0)
			{
				gLocalCommands.push(userCommand);
			}
			else
			{
				EnterCriticalSection(&outputCritical);
				cout << "<UserInput> Error: unknown command <" << userCommand << ">" << endl;
				cout << "See 'help' for a list of commands" << endl;
				LeaveCriticalSection(&outputCritical);
			}
		}
	}
	return 0;
}