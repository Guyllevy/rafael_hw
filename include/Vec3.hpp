
#ifndef VEC3_HPP
#define VEC3_HPP

#include <cmath>

class Vec3{
    public:
    double x;
    double y;
    double z;

    Vec3(double x = 0, double y = 0, double z = 0);

    // Addition
    Vec3 operator+(const Vec3& other) const;

    // Subtraction
    Vec3 operator-(const Vec3& other) const;

    // Scalar multiplication
    Vec3 operator*(double scalar) const;

    // Dot product
    double dot(const Vec3& other) const;

    // Cross product
    Vec3 cross(const Vec3& other) const;

    // magnitude
    double mag() const;

    Vec3 normalize() const;

    Vec3 rotateZ(double theta);

    double angle(const Vec3& other) const;

    void show() const;
    

};

#endif