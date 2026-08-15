#pragma once

#include "../core/common.hpp"
#include "../core/vector2.hpp"
#include "../entities/entity.hpp"
#include <vector>
#include <unordered_map>

namespace chronoraid {

class Game;

enum class TimeAbility {
    Rewind,
    TimeFreeze,
    TemporalDash,
    Echo,
    TimelineShift
};

struct TimeAbilityInfo {
    TimeAbility type;
    std::string name;
    f32 cooldown = 10.0f;
    f32 duration = 3.0f;
    f32 energy_cost = 25.0f;
    u32 required_level = 1;
    bool is_unlocked = false;
};

class TimeManager {
public:
    static TimeManager& instance() {
        static TimeManager manager;
        return manager;
    }

    void update(f64 delta_time);

    bool use_ability(TimeAbility ability, Entity* user);
    bool can_use_ability(TimeAbility ability) const;

    f32 get_cooldown(TimeAbility ability) const;
    f32 get_cooldown_percentage(TimeAbility ability) const;

    void unlock_ability(TimeAbility ability);
    bool is_ability_unlocked(TimeAbility ability) const;

    void set_time_scale_factor(f32 factor);
    f32 get_time_scale_factor() const { return time_scale_factor_; }

    void set_global_time_scale(f32 scale);
    f32 get_global_time_scale() const { return global_time_scale_; }

    bool is_time_frozen() const { return is_time_frozen_; }
    void set_time_frozen(bool frozen, Entity* source = nullptr);

private:
    TimeManager() = default;

    void execute_rewind(Entity* user);
    void execute_time_freeze(Entity* user);
    void execute_temporal_dash(Entity* user);
    void execute_echo(Entity* user);
    void execute_timeline_shift(Entity* user);

    struct AbilityState {
        f32 cooldown_remaining = 0.0f;
        bool is_unlocked = false;
    };

    std::unordered_map<TimeAbility, AbilityState> ability_states_;
    std::unordered_map<TimeAbility, TimeAbilityInfo> ability_info_;

    f32 time_scale_factor_ = 1.0f;
    f32 global_time_scale_ = 1.0f;
    bool is_time_frozen_ = false;
    Entity* time_freeze_source_ = nullptr;
    f32 time_freeze_duration_ = 0.0f;
};

struct TimeStateSnapshot {
    Vector2 position;
    Vector2 velocity;
    f32 health;
    f32 rotation;
    u32 timestamp;
};

class TimeRecorder {
public:
    static constexpr u32 MAX_SNAPSHOTS = 300;

    void record_snapshot(Entity* entity);
    std::vector<TimeStateSnapshot> get_snapshots(Entity* entity) const;
    void clear_snapshots(Entity* entity);

    void rewind_entity(Entity* entity, f32 duration_seconds);

private:
    std::unordered_map<EntityID, std::vector<TimeStateSnapshot>> snapshots_;
};

} // namespace chronoraid
