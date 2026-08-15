#include <chronoraid/quests/quest.hpp>
#include <chronoraid/core/logger.hpp>
#include <chronoraid/gameplay/player.hpp>
#include <chronoraid/progression/progression.hpp>

namespace chronoraid {

Quest::Quest(const std::string& id, const std::string& name, QuestType type)
    : id_(id), name_(name), type_(type) {
}

void Quest::set_state(QuestState state) {
    if (state_ == state) return;
    
    QuestState old_state = state_;
    state_ = state;
    
    if (state == QuestState::Completed && on_completed_) {
        on_completed_();
    }
    
    LOG_INFO("Quest '" + name_ + "' state changed to " + std::to_string(static_cast<int>(state)));
}

void Quest::add_objective(const QuestObjective& objective) {
    objectives_.push_back(objective);
}

void Quest::update_objective(const std::string& objective_id, u32 progress) {
    for (auto& obj : objectives_) {
        if (obj.id == objective_id) {
            obj.current_count = min(progress, obj.target_count);
            obj.is_completed = obj.current_count >= obj.target_count;
            check_completion();
            break;
        }
    }
}

f32 Quest::get_progress_percentage() const {
    if (objectives_.empty()) return 0.0f;
    
    u32 total = 0;
    u32 completed = 0;
    
    for (const auto& obj : objectives_) {
        if (!obj.is_optional) {
            total++;
            if (obj.is_completed) completed++;
        }
    }
    
    return total > 0 ? static_cast<f32>(completed) / static_cast<f32>(total) : 0.0f;
}

void Quest::check_completion() {
    bool all_required_completed = true;
    
    for (const auto& obj : objectives_) {
        if (!obj.is_optional && !obj.is_completed) {
            all_required_completed = false;
            break;
        }
    }
    
    if (all_required_completed && state_ == QuestState::InProgress) {
        set_state(QuestState::Completed);
    }
}

void QuestManager::register_quest(std::shared_ptr<Quest> quest) {
    if (quest) {
        quests_[quest->get_id()] = quest;
        LOG_INFO("Registered quest: " + quest->get_name());
    }
}

void QuestManager::unregister_quest(const std::string& quest_id) {
    auto it = quests_.find(quest_id);
    if (it != quests_.end()) {
        quests_.erase(it);
        LOG_INFO("Unregistered quest: " + quest_id);
    }
}

std::shared_ptr<Quest> QuestManager::get_quest(const std::string& quest_id) {
    auto it = quests_.find(quest_id);
    if (it != quests_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<Quest>> QuestManager::get_active_quests() const {
    std::vector<std::shared_ptr<Quest>> result;
    for (const auto& [id, quest] : quests_) {
        if (quest->is_active()) {
            result.push_back(quest);
        }
    }
    return result;
}

std::vector<std::shared_ptr<Quest>> QuestManager::get_completed_quests() const {
    std::vector<std::shared_ptr<Quest>> result;
    for (const auto& [id, quest] : quests_) {
        if (quest->is_completed()) {
            result.push_back(quest);
        }
    }
    return result;
}

bool QuestManager::start_quest(const std::string& quest_id) {
    auto quest = get_quest(quest_id);
    if (!quest) return false;
    
    if (quest->get_state() != QuestState::NotStarted) return false;
    
    const std::string& prereq = quest->get_prerequisite_quest();
    if (!prereq.empty()) {
        auto prereq_quest = get_quest(prereq);
        if (!prereq_quest || !prereq_quest->is_completed()) {
            LOG_WARNING("Cannot start quest: prerequisite not completed");
            return false;
        }
    }
    
    quest->set_state(QuestState::InProgress);
    LOG_INFO("Started quest: " + quest->get_name());
    return true;
}

bool QuestManager::complete_quest(const std::string& quest_id) {
    auto quest = get_quest(quest_id);
    if (!quest) return false;
    
    if (!quest->is_active()) return false;
    
    quest->set_state(QuestState::Completed);
    
    const auto& reward = quest->get_reward();
    auto player = PlayerController::instance().get_player();
    if (player) {
        player->add_experience(reward.experience);
    }
    
    LOG_INFO("Completed quest: " + quest->get_name());
    return true;
}

bool QuestManager::abandon_quest(const std::string& quest_id) {
    auto quest = get_quest(quest_id);
    if (!quest) return false;
    
    if (!quest->is_active()) return false;
    
    quest->set_state(QuestState::Abandoned);
    LOG_INFO("Abandoned quest: " + quest->get_name());
    return true;
}

bool QuestManager::fail_quest(const std::string& quest_id) {
    auto quest = get_quest(quest_id);
    if (!quest) return false;
    
    if (!quest->is_active()) return false;
    
    quest->set_state(QuestState::Failed);
    LOG_INFO("Failed quest: " + quest->get_name());
    return true;
}

void QuestManager::update_objective(const std::string& objective_id, u32 progress) {
    for (auto& [id, quest] : quests_) {
        if (quest->is_active()) {
            quest->update_objective(objective_id, progress);
        }
    }
}

void QuestManager::save() {
}

void QuestManager::load() {
}

} // namespace chronoraid
