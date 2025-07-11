#pragma once

#include "engine/levels/level.hpp"

#include <string>


namespace SHAME::Engine::Serialization{

    class LevelSerializer{
        public:
            static std::shared_ptr<Levels::Level> ImportLevel(const Filesystem::Path path);
    };
}