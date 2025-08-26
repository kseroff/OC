#include <windows.h>
#include <iostream>
#include <tchar.h>

typedef int (*SendMemory)(char*);

__declspec(dllimport) int SendMemoryUsage_(char* nameDLL);

int _tmain() {

    char temp[] = { "1 labraries" };
    printf(("\n There are %s libraries in use \n Memory Usage: %lu KB \n"), temp, SendMemoryUsage_(temp));

    //////////////////////////////////////////////////////////

    HINSTANCE hDLL = LoadLibrary(L"Dll2.dll");
    if (hDLL == NULL) {
        std::cerr << "Failed to load DLL." << std::endl;
        return 1;
    }

    // Получение указателя на функцию

    SendMemory addFunc = (SendMemory)GetProcAddress(hDLL, "SendMemoryUsage");

    if (addFunc == NULL) {
        std::cerr << "Failed to get function pointer." << std::endl;
        FreeLibrary(hDLL);
        return 1;
    }

    // Вызов функции
    char temp2[] = { "2 labraries" };
    //std::cout<<std::endl<<addFunc(temp2);
    printf(("\n There are %s libraries in use \n Memory Usage: %lu KB \n"), temp2, addFunc(temp2));

    // Освобождение первой DLL
    FreeLibrary(hDLL);


    return 0;
}
