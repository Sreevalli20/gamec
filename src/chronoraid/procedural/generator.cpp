#include <chronoraid/procedural/generator.hpp>
#include <chronoraid/core/logger.hpp>
#include <algorithm>
#include <queue>

namespace chronoraid {

void ProceduralGenerator::set_seed(u32 seed) {
    seed_ = seed;
    random_.set_seed(seed);
}

void ProceduralGenerator::set_map_size(u32 width, u32 height) {
    map_width_ = width;
    map_height_ = height;
}

World ProceduralGenerator::generate() {
    World world;
    world.initialize(map_width_, map_height_);
    
    switch (algorithm_) {
        case GenerationAlgorithm::RandomWalk:
            generate_random_walk();
            break;
        case GenerationAlgorithm::CellularAutomata:
            generate_cellular_automata();
            break;
        case GenerationAlgorithm::BSP:
            generate_bsp();
            break;
        case GenerationAlgorithm::DrunkardWalk:
            generate_drunkard_walk();
            break;
        case GenerationAlgorithm::RoomPlacement:
            generate_room_placement();
            break;
    }
    
    LOG_INFO("Generated world with seed: " + std::to_string(seed_));
    return world;
}

World ProceduralGenerator::generate_from_seed(u32 seed) {
    ProceduralGenerator generator;
    generator.set_seed(seed);
    return generator.generate();
}

void ProceduralGenerator::generate_random_walk() {
    auto& tilemap = World::instance().get_tile_map();
    tilemap.clear(TileType::Wall);
    
    Vector2 position(map_width_ / 2, map_height_ / 2);
    u32 steps = map_width_ * map_height_ * 2;
    
    for (u32 i = 0; i < steps; ++i) {
        u32 x = static_cast<u32>(position.x);
        u32 y = static_cast<u32>(position.y);
        
        if (tilemap.is_valid(x, y)) {
            tilemap.set_tile_type(x, y, TileType::Floor);
        }
        
        int direction = random_.next_i32(0, 3);
        switch (direction) {
            case 0: position.x += 1.0f; break;
            case 1: position.x -= 1.0f; break;
            case 2: position.y += 1.0f; break;
            case 3: position.y -= 1.0f; break;
        }
        
        position.x = clamp(position.x, 1.0f, static_cast<f32>(map_width_ - 2));
        position.y = clamp(position.y, 1.0f, static_cast<f32>(map_height_ - 2));
    }
    
    World::instance().set_spawn_point(Vector2(map_width_ / 2, map_height_ / 2));
}

void ProceduralGenerator::generate_cellular_automata() {
    auto& tilemap = World::instance().get_tile_map();
    tilemap.clear(TileType::Wall);
    
    for (u32 y = 0; y < map_height_; ++y) {
        for (u32 x = 0; x < map_width_; ++x) {
            if (random_.next_bool(0.45f)) {
                tilemap.set_tile_type(x, y, TileType::Floor);
            }
        }
    }
    
    for (int iteration = 0; iteration < 5; ++iteration) {
        TileMap new_tilemap = tilemap;
        for (u32 y = 1; y < map_height_ - 1; ++y) {
            for (u32 x = 1; x < map_width_ - 1; ++x) {
                u32 wall_count = 0;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0) continue;
                        if (tilemap.get_tile(x + dx, y + dy).type == TileType::Wall) {
                            wall_count++;
                        }
                    }
                }
                
                if (wall_count > 4) {
                    new_tilemap.set_tile_type(x, y, TileType::Wall);
                } else if (wall_count < 3) {
                    new_tilemap.set_tile_type(x, y, TileType::Floor);
                }
            }
        }
        tilemap = new_tilemap;
    }
    
    World::instance().set_spawn_point(Vector2(map_width_ / 2, map_height_ / 2));
}

