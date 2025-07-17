#include <iostream>

extern "C" void InitGameComponents() {
    // This function forces static initializers like `autoRegister_*` to run
    std::cout << "[INFO] [GAME/GAME_MODULE] : Game components initialized.\n";
}