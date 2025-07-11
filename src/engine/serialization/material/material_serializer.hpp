#pragma once

#include "engine/rendering/material/material.hpp"

#include "engine/filesystem/filesystem.hpp"

namespace SHAME::Engine::Serialization{

    class MaterialSerializer{
        public:
            static std::shared_ptr<Rendering::Material> ImportMaterial(const Filesystem::Path path);
    };

}