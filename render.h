#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool iscool;
} GameState;

typedef union{
    struct {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };
    uint8_t vals[4];
} ColorRGBA;


void render(uint8_t *frame_buffer, uint32_t frame_buffer_len, uint32_t width, uint32_t height, GameState *state);
