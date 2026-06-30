#pragma once

#include "core.h"

static u64 os_get_pagesize(void);

static void *os_mem_reserve(u64 size);
static void os_mem_commit(void *base, u64 size);
static void os_mem_decommit(void *base, u64 size);
static void os_mem_free(void *base, u64 size);

static Str8 os_read_entire_file(Arena *arena, Str8 file_path);

static u64 os_get_usec(void);
