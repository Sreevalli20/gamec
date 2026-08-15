#pragma once

#include "../core/common.hpp"
#include <string>
#include <vector>

namespace chronoraid {

class Player;
class Statistics;
class AutoSave;

enum class SaveVersion {
    Version_1_0 = 1,
    Current = Version_1_0
};

struct SaveData {
    SaveVersion version = SaveVersion::Current;
    
    u32 player_level = 1;
    u32 player_experience = 0;
    u32 player_currency = 0;
    f32 player_health = 100.0f;
    f32 player_max_health = 100.0f;
    
    std::vector<std::string> inventory_items;
    std::vector<u32> inventory_counts;
    std::vector<std::string> equipped_items;
    
    std::vector<std::string> completed_quests;
    std::vector<std::string> active_quests;
    
    std::vector<std::string> unlocked_upgrades;
    
    u32 world_seed = 0;
    u32 current_room_index = 0;
    
    std::vector<std::string> unlocked_achievements;
    
    std::string settings_data;
    
    u32 total_deaths = 0;
    u32 total_kills = 0;
    u32 total_play_time = 0;
};

class SaveSystem {
public:
    static SaveSystem& instance() {
        static SaveSystem system;
        return system;
    }

    bool save_game(u32 slot, const SaveData& data);
    bool load_game(u32 slot, SaveData& data);
    
    bool delete_save(u32 slot);
    bool has_save(u32 slot) const;
    
    std::vector<u32> get_available_save_slots() const;
    u32 get_latest_save_slot() const;
    
    bool create_backup(u32 slot);
    bool restore_backup(u32 slot);
    
    void set_save_directory(const std::string& directory);
    const std::string& get_save_directory() const { return save_directory_; }

private:
    SaveSystem() : save_directory_("saves") {}

    std::string get_save_filename(u32 slot) const;
    std::string get_backup_filename(u32 slot) const;
    
    bool serialize(const SaveData& data, std::string& output) const;
    bool deserialize(const std::string& input, SaveData& data) const;
    
    bool write_file(const std::string& filename, const std::string& content) const;
    bool read_file(const std::string& filename, std::string& content) const;

    std::string save_directory_;
};

class AutoSave {
public:
    static AutoSave& instance() {
        static AutoSave autosave;
        return autosave;
    }

    void enable(f32 interval_seconds = 300.0f);
    void disable();
    
    void update(f64 delta_time);
    void trigger_save();

    void set_save_slot(u32 slot) { save_slot_ = slot; }

private:
    AutoSave() : is_enabled_(false), save_interval_(300.0f), save_timer_(0.0f), save_slot_(0) {}

    bool is_enabled_;
    f32 save_interval_;
    f32 save_timer_;
    u32 save_slot_;
};

} // namespace chronoraid
