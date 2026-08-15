#pragma once

#include "../core/common.hpp"
#include <string>
#include <functional>
#include <unordered_map>

namespace chronoraid {

enum class AchievementType {
    KillCount,
    BossDefeated,
    ItemCollected,
    Level milestone,
    TimeBased,
    ScoreBased,
    Special
};

struct Achievement {
    std::string id;
    std::string name;
    std::string description;
    AchievementType type;
    u32 target_value = 1;
    u32 current_value = 0;
    bool is_unlocked = false;
    bool is_hidden = false;
    f64 unlock_time = 0.0;
};

class AchievementManager {
public:
    static AchievementManager& instance() {
        static AchievementManager manager;
        return manager;
    }

    void register_achievement(const Achievement& achievement);
    void unregister_achievement(const std::string& id);

    Achievement* get_achievement(const std::string& id);
    const Achievement* get_achievement(const std::string& id) const;

    std::vector<Achievement*> get_all_achievements();
    std::vector<Achievement*> get_unlocked_achievements();
    std::vector<Achievement*> get_locked_achievements();

    bool unlock_achievement(const std::string& id);
    void update_progress(const std::string& id, u32 progress);

    void register_callback(const std::string& id, std::function<void()> callback);

    void save();
    void load();

    u32 get_unlocked_count() const;
    u32 get_total_count() const;
    f32 get_completion_percentage() const;

private:
    AchievementManager() = default;

    std::unordered_map<std::string, Achievement> achievements_;
    std::unordered_map<std::string, std::function<void()>> callbacks_;
};

} // namespace chronoraid
