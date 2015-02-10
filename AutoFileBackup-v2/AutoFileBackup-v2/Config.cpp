#include "Config.hpp"

#include <stdlib.h>

#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;
/////////////////
// NewConfig() //
/////////////////

// Generate new config.txt, properly formatted
bool NewConfig()
{
	cout << "Creating " << CONFIGFILE << "..." << endl;

	ofstream configFile(CONFIGFILE);

	if (configFile.fail())
	{
		configFile.clear();
		configFile.close();
		cout << "Error: configFile.fail() triggered..." << endl;
		return false;
	}
	else if (configFile.is_open())
	{
		configFile << "=== Configuration ===" << endl;
		configFile << "$Password=" << endl;
		configFile << "$Online Mode=on" << endl;
		configFile << "$Server Port Number=" << endl;
		configFile << "$Batch File Path=" << endl;
		configFile << "$Map Source Path=" << endl;
		configFile << "$Map Dest Path=" << endl;
		configFile << "$Backup Directory Name=" << endl;
		configFile << "$Backup Interval (in minutes)=" << endl;
		configFile << "$Unique Backups=" << endl << endl;

		configFile << "=== No Modify ===" << endl;
		configFile << "$Last Backup Number=1" << endl;
	}
	else
	{
		cout << "Error: " << CONFIGFILE << " could not be opened." << endl;

		configFile.close();
		return false;
	}

	configFile.close();


	return true;
}

//////////////////
// LoadConfig() //
//////////////////

