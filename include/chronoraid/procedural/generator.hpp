#pragma once

#include "../world/world.hpp"
#include "../core/random.hpp"
#include "../core/common.hpp"
#include <vector>

namespace chronoraid {

enum class GenerationAlgorithm {
    RandomWalk,
    CellularAutomata,
    BSP,
    DrunkardWalk,
    RoomPlacement
};

class ProceduralGenerator {
public:
    ProceduralGenerator() = default;

    void set_seed(u32 seed);
    u32 get_seed() const { return seed_; }

    void set_algorithm(GenerationAlgorithm algo) { algorithm_ = algo; }
    GenerationAlgorithm get_algorithm() const { return algorithm_; }

    void set_map_size(u32 width, u32 height);
    void set_room_count(u32 count) { room_count_ = count; }
    void set_min_room_size(u32 size) { min_room_size_ = size; }
    void set_max_room_size(u32 size) { max_room_size_ = size; }

    World generate();

    static World generate_from_seed(u32 seed);

private:
    void generate_random_walk();
    void generate_cellular_automata();
    void generate_bsp();
    void generate_drunkard_walk();
    void generate_room_placement();

    void connect_rooms(const std::vector<Room>& rooms);
    void place_doors();
    void place_hazards();
    void place_spawn_points();

    u32 seed_ = 0;
    Random random_;
    GenerationAlgorithm algorithm_ = GenerationAlgorithm::RoomPlacement;

    u32 map_width_ = 100;
    u32 map_height_ = 100;
    u32 room_count_ = 10;
    u32 min_room_size_ = 5;
    u32 max_room_size_ = 15;
};

class BSPNode {
public:
    BSPNode(const Rect& bounds) : bounds_(bounds) {}

    const Rect& get_bounds() const { return bounds_; }
    BSPNode* get_left() const { return left_.get(); }
    BSPNode* get_right() const { return right_.get(); }

    bool split(u32 min_size);
    Room extract_room();

private:
    Rect bounds_;
    std::unique_ptr<BSPNode> left_;
    std::unique_ptr<BSPNode> right_;
};

} // namespace chronoraid
