//
// Created by berke on 8/5/2026.
//

#include "../headers/renderer.h"

int w_w, w_h;

#ifdef _WIN32
DWORD* screen_buffer;
static HWND r_HWND = NULL;
#endif

#ifdef _WIN32
BOOL CALLBACK GetThreadWindowsCallBack(HWND hwnd, LPARAM lParam) {
    *(HWND*)lParam = hwnd;
    return FALSE;
}

int r_init_win(const char* title, const unsigned long window_flags) {
    // Fetch HINSTANCE automatically
    HINSTANCE hInstance = GetModuleHandle(NULL);

    const wchar_t CLASS_NAME[] = L"WindowClass";
    wchar_t wTitle[256];
    MultiByteToWideChar(CP_UTF8, 0, title, -1, wTitle, 256);

    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClassEx(&wc)) return 0;

    RECT rc = {0, 0, w_w, w_h};

    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    r_HWND = CreateWindowEx(
      0, CLASS_NAME, wTitle,
      window_flags,
      CW_USEDEFAULT, CW_USEDEFAULT,
      rc.right - rc.left, rc.bottom - rc.top,
      NULL, NULL, hInstance, NULL
    );

    if (r_HWND == NULL) return 0;

    ShowWindow(r_HWND, SW_SHOW);
    UpdateWindow(r_HWND);

    return 1;
}

bool r_poll_events_win() {
    MSG msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) return 0;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return true;
}

bool r_update_window_win() {
    if (r_HWND != NULL) {
        ShowWindow(r_HWND, SW_SHOW);
        InvalidateRect(r_HWND, NULL, FALSE);
        UpdateWindow(r_HWND);

        return true;
    }

    return false;
}

#endif

bool bp_r_poll_events() {
#ifdef _WIN32
    return r_poll_events_win();
#endif
}

void bp_r_init_window(const int w, const int h, const char* title, const unsigned long window_flags) {
    w_w = w;
    w_h = h;
    //todo custom malloc
#ifdef _WIN32
    screen_buffer = malloc(BP_SCREEN_BUFFER_SIZE * sizeof(DWORD));
    r_init_win(title, window_flags);
#endif
}
void bp_r_update_window() {
#ifdef _WIN32
    r_update_window_win();
#endif
}

// ==============
// Draw Functions
// ==============
void r_draw_line(int x, int y, const int x1, const int y1) {
    const int dx = abs(x - x1);
    const int dy = abs(y - y1);

    const char sx = (x < x1) ? 1 : -1;
    const char sy = (y < y1) ? 1 : -1;

    int err = dx - dy;

    for (;;) {
        bp_r_set_pixel(x, y);

        if (x == x1 && y == y1) break;

        const int e2 = err * 2;

        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}