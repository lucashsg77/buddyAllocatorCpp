#ifndef TESTS_HPP
#define TESTS_HPP
#include <iostream>
#include <cmath>
#include <chrono>
#include <exception>
#include <random>
#include <cassert>
#include <algorithm>
#include "../include/BuddyAllocator.hpp"

// Utility function declarations
void detailedAssert(bool condition, const std::string& message);

// Test function declarations
void testPerformance();
void testBoundaryCases();
void testRepeatedAllocations();
void testFunctionality();
void testFragmentation();
void testRandomized();
void testBoundaryChecks();
void testStressTest();
void testCustomClassAllocation();

#endif