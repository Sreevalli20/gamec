#pragma once

#include "../core/common.hpp"
#include "../core/vector2.hpp"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace chronoraid {

class Entity;
class Component;
class EntityManager;

using EntityID = u64;

class Entity {
public:
    Entity(EntityID id) : id_(id) {}
    virtual ~Entity() = default;

    EntityID get_id() const { return id_; }
    const std::string& get_name() const { return name_; }
    void set_name(const std::string& name) { name_ = name; }

    bool is_active() const { return is_active_; }
    void set_active(bool active) { is_active_ = active; }

    bool is_destroyed() const { return is_destroyed_; }
    void destroy() { is_destroyed_ = true; }

    template<typename T, typename... Args>
    T* add_component(Args&&... args) {
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = component.get();
        components_[typeid(T).hash_code()] = std::move(component);
        return ptr;
    }

    template<typename T>
    T* get_component() {
        auto it = components_.find(typeid(T).hash_code());
        if (it != components_.end()) {
            return static_cast<T*>(it->second.get());
        }
        return nullptr;
    }

    template<typename T>
    const T* get_component() const {
        auto it = components_.find(typeid(T).hash_code());
        if (it != components_.end()) {
            return static_cast<const T*>(it->second.get());
        }
        return nullptr;
    }

    template<typename T>
    bool has_component() const {
        return components_.count(typeid(T).hash_code()) > 0;
    }

    template<typename T>
    void remove_component() {
        components_.erase(typeid(T).hash_code());
    }

    virtual void update(f64 delta_time);
    virtual void render();

protected:
    EntityID id_;
    std::string name_;
    bool is_active_ = true;
    bool is_destroyed_ = false;

    std::unordered_map<size_t, std::unique_ptr<Component>> components_;
};

class Component {
public:
    virtual ~Component() = default;

    Entity* get_owner() const { return owner_; }
    void set_owner(Entity* owner) { owner_ = owner; }

    virtual void update(f64 delta_time) {}
    virtual void render() {}

protected:
    Entity* owner_ = nullptr;
};

} // namespace chronoraid
