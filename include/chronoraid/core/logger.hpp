#pragma once

#include "common.hpp"
#include <string>
#include <sstream>
#include <iostream>

namespace chronoraid {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Fatal
};

class Logger {
public:
    static Logger& instance() {
        static Logger logger;
        return logger;
    }

    void set_level(LogLevel level) {
        min_level_ = level;
    }

    void log(LogLevel level, const std::string& message) {
        if (level < min_level_) return;
        
        std::ostream& stream = (level >= LogLevel::Error) ? std::cerr : std::cout;
        stream << "[" << level_to_string(level) << "] " << message << std::endl;
    }

    void debug(const std::string& message) { log(LogLevel::Debug, message); }
    void info(const std::string& message) { log(LogLevel::Info, message); }
    void warning(const std::string& message) { log(LogLevel::Warning, message); }
    void error(const std::string& message) { log(LogLevel::Error, message); }
    void fatal(const std::string& message) { log(LogLevel::Fatal, message); }

private:
    Logger() : min_level_(LogLevel::Info) {}

    static const char* level_to_string(LogLevel level) {
        switch (level) {
            case LogLevel::Debug: return "DEBUG";
            case LogLevel::Info: return "INFO";
            case LogLevel::Warning: return "WARN";
            case LogLevel::Error: return "ERROR";
            case LogLevel::Fatal: return "FATAL";
            default: return "UNKNOWN";
        }
    }

    LogLevel min_level_;
};

#define LOG_DEBUG(msg) chronoraid::Logger::instance().debug(msg)
#define LOG_INFO(msg) chronoraid::Logger::instance().info(msg)
#define LOG_WARNING(msg) chronoraid::Logger::instance().warning(msg)
#define LOG_ERROR(msg) chronoraid::Logger::instance().error(msg)
#define LOG_FATAL(msg) chronoraid::Logger::instance().fatal(msg)

} // namespace chronoraid
