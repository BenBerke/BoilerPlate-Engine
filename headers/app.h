//
// Created by berke on 9/11/2026.
//

#ifndef BPENGINE_APP_H
#define BPENGINE_APP_H

#define UNICODE
#define _UNICODE
#ifdef _WIN32
#include <windows.h>
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

#endif //BPENGINE_APP_H