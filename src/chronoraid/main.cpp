#include <chronoraid/core/game.hpp>
#include <chronoraid/core/logger.hpp>
#include <chronoraid/gameplay/player.hpp>
#include <chronoraid/gameplay/enemy.hpp>
#include <chronoraid/world/world.hpp>
#include <chronoraid/procedural/generator.hpp>
#include <chronoraid/inventory/inventory.hpp>
#include <chronoraid/inventory/loot_generator.hpp>
#include <chronoraid/quests/quest.hpp>
#include <chronoraid/progression/progression.hpp>
#include <chronoraid/save/save_system.hpp>
#include <chronoraid/achievements/achievement.hpp>
#include <chronoraid/ui/terminal_ui.hpp>
#include <chronoraid/gameplay/game_modes.hpp>
#include <chronoraid/statistics/statistics.hpp>
#include <iostream>
#include <memory>

using namespace chronoraid;

class MainMenuState : public GameState {
public:
    void on_enter() override {
        LOG_INFO("Entering Main Menu");
    }

    void process_input() override {
        char key;
        if (TerminalUI::instance().read_key(key)) {
            switch (key) {
                case '1':
                    Game::instance().push_state<GameState>();
                    break;
                case '2':
                    LOG_INFO("Continue Game selected");
                    break;
                case '3':
                    LOG_INFO("Settings selected");
                    break;
                case '4':
                case 'q':
                case 27:
                    Game::instance().quit();
                    break;
            }
        }
    }

    void update(f64 delta_time) override {
    }

    void render() override {
        TerminalUI::instance().clear();
        TerminalUI::instance().set_color(Color::Cyan);
        TerminalUI::instance().print_at(2, 2, "========================================");
        TerminalUI::instance().print_at(2, 3, "     CHRONO RAID: THE LAST TIMELINE     ");
        TerminalUI::instance().print_at(2, 4, "========================================");
        TerminalUI::instance().reset_color();
        
        TerminalUI::instance().print_at(2, 7, "1. New Game");
        TerminalUI::instance().print_at(2, 8, "2. Continue Game");
        TerminalUI::instance().print_at(2, 9, "3. Settings");
        TerminalUI::instance().print_at(2, 10, "4. Quit");
        
        TerminalUI::instance().print_at(2, 13, "Press 1-4 to select");
        TerminalUI::instance().refresh();
    }
};

class GameplayState : public GameState {
private:
    World world;

public:
    void on_enter() override {
        LOG_INFO("Entering Gameplay");
        
        world.initialize(80, 25);
        
        ProceduralGenerator generator;
        generator.set_seed(481928);
        generator.set_algorithm(GenerationAlgorithm::RoomPlacement);
        generator.set_map_size(80, 25);
        generator.set_room_count(8);
        world = generator.generate();
        
        EntityID player_id = EntityManager::instance().create_entity();
        auto player = EntityManager::instance().get_entity(player_id);
        player->set_name("Player");
        player->add_component<TransformComponent>();
        player->add_component<HealthComponent>(100.0f);
        player->add_component<MovementComponent>(150.0f);
        player->add_component<CombatComponent>(15.0f);
        player->add_component<ExperienceComponent>();
        
        auto transform = player->get_component<TransformComponent>();
        if (transform) {
            transform->position = world.get_spawn_point();
        }
        
        PlayerController::instance().set_player(player);
        
        GameSettings::instance().set_difficulty(Difficulty::Normal);
        ScoreSystem::instance().reset();
        Statistics::instance().reset();
        
        TerminalUI::instance().enable_raw_mode();
    }

    void on_exit() override {
        LOG_INFO("Exiting Gameplay");
        TerminalUI::instance().disable_raw_mode();
        TerminalUI::instance().show_cursor();
    }

    void process_input() override {
        PlayerController::instance().process_input();
        
        char key;
        if (TerminalUI::instance().read_key(key)) {
            if (key == 27) {
                Game::instance().push_state<MainMenuState>();
            }
        }
    }

    void update(f64 delta_time) override {
        PlayerController::instance().update(delta_time);
        EntityManager::instance().update(delta_time);
        TimeManager::instance().update(delta_time);
        CombatSystem::instance().update(delta_time);
        AISystem::instance().update(delta_time);
        ScoreSystem::instance().update(delta_time);
        Statistics::instance().record_play_time(delta_time);
        AutoSave::instance().update(delta_time);
    }

    void render() override {
        TerminalUI::instance().clear();
        
        const auto& tilemap = world.get_tile_map();
        
        for (u32 y = 0; y < tilemap.get_height(); ++y) {
            for (u32 x = 0; x < tilemap.get_width(); ++x) {
                const auto& tile = tilemap.get_tile(x, y);
                char ch = ' ';
                Color color = Color::White;
                
                switch (tile.type) {
                    case TileType::Wall:
                        ch = '#';
                        color = Color::BrightBlack;
                        break;
                    case TileType::Floor:
                        ch = '.';
                        color = Color::Black;
                        break;
                    case TileType::Door:
                        ch = '+';
                        color = Color::Yellow;
                        break;
                    case TileType::Hazard:
                        ch = '^';
                        color = Color::Red;
                        break;
                    case TileType::Exit:
                        ch = '>';
                        color = Color::Green;
                        break;
                    default:
                        ch = ' ';
                        break;
                }
                
                TerminalUI::instance().set_color(color);
                TerminalUI::instance().print_at(static_cast<i32>(x), static_cast<i32>(y), std::string(1, ch));
            }
        }
        
        auto player = PlayerController::instance().get_player();
        if (player) {
            auto transform = player->get_component<TransformComponent>();
            if (transform) {
                auto pos = transform->position;
                TerminalUI::instance().set_color(Color::Cyan);
                TerminalUI::instance().print_at(static_cast<i32>(pos.x), static_cast<i32>(pos.y), "@");
            }
        }
        
        TerminalUI::instance().reset_color();
        
        player = PlayerController::instance().get_player();
        if (player) {
            auto health = player->get_component<HealthComponent>();
            auto exp = player->get_component<ExperienceComponent>();
            
            if (health) {
                TerminalUI::instance().print_at(0, 25, "HP: ");
                TerminalUI::instance().set_color(Color::Red);
                TerminalUI::instance().print(std::to_string(static_cast<i32>(health->current_health)));
                TerminalUI::instance().print("/");
                TerminalUI::instance().print(std::to_string(static_cast<i32>(health->max_health)));
                TerminalUI::instance().reset_color();
            }
            
            if (exp) {
                TerminalUI::instance().print("  LVL: ");
                TerminalUI::instance().set_color(Color::Yellow);
                TerminalUI::instance().print(std::to_string(exp->level));
                TerminalUI::instance().reset_color();
            }
            
            TerminalUI::instance().print("  Score: ");
            TerminalUI::instance().set_color(Color::Green);
            TerminalUI::instance().print(std::to_string(ScoreSystem::instance().get_score()));
            TerminalUI::instance().reset_color();
        }
        
        TerminalUI::instance().print_at(0, 26, "WASD: Move | SPACE: Attack | ESC: Menu");
        
        TerminalUI::instance().refresh();
    }
};

int main() {
    Logger::instance().set_level(LogLevel::Info);
    
    LOG_INFO("ChronoRaid: The Last Timeline starting...");
    
    if (!Game::instance().initialize()) {
        LOG_FATAL("Failed to initialize game");
        return 1;
    }
    
    Game::instance().push_state<MainMenuState>();
    
    Game::instance().run();
    
    Game::instance().shutdown();
    
    LOG_INFO("ChronoRaid shutdown complete");
    
    return 0;
}
