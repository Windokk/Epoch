#pragma once

#include <chrono>
#include <mutex>
#include <thread>

namespace SHAME::Engine::Time{

    struct TimeStamp{
        int milliseconds;
        int seconds;
        int minutes;
        int hours;
        int days;
    };

    class TimeManager {
        public:
            static TimeManager& GetInstance() {
                static TimeManager instance;
                return instance;
            }

            void Init(float fixedStep = 1.0f / 60.0f, float maxAccumulated = 0.66f, float targetFPS = 60.0f){
                TimeManager::fixedDeltaTime = fixedStep;
                TimeManager::maxAccumulatedTime = maxAccumulated;
                TimeManager::targetFrameTime = 1 / targetFPS;
                accumulator = 0.0f;
                deltaTime = 0.0f;
                TimeManager::lastTime = std::chrono::high_resolution_clock::now();
            }

            void Tick() {
                frameStartTime = std::chrono::high_resolution_clock::now();
                auto delta = std::chrono::duration<float>(frameStartTime - lastTime);
                deltaTime = delta.count();
                lastTime = frameStartTime;

                accumulator += deltaTime;
                if (accumulator > maxAccumulatedTime)
                    accumulator = maxAccumulatedTime;
            }

            bool ShouldStepPhysics() {
                return accumulator >= fixedDeltaTime;
            }

            void ConsumeFixedStep() {
                accumulator -= fixedDeltaTime;
            }

            void FrameLimit() {
                auto now = std::chrono::high_resolution_clock::now();
                float frameDuration = std::chrono::duration<float>(now - frameStartTime).count();
                float sleepTime = targetFrameTime - frameDuration;
                if (sleepTime > 0.0f) {
                    std::this_thread::sleep_for(std::chrono::duration<float>(sleepTime));
                }
            }

            float GetFixedDeltaTime() { return fixedDeltaTime; }
            float GetInterpolationAlpha() { return accumulator / fixedDeltaTime; }
            float GetDeltaTime() { return deltaTime; }

            TimeStamp CurrentTime() {
                using namespace std::chrono;

                auto now = system_clock::now();
                auto now_time_t = system_clock::to_time_t(now);
                auto now_ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

                std::tm local_tm;
        #ifdef _WIN32
                localtime_s(&local_tm, &now_time_t);
        #else
                localtime_r(&now_time_t, &local_tm);
        #endif

                TimeStamp ts;
                ts.days = local_tm.tm_yday;
                ts.hours = local_tm.tm_hour;
                ts.minutes = local_tm.tm_min;
                ts.seconds = local_tm.tm_sec;
                ts.milliseconds = static_cast<int>(now_ms.count());

                return ts;
            }

        private:
            TimeManager() = default;
            ~TimeManager() = default;

            TimeManager(const TimeManager&) = delete;
            TimeManager& operator=(const TimeManager&) = delete;

            float fixedDeltaTime;
            float maxAccumulatedTime;
            float targetFrameTime;

            float deltaTime;
            float accumulator;

            std::chrono::high_resolution_clock::time_point lastTime;
            std::chrono::high_resolution_clock::time_point frameStartTime;
    };
}