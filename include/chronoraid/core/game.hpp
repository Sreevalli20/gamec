#pragma once

#include "common.hpp"
#include "timer.hpp"
#include "input.hpp"
#include "logger.hpp"
#include <memory>

namespace chronoraid {

class GameState;

class Game {
public:
    static Game& instance() {
        static Game game;
        return game;
    }

    bool initialize();
    void shutdown();
    void run();
    void quit();

    f64 get_delta_time() const { return delta_time_; }
    f64 get_total_time() const { return total_time_; }
    u32 get_frame_count() const { return frame_count_; }

    void set_state(std::unique_ptr<GameState> state);

    template<typename T, typename... Args>
    void push_state(Args&&... args) {
        set_state(std::make_unique<T>(std::forward<Args>(args)...));
    }

    bool is_running() const { return is_running_; }

private:
    Game() = default;
    ~Game() = default;

    void process_input();
    void update(f64 delta_time);
    void render();

    bool is_running_ = false;
    f64 delta_time_ = 0.0;
    f64 total_time_ = 0.0;
    u32 frame_count_ = 0;

    DeltaTimer delta_timer_;
    Timer total_timer_;

    std::unique_ptr<GameState> current_state_;
};

class GameState {
public:
    virtual ~GameState() = default;

    virtual void on_enter() {}
    virtual void on_exit() {}
    virtual void on_pause() {}
    virtual void on_resume() {}

    virtual void process_input() = 0;
    virtual void update(f64 delta_time) = 0;
    virtual void render() = 0;
};

} // namespace chronoraid
