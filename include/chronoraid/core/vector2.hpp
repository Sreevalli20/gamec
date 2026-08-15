#pragma once

#include "common.hpp"
#include <cmath>

namespace chronoraid {

struct Vector2 {
    f32 x;
    f32 y;

    constexpr Vector2() : x(0.0f), y(0.0f) {}
    constexpr Vector2(f32 x_, f32 y_) : x(x_), y(y_) {}

    static constexpr Vector2 zero() { return Vector2(0.0f, 0.0f); }
    static constexpr Vector2 one() { return Vector2(1.0f, 1.0f); }
    static constexpr Vector2 up() { return Vector2(0.0f, 1.0f); }
    static constexpr Vector2 down() { return Vector2(0.0f, -1.0f); }
    static constexpr Vector2 left() { return Vector2(-1.0f, 0.0f); }
    static constexpr Vector2 right() { return Vector2(1.0f, 0.0f); }

    constexpr f32 length_squared() const {
        return x * x + y * y;
    }

    f32 length() const {
        return std::sqrt(length_squared());
    }

    Vector2 normalized() const {
        f32 len = length();
        if (len > EPSILON) {
            return Vector2(x / len, y / len);
        }
        return Vector2::zero();
    }

    constexpr f32 dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }

    f32 distance_to(const Vector2& other) const {
        return (*this - other).length();
   }

    constexpr Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    constexpr Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }

    constexpr Vector2 operator*(f32 scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    constexpr Vector2 operator/(f32 scalar) const {
        return Vector2(x / scalar, y / scalar);
    }

    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2& operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vector2& operator*=(f32 scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vector2& operator/=(f32 scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    constexpr bool operator==(const Vector2& other) const {
        return x == other.x && y == other.y;
    }

    constexpr bool operator!=(const Vector2& other) const {
        return !(*this == other);
    }
};

} // namespace chronoraid
