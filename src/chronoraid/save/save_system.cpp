#include <chronoraid/save/save_system.hpp>
#include <chronoraid/core/logger.hpp>
#include <chronoraid/gameplay/player.hpp>
#include <chronoraid/statistics/statistics.hpp>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace chronoraid {

bool SaveSystem::save_game(u32 slot, const SaveData& data) {
    std::string filename = get_save_filename(slot);
    
    std::string content;
    if (!serialize(data, content)) {
        LOG_ERROR("Failed to serialize save data");
        return false;
    }
    
    if (!write_file(filename, content)) {
        LOG_ERROR("Failed to write save file: " + filename);
        return false;
    }
    
    create_backup(slot);
    
    LOG_INFO("Game saved to slot " + std::to_string(slot));
    return true;
}

bool SaveSystem::load_game(u32 slot, SaveData& data) {
    std::string filename = get_save_filename(slot);
    
    std::string content;
    if (!read_file(filename, content)) {
        LOG_ERROR("Failed to read save file: " + filename);
        return false;
    }
    
    if (!deserialize(content, data)) {
        LOG_ERROR("Failed to deserialize save data");
        return false;
    }
    
    if (data.version != SaveVersion::Current) {
        LOG_WARNING("Save version mismatch, attempting to load anyway");
    }
    
    LOG_INFO("Game loaded from slot " + std::to_string(slot));
    return true;
}

