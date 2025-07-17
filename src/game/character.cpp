#include "character.hpp"

Character::Character(SHAME::Engine::ECS::Objects::Actor* parent, uint32_t local_id)
    : Script(parent, local_id) {
    // Init
}

void Character::Deserialize(json componentData) {
    // Deserialize fields
}

json Character::Serialize() {
    return json();
}

void Character::Begin() {
    std::cout << "Character Begin" << std::endl;
}

void Character::Tick() {
    std::cout << "Character Tick" << std::endl;
}


REGISTER_COMPONENT(Character);