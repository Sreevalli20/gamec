#pragma once

#include "../entities/entity.hpp"
#include "../entities/components.hpp"
#include "../core/common.hpp"
#include "../core/vector2.hpp"
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

namespace chronoraid {

enum class NPCType {
    Vendor,
    QuestGiver,
    Informant,
    Blacksmith,
    Alchemist,
    Trainer
};

enum class DialogueOption {
    Continue,
    Trade,
    AcceptQuest,
    DeclineQuest,
    Goodbye
};

struct DialogueNode {
    std::string text;
    std::vector<std::pair<std::string, DialogueOption>> choices;
    std::function<void()> on_select;
};

class NPC : public Entity {
public:
    NPC(EntityID id, NPCType type, const std::string& name);

    void update(f64 delta_time) override;

    NPCType get_type() const { return type_; }
    const std::string& get_name() const { return name_; }

    void add_dialogue_node(const std::string& id, const DialogueNode& node);
    DialogueNode* get_dialogue_node(const std::string& id);
    void set_start_dialogue(const std::string& id) { start_dialogue_id_ = id; }

    bool can_interact() const { return can_interact_; }
    void set_can_interact(bool can) { can_interact_ = can; }

    void interact(Entity* player);

    bool is_vendor() const { return type_ == NPCType::Vendor; }
    bool is_quest_giver() const { return type_ == NPCType::QuestGiver; }

    void set_reputation(i32 value) { reputation_ = value; }
    i32 get_reputation() const { return reputation_; }
    void modify_reputation(i32 amount) { reputation_ += amount; }

private:
    NPCType type_;
    std::string name_;
    std::unordered_map<std::string, DialogueNode> dialogue_nodes_;
    std::string start_dialogue_id_;
    bool can_interact_ = true;
    i32 reputation_ = 0;
};

class VendorNPC : public NPC {
public:
    VendorNPC(EntityID id, const std::string& name);

    void add_item_for_sale(const std::string& item_id, u32 price);
    void remove_item_for_sale(const std::string& item_id);

    bool buy_item(const std::string& item_id, Entity* player);
    bool sell_item(const std::string& item_id, Entity* player);

    const std::vector<std::pair<std::string, u32>>& get_inventory() const { return shop_inventory_; }

    f32 get_discount_multiplier() const;

private:
    std::vector<std::pair<std::string, u32>> shop_inventory_;
};

class NPCManager {
public:
    static NPCManager& instance() {
        static NPCManager manager;
        return manager;
    }

    void register_npc(NPC* npc);
    void unregister_npc(EntityID id);

    NPC* get_npc(EntityID id);
    std::vector<NPC*> get_npcs_in_range(const Vector2& position, f32 range);

    void update(f64 delta_time);

private:
    NPCManager() = default;

    std::unordered_map<EntityID, NPC*> npcs_;
};

} // namespace chronoraid
