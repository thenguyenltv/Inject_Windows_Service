#include "smb.h"

CSMB::CSMB(LPWSTR IP, LPWSTR username, LPWSTR password)
{
	this->IP = IP;
	this->username = username;
	this->password = password;
}

CSMB::~CSMB()
{

}

bool CSMB::SMBConnect(LPWSTR role)
{
	LPWSTR localname = NULL;
	LPWSTR remotename = (LPWSTR)malloc(100 * sizeof(wchar_t));
	if (remotename != NULL)
		swprintf_s(remotename, 100, L"\\\\%s\\%s", IP, role);

	wprintf(L"Calling WNetAddConnection2 with\n");
	wprintf(L"  lpRemoteName = %s\n", remotename);
	wprintf(L"  lpUsername = %s\n", username);
	wprintf(L"  lpPassword = %s\n", password);

	// Zero out the NETRESOURCE struct
	NETRESOURCE nr;
	memset(&nr, 0, sizeof(NETRESOURCE));
	nr.dwType = RESOURCETYPE_ANY;
	nr.lpLocalName = NULL;
	nr.lpRemoteName = (LPWSTR)remotename;
	nr.lpProvider = NULL;

	DWORD dwRetVal;
	DWORD dwFlags;
	dwFlags = CONNECT_UPDATE_PROFILE;

	// Disconnect if already connect
	if (remotename == NULL)
	{
		wprintf(L"Error with name of remote (remotename)!\n");
		return false;
	}
	dwRetVal = WNetCancelConnection2(remotename, CONNECT_UPDATE_PROFILE, FALSE);
	if (dwRetVal == NO_ERROR)
	{
		wprintf(L"Connection added to %s\n", nr.lpRemoteName);
		return true;
	}
	// Connect
	dwRetVal = WNetAddConnection2(&nr, (wchar_t*)password, (wchar_t*)username, dwFlags);
	if (dwRetVal == NO_ERROR)
	{
		wprintf(L"Connection added to %s\n", nr.lpRemoteName);
		return true;
	}
	else
	{
		wprintf(L"WNetAddConnection2 failed with error: %u\n", dwRetVal);
		return false;
	}
	return true;
}

bool CSMB::SMBDisconnect()
{
	return WNetCancelConnection2W(IP, 0, TRUE);
}

bool CSMB::copyFile(LPWSTR src, LPWSTR dst)
{
	if (!CopyFile(src, dst, FALSE))
	{
		wprintf(L"Copy file failed with error: %u\n", GetLastError());
		return false;
	}
	return true;
}

bool CSMB::stopService(LPWSTR dst)
{
	return true;
}

bool CSMB::deleteService(LPWSTR dst)
{
	return true;
}

// Make sure the service is not already installed.
SC_HANDLE CSMB::createService(SC_HANDLE schSCManager, LPWSTR dst)
{
	SC_HANDLE schService;
	schService = OpenService(
		schSCManager,
		L"svc",
		SERVICE_ALL_ACCESS);
	if (schService) // service exist
	{
		// deleteService(dst);
		wprintf(L"Service is already installed!\n");
		// just notice
		// __Upgrade in the future
		wprintf(L"Try delete it before installing the new version!\n");
		return NULL;
	}
	else // service does not exist
	{
		schService = CreateService(
			schSCManager,
			L"svc",
			L"svc",
			SERVICE_ALL_ACCESS,
			SERVICE_WIN32_OWN_PROCESS,
			SERVICE_DEMAND_START,
			SERVICE_ERROR_NORMAL,
			dst,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL);
		if (schService == NULL)
		{
			wprintf(L"CreateService failed with error: %u\n", GetLastError());
			return NULL;
		}
	}
	return schService;
}

bool CSMB::startService(SC_HANDLE schService)
{
	// Start service
	if (!StartService(schService, 0, NULL))
	{
		wprintf(L"StartService failed with error: %u\n", GetLastError());
		return false;
	}
	return true;
}

bool CSMB::runService(LPWSTR dst)
{
	// Open service manager in remote TO CREATE SERVICE AND START SERVICE
	SC_HANDLE schSCManager;
	schSCManager = OpenSCManager(
		IP,
		SERVICES_ACTIVE_DATABASE,
		SC_MANAGER_CREATE_SERVICE);
	if (schSCManager == NULL)
	{
		wprintf(L"OpenSCManager failed with error: %u\n", GetLastError());
		return false;
	}

	// Create service
	SC_HANDLE hService = createService(schSCManager, dst);
	if (hService == NULL)
	{
		wprintf(L"CreateService failed with error: %u\n", GetLastError());
		return false;
	}

	// Start service
	if (!startService(hService))
	{
		wprintf(L"StartService failed with error: %u\n", GetLastError());
		return false;
	}

	// close handle
	if (CloseServiceHandle(hService) == 0)
	{
		wprintf(L"CloseServiceHandle failed with error: %u\n", GetLastError());
		return false;
	}
	return true;
}

