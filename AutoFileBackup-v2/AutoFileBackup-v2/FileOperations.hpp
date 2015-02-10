#ifndef _FILE_OPERATIONS_HPP_
#define _FILE_OPERATIONS_HPP_

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "Ws2_32.lib")
#include <string>
using namespace std;

#include <boost/filesystem.hpp>
#include <boost/progress.hpp>

namespace filesys = boost::filesystem3;
namespace fs = boost::filesystem;

#define BACKUPLOG "backup_log.txt"

unsigned int __stdcall FileOpMain(void* configParams);
bool Backup(fs::path source, fs::path destination, string backupName, unsigned short backupNumber);
bool ForceBackup(fs::path source, fs::path destination);
bool RestoreBackup(fs::path source, fs::path destination, string userCommand, ConfigInfo& configInfo);

#endif