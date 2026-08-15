#pragma once

#include "common.hpp"
#include "vector2.hpp"
#include <unordered_map>
#include <functional>

namespace chronoraid {

enum class KeyCode {
    Unknown,
    Up, Down, Left, Right,
    W, A, S, D,
    Space, Enter, Escape,
    Tab, Shift, Ctrl,
    One, Two, Three, Four, Five,
    Q, E, R, F,
    I, P, M,
    Count
};

enum class MouseButton {
    Left,
    Right,
    Middle,
    Count
};

enum class KeyState {
    Released,
    Pressed,
    Held
};

class InputManager {
public:
    static InputManager& instance() {
        static InputManager manager;
        return manager;
    }

    void update() {
        for (auto& [key, state] : key_states_) {
            if (state == KeyState::Pressed) {
                state = KeyState::Held;
            }
        }
        for (auto& [button, state] : mouse_states_) {
            if (state == KeyState::Pressed) {
                state = KeyState::Held;
            }
        }
    }

    void set_key_state(KeyCode key, KeyState state) {
        key_states_[key] = state;
    }

    void set_mouse_state(MouseButton button, KeyState state) {
        mouse_states_[button] = state;
    }

    void set_mouse_position(const Vector2& pos) {
        mouse_position_ = pos;
    }

    bool is_key_pressed(KeyCode key) const {
        auto it = key_states_.find(key);
        return it != key_states_.end() && it->second == KeyState::Pressed;
    }

    bool is_key_held(KeyCode key) const {
        auto it = key_states_.find(key);
        return it != key_states_.end() && (it->second == KeyState::Pressed || it->second == KeyState::Held);
    }

    bool is_key_released(KeyCode key) const {
        auto it = key_states_.find(key);
        return it != key_states_.end() && it->second == KeyState::Released;
    }

    bool is_mouse_pressed(MouseButton button) const {
        auto it = mouse_states_.find(button);
        return it != mouse_states_.end() && it->second == KeyState::Pressed;
    }

    bool is_mouse_held(MouseButton button) const {
        auto it = mouse_states_.find(button);
        return it != mouse_states_.end() && (it->second == KeyState::Pressed || it->second == KeyState::Held);
    }

    Vector2 get_mouse_position() const {
        return mouse_position_;
    }

private:
    InputManager() = default;

    std::unordered_map<KeyCode, KeyState> key_states_;
    std::unordered_map<MouseButton, KeyState> mouse_states_;
    Vector2 mouse_position_;
};

} // namespace chronoraid
