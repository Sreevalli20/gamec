#include <chronoraid/world/world.hpp>
#include <chronoraid/core/logger.hpp>
#include <chronoraid/entities/entity.hpp>

namespace chronoraid {

TileMap::TileMap(u32 width, u32 height) 
    : width_(width), height_(height), tiles_(width * height) {
}

Tile& TileMap::get_tile(u32 x, u32 y) {
    if (x >= width_ || y >= height_) {
        static Tile invalid_tile;
        return invalid_tile;
    }
    return tiles_[y * width_ + x];
}

const Tile& TileMap::get_tile(u32 x, u32 y) const {
    if (x >= width_ || y >= height_) {
        static Tile invalid_tile;
        return invalid_tile;
    }
    return tiles_[y * width_ + x];
}

void TileMap::set_tile(u32 x, u32 y, const Tile& tile) {
    if (x < width_ && y < height_) {
        tiles_[y * width_ + x] = tile;
    }
}

void TileMap::set_tile_type(u32 x, u32 y, TileType type) {
    if (x < width_ && y < height_) {
        tiles_[y * width_ + x].type = type;
        tiles_[y * width_ + x].is_solid = (type == TileType::Wall);
        tiles_[y * width_ + x].is_hazard = (type == TileType::Hazard || type == TileType::Lava);
    }
}

bool TileMap::is_solid(u32 x, u32 y) const {
    if (x >= width_ || y >= height_) return true;
    return tiles_[y * width_ + x].is_solid;
}

bool TileMap::is_valid(u32 x, u32 y) const {
    return x < width_ && y < height_;
}

void TileMap::clear(TileType type) {
    for (auto& tile : tiles_) {
        tile.type = type;
        tile.is_solid = (type == TileType::Wall);
        tile.is_hazard = false;
    }
}

Vector2 TileMap::tile_to_world(u32 x, u32 y) const {
    return Vector2(static_cast<f32>(x) * tile_size_, static_cast<f32>(y) * tile_size_);
}

std::pair<u32, u32> TileMap::world_to_tile(const Vector2& world_pos) const {
    return {
        static_cast<u32>(world_pos.x / tile_size_),
        static_cast<u32>(world_pos.y / tile_size_)
    };
}

void World::initialize(u32 width, u32 height) {
    tile_map_ = TileMap(width, height);
    rooms_.clear();
    current_room_ = nullptr;
    spawn_point_ = Vector2(5.0f, 5.0f);
    LOG_INFO("World initialized with size: " + std::to_string(width) + "x" + std::to_string(height));
}

void World::shutdown() {
    rooms_.clear();
    current_room_ = nullptr;
    LOG_INFO("World shutdown");
}

Entity* World::spawn_entity(const Vector2& position) {
    auto entity = EntityManager::instance().create_entity();
    auto transform = entity->get_component<TransformComponent>();
    if (transform) {
        transform->position = position;
    }
    return entity;
}

void World::destroy_entity(EntityID id) {
    EntityManager::instance().destroy_entity(id);
}

void World::update(f64 delta_time) {
}

void World::render() {
}

} // namespace chronoraid
