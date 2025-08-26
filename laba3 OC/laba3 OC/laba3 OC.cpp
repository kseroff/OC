#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#define BUFFER_SIZE sizeof(int)

void PrintIntermediateResult(int* pValue)
{
    // Вывод текущего значения переменной
    _tprintf(_T("Current value: %d\n"), *pValue);
}

int _tmain(int argc, _TCHAR* argv[])
{
    HANDLE hMapFile;
    LPCTSTR pBuf;

    // Создание семафоров
    HANDLE semIncrement1 = CreateSemaphore(NULL, 1, 1, L"SemIncrement1");
    HANDLE semDecrement = CreateSemaphore(NULL, 1, 1, L"SemDecrement");

    // Создание общей переменной в разделяемой памяти
    hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUFFER_SIZE, L"MySharedMemory");
    if (hMapFile == NULL)
    {
        printf("\nCould not create file mapping object!!!");
        exit(0);
    }

    pBuf = (LPTSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUFFER_SIZE);
    if (pBuf == NULL)
    {
        printf("\nCould not create file mapping object!!!");
        CloseHandle(hMapFile);
        return 1;
    }

    // Инициализация общей переменной
    int* pValue = (int*)pBuf;
    *pValue = 0;

    // Открываем дескриптор разделяемой переменной
    HANDLE hMutex = CreateMutex(NULL, FALSE, L"MyMutex");

    // Запускаем цикл
    while (1)
    {
        // Ожидаем изменений в разделяемой переменной
        WaitForSingleObject(hMutex, INFINITE);

        // Выводим текущее значение переменной
        PrintIntermediateResult(pValue);

        // Освобождаем мьютекс
        ReleaseMutex(hMutex);

    }

    // Освобождение ресурсов
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
    CloseHandle(semIncrement1);
    CloseHandle(semDecrement);

    return 0;
}
