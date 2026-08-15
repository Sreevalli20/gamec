#include <chronoraid/achievements/achievement.hpp>
#include <chronoraid/core/logger.hpp>

namespace chronoraid {

void AchievementManager::register_achievement(const Achievement& achievement) {
    achievements_[achievement.id] = achievement;
    LOG_INFO("Registered achievement: " + achievement.name);
}

void AchievementManager::unregister_achievement(const std::string& id) {
    auto it = achievements_.find(id);
    if (it != achievements_.end()) {
        achievements_.erase(it);
        LOG_INFO("Unregistered achievement: " + id);
    }
}

Achievement* AchievementManager::get_achievement(const std::string& id) {
    auto it = achievements_.find(id);
    if (it != achievements_.end()) {
        return &it->second;
    }
    return nullptr;
}

const Achievement* AchievementManager::get_achievement(const std::string& id) const {
    auto it = achievements_.find(id);
    if (it != achievements_.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<Achievement*> AchievementManager::get_all_achievements() {
    std::vector<Achievement*> result;
    for (auto& [id, achievement] : achievements_) {
        result.push_back(&achievement);
    }
    return result;
}

std::vector<Achievement*> AchievementManager::get_unlocked_achievements() {
    std::vector<Achievement*> result;
    for (auto& [id, achievement] : achievements_) {
        if (achievement.is_unlocked) {
            result.push_back(&achievement);
        }
    }
    return result;
}

std::vector<Achievement*> AchievementManager::get_locked_achievements() {
    std::vector<Achievement*> result;
    for (auto& [id, achievement] : achievements_) {
        if (!achievement.is_unlocked) {
            result.push_back(&achievement);
        }
    }
    return result;
}

bool AchievementManager::unlock_achievement(const std::string& id) {
    auto it = achievements_.find(id);
    if (it == achievements_.end()) return false;
    
    if (it->second.is_unlocked) return false;
    
    it->second.is_unlocked = true;
    it->second.unlock_time = Game::instance().get_total_time();
    
    LOG_INFO("Achievement unlocked: " + it->second.name);
    
    auto callback_it = callbacks_.find(id);
    if (callback_it != callbacks_.end()) {
        callback_it->second();
    }
    
    return true;
}

void AchievementManager::update_progress(const std::string& id, u32 progress) {
    auto it = achievements_.find(id);
    if (it == achievements_.end()) return;
    
    it->second.current_value = progress;
    
    if (it->second.current_value >= it->second.target_value) {
        unlock_achievement(id);
    }
}

void AchievementManager::register_callback(const std::string& id, std::function<void()> callback) {
    callbacks_[id] = callback;
}

void AchievementManager::save() {
}

void AchievementManager::load() {
}

u32 AchievementManager::get_unlocked_count() const {
    u32 count = 0;
    for (const auto& [id, achievement] : achievements_) {
        if (achievement.is_unlocked) count++;
    }
    return count;
}

u32 AchievementManager::get_total_count() const {
    return static_cast<u32>(achievements_.size());
}

f32 AchievementManager::get_completion_percentage() const {
    u32 total = get_total_count();
    if (total == 0) return 0.0f;
    return static_cast<f32>(get_unlocked_count()) / static_cast<f32>(total);
}

} // namespace chronoraid
