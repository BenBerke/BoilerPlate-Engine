#include "bpengine.h"

int main(void) {
    bpe_init(960, 600, "BoilerPlate2D", BP_SF_DEFAULT, 4096);

    bool running = true;
    while (running) {
        running = bpe_update();

        bp_r_set_draw_color(255, 255, 255);
        bp_r_clear_window();

        bp_r_update_window();

        // Draw something here
    }

    return 0;
}