void ProceduralGenerator::generate_bsp() {
    auto& tilemap = World::instance().get_tile_map();
    tilemap.clear(TileType::Wall);
    
    Rect bounds(1, 1, map_width_ - 2, map_height_ - 2);
    auto root = std::make_unique<BSPNode>(bounds);
    
    std::queue<BSPNode*> queue;
    queue.push(root.get());
    
    while (!queue.empty()) {
        BSPNode* node = queue.front();
        queue.pop();
        
        if (node->split(min_room_size_)) {
            queue.push(node->get_left());
            queue.push(node->get_right());
        }
    }
    
    std::vector<Room> rooms;
    std::queue<BSPNode*> extract_queue;
    extract_queue.push(root.get());
    
    while (!extract_queue.empty()) {
        BSPNode* node = extract_queue.front();
        extract_queue.pop();
        
        if (!node->get_left() && !node->get_right()) {
            Room room = node->extract_room();
            rooms.push_back(room);
            
            const auto& room_bounds = room.get_bounds();
            for (u32 y = static_cast<u32>(room_bounds.top()); y < static_cast<u32>(room_bounds.bottom()); ++y) {
                for (u32 x = static_cast<u32>(room_bounds.left()); x < static_cast<u32>(room_bounds.right()); ++x) {
                    if (tilemap.is_valid(x, y)) {
                        tilemap.set_tile_type(x, y, TileType::Floor);
                    }
                }
            }
        } else {
            if (node->get_left()) extract_queue.push(node->get_left());
            if (node->get_right()) extract_queue.push(node->get_right());
        }
    }
    
    connect_rooms(rooms);
    
    for (const auto& room : rooms) {
        World::instance().add_room(room);
    }
    
    if (!rooms.empty()) {
        World::instance().set_spawn_point(rooms[0].get_bounds().center());
    }
}

void ProceduralGenerator::generate_drunkard_walk() {
    auto& tilemap = World::instance().get_tile_map();
    tilemap.clear(TileType::Wall);
    
    std::vector<Vector2> walkers;
    u32 num_walkers = 10;
    
    for (u32 i = 0; i < num_walkers; ++i) {
        walkers.push_back(Vector2(map_width_ / 2, map_height_ / 2));
    }
    
    u32 steps = map_width_ * map_height_ * 3;
    
    for (u32 step = 0; step < steps; ++step) {
        for (auto& walker : walkers) {
            u32 x = static_cast<u32>(walker.x);
            u32 y = static_cast<u32>(walker.y);
            
            if (tilemap.is_valid(x, y)) {
                tilemap.set_tile_type(x, y, TileType::Floor);
            }
            
            int direction = random_.next_i32(0, 3);
            switch (direction) {
                case 0: walker.x += 1.0f; break;
                case 1: walker.x -= 1.0f; break;
                case 2: walker.y += 1.0f; break;
                case 3: walker.y -= 1.0f; break;
            }
            
            walker.x = clamp(walker.x, 1.0f, static_cast<f32>(map_width_ - 2));
            walker.y = clamp(walker.y, 1.0f, static_cast<f32>(map_height_ - 2));
        }
    }
    
    World::instance().set_spawn_point(Vector2(map_width_ / 2, map_height_ / 2));
}

void ProceduralGenerator::generate_room_placement() {
    auto& tilemap = World::instance().get_tile_map();
    tilemap.clear(TileType::Wall);
    
    std::vector<Room> rooms;
    u32 attempts = 0;
    const u32 max_attempts = 1000;
    
    while (rooms.size() < room_count_ && attempts < max_attempts) {
        u32 w = random_.next_u32(min_room_size_, max_room_size_);
        u32 h = random_.next_u32(min_room_size_, max_room_size_);
        u32 x = random_.next_u32(1, map_width_ - w - 2);
        u32 y = random_.next_u32(1, map_height_ - h - 2);
        
        Rect new_room(static_cast<f32>(x), static_cast<f32>(y), static_cast<f32>(w), static_cast<f32>(h));
        
        bool overlaps = false;
        for (const auto& room : rooms) {
            Rect expanded = room.get_bounds();
            expanded.x -= 2.0f;
            expanded.y -= 2.0f;
            expanded.width += 4.0f;
            expanded.height += 4.0f;
            
            if (expanded.intersects(new_room)) {
                overlaps = true;
                break;
            }
        }
        
        if (!overlaps) {
            Room room(new_room);
            
            for (u32 ry = y; ry < y + h; ++ry) {
                for (u32 rx = x; rx < x + w; ++rx) {
                    if (tilemap.is_valid(rx, ry)) {
                        tilemap.set_tile_type(rx, ry, TileType::Floor);
                    }
                }
            }
            
            room.add_enemy_spawn_point(new_room.center());
            rooms.push_back(room);
        }
        
        attempts++;
    }
    
    connect_rooms(rooms);
    
    for (const auto& room : rooms) {
        World::instance().add_room(room);
    }
    
    if (!rooms.empty()) {
        World::instance().set_spawn_point(rooms[0].get_bounds().center());
        World::instance().set_current_room(&World::instance().get_rooms()[0]);
    }
    
    LOG_INFO("Generated " + std::to_string(rooms.size()) + " rooms in " + std::to_string(attempts) + " attempts");
}

