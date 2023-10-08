#include "../include/myClass.hpp"
#include "../include/BuddyAllocator.hpp"

MyClass::MyClass(int x_val, double y_val, const std::string& z_val) 
    : x(x_val), y(y_val), z(z_val) {}

int MyClass::getValueX() const {
    return x;
}

double MyClass::getValueY() const {
    return y;
}

std::string MyClass::getValueZ() const {
    return z;
}

void MyClass::setValueX(int new_x) {
    x = new_x;
}

void MyClass::setValueY(double new_y) {
    y = new_y;
}

void MyClass::setValueZ(const std::string& new_z) {
    z = new_z;
}

double MyClass::computeProduct() const {
    return x * y;
}

bool MyClass::isEqual(const MyClass& other) const {
    return x == other.x && y == other.y && z == other.z;
}

void* MyClass::operator new(size_t size) {
    static BuddyAllocator myAllocator(1024 * 1024, 32);  // 1 MB memory pool
    return myAllocator.allocate(size);
}

void MyClass::operator delete(void* ptr) {
    static BuddyAllocator myAllocator(1024 * 1024, 32);
    myAllocator.deallocate(ptr);
}