// Load config file into ConfigInfo struct.
bool ValidCharacter(char ch, bool filePath);
bool LoadConfig(ConfigInfo& config)
{
	// variables //
	bool foundEquals = false;
	bool foundColon = false;

	// This is used to check the availability of the correct variables.
	short variableCheck = 0;
	const short varibalCount = 10;

	string fileParse;
	string strVariable;
	string::iterator sIterate;
	// ========= //

	ifstream configFile(CONFIGFILE);

	if (configFile.is_open())
	{
		// file parser, checks one line at a time.
		while (getline(configFile, fileParse))
		{
			foundEquals = false;
			foundColon = false;

			// .find checks if its argument exists anywhere in the line(s) it currently holds.
			if (fileParse.find("$Password=") != -1)
			{
				variableCheck++;
				strVariable.clear();
				for (sIterate = fileParse.begin(); sIterate < fileParse.end(); sIterate++)
				{
					if (foundEquals)
					{
						if (!ValidCharacter((char)*sIterate, false))
						{
							cout << "Error: Invalid character (" << *sIterate << ") found in: " << endl;
							cout << fileParse << endl;
							return false;
						}

						strVariable.push_back(*sIterate);
					}
					else if (*sIterate == '=')
					{
						foundEquals = true;
					}
				}

				if (strVariable.size() == 0)
				{
					config.password = " ";
				}
				else
				{
					config.password = strVariable;
				}
			}
			else if (fileParse.find("$Online Mode=") != -1)
			{
				variableCheck++;
				strVariable.clear();
				for (sIterate = fileParse.begin(); sIterate < fileParse.end(); sIterate++)
				{
					if (foundEquals)
					{
						if (!ValidCharacter((char)*sIterate, false))
						{
							cout << "Error: Invalid character (" << *sIterate << ") found in: " << endl;
							cout << fileParse << endl;
							return false;
						}

						strVariable.push_back(*sIterate);
					}
					else if (*sIterate == '=')
					{
						foundEquals = true;
					}
				}

				if (strVariable == "")
				{
					cout << "Error: empty veriable for " << fileParse << endl;
					return false;
				}

				config.onlineMode = strVariable;
			}
			else if (fileParse.find("$Server Port Number=") != -1)
			{
				variableCheck++;
				strVariable.clear();
				for (sIterate = fileParse.begin(); sIterate < fileParse.end(); sIterate++)
				{
					if (foundEquals)
					{
						if (!ValidCharacter((char)*sIterate, false))
						{
							cout << "Error: Invalid character (" << *sIterate << ") found in: " << endl;
							cout << fileParse << endl;
							return false;
						}

						strVariable.push_back(*sIterate);
					}
					else if (*sIterate == '=')
					{
						foundEquals = true;
					}
				}

				if (strVariable == "")
				{
					cout << "Error: empty veriable for " << fileParse << endl;
					return false;
				}

				config.serverPort = strVariable;
			}
			else if (fileParse.find("$Batch File Path=") != -1)
			{
				variableCheck++;
				strVariable.clear();
				for (sIterate = fileParse.begin(); sIterate < fileParse.end(); sIterate++)
				{
					if (foundEquals)
					{
						if (*sIterate == '/')
						{
							*sIterate = '\\';
						}
						else if (*sIterate == ':' && !foundColon)
						{
							foundColon = true;
						}
						else if (!ValidCharacter((char)*sIterate, true))
						{
							cout << "Error: Invalid character (" << *sIterate << ") found in: " << endl;
							cout << fileParse << endl;
							return false;
						}

						strVariable.push_back(*sIterate);
					}
					else if (*sIterate == '=')
					{
						foundEquals = true;
					}
				}

				if (strVariable == "")
				{
					cout << "Error: empty veriable for " << fileParse << endl;
					return false;
				}

				config.batchPath = strVariable;
			}
			else if (fileParse.find("$Map Source Path=") != -1)
			{
				variableCheck++;
				strVariable.clear();
				for (sIterate = fileParse.begin(); sIterate < fileParse.end(); sIterate++)
				{
					if (foundEquals)
					{
						if (*sIterate == '/')
						{
							*sIterate = '\\';
						}
						else if (*sIterate == ':' && !foundColon)
						{
							foundColon = true;
						}
						else if (!ValidCharacter((char)*sIterate, true))
						{
							cout << "Error: Invalid character (" << *sIterate << ") found in: " << endl;
							cout << fileParse << endl;
							return false;
						}

						strVariable.push_back(*sIterate);
					}
					else if (*sIterate == '=')
					{
						foundEquals = true;
					}
				}

				if (strVariable == "")
				{
					cout << "Error: empty veriable for " << fileParse << endl;
					return false;
				}

				config.mapSourcePath = strVariable;
			}
			else if (fileParse.find("$Map Dest Path=") != -1)
			{
				variableCheck++;
				strVariable.clear();
				for (sIterate = fileParse.begin(); sIterate < fileParse.end(); sIterate++)
				{
					if (foundEquals)
					{
						if (*sIterate == '/')
						{
							*sIterate = '\\';
						}
						else if (*sIterate == ':' && !foundColon)
						{
							foundColon = true;
						}
						else if (!ValidCharacter((char)*sIterate, true))
						{
							cout << "Error: Invalid character (" << *sIterate << ") found in: " << endl;
							cout << fileParse << endl;
							return false;
						}

						strVariable.push_back(*sIterate);
					}
					else if (*sIterate == '=')
					{
						foundEquals = true;
					}
				}

				if (strVariable == "")
				{
					cout << "Error: empty veriable for " << fileParse << endl;
					return false;
				}

				config.mapDestPath = strVariable;
			}
			else if (fileParse.find("$Backup Directory Name=") != -1)
			{
				variableCheck++;
				strVariable.clear();
				for (sIterate = fileParse.begin(); sIterate < fileParse.end(); sIterate++)
				{
					if (foundEquals)
					{
						if (!ValidCharacter((char)*sIterate, true))
						{
							cout << "Error: Invalid character (" << *sIterate << ") found in: " << endl;
							cout << fileParse << endl;
							return false;
						}

						strVariable.push_back(*sIterate);
					}
					else if (*sIterate == '=')
					{
						foundEquals = true;
					}
				}

				if (strVariable == "")
				{
					cout << "Error: empty veriable for " << fileParse << endl;
					return false;
				}

				config.backupDirName = strVariable;
			}
			else if (fileParse.find("$Backup Interval (in minutes)=") != -1)
			{
				variableCheck++;
				strVariable.clear();
				for (sIterate = fileParse.begin(); sIterate < fileParse.end(); sIterate++)
				{
					if (foundEquals)
					{
						strVariable.push_back(*sIterate);
					}
					else if (*sIterate == '=')
					{
						foundEquals = true;
					}
				}

				if (strVariable == "")
				{
					cout << "Error: empty veriable for " << fileParse << endl;
					return false;
				}

				config.backupInterval = atoi(strVariable.c_str());
			}
			else if (fileParse.find("$Unique Backups=") != -1)
			{
				variableCheck++;
				strVariable.clear();
				for (sIterate = fileParse.begin(); sIterate < fileParse.end(); sIterate++)
				{
					if (foundEquals)
					{
						strVariable.push_back(*sIterate);
					}
					else if (*sIterate == '=')
					{
						foundEquals = true;
					}
				}

				if (strVariable == "")
				{
					cout << "Error: empty veriable for " << fileParse << endl;
					return false;
				}

				config.numOfBackups = atoi(strVariable.c_str());
			}
			else if (fileParse.find("$Last Backup Number=") != -1)
			{
				variableCheck++;
				strVariable.clear();
				for (sIterate = fileParse.begin(); sIterate < fileParse.end(); sIterate++)
				{
					if (foundEquals)
					{
						strVariable.push_back(*sIterate);
					}
					else if (*sIterate == '=')
					{
						foundEquals = true;
					}
				}

				if (strVariable == "")
				{
					cout << "Error: empty veriable for " << fileParse << endl;
					return false;
				}

				config.backupSwitch = atoi(strVariable.c_str());
			}
		}
	}
	else
	{
		cout << "Error: Unable to open file" << endl;
		configFile.close();
		return false;
	}

	// Missing or duplicate variables in configuration file
	if (variableCheck != varibalCount)
	{
		if (variableCheck < varibalCount)
		{
			cout << "Error: " << CONFIGFILE << " is missing variables, or conflicting with another file of the same name." << endl;
		}
		else if (variableCheck > varibalCount)
		{
			cout << "Error: " << CONFIGFILE << " has duplicate variables." << endl;
		}
		cout << "Delete or move " << CONFIGFILE << " out of the folder and run this program again to generate a new one." << endl;
		return false;
	}

	configFile.close();
	return true;
}

