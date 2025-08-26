#include <stdio.h>
#include <windows.h>

void main()
{
    HANDLE hFile;
    OVERLAPPED over;
    DWORD dwError;
    BOOL bResult;
    LARGE_INTEGER fileSize;

    over.Offset = 0;
    over.OffsetHigh = 0;

    // Создаём событие для контроля за асинхронным чтением
    over.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (over.hEvent == NULL)
    {
        printf("\nError create event!!!");
        exit(0);
    }

    hFile = CreateFile(L"opo.txt", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("\nError opening file!!!");
        exit(0);
    }

    // Получаем размер файла
    bResult = GetFileSizeEx(hFile, &fileSize);
    if (!bResult)
    {
        printf("\nError getting file size!!!");
        CloseHandle(hFile);
        exit(0);
    }

    int bufferSize = 4096; // Размер буфера чтения
    int bytesRead;
    int iLatinChars = 0;
    char* buffer = (char*)malloc(bufferSize);

    while (over.Offset < fileSize.QuadPart)
    {
        bResult = ReadFile(hFile, buffer, bufferSize, (LPDWORD)&bytesRead, &over);

        if (!bResult)
        {
            switch (dwError = GetLastError())
            {
            case ERROR_HANDLE_EOF:
                // Мы достигли конца файла в течение вызова ReadFile
                break;

            case ERROR_IO_PENDING:
                // Асинхронный ввод-вывод все еще происходит
                bResult = GetOverlappedResult(hFile, &over, (LPDWORD)&bytesRead, TRUE);

                if (!bResult)
                {
                    printf("\nError work to file!!!");
                    break;
                }
                break;

            default:
                printf("\nError work to file!!!");
                break;
            }
        }

        // Подсчет символов латинского алфавита
        for (int i = 0; i < bytesRead; ++i)
        {
            if ((buffer[i] >= 'a' && buffer[i] <= 'z') || (buffer[i] >= 'A' && buffer[i] <= 'Z'))
                iLatinChars++;
        }

        over.Offset += bytesRead;
    }

    printf("\nIn text %d Latin characters!!!", iLatinChars);
    getchar();

    CloseHandle(hFile);
    free(buffer);
}
