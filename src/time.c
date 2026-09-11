//
// Created by berke on 9/11/2026.
//

#include "../headers/app.h"
#include "../headers/time.h"

#ifdef _WIN32
u64 get_time_ns_win() {
    u64 ns;
    GetSystemTimePreciseAsFileTime((FILETIME*)&ns);
    return ns * 100;
}

u64 get_time_tick_win() {
    u64 t;
    QueryPerformanceCounter((LARGE_INTEGER*)&t);
    return t;
}

#endif

u64 g_get_time_ns() {
#ifdef _WIN32
    return get_time_ns_win();
#endif
}

u64 g_get_time_tick() {
#ifdef _WIN32
    return get_time_tick_win();
#endif
}