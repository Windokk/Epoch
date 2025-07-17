#include "engine/ecs/components/core/script.hpp"
#include "engine/ecs/components/core/component_macros.hpp"

using namespace nlohmann;

BEGIN_COMPONENT(Character, SHAME::Engine::ECS::Components::Script)

    void Deserialize(json componentData) override;
    json Serialize() override;

    void Begin() override;
    void Tick() override;

END_COMPONENT(Character)