#include "debugger.hpp"

#include <iomanip>
#include <ctime>

namespace SHAME::Engine::Debugging{
    
    Level Debugger::currentMinLevel = Level::Log;
    std::ofstream Debugger::logFile;

    void Debugger::EnableFileLogging(const std::string& filepath) {
        logFile.open(filepath, std::ios::out | std::ios::app);
    }

    void Debugger::SetMinimumLevel(Level level) {
        currentMinLevel = level;
    }

    std::string Debugger::LevelToString(Level level) {
        switch (level) {
            case Level::Log:    return "LOG";
            case Level::Info:   return "INFO";
            case Level::Warning:return "WARNING";
            case Level::Error:  return "ERROR";
            case Level::Fatal:  return "FATAL";
        }
        return "UNKNOWN";
    }

    std::string Debugger::GetTimestamp() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::tm tm{};
        localtime_s(&tm, &now_c);

        std::ostringstream oss;
        oss << std::put_time(&tm, "[%Y-%m-%d %H:%M:%S]");
        return oss.str();
    }

    void Debugger::Log(Level level, const std::string& message, const char* file, int line) {
        if (level < currentMinLevel)
            return;

        std::string output = GetTimestamp() + " [" + LevelToString(level) + "] (" + file + ":" + std::to_string(line) + ") " + message;

        std::cout << output << std::endl;

        if (logFile.is_open())
            logFile << output << std::endl;

        if (level == Level::Fatal)
            std::terminate(); // crash
    }
}