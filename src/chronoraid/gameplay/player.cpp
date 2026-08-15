#include <chronoraid/gameplay/player.hpp>
#include <chronoraid/core/input.hpp>
#include <chronoraid/core/logger.hpp>
#include <chronoraid/entities/entity_manager.hpp>
#include <chronoraid/combat/combat_system.hpp>
#include <chronoraid/world/world.hpp>
#include <chronoraid/gameplay/time_mechanic.hpp>
#include <chronoraid/progression/progression.hpp>

namespace chronoraid {

Player::Player(EntityID id) : Entity(id) {
    set_name("Player");
    
    add_component<TransformComponent>();
    add_component<HealthComponent>(100.0f);
    add_component<MovementComponent>(150.0f);
    add_component<CombatComponent>(15.0f);
    add_component<ExperienceComponent>();
    add_component<CollisionComponent>();
    
    auto collision = get_component<CollisionComponent>();
    if (collision) {
        collision->bounds = Rect(0, 0, 1.0f, 1.0f);
        collision->collision_layer = 1;
        collision->collision_mask = 2;
    }
}

void Player::update(f64 delta_time) {
    Entity::update(delta_time);
    
    energy_ = min(max_energy_, energy_ + energy_regen_rate_ * static_cast<f32>(delta_time));
    
    update_abilities(delta_time);
    
    auto health = get_component<HealthComponent>();
    if (health && health->is_dead) {
        on_death();
    }
}

void Player::move(const Vector2& direction) {
    auto movement = get_component<MovementComponent>();
    if (movement) {
        movement->apply_force(direction * movement->acceleration);
    }
}

void Player::attack() {
    auto combat = get_combat();
    if (combat && combat->can_attack()) {
        combat->attack();
        
        auto transform = get_transform();
        if (transform) {
            Vector2 attack_pos = transform->position + transform->forward() * 2.0f;
            
            for (auto& [id, entity] : *(&EntityManager::instance())) {
                if (entity && entity != this && !entity->is_destroyed()) {
                    auto enemy_transform = entity->get_component<TransformComponent>();
                    if (enemy_transform) {
                        f32 distance = attack_pos.distance_to(enemy_transform->position);
                        if (distance < 2.0f) {
                            CombatSystem::instance().perform_attack(this, entity);
                        }
                    }
                }
            }
        }
    }
}

void Player::use_ability(u32 ability_index) {
    auto time_mgr = &TimeManager::instance();
    
    switch (ability_index) {
        case 0:
            if (energy_ >= 25.0f) {
                energy_ -= 25.0f;
                time_mgr->use_ability(TimeAbility::TemporalDash, this);
            }
            break;
        case 1:
            if (energy_ >= 30.0f) {
                energy_ -= 30.0f;
                time_mgr->use_ability(TimeAbility::TimeFreeze, this);
            }
            break;
        case 2:
            if (energy_ >= 40.0f) {
                energy_ -= 40.0f;
                time_mgr->use_ability(TimeAbility::Rewind, this);
            }
            break;
    }
}

void Player::interact() {
}

void Player::take_damage(f32 amount, Entity* source) {
    auto health = get_health_comp();
    if (health) {
        health->take_damage(amount);
        Statistics::instance().record_damage_received(amount);
    }
}

void Player::heal(f32 amount) {
    auto health = get_component<HealthComponent>();
    if (health) {
        health->heal(amount);
    }
}

void Player::add_experience(u32 amount) {
    auto exp = get_experience_comp();
    if (exp) {
        exp->add_xp(amount);
    }
}

bool Player::is_alive() const {
    auto health = get_health_comp();
    return health && !health->is_dead;
}

void Player::on_death() {
    Statistics::instance().record_death();
    LOG_INFO("Player died");
}

void Player::update_abilities(f64 delta_time) {
}

void PlayerController::update(f64 delta_time) {
    if (!player_) return;
    
    auto movement = player_->get_movement();
    auto transform = player_->get_transform();
    
    if (movement && transform) {
        f32 dt = static_cast<f32>(delta_time);
        transform->position += movement->velocity * dt;
        
        const auto& tilemap = World::instance().get_tile_map();
        auto [tile_x, tile_y] = tilemap.world_to_tile(transform->position);
        
        if (tilemap.is_solid(tile_x, tile_y)) {
            transform->position -= movement->velocity * dt;
            movement->velocity = Vector2::zero();
        }
        
        transform->position.x = clamp(transform->position.x, 0.0f, static_cast<f32>(tilemap.get_width() - 1));
        transform->position.y = clamp(transform->position.y, 0.0f, static_cast<f32>(tilemap.get_height() - 1));
    }
}

void PlayerController::process_input() {
    if (!player_) return;
    
    input_direction_ = Vector2::zero();
    
    auto& input = InputManager::instance();
    
    if (input.is_key_held(KeyCode::W) || input.is_key_held(KeyCode::Up)) {
        input_direction_.y += 1.0f;
    }
    if (input.is_key_held(KeyCode::S) || input.is_key_held(KeyCode::Down)) {
        input_direction_.y -= 1.0f;
    }
    if (input.is_key_held(KeyCode::A) || input.is_key_held(KeyCode::Left)) {
        input_direction_.x -= 1.0f;
    }
    if (input.is_key_held(KeyCode::D) || input.is_key_held(KeyCode::Right)) {
        input_direction_.x += 1.0f;
    }
    
    if (input_direction_.length_squared() > 0.0f) {
        input_direction_ = input_direction_.normalized();
        auto movement = player_->get_component<MovementComponent>();
        if (movement) {
            movement->apply_force(input_direction_ * movement->acceleration);
        }
    }
    
    if (input.is_key_pressed(KeyCode::Space)) {
        auto combat = player_->get_component<CombatComponent>();
        if (combat && combat->can_attack()) {
            combat->attack();
        }
    }
    
    if (input.is_key_pressed(KeyCode::Q)) {
        auto time_mgr = &TimeManager::instance();
        auto health = player_->get_component<HealthComponent>();
        if (health && health->current_health >= 25.0f) {
            health->current_health -= 25.0f;
            time_mgr->use_ability(TimeAbility::TemporalDash, player_);
        }
    }
    if (input.is_key_pressed(KeyCode::E)) {
        auto time_mgr = &TimeManager::instance();
        auto health = player_->get_component<HealthComponent>();
        if (health && health->current_health >= 30.0f) {
            health->current_health -= 30.0f;
            time_mgr->use_ability(TimeAbility::TimeFreeze, player_);
        }
    }
    if (input.is_key_pressed(KeyCode::R)) {
        auto time_mgr = &TimeManager::instance();
        auto health = player_->get_component<HealthComponent>();
        if (health && health->current_health >= 40.0f) {
            health->current_health -= 40.0f;
            time_mgr->use_ability(TimeAbility::Rewind, player_);
        }
    }
}

} // namespace chronoraid
