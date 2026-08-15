#include <chronoraid/progression/progression.hpp>
#include <chronoraid/core/logger.hpp>
#include <cassert>
#include <iostream>

using namespace chronoraid;

void test_experience_and_leveling() {
    std::cout << "Testing experience and leveling..." << std::endl;
    
    PlayerProgression prog;
    
    assert(prog.get_level() == 1);
    assert(prog.get_experience() == 0);
    
    prog.add_experience(50);
    assert(prog.get_experience() == 50);
    assert(prog.get_level() == 1);
    
    prog.add_experience(50);
    assert(prog.get_level() == 2);
    assert(prog.get_skill_points() == 1);
    
    prog.add_experience(200);
    assert(prog.get_level() == 3);
    assert(prog.get_skill_points() == 2);
    
    std::cout << "Experience and leveling test passed!" << std::endl;
}

void test_skill_tree() {
    std::cout << "Testing skill tree..." << std::endl;
    
    SkillTree tree;
    
    auto health_node = tree.get_node("health_1");
    assert(health_node != nullptr);
    assert(health_node->name == "Vitality");
    
    tree.add_points(3);
    
    assert(tree.can_unlock("health_1"));
    assert(tree.unlock_node("health_1"));
    assert(health_node->current_level == 1);
    assert(health_node->is_unlocked);
    assert(tree.get_available_points() == 2);
    
    assert(tree.can_unlock("health_1"));
    assert(tree.unlock_node("health_1"));
    assert(health_node->current_level == 2);
    
    assert(tree.get_total_spent_points() == 2);
    
    auto unlocked = tree.get_unlocked_nodes();
    assert(!unlocked.empty());
    
    std::cout << "Skill tree test passed!" << std::endl;
}

void test_upgrade_branches() {
    std::cout << "Testing upgrade branches..." << std::endl;
    
    SkillTree tree;
    
    auto combat_node = tree.get_node("attack_1");
    auto survival_node = tree.get_node("health_1");
    auto temporal_node = tree.get_node("time_1");
    
    assert(combat_node->branch == UpgradeBranch::Combat);
    assert(survival_node->branch == UpgradeBranch::Survival);
    assert(temporal_node->branch == UpgradeBranch::Temporal);
    
    tree.add_points(10);
    
    assert(tree.unlock_node("attack_1"));
    assert(tree.unlock_node("health_1"));
    assert(tree.unlock_node("time_1"));
    
    f32 attack_bonus = combat_node->get_stat_bonus("attack_power");
    f32 health_bonus = survival_node->get_stat_bonus("health");
    
    assert(attack_bonus > 0.0f);
    assert(health_bonus > 0.0f);
    
    std::cout << "Upgrade branches test passed!" << std::endl;
}

void test_currency() {
    std::cout << "Testing currency..." << std::endl;
    
    PlayerProgression prog;
    
    assert(prog.get_currency() == 0);
    
    prog.add_currency(100);
    assert(prog.get_currency() == 100);
    
    assert(prog.spend_currency(50));
    assert(prog.get_currency() == 50);
    
    assert(!prog.spend_currency(100));
    assert(prog.get_currency() == 50);
    
    std::cout << "Currency test passed!" << std::endl;
}

void test_progression_reset() {
    std::cout << "Testing progression reset..." << std::endl;
    
    PlayerProgression prog;
    
    prog.add_experience(500);
    prog.add_currency(200);
    prog.add_skill_points(5);
    
    prog.reset();
    
    assert(prog.get_level() == 1);
    assert(prog.get_experience() == 0);
    assert(prog.get_currency() == 0);
    assert(prog.get_skill_points() == 0);
    
    std::cout << "Progression reset test passed!" << std::endl;
}

int main() {
    std::cout << "=== Progression System Tests ===" << std::endl;
    
    test_experience_and_leveling();
    test_skill_tree();
    test_upgrade_branches();
    test_currency();
    test_progression_reset();
    
    std::cout << "=== All Progression Tests Passed ===" << std::endl;
    
    return 0;
}
