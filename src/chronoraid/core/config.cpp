#include <chronoraid/core/config.hpp>
#include <fstream>
#include <sstream>

namespace chronoraid {

void Config::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        LOG_WARNING("Could not open config file: " + filename);
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            values_[key] = value;
        }
    }

    LOG_INFO("Config loaded from: " + filename);
}

void Config::save(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        LOG_ERROR("Could not save config file: " + filename);
        return;
    }

    for (const auto& [key, value] : values_) {
        file << key << " = " << value << "\n";
    }

    LOG_INFO("Config saved to: " + filename);
}

void Config::set(const std::string& key, const std::string& value) {
    values_[key] = value;
}

void Config::set_int(const std::string& key, i32 value) {
    values_[key] = std::to_string(value);
}

void Config::set_float(const std::string& key, f32 value) {
    values_[key] = std::to_string(value);
}

void Config::set_bool(const std::string& key, bool value) {
    values_[key] = value ? "true" : "false";
}

std::string Config::get(const std::string& key, const std::string& default_value) const {
    auto it = values_.find(key);
    if (it != values_.end()) {
        return it->second;
    }
    return default_value;
}

i32 Config::get_int(const std::string& key, i32 default_value) const {
    auto it = values_.find(key);
    if (it != values_.end()) {
        try {
            return std::stoi(it->second);
        } catch (...) {
            return default_value;
        }
    }
    return default_value;
}

f32 Config::get_float(const std::string& key, f32 default_value) const {
    auto it = values_.find(key);
    if (it != values_.end()) {
        try {
            return std::stof(it->second);
        } catch (...) {
            return default_value;
        }
    }
    return default_value;
}

bool Config::get_bool(const std::string& key, bool default_value) const {
    auto it = values_.find(key);
    if (it != values_.end()) {
        std::string value = it->second;
        for (char& c : value) c = std::tolower(c);
        return value == "true" || value == "1" || value == "yes";
    }
    return default_value;
}

bool Config::has(const std::string& key) const {
    return values_.find(key) != values_.end();
}

} // namespace chronoraid
