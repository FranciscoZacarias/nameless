#ifndef F_WIN32_H
#define F_WIN32_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <userenv.h>

internal void win32_init(void);

//~ Memory
internal void* memory_reserve(u64 size);
internal b32   memory_commit(void* memory, u64 size);
internal void  memory_decommit(void* memory, u64 size);
internal void  memory_release(void* memory, u64 size);

internal u64 memory_get_page_size();

//~ Threading
typedef u64 thread_func(void* context); 

typedef struct Thread {
  u64 v[1];
} Thread;

internal Thread thread_create(thread_func* start, void* context);
internal void   thread_wait_for_join(Thread* other);
internal void   thread_wait_for_join_all(Thread** threads, u32 count);
internal void   thread_wait_for_join_any(Thread** threads, u32 count);

//~ File handling
internal b32    file_create(String file_name);
internal b32    file_exists(String file_name);
internal u32    file_write(String file_name, u8* data, u64 data_size);
internal u32    file_size(String file_name);
internal String file_load(Arena* arena, String file_name);
internal u64    file_get_last_modified_time(String file_name);

//~ Logging 
internal void print_string(String string); // TODO(fz): This should be abstracted into a more generic win32_print that then String can use to implement it's own print_string

//~ Error
internal void error_message(const char *fmt, ...); // TODO(fz): We need a more refined error system;

#endif // F_WIN32_H