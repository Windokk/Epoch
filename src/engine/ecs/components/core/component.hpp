#pragma once

#include <cstdint>

#include <nlohmann/json.hpp>

using namespace nlohmann;

namespace SHAME::Engine::ECS{
    namespace Objects{
        class Actor;
    }

    namespace Components{
        
        class Component{

            public:
                Component(Objects::Actor *parent, uint32_t local_id);
                virtual ~Component();
                virtual void Destroy();
                Objects::Actor *parent = nullptr;

                uint32_t local_id;

                virtual void Activate() { activated = true; }
                virtual void DeActivate() { activated = false; }

                virtual void Deserialize(json componentData) {};
                virtual json Serialize() { json dummy; return dummy; };

                bool Active(){
                    return activated;
                }

            private:

            protected:
                bool activated = true;

        };
    }
    
}