#include <iostream>
#include <cmath>
#include <chrono>
#include <exception>
#include <random>
#include <cassert>
#include <algorithm>
#include "../include/BuddyAllocator.hpp"
#include "../include/tests.hpp"
#include "../include/myClass.hpp"
void detailedAssert(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "Assertion failed: " << message << std::endl;
        throw std::runtime_error("Assertion failed: " + message);
    }
}

void testPerformance() {
    const size_t numAllocations = 1500000;
    const size_t numTrials = 1000;
    std::cout << "Running Performance tests, this test will make " << numAllocations << " allocs for " << numTrials << " times" << std::endl;
    std::vector<void*> pointers;
    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<size_t> sizeDist(1, 128);  // Random sizes between 1 and 128 bytes

    BuddyAllocator allocator(1024 * 1024, 32);
    std::vector<double> buddyTimes;
    std::vector<double> newDeleteTimes;

    for (size_t trial = 0; trial < numTrials; ++trial) {
        pointers.clear();
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < numAllocations; ++i) {
            pointers.push_back(allocator.allocate(sizeDist(g)));
        }
        std::shuffle(pointers.begin(), pointers.end(), g);  // Randomize the deallocation order
        for (size_t i = 0; i < numAllocations; ++i) {
            allocator.deallocate(pointers[i]);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff = end - start;
        buddyTimes.push_back(diff.count());

        pointers.clear();

        start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < numAllocations; ++i) {
            pointers.push_back(new char[sizeDist(g)]);
        }
        for (size_t i = 0; i < numAllocations; ++i) {
            delete[] static_cast<char*>(pointers[i]);
        }
        end = std::chrono::high_resolution_clock::now();
        diff = end - start;
        newDeleteTimes.push_back(diff.count());
    }
    
    std::cout << "Completed all trials in testPerformance" << std::endl;

    double avgBuddy = std::accumulate(buddyTimes.begin(), buddyTimes.end(), 0.0) / numTrials;
    double avgNewDelete = std::accumulate(newDeleteTimes.begin(), newDeleteTimes.end(), 0.0) / numTrials;

    std::cout << "Average BuddyAllocator time (over " << numTrials << " trials): " << avgBuddy << " seconds" << std::endl;
    std::cout << "Average new and delete time (over " << numTrials << " trials): " << avgNewDelete << " seconds" << std::endl;

    double speedUp = avgNewDelete / avgBuddy;
    std::cout << "BuddyAllocator has approximately " << speedUp << " times the speed of std new and delete." << std::endl;
}

void testFunctionality() {
    std::cout << "Running functionality tests..." << std::endl;

    BuddyAllocator allocator(1024, 32);
    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<size_t> powerDist(0, 4);  // Random powers for 2^x where x is between 0 and 4

    size_t sizeToAllocate = 1 << powerDist(g);
    void* ptr1 = allocator.allocate(sizeToAllocate);
    detailedAssert(ptr1, "Random allocation of size " + std::to_string(sizeToAllocate) + " bytes failed");

    sizeToAllocate = 1 << powerDist(g);
    void* ptr2 = allocator.allocate(sizeToAllocate);
    detailedAssert(ptr2, "Random allocation of size " + std::to_string(sizeToAllocate) + " bytes failed");

    // Demonstrating coalescing: deallocate in reverse order
    allocator.deallocate(ptr2);
    allocator.deallocate(ptr1);

    sizeToAllocate = 1 << powerDist(g);
    void* ptr3 = allocator.allocate(sizeToAllocate);
    detailedAssert(ptr3, "Random allocation of size " + std::to_string(sizeToAllocate) + " bytes failed");

    sizeToAllocate = 1 << powerDist(g);
    void* ptr4 = allocator.allocate(sizeToAllocate);
    detailedAssert(ptr4, "Random allocation of size " + std::to_string(sizeToAllocate) + " bytes failed");

    // Allocate almost all of the remaining memory to set up for the expected failure
    void* setupPtr = allocator.allocate(480);  // This will leave less than 512 bytes free
    detailedAssert(setupPtr, "Setup allocation of 480 bytes failed");

    void* ptr5 = allocator.allocate(512);  // This allocation should now fail
    detailedAssert(!ptr5, "Expected allocation of 512 bytes to fail but it succeeded");  // Note the assertion change

    // Deallocate remaining allocations to reset state
    allocator.deallocate(ptr3);
    allocator.deallocate(ptr4);
    allocator.deallocate(setupPtr);

    std::cout << "Functionality tests passed!" << std::endl;
}

void testBoundaryCases() {
    std::cout << "Running boundary tests..." << std::endl;

    BuddyAllocator allocator(1024, 32);

    void* ptr1 = allocator.allocate(1024);  // Allocate the entire memory
    detailedAssert(ptr1, "Allocation of entire memory (1024 bytes) failed");

    allocator.deallocate(ptr1);

    void* ptr2 = allocator.allocate(32);  // Allocate the smallest block
    detailedAssert(ptr2, "Allocation of minimum block (32 bytes) failed");

    allocator.deallocate(ptr2);

    std::cout << "Boundary tests passed!" << std::endl;
}

