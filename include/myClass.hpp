#ifndef MY_CLASS_HPP
#define MY_CLASS_HPP
#include <string>
#include <iostream>
class MyClass {
private:
    int x;
    double y;
    std::string z;

public:
    MyClass(int x_val, double y_val, const std::string& z_val);
    void* operator new(size_t size);
    void operator delete(void* ptr);

    // Newly added member functions
    int getValueX() const;
    double getValueY() const;
    std::string getValueZ() const;
    void setValueX(int new_x);
    void setValueY(double new_y);
    void setValueZ(const std::string& new_z);
    double computeProduct() const;
    bool isEqual(const MyClass& other) const;
};
#endif