// Check validity of characters
bool ValidCharacter(char ch, bool filePath)
{
	if (!filePath)
	{
		switch (ch)
		{
		case '/':
		case '\\':
			return false;
		}
	}

	switch (ch)
	{
	case '"':
	case '<':
	case '>':
	case ':':
	case '|':
	case '?':
	case '*':
		return false;

	default:
		return true;
	}

	cout << "Error: switch in ValidCharacter failed" << endl;
	return false;
}

bool SaveConfig(ConfigInfo& config)
{
	ofstream configFile(CONFIGFILE, ios::trunc);

	if (configFile.fail())
	{
		configFile.clear();
		configFile.close();
		cout << "Error: configFile.fail() triggered..." << endl;
		return false;
	}
	else if (configFile.is_open())
	{
		configFile << "=== Configuration ===" << endl;
		configFile << "$Password=" << config.password << endl;
		configFile << "$Online Mode=" << config.onlineMode << endl;
		configFile << "$Server Port Number=" << config.serverPort << endl;
		configFile << "$Batch File Path=" << config.batchPath << endl;
		configFile << "$Map Source Path=" << config.mapSourcePath << endl;
		configFile << "$Map Dest Path=" << config.mapDestPath << endl;
		configFile << "$Backup Directory Name=" << config.backupDirName << endl;
		configFile << "$Backup Interval (in minutes)=" << config.backupInterval << endl;
		configFile << "$Unique Backups=" << config.numOfBackups << endl << endl;

		configFile << "=== No Modify ===" << endl;
		configFile << "$Last Backup Number=" << config.backupSwitch << endl;
	}
	else
	{
		cout << "Error: " << CONFIGFILE << " could not be opened." << endl;

		configFile.close();
		return false;
	}

	configFile.close();


	return true;
}