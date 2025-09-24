#pragma once

#include "engine/rendering/utils.hpp"

#include <memory>

namespace EPOCH::Engine::Rendering {
    
    class Shader;

    class FrameBuffer{
    public:
        FrameBuffer(float width, float height, std::shared_ptr<Shader> shader);
        void Shutdown();
        unsigned int GetFrameTexture();
        void RescaleFrameBuffer(float width, float height);
        void Draw(unsigned int VAO);
        void Bind() const;
        void Unbind() const;
        void SetShader(std::shared_ptr<Shader> shader);
    private:
        unsigned int fbo;
        unsigned int texture;
        unsigned int rbo;
        std::shared_ptr<Shader> shader;
    };
}