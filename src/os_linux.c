#include "core.h"
#include "os.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

u64 os_get_pagesize(void) {
  return (u64)sysconf(_SC_PAGESIZE);
}

void *os_mem_reserve(u64 size) {
  runtime_assert(size > 0);
  void *buffer =
      mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  runtime_assert(buffer != MAP_FAILED);
  runtime_assert((u64)buffer % os_get_pagesize() == 0);
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

Str8 os_read_entire_file(Arena *arena, Str8 file_path) {
  Str8 res = {0};
  Arena *scratch = get_scratch(arena);
  i32 fd = open(cstr(scratch, file_path), O_RDONLY);
  runtime_assert(fd != -1);

  i64 file_size = lseek(fd, 0, SEEK_END);
  runtime_assert(file_size != -1);

  lseek(fd, 0, SEEK_SET);

  res.data = arena_push(arena, (u64)file_size, align_of(u8));
  res.len = (u64)file_size;

  i64 bytes_read = read(fd, res.data, res.len);
  runtime_assert(bytes_read == (i64)res.len);
  close(fd);
  free_scratch(scratch);
  return res;
}
