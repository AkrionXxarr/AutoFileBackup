#include "Globals.hpp"
#include "Config.hpp"
#include "FileOperations.hpp"

#include <Shellapi.h>

#include <time.h>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

/////////////////////////////////
// Main File Operations thread //
/////////////////////////////////
unsigned int __stdcall FileOpMain(void* configParams)
{
	EnterCriticalSection(&outputCritical);
	cout << "<FileOp> thread initialized" << endl;
	LeaveCriticalSection(&outputCritical);

	// variables //
	ConfigInfo* baseConfig = (ConfigInfo*) configParams;
	ConfigInfo configInfo = *baseConfig;

	bool print = true;
	bool hold = false;
	bool forceBackup = false;
	bool firstIteration = true;

	const unsigned short SLEEP_TIME = 100;

	unsigned short backupCount = configInfo.backupSwitch;
	unsigned short backupErrorCount = 0;
	unsigned short lastBackup = 0;

	HINSTANCE hInst;

	time_t tBaseLine;
	time_t seconds;

	struct tm timeinfo;
	char baseTimeStr[120];
	char timeStr[120];

	string userCommand;
	stringstream clientReturn;
	// ========= //

	unsigned short interval = configInfo.backupInterval * 60;
	tBaseLine = time(NULL);

	EnterCriticalSection(&outputCritical);
	time(&tBaseLine);
	localtime_s(&timeinfo, &tBaseLine);
	strftime(baseTimeStr, 120, "%B %d, %Y, %I:%M %p", &timeinfo);

	cout << "<FileOps> Backup loop entry time: " << baseTimeStr << endl;
	LeaveCriticalSection(&outputCritical);

	ofstream backupLog(BACKUPLOG, ios::trunc);
	if (backupLog.fail())
	{
		backupLog.clear();
		EnterCriticalSection(&outputCritical);
		cout << "<FileOps> Error: backupLog.fail() triggered..." << endl;
		LeaveCriticalSection(&outputCritical);
	}
	else if (backupLog.is_open())
	{
		backupLog << "> Backup loop entry time: " << baseTimeStr << endl;
	}
	else
	{
		EnterCriticalSection(&outputCritical);
		cout << "<FileOps> Error: " << BACKUPLOG << " could not be opened." << endl;
		LeaveCriticalSection(&outputCritical);
	}
	backupLog.close();

	// Main backup loop //
	while (!gQuitProgram)
	{
		Sleep(SLEEP_TIME);
		seconds = time(NULL) - tBaseLine;

		if ((seconds % (configInfo.backupInterval * 60) == 0 || firstIteration) && print)
		{
			print = false;
			hold = true;

			if (backupCount > configInfo.numOfBackups)
			{
				backupCount = 1;
			}
			else if (backupCount < 1)
			{
				backupCount = 1;
			}

			if (firstIteration)
			{
				EnterCriticalSection(&outputCritical);
				cout << "<FileOps> First iteration - Making initial backup" << endl;
				LeaveCriticalSection(&outputCritical);
				firstIteration = false;
			}

			EnterCriticalSection(&outputCritical);
			cout << "<FileOps> Attemping backup copy to directory: " << configInfo.backupDirName << backupCount << "..." << endl;
			LeaveCriticalSection(&outputCritical);
			if (!Backup(configInfo.mapSourcePath, configInfo.mapDestPath, configInfo.backupDirName, backupCount))
			{
				EnterCriticalSection(&outputCritical);
				time(&seconds);
				localtime_s(&timeinfo, &seconds);
				strftime(timeStr, 120, "%B %d, %Y, %I:%M %p", &timeinfo);

				cout << "<FileOps> Error Time: " << timeStr << endl;
				LeaveCriticalSection(&outputCritical);

				ofstream backupLog(BACKUPLOG, ios::app);
				if (backupLog.fail())
				{
					backupLog.clear();
					EnterCriticalSection(&outputCritical);
					cout << "<FileOps> Error: backupLog.fail() triggered..." << endl;
					LeaveCriticalSection(&outputCritical);
				}
				else if (backupLog.is_open())
				{
					backupLog << "> Error Time: " << timeStr << endl;
				}
				else
				{
					EnterCriticalSection(&outputCritical);
					cout << "<FileOps> Error: " << BACKUPLOG << " could not be opened." << endl;
					LeaveCriticalSection(&outputCritical);
				}
				backupLog.close();

				backupErrorCount++;
			}
			else
			{
				EnterCriticalSection(&outputCritical);
				time(&seconds);
				localtime_s(&timeinfo, &seconds);
				strftime(timeStr, 120, "%B %d, %Y, %I:%M %p", &timeinfo);

				cout << "<FileOps> Last successful backup at: " << timeStr << endl;
				cout << "          In folder: " << configInfo.backupDirName << backupCount << endl; 
				LeaveCriticalSection(&outputCritical);

				ofstream backupLog(BACKUPLOG, ios::app);
				if (backupLog.fail())
				{
					backupLog.clear();
					EnterCriticalSection(&outputCritical);
					cout << "<FileOps> Error: backupLog.fail() triggered..." << endl;
					LeaveCriticalSection(&outputCritical);
				}
				else if (backupLog.is_open())
				{
					backupLog << "> Last successful backup at: " << timeStr << endl;
					backupLog << "  In folder: " << configInfo.backupDirName << backupCount << endl;
				}
				else
				{
					EnterCriticalSection(&outputCritical);
					cout << "<FileOps> Error: " << BACKUPLOG << " could not be opened." << endl;
					LeaveCriticalSection(&outputCritical);
				}
				backupLog.close();

				lastBackup = backupCount;
				forceBackup = false;
				backupCount++;

				configInfo.backupSwitch = backupCount;
				SaveConfig(configInfo);
			}
		}
		else if (seconds % (configInfo.backupInterval * 60) != 0 && hold)
		{
			print = true;
			hold = false;
		}

		if (!gLocalCommands.empty())
		{
			userCommand = gLocalCommands.front();

			// Force backup (Local) //
			if (_stricmp(userCommand.c_str(), "force") == 0)
			{
				EnterCriticalSection(&outputCritical);
				cout << "<FileOps> Attemping backup copy to directory: ForceBackup... " << endl;
				LeaveCriticalSection(&outputCritical);
				if (!ForceBackup(configInfo.mapSourcePath, configInfo.mapDestPath))
				{
					EnterCriticalSection(&outputCritical);
					time(&seconds);
					localtime_s(&timeinfo, &seconds);
					strftime(timeStr, 120, "%B %d, %Y, %I:%M %p", &timeinfo);

					cout << "<FileOps> Error Time: " << timeStr << endl;
					LeaveCriticalSection(&outputCritical);

					ofstream backupLog(BACKUPLOG, ios::app);
					if (backupLog.fail())
					{
						backupLog.clear();
						EnterCriticalSection(&outputCritical);
						cout << "<FileOps> Error: backupLog.fail() triggered..." << endl;
						LeaveCriticalSection(&outputCritical);
					}
					else if (backupLog.is_open())
					{
						backupLog << "> Error Time: " << timeStr << endl;
					}
					else
					{
						EnterCriticalSection(&outputCritical);
						cout << "<FileOps> Error: " << BACKUPLOG << " could not be opened." << endl;
						LeaveCriticalSection(&outputCritical);
					}
					backupLog.close();

					backupErrorCount++;
				}
				else
				{
					EnterCriticalSection(&outputCritical);
					time(&seconds);
					localtime_s(&timeinfo, &seconds);
					strftime(timeStr, 120, "%B %d, %Y, %I:%M %p", &timeinfo);

					cout << "<FileOps> Last successful backup at: " << timeStr << endl;
					cout << "          In folder: ForceBackup" << endl; 
					LeaveCriticalSection(&outputCritical);

					ofstream backupLog(BACKUPLOG, ios::app);
					if (backupLog.fail())
					{
						backupLog.clear();
						EnterCriticalSection(&outputCritical);
						cout << "<FileOps> Error: backupLog.fail() triggered..." << endl;
						LeaveCriticalSection(&outputCritical);
					}
					else if (backupLog.is_open())
					{
						backupLog << "> Last successful backup at: " << timeStr << endl;
						backupLog << "  In folder: ForceBackup" << endl;
					}
					else
					{
						EnterCriticalSection(&outputCritical);
						cout << "<FileOps> Error: " << BACKUPLOG << " could not be opened." << endl;
						LeaveCriticalSection(&outputCritical);
					}
					backupLog.close();

					forceBackup = true;
				}
			}
			// Restore backup (local) //
			else if (userCommand.find(":restore") != -1)
			{
				EnterCriticalSection(&outputCritical);
				cout << "<FileOps> Attemping to restore from a backup..." << endl;
				LeaveCriticalSection(&outputCritical);
				if (!RestoreBackup(configInfo.mapDestPath, configInfo.mapSourcePath, userCommand, configInfo))
				{
					EnterCriticalSection(&outputCritical);
					time(&seconds);
					localtime_s(&timeinfo, &seconds);
					strftime(timeStr, 120, "%B %d, %Y, %I:%M %p", &timeinfo);

					cout << "<FileOps> Error Time: " << timeStr << endl;
					LeaveCriticalSection(&outputCritical);

					ofstream backupLog(BACKUPLOG, ios::app);
					if (backupLog.fail())
					{
						backupLog.clear();
						EnterCriticalSection(&outputCritical);
						cout << "<FileOps> Error: backupLog.fail() triggered..." << endl;
						LeaveCriticalSection(&outputCritical);
					}
					else if (backupLog.is_open())
					{
						backupLog << "> Error Time: " << timeStr << endl;
					}
					else
					{
						EnterCriticalSection(&outputCritical);
						cout << "<FileOps> Error: " << BACKUPLOG << " could not be opened." << endl;
						LeaveCriticalSection(&outputCritical);
					}
					backupLog.close();

					backupErrorCount++;
				}
				else
				{
					EnterCriticalSection(&outputCritical);
					time(&seconds);
					localtime_s(&timeinfo, &seconds);
					strftime(timeStr, 120, "%B %d, %Y, %I:%M %p", &timeinfo);

					cout << "<FileOps> Backup restored successfully at: " << timeStr << endl;
					LeaveCriticalSection(&outputCritical);

					ofstream backupLog(BACKUPLOG, ios::app);
					if (backupLog.fail())
					{
						backupLog.clear();
						EnterCriticalSection(&outputCritical);
						cout << "<FileOps> Error: backupLog.fail() triggered..." << endl;
						LeaveCriticalSection(&outputCritical);
					}
					else if (backupLog.is_open())
					{
						backupLog << "> Successful backup restore at: " << timeStr << endl;
					}
					else
					{
						EnterCriticalSection(&outputCritical);
						cout << "<FileOps> Error: " << BACKUPLOG << " could not be opened." << endl;
						LeaveCriticalSection(&outputCritical);
					}
					backupLog.close();
				}
			}
			// status (local)
			else if (_stricmp(userCommand.c_str(), "status") == 0)
			{
				EnterCriticalSection(&outputCritical);
				cout << "  === Program Status ===" << endl;
				cout << "> Backup Interval: " << configInfo.backupInterval << " minute(s)" << endl;
				cout << "> Unique backups: " << configInfo.numOfBackups << endl;
				cout << "> Last successful backup at: " << timeStr << endl;
				if (forceBackup)
				{
					cout << "> In folder: ForceBackup" << endl;
				}
				else
				{
					cout << "> In folder: " << configInfo.backupDirName << lastBackup << endl; 
				}
				cout << "> Number of errors since program start: " << backupErrorCount << endl;
				LeaveCriticalSection(&outputCritical);
			}
			// start-server (local)
			else if (_stricmp(userCommand.c_str(), "start-server") == 0)
			{
				hInst = ShellExecute(0,
									  "open",
									  configInfo.batchPath.c_str(),
									  0,
									  0,
									  SW_SHOW);

				if ((int)hInst < 32)
				{
					EnterCriticalSection(&outputCritical);
					cout << "<FileOps> ShellExecute error: " << (int)hInst << endl;
					LeaveCriticalSection(&outputCritical);

					ofstream backupLog(BACKUPLOG, ios::app);
					if (backupLog.fail())
					{
						backupLog.clear();
						EnterCriticalSection(&outputCritical);
						cout << "<FileOps> Error: backupLog.fail() triggered..." << endl;
						LeaveCriticalSection(&outputCritical);
					}
					else if (backupLog.is_open())
					{
						backupLog << "> ShellExecute error: " << (int)hInst << endl;
					}
					else
					{
						EnterCriticalSection(&outputCritical);
						cout << "<FileOps> Error: " << BACKUPLOG << " could not be opened." << endl;
						LeaveCriticalSection(&outputCritical);
					}
					backupLog.close();
				}
				EnterCriticalSection(&outputCritical);
				cout << "<FileOps> ShellExecute successful, check server." << endl;
				LeaveCriticalSection(&outputCritical);
			}
			else
			{
				EnterCriticalSection(&outputCritical);
				cout << "<FileOps> Error: Bad command <" << userCommand << "> made it to FileOperations" << endl;
				LeaveCriticalSection(&outputCritical);
			}

			userCommand.clear();
			gLocalCommands.pop();
		}
		else if (!gClientRecv.empty())
		{
			userCommand = gClientRecv.front();

			EnterCriticalSection(&outputCritical);
			cout << "Client command recieved: " << userCommand << endl;
			LeaveCriticalSection(&outputCritical);

			// Force backup (client) //
			if (_stricmp(userCommand.c_str(), "force") == 0)
			{
				EnterCriticalSection(&outputCritical);
				cout << "<FileOps> Attemping backup copy to directory: ForceBackup... " << endl;
				LeaveCriticalSection(&outputCritical);

				if (!ForceBackup(configInfo.mapSourcePath, configInfo.mapDestPath))
				{
					EnterCriticalSection(&outputCritical);
					time(&seconds);
					localtime_s(&timeinfo, &seconds);
					strftime(timeStr, 120, "%B %d, %Y, %I:%M %p", &timeinfo);

					cout << "<FileOps> Error Time: " << timeStr << endl;
					LeaveCriticalSection(&outputCritical);

					gClientSend.push("Force backup failed, check backup log for more details\n");

					ofstream backupLog(BACKUPLOG, ios::app);
					if (backupLog.fail())
					{
						backupLog.clear();
						EnterCriticalSection(&outputCritical);
						cout << "<FileOps> Error: backupLog.fail() triggered..." << endl;
						LeaveCriticalSection(&outputCritical);
					}
					else if (backupLog.is_open())
					{
						backupLog << "> Error Time: " << timeStr << endl;
					}
					else
					{
						EnterCriticalSection(&outputCritical);
						cout << "<FileOps> Error: " << BACKUPLOG << " could not be opened." << endl;
						LeaveCriticalSection(&outputCritical);
					}
					backupLog.close();

					backupErrorCount++;
				}
				else
				{
					EnterCriticalSection(&outputCritical);
					time(&seconds);
					localtime_s(&timeinfo, &seconds);
					strftime(timeStr, 120, "%B %d, %Y, %I:%M %p", &timeinfo);

					cout << "<FileOps> Last successful backup at: " << timeStr << endl;
					cout << "          In folder: ForceBackup" << endl; 
					LeaveCriticalSection(&outputCritical);

					gClientSend.push("Force backup succeeded\n");

					ofstream backupLog(BACKUPLOG, ios::app);
					if (backupLog.fail())
					{
						backupLog.clear();
						EnterCriticalSection(&outputCritical);
						cout << "<FileOps> Error: backupLog.fail() triggered..." << endl;
						LeaveCriticalSection(&outputCritical);
					}
					else if (backupLog.is_open())
					{
						backupLog << "> Last successful backup at: " << timeStr << endl;
						backupLog << "  In folder: ForceBackup" << endl;
					}
					else
					{
						EnterCriticalSection(&outputCritical);
						cout << "<FileOps> Error: " << BACKUPLOG << " could not be opened." << endl;
						LeaveCriticalSection(&outputCritical);
					}
					backupLog.close();

					forceBackup = true;
				}
			}
			// Get Log (client) //
			else if (_stricmp(userCommand.c_str(), "get-log") == 0)
			{
				gSendLog = true;
			}
			// Backup restore (client) //
			else if (userCommand.find(":restore") != -1)
			{
				EnterCriticalSection(&outputCritical);
				cout << "<FileOps> Attemping to restore from a backup..." << endl;
				LeaveCriticalSection(&outputCritical);

				if (!RestoreBackup(configInfo.mapDestPath, configInfo.mapSourcePath, userCommand, configInfo))
				{
					EnterCriticalSection(&outputCritical);
					time(&seconds);
					localtime_s(&timeinfo, &seconds);
					strftime(timeStr, 120, "%B %d, %Y, %I:%M %p", &timeinfo);

					cout << "<FileOps> Error Time: " << timeStr << endl;
					LeaveCriticalSection(&outputCritical);

					gClientSend.push("Restore failed, check backup log for more details\n");

					ofstream backupLog(BACKUPLOG, ios::app);
					if (backupLog.fail())
					{
						backupLog.clear();
						EnterCriticalSection(&outputCritical);
						cout << "<FileOps> Error: backupLog.fail() triggered..." << endl;
						LeaveCriticalSection(&outputCritical);
					}
					else if (backupLog.is_open())
					{
						backupLog << "> Error Time: " << timeStr << endl;
					}
					else
					{
						EnterCriticalSection(&outputCritical);
						cout << "<FileOps> Error: " << BACKUPLOG << " could not be opened." << endl;
						LeaveCriticalSection(&outputCritical);
					}
					backupLog.close();

					backupErrorCount++;
				}
				else
				{
					EnterCriticalSection(&outputCritical);
					time(&seconds);
					localtime_s(&timeinfo, &seconds);
					strftime(timeStr, 120, "%B %d, %Y, %I:%M %p", &timeinfo);

					cout << "<FileOps> Backup restored successfully at: " << timeStr << endl;
					LeaveCriticalSection(&outputCritical);

					gClientSend.push("Restore succeeded\n");

					ofstream backupLog(BACKUPLOG, ios::app);
					if (backupLog.fail())
					{
						backupLog.clear();
						EnterCriticalSection(&outputCritical);
						cout << "<FileOps> Error: backupLog.fail() triggered..." << endl;
						LeaveCriticalSection(&outputCritical);
					}
					else if (backupLog.is_open())
					{
						backupLog << "> Successful backup restore at: " << timeStr << endl;
					}
					else
					{
						EnterCriticalSection(&outputCritical);
						cout << "<FileOps> Error: " << BACKUPLOG << " could not be opened." << endl;
						LeaveCriticalSection(&outputCritical);
					}
					backupLog.close();
				}
			}
			// status (client) //
			else if (_stricmp(userCommand.c_str(), "status") == 0)
			{
				gClientSend.push("=== Program Status ===\n");

				clientReturn << "> Backup Interval: " << configInfo.backupInterval << " minute(s)" << endl;
				gClientSend.push(clientReturn.str());
				clientReturn.str("");

				clientReturn << "> Unique Backups: " << configInfo.numOfBackups << endl;
				gClientSend.push(clientReturn.str());
				clientReturn.str("");

				clientReturn << "> Last successful backup at: " << timeStr << endl;
				gClientSend.push(clientReturn.str());
				clientReturn.str("");

				if (forceBackup)
				{
					clientReturn << "  In folder: ForceBackup" << endl;
					gClientSend.push(clientReturn.str());
				}
				else
				{
					clientReturn << "  In folder: " << configInfo.backupDirName << lastBackup << endl; 
					gClientSend.push(clientReturn.str());
				}
				clientReturn.str("");

				clientReturn << "> Number of errors since program start: " << backupErrorCount << endl;
				gClientSend.push(clientReturn.str());
				clientReturn.str("");
			}
			// start-server (client //
			else if (_stricmp(userCommand.c_str(), "start-server") == 0)
			{
				hInst = ShellExecute(0,
									 "open",
									 configInfo.batchPath.c_str(),
									 0,
									 0,
									 SW_SHOW);

				if ((int)hInst < 32)
				{
					EnterCriticalSection(&outputCritical);
					cout << "<FileOps> ShellExecute error: " << (int)hInst << endl;
					LeaveCriticalSection(&outputCritical);

					clientReturn << "ShellExecute error: " << (int)hInst << endl;
					gClientSend.push(clientReturn.str());
					clientReturn.str("");

					ofstream backupLog(BACKUPLOG, ios::app);
					if (backupLog.fail())
					{
						backupLog.clear();
						EnterCriticalSection(&outputCritical);
						cout << "<FileOps> Error: backupLog.fail() triggered..." << endl;
						LeaveCriticalSection(&outputCritical);
					}
					else if (backupLog.is_open())
					{
						backupLog << "> ShellExecute error: " << (int)hInst << endl;
					}
					else
					{
						EnterCriticalSection(&outputCritical);
						cout << "<FileOps> Error: " << BACKUPLOG << " could not be opened." << endl;
						LeaveCriticalSection(&outputCritical);
					}
					backupLog.close();
				}

				gClientSend.push("ShellExecute successful, check server.");
			}
			else
			{
				time(&seconds);
				localtime_s(&timeinfo, &seconds);
				strftime(timeStr, 120, "%B %d, %Y, %I:%M %p", &timeinfo);

				EnterCriticalSection(&outputCritical);
				cout << "<FileOps> Error: Bad command <" << userCommand << "> made it to FileOperations" << endl;
				cout << "          Either an unseen error has occured, or an unknown client is being used." << endl;
				cout << "<FileOps> Error time: " << timeStr << endl;
				LeaveCriticalSection(&outputCritical);

				ofstream backupLog(BACKUPLOG, ios::app);
				if (backupLog.fail())
				{
					backupLog.clear();
					EnterCriticalSection(&outputCritical);
					cout << "<FileOps> Error: backupLog.fail() triggered..." << endl;
					LeaveCriticalSection(&outputCritical);
				}
				else if (backupLog.is_open())
				{
					backupLog << "> Error: Bad command <" << userCommand << "> made it to FileOperations" << endl;
					backupLog << "  Either an unseen error has occured, or an unknown client is being used." << endl;
					backupLog << "> Error time: " << timeStr << endl;
				}
				else
				{
					EnterCriticalSection(&outputCritical);
					cout << "<FileOps> Error: " << BACKUPLOG << " could not be opened." << endl;
					LeaveCriticalSection(&outputCritical);
				}
				backupLog.close();

				clientReturn << "Error: Bad command <" << userCommand << "> made it to FileOperations" << endl;
				gClientSend.push(clientReturn.str());
				clientReturn.str("");
			}

			userCommand.clear();
			gClientRecv.pop();
		}
	}

	return 0;
}

