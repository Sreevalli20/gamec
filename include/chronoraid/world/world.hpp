#pragma once

#include "../core/common.hpp"
#include "../core/vector2.hpp"
#include "../core/rect.hpp"
#include "../entities/entity.hpp"
#include <vector>
#include <memory>

namespace chronoraid {

class EntityManager;

enum class TileType {
    Empty,
    Floor,
    Wall,
    Door,
    Hazard,
    Water,
    Lava,
    Ice,
    Exit
};

struct Tile {
    TileType type = TileType::Empty;
    u32 variant = 0;
    bool is_solid = false;
    bool is_hazard = false;
    f32 damage_amount = 0.0f;
};

class TileMap {
public:
    TileMap(u32 width, u32 height);
    ~TileMap() = default;

    u32 get_width() const { return width_; }
    u32 get_height() const { return height_; }

    Tile& get_tile(u32 x, u32 y);
    const Tile& get_tile(u32 x, u32 y) const;

    void set_tile(u32 x, u32 y, const Tile& tile);
    void set_tile_type(u32 x, u32 y, TileType type);

    bool is_solid(u32 x, u32 y) const;
    bool is_valid(u32 x, u32 y) const;

    void clear(TileType type = TileType::Empty);

    Vector2 tile_to_world(u32 x, u32 y) const;
    std::pair<u32, u32> world_to_tile(const Vector2& world_pos) const;

    void set_tile_size(f32 size) { tile_size_ = size; }
    f32 get_tile_size() const { return tile_size_; }

private:
    u32 width_;
    u32 height_;
    f32 tile_size_ = 32.0f;
    std::vector<Tile> tiles_;
};

class Room {
public:
    Room(const Rect& bounds) : bounds_(bounds) {}

    const Rect& get_bounds() const { return bounds_; }
    void set_bounds(const Rect& bounds) { bounds_ = bounds; }

    const std::vector<Vector2>& get_enemy_spawn_points() const { return enemy_spawn_points_; }
    void add_enemy_spawn_point(const Vector2& point) { enemy_spawn_points_.push_back(point); }

    const std::vector<Vector2>& get_item_spawn_points() const { return item_spawn_points_; }
    void add_item_spawn_point(const Vector2& point) { item_spawn_points_.push_back(point); }

    bool is_cleared() const { return is_cleared_; }
    void set_cleared(bool cleared) { is_cleared_ = cleared; }

    bool has_boss() const { return has_boss_; }
    void set_has_boss(bool has_boss) { has_boss_ = has_boss; }

private:
    Rect bounds_;
    std::vector<Vector2> enemy_spawn_points_;
    std::vector<Vector2> item_spawn_points_;
    bool is_cleared_ = false;
    bool has_boss_ = false;
};

class World {
public:
    static World& instance() {
        static World world;
        return world;
    }

    void initialize(u32 width, u32 height);
    void shutdown();

    TileMap& get_tile_map() { return tile_map_; }
    const TileMap& get_tile_map() const { return tile_map_; }

    const std::vector<Room>& get_rooms() const { return rooms_; }
    void add_room(const Room& room) { rooms_.push_back(room); }

    Room* get_current_room() { return current_room_; }
    void set_current_room(Room* room) { current_room_ = room; }

    Entity* spawn_entity(const Vector2& position);
    void destroy_entity(EntityID id);

    void update(f64 delta_time);
    void render();

    Vector2 get_spawn_point() const { return spawn_point_; }
    void set_spawn_point(const Vector2& point) { spawn_point_ = point; }

private:
    World() = default;

    TileMap tile_map_;
    std::vector<Room> rooms_;
    Room* current_room_ = nullptr;
    Vector2 spawn_point_;
};

} // namespace chronoraid
