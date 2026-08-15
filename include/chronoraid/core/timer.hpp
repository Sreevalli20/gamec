#pragma once

#include "common.hpp"
#include <chrono>

namespace chronoraid {

class Timer {
public:
    Timer() : start_time_(clock::now()), paused_time_(0), is_paused_(false) {}

    void start() {
        start_time_ = clock::now();
        paused_time_ = 0;
        is_paused_ = false;
    }

    void pause() {
        if (!is_paused_) {
            pause_time_ = clock::now();
            is_paused_ = true;
        }
    }

    void resume() {
        if (is_paused_) {
            auto now = clock::now();
            paused_time_ += std::chrono::duration_cast<duration>(now - pause_time_).count();
            is_paused_ = false;
        }
    }

    f64 elapsed_seconds() const {
        if (is_paused_) {
            return std::chrono::duration_cast<duration>(pause_time_ - start_time_).count() - paused_time_;
        }
        return std::chrono::duration_cast<duration>(clock::now() - start_time_).count() - paused_time_;
    }

    f64 elapsed_milliseconds() const {
        return elapsed_seconds() * 1000.0;
    }

    void reset() {
        start();
    }

    bool is_paused() const {
        return is_paused_;
    }

private:
    using clock = std::chrono::high_resolution_clock;
    using time_point = std::chrono::time_point<clock>;
    using duration = std::chrono::duration<f64>;

    time_point start_time_;
    time_point pause_time_;
    f64 paused_time_;
    bool is_paused_;
};

class DeltaTimer {
public:
    DeltaTimer() : last_time_(clock::now()) {}

    f64 tick() {
        auto now = clock::now();
        f64 delta = std::chrono::duration_cast<std::chrono::duration<f64>>(now - last_time_).count();
        last_time_ = now;
        return delta;
    }

    void reset() {
        last_time_ = clock::now();
    }

private:
    using clock = std::chrono::high_resolution_clock;
    using time_point = std::chrono::time_point<clock>;

    time_point last_time_;
};

} // namespace chronoraid
