#include <chronoraid/quests/quest.hpp>
#include <chronoraid/core/logger.hpp>
#include <cassert>
#include <iostream>

using namespace chronoraid;

void test_quest_creation() {
    std::cout << "Testing quest creation..." << std::endl;
    
    auto quest = std::make_shared<Quest>("quest_01", "First Steps", QuestType::Main);
    
    assert(quest->get_id() == "quest_01");
    assert(quest->get_name() == "First Steps");
    assert(quest->get_type() == QuestType::Main);
    assert(quest->get_state() == QuestState::NotStarted);
    
    std::cout << "Quest creation test passed!" << std::endl;
}

void test_quest_objectives() {
    std::cout << "Testing quest objectives..." << std::endl;
    
    auto quest = std::make_shared<Quest>("quest_02", "Kill Enemies", QuestType::Main);
    
    QuestObjective obj1;
    obj1.id = "kill_5";
    obj1.description = "Kill 5 enemies";
    obj1.target_count = 5;
    obj1.current_count = 0;
    
    QuestObjective obj2;
    obj2.id = "collect_item";
    obj2.description = "Collect the item";
    obj2.target_count = 1;
    obj2.current_count = 0;
    obj2.is_optional = true;
    
    quest->add_objective(obj1);
    quest->add_objective(obj2);
    
    assert(quest->get_objectives().size() == 2);
    
    quest->update_objective("kill_5", 3);
    assert(quest->get_objectives()[0].current_count == 3);
    
    quest->update_objective("kill_5", 5);
    assert(quest->get_objectives()[0].is_completed);
    
    std::cout << "Quest objectives test passed!" << std::endl;
}

void test_quest_progress() {
    std::cout << "Testing quest progress..." << std::endl;
    
    auto quest = std::make_shared<Quest>("quest_03", "Progress Quest", QuestType::Main);
    
    QuestObjective obj;
    obj.id = "obj1";
    obj.description = "Complete objective";
    obj.target_count = 10;
    
    quest->add_objective(obj);
    
    assert(quest->get_progress_percentage() == 0.0f);
    
    quest->update_objective("obj1", 5);
    assert(quest->get_progress_percentage() == 0.5f);
    
    quest->update_objective("obj1", 10);
    assert(quest->get_progress_percentage() == 1.0f);
    
    std::cout << "Quest progress test passed!" << std::endl;
}

void test_quest_states() {
    std::cout << "Testing quest states..." << std::endl;
    
    auto quest = std::make_shared<Quest>("quest_04", "State Quest", QuestType::Main);
    
    assert(quest->get_state() == QuestState::NotStarted);
    assert(!quest->is_active());
    assert(!quest->is_completed());
    
    quest->set_state(QuestState::InProgress);
    assert(quest->is_active());
    
    quest->set_state(QuestState::Completed);
    assert(quest->is_completed());
    
    quest->set_state(QuestState::Failed);
    assert(quest->is_failed());
    
    std::cout << "Quest states test passed!" << std::endl;
}

void test_quest_manager() {
    std::cout << "Testing quest manager..." << std::endl;
    
    QuestManager& manager = QuestManager::instance();
    
    auto quest1 = std::make_shared<Quest>("quest_05", "Quest 1", QuestType::Main);
    auto quest2 = std::make_shared<Quest>("quest_06", "Quest 2", QuestType::Side);
    
    manager.register_quest(quest1);
    manager.register_quest(quest2);
    
    assert(manager.get_quest("quest_05") != nullptr);
    assert(manager.get_quest("quest_06") != nullptr);
    
    assert(manager.start_quest("quest_05"));
    assert(manager.get_quest("quest_05")->is_active());
    
    assert(manager.complete_quest("quest_05"));
    assert(manager.get_quest("quest_05")->is_completed());
    
    assert(manager.start_quest("quest_06"));
    assert(manager.abandon_quest("quest_06"));
    assert(manager.get_quest("quest_06")->get_state() == QuestState::Abandoned);
    
    std::cout << "Quest manager test passed!" << std::endl;
}

void test_quest_prerequisites() {
    std::cout << "Testing quest prerequisites..." << std::endl;
    
    QuestManager& manager = QuestManager::instance();
    
    auto prereq = std::make_shared<Quest>("prereq_quest", "Prerequisite", QuestType::Main);
    auto main_quest = std::make_shared<Quest>("main_quest", "Main Quest", QuestType::Main);
    main_quest->set_prerequisite_quest("prereq_quest");
    
    manager.register_quest(prereq);
    manager.register_quest(main_quest);
    
    assert(!manager.start_quest("main_quest"));
    
    manager.start_quest("prereq_quest");
    manager.complete_quest("prereq_quest");
    
    assert(manager.start_quest("main_quest"));
    
    std::cout << "Quest prerequisites test passed!" << std::endl;
}

void test_quest_rewards() {
    std::cout << "Testing quest rewards..." << std::endl;
    
    auto quest = std::make_shared<Quest>("reward_quest", "Reward Quest", QuestType::Main);
    
    QuestReward reward;
    reward.experience = 500;
    reward.currency = 100;
    reward.item_ids = {"item_01", "item_02"};
    
    quest->set_reward(reward);
    
    assert(quest->get_reward().experience == 500);
    assert(quest->get_reward().currency == 100);
    assert(quest->get_reward().item_ids.size() == 2);
    
    std::cout << "Quest rewards test passed!" << std::endl;
}

void test_quest_callbacks() {
    std::cout << "Testing quest callbacks..." << std::endl;
    
    auto quest = std::make_shared<Quest>("callback_quest", "Callback Quest", QuestType::Main);
    
    bool callback_called = false;
    quest->set_on_completed_callback([&callback_called]() {
        callback_called = true;
    });
    
    QuestObjective obj;
    obj.id = "obj";
    obj.description = "Objective";
    obj.target_count = 1;
    quest->add_objective(obj);
    
    quest->set_state(QuestState::InProgress);
    quest->update_objective("obj", 1);
    
    assert(callback_called);
    
    std::cout << "Quest callbacks test passed!" << std::endl;
}

int main() {
    std::cout << "=== Quest System Tests ===" << std::endl;
    
    test_quest_creation();
    test_quest_objectives();
    test_quest_progress();
    test_quest_states();
    test_quest_manager();
    test_quest_prerequisites();
    test_quest_rewards();
    test_quest_callbacks();
    
    std::cout << "=== All Quest Tests Passed ===" << std::endl;
    
    return 0;
}
