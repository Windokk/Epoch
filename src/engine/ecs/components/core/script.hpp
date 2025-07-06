#pragma once

#include "engine/ecs/components/core/component.hpp"

#include "engine/events/event_system.hpp"

namespace SHAME::Engine::ECS::Components
{
    class Script : public Component{
        public:
            Script(Objects::Actor *parent, uint32_t local_id);

            virtual void Begin();

            virtual void Tick();

            virtual void End();

            virtual void OnContactAdded(Events::ContactAddedEvent& event);

            virtual void OnContactPersisted(Events::ContactPersistedEvent& event);

            virtual void OnContactEnded(Events::ContactRemovedEvent& event);

            virtual void OnActivated();

            virtual void OnDeactivated();

            void Activate() override
            {
                Component::Activate();

                OnActivated();
            }

            void DeActivate() override
            {
                Component::DeActivate();

                OnDeactivated();
            }

        private:


    };
}