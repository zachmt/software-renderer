#pragma once

#include "core.h"

u64 os_get_pagesize(void);

void *os_mem_reserve(u64 size);
void os_mem_commit(void *base, u64 size);
void os_mem_decommit(void *base, u64 size);
void os_mem_free(void *base, u64 size);

Str8 os_read_entire_file(Arena *arena, Str8 file_path);

u64 os_get_usec(void);