bool SaveSystem::delete_save(u32 slot) {
    std::string filename = get_save_filename(slot);
    
    try {
        if (std::filesystem::exists(filename)) {
            std::filesystem::remove(filename);
            LOG_INFO("Deleted save slot " + std::to_string(slot));
            return true;
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to delete save: " + std::string(e.what()));
    }
    
    return false;
}

bool SaveSystem::has_save(u32 slot) const {
    std::string filename = get_save_filename(slot);
    try {
        return std::filesystem::exists(filename);
    } catch (...) {
        return false;
    }
}

std::vector<u32> SaveSystem::get_available_save_slots() const {
    std::vector<u32> slots;
    for (u32 i = 0; i < 10; ++i) {
        if (has_save(i)) {
            slots.push_back(i);
        }
    }
    return slots;
}

u32 SaveSystem::get_latest_save_slot() const {
    auto slots = get_available_save_slots();
    if (slots.empty()) return 0;
    
    u32 latest = slots[0];
    std::filesystem::file_time_type latest_time;
    
    for (u32 slot : slots) {
        std::string filename = get_save_filename(slot);
        try {
            auto time = std::filesystem::last_write_time(filename);
            if (time > latest_time) {
                latest_time = time;
                latest = slot;
            }
        } catch (...) {}
    }
    
    return latest;
}

bool SaveSystem::create_backup(u32 slot) {
    std::string filename = get_save_filename(slot);
    std::string backup = get_backup_filename(slot);
    
    try {
        if (std::filesystem::exists(filename)) {
            std::filesystem::copy_file(filename, backup, 
                std::filesystem::copy_options::overwrite_existing);
            return true;
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to create backup: " + std::string(e.what()));
    }
    
    return false;
}

bool SaveSystem::restore_backup(u32 slot) {
    std::string filename = get_save_filename(slot);
    std::string backup = get_backup_filename(slot);
    
    try {
        if (std::filesystem::exists(backup)) {
            std::filesystem::copy_file(backup, filename,
                std::filesystem::copy_options::overwrite_existing);
            LOG_INFO("Restored backup for slot " + std::to_string(slot));
            return true;
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to restore backup: " + std::string(e.what()));
    }
    
    return false;
}

void SaveSystem::set_save_directory(const std::string& directory) {
    save_directory_ = directory;
    try {
        std::filesystem::create_directories(directory);
    } catch (...) {}
}

std::string SaveSystem::get_save_filename(u32 slot) const {
    return save_directory_ + "/save_" + std::to_string(slot) + ".dat";
}

std::string SaveSystem::get_backup_filename(u32 slot) const {
    return save_directory_ + "/save_" + std::to_string(slot) + ".bak";
}

bool SaveSystem::serialize(const SaveData& data, std::string& output) const {
    std::ostringstream oss;
    
    oss << "version=" << static_cast<int>(data.version) << "\n";
    oss << "player_level=" << data.player_level << "\n";
    oss << "player_experience=" << data.player_experience << "\n";
    oss << "player_currency=" << data.player_currency << "\n";
    oss << "player_health=" << data.player_health << "\n";
    oss << "player_max_health=" << data.player_max_health << "\n";
    oss << "world_seed=" << data.world_seed << "\n";
    oss << "current_room=" << data.current_room_index << "\n";
    oss << "total_deaths=" << data.total_deaths << "\n";
    oss << "total_kills=" << data.total_kills << "\n";
    oss << "total_play_time=" << data.total_play_time << "\n";
    
    oss << "inventory_items=";
    for (size_t i = 0; i < data.inventory_items.size(); ++i) {
        if (i > 0) oss << ",";
        oss << data.inventory_items[i];
    }
    oss << "\n";
    
    oss << "inventory_counts=";
    for (size_t i = 0; i < data.inventory_counts.size(); ++i) {
        if (i > 0) oss << ",";
        oss << data.inventory_counts[i];
    }
    oss << "\n";
    
    oss << "completed_quests=";
    for (size_t i = 0; i < data.completed_quests.size(); ++i) {
        if (i > 0) oss << ",";
        oss << data.completed_quests[i];
    }
    oss << "\n";
    
    oss << "unlocked_achievements=";
    for (size_t i = 0; i < data.unlocked_achievements.size(); ++i) {
        if (i > 0) oss << ",";
        oss << data.unlocked_achievements[i];
    }
    oss << "\n";
    
    output = oss.str();
    return true;
}

bool SaveSystem::deserialize(const std::string& input, SaveData& data) const {
    std::istringstream iss(input);
    std::string line;
    
    while (std::getline(iss, line)) {
        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;
        
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        if (key == "version") {
            data.version = static_cast<SaveVersion>(std::stoi(value));
        } else if (key == "player_level") {
            data.player_level = std::stoul(value);
        } else if (key == "player_experience") {
            data.player_experience = std::stoul(value);
        } else if (key == "player_currency") {
            data.player_currency = std::stoul(value);
        } else if (key == "player_health") {
            data.player_health = std::stof(value);
        } else if (key == "player_max_health") {
            data.player_max_health = std::stof(value);
        } else if (key == "world_seed") {
            data.world_seed = std::stoul(value);
        } else if (key == "current_room") {
            data.current_room_index = std::stoul(value);
        } else if (key == "total_deaths") {
            data.total_deaths = std::stoul(value);
        } else if (key == "total_kills") {
            data.total_kills = std::stoul(value);
        } else if (key == "total_play_time") {
            data.total_play_time = std::stoull(value);
        } else if (key == "inventory_items") {
            data.inventory_items.clear();
            std::istringstream item_stream(value);
            std::string item;
            while (std::getline(item_stream, item, ',')) {
                data.inventory_items.push_back(item);
            }
        } else if (key == "inventory_counts") {
            data.inventory_counts.clear();
            std::istringstream count_stream(value);
            std::string count;
            while (std::getline(count_stream, count, ',')) {
                data.inventory_counts.push_back(std::stoul(count));
            }
        } else if (key == "completed_quests") {
            data.completed_quests.clear();
            std::istringstream quest_stream(value);
            std::string quest;
            while (std::getline(quest_stream, quest, ',')) {
                data.completed_quests.push_back(quest);
            }
        } else if (key == "unlocked_achievements") {
            data.unlocked_achievements.clear();
            std::istringstream ach_stream(value);
            std::string ach;
            while (std::getline(ach_stream, ach, ',')) {
                data.unlocked_achievements.push_back(ach);
            }
        }
    }
    
    return true;
}

bool SaveSystem::write_file(const std::string& filename, const std::string& content) const {
    try {
        std::filesystem::create_directories(save_directory_);
        std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) return false;
        file << content;
        file.close();
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Write file error: " + std::string(e.what()));
        return false;
    }
}

bool SaveSystem::read_file(const std::string& filename, std::string& content) const {
    try {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) return false;
        
        std::ostringstream oss;
        oss << file.rdbuf();
        content = oss.str();
        file.close();
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Read file error: " + std::string(e.what()));
        return false;
    }
}

void AutoSave::enable(f32 interval_seconds) {
    is_enabled_ = true;
    save_interval_ = interval_seconds;
    save_timer_ = 0.0f;
}

void AutoSave::disable() {
    is_enabled_ = false;
}

void AutoSave::update(f64 delta_time) {
    if (!is_enabled_) return;
    
    save_timer_ += static_cast<f32>(delta_time);
    
    if (save_timer_ >= save_interval_) {
        trigger_save();
        save_timer_ = 0.0f;
    }
}

void AutoSave::trigger_save() {
    if (!is_enabled_) return;
    
    SaveData data;
    
    auto player = PlayerController::instance().get_player();
    if (player) {
        data.player_level = player->get_level();
        data.player_experience = player->get_experience();
        data.player_health = player->get_health();
        data.player_max_health = player->get_max_health();
    }
    
    data.world_seed = 0;
    data.total_deaths = Statistics::instance().get_total_deaths();
    data.total_kills = Statistics::instance().get_total_kills();
    data.total_play_time = static_cast<u32>(Statistics::instance().get_total_play_time());
    
    SaveSystem::instance().save_game(save_slot_, data);
}

} // namespace chronoraid
