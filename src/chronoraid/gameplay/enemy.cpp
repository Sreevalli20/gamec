#include <chronoraid/gameplay/enemy.hpp>
#include <chronoraid/core/logger.hpp>
#include <chronoraid/entities/entity_manager.hpp>
#include <chronoraid/combat/combat_system.hpp>
#include <chronoraid/inventory/loot_generator.hpp>
#include <chronoraid/gameplay/player.hpp>

namespace chronoraid {

Enemy::Enemy(EntityID id, EnemyType type) : Entity(id), type_(type) {
    set_name(get_enemy_name(type));
    
    add_component<TransformComponent>();
    add_component<HealthComponent>();
    add_component<MovementComponent>();
    add_component<CombatComponent>();
    add_component<CollisionComponent>();
    
    initialize_stats();
    
    auto collision = get_component<CollisionComponent>();
    if (collision) {
        collision->bounds = Rect(0, 0, 1.0f, 1.0f);
        collision->collision_layer = 2;
        collision->collision_mask = 1;
    }
    
    switch (type_) {
        case EnemyType::ChronoDrone:
            ai_controller_ = std::make_unique<ChaseAI>(200.0f, 30.0f);
            break;
        case EnemyType::RiftBeast:
            ai_controller_ = std::make_unique<TankAI>(400.0f, 5.0f);
            break;
        case EnemyType::VoidArcher:
            ai_controller_ = std::make_unique<RangedAI>(250.0f, 80.0f);
            break;
        case EnemyType::TimeAssassin:
            ai_controller_ = std::make_unique<ChaseAI>(300.0f, 25.0f);
            break;
        case EnemyType::TemporalTank:
            ai_controller_ = std::make_unique<TankAI>(350.0f, 8.0f);
            break;
        case EnemyType::RiftMage:
            ai_controller_ = std::make_unique<RangedAI>(300.0f, 100.0f);
            break;
        default:
            ai_controller_ = std::make_unique<ChaseAI>(200.0f, 30.0f);
            break;
    }
    
    AISystem::instance().register_entity(this, std::move(ai_controller_));
}

void Enemy::update(f64 delta_time) {
    Entity::update(delta_time);
    
    auto health = get_health_comp();
    if (health && health->is_dead) {
        on_death();
        return;
    }
    
    auto movement = get_movement();
    auto transform = get_transform();
    
    if (movement && transform) {
        f32 dt = static_cast<f32>(delta_time);
        transform->position += movement->velocity * dt;
        
        const auto& tilemap = World::instance().get_tile_map();
        auto [tile_x, tile_y] = tilemap.world_to_tile(transform->position);
        
        if (tilemap.is_solid(tile_x, tile_y)) {
            transform->position -= movement->velocity * dt;
            movement->velocity = Vector2::zero();
        }
    }
}

void Enemy::take_damage(f32 amount, Entity* source) {
    auto health = get_health_comp();
    if (health) {
        health->take_damage(amount);
    }
    
    if (target_ == nullptr && source) {
        target_ = source;
    }
}

void Enemy::on_death() {
    destroy();
    
    Statistics::instance().record_enemy_kill(is_boss());
    
    auto player = PlayerController::instance().get_player();
    if (player) {
        player->add_experience(experience_reward_);
    }
    
    auto loot = LootGenerator::instance().generate_loot(
        player ? player->get_level() : 1,
        1,
        is_boss() ? 3 : 1
    );
    
    AISystem::instance().unregister_entity(get_id());
    
    LOG_INFO(name_ + " defeated");
}

const char* Enemy::get_enemy_name(EnemyType type) {
    switch (type) {
        case EnemyType::ChronoDrone: return "Chrono Drone";
        case EnemyType::RiftBeast: return "Rift Beast";
        case EnemyType::VoidArcher: return "Void Archer";
        case EnemyType::TimeAssassin: return "Time Assassin";
        case EnemyType::TemporalTank: return "Temporal Tank";
        case EnemyType::RiftMage: return "Rift Mage";
        case EnemyType::Boss: return "Boss";
        default: return "Unknown Enemy";
    }
}

void Enemy::initialize_stats() {
    f32 diff_mult = EnemyFactory::instance().difficulty_multiplier_;
    
    auto health = get_health_comp();
    auto combat = get_combat();
    auto movement = get_movement();
    
    switch (type_) {
        case EnemyType::ChronoDrone:
            if (health) { health->max_health = 50.0f * diff_mult; health->current_health = health->max_health; }
            if (combat) { combat->attack_power = 8.0f * diff_mult; combat->attack_speed = 1.5f; }
            if (movement) { movement->speed = 80.0f; }
            experience_reward_ = 15;
            break;
            
        case EnemyType::RiftBeast:
            if (health) { health->max_health = 120.0f * diff_mult; health->current_health = health->max_health; }
            if (combat) { combat->attack_power = 15.0f * diff_mult; combat->attack_speed = 0.8f; }
            if (movement) { movement->speed = 60.0f; }
            experience_reward_ = 30;
            break;
            
        case EnemyType::VoidArcher:
            if (health) { health->max_health = 40.0f * diff_mult; health->current_health = health->max_health; }
            if (combat) { combat->attack_power = 12.0f * diff_mult; combat->attack_speed = 1.2f; }
            if (movement) { movement->speed = 70.0f; }
            experience_reward_ = 25;
            break;
            
        case EnemyType::TimeAssassin:
            if (health) { health->max_health = 60.0f * diff_mult; health->current_health = health->max_health; }
            if (combat) { combat->attack_power = 20.0f * diff_mult; combat->attack_speed = 2.0f; combat->critical_chance = 0.15f; }
            if (movement) { movement->speed = 120.0f; }
            experience_reward_ = 40;
            break;
            
        case EnemyType::TemporalTank:
            if (health) { health->max_health = 200.0f * diff_mult; health->current_health = health->max_health; }
            if (combat) { combat->attack_power = 25.0f * diff_mult; combat->attack_speed = 0.6f; combat->armor = 20.0f; }
            if (movement) { movement->speed = 40.0f; }
            experience_reward_ = 50;
            break;
            
        case EnemyType::RiftMage:
            if (health) { health->max_health = 55.0f * diff_mult; health->current_health = health->max_health; }
            if (combat) { combat->attack_power = 18.0f * diff_mult; combat->attack_speed = 1.0f; }
            if (movement) { movement->speed = 65.0f; }
            experience_reward_ = 35;
            break;
            
        case EnemyType::Boss:
            if (health) { health->max_health = 500.0f * diff_mult; health->current_health = health->max_health; }
            if (combat) { combat->attack_power = 30.0f * diff_mult; combat->attack_speed = 1.0f; combat->armor = 30.0f; }
            if (movement) { movement->speed = 50.0f; }
            experience_reward_ = 200;
            break;
            
        default:
            if (health) { health->max_health = 50.0f; health->current_health = health->max_health; }
            if (combat) { combat->attack_power = 10.0f; }
            experience_reward_ = 10;
            break;
    }
}

Enemy* EnemyFactory::create_enemy(EnemyType type, const Vector2& position) {
    EntityID id = EntityManager::instance().create_entity();
    auto entity = EntityManager::instance().get_entity(id);
    auto enemy = static_cast<Enemy*>(entity);
    
    if (enemy) {
        auto transform = enemy->get_transform();
        if (transform) {
            transform->position = position;
        }
    }
    
    return enemy;
}

Enemy* EnemyFactory::create_boss(const std::string& boss_id, const Vector2& position) {
    EntityID id = EntityManager::instance().create_entity();
    auto entity = EntityManager::instance().get_entity(id);
    
    Enemy* boss = nullptr;
    
    if (boss_id == "clockmaker") {
        boss = static_cast<ClockmakerBoss*>(entity);
    } else if (boss_id == "paradox_beast") {
        boss = static_cast<ParadoxBeastBoss*>(entity);
    } else if (boss_id == "null_emperor") {
        boss = static_cast<NullEmperorBoss*>(entity);
    } else {
        boss = static_cast<Enemy*>(entity);
    }
    
    if (boss) {
        auto transform = boss->get_transform();
        if (transform) {
            transform->position = position;
        }
    }
    
    return boss;
}

} // namespace chronoraid
