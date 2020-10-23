#pragma once

namespace process_search
{
    struct EnumData;

    HANDLE GetProcessByName(PCSTR name);

    BOOL CALLBACK EnumProc(HWND hWnd, LPARAM lParam);
    HWND FindWindowFromProcessId(DWORD dwProcessId);

    HWND FindWindowFromProcess(HANDLE hProcess);
};