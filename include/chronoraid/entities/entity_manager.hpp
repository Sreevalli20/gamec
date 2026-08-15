#pragma once

#include "entity.hpp"
#include "../core/common.hpp"
#include <unordered_map>
#include <memory>

namespace chronoraid {

class EntityManager {
public:
    static EntityManager& instance() {
        static EntityManager manager;
        return manager;
    }

    Entity* create_entity();
    void destroy_entity(EntityID id);
    Entity* get_entity(EntityID id);

    template<typename T, typename... Args>
    T* create_entity(Args&&... args) {
        Entity* entity = create_entity();
        T* derived = static_cast<T*>(entity);
        derived->T(std::forward<Args>(args)...);
        return derived;
    }

    void update(f64 delta_time);
    void render();

    void clear();

private:
    EntityManager() : next_id_(1) {}

    EntityID next_id_;
    std::unordered_map<EntityID, std::unique_ptr<Entity>> entities_;
};

} // namespace chronoraid
