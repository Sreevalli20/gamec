#pragma once

#include "../entities/entity.hpp"
#include "../entities/components.hpp"
#include "../core/common.hpp"
#include <vector>
#include <functional>

namespace chronoraid {

class TimeRecorder;

struct DamageInfo {
    f32 base_damage = 0.0f;
    f32 final_damage = 0.0f;
    bool is_critical = false;
    Entity* source = nullptr;
    Entity* target = nullptr;
};

struct AttackResult {
    bool hit = false;
    f32 damage = 0.0f;
    bool is_critical = false;
    bool is_blocked = false;
};

class CombatSystem {
public:
    static CombatSystem& instance() {
        static CombatSystem system;
        return system;
    }

    void update(f64 delta_time);

    AttackResult perform_attack(Entity* attacker, Entity* target);
    void apply_damage(Entity* target, f32 damage, Entity* source = nullptr);
    void apply_healing(Entity* target, f32 amount);
    void apply_knockback(Entity* entity, const Vector2& direction, f32 force);

    void register_on_damage_callback(std::function<void(const DamageInfo&)> callback);
    void register_on_death_callback(std::function<void(Entity*)> callback);

private:
    CombatSystem() = default;

    std::vector<std::function<void(const DamageInfo&)>> on_damage_callbacks_;
    std::vector<std::function<void(Entity*)>> on_death_callbacks_;
};

class Projectile : public Entity {
public:
    Projectile(EntityID id) : Entity(id) {}

    void set_source(Entity* source) { source_ = source; }
    Entity* get_source() const { return source_; }

    void set_damage(f32 damage) { damage_ = damage; }
    f32 get_damage() const { return damage_; }

    void set_direction(const Vector2& dir) { direction_ = dir.normalized(); }
    Vector2 get_direction() const { return direction_; }

    void set_speed(f32 speed) { speed_ = speed; }
    f32 get_speed() const { return speed_; }

    void set_lifetime(f32 lifetime) { lifetime_ = lifetime; }
    f32 get_lifetime() const { return lifetime_; }

    void update(f64 delta_time) override;

private:
    Entity* source_ = nullptr;
    f32 damage_ = 10.0f;
    Vector2 direction_ = Vector2::right();
    f32 speed_ = 300.0f;
    f32 lifetime_ = 3.0f;
    f32 current_lifetime_ = 0.0f;
};

} // namespace chronoraid
