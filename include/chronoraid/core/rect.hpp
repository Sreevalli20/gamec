#pragma once

#include "vector2.hpp"
#include "common.hpp"

namespace chronoraid {

struct Rect {
    f32 x;
    f32 y;
    f32 width;
    f32 height;

    constexpr Rect() : x(0.0f), y(0.0f), width(0.0f), height(0.0f) {}
    constexpr Rect(f32 x_, f32 y_, f32 w, f32 h) : x(x_), y(y_), width(w), height(h) {}

    constexpr f32 left() const { return x; }
    constexpr f32 right() const { return x + width; }
    constexpr f32 top() const { return y; }
    constexpr f32 bottom() const { return y + height; }

    constexpr Vector2 center() const {
        return Vector2(x + width / 2.0f, y + height / 2.0f);
    }

    constexpr Vector2 position() const {
        return Vector2(x, y);
    }

    constexpr Vector2 size() const {
        return Vector2(width, height);
    }

    bool contains(const Vector2& point) const {
        return point.x >= x && point.x <= right() &&
               point.y >= y && point.y <= bottom();
    }

    bool intersects(const Rect& other) const {
        return x < other.right() && right() > other.x &&
               y < other.bottom() && bottom() > other.y;
    }

    bool contains(const Rect& other) const {
        return other.x >= x && other.right() <= right() &&
               other.y >= y && other.bottom() <= bottom();
    }
};

} // namespace chronoraid
