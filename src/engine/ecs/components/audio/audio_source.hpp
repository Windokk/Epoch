#pragma once

#include "engine/audio/audio_manager.hpp"

#include "engine/ecs/components/core/component.hpp"

#include "engine/rendering/utils.hpp"

namespace EPOCH::Engine::ECS::Components
{

    class AudioSource : public Component {
        private:
            std::unique_ptr<Filesystem::Path> path;
            Audio::AudioID audioID;
            float volume = -1.0f;


        public:
            AudioSource(Objects::Actor *parent, uint32_t local_id);

            void SetPath(std::string path);
            void SetVolume(float volume);
            void Play();
            void Pause();
            void RemoveSound();
            void Update();

            void DeActivate() override
            {
                Component::DeActivate();

                Pause();
            }
    };
}