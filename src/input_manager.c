//
// Created by berke on 8/5/2026.
//

#include "../headers/app.h"
#include "../headers/input_manager.h"
#include "../headers/typedefs.h"
#include "../headers/config.h"
#include "../headers/mmu.h"

static bp_ptr im_memory = {null, ~0};

#define IM_KEY_COUNT 256

#define IM_KEYS_SIZE (sizeof(bool) * IM_KEY_COUNT)
#define IM_LAST_KEYS_SIZE IM_KEYS_SIZE
#define IM_MOUSE_POS_SIZE (sizeof(unsigned int) * 2)

#define IM_LAST_KEYS_OFFSET (IM_KEYS_SIZE)
#define IM_MOUSE_POS_OFFSET (IM_KEYS_SIZE + IM_LAST_KEYS_SIZE)
#define TOTAL_SIZE (IM_KEYS_SIZE + IM_LAST_KEYS_SIZE + IM_MOUSE_POS_SIZE)

#define GET_KEY (bp_keys()[(int)(keycode)])
#define GET_KEY_LAST (bp_last_keys()[(int)(keycode)])

static bool* bp_keys(void) { return (bool*)im_memory.ptr;}
static bool* bp_last_keys(void) { return (bool*)((byte*)im_memory.ptr + IM_LAST_KEYS_OFFSET); }
static unsigned int* bp_mouse_pos(void) { return (unsigned int*)((byte*)im_memory.ptr + IM_MOUSE_POS_OFFSET); }

void bp_im_init() {
    im_memory = bp_malloc(TOTAL_SIZE);
    bp_memset(&im_memory, 0, TOTAL_SIZE);
}
void bp_im_begin() {
    bp_ptr current_keys = {.ptr = bp_keys(), .size = IM_KEYS_SIZE};
    bp_ptr last_keys = {.ptr = bp_last_keys(), .size = IM_LAST_KEYS_SIZE};
    bp_memcpy(&last_keys, &current_keys, IM_KEYS_SIZE);
}

bool bp_im_key_get(const KEYCODES keycode) { return GET_KEY; }
bool bp_im_key_get_down(const KEYCODES keycode) {return (GET_KEY) && !(GET_KEY_LAST);}
bool bp_im_key_get_up(const KEYCODES keycode) {return !(GET_KEY) && (GET_KEY_LAST);}

unsigned int bp_im_mouse_pos_x() { return bp_mouse_pos()[0]; }
unsigned int bp_im_mouse_pos_y() { return bp_mouse_pos()[1]; }

#ifdef _WIN32
LRESULT CALLBACK WindowProc(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
    unsigned char wKey;

    switch (uMsg) {
        case WM_MOUSEMOVE:
            unsigned int* mouse_pos = bp_mouse_pos();
            mouse_pos[0] = (unsigned int)LOWORD(lParam);
            mouse_pos[1] = (unsigned int)HIWORD(lParam);
            break;
        case WM_LBUTTONDOWN: {
            unsigned int x = LOWORD(lParam);
            unsigned int y = HIWORD(lParam);
            break;
        }
        case WM_KEYDOWN: {
            if (im_memory.ptr == NULL) break;
            const unsigned int key = (unsigned int)wParam;
            if (key < IM_KEY_COUNT) bp_keys()[key] = true;
            break;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            if (im_memory.ptr == NULL) break;
            const unsigned int key = (unsigned int)wParam;
            if (key < IM_KEY_COUNT) bp_keys()[key] = false;
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_PAINT:
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            BITMAPINFO bmi = {};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = w_w;
            bmi.bmiHeader.biHeight = -w_h;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_BITFIELDS;

            ((DWORD*)bmi.bmiColors)[0] = 0xFF000000;
            ((DWORD*)bmi.bmiColors)[1] = 0x00FF0000;
            ((DWORD*)bmi.bmiColors)[2] = 0x0000FF00;

            StretchDIBits(
                hdc,
                0, 0, w_w, w_h,
                0, 0, w_w, w_h,
                screen_buffer, &bmi,
                DIB_RGB_COLORS, SRCCOPY
            );

            EndPaint(hWnd, &ps);
            return 0;
            break;
        case WM_SIZE:
            w_w = LOWORD(lParam);
            w_h = HIWORD(lParam);

            if (w_w == 0 || w_h == 0) break;
            if (screen_buffer != NULL) free(screen_buffer);
            screen_buffer = malloc(BP_SCREEN_BUFFER_SIZE * sizeof(DWORD));
            memset(screen_buffer, 0, BP_SCREEN_BUFFER_SIZE * sizeof(DWORD));
            break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
#endif
