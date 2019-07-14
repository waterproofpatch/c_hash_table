// exists only to allow mocking of malloc, free

/**
  @brief wrapper for free
  @param size: number of bytes to allocate
  @return: ptr to allocated memory
*/
void* wrap_malloc(size_t size);

/**
  @brief wrapper for free
  @param ptr: the ptr returned from wrap_malloc
*/
void wrap_free(void* ptr);
