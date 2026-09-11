#include "bpengine.h"

int main(void) {
    bp_r_init_window(960, 600, "Test", BP_SF_DEFAULT);

    bp_im_init();

    while (bp_r_poll_events()) {
        bp_im_begin();

        bp_r_set_draw_color(255, 255, 255);
        bp_r_clear_window();

        bp_r_update_window();
    }

    return 0;
}