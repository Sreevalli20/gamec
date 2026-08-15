#include <chronoraid/core/game.hpp>
#include <chronoraid/core/logger.hpp>

namespace chronoraid {

bool Game::initialize() {
    LOG_INFO("Initializing ChronoRaid game engine...");
    
    delta_timer_.reset();
    total_timer_.start();
    
    is_running_ = true;
    
    LOG_INFO("Game initialized successfully");
    return true;
}

void Game::shutdown() {
    LOG_INFO("Shutting down ChronoRaid game engine...");
    
    current_state_.reset();
    
    total_timer_.pause();
    
    is_running_ = false;
    
    LOG_INFO("Game shutdown complete");
}

void Game::run() {
    LOG_INFO("Starting main game loop");
    
    while (is_running_) {
        delta_time_ = delta_timer_.tick();
        total_time_ = total_timer_.elapsed_seconds();
        frame_count_++;
        
        process_input();
        
        if (current_state_) {
            current_state_->update(delta_time_);
        }
        
        render();
    }
}

void Game::quit() {
    LOG_INFO("Quit requested");
    is_running_ = false;
}

void Game::process_input() {
    if (current_state_) {
        current_state_->process_input();
    }
}

void Game::update(f64 delta_time) {
    if (current_state_) {
        current_state_->update(delta_time);
    }
}

void Game::render() {
    if (current_state_) {
        current_state_->render();
    }
}

void Game::set_state(std::unique_ptr<GameState> state) {
    if (current_state_) {
        current_state_->on_exit();
    }
    
    current_state_ = std::move(state);
    
    if (current_state_) {
        current_state_->on_enter();
    }
}

} // namespace chronoraid
