#include <chronoraid/npc/npc.hpp>
#include <chronoraid/core/logger.hpp>
#include <chronoraid/inventory/inventory.hpp>
#include <chronoraid/inventory/item.hpp>
#include <chronoraid/core/random.hpp>

namespace chronoraid {

NPC::NPC(EntityID id, NPCType type, const std::string& name) 
    : Entity(id), type_(type), name_(name) {
    set_name(name);
    
    add_component<TransformComponent>();
    add_component<CollisionComponent>();
    
    auto collision = get_component<CollisionComponent>();
    if (collision) {
        collision->bounds = Rect(0, 0, 1.0f, 1.0f);
        collision->collision_layer = 4;
        collision->collision_mask = 1;
    }
}

void NPC::update(f64 delta_time) {
    Entity::update(delta_time);
}

void NPC::add_dialogue_node(const std::string& id, const DialogueNode& node) {
    dialogue_nodes_[id] = node;
}

DialogueNode* NPC::get_dialogue_node(const std::string& id) {
    auto it = dialogue_nodes_.find(id);
    if (it != dialogue_nodes_.end()) {
        return &it->second;
    }
    return nullptr;
}

void NPC::interact(Entity* player) {
    if (!can_interact_) return;
    
    DialogueNode* start_node = get_dialogue_node(start_dialogue_id_);
    if (start_node) {
        LOG_INFO("Interacting with " + name_ + ": " + start_node->text);
    }
}

VendorNPC::VendorNPC(EntityID id, const std::string& name) 
    : NPC(id, NPCType::Vendor, name) {
}

void VendorNPC::add_item_for_sale(const std::string& item_id, u32 price) {
    shop_inventory_.emplace_back(item_id, price);
}

void VendorNPC::remove_item_for_sale(const std::string& item_id) {
    shop_inventory_.erase(
        std::remove_if(shop_inventory_.begin(), shop_inventory_.end(),
            [&item_id](const auto& pair) { return pair.first == item_id; }),
        shop_inventory_.end()
    );
}

bool VendorNPC::buy_item(const std::string& item_id, Entity* player) {
    for (const auto& [id, price] : shop_inventory_) {
        if (id == item_id) {
            f32 discounted_price = price * get_discount_multiplier();
            
            auto item = ItemDatabase::instance().create_item(item_id);
            if (item) {
                LOG_INFO("Sold item: " + item_id + " for " + std::to_string(discounted_price));
                return true;
            }
        }
    }
    return false;
}

bool VendorNPC::sell_item(const std::string& item_id, Entity* player) {
    return false;
}

f32 VendorNPC::get_discount_multiplier() const {
    f32 reputation_bonus = reputation_ * 0.01f;
    return 1.0f - clamp(reputation_bonus, 0.0f, 0.5f);
}

void NPCManager::register_npc(NPC* npc) {
    if (npc) {
        npcs_[npc->get_id()] = npc;
        LOG_INFO("Registered NPC: " + npc->get_name());
    }
}

void NPCManager::unregister_npc(EntityID id) {
    auto it = npcs_.find(id);
    if (it != npcs_.end()) {
        npcs_.erase(it);
        LOG_INFO("Unregistered NPC: " + std::to_string(id));
    }
}

NPC* NPCManager::get_npc(EntityID id) {
    auto it = npcs_.find(id);
    if (it != npcs_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<NPC*> NPCManager::get_npcs_in_range(const Vector2& position, f32 range) {
    std::vector<NPC*> result;
    
    for (auto& [id, npc] : npcs_) {
        auto transform = npc->get_component<TransformComponent>();
        if (transform) {
            if (transform->position.distance_to(position) <= range) {
                result.push_back(npc);
            }
        }
    }
    
    return result;
}

void NPCManager::update(f64 delta_time) {
    for (auto& [id, npc] : npcs_) {
        if (npc) {
            npc->update(delta_time);
        }
    }
}

} // namespace chronoraid
