#include <chronoraid/statistics/statistics.hpp>
#include <chronoraid/core/logger.hpp>

namespace chronoraid {

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
