#pragma once

#include <string>
#include <chrono>
#include <iostream>
#include <fstream>

namespace SHAME::Engine::Debugging{
    enum class Level {
        Log,
        Info,
        Warning,
        Error,
        Fatal
    };

    class Debugger {
    public:
        static void Log(Level level, const std::string& message, const char* file, int line);

        // Optional: Set output file, filters, etc.
        static void EnableFileLogging(const std::string& filepath);
        static void SetMinimumLevel(Level level);

    private:
        static std::string LevelToString(Level level);
        static std::string GetTimestamp();
        static Level currentMinLevel;
        static std::ofstream logFile;
    };
}

#define DEBUG_LOG(msg)       SHAME::Engine::Debugging::Debugger::Log(SHAME::Engine::Debugging::Level::Log,    msg, __FILE__, __LINE__)
#define DEBUG_INFO(msg)      SHAME::Engine::Debugging::Debugger::Log(SHAME::Engine::Debugging::Level::Info,   msg, __FILE__, __LINE__)
#define DEBUG_WARNING(msg)   SHAME::Engine::Debugging::Debugger::Log(SHAME::Engine::Debugging::Level::Warning,msg, __FILE__, __LINE__)
#define DEBUG_ERROR(msg)     SHAME::Engine::Debugging::Debugger::Log(SHAME::Engine::Debugging::Level::Error,  msg, __FILE__, __LINE__)
#define DEBUG_FATAL(msg)     SHAME::Engine::Debugging::Debugger::Log(SHAME::Engine::Debugging::Level::Fatal,  msg, __FILE__, __LINE__)