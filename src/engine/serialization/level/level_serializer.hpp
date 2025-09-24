#pragma once

#include "engine/levels/level.hpp"

#include <string>


namespace EPOCH::Engine::Serialization{

    std::shared_ptr<Levels::Level> ImportLevel(const Filesystem::Path path);
}