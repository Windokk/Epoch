#pragma once 

#include "engine/filesystem/filesystem.hpp"

#include "audioID.hpp"

#include <fmod/fmod.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <string>
#include <iostream>

namespace SHAME::Engine::Audio
{
    struct Sound{
        FMOD_SOUND* fmod_sound;
        Filesystem::Path path;
        glm::vec3 pos;
        std::string buffer;
        bool isPlaying = false;
        float initialVolume;
    };
    
    class AudioManager{
        public:

        static AudioManager& GetInstance() {
            static AudioManager instance;
            return instance;
        }
        
        static void Init(float masterVolume);
        static void Shutdown();
        static void Tick();
        static void CreateSound(AudioID id, Filesystem::Path path, glm::vec3 pos);
        static void RemoveSound(AudioID id);
        static void PlaySound(AudioID id, float volume);
        static void PauseSound(AudioID id);
        static void UpdateSound(AudioID id, glm::vec3 pos, float volume);
        static void Update(glm::vec3 listenerPos, glm::vec2 listenerFacingNormalized, float maxDistance);

        private:

        AudioManager() = default;
        ~AudioManager() = default;
        AudioManager(const AudioManager&) = delete;
        AudioManager& operator=(const AudioManager&) = delete;

        static FMOD_SYSTEM *system;
        static std::unordered_map<std::string, FMOD_CHANNEL*> channels;
	    static FMOD_CREATESOUNDEXINFO exinfo;
        static float masterVolume;
    };

}