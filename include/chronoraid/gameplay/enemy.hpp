#pragma once

#include "../entities/entity.hpp"
#include "../entities/components.hpp"
#include "../ai/ai_system.hpp"
#include "../core/common.hpp"
#include <string>

namespace chronoraid {

class PlayerController;
class EnemyFactory;

enum class EnemyType {
    ChronoDrone,
    RiftBeast,
    VoidArcher,
    TimeAssassin,
    TemporalTank,
    RiftMage,
    Boss
};

class Enemy : public Entity {
public:
    Enemy(EntityID id, EnemyType type);

    void update(f64 delta_time) override;

    EnemyType get_type() const { return type_; }
    const std::string& get_name() const { return name_; }

    void set_target(Entity* target) { target_ = target; }
    Entity* get_target() const { return target_; }

    void take_damage(f32 amount, Entity* source = nullptr);
    void on_death();

    bool is_boss() const { return type_ == EnemyType::Boss; }

    TransformComponent* get_transform() { return get_component<TransformComponent>(); }
    HealthComponent* get_health_comp() { return get_component<HealthComponent>(); }
    MovementComponent* get_movement() { return get_component<MovementComponent>(); }
    CombatComponent* get_combat() { return get_component<CombatComponent>(); }

private:
    void initialize_stats();

    EnemyType type_;
    std::string name_;
    Entity* target_ = nullptr;
    std::unique_ptr<AIController> ai_controller_;
    u32 experience_reward_ = 10;
};

class EnemyFactory {
public:
    static EnemyFactory& instance() {
        static EnemyFactory factory;
        return factory;
    }

    Enemy* create_enemy(EnemyType type, const Vector2& position);
    Enemy* create_boss(const std::string& boss_id, const Vector2& position);

    void set_difficulty_multiplier(f32 multiplier) { difficulty_multiplier_ = multiplier; }

private:
    EnemyFactory() = default;

    f32 difficulty_multiplier_ = 1.0f;
};

} // namespace chronoraid
