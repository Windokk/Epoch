#pragma once

#include "engine/ecs/components/core/component.hpp"

#include "engine/events/event_system.hpp"

namespace Epoch::Engine::ECS::Components
{
    class Script : public Component{
        public:
            Script(Objects::Actor *parent, uint32_t local_id);

            virtual void OnLevelLoaded();

            virtual void OnLevelUnloaded();

            virtual void Begin();

            virtual void Tick();

            virtual void OnDestroyed();

            virtual void OnContactAdded(const Events::ContactAddedEvent& event);

            virtual void OnContactPersisted(const Events::ContactPersistedEvent& event);

            virtual void OnContactEnded(const Events::ContactRemovedEvent& event);

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
            
            bool beginCalled = false;

        private:


    };
}