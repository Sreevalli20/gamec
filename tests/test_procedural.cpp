#include <chronoraid/procedural/generator.hpp>
#include <chronoraid/world/world.hpp>
#include <chronoraid/core/logger.hpp>
#include <cassert>
#include <iostream>

using namespace chronoraid;

void test_deterministic_generation() {
    std::cout << "Testing deterministic generation..." << std::endl;
    
    u32 seed = 12345;
    
    ProceduralGenerator gen1;
    gen1.set_seed(seed);
    gen1.set_algorithm(GenerationAlgorithm::RoomPlacement);
    gen1.set_map_size(50, 30);
    gen1.set_room_count(5);
    World world1 = gen1.generate();
    
    ProceduralGenerator gen2;
    gen2.set_seed(seed);
    gen2.set_algorithm(GenerationAlgorithm::RoomPlacement);
    gen2.set_map_size(50, 30);
    gen2.set_room_count(5);
    World world2 = gen2.generate();
    
    const auto& tilemap1 = world1.get_tile_map();
    const auto& tilemap2 = world2.get_tile_map();
    
    bool identical = true;
    for (u32 y = 0; y < tilemap1.get_height() && identical; ++y) {
        for (u32 x = 0; x < tilemap1.get_width() && identical; ++x) {
            if (tilemap1.get_tile(x, y).type != tilemap2.get_tile(x, y).type) {
                identical = false;
            }
        }
    }
    
    assert(identical);
    
    std::cout << "Deterministic generation test passed!" << std::endl;
}

void test_room_placement() {
    std::cout << "Testing room placement..." << std::endl;
    
    ProceduralGenerator gen;
    gen.set_seed(54321);
    gen.set_algorithm(GenerationAlgorithm::RoomPlacement);
    gen.set_map_size(80, 40);
    gen.set_room_count(8);
    
    World world = gen.generate();
    
    const auto& tilemap = world.get_tile_map();
    const auto& rooms = world.get_rooms();
    
    assert(rooms.size() > 0);
    assert(rooms.size() <= 8);
    
    u32 floor_count = 0;
    for (u32 y = 0; y < tilemap.get_height(); ++y) {
        for (u32 x = 0; x < tilemap.get_width(); ++x) {
            if (tilemap.get_tile(x, y).type == TileType::Floor) {
                floor_count++;
            }
        }
    }
    
    assert(floor_count > 100);
    
    std::cout << "Room placement test passed!" << std::endl;
}

void test_random_walk() {
    std::cout << "Testing random walk..." << std::endl;
    
    ProceduralGenerator gen;
    gen.set_seed(99999);
    gen.set_algorithm(GenerationAlgorithm::RandomWalk);
    gen.set_map_size(40, 20);
    
    World world = gen.generate();
    
    const auto& tilemap = world.get_tile_map();
    
    u32 floor_count = 0;
    for (u32 y = 0; y < tilemap.get_height(); ++y) {
        for (u32 x = 0; x < tilemap.get_width(); ++x) {
            if (tilemap.get_tile(x, y).type == TileType::Floor) {
                floor_count++;
            }
        }
    }
    
    assert(floor_count > 50);
    
    std::cout << "Random walk test passed!" << std::endl;
}

void test_cellular_automata() {
    std::cout << "Testing cellular automata..." << std::endl;
    
    ProceduralGenerator gen;
    gen.set_seed(77777);
    gen.set_algorithm(GenerationAlgorithm::CellularAutomata);
    gen.set_map_size(40, 20);
    
    World world = gen.generate();
    
    const auto& tilemap = world.get_tile_map();
    
    u32 floor_count = 0;
    u32 wall_count = 0;
    for (u32 y = 0; y < tilemap.get_height(); ++y) {
        for (u32 x = 0; x < tilemap.get_width(); ++x) {
            if (tilemap.get_tile(x, y).type == TileType::Floor) {
                floor_count++;
            } else if (tilemap.get_tile(x, y).type == TileType::Wall) {
                wall_count++;
            }
        }
    }
    
    assert(floor_count > 0);
    assert(wall_count > 0);
    
    std::cout << "Cellular automata test passed!" << std::endl;
}

void test_bsp_generation() {
    std::cout << "Testing BSP generation..." << std::endl;
    
    ProceduralGenerator gen;
    gen.set_seed(11111);
    gen.set_algorithm(GenerationAlgorithm::BSP);
    gen.set_map_size(60, 30);
    gen.set_min_room_size(5);
    
    World world = gen.generate();
    
    const auto& rooms = world.get_rooms();
    
    assert(rooms.size() > 0);
    
    std::cout << "BSP generation test passed!" << std::endl;
}

void test_tilemap() {
    std::cout << "Testing tilemap..." << std::endl;
    
    TileMap tilemap(20, 15);
    
    assert(tilemap.get_width() == 20);
    assert(tilemap.get_height() == 15);
    
    tilemap.set_tile_type(5, 5, TileType::Wall);
    assert(tilemap.get_tile(5, 5).type == TileType::Wall);
    assert(tilemap.is_solid(5, 5));
    
    tilemap.set_tile_type(5, 5, TileType::Floor);
    assert(!tilemap.is_solid(5, 5));
    
    assert(!tilemap.is_valid(25, 5));
    assert(!tilemap.is_valid(5, 20));
    
    Vector2 world_pos = tilemap.tile_to_world(3, 4);
    assert(world_pos.x == 3.0f * 32.0f);
    assert(world_pos.y == 4.0f * 32.0f);
    
    auto [tile_x, tile_y] = tilemap.world_to_tile(world_pos);
    assert(tile_x == 3);
    assert(tile_y == 4);
    
    std::cout << "Tilemap test passed!" << std::endl;
}

int main() {
    std::cout << "=== Procedural Generation Tests ===" << std::endl;
    
    test_deterministic_generation();
    test_room_placement();
    test_random_walk();
    test_cellular_automata();
    test_bsp_generation();
    test_tilemap();
    
    std::cout << "=== All Procedural Generation Tests Passed ===" << std::endl;
    
    return 0;
}
