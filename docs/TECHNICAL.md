# ChronoRaid Technical Documentation

## System Architecture

### Game Loop
The game follows a standard game loop pattern:
1. **Process Input**: Handle keyboard input via InputManager
2. **Update**: Update all systems with delta time
3. **Render**: Draw the game state to terminal

Delta time is calculated using high-resolution timers for consistent gameplay across different frame rates.

### Entity-Component System

#### Design Philosophy
- Composition over inheritance
- Data-oriented design where practical
- Minimal overhead with smart pointers
- Type-safe component access

#### Component Types
- **TransformComponent**: Position, scale, rotation
- **HealthComponent**: Current/max health, death state
- **MovementComponent**: Velocity, speed, acceleration, friction
- **CombatComponent**: Attack power, speed, cooldowns, critical stats
- **ExperienceComponent**: XP, level, progression
- **CollisionComponent**: Bounding box, collision layers/masks
- **StatusEffectComponent**: Active status effects with durations

#### Entity Lifecycle
1. Created via EntityManager::create_entity()
2. Components added via add_component<T>()
3. Updated each frame via update(delta_time)
4. Destroyed via destroy() - marked for cleanup
5. Removed from EntityManager on next update cycle

### Combat System

#### Damage Calculation
```
final_damage = base_damage * (1 - armor / (armor + 100))
```

#### Critical Hits
```
if (random() < critical_chance):
    damage *= critical_multiplier
```

#### Attack Flow
1. Check cooldown via can_attack()
2. Call attack() to reset cooldown
3. Calculate damage via calculate_damage()
4. Apply via CombatSystem::perform_attack()
5. Target receives damage via HealthComponent::take_damage()
6. Death callbacks triggered if health <= 0

### AI System

#### State Machine
Each AI controller implements a state machine:
- **Idle**: No target, waiting
- **Patrol**: Moving between waypoints
- **Chase**: Moving toward target
- **Attack**: In range, attacking
- **Retreat**: Moving away from target
- **Investigate**: Checking last known position
- **Dead**: Entity destroyed

#### AI Controllers
- **ChaseAI**: Simple chase behavior with attack range
- **PatrolAI**: Waypoint-based patrol with wait times
- **RangedAI**: Maintains distance, strafes, kites
- **TankAI**: Charge attacks, high durability

### Procedural Generation

#### Algorithms

##### Room Placement
1. Attempt to place rooms randomly
2. Check for overlaps with existing rooms
3. Connect rooms with L-shaped corridors
4. Place spawn points in rooms

##### Random Walk
1. Start at center of map
2. Take random steps for N iterations
3. Carve floor tiles at each step
4. Clamp to map boundaries

##### Cellular Automata
1. Randomly initialize grid (45% walls)
2. Apply smoothing rules for N iterations
3. Rule: If >4 neighbors are walls, become wall
4. Rule: If <3 neighbors are walls, become floor

##### Binary Space Partitioning (BSP)
1. Start with entire map as one node
2. Recursively split nodes
3. Extract rooms from leaf nodes
4. Connect rooms with corridors

##### Drunkard's Walk
1. Multiple walkers start at center
2. Each walker takes random steps
3. Carve floors at walker positions
4. Creates organic cave-like structures

#### Deterministic Seeding
- Random instance seeded with u32
- Same seed produces identical output
- Enables reproducible gameplay and testing

### Time Mechanics

#### Time Abilities

##### Rewind
- Records entity state snapshots (position, velocity, health)
- Stores up to 300 snapshots (5 seconds at 60 FPS)
- On use: restores state from 2 seconds ago
- Also heals player for balance

##### Time Freeze
- Sets global time scale for enemies
- Enemies update at reduced rate
- Player moves normally
- Duration: 3 seconds

##### Temporal Dash
- Instant movement in facing direction
- High velocity for brief period
- 5 unit distance
- Invulnerable during dash

##### Echo
- Creates temporary clone at previous position
- Clone attacks for duration
- Useful for distraction

##### Timeline Shift
- Reduces global time scale to 0.5x
- Affects all entities including player
- Strategic slowdown
- Duration: 5 seconds

#### Time Recorder
- Records snapshots every frame
- Circular buffer with max capacity
- Timestamped for precise rewinding
- Per-entity recording

### Inventory System

