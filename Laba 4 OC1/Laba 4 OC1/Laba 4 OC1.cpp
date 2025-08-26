#include <windows.h>
#include <stdio.h>

#define MAILSLOT_NAME_IN  L"\\\\.\\mailslot\\MyMailSlotIn"
#define MAILSLOT_NAME_OUT L"\\\\.\\mailslot\\MyMailSlotOut"

int main() {
    HANDLE hMailSlotIn = CreateFile(MAILSLOT_NAME_IN, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    HANDLE hMailSlotOut = CreateFile(MAILSLOT_NAME_OUT, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

    if (hMailSlotIn == INVALID_HANDLE_VALUE || hMailSlotOut == INVALID_HANDLE_VALUE) {
        printf("Failed to open mailslots, error %lu\n", GetLastError());
        return 1;
    }

    printf("Enter a message to send to the server (type 'exit' to quit):\n");

    while (1) {
        char buffer[256];
        DWORD bytesRead;

        // Чтение сообщений от сервера
        if (ReadFile(hMailSlotOut, buffer, sizeof(buffer), &bytesRead, NULL)) {
            buffer[bytesRead] = '\0';
            printf("Received message from server: %s", buffer);
        }
        else {
            printf("Failed to read mailslot, error %lu\n", GetLastError());
            break;
        }

        // Отправка сообщений серверу
        fgets(buffer, sizeof(buffer), stdin);

        if (strcmp(buffer, "exit\n") == 0) {
            break;
        }

        DWORD bytesWritten;

        if (WriteFile(hMailSlotIn, buffer, strlen(buffer), &bytesWritten, NULL)) {
            printf("Message sent to server: %s", buffer);
        }
        else {
            printf("Failed to write to mailslot, error %lu\n", GetLastError());
            break;
        }
    }

    CloseHandle(hMailSlotIn);
    CloseHandle(hMailSlotOut);

    return 0;
}
