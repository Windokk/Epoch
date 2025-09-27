#include "debugger.hpp"

#include <iomanip>
#include <ctime>

namespace EPOCH::Engine::Debugging{

    void Debugger::EnableTimestamp()
    {
        useTimestamp = true;
    }

    void Debugger::EnableFileLogging(const std::string &filepath)
    {
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

        std::string output = (useTimestamp ? GetTimestamp()+" " : "") + "[" + LevelToString(level) + "] (" + file + ":" + std::to_string(line) + ") " + message;

        std::cout << output << std::endl;

        if (logFile.is_open())
            logFile << output << std::endl;

        if (level == Level::Fatal){
            std::cout << "EPOCH Engine has crashed. Press Enter to exit..." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
            std::terminate(); // crash
        }
            
    }
}