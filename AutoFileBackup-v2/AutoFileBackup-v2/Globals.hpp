#ifndef _GLOBALS_HPP_
#define _GLOBALS_HPP_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Windows
#include <windows.h>

// Data Structures
#include <list>
#include <queue>
#include <deque>

// Misc.
#include <string>
using namespace std;

enum THREADS { SERVER_THREAD, FILEOPS_THREAD, USERINPUT_THREAD };

extern bool gSendLog;
extern bool gQuitProgram;
extern bool gKillClient;
extern CRITICAL_SECTION outputCritical;

extern queue<string> gLocalCommands;
extern queue<string> gClientRecv;
extern queue<string> gClientSend;

#endif