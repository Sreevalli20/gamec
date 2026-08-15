#include <chronoraid/progression/progression.hpp>
#include <chronoraid/core/logger.hpp>
#include <algorithm>

namespace chronoraid {

f32 UpgradeNode::get_stat_bonus(const std::string& stat_name) const {
    f32 base_bonus = 10.0f * current_level_;
    
    if (stat_name == "attack_power") return base_bonus;
    if (stat_name == "defense") return base_bonus * 0.8f;
    if (stat_name == "health") return base_bonus * 2.0f;
    if (stat_name == "energy") return base_bonus * 1.5f;
    if (stat_name == "critical_chance") return base_bonus * 0.01f;
    
    return 0.0f;
}

SkillTree::SkillTree() {
    UpgradeNode health_node;
    health_node.id = "health_1";
    health_node.name = "Vitality";
    health_node.description = "Increases maximum health";
    health_node.branch = UpgradeBranch::Survival;
    health_node.cost = 1;
    health_node.max_level = 5;
    register_node(health_node);
    
    UpgradeNode attack_node;
    attack_node.id = "attack_1";
    attack_node.name = "Strength";
    attack_node.description = "Increases attack power";
    attack_node.branch = UpgradeBranch::Combat;
    attack_node.cost = 1;
    attack_node.max_level = 5;
    register_node(attack_node);
    
    UpgradeNode time_node;
    time_node.id = "time_1";
    time_node.name = "Temporal Mastery";
    time_node.description = "Reduces time ability cooldowns";
    time_node.branch = UpgradeBranch::Temporal;
    time_node.cost = 2;
    time_node.max_level = 3;
    register_node(time_node);
}

void SkillTree::register_node(const UpgradeNode& node) {
    nodes_[node.id] = node;
}

UpgradeNode* SkillTree::get_node(const std::string& node_id) {
    auto it = nodes_.find(node_id);
    if (it != nodes_.end()) {
        return &it->second;
    }
    return nullptr;
}

const UpgradeNode* SkillTree::get_node(const std::string& node_id) const {
    auto it = nodes_.find(node_id);
    if (it != nodes_.end()) {
        return &it->second;
    }
    return nullptr;
}

bool SkillTree::can_unlock(const std::string& node_id) const {
    auto it = nodes_.find(node_id);
    if (it == nodes_.end()) return false;
    
    const auto& node = it->second;
    if (node.current_level >= node.max_level) return false;
    if (available_points_ < node.cost) return false;
    
    return check_prerequisites(node);
}

bool SkillTree::check_prerequisites(const UpgradeNode& node) const {
    for (const auto& prereq_id : node.prerequisites) {
        auto prereq_it = nodes_.find(prereq_id);
        if (prereq_it == nodes_.end() || !prereq_it->second.is_unlocked) {
            return false;
        }
    }
    return true;
}

bool SkillTree::unlock_node(const std::string& node_id) {
    if (!can_unlock(node_id)) return false;
    
    auto it = nodes_.find(node_id);
    if (it == nodes_.end()) return false;
    
    auto& node = it->second;
    available_points_ -= node.cost;
    total_spent_points_ += node.cost;
    node.current_level++;
    node.is_unlocked = true;
    
    LOG_INFO("Unlocked upgrade node: " + node.name);
    return true;
}

std::vector<const UpgradeNode*> SkillTree::get_unlocked_nodes() const {
    std::vector<const UpgradeNode*> result;
    for (const auto& [id, node] : nodes_) {
        if (node.is_unlocked) {
            result.push_back(&node);
        }
    }
    return result;
}

std::vector<const UpgradeNode*> SkillTree::get_available_nodes() const {
    std::vector<const UpgradeNode*> result;
    for (const auto& [id, node] : nodes_) {
        if (can_unlock(id)) {
            result.push_back(&node);
        }
    }
    return result;
}

PlayerProgression::PlayerProgression() {
}

void PlayerProgression::add_experience(u32 amount) {
    current_xp_ += amount;
    
    while (current_xp_ >= xp_to_next_level_) {
        level_up();
    }
}

void PlayerProgression::level_up() {
    current_xp_ -= xp_to_next_level_;
    level_++;
    xp_to_next_level_ = static_cast<u32>(xp_to_next_level_ * 1.5f);
    skill_points_++;
    
    LOG_INFO("Level up! Now level " + std::to_string(level_));
}

f32 PlayerProgression::get_xp_percentage() const {
    return static_cast<f32>(current_xp_) / static_cast<f32>(xp_to_next_level_);
}

bool PlayerProgression::spend_skill_point() {
    if (skill_points_ > 0) {
        skill_points_--;
        return true;
    }
    return false;
}

bool PlayerProgression::spend_currency(u32 amount) {
    if (currency_ >= amount) {
        currency_ -= amount;
        return true;
    }
    return false;
}

void PlayerProgression::reset() {
    level_ = 1;
    current_xp_ = 0;
    xp_to_next_level_ = 100;
    skill_points_ = 0;
    currency_ = 0;
    skill_tree_ = SkillTree();
}

void PlayerProgression::save() {
}

void PlayerProgression::load() {
}

} // namespace chronoraid
