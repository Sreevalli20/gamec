#pragma once

#include "enemy.hpp"
#include "../core/common.hpp"
#include <vector>
#include <functional>

namespace chronoraid {

class Player;

enum class BossPhase {
    Phase1,
    Phase2,
    Phase3,
    Enraged
};

struct BossAttack {
    std::string name;
    f32 cooldown = 2.0f;
    f32 damage = 20.0f;
    f32 telegraph_duration = 0.5f;
    bool is_telegraphed = true;
};

class Boss : public Enemy {
public:
    Boss(EntityID id, const std::string& boss_name);

    void update(f64 delta_time) override;

    void add_phase(BossPhase phase, f32 health_threshold);
    void add_attack(const BossAttack& attack);

    BossPhase get_current_phase() const { return current_phase_; }

    void set_on_phase_change_callback(std::function<void(BossPhase)> callback) {
        on_phase_change_ = callback;
    }

protected:
    void check_phase_transition();
    void execute_attack_pattern(f64 delta_time);
    void telegraph_attack(const BossAttack& attack);

    BossPhase current_phase_ = BossPhase::Phase1;
    std::vector<std::pair<BossPhase, f32>> phase_thresholds_;
    std::vector<BossAttack> attacks_;
    f32 attack_timer_ = 0.0f;
    usize current_attack_index_ = 0;
    bool is_telegraphing_ = false;
    f32 telegraph_timer_ = 0.0f;

    std::function<void(BossPhase)> on_phase_change_;
};

class ClockmakerBoss : public Boss {
public:
    ClockmakerBoss(EntityID id);

    void update(f64 delta_time) override;

private:
    void time_slam_attack();
    void clockwork_minions();
    void temporal_zone();

    f32 time_slow_factor_ = 0.5f;
    f32 minion_spawn_timer_ = 0.0f;
};

class ParadoxBeastBoss : public Boss {
public:
    ParadoxBeastBoss(EntityID id);

    void update(f64 delta_time) override;

private:
    void paradox_shift();
    void reality_tear();
    void mirror_attack();

    bool is_mirrored_ = false;
    f32 mirror_timer_ = 0.0f;
};

class NullEmperorBoss : public Boss {
public:
    NullEmperorBoss(EntityID id);

    void update(f64 delta_time) override;

private:
    void void_wave();
    void nullification_field();
    void entropy_cascade();

    u32 phase_3_attack_count_ = 0;
};

} // namespace chronoraid