//////////////////////
// Backup Functions //
//////////////////////
bool RecursiveCopy(fs::path source, fs::path destination);

bool Backup(fs::path source, fs::path destination, string backupName, unsigned short backupNumber)
{
	stringstream itos; // int to string
	itos << backupNumber;
	backupName.append(itos.str());

	destination /= backupName;

	return RecursiveCopy(source, destination);
}

bool ForceBackup(fs::path source, fs::path destination)
{
	destination /= "ForceBackup";

	return RecursiveCopy(source, destination);
}

bool RestoreBackup(fs::path source, fs::path destination, string userCommand, ConfigInfo &configInfo)
{
	string folder;
	string::iterator iter;

	for (iter = userCommand.begin(); iter < userCommand.end(); iter++)
	{
		if (*iter == ':')
		{
			break;
		}
		folder.push_back(*iter);
	}

	source /= folder;

	ofstream backupLog(BACKUPLOG, ios::app);
	if (backupLog.fail())
	{
		backupLog.clear();
		EnterCriticalSection(&outputCritical);
		cout << "<FileOps> Error: backupLog.fail() triggered..." << endl;
		LeaveCriticalSection(&outputCritical);
	}
	else if (backupLog.is_open())
	{
		backupLog << "> Attempting to restore from: " << folder << endl;
	}
	else
	{
		EnterCriticalSection(&outputCritical);
		cout << "<FileOps> Error: " << BACKUPLOG << " could not be opened." << endl;
		LeaveCriticalSection(&outputCritical);
	}
	backupLog.close();

	return RecursiveCopy(source, destination);
}

