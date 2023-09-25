#pragma once
#ifndef UNICODE
#define UNICODE
#endif
#pragma comment(lib, "mpr.lib")

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <Winnetwk.h>
#include <wtsapi32.h>

class CSMB
{
private:
	LPWSTR IP;
	LPWSTR username;
	LPWSTR password;
public:
	CSMB(LPWSTR IP, LPWSTR username, LPWSTR password);
	~CSMB();
	// method
	bool SMBConnect(LPWSTR role);
	bool SMBDisconnect();
	bool copyFile(LPWSTR src, LPWSTR dst);
	bool stopService(LPWSTR dst);
	bool deleteService(LPWSTR dst);
	SC_HANDLE createService(SC_HANDLE, LPWSTR dst);
	bool startService(SC_HANDLE);
	bool runService(LPWSTR dst);
};

