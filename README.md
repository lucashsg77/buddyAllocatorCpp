
# Buddy Memory Allocator

The Buddy Allocator is a memory allocation system that divides memory into partitions to satisfy memory requests. This allocator is efficient in terms of both time and space due to its ability to split and combine memory blocks.

## Table of Contents

- [Features](#features)
- [Getting Started](#getting-started)
  - [Folder Layout](#folder-layout)
  - [Compilation](#compilation)
  - [Visual Studio Setup](#visual-studio-setup)
- [Example Usages](#example-usages)
- [Performance Considerations](#performance-considerations)
- [Pros and Cons](#pros-and-cons)
- [Conclusion](#conclusion)

## Features

- **Power-of-Two Block Sizes**: All allocated blocks are of sizes that are powers of two, which simplifies the split and merge operations.
- **Coalescing**: When blocks are freed, the Buddy Allocator automatically merges them with their "buddy" blocks if they're also free.
- **Thread Safety**: The allocator is thread-safe, ensuring that multiple threads can allocate and deallocate memory without issues.
- **Fixed Overhead**: Block metadata is stored directly within the block, ensuring a consistent overhead.

## Getting Started

### Folder Layout

```
BuddyAllocator/
|-- src/
|   |-- BuddyAllocator.cpp  # Implementation of the Buddy Allocator
|   `-- BuddyAllocator.h    # Header file with the Buddy Allocator class declaration
|-- tests/
|   `-- test.cpp            # Test cases for the Buddy Allocator
`-- README.md
```

### Compilation

For optimal performance, compile with optimizations enabled:

```bash
g++ -O3 -g BuddyAllocator.cpp -o output
```

### Visual Studio Setup

1. Create a new Empty Project in Visual Studio.
2. Add `BuddyAllocator.cpp` and `BuddyAllocator.h` to the project's source files.
3. Set the optimization level to `/O2` for best performance in the project properties.
4. Build and run the project.

## Example Usages

### Basic Allocation and Deallocation

```cpp
#include "BuddyAllocator.h"

int main() {
    BuddyAllocator allocator(1024, 32); // 1KB total memory, 32 bytes minimum block size
    void* memoryBlock = allocator.allocate(100); // Allocate 100 bytes.
    // ... use the memory ...
    allocator.deallocate(memoryBlock);
    return 0;
}
```

### Using with Custom Classes

```cpp
#include "BuddyAllocator.h"

class MyClass {
public:
    MyClass() { /* ... */ }
    void* operator new(size_t size) {
        static BuddyAllocator myAllocator(1024 * 1024, 32);
        return myAllocator.allocate(size);
    }
    void operator delete(void* ptr) {
        static BuddyAllocator myAllocator(1024 * 1024, 32);
        myAllocator.deallocate(ptr);
    }
};

int main() {
    MyClass* object = new MyClass();
    // ... use the object ...
    delete object;
    return 0;
}
```

## Performance Considerations

For low counts of allocations (e.g., less than 200k), the Buddy Allocator might sometimes be slower than the standard `new` and `delete` operations due to:

1. **Overhead of Block Management**: Maintains free lists and performs block splitting/coalescing.
2. **Memory Wastage**: Rounds up sizes to the nearest power of two.
3. **Initial Memory Pooling**: Sets up an initial memory pool.
4. **Standard Library Optimizations**: Modern C++ standard libraries have highly optimized memory allocators tailored for general use.

For larger numbers of allocations or specialized use cases, the Buddy Allocator can provide more predictable performance and reduced fragmentation.

## Pros and Cons

**Pros**:
- **Reduced Fragmentation**: The allocator's coalescing feature reduces external fragmentation.
- **Predictable Performance**: Allocation and deallocation times are generally constant.
- **Simple Free Block Management**: Uses a free list for each block size, simplifying block management.

**Cons**:
- **Space Overhead**: Memory can be wasted if the requested size is much smaller than the allocated block size.
- **Possible Slower Performance**: For small numbers of allocations, the overhead might outweigh the benefits.

## Conclusion

The Buddy Allocator is an efficient memory allocation system suitable for scenarios requiring a large number of memory allocations/deallocations. While it may not always outperform the default allocator, especially for small workloads, it can be extremely beneficial in specific contexts, such as real-time systems or memory-constrained environments.