bool RecursiveCopy(fs::path source, fs::path destination)
{
	try
	{
		fs::directory_iterator endIter;
		fs::directory_iterator dirIter(source);

		fs::path src(source);
		fs::path dest(destination);

		if (!fs::exists(source))
		{
			filesys::file_not_found;
		}

		if (!fs::exists(destination))
		{
			EnterCriticalSection(&outputCritical);
			cout << "<FileOps> Creating directory: " << destination << endl;
			LeaveCriticalSection(&outputCritical);
			fs::create_directory(destination);
		}

		for (;dirIter != endIter; ++dirIter)
		{
			src = *dirIter;
			dest /= src.filename();

			if (fs::is_directory(src))
			{
				RecursiveCopy(src, dest);
				dest.remove_filename();
				continue;
			}

			EnterCriticalSection(&outputCritical);
			cout << "<FileOps> Copying " << src.filename() << endl;
			LeaveCriticalSection(&outputCritical);
			fs::copy_file(src, dest, fs::copy_option::overwrite_if_exists);
			dest.remove_filename();
		}
	}
	catch (const fs::filesystem_error ex)
	{
		EnterCriticalSection(&outputCritical);
		cout << "<FileOps> Error Log: " << ex.what() << endl;
		LeaveCriticalSection(&outputCritical);

		ofstream backupLog(BACKUPLOG, ios::app);
		if (backupLog.fail())
		{
			backupLog.clear();
			EnterCriticalSection(&outputCritical);
			cout << "<FileOps> Error: backupLog.fail() triggered..." << endl;
			LeaveCriticalSection(&outputCritical);
		}
		else if (backupLog.is_open())
		{
			backupLog << "> Error Log: " << ex.what() << endl;
		}
		else
		{
			EnterCriticalSection(&outputCritical);
			cout << "<FileOps> Error: " << BACKUPLOG << " could not be opened." << endl;
			LeaveCriticalSection(&outputCritical);
		}
		backupLog.close();
		
		return false;
	}

	return true;
}