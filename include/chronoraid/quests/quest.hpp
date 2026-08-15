#pragma once

#include "../core/common.hpp"
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

namespace chronoraid {

class PlayerController;

enum class QuestState {
    NotStarted,
    InProgress,
    Completed,
    Failed,
    Abandoned
};

enum class QuestType {
    Main,
    Side,
    Daily,
    Weekly,
    Hidden
};

struct QuestObjective {
    std::string id;
    std::string description;
    u32 target_count = 1;
    u32 current_count = 0;
    bool is_completed = false;
    bool is_optional = false;
};

struct QuestReward {
    u32 experience = 0;
    u32 currency = 0;
    std::vector<std::string> item_ids;
};

class Quest {
public:
    Quest(const std::string& id, const std::string& name, QuestType type);

    const std::string& get_id() const { return id_; }
    const std::string& get_name() const { return name_; }
    const std::string& get_description() const { return description_; }
    void set_description(const std::string& desc) { description_ = desc; }

    QuestType get_type() const { return type_; }
    QuestState get_state() const { return state_; }
    void set_state(QuestState state);

    const std::vector<QuestObjective>& get_objectives() const { return objectives_; }
    void add_objective(const QuestObjective& objective);
    void update_objective(const std::string& objective_id, u32 progress);

    const QuestReward& get_reward() const { return reward_; }
    void set_reward(const QuestReward& reward) { reward_ = reward; }

    bool is_completed() const { return state_ == QuestState::Completed; }
    bool is_failed() const { return state_ == QuestState::Failed; }
    bool is_active() const { return state_ == QuestState::InProgress; }

    f32 get_progress_percentage() const;

    void set_prerequisite_quest(const std::string& quest_id) { prerequisite_quest_ = quest_id; }
    const std::string& get_prerequisite_quest() const { return prerequisite_quest_; }

    void set_on_completed_callback(std::function<void()> callback) { on_completed_ = callback; }

private:
    void check_completion();

    std::string id_;
    std::string name_;
    std::string description_;
    QuestType type_;
    QuestState state_ = QuestState::NotStarted;
    std::vector<QuestObjective> objectives_;
    QuestReward reward_;
    std::string prerequisite_quest_;
    std::function<void()> on_completed_;
};

class QuestManager {
public:
    static QuestManager& instance() {
        static QuestManager manager;
        return manager;
    }

    void register_quest(std::shared_ptr<Quest> quest);
    void unregister_quest(const std::string& quest_id);

    std::shared_ptr<Quest> get_quest(const std::string& quest_id);
    std::vector<std::shared_ptr<Quest>> get_active_quests() const;
    std::vector<std::shared_ptr<Quest>> get_completed_quests() const;

    bool start_quest(const std::string& quest_id);
    bool complete_quest(const std::string& quest_id);
    bool abandon_quest(const std::string& quest_id);
    bool fail_quest(const std::string& quest_id);

    void update_objective(const std::string& objective_id, u32 progress);

    void save();
    void load();

private:
    QuestManager() = default;

    std::unordered_map<std::string, std::shared_ptr<Quest>> quests_;
};

} // namespace chronoraid
