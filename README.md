# Custom Memory allocators

MemoryAllocators is a simple memory allocator that implements `malloc()`, `calloc()`, `realloc()` and `free()`.

The file `sbrkmemallocator.h` uses `sbrk()` to implement memory allocation functions.

The file `mmapallocator.h` uses `mmap()` to implement memory allocation functions.

# Run code

Just include headers and use their functions in your code!

# See also

* [Tutorial](https://arjunsreedharan.org/post/148675821737/memory-allocators-101-write-a-simple-memory)
