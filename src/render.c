#include "render.h"
#include "model.h"

#include <assert.h>
#include <stdlib.h>
#include <math.h>

static ColorRGBA red = {.r = 255, .g = 0, .b = 0, .a = 255};
static ColorRGBA green = {.r = 0, .g = 255, .b = 0, .a = 255};
static ColorRGBA blue = {.r = 0, .g = 0, .b = 255, .a = 255};
static ColorRGBA white = {.r = 255, .g = 255, .b = 255, .a = 255};
static ColorRGBA black = {.r = 0, .g = 0, .b = 0, .a = 255};

static void clear(u8 *frame_buffer, u32 frame_buffer_len, ColorRGBA color) {
    for (u32 i = 0; i < frame_buffer_len; i += 4) {
        frame_buffer[i] = color.r;
        frame_buffer[i+1] = color.g;
        frame_buffer[i+2] = color.b;
        frame_buffer[i+3] = color.a;
    }
}

static void drawPixel(u8 *frame_buffer, i32 width, i32 height, i32 x, i32 y, ColorRGBA color) {
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);
    u32 i = 4 * (y * width + x);
    frame_buffer[i] = color.r;
    frame_buffer[i+1] = color.g;
    frame_buffer[i+2] = color.b;
    frame_buffer[i+3] = color.a;
}

static void drawLine(u8 *frame_buffer, i32 width, i32 height, i32 ax, i32 ay, i32 bx, i32 by, ColorRGBA color) {
#if 0
    for (float t = 0.0f; t < 1.0f; t += 0.02f) {
        u32 x = (u32)((float)ax + t * ((float)bx - (float)ax));
        u32 y = (u32)((float)ay + t * ((float)by - (float)ay));
        drawPixel(frame_buffer, width, height, x, y, color);
    }
#else
    bool32 steep = abs(ax-bx) < abs(ay-by);
    if (steep) {
        SWAP(i32, ax, ay);
        SWAP(i32, bx, by);
    }
    if (ax > bx) {
        SWAP(i32, ax, bx);
        SWAP(i32, ay, by);
    }
    for (i32 x = ax; x < bx; x++) {
        float t = ((float)(x-ax)) / ((float)(bx-ax));
        i32 y = (u32)roundf(((float)ay + ((float)(by-ay)) * t));
        if (steep) {
            drawPixel(frame_buffer, width, height, y, x, color);
        } else {
            drawPixel(frame_buffer, width, height, x, y, color);
        }
    }
#endif
}

void UpdateAndRender(u8 *frame_buffer, u32 frame_buffer_len, i32 width, i32 height, GameState *state) {
    (void)state;

    assert(width * height * 4 <= frame_buffer_len);

    clear(frame_buffer, frame_buffer_len, black);

    drawLine(frame_buffer, width, height, 7, 3, 62, 53, red);
    drawLine(frame_buffer, width, height, 7, 3, 12, 37, blue);
    drawLine(frame_buffer, width, height, 12, 37, 62, 53, green);

    drawPixel(frame_buffer, width, height, 7, 3, white);
    drawPixel(frame_buffer, width, height, 12, 37, white);
    drawPixel(frame_buffer, width, height, 62, 53, white);
}
