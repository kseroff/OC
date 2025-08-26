#include <windows.h>
#include <stdio.h>

// Глобальные переменные
HHOOK g_hMouseHook = NULL;

// Функция обработки низкоуровневых событий мыши
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        MOUSEHOOKSTRUCT* pMouseStruct = (MOUSEHOOKSTRUCT*)lParam;

        // Проверяем, является ли событие перемещением мыши и находится ли курсор в левом верхнем углу
        if (wParam == WM_MOUSEMOVE && pMouseStruct->pt.x == 0 && pMouseStruct->pt.y == 0) {
            MessageBox(NULL, L"Курсор в левом верхнем углу!", L"Уведомление", MB_OK | MB_ICONINFORMATION);
        }
    }

    // Передаем управление следующему перехватчику в цепочке
    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

int main() {
    // Устанавливаем перехватчик для событий мыши
    g_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, NULL, 0);

    if (g_hMouseHook == NULL) {
        MessageBox(NULL, L"Не удалось установить перехватчик мыши!", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Запускаем цикл обработки сообщений
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Отключаем перехватчик перед выходом
    UnhookWindowsHookEx(g_hMouseHook);

    return 0;
}
