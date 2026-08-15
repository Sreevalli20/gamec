#pragma once

#include "../core/common.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace chronoraid {

class Player;

enum class UpgradeBranch {
    Combat,
    Temporal,
    Survival
};

struct UpgradeNode {
    std::string id;
    std::string name;
    std::string description;
    UpgradeBranch branch;
    u32 cost = 1;
    u32 max_level = 5;
    u32 current_level = 0;
    std::vector<std::string> prerequisites;
    bool is_unlocked = false;

    f32 get_stat_bonus(const std::string& stat_name) const;
};

class SkillTree {
public:
    SkillTree();

    void register_node(const UpgradeNode& node);
    UpgradeNode* get_node(const std::string& node_id);
    const UpgradeNode* get_node(const std::string& node_id) const;

    bool can_unlock(const std::string& node_id) const;
    bool unlock_node(const std::string& node_id);

    u32 get_available_points() const { return available_points_; }
    void add_points(u32 points) { available_points_ += points; }

    u32 get_total_spent_points() const { return total_spent_points_; }

    std::vector<const UpgradeNode*> get_unlocked_nodes() const;
    std::vector<const UpgradeNode*> get_available_nodes() const;

private:
    bool check_prerequisites(const UpgradeNode& node) const;

    std::unordered_map<std::string, UpgradeNode> nodes_;
    u32 available_points_ = 0;
    u32 total_spent_points_ = 0;
};

class PlayerProgression {
public:
    PlayerProgression();

    u32 get_level() const { return level_; }
    u32 get_experience() const { return current_xp_; }
    u32 get_xp_to_next_level() const { return xp_to_next_level_; }

    void add_experience(u32 amount);
    f32 get_xp_percentage() const;

    u32 get_skill_points() const { return skill_points_; }
    void add_skill_points(u32 points) { skill_points_ += points; }
    bool spend_skill_point();

    u32 get_currency() const { return currency_; }
    void add_currency(u32 amount) { currency_ += amount; }
    bool spend_currency(u32 amount);

    SkillTree& get_skill_tree() { return skill_tree_; }
    const SkillTree& get_skill_tree() const { return skill_tree_; }

    void reset();
    void save();
    void load();

private:
    void level_up();

    u32 level_ = 1;
    u32 current_xp_ = 0;
    u32 xp_to_next_level_ = 100;
    u32 skill_points_ = 0;
    u32 currency_ = 0;
    SkillTree skill_tree_;
};

} // namespace chronoraid
