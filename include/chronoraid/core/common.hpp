#pragma once

#include <cstdint>
#include <cstddef>

namespace chronoraid {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using f32 = float;
using f64 = double;

using usize = std::size_t;

constexpr f32 PI = 3.14159265358979323846f;
constexpr f32 DEG_TO_RAD = PI / 180.0f;
constexpr f32 RAD_TO_DEG = 180.0f / PI;

constexpr f32 EPSILON = 1e-6f;

template<typename T>
constexpr T clamp(T value, T min, T max) {
    return value < min ? min : (value > max ? max : value);
}

template<typename T>
constexpr T lerp(T a, T b, f32 t) {
    return a + (b - a) * t;
}

template<typename T>
constexpr T max(T a, T b) {
    return a > b ? a : b;
}

template<typename T>
constexpr T min(T a, T b) {
    return a < b ? a : b;
}

} // namespace chronoraid
