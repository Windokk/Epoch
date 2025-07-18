#include "renderer.hpp"

#include <iostream>
#include <algorithm>

#include "engine/levels/level_manager.hpp"

#include "engine/ecs/objects/actors/actor.hpp"

#include "engine/core/engine.hpp"

#include "engine/rendering/camera/camera_manager.hpp"

namespace SHAME::Engine::Rendering{
    
    GLFWwindow* Renderer::window = nullptr;
    std::vector<DrawCommand> Renderer::drawList = {};
    unsigned int  Renderer::rectVAO,  Renderer::rectVBO = -1;
    std::vector<RenderPass> Renderer::renderPasses = {};
    FrameBuffer* Renderer::viewportBuffer = nullptr;
    std::shared_ptr<Shader> Renderer::blendShader = nullptr;
    std::shared_ptr<Shader> Renderer::framebufferShader = nullptr;
    LightManager* Renderer::lightMan = nullptr;
    std::shared_ptr<Shader> Renderer::unlitShader = nullptr;
    RendererSettings Renderer::settings{};
    ShadowManager* Renderer::shadowMan = nullptr;

    void Renderer::CreateRectGeometry()
    {

        float rectVertices[] = {
            // positions     // texCoords
            -1.0f,  1.0f,     0.0f, 1.0f, // top-left
            -1.0f, -1.0f,     0.0f, 0.0f, // bottom-left
             1.0f, -1.0f,     1.0f, 0.0f, // bottom-right
        
            -1.0f,  1.0f,     0.0f, 1.0f, // top-left
             1.0f, -1.0f,     1.0f, 0.0f, // bottom-right
             1.0f,  1.0f,     1.0f, 1.0f  // top-right
        };
        
        glGenVertexArrays(1, &rectVAO);
        glGenBuffers(1, &rectVBO);
        
        glBindVertexArray(rectVAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);
        
        // position (vec2)
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texCoords (vec2)
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
        
        glBindVertexArray(0);

    }

    void Renderer::Init(GLFWwindow *window, RendererSettings settings)
    {
        Renderer::settings = settings;

        Renderer::window = window;
        
        glfwGetWindowSize(window, &settings.windowWidth, &settings.windowHeight);

        glViewport(0, 0, settings.windowWidth, settings.windowHeight);

        //FRAMBUFFERS
        CreateRectGeometry();
        blendShader = std::make_shared<Shader>(Filesystem::Path("engine_resources/shaders/fb/framebuffer.vert"),Filesystem::Path("engine_resources/shaders/fb/blend.frag"));
        framebufferShader = std::make_shared<Shader>(Filesystem::Path("engine_resources/shaders/fb/framebuffer.vert"),Filesystem::Path("engine_resources/shaders/fb/framebuffer.frag"));
        viewportBuffer = new FrameBuffer(settings.windowWidth, settings.windowHeight, framebufferShader);

        //LIGHTS
        lightMan = new LightManager();
        lightMan->Update(-1);

        //DEBUG_SHAPES
        unlitShader = std::make_shared<Shader>(Filesystem::Path("engine_resources/shaders/mesh/unlit.vert"),Filesystem::Path("engine_resources/shaders/mesh/unlit.frag"), Filesystem::Path(""));

        //MULTISAMPLING
        if(settings.antiAliasingLevel>0){
            glfwWindowHint(GLFW_SAMPLES, settings.antiAliasingLevel);
            glEnable(GL_MULTISAMPLE);
        }

        //SHADOWS
        shadowMan = new ShadowManager();
        shadowMan->Init(1024);
    }

    void Renderer::Shutdown()
    {
        for(RenderPass& renderPass : renderPasses){
            renderPass.target->Shutdown();
        }
        viewportBuffer->Shutdown();
    }

    void Renderer::Render()
    {
        if(CameraManager::GetActiveCamera() != nullptr){
            BeginFrame();

            ExecuteRenderPasses();
        }
    }

