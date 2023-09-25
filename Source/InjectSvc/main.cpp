#ifndef UNICODE
#define UNICODE
#endif
#pragma comment(lib, "mpr.lib")
#include "smb.h"

int wmain(int argc, LPWSTR argv[])
{
	if (argc != 6) {
		wprintf(L"Usage: %s <IP> <username> <password> <src svc.exe> <dst svc.exe>\n", argv[0]);
		wprintf(L"Format of [dst]: Folder\\svc.exe\n");
		exit(1);
	}

	/* Format command:
	* service.exe 192.168.23.1 thenguyen 123 MakeService.exe Folder\\svc.exe
	*/

	LPWSTR IP = (LPWSTR)argv[1];
	LPWSTR username = (LPWSTR)argv[2];
	LPWSTR password = (LPWSTR)argv[3];
	LPWSTR src = (LPWSTR)argv[4];
	LPWSTR pathRemote = (LPWSTR)argv[5];

	// pathRemote: FolderShare
	// dst: \\x.x.x.x\FolderShare
	LPWSTR dst = (wchar_t*)malloc(100 * sizeof(wchar_t));
	if (dst != NULL)
		swprintf_s(dst, 100, L"\\\\%s\\%s", IP, pathRemote);

	// usr: x.x.x.x\RemoteNamePC
	LPWSTR usr = (wchar_t*)malloc(100 * sizeof(wchar_t));
	if (usr != NULL)
		swprintf_s(usr, 100, L"%s\\%s", IP, username);

	CSMB smb(IP, username, password);
	LPWSTR role = (LPWSTR)L"admin$";
	if (smb.SMBConnect(role))
	{
		// copy file svc.exe to remote machine
		if (smb.copyFile(src, dst))
		{
			// run service
			if (!smb.runService(dst))
			{
				wprintf(L"Run service failed\n");
				return 1;
			}
		}
		else
		{
			wprintf(L"Copy file failed\n");
			return 1;
		}
		if (smb.SMBDisconnect())
		{
			wprintf(L"Disconnect success\n");
		}
		else
		{
			wprintf(L"Disconnect failed\n");
		}
	}
	else
	{
		wprintf(L"Connect failed\n");
		return 1;
	}
}