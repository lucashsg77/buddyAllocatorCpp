
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

## Projects and Use Cases

### 1. **Real-time Systems**
Real-time systems, such as those in automotive or aerospace applications, require predictable memory management performance. The Buddy Allocation system can provide consistent allocation and deallocation times, making it a suitable choice for such applications.

### 2. **Memory-Constrained Embedded Systems**
In systems where memory is a scarce resource, efficient utilization of available memory is critical. The Buddy Allocation system can help reduce fragmentation, ensuring maximum use of available memory.

### 3. **Gaming and Graphics**
In video game development, especially in the realm of graphics, dynamic memory allocation is a frequent operation. The Buddy Allocation system can help in managing memory pools for objects like textures, shaders, and meshes.

### 4. **Database Systems**
Databases often require dynamic memory allocation for operations like indexing, caching, and query processing. Using a memory management system like Buddy Allocator can ensure efficient and predictable performance.

### 5. **Networking**
In network programming, especially in high-performance servers, handling a massive number of simultaneous connections can necessitate dynamic memory allocation. The Buddy Allocation system can be used to manage connection buffers, packets, and other network-related structures.

### 6. **Operating Systems**
The Buddy Allocation algorithm itself originated in the context of operating system design. It can be used in OS development for kernel memory allocation.

### 7. **Simulations**
In scientific simulations, there are scenarios where objects or entities are created and destroyed dynamically. The Buddy Allocation system can be used to manage memory in such simulations efficiently.

### 8. **Custom Memory Allocators for Libraries**
Developers can use the Buddy Allocation system to build custom memory allocators tailored for specific data structures or algorithms, ensuring optimal performance.

## Actual Cases of Using Buddy Allocation:

### 1. **Linux Kernel**
The Linux kernel has historically used a form of the Buddy Allocation system for page allocation, ensuring efficient use of memory with reduced fragmentation.

### 2. **High-Performance Computing (HPC)**
In HPC environments where performance is paramount, custom memory allocators based on Buddy Allocation have been developed to cater to specific workloads.

### 3. **Embedded Systems**
Many embedded systems, especially those that run real-time operating systems (RTOS), use Buddy Allocation or similar algorithms to manage memory efficiently.

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

## Compilation

### Compiling the BuddyAllocator as a static library:
```
$ make libbuddy.a
```

### Compiling the BuddyAllocator as a shared library:
```
$ make libbuddy.so
```

### Compiling and running the tests against the static library:
```
$ make test_static
$ ./tests/test
```

### Compiling and running the tests against the shared library:
```
$ make test_shared
$ ./tests/test
```

## Integration

### Linking the static library:
To link the BuddyAllocator's static library with your project, you need to include the path to the `libbuddy.a` and the include path in your compile and link commands. For instance:

```
$ g++ your_source_file.cpp -Ipath_to_buddy_allocator_include_folder -Lpath_to_buddy_allocator_lib_folder -lbuddy
```

### Linking the shared library:
To link the BuddyAllocator's shared library, do the following:

```
$ g++ your_source_file.cpp -Ipath_to_buddy_allocator_include_folder -Lpath_to_buddy_allocator_lib_folder -lbuddy
```

Make sure to have the directory containing `libbuddy.so` in your `LD_LIBRARY_PATH` or use `rpath` or `runpath`.

### Clean Build Artifacts:
To clean all build artifacts, use:
```
$ make clean
```

## Visual Studio Setup

1. Create a new Empty Project in Visual Studio.
2. Add `BuddyAllocator.cpp` and `BuddyAllocator.hpp` to the project's source files.
3. Set the optimization level to `/O2` for best performance in the project properties.
4. Build and run the project.

## Example Usages

### Basic Allocation and Deallocation

```cpp
#include "BuddyAllocator.hpp"

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
#include "BuddyAllocator.hpp"

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