    void Renderer::Submit(DrawCommand cmd, bool replace)
    {
        if(replace){
            for(int i = 0; i < drawList.size(); i++){ 
                if(drawList[i].id == cmd.id){
                    drawList[i] = cmd;
                }
            }
        }
        else{
            drawList.push_back(cmd);
        }

        ReorderDrawList();
    }

    void Renderer::Submit(std::vector<DrawCommand> cmds, bool replace)
    {
        if (replace) {
            for (const auto& cmd : cmds) {
                bool found = false;
                for (auto& existingCmd : drawList) {
                    if (existingCmd.id == cmd.id) {
                        existingCmd = cmd;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    drawList.push_back(cmd);
                }
            }
        } else {
            drawList.insert(drawList.end(), cmds.begin(), cmds.end());
        }

        ReorderDrawList();
    }

    void Renderer::ReorderDrawList()
    {
        drawList.erase(
            std::remove_if(drawList.begin(), drawList.end(),
                [](const DrawCommand& cmd) {
                    return cmd.tr == nullptr;
                }),
            drawList.end()
        );
    }

    void Renderer::BeginFrame()
    {
        CameraManager::Tick();

        if(settings.enableShadows){
            shadowMan->RenderShadowMaps(Levels::LevelManager::GetLevelAt(0)->meshes);
            glfwGetWindowSize(window, &settings.windowWidth, &settings.windowHeight);
            glViewport(0, 0, Renderer::settings.windowWidth, Renderer::settings.windowHeight);
        }

        viewportBuffer->Bind();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        viewportBuffer->Unbind();
    }
  
    void Renderer::DrawScene()
    {
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // --- Main Draw Calls ---
        for (auto& cmd : drawList) {
            if (!cmd.mat || cmd.indexCount <= 0) continue;

            if(settings.enableShadows)
                shadowMan->BindShadowMaps(cmd.mat);
            cmd.mat->SetParameter("projectionView", CameraManager::GetActiveCamera()->GetMatrix());
            cmd.mat->SetParameter("model", cmd.tr->GetTransformMatrix());
            cmd.mat->SetParameter("lightNB", lightMan->GetLightsCount());
            cmd.mat->Use();
            glBindVertexArray(cmd.VAO);
            glPolygonMode(GL_FRONT_AND_BACK, cmd.fillMode);
            glDrawElements(GL_TRIANGLES, cmd.indexCount, GL_UNSIGNED_INT, nullptr);
            cmd.mat->StopUsing();
        }

        if(!Renderer::settings.showDebugShapes){
            glBindVertexArray(0);
            glUseProgram(0);
            glDisable(GL_DEPTH_TEST);
            return;
        }

        // --- Debug Physics Shapes ---
        unlitShader->Activate();
        unlitShader->setMat4("projectionView", CameraManager::GetActiveCamera()->GetMatrix());

        for (auto& physicBody : Levels::LevelManager::GetLevelAt(0)->physicsBodies) {

            glm::vec3 pos = physicBody->parent->transform->GetPosition();
            glm::quat rot = physicBody->parent->transform->GetRotation();

            glm::mat4 model = glm::translate(glm::mat4(1.0f), pos) * glm::toMat4(rot);

            unlitShader->setMat4("model", model);

            glBindVertexArray(physicBody->GetDebugShape()->GetVAO());
            glDrawElements(GL_LINES, physicBody->GetDebugShape()->GetIndexCount(), GL_UNSIGNED_INT, nullptr);
        }

        glBindVertexArray(0);
        glUseProgram(0);
        glDisable(GL_DEPTH_TEST);
    }

    void Renderer::RescaleFramebuffers(int width, int height)
    {
        for (auto& pass : renderPasses)
            if(pass.target != nullptr){
                pass.target->RescaleFrameBuffer(width, height);
            }

        viewportBuffer->RescaleFrameBuffer(width, height);

        glViewport(0, 0, width, height);

        CameraManager::UpdateSize(width, height);
    }

    void Renderer::AddRenderPass(RenderStage stage, std::function<void()> callback, std::shared_ptr<FrameBuffer> fb, bool appendToViewport, BlendMode blendMode)
    {
        renderPasses.push_back(RenderPass{stage, std::move(callback), fb, appendToViewport, blendMode});
    
        std::sort(renderPasses.begin(), renderPasses.end(),
            [](const RenderPass& a, const RenderPass& b) {
                return static_cast<int>(a.stage) < static_cast<int>(b.stage);
            });
    }

    void Renderer::ExecuteRenderPasses(){
        for (const auto& pass : renderPasses) {
            switch (pass.stage) {
                case RenderStage::UI:{
                    // No FBO
                    pass.callback();
                    break;
                }
                case RenderStage::PostProcess:{
                    if(!pass.target)
                        DEBUG_ERROR("Couldn't render pass without framebuffer");

                    if(!Renderer::settings.enablePostProcessing)
                        break;

                    pass.target->Bind();
                    viewportBuffer->Draw(rectVAO);
                    pass.target->Unbind();
                    
                    if(pass.appendToViewport){
                        viewportBuffer->Bind();
                    }
                        pass.callback();
                        pass.target->Draw(rectVAO);

                    if(pass.appendToViewport){
                        viewportBuffer->Unbind();
                        viewportBuffer->Draw(rectVAO);
                    }

                    break;
                }
                case RenderStage::Background:
                case RenderStage::Scene:
                case RenderStage::Debug:
                default:{
                    if(!pass.target)
                        DEBUG_ERROR("Couldn't render pass without framebuffer");
                    pass.target->Bind();
                    pass.callback();
                    pass.target->Unbind();
                    pass.target->Draw(rectVAO);

                    if(pass.appendToViewport){
                        viewportBuffer->Bind();

                        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                        
                        blendShader->Activate();

                        blendShader->setInt("blendMode", (int)pass.blendMode);
                        blendShader->setInt("texA", 0);
                        blendShader->setInt("texB", 1);

                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, viewportBuffer->GetFrameTexture());

                        glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, pass.target->GetFrameTexture());
        
                        glBindVertexArray(rectVAO);
                        glDisable(GL_DEPTH_TEST);
                        
                        glDrawArrays(GL_TRIANGLES, 0, 6);
                        
                        glBindVertexArray(0);

                        blendShader->Deactivate();

                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, 0);
                        glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, 0);

                        viewportBuffer->Unbind();

                        viewportBuffer->Draw(rectVAO);

                    }

                    break;
                }
            }
        }

    }

    void Renderer::ToggleFullscreen()
    {
        const bool fullscreen = glfwGetWindowMonitor(window) != nullptr;
        if(fullscreen) {
            // Restore the window position and size.
            glfwSetWindowMonitor(window, nullptr, settings.windowPosX, settings.windowPosY, settings.windowWidth, settings.windowHeight, 0);
            // Check the window position and size (if we are on a screen smaller than the initial size).
            glfwGetWindowPos(window, &settings.windowPosX, &settings.windowPosY);
            glfwGetWindowSize(window, &settings.windowWidth, &settings.windowHeight);
            RescaleFramebuffers(settings.windowWidth, settings.windowHeight);
        } else {
            // Backup the window current frame.
            glfwGetWindowPos(window, &settings.windowPosX, &settings.windowPosY);
            glfwGetWindowSize(window, &settings.windowWidth, &settings.windowHeight);
            // Move to fullscreen on the primary monitor.
            GLFWmonitor * monitor	= glfwGetPrimaryMonitor();
            const GLFWvidmode * mode = glfwGetVideoMode(monitor);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            RescaleFramebuffers(mode->width, mode->height);
        }
    }
}