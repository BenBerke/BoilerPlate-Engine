//
// Created by berke on 8/5/2026.
//

#ifndef MINIFB_RENDERER_H
#define MINIFB_RENDERER_H

#include "app.h"
#include "typedefs.h"
#include "config.h"

#ifdef _WIN32
#define BP_SF_DEFAULT WS_OVERLAPPEDWINDOW
#define BP_SF_BORDERLESS WS_POPUP
#define BP_SF_RESIZABLE WS_THICKFRAME
#define BP_SF_TITLE WS_CAPTION
#define BP_SF_MINIMIZABLE WS_MINIMIZEBOX
#define BP_SF_MAXIMIZABLE WS_MAXIMIZEBOX
#define BP_SF_SYSMENU WS_SYSMENU
#define BP_SF_VISIBLE WS_VISIBLE
#endif

typedef struct Texture {
    u32 *data;
    u32 w, h;
} Texture;

Texture png_to_bitstream(const char* filepath);

static int draw_color;

// r at the highest 8, b at the lowest 8
static inline int r_rgb_to_int(const unsigned char r, const unsigned char g, const unsigned char b) {return (r << 24) | (g << 16) | (b << 8);}

static inline void r_int_to_rgb(const unsigned int i, unsigned char* r, unsigned char* g, unsigned char* b) {
    *r = (i & 0xFF000000) >> 24;
    *g = (i & 0x00FF0000) >> 16;
    *b = (i & 0x0000FF00) >> 8;
}
static inline int cord_to_index(const int x, const int y) {return x + y * w_w;}

static inline void bp_r_set_draw_color(const unsigned char r, const unsigned char g, const unsigned char b) {
    draw_color = r_rgb_to_int(r, g, b);
}
static inline void bp_r_set_draw_color_int(const int color) {
    draw_color = color;
}

void bp_r_init_window(int w, int h, const char* title, unsigned long window_flags);
bool bp_r_poll_events();
void bp_r_update_window();

static inline void bp_r_clear_window() {
#ifdef _WIN32
    __stosd((unsigned long*)screen_buffer, draw_color, BP_SCREEN_BUFFER_SIZE);
#endif

}

// ==============
// Draw Functions
// ==============
static inline void bp_r_set_pixel(const int x, const int y) {
    if (x < 0 || x >= w_w || y < 0 || y >= w_h) return;
    screen_buffer[cord_to_index(x, y)] = draw_color;
}

void bp_r_draw_line(int x, int y, int x1, int y1);
void bp_r_draw_fill_rect(int x, int y, int w, int h);
void bp_r_draw_fill_circle(int x, int y, int half_r);

#endif //MINIFB_RENDERER_H