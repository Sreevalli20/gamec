#pragma once

#include "common.hpp"
#include <string>
#include <unordered_map>

namespace chronoraid {

class Config {
public:
    static Config& instance() {
        static Config config;
        return config;
    }

    void load(const std::string& filename);
    void save(const std::string& filename);

    void set(const std::string& key, const std::string& value);
    void set_int(const std::string& key, i32 value);
    void set_float(const std::string& key, f32 value);
    void set_bool(const std::string& key, bool value);

    std::string get(const std::string& key, const std::string& default_value = "") const;
    i32 get_int(const std::string& key, i32 default_value = 0) const;
    f32 get_float(const std::string& key, f32 default_value = 0.0f) const;
    bool get_bool(const std::string& key, bool default_value = false) const;

    bool has(const std::string& key) const;

private:
    Config() = default;

    std::unordered_map<std::string, std::string> values_;
};

} // namespace chronoraid
