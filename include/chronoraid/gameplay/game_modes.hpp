#pragma once

#include "../core/common.hpp"
#include <string>

namespace chronoraid {

class Player;

enum class GameMode {
    Story,
    Endless,
    Challenge
};

enum class Difficulty {
    Easy,
    Normal,
    Hard,
    Nightmare
};

class GameSettings {
public:
    static GameSettings& instance() {
        static GameSettings settings;
        return settings;
    }

    GameMode get_game_mode() const { return game_mode_; }
    void set_game_mode(GameMode mode) { game_mode_ = mode; }

    Difficulty get_difficulty() const { return difficulty_; }
    void set_difficulty(Difficulty diff) { difficulty_ = diff; }

    f32 get_difficulty_multiplier() const;

    u32 get_starting_level() const;
    u32 get_luck_bonus() const;

    void save();
    void load();

private:
    GameSettings() : game_mode_(GameMode::Story), difficulty_(Difficulty::Normal) {}

    GameMode game_mode_;
    Difficulty difficulty_;
};

class ScoreSystem {
public:
    static ScoreSystem& instance() {
        static ScoreSystem system;
        return system;
    }

    void reset();
    u32 get_score() const { return total_score_; }

    void add_enemy_kill_score(u32 enemy_level, bool is_boss);
    void add_combo_score(u32 combo_count);
    void add_time_bonus(f32 time_remaining);
    void add_damage_efficiency_bonus(f32 damage_dealt, f32 damage_taken);
    void add_exploration_score(u32 rooms_cleared, u32 total_rooms);
    void add_quest_completion_score(u32 quest_points);

    void increment_combo();
    void reset_combo();
    u32 get_combo() const { return current_combo_; }

    void update(f64 delta_time);
    void save_high_score();
    u32 get_high_score() const;

private:
    ScoreSystem() : total_score_(0), current_combo_(0), combo_timer_(0.0f) {}

    u32 total_score_;
    u32 current_combo_;
    f32 combo_timer_;
    static constexpr f32 COMBO_TIMEOUT = 3.0f;
};

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
