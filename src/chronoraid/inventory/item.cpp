#include <chronoraid/inventory/item.hpp>

namespace chronoraid {

Item::Item(const std::string& id, const std::string& name, ItemType type, ItemRarity rarity)
    : id_(id), name_(name), type_(type), rarity_(rarity) {
}

bool Item::is_equipment() const {
    return type_ == ItemType::Weapon || type_ == ItemType::Armor;
}

const char* Item::rarity_to_string(ItemRarity rarity) {
    switch (rarity) {
        case ItemRarity::Common: return "Common";
        case ItemRarity::Uncommon: return "Uncommon";
        case ItemRarity::Rare: return "Rare";
        case ItemRarity::Epic: return "Epic";
        case ItemRarity::Legendary: return "Legendary";
        case ItemRarity::Mythic: return "Mythic";
        default: return "Unknown";
    }
}

const char* Item::type_to_string(ItemType type) {
    switch (type) {
        case ItemType::Weapon: return "Weapon";
        case ItemType::Armor: return "Armor";
        case ItemType::Consumable: return "Consumable";
        case ItemType::Artifact: return "Artifact";
        case ItemType::Material: return "Material";
        case ItemType::Quest: return "Quest";
        default: return "Unknown";
    }
}

ItemStack::ItemStack(std::shared_ptr<Item> item, u32 count)
    : item_(item), count_(count) {
}

bool ItemStack::add(u32 amount) {
    if (!item_) return false;
    
    u32 space = get_space();
    u32 to_add = min(amount, space);
    count_ += to_add;
    return to_add == amount;
}

bool ItemStack::remove(u32 amount) {
    if (count_ < amount) return false;
    count_ -= amount;
    return true;
}

u32 ItemStack::get_space() const {
    if (!item_) return 0;
    return item_->get_max_stack() - count_;
}

bool ItemStack::can_merge_with(const ItemStack& other) const {
    if (!item_ || !other.item_) return false;
    return item_->get_id() == other.item_->get_id() && count_ < item_->get_max_stack();
}

bool ItemStack::merge_from(ItemStack& other) {
    if (!can_merge_with(other)) return false;
    
    u32 space = get_space();
    u32 to_move = min(space, other.count_);
    
    count_ += to_move;
    other.count_ -= to_move;
    
    return true;
}

} // namespace chronoraid
