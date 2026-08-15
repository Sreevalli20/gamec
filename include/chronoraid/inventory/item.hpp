#pragma once

#include "../core/common.hpp"
#include <string>
#include <memory>
#include <vector>

namespace chronoraid {

enum class ItemRarity {
    Common,
    Uncommon,
    Rare,
    Epic,
    Legendary,
    Mythic
};

enum class ItemType {
    None,
    Weapon,
    Armor,
    Consumable,
    Artifact,
    Material,
    Quest
};

enum class ItemSlot {
    None,
    MainHand,
    OffHand,
    Head,
    Chest,
    Legs,
    Feet,
    Ring,
    Amulet
};

struct ItemStats {
    f32 attack_power = 0.0f;
    f32 defense = 0.0f;
    f32 health_bonus = 0.0f;
    f32 energy_bonus = 0.0f;
    f32 movement_speed = 0.0f;
    f32 critical_chance = 0.0f;
    f32 critical_multiplier = 0.0f;
};

class Item {
public:
    Item() = default;
    Item(const std::string& id, const std::string& name, ItemType type, ItemRarity rarity);

    const std::string& get_id() const { return id_; }
    const std::string& get_name() const { return name_; }
    ItemType get_type() const { return type_; }
    ItemRarity get_rarity() const { return rarity_; }

    const std::string& get_description() const { return description_; }
    void set_description(const std::string& desc) { description_ = desc; }

    const ItemStats& get_stats() const { return stats_; }
    void set_stats(const ItemStats& stats) { stats_ = stats; }

    u32 get_max_stack() const { return max_stack_; }
    void set_max_stack(u32 max) { max_stack_ = max; }

    u32 get_value() const { return value_; }
    void set_value(u32 value) { value_ = value; }

    ItemSlot get_slot() const { return slot_; }
    void set_slot(ItemSlot slot) { slot_ = slot; }

    bool is_stackable() const { return max_stack_ > 1; }
    bool is_equipment() const;

    static const char* rarity_to_string(ItemRarity rarity);
    static const char* type_to_string(ItemType type);

private:
    std::string id_;
    std::string name_;
    std::string description_;
    ItemType type_ = ItemType::None;
    ItemRarity rarity_ = ItemRarity::Common;
    ItemStats stats_;
    u32 max_stack_ = 1;
    u32 value_ = 0;
    ItemSlot slot_ = ItemSlot::None;
};

class ItemStack {
public:
    ItemStack(std::shared_ptr<Item> item, u32 count = 1);

    std::shared_ptr<Item> get_item() const { return item_; }
    u32 get_count() const { return count_; }

    bool add(u32 amount);
    bool remove(u32 amount);
    u32 get_space() const;

    bool can_merge_with(const ItemStack& other) const;
    bool merge_from(ItemStack& other);

private:
    std::shared_ptr<Item> item_;
    u32 count_;
};

} // namespace chronoraid
