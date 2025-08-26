#include <windows.h>
#include <stdio.h>

#define MAILSLOT_SEND_NAME L"\\\\.\\mailslot\\MyMailSlotSend"
#define MAILSLOT_RECEIVE_NAME L"\\\\.\\mailslot\\MyMailSlotReceive"
#define MAX_MESSAGE_SIZE 256

HANDLE hMailSlotSend;
HANDLE hMailSlotReceive;

void SendThread()
{
    char message[MAX_MESSAGE_SIZE];

    while (1)
    {
        printf("Client 2: Enter message for Client 1: ");
        fgets(message, sizeof(message), stdin);

        // Remove newline character
        size_t len = strlen(message);
        if (len > 0 && message[len - 1] == '\n')
        {
            message[len - 1] = '\0';
        }

        DWORD bytesWritten;
        if (WriteFile(hMailSlotSend, message, strlen(message) + 1, &bytesWritten, NULL))
        {
            printf("Client 2: Message sent to Client 1\n");
        }
        else
        {
            printf("Client 2: Error writing to mailslot (%d)\n", GetLastError());
        }
    }
}

void ReceiveThread()
{
    DWORD bytesRead;
    char buffer[MAX_MESSAGE_SIZE];

    while (1)
    {
        if (ReadFile(hMailSlotReceive, buffer, sizeof(buffer), &bytesRead, NULL))
        {
            buffer[bytesRead] = '\0';
            printf("Client 2: Received message from Client 1: %s\n", buffer);
        }
    }
}

int main()
{
    // Open mailslots
    hMailSlotSend = CreateFile(MAILSLOT_SEND_NAME, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    hMailSlotReceive = CreateFile(MAILSLOT_RECEIVE_NAME, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hMailSlotSend == INVALID_HANDLE_VALUE || hMailSlotReceive == INVALID_HANDLE_VALUE)
    {
        printf("Error opening mailslots (%d)\n", GetLastError());
        return 1;
    }

    printf("Client 2 started\n");

    // Create threads for sending and receiving
    HANDLE hSendThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendThread, NULL, 0, NULL);
    HANDLE hReceiveThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveThread, NULL, 0, NULL);

    if (hSendThread == NULL || hReceiveThread == NULL)
    {
        printf("Error creating threads (%d)\n", GetLastError());
        return 1;
    }

    // Wait for threads to finish
    WaitForSingleObject(hSendThread, INFINITE);
    WaitForSingleObject(hReceiveThread, INFINITE);

    // Close handles
    CloseHandle(hMailSlotSend);
    CloseHandle(hMailSlotReceive);

    return 0;
}
