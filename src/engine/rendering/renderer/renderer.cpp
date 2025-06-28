#include "renderer.hpp"

#include <iostream>
#include <algorithm>

namespace SHAME::Engine::Rendering{
    
    Camera* Renderer::cam = nullptr;
    GLFWwindow* Renderer::window = nullptr;
    std::vector<DrawCommand> Renderer::drawList = {};
    unsigned int  Renderer::rectVAO,  Renderer::rectVBO = -1;
    std::vector<RenderPass> Renderer::renderPasses = {};
    FrameBuffer* Renderer::viewportBuffer = nullptr;
    std::shared_ptr<Shader> Renderer::blendShader = nullptr;
    std::shared_ptr<Shader> Renderer::framebufferShader = nullptr;
    LightManager* Renderer::lightman = nullptr;

    int Renderer::width = 0;
    int Renderer::height = 0;

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

    void Renderer::CreateRenderingContext(GLFWwindow *window)
    {
        GetInstance().window = window;
        
        glfwGetWindowSize(window, &width, &height);

        cam = new Camera(width, height, glm::vec3(0,0,10), glm::vec3(0,0,-1));

        CreateRectGeometry();

        blendShader = std::make_shared<Shader>("resources/shaders/fb/framebuffer.vert","resources/shaders/fb/blend.frag");
        framebufferShader = std::make_shared<Shader>("resources/shaders/fb/framebuffer.vert","resources/shaders/fb/framebuffer.frag");
        viewportBuffer = new FrameBuffer(width, height, framebufferShader);

        lightman = new LightManager();

        lightman->Update();
    }

    void Renderer::DestroyRenderingContext()
    {
        
    }

    void Renderer::Render()
    {
        BeginFrame();

        ExecuteRenderPasses();
        
        EndFrame();
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

    void Renderer::ReorderDrawList()
    {
        std::sort(drawList.begin(), drawList.end(), [](const DrawCommand& a, const DrawCommand& b) {
            return a.tr->GetPosition().z < b.tr->GetPosition().z;
        });
    }

    void Renderer::BeginFrame()
    {
        cam->UpdateMatrix(0.01f, 1000.0f);
        if(cam->canInteract) cam->Inputs(window);

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

        glClearColor(0.3f, 0.3f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for(auto& cmd : drawList){

            if (!cmd.mat || cmd.indexCount <= 0) continue;

            cmd.mat->SetParameter("projectionView", cam->GetMatrix());
            cmd.mat->SetParameter("model", cmd.tr->GetMatrix());
            cmd.mat->SetParameter("lightNB", lightman->GetLightsCount());

            cmd.mat->Use();

            glBindVertexArray(cmd.VAO);

            glBindBuffer(GL_ARRAY_BUFFER, cmd.VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, cmd.vertices.size() * sizeof(Vertex), cmd.vertices.data());

            glPolygonMode(GL_FRONT_AND_BACK, cmd.fillMode);

            glDrawElements(GL_TRIANGLES, cmd.indexCount, GL_UNSIGNED_INT, 0);

            cmd.mat->StopUsing();
        }
        
        glBindVertexArray(0);

        glDisable(GL_DEPTH_TEST);
    }

    void Renderer::RescaleFramebuffers(int width, int height)
    {
        for (auto& pass : renderPasses)
            if(pass.target != nullptr){
                pass.target->RescaleFrameBuffer(width, height);
            }

        viewportBuffer->RescaleFrameBuffer(width, height);
    }

    void Renderer::AddRenderPass(RenderStage stage, std::function<void()> callback, std::shared_ptr<FrameBuffer> fb, bool appendToViewport, BlendMode blendMode)
    {
        renderPasses.push_back(RenderPass{stage, std::move(callback), fb, appendToViewport, blendMode});
    
        std::sort(renderPasses.begin(), renderPasses.end(),
            [](const RenderPass& a, const RenderPass& b) {
                return static_cast<int>(a.stage) < static_cast<int>(b.stage);
            });
    }

    void Renderer::ExecuteRenderPasses()
    {
        for (const auto& pass : renderPasses) {
            switch (pass.stage) {
                case RenderStage::UI:
                    // No FBO
                    pass.callback();
                    break;

                case RenderStage::PostProcess:
                    if(!pass.target)
                        throw std::runtime_error("[ERROR]  [ENGINE/RENDERING] : Couldn't render pass without framebuffer");

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

                case RenderStage::Background:
                case RenderStage::Scene:
                case RenderStage::Debug:
                default:
                    if(!pass.target)
                        throw std::runtime_error("[ERROR]  [ENGINE/RENDERING] : Couldn't render pass without framebuffer");
                    pass.target->Bind();
                    pass.callback();
                    pass.target->Unbind();
                    pass.target->Draw(rectVAO);

                    if(pass.appendToViewport){
                        viewportBuffer->Bind();

                        glClearColor(0.3f, 0.3f, 1.0f, 1.0f);
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

    void Renderer::EndFrame()
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}