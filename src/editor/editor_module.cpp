#include "engine/core/engine.hpp"
#include "engine/core/resources/resources_manager.hpp"
#include "engine/rendering/ui/text.hpp"

using namespace EPOCH::Engine;
using namespace EPOCH::Engine::Rendering;
using namespace EPOCH::Engine::ECS::Components;
using namespace EPOCH::Engine::ECS::Objects;

std::shared_ptr<UI::Font> font;

extern "C" __declspec(dllexport) void InitializeSingletons(Debugging::Debugger* debugger, 
                                                            Renderer* renderer, 
                                                            Core::Resources::ResourcesManager* resourcesManager, 
                                                            CameraManager* cameraManager,
                                                            Time::TimeManager* timeManager) {
    SetDebugger(debugger);
    SetRenderer(renderer);
    SetResourcesManager(resourcesManager);
    SetCameraManager(cameraManager);
    SetTimeManager(timeManager);
}

extern "C" __declspec(dllexport) void EditorStart(){

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    std::shared_ptr<Shader> fbShader = Core::Resources::ResourcesManager::GetInstance().GetShader("shaders\\fb\\framebuffer");
    font = std::make_shared<UI::Font>("project_resources\\fonts\\NotoSans-Regular.ttf", 45);
    ECS::Objects::Actor a = ECS::Objects::Actor("test");
    a.transform->SetPosition(glm::vec3(0, (float)GetRenderer().GetCurrentHeight()-20.0f, 0));
    a.transform->SetScale(glm::vec3(0.1f, 0.1f, 0.1f));
    std::string neuil = "FPS = 60.0";
    std::shared_ptr<UI::Text> fpsText = std::make_shared<UI::Text>(font, neuil, COL_RGBA(1,1,1,1), *a.transform.get());
    std::shared_ptr<Shader> textShader = Core::Resources::GetResourcesManager().GetShader("shaders\\text\\text");
    FrameBuffer uiFB = {static_cast<float>(GetRenderer().GetCurrentWidth()), static_cast<float>(GetRenderer().GetCurrentHeight()), fbShader, true};

    auto RenderPassUI = [fpsText, textShader] {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        fpsText->SetText("FPS = "+std::to_string(static_cast<int>(1/Time::GetTimeManager().GetDeltaTime())));
        glm::mat4 projection = glm::ortho(0.0f, (float)GetRenderer().GetCurrentWidth(), 0.0f, (float)GetRenderer().GetCurrentHeight());
        fpsText->Draw(textShader, projection, GetCameraManager().GetActiveCamera()->GetView());
        glDisable(GL_BLEND);
    };

    GetRenderer().AddRenderPass(RenderStage::UI, RenderPassUI, std::make_shared<FrameBuffer>(uiFB));
}

extern "C" __declspec(dllexport) void EditorTick(){

}

extern "C" __declspec(dllexport) void EditorCleanup(){
    font->Cleanup();
}
