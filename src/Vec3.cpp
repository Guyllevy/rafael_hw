
#include <cmath>
#include <iostream>
#include "../include/Vec3.hpp"

Vec3::Vec3(double x, double y, double z) : x(x), y(y), z(z) {};

// Addition
Vec3 Vec3::operator+(const Vec3& other) const {
    return Vec3(x + other.x, y + other.y, z + other.z);
}

// Subtraction
Vec3 Vec3::operator-(const Vec3& other) const {
    return Vec3(x - other.x, y - other.y, z - other.z);
}

// Scalar multiplication
Vec3 Vec3::operator*(double scalar) const {
    return Vec3(x * scalar, y * scalar, z * scalar);
}

// Dot product
double Vec3::dot(const Vec3& other) const {
    return x * other.x + y * other.y + z * other.z;
}

// Cross product
Vec3 Vec3::cross(const Vec3& other) const {
    return Vec3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}

double Vec3::mag() const{
    return sqrt(this->dot(*this));
}

Vec3 Vec3::normalize() const{
    return (*this) * (1 / this->mag());
}

Vec3 Vec3::rotateZ(double theta){
    return Vec3(x * cos(theta) - y * sin(theta),
                x * sin(theta) + y * cos(theta),
                z);
}

double Vec3::angle(const Vec3& other) const{
    return acos(this->normalize().dot(other.normalize()));
}

void Vec3::show() const{
    std::cout << "(" <<
    x << ", " <<
    y << ", " <<
    z << ")" << std::endl;
}
