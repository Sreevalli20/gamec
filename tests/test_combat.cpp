#include <chronoraid/combat/combat_system.hpp>
#include <chronoraid/entities/entity.hpp>
#include <chronoraid/entities/components.hpp>
#include <chronoraid/core/logger.hpp>
#include <cassert>
#include <iostream>

using namespace chronoraid;

void test_damage_calculation() {
    std::cout << "Testing damage calculation..." << std::endl;
    
    EntityID id = EntityManager::instance().create_entity();
    Entity* entity = EntityManager::instance().get_entity(id);
    
    entity->add_component<CombatComponent>();
    auto combat = entity->get_component<CombatComponent>();
    combat->attack_power = 50.0f;
    combat->critical_chance = 0.5f;
    combat->critical_multiplier = 2.0f;
    
    f32 total_damage = 0.0f;
    int critical_count = 0;
    
    for (int i = 0; i < 1000; ++i) {
        f32 damage = combat->calculate_damage();
        total_damage += damage;
        if (damage > combat->attack_power + EPSILON) {
            critical_count++;
        }
    }
    
    f32 avg_damage = total_damage / 1000.0f;
    f32 critical_rate = static_cast<f32>(critical_count) / 1000.0f;
    
    assert(critical_rate > 0.4f && critical_rate < 0.6f);
    assert(avg_damage > 70.0f && avg_damage < 80.0f);
    
    EntityManager::instance().destroy_entity(id);
    
    std::cout << "Damage calculation test passed!" << std::endl;
}

void test_armor_reduction() {
    std::cout << "Testing armor reduction..." << std::endl;
    
    EntityID id = EntityManager::instance().create_entity();
    Entity* entity = EntityManager::instance().get_entity(id);
    
    entity->add_component<CombatComponent>();
    auto combat = entity->get_component<CombatComponent>();
    combat->armor = 50.0f;
    
    f32 base_damage = 100.0f;
    f32 reduced_damage = combat->calculate_received_damage(base_damage);
    
    assert(reduced_damage < base_damage);
    assert(reduced_damage > 30.0f && reduced_damage < 70.0f);
    
    EntityManager::instance().destroy_entity(id);
    
    std::cout << "Armor reduction test passed!" << std::endl;
}

void test_health_component() {
    std::cout << "Testing health component..." << std::endl;
    
    EntityID id = EntityManager::instance().create_entity();
    Entity* entity = EntityManager::instance().get_entity(id);
    
    entity->add_component<HealthComponent>(100.0f);
    auto health = entity->get_component<HealthComponent>();
    
    assert(health->current_health == 100.0f);
    assert(health->max_health == 100.0f);
    assert(!health->is_dead);
    
    health->take_damage(30.0f);
    assert(health->current_health == 70.0f);
    assert(!health->is_dead);
    
    health->heal(20.0f);
    assert(health->current_health == 90.0f);
    
    health->take_damage(100.0f);
    assert(health->current_health == 0.0f);
    assert(health->is_dead);
    
    EntityManager::instance().destroy_entity(id);
    
    std::cout << "Health component test passed!" << std::endl;
}

void test_attack_cooldown() {
    std::cout << "Testing attack cooldown..." << std::endl;
    
    EntityID id = EntityManager::instance().create_entity();
    Entity* entity = EntityManager::instance().get_entity(id);
    
    entity->add_component<CombatComponent>();
    auto combat = entity->get_component<CombatComponent>();
    combat->attack_speed = 2.0f;
    
    assert(combat->can_attack());
    
    combat->attack();
    assert(!combat->can_attack());
    
    combat->update(0.4);
    assert(!combat->can_attack());
    
    combat->update(0.1);
    assert(combat->can_attack());
    
    EntityManager::instance().destroy_entity(id);
    
    std::cout << "Attack cooldown test passed!" << std::endl;
}

void test_combat_system() {
    std::cout << "Testing combat system..." << std::endl;
    
    EntityID attacker_id = EntityManager::instance().create_entity();
    Entity* attacker = EntityManager::instance().get_entity(attacker_id);
    attacker->add_component<TransformComponent>();
    attacker->add_component<HealthComponent>(100.0f);
    attacker->add_component<CombatComponent>();
    attacker->get_component<CombatComponent>()->attack_power = 25.0f;
    
    EntityID target_id = EntityManager::instance().create_entity();
    Entity* target = EntityManager::instance().get_entity(target_id);
    target->add_component<TransformComponent>();
    target->add_component<HealthComponent>(100.0f);
    target->add_component<CombatComponent>();
    
    AttackResult result = CombatSystem::instance().perform_attack(attacker, target);
    
    assert(result.hit);
    assert(result.damage > 0.0f);
    
    auto target_health = target->get_component<HealthComponent>();
    assert(target_health->current_health < 100.0f);
    
    EntityManager::instance().destroy_entity(attacker_id);
    EntityManager::instance().destroy_entity(target_id);
    
    std::cout << "Combat system test passed!" << std::endl;
}

int main() {
    std::cout << "=== Combat System Tests ===" << std::endl;
    
    test_damage_calculation();
    test_armor_reduction();
    test_health_component();
    test_attack_cooldown();
    test_combat_system();
    
    std::cout << "=== All Combat Tests Passed ===" << std::endl;
    
    return 0;
}
