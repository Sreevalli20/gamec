#include <chronoraid/inventory/loot_generator.hpp>
#include <chronoraid/core/logger.hpp>
#include <algorithm>

namespace chronoraid {

void LootTable::add_entry(const LootTableEntry& entry) {
    entries_.push_back(entry);
}

void LootTable::remove_entry(const std::string& item_id) {
    entries_.erase(
        std::remove_if(entries_.begin(), entries_.end(),
            [&item_id](const LootTableEntry& e) { return e.item_id == item_id; }),
        entries_.end()
    );
}

std::vector<ItemStack> LootTable::generate(u32 seed) const {
    Random rng(seed);
    return generate();
}

std::vector<ItemStack> LootTable::generate() const {
    std::vector<ItemStack> result;
    
    if (entries_.empty()) return result;
    
    f32 total_weight = 0.0f;
    for (const auto& entry : entries_) {
        total_weight += entry.weight;
    }
    
    Random rng;
    f32 roll = rng.next_f32(total_weight);
    
    f32 current_weight = 0.0f;
    for (const auto& entry : entries_) {
        current_weight += entry.weight;
        if (roll <= current_weight) {
            auto item = ItemDatabase::instance().create_item(entry.item_id);
            if (item) {
                u32 count = rng.next_u32(entry.min_count, entry.max_count);
                result.emplace_back(item, count);
            }
            break;
        }
    }
    
    return result;
}

void LootGenerator::set_seed(u32 seed) {
    seed_ = seed;
    random_.set_seed(seed);
}

std::shared_ptr<Item> LootGenerator::generate_item(u32 player_level, u32 area_level) {
    ItemRarity rarity = roll_rarity(player_level);
    return generate_item_of_rarity(rarity, max(player_level, area_level));
}

std::shared_ptr<Item> LootGenerator::generate_item_of_rarity(ItemRarity rarity, u32 level) {
    auto items = ItemDatabase::instance().get_items_by_rarity(rarity);
    
    if (items.empty()) {
        items = ItemDatabase::instance().get_items_by_type(ItemType::Weapon);
    }
    
    if (items.empty()) {
        return nullptr;
    }
    
    auto item = random_.from_vector(items);
    
    if (item) {
        auto new_item = std::make_shared<Item>(*item);
        
        f32 level_multiplier = 1.0f + (level * 0.1f);
        auto stats = new_item->get_stats();
        stats.attack_power *= level_multiplier;
        stats.defense *= level_multiplier;
        stats.health_bonus *= level_multiplier;
        new_item->set_stats(stats);
        
        return new_item;
    }
    
    return nullptr;
}

std::vector<ItemStack> LootGenerator::generate_loot(u32 player_level, u32 area_level, u32 item_count) {
    std::vector<ItemStack> result;
    
    for (u32 i = 0; i < item_count; ++i) {
        auto item = generate_item(player_level, area_level);
        if (item) {
            result.emplace_back(item, 1);
        }
    }
    
    return result;
}

std::vector<ItemStack> LootGenerator::generate_boss_loot(u32 player_level, u32 boss_difficulty) {
    std::vector<ItemStack> result;
    
    u32 guaranteed_rare = 1 + (boss_difficulty / 2);
    for (u32 i = 0; i < guaranteed_rare; ++i) {
        auto item = generate_item_of_rarity(ItemRarity::Rare, player_level + boss_difficulty);
        if (item) {
            result.emplace_back(item, 1);
        }
    }
    
    if (random_.next_bool(0.3f)) {
        auto item = generate_item_of_rarity(ItemRarity::Epic, player_level + boss_difficulty);
        if (item) {
            result.emplace_back(item, 1);
        }
    }
    
    if (boss_difficulty >= 5 && random_.next_bool(0.1f)) {
        auto item = generate_item_of_rarity(ItemRarity::Legendary, player_level + boss_difficulty);
        if (item) {
            result.emplace_back(item, 1);
        }
    }
    
    return result;
}

void LootGenerator::register_loot_table(const std::string& table_id, const LootTable& table) {
    loot_tables_[table_id] = table;
}

LootTable* LootGenerator::get_loot_table(const std::string& table_id) {
    auto it = loot_tables_.find(table_id);
    if (it != loot_tables_.end()) {
        return &it->second;
    }
    return nullptr;
}

ItemRarity LootGenerator::roll_rarity(u32 level) {
    f32 roll = random_.next_f32();
    
    f32 mythic_chance = get_rarity_chance(ItemRarity::Mythic, level);
    f32 legendary_chance = get_rarity_chance(ItemRarity::Legendary, level) + mythic_chance;
    f32 epic_chance = get_rarity_chance(ItemRarity::Epic, level) + legendary_chance;
    f32 rare_chance = get_rarity_chance(ItemRarity::Rare, level) + epic_chance;
    f32 uncommon_chance = get_rarity_chance(ItemRarity::Uncommon, level) + rare_chance;
    
    if (roll < mythic_chance) return ItemRarity::Mythic;
    if (roll < legendary_chance) return ItemRarity::Legendary;
    if (roll < epic_chance) return ItemRarity::Epic;
    if (roll < rare_chance) return ItemRarity::Rare;
    if (roll < uncommon_chance) return ItemRarity::Uncommon;
    
    return ItemRarity::Common;
}

f32 LootGenerator::get_rarity_chance(ItemRarity rarity, u32 level) const {
    f32 base_chance = 0.0f;
    
    switch (rarity) {
        case ItemRarity::Common: base_chance = 0.6f; break;
        case ItemRarity::Uncommon: base_chance = 0.25f; break;
        case ItemRarity::Rare: base_chance = 0.1f; break;
        case ItemRarity::Epic: base_chance = 0.04f; break;
        case ItemRarity::Legendary: base_chance = 0.009f; break;
        case ItemRarity::Mythic: base_chance = 0.001f; break;
    }
    
    f32 level_bonus = level * 0.002f;
    return min(base_chance + level_bonus, 0.5f);
}

} // namespace chronoraid
