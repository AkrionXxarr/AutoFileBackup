#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_

#include <string>
using namespace std;

#define CONFIGFILE "config.txt"

struct ConfigInfo
{	
	unsigned short backupInterval;
	unsigned short backupSwitch;
	unsigned short numOfBackups;

	string password;
	string onlineMode;
	string batchPath;
	string mapSourcePath;
	string mapDestPath;
	string backupDirName;
	string serverPort;
};

bool NewConfig();
bool SaveConfig(ConfigInfo& config);
bool LoadConfig(ConfigInfo& config);

#endif