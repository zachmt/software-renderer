#include "render.h"

static void clear(uint8_t *frame_buffer, uint32_t frame_buffer_len, ColorRGBA color) {
    for (uint32_t i = 0; i < frame_buffer_len; i += 4) {
        frame_buffer[i] = color.r;
        frame_buffer[i+1] = color.g;
        frame_buffer[i+2] = color.b;
        frame_buffer[i+3] = color.a;
    }
}

void render(uint8_t *frame_buffer, uint32_t frame_buffer_len, uint32_t width, uint32_t height, GameState *state) {
    assert(width * height * 4 <= frame_buffer_len);
    ColorRGBA c;
    c.r = 0;
    c.g = 255;
    c.b = 0;
    c.a = 200;
    clear(frame_buffer, frame_buffer_len, c);
}
