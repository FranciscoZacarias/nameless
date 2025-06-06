
internal Arena* arena_init() {
  Arena* arena = arena_init_sized(ARENA_RESERVE_SIZE, ARENA_COMMIT_SIZE);
  return arena;
}

internal Arena* arena_init_sized(u64 reserve, u64 commit) {
  void* memory = NULL;
  
  u64 page_size = memory_get_page_size();
  reserve = AlignPow2(reserve, page_size);
  commit  = AlignPow2(commit,  page_size);
  
  Assert(ARENA_HEADER_SIZE < commit && commit <= reserve);
  
  memory  = memory_reserve(reserve);
  if(!memory_commit(memory, commit)) {
    memory = NULL;
    memory_release(memory, reserve);
  }
  
  Arena* arena = (Arena*) memory;
  
  if (arena) {
    arena->reserved    = reserve;
    arena->commited    = commit;
    arena->commit_size = commit;
    arena->position    = ARENA_HEADER_SIZE;
    arena->align       = DEFAULT_ALIGNMENT;
  } else {
    ERROR_MESSAGE_AND_EXIT("Error setting arena's memory");
  }
  
  return arena;
}

internal void* arena_push(Arena* arena, u64 size) {
  void* result = arena_push_no_zero(arena, size);
  MemoryZero(result, size);
  return result;
}

internal void* arena_push_no_zero(Arena* arena, u64 size) {
  void *result = NULL;

  if (arena->position + size <= arena->reserved) {
    u64 position_memory = AlignPow2(arena->position, arena->align);
    u64 new_position    = position_memory + size;
  
    if (arena->commited < new_position) {
      u64 commit_aligned = AlignPow2(new_position, arena->commit_size);
      u64 commit_clamped = ClampTop(commit_aligned, arena->reserved);
      u64 commit_size    = commit_clamped - arena->commited;
      if (memory_commit((u8*)arena + arena->commited, commit_size)) {
        arena->commited = commit_clamped;
      } else {
        ERROR_MESSAGE_AND_EXIT("Could not commit memory when increasing the arena's committed memory.");
      }
    }
    result = (u8*)arena + position_memory;
    arena->position = new_position;
  } else {
    ERROR_MESSAGE_AND_EXIT("Trying to allocate too much memory to a non dynamic arena.\nSize: %llu\nArena->Position: %llu\nArena->reserved: %llu\nArena->Position+Size: %llu", size, arena->position, arena->reserved, arena->position+size);
  }
  
  return result;
}

internal void  arena_pop(Arena* arena, u64 size) {
  if (size > arena->position) {
    printf("Warning :: Arena :: Trying to pop %lld bytes from arena with %lld allocated. Will pop %lld instead of %lld.\n", size, arena->position, arena->position, size);
    size = arena->position;
  }
  arena->position -= size;
}

internal void  arena_pop_to(Arena* arena, u64 pos) {
  if (pos > arena->reserved) {
    printf("Warning :: Arena :: Trying to pop over arena's reserved. Will pop only to %lld instead of %lld", arena->reserved, pos);
    pos = arena->reserved;
  } else if (pos < 0) {
    printf("Warning :: Arena :: Trying to pop negative values. Will pop to 0");
    pos = 0;
  }
  arena->position = pos;
}

internal void  arena_clear(Arena* arena) {
  arena_pop(arena, arena->position);
}

internal void  arena_free(Arena* arena) {
  memory_release((u8*)arena, arena->reserved);
}

internal void print_arena(Arena *arena, const char8* label) {
  f32 committed_percentage = ((f64)arena->position / arena->commited) * 100.0f;
  f32 reserved_percentage  = ((f64)arena->position / arena->reserved) * 100.0f;
  printf("%s: Arena { reserved: %llu, commited: %llu, commit_size: %llu, position: %llu, align: %llu, committed_percentage: %.2f%%, reserved_percentage: %.2f%% }\n",
         label, arena->reserved, arena->commited, arena->commit_size, arena->position, arena->align, committed_percentage, reserved_percentage);
}

internal Arena_Temp arena_temp_begin(Arena* arena) {
  Arena_Temp temp;
  temp.arena = arena;
  temp.temp_position = arena->position;
  return temp;
}

internal void arena_temp_end(Arena_Temp* temp) {
  arena_pop_to(temp->arena, temp->temp_position);
}
