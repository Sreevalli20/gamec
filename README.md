# ChronoRaid: The Last Timeline

A complete, playable C++20 action roguelite game featuring time manipulation mechanics, procedural generation, and deep progression systems.

## Game Description

ChronoRaid: The Last Timeline is a 2D terminal-based action roguelite where you control a time-traveling explorer attempting to repair a collapsing timeline. Navigate procedurally generated dungeons, defeat diverse enemies with unique AI patterns, challenge epic bosses, and harness time-based abilities to survive.

## Features

### Core Gameplay
- **Time Manipulation Mechanics**: Rewind, Time Freeze, Temporal Dash, Echo, and Timeline Shift abilities
- **Procedural World Generation**: Multiple algorithms (Room Placement, Random Walk, Cellular Automata, BSP, Drunkard's Walk)
- **Deterministic Seeding**: Same seed always generates the same world
- **Real-time Combat**: Damage calculation, critical hits, armor reduction, knockback

### Enemy System
- **6 Enemy Archetypes**: Chrono Drone, Rift Beast, Void Archer, Time Assassin, Temporal Tank, Rift Mage
- **Unique AI Behaviors**: Chase, Patrol, Ranged, Tank AI patterns
- **3 Distinct Bosses**: The Clockmaker, The Paradox Beast, The Null Emperor
- **Multi-phase Boss Battles**: Phase transitions, telegraphed attacks, unique mechanics

### Progression Systems
- **Experience & Leveling**: XP-based progression with skill points
- **Skill Tree**: Combat, Temporal, and Survival upgrade branches
- **Inventory System**: Items, equipment, consumables with rarity tiers
- **Loot Generation**: Deterministic loot based on level, area difficulty, and rarity chances

### Quest & Achievement Systems
- **Quest System**: Main quests, side quests, objectives, rewards
- **Achievement System**: 20+ achievements with progress tracking
- **Statistics Tracking**: Kills, deaths, damage dealt, play time, and more

### Game Modes
- **Story Mode**: Main campaign experience
- **Endless Mode**: Infinite procedural encounters
- **Challenge Mode**: Special rule-based challenges
- **Difficulty Levels**: Easy, Normal, Hard, Nightmare

### Technical Features
- **Save/Load System**: Versioned save format with backup support
- **Terminal UI**: Cross-platform terminal-based interface
- **Component-Based Entity System**: Modular architecture
- **Comprehensive Test Suite**: Unit tests for all major systems

## Architecture

### Project Structure
```
ChronoRaid/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── LICENSE                 # MIT License
├── build.bat / build.sh    # Build scripts
├── include/chronoraid/
│   ├── core/              # Core systems (game loop, input, logging)
│   ├── entities/          # Entity-component system
│   ├── combat/            # Combat mechanics
│   ├── ai/                # Enemy AI controllers
│   ├── world/             # World and tilemap
│   ├── procedural/        # Procedural generation
│   ├── inventory/         # Items and inventory
│   ├── quests/            # Quest system
│   ├── progression/       # Leveling and skills
│   ├── save/              # Save/load system
│   ├── achievements/      # Achievement tracking
│   ├── ui/                # Terminal UI
│   ├── gameplay/          # Player, enemies, bosses
│   └── npc/               # NPCs and dialogue
├── src/chronoraid/        # Implementation files
├── tests/                 # Unit tests
├── docs/                  # Technical documentation
├── assets/                # Game assets
└── config/                # Configuration files
```

### Key Systems

#### Entity-Component System
- Transform, Health, Movement, Combat, Experience, Collision, StatusEffect components
- EntityManager for lifecycle management
- Component-based architecture for flexibility

#### Combat System
- Damage calculation with armor reduction
- Critical hit system
- Attack cooldowns and speed
- Knockback and status effects

#### AI System
- State machine-based AI controllers
- Chase, Patrol, Ranged, Tank behaviors
- Configurable ranges and parameters

#### Procedural Generation
- Multiple generation algorithms
- Room placement with collision detection
- Corridor generation
- Deterministic seeding

#### Time Mechanics
- Time rewind with state recording
- Time freeze for enemies
- Temporal dash for mobility
- Echo ability for position replay
- Timeline shift for global time scale

## Build Instructions

### Prerequisites
- C++20 compatible compiler (MSVC 2022, GCC 11+, Clang 13+)
- CMake 3.20 or higher
- Windows: Visual Studio 2022 recommended

### Building on Windows
```batch
build.bat
```

### Building on Linux/macOS
```bash
chmod +x build.sh
./build.sh
```

### Manual Build
```bash
mkdir build
cd build
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

## Running the Game

After building, run the executable:
```batch
build\bin\Release\chronoraid.exe
```

## Controls

- **WASD / Arrow Keys**: Move player
- **SPACE**: Attack
- **Q**: Temporal Dash
- **E**: Time Freeze
- **R**: Rewind
- **ESC**: Return to menu
- **1-4**: Menu selection

## Gameplay Instructions

### Starting the Game
1. Run the executable
2. Select "New Game" from the main menu
3. A procedurally generated world is created
4. Navigate using WASD, attack enemies with SPACE

### Time Abilities
- **Temporal Dash (Q)**: Quick dash in facing direction (25 energy)
- **Time Freeze (E)**: Freeze nearby enemies (30 energy)
- **Rewind (R)**: Rewind player state and heal (40 energy)

### Combat
- Approach enemies and attack with SPACE
- Watch your health bar (displayed at bottom)
- Defeat enemies to gain XP and items
- Higher difficulty enemies give more rewards

### Progression
- Gain XP by defeating enemies
- Level up to earn skill points
- Spend skill points in the skill tree
- Collect loot to improve your character

### Saving
- Game auto-saves periodically
- Manual save through the menu
- Multiple save slots available

## Testing

Run the test suite:
```bash
cd build
ctest --test-dir build
```

Individual tests:
```bash
./test_combat
./test_inventory
./test_procedural
./test_progression
./test_save_load
./test_achievements
./test_quests
```

## Technical Decisions

### Terminal-Based Rendering
- Chosen for maximum compatibility
- No external graphics dependencies
- Cross-platform support (Windows/Linux/macOS)
- Lightweight and fast

### Component Architecture
- Flexible entity composition
- Easy to add new behaviors
- Clean separation of concerns
- Memory efficient with smart pointers

### Deterministic Generation
- Reproducible gameplay
- Fair competitive play
- Easier debugging
- Shareable seeds

### C++20 Features
- Concepts for type constraints
- Ranges for algorithm clarity
- std::format for string handling
- constexpr improvements
- Modules (future consideration)

## Known Limitations

- Terminal resolution limits visual fidelity
- No sound support (terminal limitation)
- No multiplayer support
- Limited to keyboard input
- Windows console may have color limitations

## Future Roadmap

### Graphics
- Add optional SDL2/OpenGL rendering backend
- Sprite support
- Particle effects
- Screen transitions

### Audio
- SDL_mixer integration
- Sound effects
- Background music
- Dynamic audio based on gameplay

### Content
- More enemy types and bosses
- Additional time abilities
- More procedural algorithms
- Larger quest chains
- NPC dialogue trees

### Multiplayer
- Local co-op
- Online multiplayer
- Leaderboards
- Shared seeds

### Modding
- Lua scripting support
- Custom enemy definitions
- Mod loading system
- Workshop integration

## License

MIT License - See LICENSE file for details

## Contributing

Contributions welcome! Please ensure:
- Code follows existing style
- All tests pass
- New features include tests
- Documentation updated

## Credits

Developed as a complete C++ game engineering demonstration.