#### Item Structure
- Unique ID for item type
- Name and description
- Type (Weapon, Armor, Consumable, etc.)
- Rarity (Common to Mythic)
- Stats (attack, defense, health, etc.)
- Max stack size
- Value (currency)

#### Rarity System
- **Common**: 60% base chance
- **Uncommon**: 25% base chance
- **Rare**: 10% base chance
- **Epic**: 4% base chance
- **Legendary**: 0.9% base chance
- **Mythic**: 0.1% base chance
- Level bonus: +0.2% per level

#### Loot Generation
- Based on player level
- Area difficulty modifier
- Boss multipliers
- Deterministic with seed

### Save/Load System

#### Save Format
- Plain text key-value pairs
- Versioned format for compatibility
- Includes: player state, inventory, quests, world seed, achievements, settings, statistics

#### Backup System
- Automatic backup on save
- Separate .bak files
- Restore on corruption
- Manual restore available

#### Error Handling
- Missing save: Return false, log error
- Corrupted save: Attempt backup, log error
- Version mismatch: Log warning, attempt load
- Invalid data: Skip field, continue

### Achievement System

#### Achievement Types
- **KillCount**: Track enemy kills
- **BossDefeated**: Track boss victories
- **ItemCollected**: Track item acquisitions
- **LevelMilestone**: Track level progression
- **TimeBased**: Time-based challenges
- **ScoreBased**: Score thresholds
- **Special**: Unique conditions

#### Progress Tracking
- Current value vs target value
- Auto-unlock when target reached
- Callback support for custom logic
- Hidden achievements for secrets

### Terminal UI

#### Rendering
- ANSI escape codes for colors (Unix)
- Console API for colors (Windows)
- Cursor positioning for UI elements
- Double buffering via clear/redraw

#### Input Handling
- Raw mode for immediate key input
- Non-blocking key reads
- Special key detection (arrows, escape)
- Cross-platform compatibility

#### UI Elements
- Menu system with selection
- Progress bars
- Text boxes/borders
- Colored text output

## Performance Considerations

### Memory Management
- Smart pointers (unique_ptr, shared_ptr) for automatic cleanup
- Object pooling for frequently created/destroyed entities
- Circular buffers for time recording
- Reserve capacity for vectors where size known

### Algorithmic Efficiency
- Spatial hashing for collision (future)
- Entity-component queries optimized
- Minimal allocations in game loop
- Cache-friendly data layout

### Rendering Optimization
- Only redraw changed tiles (future)
- Dirty rectangle tracking (future)
- Minimize string allocations
- Batch terminal operations

## Testing Strategy

### Unit Tests
- Combat calculations (damage, critical, armor)
- Inventory operations (add, remove, stack)
- Procedural generation (determinism, algorithms)
- Progression (XP, leveling, skills)
- Save/load (serialization, file I/O)
- Achievements (unlocking, progress)
- Quests (objectives, states, manager)

### Integration Tests
- Full combat flow
- AI behavior scenarios
- Boss phase transitions
- Time ability interactions
- Save/load round-trip

### Performance Tests
- Entity count benchmarks
- Generation algorithm timing
- Memory usage profiling
- Frame rate monitoring

## Build System

### CMake Configuration
- C++20 standard required
- Warnings as errors for strict compilation
- Separate build directories
- Test discovery and execution

### Platform-Specific
- Windows: MSVC 2022 with Visual Studio generator
- Linux: GCC 11+ with Unix Makefiles
- macOS: Clang 13+ with Unix Makefiles

## Debugging

### Logging System
- Log levels: Debug, Info, Warning, Error, Fatal
- Configurable log level
- File output (future)
- Timestamped entries

### Debug Commands (Future)
- God mode
- Spawn entities
- Teleport
- Toggle collision
- Display FPS
- Memory stats

## Security Considerations

### Save File Validation
- Version checking
- Data type validation
- Range checking
- Sanitization of user input

### Cheat Prevention (Future)
- Save file checksums
- Server validation for online
- Memory protection

## Future Enhancements

### Graphics Backend
- Abstract rendering interface
- SDL2 implementation
- OpenGL implementation
- Runtime backend selection

### Scripting Integration
- Lua for game logic
- Expose C++ APIs
- Mod support
- Custom enemies/items

### Networking
- Replicated entity system
- Client-server architecture
- State synchronization
- Lag compensation
