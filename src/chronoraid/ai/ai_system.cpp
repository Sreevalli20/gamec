#include <chronoraid/ai/ai_system.hpp>
#include <chronoraid/entities/components.hpp>
#include <chronoraid/core/logger.hpp>
#include <chronoraid/gameplay/player.hpp>
#include <cmath>

namespace chronoraid {

void ChaseAI::update(f64 delta_time, Entity* owner, Entity* target) {
    if (!owner || !target) {
        state_ = AIState::Idle;
        return;
    }
    
    auto owner_transform = owner->get_component<TransformComponent>();
    auto target_transform = target->get_component<TransformComponent>();
    auto owner_movement = owner->get_component<MovementComponent>();
    
    if (!owner_transform || !target_transform || !owner_movement) {
        state_ = AIState::Idle;
        return;
    }
    
    f32 distance = owner_transform->position.distance_to(target_transform->position);
    
    if (distance <= attack_range_) {
        state_ = AIState::Attack;
        auto combat = owner->get_component<CombatComponent>();
        if (combat && combat->can_attack()) {
            combat->attack();
        }
    } else if (distance <= chase_range_) {
        state_ = AIState::Chase;
        Vector2 direction = (target_transform->position - owner_transform->position).normalized();
        owner_movement->apply_force(direction * owner_movement->acceleration);
    } else {
        state_ = AIState::Patrol;
        patrol_timer_ -= static_cast<f32>(delta_time);
        if (patrol_timer_ <= 0.0f) {
            patrol_direction_ = Vector2(
                Random().next_f32(-1.0f, 1.0f),
                Random().next_f32(-1.0f, 1.0f)
            ).normalized();
            patrol_timer_ = 2.0f;
        }
        owner_movement->apply_force(patrol_direction_ * owner_movement->acceleration * 0.5f);
    }
}

void PatrolAI::update(f64 delta_time, Entity* owner, Entity* target) {
    if (!owner || waypoints_.empty()) {
        state_ = AIState::Idle;
        return;
    }
    
    auto owner_transform = owner->get_component<TransformComponent>();
    auto owner_movement = owner->get_component<MovementComponent>();
    
    if (!owner_transform || !owner_movement) {
        state_ = AIState::Idle;
        return;
    }
    
    Vector2 target_pos = waypoints_[current_waypoint_];
    f32 distance = owner_transform->position.distance_to(target_pos);
    
    if (distance < 5.0f) {
        state_ = AIState::Idle;
        wait_timer_ -= static_cast<f32>(delta_time);
        if (wait_timer_ <= 0.0f) {
            current_waypoint_ = (current_waypoint_ + 1) % waypoints_.size();
            wait_timer_ = wait_time_;
        }
    } else {
        state_ = AIState::Patrol;
        Vector2 direction = (target_pos - owner_transform->position).normalized();
        owner_movement->apply_force(direction * owner_movement->acceleration);
    }
}

void RangedAI::update(f64 delta_time, Entity* owner, Entity* target) {
    if (!owner || !target) {
        state_ = AIState::Idle;
        return;
    }
    
    auto owner_transform = owner->get_component<TransformComponent>();
    auto target_transform = target->get_component<TransformComponent>();
    auto owner_movement = owner->get_component<MovementComponent>();
    
    if (!owner_transform || !target_transform || !owner_movement) {
        state_ = AIState::Idle;
        return;
    }
    
    f32 distance = owner_transform->position.distance_to(target_transform->position);
    
    if (distance <= retreat_range_) {
        state_ = AIState::Retreat;
        Vector2 direction = (owner_transform->position - target_transform->position).normalized();
        owner_movement->apply_force(direction * owner_movement->acceleration);
    } else if (distance <= attack_range_) {
        state_ = AIState::Attack;
        auto combat = owner->get_component<CombatComponent>();
        if (combat && combat->can_attack()) {
            combat->attack();
        }
        
        strafe_timer_ -= static_cast<f32>(delta_time);
        if (strafe_timer_ <= 0.0f) {
            strafing_left_ = !strafing_left_;
            strafe_timer_ = 1.0f;
        }
        
        Vector2 to_target = (target_transform->position - owner_transform->position).normalized();
        Vector2 strafe_dir = Vector2(-to_target.y, to_target.x) * (strafing_left_ ? 1.0f : -1.0f);
        owner_movement->apply_force(strafe_dir * owner_movement->acceleration * 0.5f);
    } else {
        state_ = AIState::Chase;
        Vector2 direction = (target_transform->position - owner_transform->position).normalized();
        owner_movement->apply_force(direction * owner_movement->acceleration);
    }
}

void TankAI::update(f64 delta_time, Entity* owner, Entity* target) {
    if (!owner || !target) {
        state_ = AIState::Idle;
        return;
    }
    
    auto owner_transform = owner->get_component<TransformComponent>();
    auto target_transform = target->get_component<TransformComponent>();
    auto owner_movement = owner->get_component<MovementComponent>();
    
    if (!owner_transform || !target_transform || !owner_movement) {
        state_ = AIState::Idle;
        return;
    }
    
    f32 distance = owner_transform->position.distance_to(target_transform->position);
    
    charge_timer_ -= static_cast<f32>(delta_time);
    
    if (is_charging_) {
        state_ = AIState::Chase;
        owner_movement->velocity = charge_direction_ * owner_movement->max_speed * 1.5f;
        
        if (distance < 10.0f || charge_timer_ <= 0.0f) {
            is_charging_ = false;
            charge_timer_ = charge_cooldown_;
        }
    } else if (distance <= charge_range_ && charge_timer_ <= 0.0f) {
        is_charging_ = true;
        charge_direction_ = (target_transform->position - owner_transform->position).normalized();
        charge_timer_ = 1.5f;
    } else if (distance <= 50.0f) {
        state_ = AIState::Attack;
        auto combat = owner->get_component<CombatComponent>();
        if (combat && combat->can_attack()) {
            combat->attack();
        }
    } else {
        state_ = AIState::Chase;
        Vector2 direction = (target_transform->position - owner_transform->position).normalized();
        owner_movement->apply_force(direction * owner_movement->acceleration);
    }
}

void AISystem::update(f64 delta_time) {
    auto player = PlayerController::instance().get_player();
    
    for (auto& [id, entry] : ai_entities_) {
        if (entry.entity && entry.controller) {
            entry.controller->update(delta_time, entry.entity, player);
        }
    }
}

void AISystem::register_entity(Entity* entity, std::unique_ptr<AIController> controller) {
    if (entity) {
        AIEntry entry;
        entry.entity = entity;
        entry.controller = std::move(controller);
        ai_entities_[entity->get_id()] = std::move(entry);
        LOG_DEBUG("Registered AI for entity: " + std::to_string(entity->get_id()));
    }
}

void AISystem::unregister_entity(EntityID id) {
    auto it = ai_entities_.find(id);
    if (it != ai_entities_.end()) {
        ai_entities_.erase(it);
        LOG_DEBUG("Unregistered AI for entity: " + std::to_string(id));
    }
}

} // namespace chronoraid
