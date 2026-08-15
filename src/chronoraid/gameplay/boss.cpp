#include <chronoraid/gameplay/boss.hpp>
#include <chronoraid/core/logger.hpp>
#include <chronoraid/gameplay/enemy.hpp>
#include <chronoraid/gameplay/player.hpp>
#include <chronoraid/combat/combat_system.hpp>
#include <chronoraid/core/random.hpp>

namespace chronoraid {

Boss::Boss(EntityID id, const std::string& boss_name) 
    : Enemy(id, EnemyType::Boss) {
    name_ = boss_name;
    
    add_phase(BossPhase::Phase1, 0.66f);
    add_phase(BossPhase::Phase2, 0.33f);
    add_phase(BossPhase::Enraged, 0.0f);
    
    BossAttack basic_attack;
    basic_attack.name = "Basic Attack";
    basic_attack.cooldown = 2.0f;
    basic_attack.damage = 25.0f;
    basic_attack.telegraph_duration = 0.5f;
    add_attack(basic_attack);
}

void Boss::update(f64 delta_time) {
    Enemy::update(delta_time);
    
    auto health = get_health_comp();
    if (health) {
        check_phase_transition();
    }
    
    if (!health->is_dead) {
        execute_attack_pattern(delta_time);
    }
}

void Boss::add_phase(BossPhase phase, f32 health_threshold) {
    phase_thresholds_.emplace_back(phase, health_threshold);
}

void Boss::add_attack(const BossAttack& attack) {
    attacks_.push_back(attack);
}

void Boss::check_phase_transition() {
    auto health = get_health_comp();
    if (!health) return;
    
    f32 health_percent = health->get_health_percentage();
    
    for (const auto& [phase, threshold] : phase_thresholds_) {
        if (health_percent <= threshold && current_phase_ != phase) {
            BossPhase old_phase = current_phase_;
            current_phase_ = phase;
            
            on_phase_enter(phase);
            
            if (on_phase_change_) {
                on_phase_change_(phase);
            }
            
            LOG_INFO(name_ + " entered phase " + std::to_string(static_cast<int>(phase)));
            break;
        }
    }
}

void Boss::execute_attack_pattern(f64 delta_time) {
    if (is_telegraphing_) {
        telegraph_timer_ -= static_cast<f32>(delta_time);
        if (telegraph_timer_ <= 0.0f) {
            is_telegraphing_ = false;
            execute_current_attack();
        }
        return;
    }
    
    attack_timer_ -= static_cast<f32>(delta_time);
    
    if (attack_timer_ <= 0.0f && !attacks_.empty()) {
        current_attack_index_ = (current_attack_index_ + 1) % attacks_.size();
        const auto& attack = attacks_[current_attack_index_];
        
        if (attack.is_telegraphed) {
            is_telegraphing_ = true;
            telegraph_timer_ = attack.telegraph_duration;
            telegraph_attack(attack);
        } else {
            execute_current_attack();
        }
        
        attack_timer_ = attack.cooldown;
    }
}

void Boss::telegraph_attack(const BossAttack& attack) {
    LOG_INFO(name_ + " is telegraphing: " + attack.name);
}

void Boss::execute_current_attack() {
    if (attacks_.empty()) return;
    
    const auto& attack = attacks_[current_attack_index_];
    
    auto player = PlayerController::instance().get_player();
    if (player) {
        CombatSystem::instance().apply_damage(player, attack.damage, this);
    }
    
    LOG_INFO(name_ + " executed: " + attack.name);
}

void Boss::on_phase_enter(BossPhase phase) {
    switch (phase) {
        case BossPhase::Phase2:
            on_phase_2_enter();
            break;
        case BossPhase::Enraged:
            on_enraged_enter();
            break;
        default:
            break;
    }
}

void Boss::on_phase_2_enter() {
    auto combat = get_combat();
    if (combat) {
        combat->attack_speed *= 1.3f;
        combat->attack_power *= 1.2f;
    }
}

void Boss::on_enraged_enter() {
    auto combat = get_combat();
    if (combat) {
        combat->attack_speed *= 1.5f;
        combat->attack_power *= 1.5f;
    }
}

ClockmakerBoss::ClockmakerBoss(EntityID id) : Boss(id, "The Clockmaker") {
    BossAttack time_slam;
    time_slam.name = "Time Slam";
    time_slam.cooldown = 3.0f;
    time_slam.damage = 35.0f;
    time_slam.telegraph_duration = 1.0f;
    add_attack(time_slam);
    
    BossAttack clockwork;
    clockwork.name = "Clockwork Minions";
    clockwork.cooldown = 8.0f;
    clockwork.damage = 0.0f;
    add_attack(clockwork);
}

void ClockmakerBoss::update(f64 delta_time) {
    Boss::update(delta_time);
    
    minion_spawn_timer_ -= static_cast<f32>(delta_time);
    
    if (minion_spawn_timer_ <= 0.0f && current_phase_ >= BossPhase::Phase2) {
        clockwork_minions();
        minion_spawn_timer_ = 10.0f;
    }
}

void ClockmakerBoss::time_slam_attack() {
    auto player = PlayerController::instance().get_player();
    if (player) {
        CombatSystem::instance().apply_damage(player, 40.0f, this);
        CombatSystem::instance().apply_knockback(player, get_transform()->position - player->get_transform()->position, 500.0f);
    }
}

void ClockmakerBoss::clockwork_minions() {
    auto transform = get_transform();
    if (!transform) return;
    
    for (int i = 0; i < 3; ++i) {
        Vector2 offset = Vector2(
            Random().next_f32(-3.0f, 3.0f),
            Random().next_f32(-3.0f, 3.0f)
        );
        EnemyFactory::instance().create_enemy(EnemyType::ChronoDrone, transform->position + offset);
    }
}

void ClockmakerBoss::temporal_zone() {
    TimeManager::instance().set_time_scale_factor(0.5f);
}

ParadoxBeastBoss::ParadoxBeastBoss(EntityID id) : Boss(id, "The Paradox Beast") {
    BossAttack paradox_shift;
    paradox_shift.name = "Paradox Shift";
    paradox_shift.cooldown = 5.0f;
    paradox_shift.damage = 30.0f;
    paradox_shift.telegraph_duration = 0.8f;
    add_attack(paradox_shift);
    
    BossAttack reality_tear;
    reality_tear.name = "Reality Tear";
    reality_tear.cooldown = 4.0f;
    reality_tear.damage = 25.0f;
    add_attack(reality_tear);
}

void ParadoxBeastBoss::update(f64 delta_time) {
    Boss::update(delta_time);
    
    mirror_timer_ -= static_cast<f32>(delta_time);
    
    if (mirror_timer_ <= 0.0f) {
        is_mirrored_ = !is_mirrored_;
        mirror_timer_ = 8.0f;
    }
}

void ParadoxBeastBoss::paradox_shift() {
    auto transform = get_transform();
    if (transform) {
        transform->position = Vector2(
            Random().next_f32(10.0f, 70.0f),
            Random().next_f32(5.0f, 20.0f)
        );
    }
}

void ParadoxBeastBoss::reality_tear() {
    auto player = PlayerController::instance().get_player();
    if (player) {
        CombatSystem::instance().apply_damage(player, 30.0f, this);
    }
}

void ParadoxBeastBoss::mirror_attack() {
    is_mirrored_ = true;
    mirror_timer_ = 5.0f;
}

NullEmperorBoss::NullEmperorBoss(EntityID id) : Boss(id, "The Null Emperor") {
    BossAttack void_wave;
    void_wave.name = "Void Wave";
    void_wave.cooldown = 2.5f;
    void_wave.damage = 40.0f;
    void_wave.telegraph_duration = 1.0f;
    add_attack(void_wave);
    
    BossAttack nullification;
    nullification.name = "Nullification Field";
    nullification.cooldown = 6.0f;
    nullification.damage = 0.0f;
    add_attack(nullification);
}

void NullEmperorBoss::update(f64 delta_time) {
    Boss::update(delta_time);
    
    if (current_phase_ == BossPhase::Enraged) {
        phase_3_attack_count_++;
        
        if (phase_3_attack_count_ >= 5) {
            entropy_cascade();
            phase_3_attack_count_ = 0;
        }
    }
}

void NullEmperorBoss::void_wave() {
    auto player = PlayerController::instance().get_player();
    if (player) {
        CombatSystem::instance().apply_damage(player, 45.0f, this);
    }
}

void NullEmperorBoss::nullification_field() {
    TimeManager::instance().set_time_frozen(true, this);
}

void NullEmperorBoss::entropy_cascade() {
    auto player = PlayerController::instance().get_player();
    if (player) {
        CombatSystem::instance().apply_damage(player, 60.0f, this);
    }
}

} // namespace chronoraid
