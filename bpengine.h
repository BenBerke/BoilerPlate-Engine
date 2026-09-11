//
// Created by berke on 9/11/2026.
//

#ifndef BPENGINE_BPENGINE_H
#define BPENGINE_BPENGINE_H

#include "headers/mmu.h"

#ifndef BP_NO_MAIN
#define main user_main
#endif

#ifndef BP_NO_WINDOW
#include "headers/renderer.h"
#include "headers/input_manager.h"
#endif

#ifndef BP_NO_TIME
#include "headers/time.h"
#endif

#ifndef BP_NO_DEBUG
#include "headers/debug.h"
#endif

#ifndef BP_NO_HELPER
static inline void bpe_init(
    const int window_width, const int window_height, const char* title, const unsigned long window_flags,
    const unsigned long long app_mem_size) {
    bp_mmu_init(app_mem_size);

#ifndef BP_NO_WINDOW
    bp_r_init_window(window_width, window_height, title, window_flags);
    bp_im_init();
#endif
}
#endif

#endif //BPENGINE_BPENGINE_H