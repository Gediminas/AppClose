#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <string>

static int GetParentProcessPID(HANDLE hSnapShot, DWORD pid)
{
	PROCESSENTRY32 processInfo;
	processInfo.dwSize = sizeof(PROCESSENTRY32);

	int index=0;
	if (Process32First(hSnapShot, &processInfo))
	{
		do
		{
			if (processInfo.th32ProcessID == pid)
			{
				//TRACE("Parent Process ID: %d\n", processInfo.th32ParentProcessID);
				//TRACE("Process ID: %d\n", processInfo.th32ProcessID);
				//TRACE("Name: %s\n\n", processInfo.szExeFile);
				return processInfo.th32ParentProcessID;
			}
		}
		while(Process32Next(hSnapShot, &processInfo));
	}

	return 0;
}

static int GetTopProcessPID(DWORD pid)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);

	DWORD pid_parrent = pid;
	while (pid_parrent)
	{
		pid = pid_parrent;
		pid_parrent = GetParentProcessPID(hSnapShot, pid);
	}


	CloseHandle(hSnapShot);
	return pid;
}

static HWND GetHWNDFromPID(DWORD pid)
{
	DWORD pid_tmp, dwThreadId;

	HWND hWnd = NULL;
	HWND hTmp = ::GetTopWindow(NULL);
	
	while (hTmp)
	{
		pid_tmp = 0;
		dwThreadId = ::GetWindowThreadProcessId(hTmp, &pid_tmp);

		if (pid_tmp == pid)
		{
			return hTmp;
		}

		hTmp = ::GetNextWindow(hTmp, GW_HWNDNEXT);
	}

	return NULL;
}

int main(int argc, char* argv[])
{
	DWORD pid, top_pid;
	HWND hWnd;
	char* param;

	if (argc < 2)
	{
		printf ("\n");
		printf ("Please provide parameter - process PID\n");
		printf ("  Can be number or path to the file where number is stored\n");
		printf ("  e.g. AppClose 1234\n");
		printf ("       AppClose c:\\path\\app.pid\n");
		goto ON_ERROR;
	}

	param = argv[1];
	pid = atoi(param);

	if (pid == 0)
	{
		FILE *f = fopen(param, "r");

		if (!f)
		{
			printf ("File '%s' not found or locked...\n", param);
			goto ON_ERROR;
		}

		char str[10];
		fread(str, 1, 10, f);
		fclose(f);
		pid = atoi(str);
	}

	top_pid = GetTopProcessPID(pid);
	hWnd    = GetHWNDFromPID(top_pid);

	printf("Process PID:              %d\n", pid);
	printf("Top process PID:          %d\n", top_pid);

	if (!hWnd)
	{
		printf ("HWND not found...\n");
		goto ON_ERROR;
	}

	printf("Sending WM_CLOSE to HWND: 0x%X\n", hWnd);

	::SendMessage(hWnd, WM_CLOSE, 0, 0);

//ON_OK:
	printf("DONE\n");
	#ifdef _DEBUG
		getchar();
	#endif
	return 0;

ON_ERROR:
	//printf("ERROR\n");
	#ifdef _DEBUG
		getchar();
	#endif
	return -1;
}

