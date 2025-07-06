#include "script.hpp"

namespace SHAME::Engine::ECS::Components
{
    Script::Script(Objects::Actor *parent, uint32_t local_id) : Component(parent, local_id)
    {
        
    }

    void Script::Begin() {
    }

    void Script::Tick() {
    }

    void Script::End() {
    }

    void Script::OnContactAdded(Events::ContactAddedEvent& event) {
    }

    void Script::OnContactPersisted(Events::ContactPersistedEvent& event) {
    }

    void Script::OnContactEnded(Events::ContactRemovedEvent& event) {
    }

    void Script::OnActivated() {
    }

    void Script::OnDeactivated() {
    }
}
