#pragma once

#include "common.hpp"
#include <random>
#include <vector>

namespace chronoraid {

class Random {
public:
    explicit Random(u32 seed = 0) : generator_(seed) {}

    void set_seed(u32 seed) {
        generator_.seed(seed);
    }

    u32 next_u32() {
        return distribution_u32_(generator_);
    }

    u32 next_u32(u32 max) {
        return next_u32() % (max + 1);
    }

    u32 next_u32(u32 min, u32 max) {
        return min + next_u32() % (max - min + 1);
    }

    i32 next_i32() {
        return static_cast<i32>(next_u32());
    }

    i32 next_i32(i32 min, i32 max) {
        return min + static_cast<i32>(next_u32() % static_cast<u32>(max - min + 1));
    }

    f32 next_f32() {
        return distribution_f32_(generator_);
    }

    f32 next_f32(f32 max) {
        return next_f32() * max;
    }

    f32 next_f32(f32 min, f32 max) {
        return min + next_f32() * (max - min);
    }

    bool next_bool() {
        return next_u32() % 2 == 0;
    }

    bool next_bool(f32 probability) {
        return next_f32() < probability;
    }

    template<typename T>
    T from_vector(const std::vector<T>& vec) {
        if (vec.empty()) return T();
        return vec[next_u32(static_cast<u32>(vec.size() - 1))];
    }

private:
    std::mt19937 generator_;
    std::uniform_int_distribution<u32> distribution_u32_{0, UINT32_MAX};
    std::uniform_real_distribution<f32> distribution_f32_{0.0f, 1.0f};
};

} // namespace chronoraid
