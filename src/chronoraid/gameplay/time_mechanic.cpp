#include <chronoraid/gameplay/time_mechanic.hpp>
#include <chronoraid/core/logger.hpp>
#include <chronoraid/core/game.hpp>
#include <chronoraid/entities/components.hpp>

namespace chronoraid {

void TimeManager::update(f64 delta_time) {
    for (auto& [ability, state] : ability_states_) {
        if (state.cooldown_remaining > 0.0f) {
            state.cooldown_remaining -= static_cast<f32>(delta_time);
        }
    }
    
    if (is_time_frozen_ && time_freeze_duration_ > 0.0f) {
        time_freeze_duration_ -= static_cast<f32>(delta_time);
        if (time_freeze_duration_ <= 0.0f) {
            set_time_frozen(false);
        }
    }
}

bool TimeManager::use_ability(TimeAbility ability, Entity* user) {
    if (!can_use_ability(ability)) {
        LOG_WARNING("Cannot use time ability: on cooldown or not unlocked");
        return false;
    }
    
    auto it = ability_states_.find(ability);
    if (it != ability_states_.end()) {
        auto info_it = ability_info_.find(ability);
        if (info_it != ability_info_.end()) {
            it->second.cooldown_remaining = info_it->cooldown;
        }
    }
    
    switch (ability) {
        case TimeAbility::Rewind:
            execute_rewind(user);
            break;
        case TimeAbility::TimeFreeze:
            execute_time_freeze(user);
            break;
        case TimeAbility::TemporalDash:
            execute_temporal_dash(user);
            break;
        case TimeAbility::Echo:
            execute_echo(user);
            break;
        case TimeAbility::TimelineShift:
            execute_timeline_shift(user);
            break;
    }
    
    LOG_INFO("Used time ability: " + std::to_string(static_cast<int>(ability)));
    return true;
}

bool TimeManager::can_use_ability(TimeAbility ability) const {
    auto it = ability_states_.find(ability);
    if (it == ability_states_.end()) return false;
    
    if (!it->second.is_unlocked) return false;
    if (it->second.cooldown_remaining > 0.0f) return false;
    
    return true;
}

f32 TimeManager::get_cooldown(TimeAbility ability) const {
    auto it = ability_states_.find(ability);
    if (it != ability_states_.end()) {
        return it->second.cooldown_remaining;
    }
    return 0.0f;
}

f32 TimeManager::get_cooldown_percentage(TimeAbility ability) const {
    auto it = ability_info_.find(ability);
    if (it != ability_info_.end()) {
        f32 current = get_cooldown(ability);
        return current / it->cooldown;
    }
    return 0.0f;
}

void TimeManager::unlock_ability(TimeAbility ability) {
    auto it = ability_states_.find(ability);
    if (it != ability_states_.end()) {
        it->second.is_unlocked = true;
        LOG_INFO("Unlocked time ability: " + std::to_string(static_cast<int>(ability)));
    }
}

bool TimeManager::is_ability_unlocked(TimeAbility ability) const {
    auto it = ability_states_.find(ability);
    return it != ability_states_.end() && it->second.is_unlocked;
}

void TimeManager::set_time_scale_factor(f32 factor) {
    time_scale_factor_ = clamp(factor, 0.1f, 2.0f);
}

void TimeManager::set_global_time_scale(f32 scale) {
    global_time_scale_ = clamp(scale, 0.0f, 2.0f);
}

void TimeManager::set_time_frozen(bool frozen, Entity* source) {
    is_time_frozen_ = frozen;
    time_freeze_source_ = source;
    
    if (frozen) {
        time_freeze_duration_ = 3.0f;
        LOG_INFO("Time frozen by entity");
    } else {
        LOG_INFO("Time resumed");
    }
}

void TimeManager::execute_rewind(Entity* user) {
    if (!user) return;
    
    TimeRecorder::instance().rewind_entity(user, 2.0f);
    
    auto health = user->get_component<HealthComponent>();
    if (health) {
        health->heal(20.0f);
    }
}

void TimeManager::execute_time_freeze(Entity* user) {
    set_time_frozen(true, user);
}

void TimeManager::execute_temporal_dash(Entity* user) {
    if (!user) return;
    
    auto transform = user->get_component<TransformComponent>();
    auto movement = user->get_component<MovementComponent>();
    
    if (transform && movement) {
        Vector2 dash_dir = transform->forward();
        transform->position += dash_dir * 5.0f;
        movement->velocity = dash_dir * movement->max_speed * 2.0f;
    }
}

void TimeManager::execute_echo(Entity* user) {
    if (!user) return;
    
    auto snapshots = TimeRecorder::instance().get_snapshots(user);
    if (!snapshots.empty()) {
        auto transform = user->get_component<TransformComponent>();
        if (transform) {
            transform->position = snapshots.back().position;
        }
    }
}

void TimeManager::execute_timeline_shift(Entity* user) {
    set_global_time_scale(0.5f);
    
    static f32 shift_duration = 5.0f;
    static f32 shift_timer = 0.0f;
    shift_timer = shift_duration;
}

void TimeRecorder::record_snapshot(Entity* entity) {
    if (!entity) return;
    
    TimeStateSnapshot snapshot;
    
    auto transform = entity->get_component<TransformComponent>();
    auto movement = entity->get_component<MovementComponent>();
    auto health = entity->get_component<HealthComponent>();
    
    if (transform) {
        snapshot.position = transform->position;
        snapshot.rotation = transform->rotation;
    }
    if (movement) {
        snapshot.velocity = movement->velocity;
    }
    if (health) {
        snapshot.health = health->current_health;
    }
    
    snapshot.timestamp = static_cast<u32>(Game::instance().get_total_time() * 1000);
    
    auto& entity_snapshots = snapshots_[entity->get_id()];
    entity_snapshots.push_back(snapshot);
    
    if (entity_snapshots.size() > MAX_SNAPSHOTS) {
        entity_snapshots.erase(entity_snapshots.begin());
    }
}

std::vector<TimeStateSnapshot> TimeRecorder::get_snapshots(Entity* entity) const {
    if (!entity) return {};
    
    auto it = snapshots_.find(entity->get_id());
    if (it != snapshots_.end()) {
        return it->second;
    }
    return {};
}

void TimeRecorder::clear_snapshots(Entity* entity) {
    if (!entity) return;
    
    snapshots_.erase(entity->get_id());
}

void TimeRecorder::rewind_entity(Entity* entity, f32 duration_seconds) {
    if (!entity) return;
    
    u32 target_time = static_cast<u32>((Game::instance().get_total_time() - duration_seconds) * 1000);
    
    auto& entity_snapshots = snapshots_[entity->get_id()];
    
    for (auto it = entity_snapshots.rbegin(); it != entity_snapshots.rend(); ++it) {
        if (it->timestamp <= target_time) {
            auto transform = entity->get_component<TransformComponent>();
            auto movement = entity->get_component<MovementComponent>();
            auto health = entity->get_component<HealthComponent>();
            
            if (transform) {
                transform->position = it->position;
                transform->rotation = it->rotation;
            }
            if (movement) {
                movement->velocity = it->velocity;
            }
            if (health) {
                health->current_health = it->health;
            }
            
            LOG_INFO("Rewound entity to time: " + std::to_string(it->timestamp));
            return;
        }
    }
}

} // namespace chronoraid
