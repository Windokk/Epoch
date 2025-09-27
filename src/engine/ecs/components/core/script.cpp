#include "script.hpp"

namespace EPOCH::Engine::ECS::Components
{
    Script::Script(Objects::Actor *parent, uint32_t local_id) : Component(parent, local_id)
    {    
    }

    void Script::OnLevelLoaded()
    {
    }

    void Script::OnLevelUnloaded()
    {
    }

    void Script::Begin()
    {
    }

    void Script::Tick()
    {
    }

    void Script::OnDestroyed()
    {
    }

    void Script::OnContactAdded(const Events::ContactAddedEvent& event)
    {
    }

    void Script::OnContactPersisted(const Events::ContactPersistedEvent& event)
    {
    }

    void Script::OnContactEnded(const Events::ContactRemovedEvent& event)
    {
    }

    void Script::OnActivated()
    {
    }

    void Script::OnDeactivated()
    {
    }
}
