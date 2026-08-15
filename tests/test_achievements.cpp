#include <chronoraid/achievements/achievement.hpp>
#include <chronoraid/core/logger.hpp>
#include <cassert>
#include <iostream>

using namespace chronoraid;

void test_achievement_registration() {
    std::cout << "Testing achievement registration..." << std::endl;
    
    AchievementManager& manager = AchievementManager::instance();
    
    Achievement ach1;
    ach1.id = "first_blood";
    ach1.name = "First Blood";
    ach1.description = "Defeat your first enemy";
    ach1.type = AchievementType::KillCount;
    ach1.target_value = 1;
    
    Achievement ach2;
    ach2.id = "boss_slayer";
    ach2.name = "Boss Slayer";
    ach2.description = "Defeat a boss";
    ach2.type = AchievementType::BossDefeated;
    ach2.target_value = 1;
    
    manager.register_achievement(ach1);
    manager.register_achievement(ach2);
    
    assert(manager.get_achievement("first_blood") != nullptr);
    assert(manager.get_achievement("boss_slayer") != nullptr);
    
    assert(manager.get_total_count() == 2);
    
    std::cout << "Achievement registration test passed!" << std::endl;
}

void test_achievement_unlock() {
    std::cout << "Testing achievement unlock..." << std::endl;
    
    AchievementManager& manager = AchievementManager::instance();
    
    Achievement ach;
    ach.id = "test_ach";
    ach.name = "Test Achievement";
    ach.type = AchievementType::KillCount;
    ach.target_value = 10;
    
    manager.register_achievement(ach);
    
    assert(!manager.get_achievement("test_ach")->is_unlocked);
    
    assert(manager.unlock_achievement("test_ach"));
    assert(manager.get_achievement("test_ach")->is_unlocked);
    
    assert(!manager.unlock_achievement("test_ach"));
    
    std::cout << "Achievement unlock test passed!" << std::endl;
}

void test_achievement_progress() {
    std::cout << "Testing achievement progress..." << std::endl;
    
    AchievementManager& manager = AchievementManager::instance();
    
    Achievement ach;
    ach.id = "progress_ach";
    ach.name = "Progress Achievement";
    ach.type = AchievementType::KillCount;
    ach.target_value = 100;
    
    manager.register_achievement(ach);
    
    manager.update_progress("progress_ach", 50);
    assert(manager.get_achievement("progress_ach")->current_value == 50);
    assert(!manager.get_achievement("progress_ach")->is_unlocked);
    
    manager.update_progress("progress_ach", 100);
    assert(manager.get_achievement("progress_ach")->is_unlocked);
    
    std::cout << "Achievement progress test passed!" << std::endl;
}

void test_achievement_callbacks() {
    std::cout << "Testing achievement callbacks..." << std::endl;
    
    AchievementManager& manager = AchievementManager::instance();
    
    bool callback_called = false;
    
    Achievement ach;
    ach.id = "callback_ach";
    ach.name = "Callback Achievement";
    ach.type = AchievementType::KillCount;
    ach.target_value = 1;
    
    manager.register_achievement(ach);
    manager.register_callback("callback_ach", [&callback_called]() {
        callback_called = true;
    });
    
    manager.unlock_achievement("callback_ach");
    
    assert(callback_called);
    
    std::cout << "Achievement callbacks test passed!" << std::endl;
}

void test_achievement_completion_percentage() {
    std::cout << "Testing achievement completion percentage..." << std::endl;
    
    AchievementManager& manager = AchievementManager::instance();
    
    Achievement ach1;
    ach1.id = "ach1";
    ach1.name = "Achievement 1";
    ach1.type = AchievementType::KillCount;
    ach1.target_value = 1;
    
    Achievement ach2;
    ach2.id = "ach2";
    ach2.name = "Achievement 2";
    ach2.type = AchievementType::KillCount;
    ach2.target_value = 1;
    
    Achievement ach3;
    ach3.id = "ach3";
    ach3.name = "Achievement 3";
    ach3.type = AchievementType::KillCount;
    ach3.target_value = 1;
    
    manager.register_achievement(ach1);
    manager.register_achievement(ach2);
    manager.register_achievement(ach3);
    
    assert(manager.get_completion_percentage() == 0.0f);
    
    manager.unlock_achievement("ach1");
    assert(manager.get_completion_percentage() > 0.3f && manager.get_completion_percentage() < 0.4f);
    
    manager.unlock_achievement("ach2");
    assert(manager.get_completion_percentage() > 0.6f && manager.get_completion_percentage() < 0.7f);
    
    manager.unlock_achievement("ach3");
    assert(manager.get_completion_percentage() == 1.0f);
    
    std::cout << "Achievement completion percentage test passed!" << std::endl;
}

void test_hidden_achievements() {
    std::cout << "Testing hidden achievements..." << std::endl;
    
    AchievementManager& manager = AchievementManager::instance();
    
    Achievement hidden_ach;
    hidden_ach.id = "secret";
    hidden_ach.name = "Secret Achievement";
    hidden_ach.is_hidden = true;
    hidden_ach.type = AchievementType::Special;
    hidden_ach.target_value = 1;
    
    manager.register_achievement(hidden_ach);
    
    assert(manager.get_achievement("secret")->is_hidden);
    
    auto all = manager.get_all_achievements();
    auto locked = manager.get_locked_achievements();
    
    assert(all.size() > 0);
    
    std::cout << "Hidden achievements test passed!" << std::endl;
}

int main() {
    std::cout << "=== Achievement System Tests ===" << std::endl;
    
    test_achievement_registration();
    test_achievement_unlock();
    test_achievement_progress();
    test_achievement_callbacks();
    test_achievement_completion_percentage();
    test_hidden_achievements();
    
    std::cout << "=== All Achievement Tests Passed ===" << std::endl;
    
    return 0;
}
