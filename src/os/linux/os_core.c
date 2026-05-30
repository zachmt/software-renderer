#include "os/os_core.h"

#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>

u64 os_get_pagesize(void) {
    return (u64)sysconf(_SC_PAGESIZE);
}

void *os_mem_reserve(u64 size) {
    runtime_assert(size > 0);
    void *buffer = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    runtime_assert(buffer != MAP_FAILED);
    return buffer;
}

void os_mem_commit(void *base, u64 size) {
    runtime_assert((u64)base % os_get_pagesize() == 0);
    i32 res = mprotect(base, size, PROT_READ | PROT_WRITE);
    runtime_assert(res == 0);
}

void os_mem_decommit(void *base, u64 size) {
    runtime_assert((u64)base % os_get_pagesize() == 0);
    i32 res = madvise(base, size, MADV_DONTNEED);
    runtime_assert(res == 0);
    res = mprotect(base, size, PROT_NONE);
    runtime_assert(res == 0);
}

void os_mem_free(void *base, u64 size) {
    i32 res = munmap(base, size);
    runtime_assert(res == 0);
}
