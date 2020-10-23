#include "main.h"

namespace process_search
{
    struct EnumData {
        DWORD dwProcessId;
        HWND hWnd;
    };

    HANDLE GetProcessByName(PCSTR name)
    {
        DWORD pid = 0;

        // Create toolhelp snapshot.
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 process;
        ZeroMemory(&process, sizeof(process));
        process.dwSize = sizeof(process);

        // Walkthrough all processes.
        if (Process32First(snapshot, &process))
        {
            do
            {
                // Compare process.szExeFile based on format of name, i.e., trim file path
                // trim .exe if necessary, etc.

                if (std::string(process.szExeFile) == std::string(name))
                {
                    pid = process.th32ProcessID;
                    break;
                }
            } while (Process32Next(snapshot, &process));
        }

        CloseHandle(snapshot);

        if (pid != 0)
        {
            return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        }

        // Not found


        return NULL;
    }

    BOOL CALLBACK EnumProc(HWND hWnd, LPARAM lParam) {
        // Retrieve storage location for communication data
        EnumData& ed = *(EnumData*)lParam;
        DWORD dwProcessId = 0x0;
        // Query process ID for hWnd
        GetWindowThreadProcessId(hWnd, &dwProcessId);
        // Apply filter - if you want to implement additional restrictions,
        // this is the place to do so.
        if (ed.dwProcessId == dwProcessId) {
            // Found a window matching the process ID
            ed.hWnd = hWnd;
            // Report success
            SetLastError(ERROR_SUCCESS);
            // Stop enumeration
            return FALSE;
        }
        // Continue enumeration
        return TRUE;
    }

    HWND FindWindowFromProcessId(DWORD dwProcessId) {
        EnumData ed = { dwProcessId };
        if (!EnumWindows(EnumProc, (LPARAM)&ed) &&
            (GetLastError() == ERROR_SUCCESS)) {
            return ed.hWnd;
        }
        return NULL;
    }

    HWND FindWindowFromProcess(HANDLE hProcess) {
        return FindWindowFromProcessId(GetProcessId(hProcess));
    }
};