#pragma once

#include "engine/levels/level.hpp"

#include <string>


namespace SHAME::Engine::Serialization{

    std::shared_ptr<Levels::Level> ImportLevel(const Filesystem::Path path);
}