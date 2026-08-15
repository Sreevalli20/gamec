#pragma once

#include "item.hpp"
#include "../core/common.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace chronoraid {

class Inventory {
public:
    Inventory(u32 slots = 20);

    u32 get_slot_count() const { return slots_; }
    u32 get_used_slots() const;

    bool is_full() const;
    bool is_empty() const;

    ItemStack* get_slot(u32 index);
    const ItemStack* get_slot(u32 index) const;

    bool add_item(std::shared_ptr<Item> item, u32 count = 1);
    bool remove_item(u32 index, u32 count = 1);
    bool remove_item_by_id(const std::string& item_id, u32 count = 1);

    bool move_item(u32 from_index, u32 to_index);
    bool swap_items(u32 index_a, u32 index_b);

    u32 count_item(const std::string& item_id) const;
    bool has_item(const std::string& item_id, u32 count = 1) const;

    void clear();

    void set_on_item_added_callback(std::function<void(const ItemStack&)> callback);
    void set_on_item_removed_callback(std::function<void(const ItemStack&)> callback);

private:
    u32 find_empty_slot() const;
    u32 find_stackable_slot(std::shared_ptr<Item> item) const;

    u32 slots_;
    std::vector<std::unique_ptr<ItemStack>> items_;

    std::function<void(const ItemStack&)> on_item_added_;
    std::function<void(const ItemStack&)> on_item_removed_;
};

class Equipment {
public:
    Equipment();

    ItemStack* get_slot(ItemSlot slot);
    const ItemStack* get_slot(ItemSlot slot) const;

    bool equip(ItemStack* item_stack);
    ItemStack* unequip(ItemSlot slot);
    bool can_equip(const ItemStack* item_stack) const;

    const ItemStats& get_total_stats() const { return total_stats_; }
    void recalculate_stats();

private:
    std::unordered_map<ItemSlot, std::unique_ptr<ItemStack>> equipped_items_;
    ItemStats total_stats_;
};

class ItemDatabase {
public:
    static ItemDatabase& instance() {
        static ItemDatabase db;
        return db;
    }

    void load_from_file(const std::string& filename);
    void save_to_file(const std::string& filename);

    std::shared_ptr<Item> create_item(const std::string& item_id) const;
    void register_item(std::shared_ptr<Item> item);

    std::vector<std::shared_ptr<Item>> get_items_by_type(ItemType type) const;
    std::vector<std::shared_ptr<Item>> get_items_by_rarity(ItemRarity rarity) const;

private:
    ItemDatabase() = default;

    std::unordered_map<std::string, std::shared_ptr<Item>> items_;
};

} // namespace chronoraid
