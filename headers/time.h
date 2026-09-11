//
// Created by berke on 9/11/2026.
//

#ifndef BPENGINE_TIME_H
#define BPENGINE_TIME_H

#include "typedefs.h"

static u64 time_in_ns;

u64 g_get_time_ns();
u64 g_get_time_tick();

#endif //BPENGINE_TIME_H