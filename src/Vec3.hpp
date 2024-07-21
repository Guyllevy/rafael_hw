#include <cmath>

class Vec3{
    public:
    double x;
    double y;
    double z;

    Vec3(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}

    // Addition
    Vec3 operator+(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    // Subtraction
    Vec3 operator-(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    // Scalar multiplication
    Vec3 operator*(double scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    // Dot product
    double dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // Cross product
    Vec3 cross(const Vec3& other) const {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    double mag() const{
        return sqrt(this->dot(*this));
    }

    Vec3 normalize() const{
        return (*this) * (1 / this->mag());
    }

    double angle(const Vec3& other) const{
        return acos(this->normalize().dot(other.normalize()));
    }
};