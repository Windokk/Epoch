#pragma once

#include "engine/rendering/utils.hpp"

#include <memory>

namespace EPOCH::Engine::Rendering {
    
    class Shader;

    class FrameBuffer{
    public:
        FrameBuffer(float width, float height, std::shared_ptr<Shader> shader, bool multisampled);
        void Shutdown();
        unsigned int GetFrameTexture();
        void RescaleFrameBuffer(float width, float height);
        void Resolve();
        void Draw(unsigned int VAO);
        void Bind() const;
        void Unbind() const;
        void SetShader(std::shared_ptr<Shader> shader);
        bool isMultisampled = false;
    private:
        unsigned int fbo;
        unsigned int texture;
        unsigned int rbo;
        std::shared_ptr<Shader> shader;

        GLuint resolveFBO = 0;
        GLuint resolveTexture = 0;

        float width, height = 0;
    };
}