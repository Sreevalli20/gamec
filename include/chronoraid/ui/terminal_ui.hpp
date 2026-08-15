#pragma once

#include "../core/common.hpp"
#include "../core/vector2.hpp"
#include <string>
#include <vector>
#include <functional>

namespace chronoraid {

enum class Color {
    Black,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,
    BrightBlack,
    BrightRed,
    BrightGreen,
    BrightYellow,
    BrightBlue,
    BrightMagenta,
    BrightCyan,
    BrightWhite
};

class TerminalUI {
public:
    static TerminalUI& instance() {
        static TerminalUI ui;
        return ui;
    }

    void initialize();
    void shutdown();

    void clear();
    void refresh();

    void set_color(Color foreground, Color background = Color::Black);
    void reset_color();

    void move_cursor(i32 x, i32 y);
    void hide_cursor();
    void show_cursor();

    void print(const std::string& text);
    void print_at(i32 x, i32 y, const std::string& text);
    void print_colored(const std::string& text, Color color);

    void draw_box(i32 x, i32 y, i32 width, i32 height);
    void draw_horizontal_line(i32 x, i32 y, i32 length);
    void draw_vertical_line(i32 x, i32 y, i32 length);

    i32 get_width() const;
    i32 get_height() const;

    void set_title(const std::string& title);

    bool read_key(char& key);
    std::string read_line();

    void enable_raw_mode();
    void disable_raw_mode();

private:
    TerminalUI() = default;
    ~TerminalUI();

    bool is_initialized_ = false;
    bool raw_mode_enabled_ = false;
};

class Menu {
public:
    using Callback = std::function<void()>;

    Menu(const std::string& title);

    void add_option(const std::string& text, Callback callback);
    void clear_options();

    void render();
    void handle_input(char key);

    i32 get_selected_index() const { return selected_index_; }
    void set_selected_index(i32 index) { selected_index_ = index; }

private:
    std::string title_;
    std::vector<std::pair<std::string, Callback>> options_;
    i32 selected_index_ = 0;
};

class ProgressBar {
public:
    ProgressBar(i32 x, i32 y, i32 width, f32 value = 0.0f);

    void set_value(f32 value);
    void set_color(Color color);

    void render();

private:
    i32 x_, y_, width_;
    f32 value_;
    Color color_ = Color::Green;
};

} // namespace chronoraid
