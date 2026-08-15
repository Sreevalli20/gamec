#include <chronoraid/save/save_system.hpp>
#include <chronoraid/core/logger.hpp>
#include <cassert>
#include <iostream>
#include <filesystem>

using namespace chronoraid;

void test_save_data_serialization() {
    std::cout << "Testing save data serialization..." << std::endl;
    
    SaveData original;
    original.player_level = 10;
    original.player_experience = 5000;
    original.player_currency = 1000;
    original.player_health = 75.0f;
    original.player_max_health = 100.0f;
    original.world_seed = 12345;
    original.total_kills = 150;
    original.total_deaths = 5;
    
    original.inventory_items = {"sword_01", "potion_01"};
    original.inventory_counts = {1, 50};
    
    original.completed_quests = {"quest_main_01", "quest_side_01"};
    
    SaveSystem system;
    std::string serialized;
    assert(system.serialize(original, serialized));
    
    SaveData deserialized;
    assert(system.deserialize(serialized, deserialized));
    
    assert(deserialized.player_level == original.player_level);
    assert(deserialized.player_experience == original.player_experience);
    assert(deserialized.player_currency == original.player_currency);
    assert(deserialized.player_health == original.player_health);
    assert(deserialized.world_seed == original.world_seed);
    assert(deserialized.total_kills == original.total_kills);
    
    assert(deserialized.inventory_items.size() == original.inventory_items.size());
    assert(deserialized.completed_quests.size() == original.completed_quests.size());
    
    std::cout << "Save data serialization test passed!" << std::endl;
}

void test_save_load_file() {
    std::cout << "Testing save/load file..." << std::endl;
    
    SaveSystem system;
    system.set_save_directory("test_saves");
    
    SaveData data;
    data.player_level = 5;
    data.player_experience = 2500;
    data.world_seed = 99999;
    
    assert(system.save_game(0, data));
    assert(system.has_save(0));
    
    SaveData loaded;
    assert(system.load_game(0, loaded));
    
    assert(loaded.player_level == 5);
    assert(loaded.player_experience == 2500);
    assert(loaded.world_seed == 99999);
    
    assert(system.delete_save(0));
    assert(!system.has_save(0));
    
    std::filesystem::remove_all("test_saves");
    
    std::cout << "Save/load file test passed!" << std::endl;
}

void test_backup_system() {
    std::cout << "Testing backup system..." << std::endl;
    
    SaveSystem system;
    system.set_save_directory("test_saves");
    
    SaveData data;
    data.player_level = 8;
    data.player_experience = 4000;
    
    assert(system.save_game(1, data));
    
    data.player_level = 10;
    assert(system.save_game(1, data));
    
    assert(system.has_save(1));
    
    assert(system.restore_backup(1));
    
    SaveData loaded;
    assert(system.load_game(1, loaded));
    
    assert(loaded.player_level == 8);
    
    system.delete_save(1);
    std::filesystem::remove_all("test_saves");
    
    std::cout << "Backup system test passed!" << std::endl;
}

void test_save_version() {
    std::cout << "Testing save version..." << std::endl;
    
    SaveData data;
    assert(data.version == SaveVersion::Current);
    
    SaveSystem system;
    std::string serialized;
    system.serialize(data, serialized);
    
    SaveData loaded;
    system.deserialize(serialized, loaded);
    
    assert(loaded.version == SaveVersion::Current);
    
    std::cout << "Save version test passed!" << std::endl;
}

void test_multiple_save_slots() {
    std::cout << "Testing multiple save slots..." << std::endl;
    
    SaveSystem system;
    system.set_save_directory("test_saves");
    
    SaveData data1;
    data1.player_level = 3;
    data1.world_seed = 11111;
    
    SaveData data2;
    data2.player_level = 7;
    data2.world_seed = 22222;
    
    assert(system.save_game(0, data1));
    assert(system.save_game(1, data2));
    
    auto slots = system.get_available_save_slots();
    assert(slots.size() == 2);
    
    SaveData loaded1;
    assert(system.load_game(0, loaded1));
    assert(loaded1.player_level == 3);
    
    SaveData loaded2;
    assert(system.load_game(1, loaded2));
    assert(loaded2.player_level == 7);
    
    system.delete_save(0);
    system.delete_save(1);
    std::filesystem::remove_all("test_saves");
    
    std::cout << "Multiple save slots test passed!" << std::endl;
}

int main() {
    std::cout << "=== Save/Load System Tests ===" << std::endl;
    
    test_save_data_serialization();
    test_save_load_file();
    test_backup_system();
    test_save_version();
    test_multiple_save_slots();
    
    std::cout << "=== All Save/Load Tests Passed ===" << std::endl;
    
    return 0;
}
