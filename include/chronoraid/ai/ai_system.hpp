#pragma once

#include "../entities/entity.hpp"
#include "../entities/components.hpp"
#include "../core/vector2.hpp"
#include "../core/common.hpp"
#include "../core/random.hpp"
#include <memory>

namespace chronoraid {

class PlayerController;

enum class AIState {
    Idle,
    Patrol,
    Chase,
    Attack,
    Retreat,
    Investigate,
    Dead
};

class AIController {
public:
    virtual ~AIController() = default;

    virtual void update(f64 delta_time, Entity* owner, Entity* target) = 0;
    virtual AIState get_state() const = 0;

protected:
    AIState state_ = AIState::Idle;
};

class ChaseAI : public AIController {
public:
    ChaseAI(f32 chase_range = 300.0f, f32 attack_range = 50.0f)
        : chase_range_(chase_range), attack_range_(attack_range) {}

    void update(f64 delta_time, Entity* owner, Entity* target) override;
    AIState get_state() const override { return state_; }

    void set_chase_range(f32 range) { chase_range_ = range; }
    void set_attack_range(f32 range) { attack_range_ = range; }

private:
    f32 chase_range_;
    f32 attack_range_;
    f32 patrol_timer_ = 0.0f;
    Vector2 patrol_direction_;
};

class PatrolAI : public AIController {
public:
    PatrolAI(const std::vector<Vector2>& waypoints, f32 wait_time = 2.0f)
        : waypoints_(waypoints), wait_time_(wait_time) {}

    void update(f64 delta_time, Entity* owner, Entity* target) override;
    AIState get_state() const override { return state_; }

    void set_waypoints(const std::vector<Vector2>& waypoints) { waypoints_ = waypoints; }

private:
    std::vector<Vector2> waypoints_;
    usize current_waypoint_ = 0;
    f32 wait_time_;
    f32 wait_timer_ = 0.0f;
};

class RangedAI : public AIController {
public:
    RangedAI(f32 attack_range = 200.0f, f32 retreat_range = 80.0f)
        : attack_range_(attack_range), retreat_range_(retreat_range) {}

    void update(f64 delta_time, Entity* owner, Entity* target) override;
    AIState get_state() const override { return state_; }

private:
    f32 attack_range_;
    f32 retreat_range_;
    f32 strafe_timer_ = 0.0f;
    bool strafing_left_ = false;
};

class TankAI : public AIController {
public:
    TankAI(f32 charge_range = 400.0f, f32 charge_cooldown = 5.0f)
        : charge_range_(charge_range), charge_cooldown_(charge_cooldown) {}

    void update(f64 delta_time, Entity* owner, Entity* target) override;
    AIState get_state() const override { return state_; }

private:
    f32 charge_range_;
    f32 charge_cooldown_;
    f32 charge_timer_ = 0.0f;
    bool is_charging_ = false;
    Vector2 charge_direction_;
};

class AISystem {
public:
    static AISystem& instance() {
        static AISystem system;
        return system;
    }

    void update(f64 delta_time);
    void register_entity(Entity* entity, std::unique_ptr<AIController> controller);
    void unregister_entity(EntityID id);

private:
    AISystem() = default;

    struct AIEntry {
        Entity* entity;
        std::unique_ptr<AIController> controller;
    };

    std::unordered_map<EntityID, AIEntry> ai_entities_;
};

} // namespace chronoraid
