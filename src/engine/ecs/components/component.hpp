#pragma once

#include <cstdint>

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

                private:
                

        };
    }
    
}