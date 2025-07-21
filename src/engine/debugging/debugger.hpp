#pragma once

#include <string>
#include <chrono>
#include <iostream>
#include <fstream>

namespace SHAME::Engine::Debugging{
    enum class Level {
        Log = 0,
        Info = 1,
        Warning = 2,
        Error = 3,
        Fatal = 4
    };

    class Debugger {
    public:
        static void Log(Level level, const std::string& message, const char* file, int line);

        static void EnableTimestamp();
        static void EnableFileLogging(const std::string& filepath);
        static void SetMinimumLevel(Level level);

    private:
        static std::string LevelToString(Level level);
        static std::string GetTimestamp();
        static Level currentMinLevel;
        static std::ofstream logFile;

        static bool useTimestamp;
    };
}

#define DEBUG_LOG(msg)       SHAME::Engine::Debugging::Debugger::Log(SHAME::Engine::Debugging::Level::Log,    msg, __FILE__, __LINE__)
#define DEBUG_INFO(msg)      SHAME::Engine::Debugging::Debugger::Log(SHAME::Engine::Debugging::Level::Info,   msg, __FILE__, __LINE__)
#define DEBUG_WARNING(msg)   SHAME::Engine::Debugging::Debugger::Log(SHAME::Engine::Debugging::Level::Warning,msg, __FILE__, __LINE__)
#define DEBUG_ERROR(msg)     SHAME::Engine::Debugging::Debugger::Log(SHAME::Engine::Debugging::Level::Error,  msg, __FILE__, __LINE__)
#define DEBUG_FATAL(msg)     SHAME::Engine::Debugging::Debugger::Log(SHAME::Engine::Debugging::Level::Fatal,  msg, __FILE__, __LINE__)