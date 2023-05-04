#pragma once

#include <Nucleus/Nucleus.h>

namespace Flux {

    using namespace Nucleus;

    struct Point {

        Point() = default;

        Point(const Float32 x, const Float32 y) : x(x), y(y) {}

        Point operator+(Point const& other) const {
            return { this->x + other.x, this->y + other.y };
        }

        Point operator-(Point const& other) const {
            return { this->x - other.x, this->y - other.y };
        }

        Point operator-() const {
            return { -this->x, -this->y };
        }

        Point operator*(Point const& other) const {
            return { this->x * other.x, this->y * other.y };
        }

        Point operator/(Point const& other) const {
            return { this->x / other.x, this->y / other.y };
        }

        Point& operator+=(Point const& other) {
            this->x += other.x;
            this->y += other.y;
            return *this;
        }

        Point& operator-=(Point const& other) {
            this->x -= other.x;
            this->y -= other.y;
            return *this;
        }

        Point operator/(Float32 s) const {
            return { x / s, y / s };
        }

        Point operator*(Float32 s) const {
            return { x * s, y * s };
        }

        Float32 x = 0.0f;
        Float32 y = 0.0f;

    public:

        const static Point zero;

    };

}