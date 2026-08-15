#pragma once

#include "../entities/entity.hpp"
#include "../entities/components.hpp"
#include "../core/common.hpp"
#include "../core/vector2.hpp"
#include <memory>

namespace chronoraid {

class PlayerController;

class Player : public Entity {
public:
    Player(EntityID id);

    void update(f64 delta_time) override;

    void move(const Vector2& direction);
    void attack();
    void use_ability(u32 ability_index);
    void interact();

    f32 get_health() const;
    f32 get_max_health() const;
    f32 get_energy() const;
    f32 get_max_energy() const;
    u32 get_level() const;
    u32 get_experience() const;

    void take_damage(f32 amount, Entity* source = nullptr);
    void heal(f32 amount);
    void add_experience(u32 amount);

    bool is_alive() const;

    TransformComponent* get_transform() { return get_component<TransformComponent>(); }
    HealthComponent* get_health_comp() { return get_component<HealthComponent>(); }
    MovementComponent* get_movement() { return get_component<MovementComponent>(); }
    CombatComponent* get_combat() { return get_component<CombatComponent>(); }
    ExperienceComponent* get_experience_comp() { return get_component<ExperienceComponent>(); }

private:
    void update_abilities(f64 delta_time);

    f32 energy_ = 100.0f;
    f32 max_energy_ = 100.0f;
    f32 energy_regen_rate_ = 5.0f;
};

class PlayerController {
public:
    static PlayerController& instance() {
        static PlayerController controller;
        return controller;
    }

    void set_player(Entity* player) { player_ = player; }
    Entity* get_player() const { return player_; }

    void update(f64 delta_time);
    void process_input();

private:
    PlayerController() = default;

    Entity* player_ = nullptr;
    Vector2 input_direction_;
};

} // namespace chronoraid
