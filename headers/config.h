//
// Created by berke on 9/11/2026.
//

#ifndef BPENGINE_GLOBALS_H
#define BPENGINE_GLOBALS_H

extern int w_w, w_h;
#define BP_SCREEN_BUFFER_SIZE (w_w * w_h)

#ifdef _WIN32
extern DWORD* screen_buffer;
#endif

#endif //BPENGINE_GLOBALS_H