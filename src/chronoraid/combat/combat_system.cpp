#include <chronoraid/combat/combat_system.hpp>
#include <chronoraid/core/logger.hpp>
#include <chronoraid/entities/components.hpp>
#include <algorithm>

namespace chronoraid {

void CombatSystem::update(f64 delta_time) {
}

AttackResult CombatSystem::perform_attack(Entity* attacker, Entity* target) {
    AttackResult result;
    
    if (!attacker || !target) {
        return result;
    }
    
    auto attacker_combat = attacker->get_component<CombatComponent>();
    auto target_health = target->get_component<HealthComponent>();
    
    if (!attacker_combat || !target_health) {
        return result;
    }
    
    if (!attacker_combat->can_attack()) {
        return result;
    }
    
    attacker_combat->attack();
    
    f32 base_damage = attacker_combat->calculate_damage();
    f32 final_damage = target_health->calculate_received_damage(base_damage);
    
    result.hit = true;
    result.damage = final_damage;
    result.is_critical = base_damage != final_damage / attacker_combat->critical_multiplier;
    
    apply_damage(target, final_damage, attacker);
    
    DamageInfo info;
    info.base_damage = base_damage;
    info.final_damage = final_damage;
    info.is_critical = result.is_critical;
    info.source = attacker;
    info.target = target;
    
    for (const auto& callback : on_damage_callbacks_) {
        callback(info);
    }
    
    if (target_health->is_dead) {
        for (const auto& callback : on_death_callbacks_) {
            callback(target);
        }
    }
    
    return result;
}

void CombatSystem::apply_damage(Entity* target, f32 damage, Entity* source) {
    if (!target) return;
    
    auto health = target->get_component<HealthComponent>();
    if (health) {
        health->take_damage(damage);
    }
}

void CombatSystem::apply_healing(Entity* target, f32 amount) {
    if (!target) return;
    
    auto health = target->get_component<HealthComponent>();
    if (health) {
        health->heal(amount);
    }
}

void CombatSystem::apply_knockback(Entity* entity, const Vector2& direction, f32 force) {
    if (!entity) return;
    
    auto movement = entity->get_component<MovementComponent>();
    if (movement) {
        movement->apply_force(direction * force);
    }
}

void CombatSystem::register_on_damage_callback(std::function<void(const DamageInfo&)> callback) {
    on_damage_callbacks_.push_back(callback);
}

void CombatSystem::register_on_death_callback(std::function<void(Entity*)> callback) {
    on_death_callbacks_.push_back(callback);
}

void Projectile::update(f64 delta_time) {
    f32 dt = static_cast<f32>(delta_time);
    
    current_lifetime_ += dt;
    
    if (current_lifetime_ >= lifetime_) {
        destroy();
        return;
    }
    
    auto transform = get_component<TransformComponent>();
    if (transform) {
        transform->position += direction_ * speed_ * dt;
    }
}

} // namespace chronoraid
