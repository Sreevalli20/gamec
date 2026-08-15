#include <chronoraid/inventory/inventory.hpp>
#include <chronoraid/core/logger.hpp>

namespace chronoraid {

Inventory::Inventory(u32 slots) : slots_(slots) {
    items_.resize(slots_);
}

u32 Inventory::get_used_slots() const {
    u32 count = 0;
    for (const auto& item : items_) {
        if (item) count++;
    }
    return count;
}

bool Inventory::is_full() const {
    return get_used_slots() >= slots_;
}

bool Inventory::is_empty() const {
    return get_used_slots() == 0;
}

ItemStack* Inventory::get_slot(u32 index) {
    if (index >= slots_) return nullptr;
    return items_[index].get();
}

const ItemStack* Inventory::get_slot(u32 index) const {
    if (index >= slots_) return nullptr;
    return items_[index].get();
}

bool Inventory::add_item(std::shared_ptr<Item> item, u32 count) {
    if (!item) return false;
    
    u32 stackable_slot = find_stackable_slot(item);
    if (stackable_slot != static_cast<u32>(-1)) {
        if (items_[stackable_slot]->add(count)) {
            if (on_item_added_) on_item_added_(*items_[stackable_slot]);
            return true;
        }
        count -= items_[stackable_slot]->get_space();
    }
    
    while (count > 0) {
        u32 empty_slot = find_empty_slot();
        if (empty_slot == static_cast<u32>(-1)) return false;
        
        u32 stack_size = min(count, item->get_max_stack());
        items_[empty_slot] = std::make_unique<ItemStack>(item, stack_size);
        count -= stack_size;
        
        if (on_item_added_) on_item_added_(*items_[empty_slot]);
    }
    
    return true;
}

bool Inventory::remove_item(u32 index, u32 count) {
    if (index >= slots_ || !items_[index]) return false;
    
    if (items_[index]->remove(count)) {
        if (items_[index]->get_count() == 0) {
            if (on_item_removed_) on_item_removed_(*items_[index]);
            items_[index].reset();
        }
        return true;
    }
    
    return false;
}

bool Inventory::remove_item_by_id(const std::string& item_id, u32 count) {
    u32 remaining = count;
    
    for (u32 i = 0; i < slots_ && remaining > 0; ++i) {
        if (items_[i] && items_[i]->get_item()->get_id() == item_id) {
            u32 to_remove = min(remaining, items_[i]->get_count());
            if (remove_item(i, to_remove)) {
                remaining -= to_remove;
            }
        }
    }
    
    return remaining == 0;
}

bool Inventory::move_item(u32 from_index, u32 to_index) {
    if (from_index >= slots_ || to_index >= slots_) return false;
    if (!items_[from_index]) return false;
    
    if (items_[to_index]) {
        return swap_items(from_index, to_index);
    }
    
    items_[to_index] = std::move(items_[from_index]);
    return true;
}

bool Inventory::swap_items(u32 index_a, u32 index_b) {
    if (index_a >= slots_ || index_b >= slots_) return false;
    
    std::swap(items_[index_a], items_[index_b]);
    return true;
}

u32 Inventory::count_item(const std::string& item_id) const {
    u32 total = 0;
    for (const auto& item : items_) {
        if (item && item->get_item()->get_id() == item_id) {
            total += item->get_count();
        }
    }
    return total;
}

bool Inventory::has_item(const std::string& item_id, u32 count) const {
    return count_item(item_id) >= count;
}

void Inventory::clear() {
    items_.clear();
    items_.resize(slots_);
}

void Inventory::set_on_item_added_callback(std::function<void(const ItemStack&)> callback) {
    on_item_added_ = callback;
}

void Inventory::set_on_item_removed_callback(std::function<void(const ItemStack&)> callback) {
    on_item_removed_ = callback;
}

u32 Inventory::find_empty_slot() const {
    for (u32 i = 0; i < slots_; ++i) {
        if (!items_[i]) return i;
    }
    return static_cast<u32>(-1);
}

u32 Inventory::find_stackable_slot(std::shared_ptr<Item> item) const {
    for (u32 i = 0; i < slots_; ++i) {
        if (items_[i] && items_[i]->can_merge_with(ItemStack(*item, 0))) {
            return i;
        }
    }
    return static_cast<u32>(-1);
}

Equipment::Equipment() {
}

ItemStack* Equipment::get_slot(ItemSlot slot) {
    auto it = equipped_items_.find(slot);
    if (it != equipped_items_.end()) {
        return it->second.get();
    }
    return nullptr;
}

const ItemStack* Equipment::get_slot(ItemSlot slot) const {
    auto it = equipped_items_.find(slot);
    if (it != equipped_items_.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool Equipment::equip(ItemStack* item_stack) {
    if (!item_stack || !item_stack->get_item()) return false;
    
    ItemSlot slot = item_stack->get_item()->get_slot();
    if (slot == ItemSlot::None) return false;
    
    equipped_items_[slot] = std::make_unique<ItemStack>(*item_stack);
    recalculate_stats();
    
    return true;
}

ItemStack* Equipment::unequip(ItemSlot slot) {
    auto it = equipped_items_.find(slot);
    if (it != equipped_items_.end()) {
        ItemStack* result = it->second.get();
        equipped_items_.erase(it);
        recalculate_stats();
        return result;
    }
    return nullptr;
}

bool Equipment::can_equip(const ItemStack* item_stack) const {
    if (!item_stack || !item_stack->get_item()) return false;
    return item_stack->get_item()->get_slot() != ItemSlot::None;
}

void Equipment::recalculate_stats() {
    total_stats_ = ItemStats();
    
    for (const auto& [slot, item] : equipped_items_) {
        if (item && item->get_item()) {
            const auto& stats = item->get_item()->get_stats();
            total_stats_.attack_power += stats.attack_power;
            total_stats_.defense += stats.defense;
            total_stats_.health_bonus += stats.health_bonus;
            total_stats_.energy_bonus += stats.energy_bonus;
            total_stats_.movement_speed += stats.movement_speed;
            total_stats_.critical_chance += stats.critical_chance;
            total_stats_.critical_multiplier += stats.critical_multiplier;
        }
    }
}

void ItemDatabase::load_from_file(const std::string& filename) {
}

void ItemDatabase::save_to_file(const std::string& filename) {
}

std::shared_ptr<Item> ItemDatabase::create_item(const std::string& item_id) const {
    auto it = items_.find(item_id);
    if (it != items_.end()) {
        return std::make_shared<Item>(*it->second);
    }
    return nullptr;
}

void ItemDatabase::register_item(std::shared_ptr<Item> item) {
    if (item) {
        items_[item->get_id()] = item;
    }
}

std::vector<std::shared_ptr<Item>> ItemDatabase::get_items_by_type(ItemType type) const {
    std::vector<std::shared_ptr<Item>> result;
    for (const auto& [id, item] : items_) {
        if (item->get_type() == type) {
            result.push_back(item);
        }
    }
    return result;
}

std::vector<std::shared_ptr<Item>> ItemDatabase::get_items_by_rarity(ItemRarity rarity) const {
    std::vector<std::shared_ptr<Item>> result;
    for (const auto& [id, item] : items_) {
        if (item->get_rarity() == rarity) {
            result.push_back(item);
        }
    }
    return result;
}

} // namespace chronoraid
