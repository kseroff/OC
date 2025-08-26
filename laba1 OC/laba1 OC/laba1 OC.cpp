#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>

HANDLE hMutex;
HANDLE hFileMapping;

struct SharedData {
    int count;
    char command[256];
};

SharedData* sharedData;

DWORD WINAPI ThreadProc(LPVOID lpParam) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    TCHAR szCommandLine[] = TEXT("cmd");

    if (!CreateProcess(NULL, szCommandLine, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        printf("Ошибка при создании процесса: %d\n", GetLastError());
        return 1;
    }

    sharedData->count++;
    system("cls");
    std::cout << "Запущено процессов: " << sharedData->count;

    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD processId2 = GetProcessId(pi.hProcess);

    sharedData->count--;
    system("cls");
    std::cout << "Запущено процессов: " << sharedData->count;

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    if (processId2 == sharedData->count) {
        ReleaseMutex(hMutex);
    }

    return 0;
}

int main() {
    std::setlocale(0, "");

    hMutex = CreateMutex(NULL, FALSE, NULL);
    if (hMutex == NULL) {
        printf("Ошибка при создании мьютекса: %d\n", GetLastError());
        return 1;
    }

    hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SharedData), L"Local\\MyFileMappingObject");
    if (hFileMapping == NULL) {
        printf("Ошибка при создании файлового маппинга: %d\n", GetLastError());
        CloseHandle(hMutex);
        return 1;
    }

    sharedData = (SharedData*)MapViewOfFile(hFileMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, sizeof(SharedData));
    if (sharedData == NULL) {
        printf("Ошибка при отображении файла в память: %d\n", GetLastError());
        CloseHandle(hMutex);
        CloseHandle(hFileMapping);
        return 1;
    }

    int numThreads = 64;

    HANDLE* threads = new HANDLE[numThreads];
    for (int i = 0; i < numThreads; i++) {
        _getch();
        WaitForSingleObject(hMutex, INFINITE);
        threads[i] = CreateThread(NULL, 0, ThreadProc, reinterpret_cast<LPVOID>(i + 1), 0, NULL);
    }

    while (true) {
        std::cin.getline(sharedData->command, sizeof(sharedData->command));
        if (strcmp(sharedData->command, "exit") == 0) {
            break;
        }
    }

    ReleaseMutex(hMutex);

    WaitForMultipleObjects(numThreads, threads, TRUE, INFINITE);

    CloseHandle(hMutex);
    UnmapViewOfFile(sharedData);
    CloseHandle(hFileMapping);

    delete[] threads;

    return 0;
}