void testRepeatedAllocations() {
    std::cout << "Running repeated allocations tests..." << std::endl;

    BuddyAllocator allocator(1024 * 1024, 32);  // 1 MB of memory with a minimum block size of 32 bytes
    const size_t numAllocations = 10000000;
    const size_t blockSize = 512;  // Fixed block size for this test
    std::vector<void*> pointers;

    for (int trial = 0; trial < 10; ++trial) {
        // Allocate
        for (size_t i = 0; i < numAllocations; ++i) {
            void* ptr = allocator.allocate(blockSize);
            pointers.push_back(ptr);
        }

        // Deallocate
        for (size_t i = 0; i < numAllocations; ++i) {
            allocator.deallocate(pointers[i]);
        }
        pointers.clear();

    }

    std::cout << "Repeated allocations tests passed!" << std::endl;
}

void testFragmentation() {
    std::cout << "Running fragmentation tests..." << std::endl;

    BuddyAllocator allocator(1024, 32);

    // Fragment the memory into small blocks
    void* ptr1 = allocator.allocate(128);
    void* ptr2 = allocator.allocate(128);
    void* ptr3 = allocator.allocate(128);
    void* ptr4 = allocator.allocate(128);

    allocator.deallocate(ptr1);
    allocator.deallocate(ptr3);

    // Attempt to allocate a block larger than the fragmented free blocks
    void* ptr5 = allocator.allocate(256);
    detailedAssert(ptr5, "Fragmentation test failed");
    
    allocator.deallocate(ptr2);
    allocator.deallocate(ptr4);
    allocator.deallocate(ptr5);
    
    std::cout << "Fragmentation tests passed!" << std::endl;
}

void testRandomized() {
    std::cout << "Running randomized tests..." << std::endl;

    BuddyAllocator allocator(1024 * 1024, 32);
    std::vector<void*> pointers;
    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<size_t> sizeDist(1, 1024);
    std::uniform_int_distribution<int> actionDist(0, 1);

    for (int i = 0; i < 10000; ++i) {
        if (actionDist(g) || pointers.empty()) {
            size_t size = sizeDist(g);
            void* ptr = allocator.allocate(size);
            if (ptr) {
                pointers.push_back(ptr);
            }
        } else {
            int index = g() % pointers.size();
            allocator.deallocate(pointers[index]);
            std::swap(pointers[index], pointers.back());
            pointers.pop_back();
        }
    }

    for (void* ptr : pointers) {
        allocator.deallocate(ptr);
    }
    
    std::cout << "Randomized tests passed!" << std::endl;
}

void testBoundaryChecks() {
    std::cout << "Running boundary checks tests..." << std::endl;

    BuddyAllocator allocator(1024, 32);
    const size_t size = 512;
    void* ptr = allocator.allocate(size);
    detailedAssert(ptr, "Boundary checks allocation failed");

    char* charPtr = static_cast<char*>(ptr);  // Cast the void pointer to char pointer
    for (size_t i = 0; i < size; ++i) {
        charPtr[i] = 'A';
    }

    detailedAssert(charPtr[size - 1] == 'A', "Boundary checks write test failed");
    allocator.deallocate(ptr);

    std::cout << "Boundary checks tests passed!" << std::endl;
}

void testStressTest() {
    std::cout << "Running stress test..." << std::endl;

    BuddyAllocator allocator(1024 * 1024 * 1024, 32);  // 1 GB
    std::vector<void*> pointers;
    bool outOfMemory = false;

    while (!outOfMemory) {
        void* ptr = allocator.allocate(1024);  // 1 KB
        if (!ptr) {
            outOfMemory = true;
        } else {
            pointers.push_back(ptr);
        }
    }

    for (void* ptr : pointers) {
        allocator.deallocate(ptr);
    }
    
    std::cout << "Stress test passed!" << std::endl;
}

void testCustomClassAllocation() {
    std::cout << "Running custom class allocation tests..." << std::endl;

    MyClass* obj1 = new MyClass(5, 3.14, "Hello");
    MyClass* obj2 = new MyClass(10, 2.71, "World");
    
    // Modify and access values
    obj1->setValueX(15);
    obj2->setValueY(1.414);
    obj2->setValueZ("People");
    std::cout << "Obj1 - X: " << obj1->getValueX() << ", Y: " << obj1->getValueY() << ", Z: " << obj1->getValueZ() << std::endl;
    std::cout << "Obj2 - X: " << obj2->getValueX() << ", Y: " << obj2->getValueY() << ", Z: " << obj2->getValueZ() << std::endl;

    // Arithmetic operation
    std::cout << "Product of X and Y for Obj1: " << obj1->computeProduct() << std::endl;
    std::cout << "Product of X and Y for Obj2: " << obj2->computeProduct() << std::endl;

    // Comparison
    if (obj1->isEqual(*obj2)) {
        std::cout << "Obj1 and Obj2 are equal." << std::endl;
    } else {
        std::cout << "Obj1 and Obj2 are not equal." << std::endl;
    }

    delete obj1;
    delete obj2;

    std::cout << "Custom class allocation tests passed!" << std::endl;
}

// Update the main function to include the new tests
int main() {
        std::cout << "Starting main" << std::endl;
    try {
        testBoundaryCases();
        testRepeatedAllocations();
        testPerformance();
        testFunctionality();
        testFragmentation();
        testRandomized();
        testBoundaryChecks();
        testStressTest();
        testCustomClassAllocation();  // Add this line
    } catch (const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        return 1;
    }
    std::cout << "Exiting main" << std::endl;
    return 0;
}