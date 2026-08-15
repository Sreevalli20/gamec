#pragma once

#include "../core/common.hpp"
#include <string>
#include <unordered_map>

namespace chronoraid {

class Statistics {
public:
    static Statistics& instance() {
        static Statistics stats;
        return stats;
    }

    void reset();

    void record_enemy_kill(bool is_boss);
    void record_damage_dealt(f32 amount);
    void record_damage_received(f32 amount);
    void record_ability_use(const std::string& ability);
    void record_item_collected(const std::string& item_id);
    void record_death();
    void record_quest_completion(const std::string& quest_id);
    void record_room_cleared();
    void record_play_time(f64 delta_time);

    u32 get_total_kills() const { return total_kills_; }
    u32 get_boss_kills() const { return boss_kills_; }
    f32 get_total_damage_dealt() const { return total_damage_dealt_; }
    f32 get_total_damage_received() const { return total_damage_received_; }
    u32 get_total_deaths() const { return total_deaths_; }
    u32 get_quests_completed() const { return quests_completed_; }
    u32 get_rooms_cleared() const { return rooms_cleared_; }
    f64 get_total_play_time() const { return total_play_time_; }

    void save();
    void load();

private:
    Statistics() { reset(); }

    u32 total_kills_ = 0;
    u32 boss_kills_ = 0;
    f32 total_damage_dealt_ = 0.0f;
    f32 total_damage_received_ = 0.0f;
    u32 total_deaths_ = 0;
    u32 quests_completed_ = 0;
    u32 rooms_cleared_ = 0;
    f64 total_play_time_ = 0.0f;

    std::unordered_map<std::string, u32> ability_uses_;
    std::unordered_map<std::string, u32> items_collected_;
};

} // namespace chronoraid
