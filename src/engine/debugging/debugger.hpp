#pragma once

#include <string>
#include <chrono>
#include <iostream>
#include <fstream>

namespace EPOCH::Engine::Debugging{
    enum class Level {
        Log = 0,
        Info = 1,
        Warning = 2,
        Error = 3,
        Fatal = 4
    };

    class Debugger {
    public:
        static Debugger& GetInstance() {
            static Debugger instance;
            return instance;
        }

        void Log(Level level, const std::string& message, const char* file, int line);

        void EnableTimestamp();
        void EnableFileLogging(const std::string& filepath);
        void SetMinimumLevel(Level level);

    private:
        std::string LevelToString(Level level);
        std::string GetTimestamp();
        Level currentMinLevel;
        std::ofstream logFile;

        bool useTimestamp;

        Debugger() = default;
        ~Debugger() = default;
        Debugger(const Debugger&) = delete;
        Debugger& operator=(const Debugger&) = delete;
    };

#if defined(BUILD_ENGINE)

    // Used by the EXE/engine
    inline Debugger& GetDebugger() {
        return Debugger::GetInstance();
    }

#elif defined(BUILD_GAME)

    // Used by the Game Module DLL
    inline Debugger* gSharedDebuggerPtr = nullptr;

    inline void SetDebugger(Debugger* ptr) {
        gSharedDebuggerPtr = ptr;
    }

    inline Debugger& GetDebugger() {
        if (!gSharedDebuggerPtr)
            exit(2);
        return *gSharedDebuggerPtr;
    }

#elif defined(BUILD_EDITOR)

    // Used by the Editor Module DLL
    inline Debugger* gSharedDebuggerPtr = nullptr;

    inline void SetDebugger(Debugger* ptr) {
        gSharedDebuggerPtr = ptr;
    }

    inline Debugger& GetDebugger() {
        if (!gSharedDebuggerPtr)
            exit(2);
        return *gSharedDebuggerPtr;
    }

#endif

}

#define DEBUG_LOG(msg)       EPOCH::Engine::Debugging::GetDebugger().Log(EPOCH::Engine::Debugging::Level::Log,    msg, __FILE__, __LINE__)
#define DEBUG_INFO(msg)      EPOCH::Engine::Debugging::GetDebugger().Log(EPOCH::Engine::Debugging::Level::Info,   msg, __FILE__, __LINE__)
#define DEBUG_WARNING(msg)   EPOCH::Engine::Debugging::GetDebugger().Log(EPOCH::Engine::Debugging::Level::Warning,msg, __FILE__, __LINE__)
#define DEBUG_ERROR(msg)     EPOCH::Engine::Debugging::GetDebugger().Log(EPOCH::Engine::Debugging::Level::Error,  msg, __FILE__, __LINE__)
#define DEBUG_FATAL(msg)     EPOCH::Engine::Debugging::GetDebugger().Log(EPOCH::Engine::Debugging::Level::Fatal,  msg, __FILE__, __LINE__)