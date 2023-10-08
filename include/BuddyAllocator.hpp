#ifndef BUDDY_ALLOCATOR_HPP
#define BUDDY_ALLOCATOR_HPP
#include <cstddef>
#include <vector>
#include <mutex>
#include <cmath>
class BuddyAllocator {
private:
    struct Block {
        size_t size;
        Block* prev;
        Block* next;
    };

    std::vector<Block*> freeLists;
    char* memoryStart;
    size_t totalMemorySize;
    size_t minBlockSize;
    std::mutex allocatorMutex;

    inline size_t indexFromBlockSize(size_t blockSize) const;
    inline bool isAddressWithinBounds(uintptr_t address) const;
    inline void removeFromFreeList(Block* block, size_t index);
    inline void addToFreeList(Block* block, size_t index);
    void coalesce(Block* block);

public:
    BuddyAllocator(size_t totalSize, size_t minimumBlockSize);
    ~BuddyAllocator();
    void* allocate(size_t size);
    void deallocate(void* ptr);
};
#endif