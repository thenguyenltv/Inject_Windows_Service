/*
Reference:
    https://www.codeproject.com/Articles/499465/Simple-Windows-Service-in-Cplusplus
*/

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

// Define the service name and display name
#define SERVICE_NAME _T("svc")
#define SERVICE_DISPLAY_NAME _T("Simple Service")

// Declare the global variables for the service status and handle
SERVICE_STATUS g_ServiceStatus;
SERVICE_STATUS_HANDLE g_ServiceStatusHandle;

// Declare the global variable for the service stop event
HANDLE g_ServiceStopEvent = INVALID_HANDLE_VALUE;

// Declare the prototype of the service main function
VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv);

// Declare the prototype of the service control handler function
VOID WINAPI ServiceCtrlHandler(DWORD CtrlCode);

// Declare the prototype of the service worker thread function
DWORD WINAPI ServiceWorkerThread(LPVOID lpParam);

// The main function of the program
int _tmain(int argc, TCHAR* argv[])
{
    // Declare a variable for the service table entries
    SERVICE_TABLE_ENTRY ServiceTable[] =
    {
        {(LPWSTR)SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
        {NULL, NULL}
    };

    // Start the service control dispatcher thread for our service
    if (StartServiceCtrlDispatcher(ServiceTable) == FALSE)
    {
        // If failed, return an error code
        return GetLastError();
    }

    // If succeeded, return zero
    return 0;
}

// The service main function of service
VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv)
{
    // Register our service control handler with the SCM
    g_ServiceStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);

    if (g_ServiceStatusHandle == NULL)
    {
        // If failed, return without further actions
        return;
    }

    // Initialize the service status structure
    ZeroMemory(&g_ServiceStatus, sizeof(g_ServiceStatus));
    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwServiceSpecificExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 0;

    // Report the initial status to the SCM
    SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus);

    // Create a service stop event to wait on later
    g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (g_ServiceStopEvent == NULL)
    {
        // If failed, report the error to the SCM and stop the service
        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        g_ServiceStatus.dwWin32ExitCode = GetLastError();
        g_ServiceStatus.dwCheckPoint = 1;

        SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus);
        return;
    }

    // Report to the SCM that our service is running
    g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 0;

    SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus);

    // Start a thread that will perform the main task of the service
    HANDLE hThread = CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);

    // Wait until our service is stopped or an error occurs
    WaitForSingleObject(g_ServiceStopEvent, INFINITE);

    // Close the service stop event handle
    CloseHandle(g_ServiceStopEvent);

    // Report to the SCM that our service is stopped
    g_ServiceStatus.dwControlsAccepted = 0;
    g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 3;

    SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus);
}

// The service control handler function of our service
VOID WINAPI ServiceCtrlHandler(DWORD CtrlCode)
{
    switch (CtrlCode)
    {
    case SERVICE_CONTROL_STOP:

        if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
            break;

        // Report to the SCM that our service is stopping
        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        g_ServiceStatus.dwWin32ExitCode = 0;
        g_ServiceStatus.dwCheckPoint = 4;

        SetServiceStatus(g_ServiceStatusHandle, &g_ServiceStatus);

        // Set the service stop event
        SetEvent(g_ServiceStopEvent);

        break;

    default:
        break;
    }
}

// The service worker thread function of our service
DWORD WINAPI ServiceWorkerThread(LPVOID lpParam)
{
    // Declare a variable for the output debug string
    TCHAR szDebugString[100];

    // Declare a variable for the local time
    SYSTEMTIME stLocalTime;

    // Loop until the service is stopped
    while (WaitForSingleObject(g_ServiceStopEvent, 0) != WAIT_OBJECT_0)
    {
        // Get the local time
        GetLocalTime(&stLocalTime);

        // Format the output debug string with the current time
        wsprintf(szDebugString, _T("TEST SVC %02d:%02d:%02d"), stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);

        // Output the debug string
        OutputDebugString(szDebugString);

        // Sleep for 60 seconds
        Sleep(60000);
    }

    // Return zero
    return 0;
}

