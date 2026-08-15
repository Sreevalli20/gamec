#include <chronoraid/entities/entity_manager.hpp>
#include <chronoraid/entities/entity.hpp>
#include <chronoraid/core/logger.hpp>

namespace chronoraid {

Entity* EntityManager::create_entity() {
    EntityID id = next_id_++;
    auto entity = std::make_unique<Entity>(id);
    Entity* ptr = entity.get();
    entities_[id] = std::move(entity);
    LOG_DEBUG("Created entity with ID: " + std::to_string(id));
    return ptr;
}

void EntityManager::destroy_entity(EntityID id) {
    auto it = entities_.find(id);
    if (it != entities_.end()) {
        it->second->destroy();
        LOG_DEBUG("Marked entity for destruction: " + std::to_string(id));
    }
}

Entity* EntityManager::get_entity(EntityID id) {
    auto it = entities_.find(id);
    if (it != entities_.end() && !it->second->is_destroyed()) {
        return it->second.get();
    }
    return nullptr;
}

void EntityManager::update(f64 delta_time) {
    for (auto it = entities_.begin(); it != entities_.end(); ) {
        if (it->second->is_destroyed()) {
            LOG_DEBUG("Destroying entity: " + std::to_string(it->first));
            it = entities_.erase(it);
        } else {
            it->second->update(delta_time);
            ++it;
        }
    }
}

void EntityManager::render() {
    for (auto& [id, entity] : entities_) {
        if (!entity->is_destroyed()) {
            entity->render();
        }
    }
}

void EntityManager::clear() {
    entities_.clear();
    next_id_ = 1;
    LOG_INFO("Cleared all entities");
}

} // namespace chronoraid
