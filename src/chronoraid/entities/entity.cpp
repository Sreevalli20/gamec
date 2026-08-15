#include <chronoraid/entities/entity.hpp>
#include <chronoraid/entities/entity_manager.hpp>
#include <chronoraid/core/logger.hpp>

namespace chronoraid {

void Entity::update(f64 delta_time) {
    if (!is_active_ || is_destroyed_) return;
    
    for (auto& [hash, component] : components_) {
        if (component) {
            component->update(delta_time);
        }
    }
}

void Entity::render() {
    if (!is_active_ || is_destroyed_) return;
    
    for (auto& [hash, component] : components_) {
        if (component) {
            component->render();
        }
    }
}

} // namespace chronoraid
