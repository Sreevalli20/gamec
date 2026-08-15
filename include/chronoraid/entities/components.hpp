#pragma once

#include "entity.hpp"
#include "../core/vector2.hpp"
#include "../core/rect.hpp"

namespace chronoraid {

struct TransformComponent : public Component {
    Vector2 position;
    Vector2 scale = Vector2::one();
    f32 rotation = 0.0f;

    TransformComponent() = default;
    TransformComponent(const Vector2& pos) : position(pos) {}

    Vector2 forward() const {
        f32 rad = rotation * DEG_TO_RAD;
        return Vector2(std::cos(rad), std::sin(rad));
    }
};

struct HealthComponent : public Component {
    f32 current_health = 100.0f;
    f32 max_health = 100.0f;
    bool is_dead = false;

    HealthComponent() = default;
    HealthComponent(f32 max) : current_health(max), max_health(max) {}

    void take_damage(f32 amount) {
        current_health = max(0.0f, current_health - amount);
        if (current_health <= 0.0f) {
            is_dead = true;
        }
    }

    void heal(f32 amount) {
        current_health = min(max_health, current_health + amount);
        is_dead = false;
    }

    f32 get_health_percentage() const {
        return current_health / max_health;
    }
};

struct MovementComponent : public Component {
    Vector2 velocity;
    f32 speed = 100.0f;
    f32 max_speed = 200.0f;
    f32 acceleration = 500.0f;
    f32 friction = 10.0f;

    MovementComponent() = default;
    MovementComponent(f32 spd) : speed(spd), max_speed(spd * 2.0f) {}

    void apply_force(const Vector2& force) {
        velocity += force;
    }

    void update(f64 delta_time) override {
        f32 dt = static_cast<f32>(delta_time);
        
        velocity *= (1.0f - friction * dt);
        
        f32 speed_sq = velocity.length_squared();
        if (speed_sq > max_speed * max_speed) {
            velocity = velocity.normalized() * max_speed;
        }
    }
};

struct CombatComponent : public Component {
    f32 attack_power = 10.0f;
    f32 attack_speed = 1.0f;
    f32 attack_cooldown = 0.0f;
    f32 critical_chance = 0.05f;
    f32 critical_multiplier = 2.0f;
    f32 armor = 0.0f;

    CombatComponent() = default;
    CombatComponent(f32 power) : attack_power(power) {}

    void update(f64 delta_time) override {
        if (attack_cooldown > 0.0f) {
            attack_cooldown -= static_cast<f32>(delta_time);
        }
    }

    bool can_attack() const {
        return attack_cooldown <= 0.0f;
    }

    void attack() {
        attack_cooldown = 1.0f / attack_speed;
    }

    f32 calculate_damage() const {
        f32 damage = attack_power;
        if (Random().next_bool(critical_chance)) {
            damage *= critical_multiplier;
        }
        return damage;
    }

    f32 calculate_received_damage(f32 base_damage) const {
        f32 reduction = armor / (armor + 100.0f);
        return base_damage * (1.0f - reduction);
    }
};

struct ExperienceComponent : public Component {
    u32 current_xp = 0;
    u32 xp_to_next_level = 100;
    u32 level = 1;

    ExperienceComponent() = default;

    void add_xp(u32 amount) {
        current_xp += amount;
        while (current_xp >= xp_to_next_level) {
            level_up();
        }
    }

    void level_up() {
        current_xp -= xp_to_next_level;
        level++;
        xp_to_next_level = static_cast<u32>(xp_to_next_level * 1.5f);
    }

    f32 get_xp_percentage() const {
        return static_cast<f32>(current_xp) / static_cast<f32>(xp_to_next_level);
    }
};

struct CollisionComponent : public Component {
    Rect bounds;
    bool is_trigger = false;
    u32 collision_layer = 1;
    u32 collision_mask = 0xFFFFFFFF;

    CollisionComponent() = default;
    CollisionComponent(const Rect& b) : bounds(b) {}

    bool collides_with(const CollisionComponent& other) const {
        if ((collision_mask & other.collision_layer) == 0) return false;
        if ((other.collision_mask & collision_layer) == 0) return false;
        return bounds.intersects(other.bounds);
    }
};

struct StatusEffectComponent : public Component {
    struct StatusEffect {
        enum class Type {
            None,
            Burn,
            Freeze,
            Poison,
            Stun,
            Slow,
            Haste,
            Shield
        };

        Type type = Type::None;
        f32 duration = 0.0f;
        f32 intensity = 1.0f;
    };

    std::vector<StatusEffect> active_effects;

    void add_effect(StatusEffect::Type type, f32 duration, f32 intensity = 1.0f) {
        StatusEffect effect;
        effect.type = type;
        effect.duration = duration;
        effect.intensity = intensity;
        active_effects.push_back(effect);
    }

    void update(f64 delta_time) override {
        f32 dt = static_cast<f32>(delta_time);
        for (auto it = active_effects.begin(); it != active_effects.end(); ) {
            it->duration -= dt;
            if (it->duration <= 0.0f) {
                it = active_effects.erase(it);
            } else {
                ++it;
            }
        }
    }

    bool has_effect(StatusEffect::Type type) const {
        for (const auto& effect : active_effects) {
            if (effect.type == type) return true;
        }
        return false;
    }
};

} // namespace chronoraid
