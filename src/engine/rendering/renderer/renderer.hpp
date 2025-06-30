#pragma once

#include "engine/rendering/material/material.hpp"
#include "engine/rendering/framebuffer/framebuffer.hpp"
#include "engine/rendering/camera/camera.hpp"
#include "engine/rendering/light/light_manager.hpp"
#include "engine/ecs/components/model_component.hpp"
#include "engine/rendering/debug/debug.hpp"

namespace SHAME::Engine{

    namespace ECS::Components{
        class Transform;
    }

    namespace Rendering {

        enum class RenderStage {
            Background,
            Scene,
            PostProcess,
            Debug,
            UI
        };

        enum class BlendMode{
            Add,
            Multiply,
            Screen,
            Normal
        };

        struct RenderPass {
            RenderStage stage;
            std::function<void()> callback;
            std::shared_ptr<FrameBuffer> target = nullptr;
            bool appendToViewport = true;
            BlendMode blendMode = BlendMode::Normal;
        };

        struct DrawCommand {
            int indexCount = 0;
            unsigned int VAO;
            unsigned int VBO;
            std::vector<Vertex> vertices;
            std::shared_ptr<Material> mat;
            ECS::Components::Transform* tr;
            int id;
            int fillMode = GL_FILL;
        };

        class Renderer{
            public:

            static Renderer& GetInstance() {
                static Renderer instance;
                return instance;
            }

            static void Init(GLFWwindow *window);
            static void Shutdown();
            static void Render();

            static void Submit(DrawCommand cmd, bool replace);

            static void ReorderDrawList();

            static void DrawScene();

            static void RescaleFramebuffers(int width, int height);

            static void AddRenderPass(
                RenderStage stage, 
                std::function<void()> callback, 
                std::shared_ptr<FrameBuffer> fb = nullptr, 
                bool appendToViewport = true,
                BlendMode blendMode = BlendMode::Normal);

            
            static void ExecuteRenderPasses();

            static Camera &GetCamera() { return *cam; }

            static int GetHeight() { glfwGetWindowSize(window, &width, &height); return height; }

            static int GetWidth() { glfwGetWindowSize(window, &width, &height); return width; }

            static unsigned int GetViewportTextureID() { return viewportBuffer->GetFrameTexture(); }

            static LightManager* lightman;

            private:

            static void CreateRectGeometry();

            Renderer() = default;
            ~Renderer() = default;
            Renderer(const Renderer&) = delete;
            Renderer& operator=(const Renderer&) = delete;

            static void BeginFrame();
            static void EndFrame();

            static int width, height;

            static Camera* cam;

            static GLFWwindow *window;

            static std::vector<DrawCommand> drawList;

            static unsigned int rectVAO, rectVBO;
            
            static std::vector<RenderPass> renderPasses;

            static std::shared_ptr<Shader> blendShader;
            static FrameBuffer* viewportBuffer;
            static std::shared_ptr<Shader> framebufferShader;

            static std::shared_ptr<Shader> defaultShader;

        };
    }
}