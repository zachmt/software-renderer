#include "core.h"

static void rng_seed(u32 seed) {
    srand(seed);
}

static i32 rng_generate_i32(void) {
    return rand();
}

static f32 rng_generate_01(void) {
    return ((f32)(rand()) / (f32)RAND_MAX);
}
