#pragma once

#include "item.hpp"
#include "inventory.hpp"
#include "../core/random.hpp"
#include "../core/common.hpp"
#include <vector>
#include <unordered_map>

namespace chronoraid {

struct LootTableEntry {
    std::string item_id;
    f32 weight = 1.0f;
    u32 min_count = 1;
    u32 max_count = 1;
};

class LootTable {
public:
    void add_entry(const LootTableEntry& entry);
    void remove_entry(const std::string& item_id);

    std::vector<ItemStack> generate(u32 seed) const;
    std::vector<ItemStack> generate() const;

private:
    std::vector<LootTableEntry> entries_;
};

class LootGenerator {
public:
    static LootGenerator& instance() {
        static LootGenerator generator;
        return generator;
    }

    void set_seed(u32 seed);
    u32 get_seed() const { return seed_; }

    std::shared_ptr<Item> generate_item(u32 player_level, u32 area_level = 0);
    std::shared_ptr<Item> generate_item_of_rarity(ItemRarity rarity, u32 level);

    std::vector<ItemStack> generate_loot(u32 player_level, u32 area_level, u32 item_count);
    std::vector<ItemStack> generate_boss_loot(u32 player_level, u32 boss_difficulty);

    void register_loot_table(const std::string& table_id, const LootTable& table);
    LootTable* get_loot_table(const std::string& table_id);

    ItemRarity roll_rarity(u32 player_level);
    f32 get_rarity_chance(ItemRarity rarity, u32 level) const;

private:
    LootGenerator() : random_(0) {}

    u32 seed_;
    Random random_;

    std::unordered_map<std::string, LootTable> loot_tables_;
};

} // namespace chronoraid
