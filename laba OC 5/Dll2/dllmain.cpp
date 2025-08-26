#include "pch.h"
#include <windows.h>
#include <tchar.h>
#include <psapi.h>

#pragma data_seg(".datashr")
DWORD dwProcessesAttached = 0;
DWORD dwAllThreadsAttached = 0;
#pragma data_seg() 
#pragma comment(linker, "/SECTION:.datashr,RWS") 

DWORD dwThreadsAttached = 0;
__declspec(dllexport) int i = 0;

// Функция для отправки статистики по использованию памяти
__declspec(dllexport) int SendMemoryUsage(char* nameDLL) {

	PROCESS_MEMORY_COUNTERS_EX pmc;
	pmc.cb = sizeof(pmc);
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));

	TCHAR message[256];
	//wsprintf(message, _T("\n There are %s libraries in use \n Memory Usage: %lu KB \n"), nameDLL, (pmc.WorkingSetSize / 1024));
	return (pmc.WorkingSetSize / 1024);
}

// Функция DllMain
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		++dwProcessesAttached;
		++dwAllThreadsAttached;
		++dwThreadsAttached;
		break;

	case DLL_THREAD_ATTACH:
		++dwAllThreadsAttached;
		++dwThreadsAttached;
		break;

	case DLL_THREAD_DETACH:
		--dwAllThreadsAttached;
		--dwThreadsAttached;
		break;

	case DLL_PROCESS_DETACH:
		--dwProcessesAttached;
		--dwAllThreadsAttached;
		--dwThreadsAttached;
		break;
	}
	return TRUE;
}
