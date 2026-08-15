#include <chronoraid/gameplay/game_modes.hpp>
#include <chronoraid/core/logger.hpp>
#include <chronoraid/gameplay/player.hpp>

namespace chronoraid {

f32 GameSettings::get_difficulty_multiplier() const {
    switch (difficulty_) {
        case Difficulty::Easy: return 0.7f;
        case Difficulty::Normal: return 1.0f;
        case Difficulty::Hard: return 1.5f;
        case Difficulty::Nightmare: return 2.5f;
        default: return 1.0f;
    }
}

u32 GameSettings::get_starting_level() const {
    switch (game_mode_) {
        case GameMode::Story: return 1;
        case GameMode::Endless: return 5;
        case GameMode::Challenge: return 10;
        default: return 1;
    }
}

u32 GameSettings::get_luck_bonus() const {
    switch (difficulty_) {
        case Difficulty::Easy: return 20;
        case Difficulty::Normal: return 0;
        case Difficulty::Hard: return -10;
        case Difficulty::Nightmare: return -25;
        default: return 0;
    }
}

void GameSettings::save() {
}

void GameSettings::load() {
}

void ScoreSystem::reset() {
    total_score_ = 0;
    current_combo_ = 0;
    combo_timer_ = 0.0f;
}

void ScoreSystem::add_enemy_kill_score(u32 enemy_level, bool is_boss) {
    u32 base_score = enemy_level * 10;
    if (is_boss) {
        base_score *= 10;
    }
    
    base_score += current_combo_ * 5;
    total_score_ += base_score;
}

void ScoreSystem::add_combo_score(u32 combo_count) {
    total_score_ += combo_count * 10;
}

void ScoreSystem::add_time_bonus(f32 time_remaining) {
    total_score_ += static_cast<u32>(time_remaining * 10);
}

void ScoreSystem::add_damage_efficiency_bonus(f32 damage_dealt, f32 damage_taken) {
    if (damage_taken < EPSILON) {
        total_score_ += 500;
    } else {
        f32 efficiency = damage_dealt / damage_taken;
        total_score_ += static_cast<u32>(efficiency * 10);
    }
}

void ScoreSystem::add_exploration_score(u32 rooms_cleared, u32 total_rooms) {
    if (total_rooms > 0) {
        f32 percentage = static_cast<f32>(rooms_cleared) / static_cast<f32>(total_rooms);
        total_score_ += static_cast<u32>(percentage * 200);
    }
}

void ScoreSystem::add_quest_completion_score(u32 quest_points) {
    total_score_ += quest_points * 50;
}

void ScoreSystem::increment_combo() {
    current_combo_++;
    combo_timer_ = COMBO_TIMEOUT;
}

void ScoreSystem::reset_combo() {
    current_combo_ = 0;
    combo_timer_ = 0.0f;
}

void ScoreSystem::update(f64 delta_time) {
    if (current_combo_ > 0) {
        combo_timer_ -= static_cast<f32>(delta_time);
        if (combo_timer_ <= 0.0f) {
            reset_combo();
        }
    }
}

void ScoreSystem::save_high_score() {
    u32 current = get_high_score();
    if (total_score_ > current) {
        LOG_INFO("New high score: " + std::to_string(total_score_));
    }
}

u32 ScoreSystem::get_high_score() const {
    return total_score_;
}

void Statistics::reset() {
    total_kills_ = 0;
    boss_kills_ = 0;
    total_damage_dealt_ = 0.0f;
    total_damage_received_ = 0.0f;
    total_deaths_ = 0;
    quests_completed_ = 0;
    rooms_cleared_ = 0;
    total_play_time_ = 0.0f;
    ability_uses_.clear();
    items_collected_.clear();
}

void Statistics::record_enemy_kill(bool is_boss) {
    total_kills_++;
    if (is_boss) {
        boss_kills_++;
    }
    
    ScoreSystem::instance().increment_combo();
}

void Statistics::record_damage_dealt(f32 amount) {
    total_damage_dealt_ += amount;
}

void Statistics::record_damage_received(f32 amount) {
    total_damage_received_ += amount;
}

void Statistics::record_ability_use(const std::string& ability) {
    ability_uses_[ability]++;
}

void Statistics::record_item_collected(const std::string& item_id) {
    items_collected_[item_id]++;
}

void Statistics::record_death() {
    total_deaths_++;
    ScoreSystem::instance().reset_combo();
}

void Statistics::record_quest_completion(const std::string& quest_id) {
    quests_completed_++;
}

void Statistics::record_room_cleared() {
    rooms_cleared_++;
}

void Statistics::record_play_time(f64 delta_time) {
    total_play_time_ += delta_time;
}

void Statistics::save() {
}

void Statistics::load() {
}

} // namespace chronoraid
