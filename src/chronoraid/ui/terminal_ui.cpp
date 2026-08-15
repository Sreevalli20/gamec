#include <chronoraid/ui/terminal_ui.hpp>
#include <chronoraid/core/logger.hpp>
#include <iostream>
#include <chrono>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

namespace chronoraid {

TerminalUI::TerminalUI() = default;
TerminalUI::~TerminalUI() {
    disable_raw_mode();
}

void TerminalUI::initialize() {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hConsole, &mode);
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hConsole, mode);
#endif
    
    is_initialized_ = true;
    LOG_INFO("Terminal UI initialized");
}

void TerminalUI::shutdown() {
    disable_raw_mode();
    show_cursor();
    reset_color();
    is_initialized_ = false;
    LOG_INFO("Terminal UI shutdown");
}

void TerminalUI::clear() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void TerminalUI::refresh() {
    std::cout.flush();
}

void TerminalUI::set_color(Color foreground, Color background) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    int color_code = static_cast<int>(foreground);
    if (static_cast<int>(foreground) >= 8) {
        color_code = (color_code - 8) | FOREGROUND_INTENSITY;
    }
    
    SetConsoleTextAttribute(hConsole, color_code);
#else
    int fg = static_cast<int>(foreground) % 8;
    int bg = static_cast<int>(background) % 8;
    
    std::cout << "\033[" << (fg + 30) << ";" << (bg + 40) << "m";
#endif
}

void TerminalUI::reset_color() {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#else
    std::cout << "\033[0m";
#endif
}

void TerminalUI::move_cursor(i32 x, i32 y) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {static_cast<SHORT>(x), static_cast<SHORT>(y)};
    SetConsoleCursorPosition(hConsole, pos);
#else
    std::cout << "\033[" << (y + 1) << ";" << (x + 1) << "H";
#endif
}

void TerminalUI::hide_cursor() {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
#else
    std::cout << "\033[?25l";
#endif
}

void TerminalUI::show_cursor() {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
#else
    std::cout << "\033[?25h";
#endif
}

void TerminalUI::print(const std::string& text) {
    std::cout << text;
}

void TerminalUI::print_at(i32 x, i32 y, const std::string& text) {
    move_cursor(x, y);
    print(text);
}

void TerminalUI::print_colored(const std::string& text, Color color) {
    set_color(color);
    print(text);
    reset_color();
}

void TerminalUI::draw_box(i32 x, i32 y, i32 width, i32 height) {
    draw_horizontal_line(x, y, width);
    draw_horizontal_line(x, y + height - 1, width);
    draw_vertical_line(x, y, height);
    draw_vertical_line(x + width - 1, y, height);
}

void TerminalUI::draw_horizontal_line(i32 x, i32 y, i32 length) {
    move_cursor(x, y);
    for (i32 i = 0; i < length; ++i) {
        std::cout << "-";
    }
}

void TerminalUI::draw_vertical_line(i32 x, i32 y, i32 length) {
    for (i32 i = 0; i < length; ++i) {
        move_cursor(x, y + i);
        std::cout << "|";
    }
}

i32 TerminalUI::get_width() const {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
#else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
#endif
}

i32 TerminalUI::get_height() const {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_row;
#endif
}

void TerminalUI::set_title(const std::string& title) {
#ifdef _WIN32
    SetConsoleTitleA(title.c_str());
#else
    std::cout << "\033]0;" << title << "\007";
#endif
}

bool TerminalUI::read_key(char& key) {
#ifdef _WIN32
    if (_kbhit()) {
        key = _getch();
        return true;
    }
    return false;
#else
    if (raw_mode_enabled_) {
        char buf = 0;
        if (read(STDIN_FILENO, &buf, 1) > 0) {
            key = buf;
            return true;
        }
    }
    return false;
#endif
}

std::string TerminalUI::read_line() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

void TerminalUI::enable_raw_mode() {
#ifdef _WIN32
    
#else
    termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
    raw_mode_enabled_ = true;
#endif
}

void TerminalUI::disable_raw_mode() {
#ifdef _WIN32
    
#else
    if (raw_mode_enabled_) {
        termios term;
        tcgetattr(STDIN_FILENO, &term);
        term.c_lflag |= ICANON | ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &term);
        raw_mode_enabled_ = false;
    }
#endif
}

Menu::Menu(const std::string& title) : title_(title), selected_index_(0) {
}

void Menu::add_option(const std::string& text, Callback callback) {
    options_.emplace_back(text, callback);
}

void Menu::clear_options() {
    options_.clear();
    selected_index_ = 0;
}

void Menu::render() {
    TerminalUI::instance().set_color(Color::Cyan);
    TerminalUI::instance().print(title_);
    TerminalUI::instance().reset_color();
    TerminalUI::instance().print("\n");
    
    for (size_t i = 0; i < options_.size(); ++i) {
        if (i == static_cast<size_t>(selected_index_)) {
            TerminalUI::instance().set_color(Color::Yellow);
            TerminalUI::instance().print("> ");
        } else {
            TerminalUI::instance().print("  ");
        }
        
        TerminalUI::instance().print(options_[i].first);
        TerminalUI::instance().reset_color();
        TerminalUI::instance().print("\n");
    }
}

void Menu::handle_input(char key) {
    switch (key) {
        case 'w':
        case 'W':
        case 72:
            selected_index_ = (selected_index_ > 0) ? selected_index_ - 1 : static_cast<i32>(options_.size()) - 1;
            break;
        case 's':
        case 'S':
        case 80:
            selected_index_ = (selected_index_ < static_cast<i32>(options_.size()) - 1) ? selected_index_ + 1 : 0;
            break;
        case 13:
        case ' ':
            if (selected_index_ >= 0 && selected_index_ < static_cast<i32>(options_.size())) {
                if (options_[selected_index_].second) {
                    options_[selected_index_].second();
                }
            }
            break;
    }
}

ProgressBar::ProgressBar(i32 x, i32 y, i32 width, f32 value)
    : x_(x), y_(y), width_(width), value_(clamp(value, 0.0f, 1.0f)) {
}

void ProgressBar::set_value(f32 value) {
    value_ = clamp(value, 0.0f, 1.0f);
}

void ProgressBar::set_color(Color color) {
    color_ = color;
}

void ProgressBar::render() {
    i32 filled = static_cast<i32>(value_ * width_);
    
    TerminalUI::instance().move_cursor(x_, y_);
    TerminalUI::instance().print("[");
    
    TerminalUI::instance().set_color(color_);
    for (i32 i = 0; i < filled; ++i) {
        TerminalUI::instance().print("=");
    }
    TerminalUI::instance().reset_color();
    
    for (i32 i = filled; i < width_; ++i) {
        TerminalUI::instance().print(" ");
    }
    
    TerminalUI::instance().print("]");
}

} // namespace chronoraid
