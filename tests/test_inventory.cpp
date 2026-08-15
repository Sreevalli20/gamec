#include <chronoraid/inventory/inventory.hpp>
#include <chronoraid/inventory/item.hpp>
#include <chronoraid/core/logger.hpp>
#include <cassert>
#include <iostream>

using namespace chronoraid;

void test_item_creation() {
    std::cout << "Testing item creation..." << std::endl;
    
    auto item = std::make_shared<Item>("sword_01", "Iron Sword", ItemType::Weapon, ItemRarity::Common);
    
    assert(item->get_id() == "sword_01");
    assert(item->get_name() == "Iron Sword");
    assert(item->get_type() == ItemType::Weapon);
    assert(item->get_rarity() == ItemRarity::Common);
    
    ItemStats stats;
    stats.attack_power = 15.0f;
    stats.defense = 5.0f;
    item->set_stats(stats);
    
    assert(item->get_stats().attack_power == 15.0f);
    assert(item->get_stats().defense == 5.0f);
    
    std::cout << "Item creation test passed!" << std::endl;
}

void test_item_stack() {
    std::cout << "Testing item stack..." << std::endl;
    
    auto item = std::make_shared<Item>("potion_01", "Health Potion", ItemType::Consumable, ItemRarity::Common);
    item->set_max_stack(99);
    
    ItemStack stack(item, 50);
    
    assert(stack.get_count() == 50);
    assert(stack.get_space() == 49);
    
    assert(stack.add(30));
    assert(stack.get_count() == 80);
    
    assert(!stack.add(30));
    assert(stack.get_count() == 99);
    
    assert(stack.remove(20));
    assert(stack.get_count() == 79);
    
    assert(!stack.remove(100));
    
    std::cout << "Item stack test passed!" << std::endl;
}

void test_inventory() {
    std::cout << "Testing inventory..." << std::endl;
    
    Inventory inv(20);
    
    assert(inv.get_slot_count() == 20);
    assert(inv.is_empty());
    assert(!inv.is_full());
    
    auto item1 = std::make_shared<Item>("sword_01", "Iron Sword", ItemType::Weapon, ItemRarity::Common);
    auto item2 = std::make_shared<Item>("potion_01", "Health Potion", ItemType::Consumable, ItemRarity::Common);
    item2->set_max_stack(50);
    
    assert(inv.add_item(item1, 1));
    assert(!inv.is_empty());
    
    assert(inv.add_item(item2, 25));
    assert(inv.add_item(item2, 25));
    
    assert(inv.count_item("potion_01") == 50);
    assert(inv.has_item("potion_01", 50));
    
    assert(inv.remove_item_by_id("potion_01", 10));
    assert(inv.count_item("potion_01") == 40);
    
    assert(inv.get_used_slots() == 2);
    
    std::cout << "Inventory test passed!" << std::endl;
}

void test_item_rarity() {
    std::cout << "Testing item rarity..." << std::endl;
    
    assert(std::string(Item::rarity_to_string(ItemRarity::Common)) == "Common");
    assert(std::string(Item::rarity_to_string(ItemRarity::Legendary)) == "Legendary");
    assert(std::string(Item::rarity_to_string(ItemRarity::Mythic)) == "Mythic");
    
    std::cout << "Item rarity test passed!" << std::endl;
}

void test_equipment() {
    std::cout << "Testing equipment..." << std::endl;
    
    Equipment equip;
    
    auto weapon = std::make_shared<Item>("sword_01", "Iron Sword", ItemType::Weapon, ItemRarity::Common);
    weapon->set_slot(ItemSlot::MainHand);
    ItemStats stats;
    stats.attack_power = 20.0f;
    weapon->set_stats(stats);
    
    ItemStack weapon_stack(weapon, 1);
    
    assert(equip.equip(&weapon_stack));
    assert(equip.get_slot(ItemSlot::MainHand) != nullptr);
    assert(equip.get_total_stats().attack_power == 20.0f);
    
    equip.unequip(ItemSlot::MainHand);
    assert(equip.get_slot(ItemSlot::MainHand) == nullptr);
    assert(equip.get_total_stats().attack_power == 0.0f);
    
    std::cout << "Equipment test passed!" << std::endl;
}

int main() {
    std::cout << "=== Inventory System Tests ===" << std::endl;
    
    test_item_creation();
    test_item_stack();
    test_inventory();
    test_item_rarity();
    test_equipment();
    
    std::cout << "=== All Inventory Tests Passed ===" << std::endl;
    
    return 0;
}
