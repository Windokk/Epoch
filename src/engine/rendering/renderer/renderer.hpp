#pragma once

#include "engine/rendering/material/material.hpp"
#include "engine/rendering/framebuffer/framebuffer.hpp"
#include "engine/rendering/light/light_manager.hpp"
#include "engine/ecs/components/rendering/model_component.hpp"
#include "engine/rendering/debug/debug.hpp"
#include "engine/rendering/shadow/shadow_manager.hpp"
#include "engine/debugging/debugger.hpp"

namespace EPOCH::Engine{

    namespace ECS::Components{
        class Transform;
    }

    namespace Rendering {

        class Renderer;

        struct RendererSettings{
            bool showDebugShapes = false;
            int antiAliasingLevel = 4;
            //TODO : bool enableStatsOverlay = false;
            bool enableShadows = true;
            bool enablePostProcessing = true;
            private:
                int windowPosX, windowPosY, windowWidth, windowHeight= 0;
                bool fullscreen = false;
            friend class EPOCH::Engine::Rendering::Renderer;

        };

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
            int indexOffset = 0;
            int indexCount = 0;
            unsigned int VAO;
            unsigned int VBO;
            std::shared_ptr<Material> mat;
            std::shared_ptr<ECS::Components::Transform> tr;
            int id;
            int fillMode = GL_FILL;
        };

        class Renderer{
            public:

            static Renderer& GetInstance() {
                static Renderer instance;
                return instance;
            }

            static void Init(GLFWwindow *window, RendererSettings settings = {});
            static void InitFramebuffers();
            static void Shutdown();
            static void Render();

            static void Submit(DrawCommand cmd, bool replace);

            static void Submit(std::vector<DrawCommand> cmds, bool replace);

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

            static int GetCurrentHeight() { int height; glfwGetWindowSize(window, nullptr, &height); return height; }

            static int GetCurrentWidth() { int width; glfwGetWindowSize(window, &width, nullptr); return width; }

            static bool GetCurrentFullscreen() { return settings.fullscreen; }

            static void ToggleFullscreen();

            static unsigned int GetViewportTextureID() { return viewportBuffer->GetFrameTexture(); }

            static LightManager* lightMan;

            static ShadowManager* shadowMan;

            static GLuint GetRectVAO();

            private:

            static void CreateRectGeometry();

            Renderer() = default;
            ~Renderer() = default;
            Renderer(const Renderer&) = delete;
            Renderer& operator=(const Renderer&) = delete;

            static void BeginFrame();

            static GLFWwindow *window;

            static std::vector<DrawCommand> drawList;

            static unsigned int rectVAO, rectVBO;
            
            static std::vector<RenderPass> renderPasses;

            static std::shared_ptr<Shader> blendShader;
            static FrameBuffer* viewportBuffer;
            static std::shared_ptr<Shader> framebufferShader;

            static std::shared_ptr<Shader> unlitShader;

            static RendererSettings settings;

        };
    }
}