
#include "../include/BuddyAllocator.hpp"

    inline size_t BuddyAllocator::indexFromBlockSize(size_t blockSize) const {
        return __builtin_ctz(blockSize) - __builtin_ctz(minBlockSize);
    }

    inline bool BuddyAllocator::isAddressWithinBounds(uintptr_t address) const {
        uintptr_t start = reinterpret_cast<uintptr_t>(memoryStart);
        uintptr_t end = start + totalMemorySize;
        return address >= start && address < end;
    }

    // Free list management
    inline void BuddyAllocator::removeFromFreeList(Block* block, size_t index) {
        if (block->prev) {
            block->prev->next = block->next;
        } else {
            freeLists[index] = block->next;
        }
        if (block->next) {
            block->next->prev = block->prev;
        }
        block->prev = nullptr;
        block->next = nullptr;
    }

    inline void BuddyAllocator::addToFreeList(Block* block, size_t index) {
        block->next = freeLists[index];
        if (freeLists[index]) {
            freeLists[index]->prev = block;
        }
        freeLists[index] = block;
    }

    // Coalescing logic
    void BuddyAllocator::coalesce(Block* block) {
        size_t index = indexFromBlockSize(block->size);
    
        while (index < freeLists.size() - 1 && block->size < totalMemorySize) {
            uintptr_t blockAddress = reinterpret_cast<uintptr_t>(block);
            uintptr_t buddyAddress = blockAddress ^ block->size;
    
            if (!isAddressWithinBounds(buddyAddress)) {
                return;  // Out of bounds address detected
            }
    
            Block* buddy = reinterpret_cast<Block*>(buddyAddress);
    
            if (reinterpret_cast<char*>(block) + block->size >= memoryStart + totalMemorySize) {
                break;  // Reached the end of memory
            }
    
            // Check if the buddy block is the right size and if it's in the free list
            if (buddy->size != block->size || (buddy->prev == nullptr && freeLists[index] != buddy)) {
                break;  // No suitable buddy found
            }
    
            removeFromFreeList(buddy, index);
            block = (reinterpret_cast<char*>(block) < reinterpret_cast<char*>(buddy)) ? block : buddy;
            block->size <<= 1;
            ++index;
        }
    
        addToFreeList(block, indexFromBlockSize(block->size));
    }

    BuddyAllocator::BuddyAllocator(size_t totalSize, size_t minimumBlockSize)
        : totalMemorySize(totalSize), minBlockSize(minimumBlockSize) {

        memoryStart = new char[totalMemorySize];
        size_t listCount = __builtin_ctz(totalSize) - __builtin_ctz(minimumBlockSize) + 1;
        freeLists.resize(listCount, nullptr);

        Block* initialBlock = reinterpret_cast<Block*>(memoryStart);
        initialBlock->size = totalSize;
        initialBlock->prev = nullptr;
        initialBlock->next = nullptr;
        freeLists[listCount - 1] = initialBlock;
    }

    BuddyAllocator::~BuddyAllocator() {
        delete[] memoryStart;
    }

    void* BuddyAllocator::allocate(size_t size) {
        std::lock_guard<std::mutex> lock(allocatorMutex);
        if (size < minBlockSize) size = minBlockSize;
        if (size & (size - 1)) {  // Check if size is not already a power of two
            size = 1 << (1 + (size_t)std::log2(size));
        }

        size_t listIndex = indexFromBlockSize(size);
        for (size_t i = listIndex; i < freeLists.size(); ++i) {
            if (freeLists[i]) {
                Block* block = freeLists[i];
                removeFromFreeList(block, i);

                block->prev = nullptr;  // Initialize block's prev and next
                block->next = nullptr;
                
                // Split the block until we get a block of the desired size
                while (block->size > size) {
                    size_t halfSize = block->size >> 1;
                    Block* buddy = reinterpret_cast<Block*>(reinterpret_cast<char*>(block) + halfSize);
                    buddy->size = halfSize;
                    buddy->prev = nullptr;  // Initialize buddy's prev and next
                    buddy->next = nullptr;
                    addToFreeList(buddy, indexFromBlockSize(halfSize));
                    block->size = halfSize;
                }

                return reinterpret_cast<void*>(block);
            }
        }

        return nullptr;  // Allocation failed
    }

    void BuddyAllocator::deallocate(void* ptr) {
        std::lock_guard<std::mutex> lock(allocatorMutex);
        if (!ptr) return;
        Block* block = static_cast<Block*>(ptr);
        size_t index = indexFromBlockSize(block->size);
        if (block->prev == nullptr && freeLists[index] != block) {
            // This block is not in the free list, so it's safe to coalesce
            coalesce(block);
        }
    }