void ProceduralGenerator::connect_rooms(const std::vector<Room>& rooms) {
    auto& tilemap = World::instance().get_tile_map();
    
    for (usize i = 1; i < rooms.size(); ++i) {
        const auto& room_a = rooms[i - 1].get_bounds();
        const auto& room_b = rooms[i].get_bounds();
        
        Vector2 start = room_a.center();
        Vector2 end = room_b.center();
        
        if (random_.next_bool()) {
            for (u32 x = static_cast<u32>(min(start.x, end.x)); x <= static_cast<u32>(max(start.x, end.x)); ++x) {
                if (tilemap.is_valid(x, static_cast<u32>(start.y))) {
                    tilemap.set_tile_type(x, static_cast<u32>(start.y), TileType::Floor);
                }
            }
            for (u32 y = static_cast<u32>(min(start.y, end.y)); y <= static_cast<u32>(max(start.y, end.y)); ++y) {
                if (tilemap.is_valid(static_cast<u32>(end.x), y)) {
                    tilemap.set_tile_type(static_cast<u32>(end.x), y, TileType::Floor);
                }
            }
        } else {
            for (u32 y = static_cast<u32>(min(start.y, end.y)); y <= static_cast<u32>(max(start.y, end.y)); ++y) {
                if (tilemap.is_valid(static_cast<u32>(start.x), y)) {
                    tilemap.set_tile_type(static_cast<u32>(start.x), y, TileType::Floor);
                }
            }
            for (u32 x = static_cast<u32>(min(start.x, end.x)); x <= static_cast<u32>(max(start.x, end.x)); ++x) {
                if (tilemap.is_valid(x, static_cast<u32>(end.y))) {
                    tilemap.set_tile_type(x, static_cast<u32>(end.y), TileType::Floor);
                }
            }
        }
    }
}

void BSPNode::split(u32 min_size) {
    if (left_ || right_) return;
    
    bool split_h = random_.next_bool();
    
    if (bounds_.width > bounds_.height) {
        split_h = false;
    } else if (bounds_.height > bounds_.width) {
        split_h = true;
    }
    
    u32 max = (split_h ? static_cast<u32>(bounds_.height) : static_cast<u32>(bounds_.width)) - min_size;
    
    if (max <= min_size) {
        return;
    }
    
    u32 split = random_.next_u32(min_size, max);
    
    if (split_h) {
        left_ = std::make_unique<BSPNode>(Rect(bounds_.x, bounds_.y, bounds_.width, split));
        right_ = std::make_unique<BSPNode>(Rect(bounds_.x, bounds_.y + split, bounds_.width, bounds_.height - split));
    } else {
        left_ = std::make_unique<BSPNode>(Rect(bounds_.x, bounds_.y, split, bounds_.height));
        right_ = std::make_unique<BSPNode>(Rect(bounds_.x + split, bounds_.y, bounds_.width - split, bounds_.height));
    }
}

Room BSPNode::extract_room() {
    u32 w = random_.next_u32(static_cast<u32>(bounds_.width * 0.4f), static_cast<u32>(bounds_.width * 0.8f));
    u32 h = random_.next_u32(static_cast<u32>(bounds_.height * 0.4f), static_cast<u32>(bounds_.height * 0.8f));
    u32 x = random_.next_u32(static_cast<u32>(bounds_.x), static_cast<u32>(bounds_.x + bounds_.width - w));
    u32 y = random_.next_u32(static_cast<u32>(bounds_.y), static_cast<u32>(bounds_.y + bounds_.height - h));
    
    return Room(Rect(static_cast<f32>(x), static_cast<f32>(y), static_cast<f32>(w), static_cast<f32>(h)));
}

} // namespace chronoraid
