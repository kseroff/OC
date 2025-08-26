#include <stdio.h>
#include <Windows.h>

// Глобальная критическая секция
CRITICAL_SECTION gCriticalSection;
HANDLE gEvents[9];

// Функция, которую выполняет каждая нить
DWORD WINAPI ThreadFunc(LPVOID lpParam) {
    int threadNumber = (int)lpParam;

    while (1) {
        // Ожидаем, пока событие для данной нити не станет сигнальным
        WaitForSingleObject(gEvents[threadNumber], INFINITE);

        // Входим в критическую секцию
        EnterCriticalSection(&gCriticalSection);

        printf("Thread %d: %c\n", threadNumber, 'A' + threadNumber);

        LeaveCriticalSection(&gCriticalSection);

        // Сбрасываем событие
        ResetEvent(gEvents[threadNumber]);
    }

    return 0;
}

int main() {

    InitializeCriticalSection(&gCriticalSection);
    for (int i = 0; i < 9; i++) {
        gEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
    }

    HANDLE threads[9];
    for (int i = 0; i < 9; i++) {
        threads[i] = CreateThread(NULL, 0, ThreadFunc, (LPVOID)i, 0, NULL);
    }

    // Основная нить будет устанавливать события для других нитей
    while (1) {
        for (int i = 0; i < 9; i++) {
            SetEvent(gEvents[i]);
            Sleep(1000);
        }
    }

    WaitForMultipleObjects(9, threads, TRUE, INFINITE);

    for (int i = 0; i < 9; i++) {
        CloseHandle(gEvents[i]);
    }

    DeleteCriticalSection(&gCriticalSection);

    return 0;
}
